/* Copyright 2013-present Barefoot Networks, Inc.
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
 * Jakub Neruda (xnerud01@stud.fit.vutbr.cz)
 *
 */

#include <PI/p4info.h>
#include <PI/pi.h>
#include <PI/target/pi_meter_imp.h>
#include "helpers.hpp"

extern "C" {

pi_status_t _pi_meter_read(pi_session_handle_t session_handle, pi_dev_tgt_t dev_tgt, pi_p4_id_t meter_id, size_t index, pi_meter_spec_t *meter_spec) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_tgt);
	COMBO_UNUSED(meter_id);
	COMBO_UNUSED(index);
	COMBO_UNUSED(meter_spec);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_meter_set(pi_session_handle_t session_handle, pi_dev_tgt_t dev_tgt, pi_p4_id_t meter_id, size_t index, const pi_meter_spec_t *meter_spec) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_tgt);
	COMBO_UNUSED(meter_id);
	COMBO_UNUSED(meter_spec);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_meter_read_direct(pi_session_handle_t session_handle, pi_dev_tgt_t dev_tgt, pi_p4_id_t meter_id, pi_entry_handle_t entry_handle, pi_meter_spec_t *meter_spec) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_tgt);
	COMBO_UNUSED(meter_id);
	COMBO_UNUSED(entry_handle);
	COMBO_UNUSED(meter_spec);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

pi_status_t _pi_meter_set_direct(pi_session_handle_t session_handle, pi_dev_tgt_t dev_tgt, pi_p4_id_t meter_id, pi_entry_handle_t entry_handle, const pi_meter_spec_t *meter_spec) {
	COMBO_UNUSED(session_handle);
	COMBO_UNUSED(dev_tgt);
	COMBO_UNUSED(meter_id);
	COMBO_UNUSED(entry_handle);
	COMBO_UNUSED(meter_spec);
	return PI_STATUS_NOT_IMPLEMENTED_BY_TARGET;
}

}
