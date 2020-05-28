#include "double_multiplier_AT4.hh"


MultiplierModule::MultiplierModule(const sc_module_name &module_name)
    : sc_module(module_name), target_socket("target_socket"),
      pending_transaction(NULL) {
    target_socket(*this);

    SC_THREAD(IOPROCESS);
}

void MultiplierModule::b_transport(tlm::tlm_generic_payload &trans,
                                   sc_time &t) {
}

bool MultiplierModule::get_direct_mem_ptr(tlm::tlm_generic_payload &trans,
                                          tlm::tlm_dmi &dmi_data) {
    return false;
}

tlm::tlm_sync_enum
MultiplierModule::nb_transport_fw(tlm::tlm_generic_payload &trans,
                                  tlm::tlm_phase &phase, sc_time &t) {
    // If there is already a pending transaction, we refuse to
    // serve the request (note that when we return TLM_COMPLETED
    // we do not need to update the phase).
    if (pending_transaction != NULL) {
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    // Phase must be BEGIN_REQ, otherwise we set an
    // error response and complete the transaction.
    if (phase != tlm::BEGIN_REQ) {
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    if (DEBUG) cout << "\t\t[DM:] Received invocation of the nb_transport_fw primitive"
         << endl;
    pending_transaction = &trans;
    ioDataStruct = *((iostruct *)trans.get_data_ptr());


    if (DEBUG) cout << "\t\t[DM:] <<<END REQ>>>" << endl;
    phase = tlm::END_REQ;

    // activate the ioprocess process to elaborate data
    if (DEBUG) cout << "\t\t[DM:] Activating the IOPROCESS" << endl;
    io_event.notify();

    // return control
    if (DEBUG) cout << "\t\t[DM:] End of the nb_transport_fw primitive" << endl;
    return tlm::TLM_UPDATED;
}

unsigned int MultiplierModule::transport_dbg(tlm::tlm_generic_payload &trans) {
    return 0;
}


// Private:
void MultiplierModule::IOPROCESS() {
    sc_time timing_annotation;

    while (true) {
        wait(io_event);
        // When an io_event is notified, it means we must send the response back
        // to the initiator. We wait DELAY ns to model the delay required to
        // process the request.

        if (DEBUG) cout << "\t\t[DM:] IOPROCESS has been activated" << endl;
        wait(DELAY, SC_NS);  // delay

        if (pending_transaction->is_write()) {
            // write request: elaborate mults and return
            if (DEBUG) cout << "\t\t[DM:] Invoking the dm_function to calculate the mults"
                 << endl;
            dm_function();

        } else {
            // read transaction: return the result to the initiator
            if (DEBUG) cout << "\t\t[DM:] Returning results" << endl;
            ioDataStruct.res1 = tmp_res1;
            ioDataStruct.res2 = tmp_res2;
            // transaction went on correctly
            pending_transaction->set_response_status(tlm::TLM_OK_RESPONSE);

            // upload data and set phase to begin response
            *((iostruct *)pending_transaction->get_data_ptr()) = ioDataStruct;
        }



        if (DEBUG) cout << "\t\t[DM:] <<<BEGIN RESP>>>" << endl;
        tlm::tlm_phase phase = tlm::BEGIN_RESP;

        if (DEBUG) cout << "\t\t[DM:] Invoking the nb_transport_bw primitive [WRITE]"
             << endl;
        target_socket->nb_transport_bw(*pending_transaction, phase,
                                       timing_annotation);

        pending_transaction = NULL;
    }
}

void MultiplierModule::dm_function() {
    if (DEBUG) cout << "\t\t[DM:] Calculating dm_function ... " << endl;
    tmp_op1 = binary_to_float(ioDataStruct.op1.to_string());
    tmp_op2 = binary_to_float(ioDataStruct.op2.to_string());
    tmp_op3 = binary_to_float(ioDataStruct.op3.to_string());
    tmp_op4 = binary_to_float(ioDataStruct.op4.to_string());

    tmp_res1 = float_to_binary(tmp_op1 * tmp_op2).c_str();
    tmp_res2 = float_to_binary(tmp_op3 * tmp_op4).c_str();
}

void MultiplierModule::end_of_elaboration() {
}

void MultiplierModule::reset() {
}