#include "testbench_RTL.hh"
#include <bitset>
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
    SC_THREAD(rnd_test);
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


void TestbenchModule::rnd_test() {
    // Random settings
    std::random_device seed;
    std::minstd_rand generator1(seed());
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


    for (size_t mult_counter = 0; mult_counter < TESTS_NUM; mult_counter++) {
        // Exec
        wait();
        op1.write(float_to_binary(random_float(generator1)).c_str());
        op2.write(float_to_binary(random_float(generator1)).c_str());
        ready.write(sc_logic_1);
        wait();
        ready.write(sc_logic_0);
        wait();

        while (done.read() == sc_logic_0)
            wait();
    }

    sc_stop();
}