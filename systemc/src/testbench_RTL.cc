#include "testbench_RTL.hh"
#include <cstdlib>
#include <ctime>

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
    // Init
    wait();
    std::cout << "Init" << std::endl;
    ready.write(sc_logic(0));

    // Reset
    wait();
    std::cout << "Reset" << std::endl;
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    // Ready1
    wait();
    std::cout << "Ready1" << std::endl;
    ready.write(sc_logic(1));
    op1.write("01000000000000000000000000000000");  // 2.0
    op2.write("01000000001000000000000000000000");  // 2.5
    wait();
    op1.write("00111111101000000000000000000000");  // 1.25
    op2.write("00111111100000000000000000000000");  // 1.0
    ready.write(sc_logic(0));
    while (done.read() == sc_logic(0)) wait();
    std::cout << " >>res1: " << res.read() << std::endl;
    wait();
    std::cout << " >>res2: " << res.read() << std::endl;

    // Ready2
    wait();
    std::cout << "Ready2" << std::endl;
    ready.write(sc_logic(1));
    op1.write("01000010110010000110011001100110");  // 100.2
    op2.write("10000000000000000000000000000000");  //-0.0
    wait();
    op1.write("11111111100000000000000000000000");  //-inf;
    op2.write("01000101000110000101101101110101");  // 2437.716
    ready.write(sc_logic(0));
    while (done.read() == sc_logic(0)) wait();
    std::cout << " >>res1: " << res.read() << std::endl;
    wait();
    std::cout << " >>res2: " << res.read() << std::endl;

    sc_stop();
}
