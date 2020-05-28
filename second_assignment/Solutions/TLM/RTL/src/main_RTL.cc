#include "double_multiplier_RTL.hh"
#include "testbench_RTL.hh"



int sc_main(int argc, char *argv[]) {
    sc_signal<bool> clk;

    sc_signal<sc_logic> rst;
    sc_signal<sc_lv<32>> op1, op2;
    sc_signal<sc_lv<32>> res;
    sc_signal<sc_logic> ready;
    sc_signal<sc_logic> done;

    TestbenchModule tb("testbench");
    tb.clk(clk);
    tb.rst(rst);
    tb.done(done);
    tb.res(res);
    tb.ready(ready);
    tb.op1(op1);
    tb.op2(op2);

    DoubleMultiplierModule dm("dut");
    dm.clk(clk);
    dm.rst(rst);
    dm.ready(ready);
    dm.op1(op1);
    dm.op2(op2);
    dm.done(done);
    dm.res(res);

    sc_start();

    return 0;
};