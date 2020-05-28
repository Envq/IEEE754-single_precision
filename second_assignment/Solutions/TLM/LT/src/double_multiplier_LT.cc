#include "double_multiplier_LT.hh"


MultiplierModule::MultiplierModule(const sc_module_name &module_name)
    : sc_module(module_name), target_socket("target_socket"),
      pending_transaction(NULL) {
    target_socket(*this);
}

void MultiplierModule::b_transport(tlm::tlm_generic_payload &trans,
                                   sc_time &t) {
    timing_annotation = SC_ZERO_TIME;

    ioDataStruct = *((iostruct *)trans.get_data_ptr());

    if (trans.is_write()) {
        if (DEBUG) cout << "\t\t[DM:] Received invocation of the b_transport primitive [WRITE]"
             << endl;
        if (DEBUG) cout << "\t\t[DM:] Invoking the dm_function to calculate the mults"
             << endl;
        dm_function();
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        ioDataStruct.res1 = tmp_res1;
        ioDataStruct.res2 = tmp_res2;
        *((iostruct *)trans.get_data_ptr()) = ioDataStruct;

    } else if (trans.is_read()) {
        if (DEBUG) cout << "\t\t[DM:] Received invocation of the b_transport primitive [READ]"
             << endl;
        ioDataStruct.res1 = tmp_res1;
        ioDataStruct.res2 = tmp_res2;
        if (DEBUG) cout << "\t\t[DM:] Returning results: " << tmp_res1 << " and "
             << tmp_res2 << endl;
        *((iostruct *)trans.get_data_ptr()) = ioDataStruct;
    }
    t += timing_annotation;
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
void MultiplierModule::dm_function() {
    if (DEBUG) cout << "\t\t[DM:] Calculating dm_function ... " << endl;
    tmp_op1 = binary_to_float(ioDataStruct.op1.to_string());
    tmp_op2 = binary_to_float(ioDataStruct.op2.to_string());
    tmp_op3 = binary_to_float(ioDataStruct.op3.to_string());
    tmp_op4 = binary_to_float(ioDataStruct.op4.to_string());

    tmp_res1 = float_to_binary(tmp_op1 * tmp_op2).c_str();
    tmp_res2 = float_to_binary(tmp_op3 * tmp_op4).c_str();
    timing_annotation += sc_time(DELAY, SC_NS);
}

void MultiplierModule::end_of_elaboration() {
}

void MultiplierModule::reset() {
}