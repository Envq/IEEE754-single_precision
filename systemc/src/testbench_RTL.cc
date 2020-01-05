#include "testbench_RTL.hh"
#include <bitset>
#include <random>
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

std::string inc_binary_string(const std::string &val_str) {
    uint32_t val = std::bitset<32>(val_str).to_ulong();
    val++;
    return float_to_binary(val);
}
//*********************

TestbenchModule::TestbenchModule(sc_module_name name) {
    // SC_THREAD(run_selected);
    // SC_THREAD(run_rnd);
    SC_THREAD(run_all);
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


void TestbenchModule::run_selected() {
    // Varibles
    std::string bits1_1, bits2_1, bits1_2, bits2_2;

    // Reset
    wait();
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    // Exec
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
              << "]" << std::endl
              << std::endl;

    sc_stop();
}


void TestbenchModule::run_rnd() {
    // Random settings
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_real_distribution<float> random_float(
        0, std::numeric_limits<float>::max());

    // Varibles
    unsigned int num_tests = 10000;
    unsigned int fails = 0;
    float num1_1, num2_1, num1_2, num2_2;
    std::string res1, res2;

    // Reset
    wait();
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    for (size_t i = 0; i < num_tests; i++) {
        // Init
        num1_1 = random_float(generator);
        num2_1 = random_float(generator);
        num1_2 = random_float(generator);
        num2_2 = random_float(generator);

        // Exec
        wait();
        ready.write(sc_logic(1));
        op1.write(float_to_binary(num1_1).c_str());
        op2.write(float_to_binary(num2_1).c_str());
        wait();
        op1.write(float_to_binary(num1_2).c_str());
        op2.write(float_to_binary(num2_2).c_str());
        ready.write(sc_logic(0));
        while (done.read() == sc_logic(0))
            wait();
        res1 = res.read().to_string();
        wait();
        res2 = res.read().to_string();

        if (res1 != float_to_binary(num1_1 * num2_1)) {
            std::cout << "ERR: [" << num1_1 << " * " << num2_1 << "]"
                      << std::endl;
            std::cout << ">>> correct result: "
                      << float_to_binary(num1_1 * num2_1) << std::endl;
            std::cout << ">>> current result: " << res1 << std::endl
                      << std::endl;
            fails++;
        }

        if (res2 != float_to_binary(num1_2 * num2_2)) {
            std::cout << "ERR: [" << num1_2 << " * " << num2_2 << "]"
                      << std::endl;
            std::cout << ">>> correct result: "
                      << float_to_binary(num1_2 * num2_2) << std::endl;
            std::cout << ">>> current result: " << res2 << std::endl
                      << std::endl;
            fails++;
        }
    }

    std::cout << std::endl << "Tests: " << num_tests << std::endl;
    std::cout << "Fails: " << fails << std::endl;
    std::cout << "Success: " << num_tests - fails << std::endl;

    sc_stop();
}


void TestbenchModule::run_all() {
    // Varibles
    std::string max_num = "01111111100000000000000000000000";
    unsigned int num_tests = 4611686018427387904;  // 2^31 * 2^31
    unsigned int fails = 0;
    float num1_1, num2_1, num1_2, num2_2;
    std::string bits1, bits2 = "01111111100000000000000000000000";
    std::string res1, res2;

    // Reset
    wait();
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    while (bits1 != max_num) {
        while (bits2 != max_num) {
            // Exec
            wait();
            ready.write(sc_logic(1));
            op1.write(bits1.c_str());
            op2.write(bits2.c_str());
            num1_1 = binary_to_float(bits1);
            num2_1 = binary_to_float(bits2);
            inc_binary_string(bits2);
            wait();
            op1.write(bits1.c_str());
            op2.write(bits2.c_str());
            num1_2 = binary_to_float(bits1);
            num2_2 = binary_to_float(bits2);
            inc_binary_string(bits2);
            ready.write(sc_logic(0));
            while (done.read() == sc_logic(0))
                wait();
            res1 = res.read().to_string();
            wait();
            res2 = res.read().to_string();

            if (res1 != float_to_binary(num1_1 * num2_1)) {
                std::cout << "ERR: [" << num1_1 << " * " << num2_1 << "]"
                          << std::endl;
                std::cout << ">>> correct result: "
                          << float_to_binary(num1_1 * num2_1) << std::endl;
                std::cout << ">>> current result: " << res1 << std::endl
                          << std::endl;
                fails++;
            }

            if (res2 != float_to_binary(num1_2 * num2_2)) {
                std::cout << "ERR: [" << num1_2 << " * " << num2_2 << "]"
                          << std::endl;
                std::cout << ">>> correct result: "
                          << float_to_binary(num1_2 * num2_2) << std::endl;
                std::cout << ">>> current result: " << res2 << std::endl
                          << std::endl;
                fails++;
            }
        }
    }

    std::cout << std::endl << "Tests: " << num_tests << std::endl;
    std::cout << "Fails: " << fails << std::endl;
    std::cout << "Success: " << num_tests - fails << std::endl;

    sc_stop();
}