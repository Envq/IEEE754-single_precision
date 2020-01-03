#ifndef DM2_H
#define DM2_H

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

    // Internal signals
    typedef enum {
        ST_START,
        ST_EVAL1,
        ST_EVAL2,
        ST_EVAL3,
        ST_CHECK1,
        ST_ELAB,
        ST_UNDERF,
        ST_CHECK2,
        ST_NORM1,
        ST_ROUND,
        ST_CHECK3,
        ST_NORM2,
        ST_OVERF,
        ST_FINISH
    } MULT_STATE;

    typedef enum {
        T_NUM,
        T_NAN,
        T_ZER,
        T_INF
    } MULT_TYPE;

    sc_signal<int> STATE, NEXT_STATE;
    sc_signal<int> res_type;
    sc_signal<sc_logic> norm_again;

    // Variables
    sc_logic sign1, sign2;
    sc_lv<10> esp1, esp2;
    sc_lv<24> mant1, mant2;
    sc_lv<10> esp_tmp;
    sc_lv<48> mant_tmp;
    MULT_TYPE op1_type, op2_type;

    SC_HAS_PROCESS(MultiplierModule);
    MultiplierModule(const sc_module_name &module_name);
    ~MultiplierModule();

  private:
    void fsm();
    void datapath();
};


#endif