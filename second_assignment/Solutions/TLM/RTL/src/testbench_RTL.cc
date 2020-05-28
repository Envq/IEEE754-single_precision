#include "testbench_RTL.hh"
#include <bitset>
#include <chrono>
#include <random>


//### auxiliary functions ##################################
float binary_to_float(const std::string &val_str) {
    uint32_t val = std::bitset<32>(val_str).to_ulong();
    return *reinterpret_cast<float *>(&val);
}

std::string float_to_binary(const float &value) {
    union {
        float input;
        int output;
    } data;

    data.input = value;
    return std::bitset<sizeof(float) * 8>(data.output).to_string();
}
//##########################################################


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

    clock_t start_time = std::clock();
    clock_t cicle_time = start_time;


    for (size_t i = 0; i < TESTS_NUM; i++) {
        // Exec
        wait();
        ready.write(sc_logic_1);
        wait();
        ready.write(sc_logic_0);
        op1.write(float_to_binary(random_float(generator)).c_str());
        op2.write(float_to_binary(random_float(generator)).c_str());
        // op1.write("01000000000000000000000000000000");  // 2.0
        // op2.write("01000000010000000000000000000000");  // 3.0
        wait();
        op1.write(float_to_binary(random_float(generator)).c_str());
        op2.write(float_to_binary(random_float(generator)).c_str());
        // op1.write("01000000101000000000000000000000");  // 5.0
        // op2.write("00111111000000000000000000000000");  // 0.5
        wait();

        while (done.read() == sc_logic_0)
            wait();

        // std::cout << "\t 1): " << res.read() << " => "
        //           << binary_to_float(res.read().to_string()) << std::endl;


        wait();
        // std::cout << "\t 2): " << res.read() << " => "
        //           << binary_to_float(res.read().to_string()) << std::endl;
    }

    sc_stop();
}