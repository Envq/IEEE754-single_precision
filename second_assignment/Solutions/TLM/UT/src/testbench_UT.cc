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

        payload.set_data_ptr((unsigned char *)&dm_packet);
        payload.set_address(0);
        payload.set_write();

        // start write transaction
        initiator_socket->b_transport(payload, local_time);


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