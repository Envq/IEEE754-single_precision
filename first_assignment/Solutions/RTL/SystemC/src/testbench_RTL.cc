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
    int NUM_TEST = 12;
    unsigned int fails = 0;
    std::string mem[] = {
        // Test1
        "11111111100000000000000000000000",  //-inf
        "01000000000000000000000000000000",  // 2.0

        // Test2
        "00111111101000000000000000000000",  // 1.25
        "00000000000000000000000000000000",  // 0.0

        // Test3
        "11111111100000000000000000000000",  //-inf
        "00000000000000000000000000000000",  // 0.0

        // Test4
        "01111111100000000000000000000000",  //+inf
        "01111111100000000000000000000000",  //+inf

        // Test5
        "01000001100000100000000000000000",  // 16.25
        "01000001000100000000000000000000",  // 9.0

        // Test6
        "01000001101100000000000000000000",  // 22.0
        "11000000101000000000000000000000",  //-5.0

        // Test7
        "01000000010110011001100110011010",  // 3.400000095367431640625
        "01000000010110011001100110011010",  // 3.400000095367431640625

        // Test8
        "01000010101001111010100011110110",  // 83.8300018310546875
        "00111110000011110101110000101001",  // 0.14000000059604644775390625

        // Test9
        "01000010111101110111100011110010",  // 123.7362213134765625
        "00111010101000010011011111110100",  // 0.0012300000526010990142822265625

        // Test10
        "01000010100100011100110011001101",  // 72.90000152587890625
        "01000001000100000000000000000000",  // 9.0

        // Test11
        "01111111110000000000000000000000",  // 
        "01000001000100000000000000000000",  // 

        // Test12
        "01111111110000000000000000000010",  // 
        "01000001000100000000000000000000"   // 
    };

    // Variables;
    std::string op1_A, op2_A, res_A, res_correct_A;
    std::string op1_B, op2_B, res_B, res_correct_B;

    // Init
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(0));
    int i = 0;

    // Reset
    wait();
    rst.write(sc_logic(1));
    wait();
    rst.write(sc_logic(0));


    while (i < NUM_TEST * 2) {
        op1_A = mem[i];
        op2_A = mem[i + 1];
        op1_B = mem[i + 2];
        op2_B = mem[i + 3];

        wait();
        ready.write(sc_logic(1));
        op1.write(op1_A.c_str());
        op2.write(op2_A.c_str());
        wait();
        op1.write(op1_B.c_str());
        op2.write(op2_B.c_str());
        wait();
        ready.write(sc_logic(0));

        while (done.read() == sc_logic(0))
            wait();

        res_A = res.read().to_string();
        res_correct_A =
            float_to_binary(binary_to_float(op1_A) * binary_to_float(op2_A));
        std::cout << "Test " << 1 + i / 2 << " :"
                  << "\n    " << binary_to_float(op1_A) << " * "
                  << binary_to_float(op2_A)
                  << "\n    RES:     [" << res_A << "] == " << binary_to_float(res_A)
                  << "\n    CORRECT: [" << res_correct_A << "] == " << binary_to_float(res_correct_A) << std::endl;

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
                  << binary_to_float(op2_B)
                  << "\n    RES:     [" << res_B << "] == " << binary_to_float(res_B)
                  << "\n    CORRECT: [" << res_correct_B << "] == " << binary_to_float(res_correct_B) << std::endl;

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


    std::cout << float_to_binary(std::numeric_limits<float>::min()) << std::endl;
    std::cout << std::numeric_limits<float>::min() << std::endl;

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

    // Variables
    const unsigned int TESTS_NUM = 100000;
    const bool PRINT_MULT = false;
    const float TIME_PERIOD = 2.0;  // sec
    unsigned long time_counter = 0;
    unsigned int fails = 0;
    float num1_1, num2_1, num1_2, num2_2;
    std::string res1, res2;

    // Init
    wait();
    ready.write(sc_logic(0));
    rst.write(sc_logic(0));


    // Reset
    wait();
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