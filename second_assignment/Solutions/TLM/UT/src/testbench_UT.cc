#include "testbench_UT.hh"


TestbenchModule::TestbenchModule(sc_module_name name) : sc_module(name) {
    initiator_socket(*this);
    SC_THREAD(run);
}

void TestbenchModule::invalidate_direct_mem_ptr(uint64 start_range,
                                                uint64 end_range) {
}

tlm::tlm_sync_enum
TestbenchModule::nb_transport_bw(tlm::tlm_generic_payload &trans,
                                 tlm::tlm_phase &phase, sc_time &t) {
    return tlm::TLM_COMPLETED;
}

void TestbenchModule::run() {
    // TLM settings
    sc_time local_time;
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

        payload.set_data_ptr((unsigned char *)&mult_packet);
        payload.set_address(0);
        payload.set_write();

        // start write transaction
        initiator_socket->b_transport(payload, local_time);

        // start read transaction
        if (payload.get_response_status() == tlm::TLM_OK_RESPONSE) {
            // cout << "\t is: " << root_packet.result << endl;           
        }
    }

    sc_stop();
}