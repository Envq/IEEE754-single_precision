#ifndef MULT_H
#define MULT_H

#include <systemc.h>

class MultiplierModule : public sc_core::sc_module {
  public:
    // Ports
    sc_in<bool> clk;
    sc_in<sc_logic> rst;
    sc_in<sc_logic> ready;
    sc_in<sc_lv<32>> op1;
    sc_in<sc_lv<32>> op2;
    sc_out<sc_logic> done;
    sc_out<sc_lv<32>> res;

    // States
    typedef enum {
        ST_START,
        ST_INIT,
        ST_SNAN1,
        ST_SNAN2,
        ST_QNAN,
        ST_ZERO,
        ST_INF,
        ST_ADJ3,
        ST_ADJ2,
        ST_ADJ1,
        ST_ELAB,
        ST_SHIFTR,
        ST_SHIFTL,
        ST_NORM,
        ST_CHECK,
        ST_SUBNORM,
        ST_ROUND,
        ST_WRITE,
        ST_FINISH
    } MULT_STATE;

    // Signals
    sc_signal<int> STATE, NEXT_STATE;
    sc_signal<sc_logic> sign1, sign2;
    sc_signal<sc_lv<8>> esp1, esp2;
    sc_signal<sc_lv<24>> mant1, mant2;
    sc_signal<sc_lv<10>> esp_tmp;
    sc_signal<sc_lv<48>> mant_tmp;

    SC_HAS_PROCESS(MultiplierModule);
    MultiplierModule(const sc_module_name &module_name);
    ~MultiplierModule();

  private:
    void fsm();
    void datapath();
};


#endif