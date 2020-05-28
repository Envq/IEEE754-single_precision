#ifndef define_h
#define define_h

#define DEBUG 0
#define TESTNUM 1000000
#define PERIOD 10

#include <bitset>


//### auxiliary functions ##################################
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
//##########################################################


#endif