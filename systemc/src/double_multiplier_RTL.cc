#include "double_multiplier_RTL.hh"


DoubleMultiplierModule::DoubleMultiplierModule(
    const sc_module_name &module_name)
    : sc_module(module_name), clk("clk"), rst("rst"), ready("ready"),
      op1("op1"), op2("op2"), done("done"), res("res") {
    mult1 = new MultiplierModule("multiplier1");
    mult1->clk(clk);
    mult1->rst(rst);
    mult1->ready(ready1);
    mult1->op1(op1_tmp);
    mult1->op2(op2_tmp);
    mult1->done(done1);
    mult1->res(res1);

    mult2 = new MultiplierModule("multiplier2");
    mult2->clk(clk);
    mult2->rst(rst);
    mult2->ready(ready2);
    mult2->op1(op1_tmp);
    mult2->op2(op2_tmp);
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
        if (ready.read() == 1) {
            NEXT_STATE = ST_READ_SEC;
        } else {
            NEXT_STATE = STATE;
        }
        break;

    case ST_READ_SEC:
        NEXT_STATE = ST_WAIT;
        break;

    case ST_WAIT:
        if (done1.read() == 1) {
            if (done2.read() == 1) {
                NEXT_STATE = ST_RET1;
            } else {
                NEXT_STATE = ST_WAIT2;
            }
        } else {
            if (done2.read() == 1) {
                NEXT_STATE = ST_WAIT1;
            } else {
                NEXT_STATE = STATE;
            }
        }
        break;

    case ST_WAIT1:
        if (done1.read() == 1) {
            NEXT_STATE = ST_RET1;
        } else {
            NEXT_STATE = STATE;
        }
        break;

    case ST_WAIT2:
        if (done2.read() == 1) {
            NEXT_STATE = ST_RET1;
        } else {
            NEXT_STATE = STATE;
        }
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

    } else if (clk.read() == 1) {
        STATE = NEXT_STATE;

        switch (STATE) {
        case ST_START:
            if (ready.read() == 1) {
                done.write(sc_logic(0));
                ready1.write(sc_logic(1));
                ready2.write(sc_logic(0));
                op1_tmp.write(op1.read());
                op2_tmp.write(op2.read());
            } else {
                done.write(sc_logic(0));
                ready1.write(sc_logic(0));
                ready2.write(sc_logic(0));
            }

            break;

        case ST_READ_SEC:
            ready1.write(sc_logic(0));
            op1_tmp.write(op1.read());
            op2_tmp.write(op2.read());
            ready2.write(sc_logic(1));
            break;

        case ST_WAIT:
            ready2.write(sc_logic(0));
            break;

        case ST_WAIT1:
            break;

        case ST_WAIT2:
            break;

        case ST_RET1:
            done.write(sc_logic(1));
            res.write(res1.read());
            break;

        case ST_RET2:
            res.write(res2.read());
            break;

        default:
            break;
        }
    }
}
