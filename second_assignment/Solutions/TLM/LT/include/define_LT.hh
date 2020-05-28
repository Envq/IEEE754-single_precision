#ifndef define_h
#define define_h

#define DEBUG 1
#define TESTNUM 1000000
#define DELAY 100

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


union {
    float floating_point;
    uint32_t unsigned_int;
} convertitor32;

static float uint_to_float(const uint32_t &val) {
    convertitor32.unsigned_int = val;
    return convertitor32.floating_point;
}

static uint32_t float_to_uint(const float &val) {
    convertitor32.floating_point = val;
    return convertitor32.unsigned_int;
}

#define ADDRESS_TYPE int
#define DATA_TYPE iostruct

#endif