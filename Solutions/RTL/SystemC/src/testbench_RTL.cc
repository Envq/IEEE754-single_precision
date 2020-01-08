#include "testbench_RTL.hh"
#include <bitset>
#include <chrono>
#include <random>



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
    return std::bitset<32>(++val).to_string();
}
//*********************

TestbenchModule::TestbenchModule(sc_module_name name) {
    SC_THREAD(targeted_test);
    // SC_THREAD(rnd_test);
    // SC_THREAD(run_all);
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


void TestbenchModule::targeted_test() {
    // Variables
    std::string bits1_1, bits2_1, bits1_2, bits2_2;

    // Init
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(0));

    // Wait for startup FPGA
    for (size_t i = 0; i < 1; i++) {
        wait();
    }

    // Reset
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));
    wait();

    // Exec1
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
              << "]" << std::endl
              << std::endl;

    // Exec2
    wait();
    ready.write(sc_logic(1));
    bits1_1 = "01000010110010000110011001100110";  // 100.2
    bits2_1 = "01111111111111111111111111111111";  // NAN
    op1.write(bits1_1.c_str());
    op2.write(bits2_1.c_str());
    wait();
    bits1_2 = "11111111100000000000000000000000";  //-inf;
    bits2_2 = "10000000000000000000000000000000";  //-0.0
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


void TestbenchModule::rnd_test() {
    // Random settings
    std::random_device seed;
    std::mt19937 generator(seed());
    std::minstd_rand generator1(seed());
    std::ranlux48_base generator2(seed());
    std::uniform_real_distribution<float> random_float(
        0, sqrt(std::numeric_limits<float>::max()));

    // Variables
    const unsigned int TESTS_NUM = 1000;
    const bool PRINT_MULT = false;
    const float TIME_PERIOD = 2.0;  // sec
    unsigned long time_counter = 0;
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

    clock_t start_time = std::clock();
    clock_t cicle_time = start_time;

    for (size_t mult_counter = 0; mult_counter < (TESTS_NUM / 2);
         mult_counter++) {
        // Init
        num1_1 = random_float(generator1);
        num2_1 = random_float(generator1);
        num1_2 = random_float(generator1);
        num2_2 = random_float(generator1);

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
        } else if (PRINT_MULT) {
            std::cout << "Mult1: " << num1_1 << " * " << num2_1 << " = "
                      << binary_to_float(res1) << "[" << res1 << "]"
                      << std::endl;
        }

        if (res2 != float_to_binary(num1_2 * num2_2)) {
            std::cout << "ERR: [" << num1_2 << " * " << num2_2 << "]"
                      << std::endl;
            std::cout << ">>> correct result: "
                      << float_to_binary(num1_2 * num2_2) << std::endl;
            std::cout << ">>> current result: " << res2 << std::endl
                      << std::endl;
            fails++;
        } else if (PRINT_MULT) {
            std::cout << "Mult2: " << num1_2 << " * " << num2_2 << " = "
                      << binary_to_float(res2) << "[" << res2 << "]"
                      << std::endl;
        }

        if ((static_cast<float>(std::clock() - cicle_time) / CLOCKS_PER_SEC) >
            TIME_PERIOD) {
            cicle_time = std::clock();
            std::cout << ++time_counter << " of [" << TIME_PERIOD << " sec]"
                      << " ==> " << mult_counter << " mult done." << std::endl;
        }
    }
    clock_t end_time = std::clock();
    float time_elapsed =
        static_cast<float>(end_time - start_time) / CLOCKS_PER_SEC;

    std::cout << std::endl << "Tests: " << TESTS_NUM << std::endl;
    std::cout << "Fails: " << fails << std::endl;
    std::cout << "Success: " << TESTS_NUM - fails << std::endl;
    std::cout << "Time elapsed: " << time_elapsed << std::endl;

    sc_stop();
}


void TestbenchModule::full_test() {
    // Variables
    const float TIME_PERIOD = 60.0;  // sec
    std::string max_num = "01111111100000000000000000000000";
    unsigned long time_counter = 0;
    unsigned long mult_counter = 0;
    unsigned long fails = 0;
    float num1_1, num2_1, num1_2, num2_2;
    std::string bits1 = "00000000100000000000000000000000";
    std::string bits2 = "00000000100000000000000000000000";
    std::string res1, res2;

    // Reset
    wait();
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));

    clock_t start_time = std::clock();
    clock_t cicle_time = start_time;

    while (bits1 != max_num) {
        while (bits2 != max_num) {
            // Exec
            wait();
            ready.write(sc_logic(1));
            op1.write(bits1.c_str());
            op2.write(bits2.c_str());
            num1_1 = binary_to_float(bits1);
            num2_1 = binary_to_float(bits2);
            bits2 = inc_binary_string(bits2);
            wait();
            op1.write(bits1.c_str());
            op2.write(bits2.c_str());
            num1_2 = binary_to_float(bits1);
            num2_2 = binary_to_float(bits2);
            bits2 = inc_binary_string(bits2);
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

            mult_counter++;

            if ((static_cast<float>(std::clock() - cicle_time) /
                 CLOCKS_PER_SEC) > TIME_PERIOD) {
                cicle_time = std::clock();
                std::cout << ++time_counter << " of [" << TIME_PERIOD << " sec]"
                          << " ==> " << mult_counter << " mult done."
                          << std::endl;
            }
        }

        bits1 = inc_binary_string(bits1);
        bits2 = "00000000000000000000000000000000";
    }
    clock_t end_time = std::clock();
    float time_elapsed =
        static_cast<float>(end_time - start_time) / CLOCKS_PER_SEC;

    std::cout << std::endl << "Tests: " << std::endl;
    std::cout << "--Fails: " << fails << std::endl;
    std::cout << "--Time elapsed: " << time_elapsed << std::endl;

    sc_stop();
}