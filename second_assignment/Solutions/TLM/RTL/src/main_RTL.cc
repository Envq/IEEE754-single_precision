#include "testbench_RTL.hh"
#include "multiplier_RTL.hh"



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

    MultiplierModule m("dut");
    m.clk(clk);
    m.rst(rst);
    m.ready(ready);
    m.op1(op1);
    m.op2(op2);
    m.done(done);
    m.res(res);

    sc_start();

    return 0;
};