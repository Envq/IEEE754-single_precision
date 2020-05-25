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
        // cout << "Unexpected state" << endl;
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    // The phase should be BEGIN_RESP.
    if (phase != tlm::BEGIN_RESP) {
        // cout << "Unexpected phase" << endl;
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    available_response.notify();
    phase = tlm::END_RESP;

    return tlm::TLM_COMPLETED;
}

void TestbenchModule::run() {
    // TLM settings
    sc_time local_time = SC_ZERO_TIME;
    iostruct mult_packet;
    tlm::tlm_generic_payload payload;

    // Random settings
    std::random_device seed;
    std::minstd_rand generator1(seed());
    std::uniform_real_distribution<float> random_float(
        sqrt(std::numeric_limits<float>::min()),
        sqrt(std::numeric_limits<float>::max()));


    for (int i = 0; i < TESTS_NUM; i++) {
        mult_packet.op1 = float_to_binary(random_float(generator1)).c_str();
        mult_packet.op2 = float_to_binary(random_float(generator1)).c_str();

        tlm::tlm_phase phase = tlm::BEGIN_REQ;
        payload.set_data_ptr((unsigned char *)&mult_packet);
        payload.set_address(0);
        payload.set_write();

        tlm::tlm_sync_enum result =
            initiator_socket->nb_transport_fw(payload, phase, local_time);

        if (result == tlm::TLM_COMPLETED) {
            // If the target immediately completes the transaction something
            // wrong happened. We should inspect the response status and
            // take appropriate actions.
            std::cout << "Transaction error. Forcing simulation stop."
                      << std::endl;
            sc_stop();
        }

        // Phase must be END_REQ
        if (phase != tlm::END_REQ) {
            std::cout << "Unexpected protocol phase. Forcing simulation stop."
                      << std::endl;
            sc_stop();
        }

        response_pending = true;
        wait(available_response);
        response_pending = false;

        // start read transaction
        phase = tlm::BEGIN_REQ;
        payload.set_address(0);
        payload.set_data_ptr((unsigned char *)&mult_packet);
        payload.set_read();

        result = initiator_socket->nb_transport_fw(payload, phase, local_time);

        if (result == tlm::TLM_COMPLETED) {
            // If the target immediately completes the transaction something
            // wrong happened. We should inspect the response status and
            // take appropriate actions. 
            std::cout << "Transaction error. Forcing simulation stop."
                      << std::endl;
            sc_stop();
        }

        // Phase must be END_REQ
        if (phase != tlm::END_REQ) {
            std::cout << "Unexpected protocol phase. Forcing simulation stop."
                      << std::endl;
            sc_stop();
        }

        response_pending = true;
        wait(available_response);
        response_pending = false;

        // start read transaction
        if (payload.get_response_status() == tlm::TLM_OK_RESPONSE) {
            // cout << "\t is: " << root_packet.result << endl;
        }
    }

    sc_stop();
}