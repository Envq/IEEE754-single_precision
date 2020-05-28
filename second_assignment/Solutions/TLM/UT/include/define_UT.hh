#ifndef define_h
#define define_h

#define DEBUG 0
#define TESTNUM 1000000

#include <bitset>
#include <systemc.h>

struct iostruct {
    sc_lv<32> op1;
    sc_lv<32> op2;
    sc_lv<32> op3;
    sc_lv<32> op4;
    sc_lv<32> res1;
    sc_lv<32> res2;
};

static float binary_to_float(const std::string &val_str) {
    uint32_t val = std::bitset<32>(val_str).to_ulong();
    return *reinterpret_cast<float *>(&val);
}

static std::string float_to_binary(const float &value) {
    union {
        float input;
        int output;
    } data;

    data.input = value;
    return std::bitset<sizeof(float) * 8>(data.output).to_string();
}

#define ADDRESS_TYPE int
#define DATA_TYPE iostruct

#endif