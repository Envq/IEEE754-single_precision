#include "testbench_RTL.hh"
#include "define_RTL.hh"
#include <random>


TestbenchModule::TestbenchModule(sc_module_name name) {
    SC_THREAD(run);
    sensitive << clk.pos();

    SC_THREAD(clk_gen);
}

TestbenchModule::~TestbenchModule() {
}


void TestbenchModule::clk_gen() {
    while (true) {
        clk.write(true);
        wait(PERIOD / 2, SC_NS);
        clk.write(false);
        wait(PERIOD / 2, SC_NS);
    }
}

void TestbenchModule::run() {
    // Random settings
    std::random_device seed;
    std::minstd_rand generator(seed());
    std::uniform_real_distribution<float> random_float(
        sqrt(std::numeric_limits<float>::min()),
        sqrt(std::numeric_limits<float>::max()));

    // Init
    wait();
    ready.write(sc_logic_0);
    rst.write(sc_logic_0);

    // Reset
    wait();
    rst.write(sc_logic_1);
    wait();
    rst.write(sc_logic_0);

    for (size_t i = 0; i < (DEBUG ? 1 : TESTNUM); i++) {
        // Exec
        wait();
        ready.write(sc_logic_1);
        wait();
        ready.write(sc_logic_0);
        if (!DEBUG) {
            op1.write(float_to_uint(random_float(generator)));
            op2.write(float_to_uint(random_float(generator)));
        } else {
            op1.write(0x40000000);  // 2.0
            op2.write(0x40400000);  // 3.0
        }
        wait();
        if (!DEBUG) {
            op1.write(float_to_uint(random_float(generator)));
            op2.write(float_to_uint(random_float(generator)));
        } else {
            op1.write(0x40a00000);  // 5.0
            op2.write(0x3f000000);  // 0.5
        }
        wait();

        while (done.read() == sc_logic_0)
            wait();

        if (DEBUG) {
            std::cout << "\t 1): " << res.read() << " => "
                      << uint_to_float(res.read().to_uint()) << std::endl;
        }


        wait();
        if (DEBUG) {
            std::cout << "\t 2): " << res.read() << " => "
                      << uint_to_float(res.read().to_uint()) << std::endl;
        }
    }

    sc_stop();
}