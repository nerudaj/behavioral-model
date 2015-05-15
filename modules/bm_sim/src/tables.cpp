#include <iostream>

#include "bm_sim/tables.h"

#include "bm_sim/event_logger.h"

using std::vector;
using std::copy;
using std::string;

MatchTable::ErrorCode
MatchTable::get_and_set_handle(entry_handle_t *handle)
{
  if(num_entries >= size) { // table is full
    return TABLE_FULL;
  }

  if(handles.get_handle(handle)) return ERROR;

  num_entries++;
  return SUCCESS;
}

MatchTable::ErrorCode
MatchTable::unset_handle(entry_handle_t handle)
{
  if(handles.release_handle(handle)) return INVALID_HANDLE;

  num_entries--;
  return SUCCESS;
}

bool
MatchTable::valid_handle(entry_handle_t handle) const {
  return handles.valid_handle(handle);
}

MatchTable::ErrorCode
MatchTable::delete_entry(entry_handle_t handle)
{
  return unset_handle(handle);
}

const ControlFlowNode *
MatchTable::operator()(Packet *pkt) const
{
  static thread_local ByteContainer lookup_key;
  lookup_key.clear();
  build_key(*pkt->get_phv(), lookup_key);
  const MatchEntry *entry = lookup(lookup_key);
  if(!entry) {
    ELOGGER->table_miss(*pkt, *this);
    default_action_entry(pkt);
    return default_next_node;
  }
  else {
    ELOGGER->table_hit(*pkt, *this, *entry);
    if(with_counters) {
      update_counters(get_entry_handle(*entry), *pkt);
    }
    entry->action_entry(pkt);
    return entry->next_table;
  }
}


const ExactMatchEntry *
ExactMatchTable::lookup(const ByteContainer &key) const
{
  boost::shared_lock<boost::shared_mutex> lock(t_mutex);

  auto entry_it = entries_map.find(key);
  if(entry_it == entries_map.end()) return nullptr;
  return &entries[entry_it->second];
}

MatchTable::ErrorCode
ExactMatchTable::add_entry(ExactMatchEntry &&entry, entry_handle_t *handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;
  
  entries[*handle] = std::move(entry);
  // the key is copied a second time, but it should not incur a significant cost
  ByteContainer &key = entries[*handle].key;
  entries_map[key] = *handle;

  return SUCCESS;
}

MatchTable::ErrorCode
ExactMatchTable::add_entry(const std::vector<MatchKeyParam> &match_key,
			   const ActionFn &action_fn,
			   const ActionData &action_data,
			   entry_handle_t *handle,
			   int priority/*not used*/)
{
  (void) priority;

  ByteContainer new_key;
  new_key.reserve(nbytes_key);

  // take care of valid first

  for(const MatchKeyParam &param : match_key) {
    if(param.type == MatchKeyParam::Type::VALID)
      new_key.append(param.key);
  }

  for(const MatchKeyParam &param : match_key) {
    switch(param.type) {
    case MatchKeyParam::Type::EXACT:
      new_key.append(param.key);
      break;
    case MatchKeyParam::Type::VALID: // already done
      break;
    default:
      assert(0 && "invalid param type in match_key");
      break;
    }
  }

  ActionFnEntry action_entry(&action_fn, action_data);
  const ControlFlowNode *next_node = get_next_node(action_fn.get_id());
  ExactMatchEntry entry(new_key, std::move(action_entry), next_node);

  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;

  entries[*handle] = std::move(entry);
  entries_map[std::move(new_key)] = *handle;

  return SUCCESS;
}

MatchTable::ErrorCode
ExactMatchTable::delete_entry(entry_handle_t handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  ByteContainer &key = entries[handle].key;
  entries_map.erase(key);
  return MatchTable::delete_entry(handle);
}

MatchTable::ErrorCode
ExactMatchTable::modify_entry(entry_handle_t handle,
			      const ActionFnEntry &action_entry,
			      const ControlFlowNode *next_table)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  ExactMatchEntry &entry = entries[handle];
  entry.action_entry = action_entry;
  entry.next_table = next_table;

  return SUCCESS;
}

const LongestPrefixMatchEntry *
LongestPrefixMatchTable::lookup(const ByteContainer &key) const
{
  boost::shared_lock<boost::shared_mutex> lock(t_mutex);

  entry_handle_t handle;
  if(entries_trie.lookup(key, &handle)) {
    return &entries[handle];
  }
  return nullptr;
}

MatchTable::ErrorCode
LongestPrefixMatchTable::add_entry(LongestPrefixMatchEntry &&entry,
				   entry_handle_t *handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;
  
  entries[*handle] = std::move(entry);
  ByteContainer &key = entries[*handle].key;
  int &prefix_length = entries[*handle].prefix_length;
  assert(prefix_length >= 0);
  entries_trie.insert_prefix(key, prefix_length, *handle);

  return SUCCESS;
}

MatchTable::ErrorCode
LongestPrefixMatchTable::add_entry(const std::vector<MatchKeyParam> &match_key,
				   const ActionFn &action_fn,
				   const ActionData &action_data,
				   entry_handle_t *handle,
				   int priority/*not used*/)
{
  (void) priority;

  ByteContainer new_key;
  new_key.reserve(nbytes_key);
  int prefix_length = 0;
  const MatchKeyParam *lpm_param = nullptr;

  for(const MatchKeyParam &param : match_key) {
    if(param.type == MatchKeyParam::Type::VALID)
      new_key.append(param.key);
  }

  for(const MatchKeyParam &param : match_key) {
    switch(param.type) {
    case MatchKeyParam::Type::EXACT:
      new_key.append(param.key);
      prefix_length += param.key.size();
      break;
    case MatchKeyParam::Type::LPM:
      assert(!lpm_param && "more than one lpm param in match key");
      lpm_param = &param;
      break;
    case MatchKeyParam::Type::VALID: // already done
      break;
    default:
      assert(0 && "invalid param type in match_key");
      break;
    }
  }

  assert(lpm_param && "no lpm param in match key");
  new_key.append(lpm_param->key);
  prefix_length += lpm_param->prefix_length;

  ActionFnEntry action_entry(&action_fn, action_data);
  const ControlFlowNode *next_node = get_next_node(action_fn.get_id());
  LongestPrefixMatchEntry entry(new_key, std::move(action_entry),
				prefix_length, next_node);

  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;

  entries[*handle] = std::move(entry);
  entries_trie.insert_prefix(std::move(new_key), prefix_length, *handle);

  return SUCCESS;
}

MatchTable::ErrorCode
LongestPrefixMatchTable::delete_entry(entry_handle_t handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  LongestPrefixMatchEntry &entry = entries[handle];
  assert(entries_trie.delete_prefix(entry.key, entry.prefix_length));
  return MatchTable::delete_entry(handle);
}

MatchTable::ErrorCode
LongestPrefixMatchTable::modify_entry(entry_handle_t handle,
				      const ActionFnEntry &action_entry,
				      const ControlFlowNode *next_table)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  LongestPrefixMatchEntry &entry = entries[handle];
  entry.action_entry = action_entry;
  entry.next_table = next_table;

  return SUCCESS;
}

const TernaryMatchEntry *
TernaryMatchTable::lookup(const ByteContainer &key) const
{
  boost::shared_lock<boost::shared_mutex> lock(t_mutex);

  int max_priority = 0;
  bool match;

  const TernaryMatchEntry *entry;
  const TernaryMatchEntry *max_entry = nullptr;

  for(auto it = handles.begin(); it != handles.end(); ++it) {
    entry = &entries[*it];

    if(entry->priority <= max_priority) continue;
    
    match = true;
    for(size_t byte_index = 0; byte_index < nbytes_key; byte_index++) {
      if(entry->key[byte_index] != (key[byte_index] & entry->mask[byte_index])) {
	match = false;
	break;
      }
    }

    if(match) {
      max_priority = entry->priority;
      max_entry = entry;
    }
  }

  return max_entry;
}

MatchTable::ErrorCode
TernaryMatchTable::add_entry(TernaryMatchEntry &&entry, entry_handle_t *handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;
  
  entries[*handle] = std::move(entry);

  return SUCCESS;
}

static std::string create_mask_from_pref_len(int prefix_length, int size) {
  std::string mask(size, '\xff');
  std::fill(mask.begin(), mask.begin() + (prefix_length / 8), '\xff');
  if(prefix_length % 8 != 0) {
    mask[prefix_length / 8] = (char) 0xFF << (8 - (prefix_length % 8));
  }
  return mask;
}

MatchTable::ErrorCode
TernaryMatchTable::add_entry(const std::vector<MatchKeyParam> &match_key,
			     const ActionFn &action_fn,
			     const ActionData &action_data,
			     entry_handle_t *handle,
			     int priority)
{
  ByteContainer new_key;
  ByteContainer new_mask;
  new_key.reserve(nbytes_key);

  for(const MatchKeyParam &param : match_key) {
    if(param.type == MatchKeyParam::Type::VALID)
      new_key.append(param.key);
  }

  for(const MatchKeyParam &param : match_key) {
    switch(param.type) {
    case MatchKeyParam::Type::EXACT:
      new_key.append(param.key);
      new_mask.append(std::string(param.key.size(), '\xff'));
      break;
    case MatchKeyParam::Type::LPM:
      new_key.append(param.key);
      new_mask.append(create_mask_from_pref_len(param.prefix_length, param.key.size()));
      break;
    case MatchKeyParam::Type::TERNARY:
      new_key.append(param.key);
      new_mask.append(param.mask);
      break;
    case MatchKeyParam::Type::VALID: // already done
      break;
    default:
      assert(0 && "invalid param type in match_key");
      break;
    }
  }

  ActionFnEntry action_entry(&action_fn, action_data);
  const ControlFlowNode *next_node = get_next_node(action_fn.get_id());
  TernaryMatchEntry entry(std::move(new_key), std::move(action_entry),
			  std::move(new_mask), priority, next_node);

  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  ErrorCode status = get_and_set_handle(handle);
  if(status != SUCCESS) return status;

  entries[*handle] = std::move(entry);

  return SUCCESS;
}

MatchTable::ErrorCode
TernaryMatchTable::delete_entry(entry_handle_t handle)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  return MatchTable::delete_entry(handle);
}

MatchTable::ErrorCode
TernaryMatchTable::modify_entry(entry_handle_t handle,
				const ActionFnEntry &action_entry,
				const ControlFlowNode *next_table)
{
  boost::unique_lock<boost::shared_mutex> lock(t_mutex);

  if(!valid_handle(handle)) return INVALID_HANDLE;
  TernaryMatchEntry &entry = entries[handle];
  entry.action_entry = action_entry;
  entry.next_table = next_table;

  return SUCCESS;
}
