#include "testbench_RTL.hh"
#include <bitset>
#include <sstream>



//** auxiliary functions
float binary_to_float(const std::string &val_str) {
    uint32_t val = std::bitset<32>(val_str).to_ulong();
    return *reinterpret_cast<float *>(&val);
}

std::string float_to_binary(const float &value) {
    union {
        float input;
        std::uint32_t output;
    } data;

    data.input = value;
    return std::bitset<sizeof(float) * 8>(data.output).to_string();
}
//*********************

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
    std::string bits1_1, bits2_1, bits1_2, bits2_2;


    // Init
    wait();
    ready.write(sc_logic(0));

    // Reset
    wait();
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    // Ready1************************
    wait();
    ready.write(sc_logic(1));
    bits1_1 = "01000000000000000000000000000000";  // 2.0
    bits2_1 = "01000000001000000000000000000000";  // 2.5
    op1.write(bits1_1.c_str());
    op2.write(bits2_1.c_str());
    wait();
    bits1_2 = "00111111101000000000000000000000";  // 1.25
    bits2_2 = "00111111100000000000000000000000";  // 1.0
    op1.write(bits1_2.c_str());
    op2.write(bits2_2.c_str());
    ready.write(sc_logic(0));
    while (done.read() == sc_logic(0))
        wait();
    std::cout << "Mult1: " << binary_to_float(bits1_1) << " * "
              << binary_to_float(bits2_1) << " = "
              << binary_to_float(res.read().to_string()) << "[" << res.read()
              << "]" << std::endl;
    wait();
    std::cout << "Mult2: " << binary_to_float(bits1_2) << " * "
              << binary_to_float(bits2_2) << " = "
              << binary_to_float(res.read().to_string()) << "[" << res.read()
              << "]" << std::endl << std::endl;
    //*******************************

    // Ready2************************
    wait();
    ready.write(sc_logic(1));
    bits1_1 = "01000010110010000110011001100110";  // 100.2
    bits2_1 = "10000000000000000000000000000000";  //-0.0
    op1.write(bits1_1.c_str());
    op2.write(bits2_1.c_str());
    wait();
    bits1_2 = "11111111100000000000000000000000";  //-inf;
    bits2_2 = "01000101000110000101101101110101";  // 2437.716
    op1.write(bits1_2.c_str());
    op2.write(bits2_2.c_str());
    ready.write(sc_logic(0));
    while (done.read() == sc_logic(0))
        wait();
    std::cout << "Mult1: " << binary_to_float(bits1_1) << " * "
              << binary_to_float(bits2_1) << " = "
              << binary_to_float(res.read().to_string()) << "[" << res.read()
              << "]" << std::endl;
    wait();
    std::cout << "Mult2: " << binary_to_float(bits1_2) << " * "
              << binary_to_float(bits2_2) << " = "
              << binary_to_float(res.read().to_string()) << "[" << res.read()
              << "]" << std::endl << std::endl;
    //*******************************

    sc_stop();
}
