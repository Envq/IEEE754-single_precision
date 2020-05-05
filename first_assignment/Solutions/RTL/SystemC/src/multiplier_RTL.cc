#include "multiplier_RTL.hh"


MultiplierModule::MultiplierModule(const sc_module_name &module_name)
    : sc_module(module_name), clk("clk"), rst("rst"), ready("ready"),
      op1("op1"), op2("op2"), done("done"), res("res") {
    SC_METHOD(fsm);
    sensitive << STATE << ready << res_type << norm_again;
    SC_METHOD(datapath);
    sensitive << clk.pos() << rst.pos();
}

MultiplierModule::~MultiplierModule() {
}

void MultiplierModule::fsm() {
    switch (STATE) {
    case ST_START:
        if (ready.read() == 1) {
            NEXT_STATE = ST_EVAL1;
        } else {
            NEXT_STATE = STATE;
        }
        break;

    case ST_EVAL1:
        NEXT_STATE = ST_EVAL2;
        break;

    case ST_EVAL2:
        NEXT_STATE = ST_EVAL3;
        break;

    case ST_EVAL3:
        NEXT_STATE = ST_CHECK1;
        break;

    case ST_CHECK1:
        if (res_type.read() == T_NUM) {
            NEXT_STATE = ST_ELAB;
        } else {
            NEXT_STATE = ST_FINISH;
        }
        break;

    case ST_ELAB:
        NEXT_STATE = ST_UNDERF;
        break;

    case ST_UNDERF:
        NEXT_STATE = ST_CHECK2;
        break;

    case ST_CHECK2:
        if (res_type.read() == T_NUM) {
            NEXT_STATE = ST_NORM1;
        } else {
            NEXT_STATE = ST_FINISH;
        }
        break;

    case ST_NORM1:
        NEXT_STATE = ST_ROUND;
        break;

    case ST_ROUND:
        NEXT_STATE = ST_CHECK3;
        break;

    case ST_CHECK3:
        if (norm_again.read() == 1) {
            NEXT_STATE = ST_NORM2;
        } else {
            NEXT_STATE = ST_OVERF;
        }
        break;

    case ST_NORM2:
        NEXT_STATE = ST_OVERF;
        break;

    case ST_OVERF:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_FINISH:
        NEXT_STATE = ST_START;
        break;

    default:
        NEXT_STATE = STATE;
        break;
    }
}

void MultiplierModule::datapath() {
    if (rst.read() == 1) {                   //Reset regs
            STATE = ST_START;
            done.write(sc_logic(0));           
            norm_again.write(sc_logic(0));   
            sign1 = sc_logic(0);             
            esp1 = "0000000000";
            mant1 = "000000000000000000000000";
            sign2 = sc_logic(0);             
            esp2 = "0000000000";
            mant2 = "000000000000000000000000";
            op1_type = T_NUM;
            op2_type = T_NUM;
            res_type = T_NUM;
            mant_tmp = "000000000000000000000000000000000000000000000000";
            esp_tmp = "0000000000";
            res = "00000000000000000000000000000000";

    } else if (clk.read() == 1) {
        STATE = NEXT_STATE;

        switch (NEXT_STATE) {
        case ST_START:
            done.write(sc_logic(0));
            norm_again.write(sc_logic(0));
            sign1 = op1.read()[31];
            esp1 = ("00", op1.read().range(30, 23));
            mant1 = ('1', op1.read().range(22, 0));
            sign2 = op2.read()[31];
            esp2 = ("00", op2.read().range(30, 23));
            mant2 = ('1', op2.read().range(22, 0));
            break;

        case ST_EVAL1:
            if (esp1.range(7, 0) == 255) {
                if (mant1.range(22, 0) == 0) {
                    op1_type = T_INF;
                } else {
                    op1_type = T_NAN;
                }
            } else {
                if (esp1.range(7, 0) == 0 && (mant1.range(22, 0) == 0)) {
                    op1_type = T_ZER;
                } else {
                    op1_type = T_NUM;
                }
            }
            break;

        case ST_EVAL2:
            if (esp2.range(7, 0) == 255) {
                if (mant2.range(22, 0) == 0) {
                    op2_type = T_INF;
                } else {
                    op2_type = T_NAN;
                }
            } else {
                if (esp2.range(7, 0) == 0 && (mant2.range(22, 0) == 0)) {
                    op2_type = T_ZER;
                } else {
                    op2_type = T_NUM;
                }
            }
            break;

        case ST_EVAL3:
            if (op1_type == T_NAN || op2_type == T_NAN ||
                (op1_type == T_ZER && op2_type == T_INF) ||
                (op2_type == T_ZER && op1_type == T_INF)) {
                res_type = T_NAN;
            } else if (op1_type == T_ZER || op2_type == T_ZER) {
                res_type = T_ZER;
            } else if (op1_type == T_INF || op2_type == T_INF) {
                res_type = T_INF;
            } else {
                res_type = T_NUM;
            }
            break;

        case ST_CHECK1:
            break;

        case ST_ELAB:
            esp_tmp = esp1.to_int() + esp2.to_int() - 127;
            mant_tmp = mant1.to_uint64() * mant2.to_uint64();
            break;

        case ST_UNDERF:
            if (esp_tmp[9] == 1) {
                res_type = T_ZER;
            } else {
                res_type = T_NUM;
            }
            break;

        case ST_CHECK2:
            break;

        case ST_NORM1:
            if (mant_tmp[47] == 1) {
                esp_tmp = esp_tmp.to_int() + 1;
            } else {
                mant_tmp = mant_tmp << 1;
            }

            break;

        case ST_ROUND:
            if (mant_tmp[23] == 1) {
                norm_again.write(sc_logic(1));
            } else {
                norm_again.write(sc_logic(0));
            }
            break;

        case ST_CHECK3:
            break;

        case ST_NORM2:
            if (mant_tmp.range(46, 24) == 8388607) {
                esp_tmp = esp_tmp.to_int() + 1;
            }
            mant_tmp.range(46, 24) = mant_tmp.range(46, 24).to_uint() + 1;
            break;

        case ST_OVERF:
            if (esp_tmp[8] == 1) {
                res_type = T_INF;
            } else {
                res_type = T_NUM;
            }
            break;

        case ST_FINISH:
            res_tmp << (sign1 xor sign2);
            switch (res_type) {
            case T_ZER:
                res_tmp << "0000000000000000000000000000000";
                res.write(res_tmp.str().c_str());
                break;

            case T_INF:
                res_tmp << "1111111100000000000000000000000";
                res.write(res_tmp.str().c_str());
                break;

            case T_NAN:
                res_tmp << "1111111111111111111111111111111";
                res.write(res_tmp.str().c_str());
                break;

            case T_NUM:
                res_tmp << esp_tmp.range(7, 0) << mant_tmp(46, 24);
                res.write(res_tmp.str().c_str());
                break;

            default:
                break;
            }
            done.write(sc_logic(1));
            res_tmp.str("");    // Clean stream
            break;

        default:
            break;
        }
    }
}
