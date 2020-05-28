#include "testbench_AT4.hh"


TestbenchModule::TestbenchModule(sc_module_name name)
    : sc_module(name), response_pending(false) {
    initiator_socket(*this);
    SC_THREAD(run);
}

void TestbenchModule::invalidate_direct_mem_ptr(uint64 start_range,
                                                uint64 end_range) {
}

tlm::tlm_sync_enum
TestbenchModule::nb_transport_bw(tlm::tlm_generic_payload &trans,
                                 tlm::tlm_phase &phase, sc_time &t) {
    // There must be a pending response.
    if (!response_pending) {
        cout << "[ERROR:] Unexpected state for nb_transport_be" << endl;
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    // The phase should be BEGIN_RESP.
    if (phase != tlm::BEGIN_RESP) {
        if (DEBUG) cout << "[ERROR:] Unexpected phase for nb_transport_be" << endl;
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    if (DEBUG) cout << "[TB:] Performing nb_transport_bw primitive" << endl;
    // reactivate the initiator process to go on with computation
    available_response.notify();

    if (DEBUG) cout << "[TB:] <<<END RESP>>>" << endl;
    phase = tlm::END_RESP;

    return tlm::TLM_COMPLETED;
}

void TestbenchModule::run() {
    sc_time local_time = SC_ZERO_TIME;
    iostruct dm_packet;
    tlm::tlm_generic_payload payload;

    // Random settings
    std::random_device seed;
    std::minstd_rand generator(seed());
    std::uniform_real_distribution<float> random_float(
        sqrt(std::numeric_limits<float>::min()),
        sqrt(std::numeric_limits<float>::max()));

    for (int i = 0; i < (DEBUG ? 1 : TESTNUM); i++) {
        dm_packet.op1 = float_to_binary(random_float(generator)).c_str();
        dm_packet.op2 = float_to_binary(random_float(generator)).c_str();
        dm_packet.op3 = float_to_binary(random_float(generator)).c_str();
        dm_packet.op4 = float_to_binary(random_float(generator)).c_str();
        if (DEBUG) {
            dm_packet.op1 = "01000000000000000000000000000000";  // 2.0
            dm_packet.op2 = "01000000010000000000000000000000";  // 3.0
            dm_packet.op3 = "01000000101000000000000000000000";  // 5.0
            dm_packet.op4 = "00111111000000000000000000000000";  // 0.5
            cout << "[TB:] Want to performe double_multiplication" << endl;
        }

        if (DEBUG) cout << "[TB:] <<<BEGIN REQ>>>" << endl;
        tlm::tlm_phase phase = tlm::BEGIN_REQ;
        payload.set_data_ptr((unsigned char *)&dm_packet);
        payload.set_address(0);
        payload.set_write();

        if (DEBUG) cout << "[TB:] Invoking the nb_transport_fw primitive [WRITE]" << endl;
        tlm::tlm_sync_enum result =
            initiator_socket->nb_transport_fw(payload, phase, local_time);

        if (result == tlm::TLM_COMPLETED) {
            // If the target immediately completes the transaction something
            // wrong happened. We should inspect the response status and
            // take appropriate actions. In this case we just stop the
            // simulation.
            if (DEBUG) cout << "[ERROR:] Transaction error on nb_transport_fw. Forcing "
                    "simulation stop."
                 << endl;
            sc_stop();
        }

        // Phase must be an end request, used by the target to notify that
        // it accepted the request
        // else, there is something wrong in the protocol
        if (phase != tlm::END_REQ) {
            if (DEBUG) cout << "[ERROR: ] Unexpected protocol phase on nb_transport_fw. "
                    "Forcing simulation stop."
                 << endl;
            sc_stop();
        }

        if (DEBUG) cout << "[TB:] Waiting for nb_transport_bw to be invoked " << endl;
        response_pending = true;   // we have one request pending
        wait(available_response);  // thus we suspend until the protocol is
                                   // finished
        if (DEBUG) cout << "[TB:] Stop to waiting for nb_transport_bw invocation" << endl;
        response_pending = false;

        // start read transaction
        if (DEBUG) cout << "[TB:] <<<BEGIN REQ>>>" << endl;
        phase = tlm::BEGIN_REQ;
        payload.set_address(0);
        payload.set_data_ptr((unsigned char *)&dm_packet);
        payload.set_read();

        if (DEBUG) cout << "[TB:] Invoking the nb_transport_fw primitive [READ]" << endl;
        result = initiator_socket->nb_transport_fw(payload, phase, local_time);

        if (result == tlm::TLM_COMPLETED) {
            if (DEBUG) cout << "[ERROR:] Transaction error on nb_transport_fw. Forcing "
                    "simulation stop."
                 << endl;
            sc_stop();
        }

        if (phase != tlm::END_REQ) {
            if (DEBUG) cout << "[ERROR: ] Unexpected protocol phase on nb_transport_fw. "
                    "Forcing simulation stop."
                 << endl;
            sc_stop();
        }

        if (DEBUG) cout << "[TB:] Waiting for nb_transport_bw to be invoked " << endl;
        response_pending = true;   // we have one request pending
        wait(available_response);  // thus we suspend until the protocol is
                                   // finished
        if (DEBUG) cout << "[TB:] Stop to waiting for nb_transport_bw invocation" << endl;
        response_pending = false;

        // start read transaction
        if (payload.get_response_status() == tlm::TLM_OK_RESPONSE && DEBUG) {
            cout << "[TB:] Results are: " << endl;
            cout << "\t 1): " << dm_packet.res1 << " => "
                 << binary_to_float(dm_packet.res1.to_string()) << endl;
            cout << "\t 2): " << dm_packet.res2 << " => "
                 << binary_to_float(dm_packet.res2.to_string()) << endl
                 << endl;
        }
    }

    sc_stop();
}