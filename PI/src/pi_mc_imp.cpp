/* Copyright 2018-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

#include <PI/pi_mc.h>
#include <PI/target/pi_mc_imp.h>
#include "helpers.hpp"

pi_status_t _pi_mc_session_init(pi_mc_session_handle_t *session_handle) {
	COMBO_UNUSED(session_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_session_cleanup(pi_mc_session_handle_t session_handle) {
	COMBO_UNUSED(session_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_grp_create(pi_mc_session_handle_t session_handle,
                              pi_dev_id_t dev_id, pi_mc_grp_id_t grp_id,
                              pi_mc_grp_handle_t *grp_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(grp_id);
	COMBO_UNUSED(grp_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_grp_delete(pi_mc_session_handle_t session_handle,
                              pi_dev_id_t dev_id,
                              pi_mc_grp_handle_t grp_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(grp_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_node_create(pi_mc_session_handle_t session_handle,
                               pi_dev_id_t dev_id, pi_mc_rid_t rid,
                               size_t eg_ports_count,
                               const pi_mc_port_t *eg_ports,
                               pi_mc_node_handle_t *node_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(rid);
	COMBO_UNUSED(eg_ports_count);
	COMBO_UNUSED(eg_ports);
	COMBO_UNUSED(node_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_node_modify(pi_mc_session_handle_t session_handle,
                               pi_dev_id_t dev_id,
                               pi_mc_node_handle_t node_handle,
                               size_t eg_ports_count,
                               const pi_mc_port_t *eg_ports) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(node_handle);
	COMBO_UNUSED(eg_ports_count);
	COMBO_UNUSED(eg_ports);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_node_delete(pi_mc_session_handle_t session_handle,
                               pi_dev_id_t dev_id,
                               pi_mc_node_handle_t node_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(node_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_grp_attach_node(pi_mc_session_handle_t session_handle,
                                   pi_dev_id_t dev_id,
                                   pi_mc_grp_handle_t grp_handle,
                                   pi_mc_node_handle_t node_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(grp_handle);
	COMBO_UNUSED(node_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_mc_grp_detach_node(pi_mc_session_handle_t session_handle,
                                   pi_dev_id_t dev_id,
                                   pi_mc_grp_handle_t grp_handle,
                                   pi_mc_node_handle_t node_handle) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_id);
	COMBO_UNUSED(grp_handle);
	COMBO_UNUSED(node_handle);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}
