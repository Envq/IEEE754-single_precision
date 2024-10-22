#include "double_multiplier_RTL.hh"


DoubleMultiplierModule::DoubleMultiplierModule(
    const sc_module_name &module_name)
    : sc_module(module_name), clk("clk"), rst("rst"), ready("ready"),
      op1("op1"), op2("op2"), done("done"), res("res") {
    mult1 = new MultiplierModule("mult1");
    mult1->clk(clk);
    mult1->rst(rst);
    mult1->ready(ready1);
    mult1->op1(op1);
    mult1->op2(op2);
    mult1->done(done1);
    mult1->res(res1);

    mult2 = new MultiplierModule("mult2");
    mult2->clk(clk);
    mult2->rst(rst);
    mult2->ready(ready2);
    mult2->op1(op1);
    mult2->op2(op2);
    mult2->done(done2);
    mult2->res(res2);

    SC_METHOD(fsm);
    sensitive << STATE << ready << done1 << done2;
    SC_METHOD(datapath);
    sensitive << clk.pos() << rst.pos();
}

DoubleMultiplierModule::~DoubleMultiplierModule() {
    delete mult1;
    delete mult2;
}

void DoubleMultiplierModule::fsm() {
    switch (STATE) {
    case ST_START:
        if (ready.read() == 1)
            NEXT_STATE = ST_RUN1;
        else
            NEXT_STATE = STATE;
        break;

    case ST_RUN1:
        NEXT_STATE = ST_RUN2;
        break;

    case ST_RUN2:
        NEXT_STATE = ST_WAIT;
        break;

    case ST_WAIT:
        if (done1.read() == 1) {
            if (done2.read() == 1)
                NEXT_STATE = ST_RET1;
            else
                NEXT_STATE = ST_WAIT2;
        } else {
            if (done2.read() == 1)
                NEXT_STATE = ST_WAIT1;
            else
                NEXT_STATE = STATE;
        }
        break;

    case ST_WAIT1:
        if (done1.read() == 1)
            NEXT_STATE = ST_RET1;
        else
            NEXT_STATE = STATE;
        break;

    case ST_WAIT2:
        if (done2.read() == 1)
            NEXT_STATE = ST_RET1;
        else
            NEXT_STATE = STATE;
        break;

    case ST_RET1:
        NEXT_STATE = ST_RET2;
        break;

    case ST_RET2:
        NEXT_STATE = ST_START;
        break;

    default:
        NEXT_STATE = ST_START;
        break;
    }
}

void DoubleMultiplierModule::datapath() {
    if (rst.read() == 1) {
        STATE = ST_START;
        done.write(sc_logic_0);
        res.write(0);
        ready1.write(sc_logic_0);
        ready2.write(sc_logic_0);

    } else if (clk.read() == 1) {
        STATE = NEXT_STATE;

        switch (NEXT_STATE) {
        case ST_START:
            done.write(sc_logic_0);
            ready1.write(sc_logic_0);
            ready2.write(sc_logic_0);
            break;

        case ST_RUN1:
            ready1.write(sc_logic_1);
            break;

        case ST_RUN2:
            ready2.write(sc_logic_1);
            break;

        case ST_WAIT:
            ready1.write(sc_logic_0);
            ready2.write(sc_logic_0);
            break;

        case ST_WAIT1:
            // Do nothing
            break;

        case ST_WAIT2:
            // Do nothing
            break;

        case ST_RET1:
            done.write(sc_logic_1);
            res.write(res1.read());
            break;

        case ST_RET2:
            res.write(res2.read());
            break;

        default:
            // Do nothing
            break;
        }
    }
}