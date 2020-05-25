#include "multiplier_UT.hh"


MultiplierModule::MultiplierModule(const sc_module_name &module_name)
    : sc_module(module_name), target_socket("target_socket"),
      pending_transaction(NULL) {
    target_socket(*this);
}

void MultiplierModule::b_transport(tlm::tlm_generic_payload &trans,
                                   sc_time &t) {
    ioDataStruct = *((iostruct *)trans.get_data_ptr());

    if (trans.is_write()) {
        multiplier_function();
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        ioDataStruct.result = tmp_result;
        *((iostruct *)trans.get_data_ptr()) = ioDataStruct;

    } else if (trans.is_read()) {
        ioDataStruct.result = tmp_result;
        *((iostruct *)trans.get_data_ptr()) = ioDataStruct;
    }
}

bool MultiplierModule::get_direct_mem_ptr(tlm::tlm_generic_payload &trans,
                                          tlm::tlm_dmi &dmi_data) {
    return false;
}

tlm::tlm_sync_enum
MultiplierModule::nb_transport_fw(tlm::tlm_generic_payload &trans,
                                  tlm::tlm_phase &phase, sc_time &t) {
    return tlm::TLM_COMPLETED;
}

unsigned int MultiplierModule::transport_dbg(tlm::tlm_generic_payload &trans) {
    return 0;
}


// Private:
void MultiplierModule::multiplier_function() {
    tmp_op1 = binary_to_float(ioDataStruct.op1.to_string());
    tmp_op2 = binary_to_float(ioDataStruct.op2.to_string());

    tmp_result = float_to_binary(tmp_op1 * tmp_op2).c_str();    
}

void MultiplierModule::end_of_elaboration() {
}

void MultiplierModule::reset() {
}