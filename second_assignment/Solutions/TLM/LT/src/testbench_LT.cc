#include "testbench_LT.hh"


TestbenchModule::TestbenchModule(sc_module_name name) : sc_module(name) {
    initiator_socket(*this);
    SC_THREAD(run);

    m_qk.set_global_quantum(sc_time(DELAY*5, SC_NS));  // set the global quantum
    m_qk.reset();                                    // reset the local quantum
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
    sc_time local_time = m_qk.get_local_time();
    iostruct dm_packet;
    tlm::tlm_generic_payload payload;

    // Random settings
    std::random_device seed;
    std::minstd_rand generator(seed());
    std::uniform_real_distribution<float> random_float(
        sqrt(std::numeric_limits<float>::min()),
        sqrt(std::numeric_limits<float>::max()));


    for (int i = 0; i < (DEBUG ? 1 : TESTNUM); i++) {
        dm_packet.op1 = float_to_uint(random_float(generator));
        dm_packet.op2 = float_to_uint(random_float(generator));
        dm_packet.op3 = float_to_uint(random_float(generator));
        dm_packet.op4 = float_to_uint(random_float(generator));
        if (DEBUG) {
            dm_packet.op1 = 0x40000000;  // 2.0
            dm_packet.op2 = 0x40400000;  // 3.0
            dm_packet.op3 = 0x40a00000;  // 5.0
            dm_packet.op4 = 0x3f000000;  // 0.5
            cout << "[TB:] Want to performe double_multiplication" << endl;
        }

        payload.set_data_ptr((unsigned char *)&dm_packet);
        payload.set_address(0);
        payload.set_write();

        // update the local time variable to send it to the target
        local_time = m_qk.get_local_time();  // retrive current local time

        // start write transaction
        if (DEBUG) cout << "[TB:] Invoking the b_transport primitive" << endl;
        initiator_socket->b_transport(payload, local_time);

        // start read transaction
        if (payload.get_response_status() == tlm::TLM_OK_RESPONSE) {
            if (DEBUG) {
                cout << "[TB:] Results are: " << endl;
                cout << "\t 1): " << dm_packet.res1 << " => "
                    << uint_to_float(dm_packet.res1.to_uint()) << endl;
                cout << "\t 2): " << dm_packet.res2 << " => "
                    << uint_to_float(dm_packet.res2.to_uint()) << endl
                    << endl;
            }
        }


        // temporal decoupling> get time and check if we have to synchronize
        // with the target
        if (DEBUG) cout << "Time: " << sc_time_stamp() << " + " << local_time << endl
             << "----------------------------------------------------" << endl;
        m_qk.set(local_time);    // time consumed by transport
        if (m_qk.need_sync()) {  // check local time against quantum
            m_qk.sync();         // synchronize if necessary
            if (DEBUG) cout << "SYNCHRONIZING" << endl
                 << "####################################################"
                 << endl;
        }
    }

    sc_stop();
}