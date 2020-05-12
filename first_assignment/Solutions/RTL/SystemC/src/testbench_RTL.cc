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


// MEMORY
const std::string mem[] = {
    // Test 1
    "00000000000000000000000000000000",  // 0.0
    "11111111100000000000000000000000",  //-inf

    // Test 2
    "01111111100000000000000000000000",  //+inf
    "00000000000000000000000000000000",  // 0.0

    // Test 3
    "01111111100000000000000000000110",  // nan signal
    "00111111101000000000000000000000",  // 1.25

    // Test 4
    "00111111101000000000000000000000",  // 1.25
    "11111111100000000000010000000110",  // nan signal

    // Test 5
    "00111111101000000000000000000000",  // 1.25
    "00000000000000000000000000000000",  // 0.0

    // Test 6
    "10000000000000000000000000000000",  //-0.0
    "00111111101000000000000000000000",  // 1.25

    // Test 7
    "11111111100000000000000000000000",  //-inf
    "01000000000000000000000000000000",  // 2.0

    // Test 8
    "01000000000000000000000000000000",  // 2.0
    "01111111100000000000000000000000",  // inf

    // Test 9
    "00000000010000000000000000000000",  // 5.87747175411e-39 (DENOM)
    "00000000010000000000000000000000",  // 5.87747175411e-39 (DENOM)

    // Test 10
    "00111111110000000000000000000000",  // 1.5
    "00000000010000000000000000000000",  // 5.87747175411e-39 (DENOM)

    // Test 11
    "00000000010000000000000000000000",  // 5.87747175411e-39 (DENOM)
    "00111110110000000000000000000000",  // 0.375

    // Test 12
    "01100000000000000000000000000000",  // 36893488147419103232
    "01100000000000000000000000000000",  // 36893488147419103232

    // Test 13
    "01000010111101110111100011110010",  // 123.7362213134765625
    "00111010101000010011011111110100",  // 0.0012300000526010990142822265625

    // Test 14
    "01000010100100011100110011001101",  // 72.90000152587890625
    "01000001000100000000000000000000",  // 9.0

    // Test 15
    "01000000011111111111111111111110",  // 3.999999523162841796875
    "01000000000000000000000000000001",  // 2.0000002384185791015625

    // Test 16
    "01000001000000000000000000000000",  // 8.0
    "00000000000000000000000000000001",  // 1.40129846432e-45 (DENOM)

    // Test 17
    "01000001100000000000000000000000",  // 16.0
    "00000000010000000000000000000000",  // 5.87747175411e-39 (DENOM)

    // Test 18
    "00111100000000000000000000000000",  // 0.0078125
    "00000011100000000000000000000000",  // 7.52316384526e-37

    // Test 19
    "01000001010110000000000000000000",  // 13.5
    "00111111100000000000000000000000",  // 1.0

    // Test 20
    "01000000000000000000000000000000",  // 2.0
    "01000000001000000000000000000000",  // 2.5
};


TestbenchModule::TestbenchModule(sc_module_name name) {
    // SC_THREAD(target_test);
    // SC_THREAD(full_target_test);
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


void TestbenchModule::target_test() {
    // Variables
    std::string num1, num2, res_mult, res_correct;

    // Settings
    int numTest = 20;

    // Init
    wait();
    ready.write(sc_logic_0);
    rst.write(sc_logic_0);

    // Reset
    wait();
    rst.write(sc_logic_1);
    wait();
    rst.write(sc_logic_0);

    // Test
    num1 = mem[(numTest * 2) - 2];
    num2 = mem[(numTest * 2) - 1];
    std::cout << "Test " << numTest << " :"
              << "\n    " << binary_to_float(num1) << " * "
              << binary_to_float(num2) << std::endl;

    wait();
    ready.write(sc_logic_1);
    wait();
    ready.write(sc_logic_0);
    op1.write(num1.c_str());
    op2.write(num2.c_str());
    wait();
    wait();

    while (done.read() == sc_logic_0)
        wait();

    res_mult = res.read().to_string();
    res_correct =
        float_to_binary(binary_to_float(num1) * binary_to_float(num2));
    std::cout << "    RES:     [" << res_mult
              << "] == " << binary_to_float(res_mult) << "\n    CORRECT: ["
              << res_correct << "] == " << binary_to_float(res_correct)
              << std::endl;

    if (res_mult == res_correct) {
        std::cout << "==> OK\n" << std::endl;
    } else {
        std::cout << "==> FAILED\n" << std::endl;
    }

    sc_stop();
}


void TestbenchModule::full_target_test() {
    int NUM_TEST = 20;
    unsigned int fails = 0;

    // Variables;
    std::string op1_A, op2_A, res_A, res_correct_A;
    std::string op1_B, op2_B, res_B, res_correct_B;

    // Init
    wait();
    ready.write(sc_logic_0);
    rst.write(sc_logic_0);

    // Reset
    wait();
    rst.write(sc_logic_1);
    wait();
    rst.write(sc_logic_0);

    // Test
    int i = 0;
    while (i < NUM_TEST * 2) {
        op1_A = mem[i];
        op2_A = mem[i + 1];
        op1_B = mem[i + 2];
        op2_B = mem[i + 3];

        wait();
        ready.write(sc_logic_1);
        wait();
        ready.write(sc_logic_0);
        op1.write(op1_A.c_str());
        op2.write(op2_A.c_str());
        wait();
        op1.write(op1_B.c_str());
        op2.write(op2_B.c_str());
        wait();

        while (done.read() == sc_logic_0)
            wait();

        res_A = res.read().to_string();
        res_correct_A =
            float_to_binary(binary_to_float(op1_A) * binary_to_float(op2_A));
        std::cout << "Test " << 1 + i / 2 << " :"
                  << "\n    " << binary_to_float(op1_A) << " * "
                  << binary_to_float(op2_A) << "\n    RES:     [" << res_A
                  << "] == " << binary_to_float(res_A) << "\n    CORRECT: ["
                  << res_correct_A << "] == " << binary_to_float(res_correct_A)
                  << std::endl;

        if (res_A == res_correct_A) {
            std::cout << "==> OK\n" << std::endl;
        } else {
            std::cout << "==> FAILED\n" << std::endl;
            fails++;
        }

        wait();
        res_B = res.read().to_string();
        res_correct_B =
            float_to_binary(binary_to_float(op1_B) * binary_to_float(op2_B));
        std::cout << "Test " << 2 + i / 2 << " :"
                  << "\n    " << binary_to_float(op1_B) << " * "
                  << binary_to_float(op2_B) << "\n    RES:     [" << res_B
                  << "] == " << binary_to_float(res_B) << "\n    CORRECT: ["
                  << res_correct_B << "] == " << binary_to_float(res_correct_B)
                  << std::endl;

        if (res_B == res_correct_B) {
            std::cout << "==> OK\n" << std::endl;
        } else {
            std::cout << "==> FAILED\n" << std::endl;
            fails++;
        }

        i += 4;
    }

    std::cout << std::endl << "Tests: " << NUM_TEST << std::endl;
    std::cout << "Fails: " << fails << std::endl;
    std::cout << "Success: " << NUM_TEST - fails << std::endl;

    sc_stop();
}


void TestbenchModule::rnd_test() {
    // Random settings
    std::random_device seed;
    std::mt19937 generator(seed());
    std::minstd_rand generator1(seed());
    std::ranlux48_base generator2(seed());
    std::uniform_real_distribution<float> random_float(
        sqrt(std::numeric_limits<float>::min()),
        sqrt(std::numeric_limits<float>::max()));

    // Setttings
    const unsigned int TESTS_NUM = 100000;
    const bool PRINT_MULT = false;
    const float TIME_PERIOD = 2.0;  // sec

    // Variables
    unsigned long time_counter = 0;
    unsigned int fails = 0;
    float num1_A, num2_A, num1_B, num2_B;
    std::string res1, res2;

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

    for (size_t mult_counter = 0; mult_counter < (TESTS_NUM / 2);
         mult_counter++) {
        // Init
        num1_A = random_float(generator1);
        num2_A = random_float(generator1);
        num1_B = random_float(generator1);
        num2_B = random_float(generator1);

        // Exec
        wait();
        ready.write(sc_logic_1);
        wait();
        ready.write(sc_logic_0);
        op1.write(float_to_binary(num1_A).c_str());
        op2.write(float_to_binary(num2_A).c_str());
        wait();
        op1.write(float_to_binary(num1_B).c_str());
        op2.write(float_to_binary(num2_B).c_str());
        wait();

        while (done.read() == sc_logic_0)
            wait();

        res1 = res.read().to_string();
        if (res1 != float_to_binary(num1_A * num2_A)) {
            std::cout << "ERR: [" << num1_A << " * " << num2_A << "]"
                      << std::endl;
            std::cout << ">>> correct result: "
                      << float_to_binary(num1_A * num2_A) << std::endl;
            std::cout << ">>> current result: " << res1 << std::endl
                      << std::endl;
            fails++;
        } else if (PRINT_MULT) {
            std::cout << "Mult1: " << num1_A << " * " << num2_A << " = "
                      << binary_to_float(res1) << "[" << res1 << "]"
                      << std::endl;
        }

        wait();
        res2 = res.read().to_string();
        if (res2 != float_to_binary(num1_B * num2_B)) {
            std::cout << "ERR: [" << num1_B << " * " << num2_B << "]"
                      << std::endl;
            std::cout << ">>> correct result: "
                      << float_to_binary(num1_B * num2_B) << std::endl;
            std::cout << ">>> current result: " << res2 << std::endl
                      << std::endl;
            fails++;
        } else if (PRINT_MULT) {
            std::cout << "Mult2: " << num1_B << " * " << num2_B << " = "
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