#ifndef TB_H
#define TB_H

#define PERIOD 10
#define TESTS_NUM 1000000

#include <systemc.h>

class TestbenchModule : public sc_core::sc_module {
  public:
    sc_out<bool> clk;
    sc_out<sc_logic> rst;
    sc_in<sc_lv<32>> res;
    sc_in<sc_logic> done;
    sc_out<sc_lv<32>> op1;
    sc_out<sc_lv<32>> op2;
    sc_out<sc_logic> ready;

    SC_HAS_PROCESS(TestbenchModule);
    TestbenchModule(sc_module_name name);
    ~TestbenchModule();

  private:
    void clk_gen();
    void run();
};

#endif
