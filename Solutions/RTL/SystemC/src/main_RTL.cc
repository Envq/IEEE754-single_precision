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

    DoubleMultiplierModule dm("top_level");
    dm.clk(clk);
    dm.rst(rst);
    dm.ready(ready);
    dm.op1(op1);
    dm.op2(op2);
    dm.done(done);
    dm.res(res);

    sc_trace_file *fp = sc_create_vcd_trace_file("bin/wave");
    sc_trace(fp, clk, "main.clk(main)");
    sc_trace(fp, rst, "main.rst(main)");
    sc_trace(fp, ready, "main.ready(main)");
    sc_trace(fp, op1, "main.op1(main)");
    sc_trace(fp, op2, "main.op2(main)");
    sc_trace(fp, done, "main.done(main)");
    sc_trace(fp, res, "main.res(main)");

    // internal segnal of DM:
    sc_trace(fp, dm.STATE, "dm.STATE(dm)");
    sc_trace(fp, dm.NEXT_STATE, "dm.NEXT_STATE(dm)");
    sc_trace(fp, dm.ready1, "dm.ready1(dm)");
    sc_trace(fp, dm.ready2, "dm.ready2(dm)");
    sc_trace(fp, dm.op1_tmp1, "dm.op1_tmp1(dm)");
    sc_trace(fp, dm.op2_tmp1, "dm.op2_tmp1(dm)");
    sc_trace(fp, dm.op1_tmp2, "dm.op1_tmp2(dm)");
    sc_trace(fp, dm.op2_tmp2, "dm.op2_tmp2(dm)");
    sc_trace(fp, dm.res1, "dm.res1(dm)");
    sc_trace(fp, dm.res2, "dm.res2(dm)");
    sc_trace(fp, dm.done1, "dm.done1(dm)");
    sc_trace(fp, dm.done2, "dm.done2(dm)");

    // internal segnal of mult1:
    sc_trace(fp, dm.mult1->STATE, "mult1.STATE(mult1)");
    sc_trace(fp, dm.mult1->NEXT_STATE, "mult1.NEXT_STATE(mult1)");
    sc_trace(fp, dm.mult1->res_type, "mult1.res_type(mult1)");
    sc_trace(fp, dm.mult1->norm_again, "mult1.norm_again(mult1)");
    sc_trace(fp, dm.mult1->sign1, "mult1.sign1(mult1)");
    sc_trace(fp, dm.mult1->sign2, "mult1.sign2(mult1)");
    sc_trace(fp, dm.mult1->esp1, "mult1.esp1(mult1)");
    sc_trace(fp, dm.mult1->esp2, "mult1.esp2(mult1)");
    sc_trace(fp, dm.mult1->mant1, "mult1.mant1(mult1)");
    sc_trace(fp, dm.mult1->mant2, "mult1.mant2(mult1)");
    sc_trace(fp, dm.mult1->esp_tmp, "mult1.esp_tmp(mult1)");
    sc_trace(fp, dm.mult1->mant_tmp, "mult1.mant_tmp(mult1)");
    sc_trace(fp, dm.mult1->op1_type, "mult1.op1_type(mult1)");
    sc_trace(fp, dm.mult1->op2_type, "mult1.op2_type(mult1)");

    // internal segnal of mult2:
    sc_trace(fp, dm.mult2->STATE, "mult2.STATE(mult2)");
    sc_trace(fp, dm.mult2->NEXT_STATE, "mult2.NEXT_STATE(mult2)");
    sc_trace(fp, dm.mult2->res_type, "mult2.res_type(mult2)");
    sc_trace(fp, dm.mult2->norm_again, "mult2.norm_again(mult2)");
    sc_trace(fp, dm.mult2->sign1, "mult2.sign1(mult2)");
    sc_trace(fp, dm.mult2->sign2, "mult2.sign2(mult2)");
    sc_trace(fp, dm.mult2->esp1, "mult2.esp1(mult2)");
    sc_trace(fp, dm.mult2->esp2, "mult2.esp2(mult2)");
    sc_trace(fp, dm.mult2->mant1, "mult2.mant1(mult2)");
    sc_trace(fp, dm.mult2->mant2, "mult2.mant2(mult2)");
    sc_trace(fp, dm.mult2->esp_tmp, "mult2.esp_tmp(mult2)");
    sc_trace(fp, dm.mult2->mant_tmp, "mult2.mant_tmp(mult2)");
    sc_trace(fp, dm.mult2->op1_type, "mult2.op1_type(mult2)");
    sc_trace(fp, dm.mult2->op2_type, "mult2.op2_type(mult2)");

    sc_start();
    sc_close_vcd_trace_file(fp);

    return 0;
};
