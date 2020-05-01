#ifndef DM_H
#define DM_H

#include "multiplier_RTL.hh"
#include <systemc.h>


class DoubleMultiplierModule : public sc_core::sc_module {
  public:
    // Ports
    sc_in<bool> clk;
    sc_in<sc_logic> rst;

    sc_in<sc_logic> ready;
    sc_in<sc_lv<32>> op1;
    sc_in<sc_lv<32>> op2;

    sc_out<sc_logic> done;
    sc_out<sc_lv<32>> res;

    // Internal signals
    typedef enum{
        ST_START,
        ST_RUN1,
        ST_RUN2,
        ST_WAIT,
        ST_WAIT1,
        ST_WAIT2,
        ST_RET1,
        ST_RET2
    } DMUL_STATE;

    sc_signal<int> STATE, NEXT_STATE;
    sc_signal<sc_logic> ready1, ready2;
    sc_signal<sc_lv<32>> op1_tmp1, op2_tmp1, op1_tmp2, op2_tmp2;
    sc_signal<sc_lv<32>> res1, res2;
    sc_signal<sc_logic> done1, done2;

    // Components
    MultiplierModule *mult1;
    MultiplierModule *mult2;

    SC_HAS_PROCESS(DoubleMultiplierModule);
    DoubleMultiplierModule(const sc_module_name &module_name);
    ~DoubleMultiplierModule();

  private:
    void fsm();
    void datapath();
};


#endif