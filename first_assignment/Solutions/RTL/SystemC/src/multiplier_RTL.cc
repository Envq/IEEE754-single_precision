#include "multiplier_RTL.hh"


MultiplierModule::MultiplierModule(const sc_module_name &module_name)
    : sc_module(module_name), clk("clk"), rst("rst"), ready("ready"),
      op1("op1"), op2("op2"), done("done"), res("res") {

    SC_METHOD(fsm);
    sensitive << STATE << ready << esp1 << mant1 << esp2 << mant2 << esp_tmp
              << mant_tmp;              
    SC_METHOD(datapath);
    sensitive << clk.pos() << rst.pos();
}

MultiplierModule::~MultiplierModule() {
}

void MultiplierModule::fsm() {
    switch (STATE) {
    case ST_START:
        if (ready.read() == 1)
            NEXT_STATE = ST_INIT;
        else
            NEXT_STATE = STATE;
        break;

    case ST_INIT:
        if (((esp1.read() == 0 && mant1.read().range(22, 0) == 0) &&
             (esp2.read() == 255 && mant2.read().range(22, 0) == 0)) ||
            ((esp1.read() == 255 && mant1.read().range(22, 0) == 0) &&
             (esp2.read() == 0 && mant2.read().range(22, 0) == 0)))
            NEXT_STATE = ST_QNAN;

        else if (esp2.read() == 255 && mant2.read().range(22, 0) != 0)
            NEXT_STATE = ST_SNAN2;

        else if (esp1.read() == 255 && mant1.read().range(22, 0) != 0)
            NEXT_STATE = ST_SNAN1;

        else if ((esp1.read() == 0 && mant1.read().range(22, 0) == 0) ||
                 (esp2.read() == 0 && mant2.read().range(22, 0) == 0))
            NEXT_STATE = ST_ZERO;

        else if ((esp1.read() == 255 && mant1.read().range(22, 0) == 0) ||
                 (esp2.read() == 255 && mant2.read().range(22, 0) == 0))
            NEXT_STATE = ST_INF;

        else if ((esp1.read() == 0 && mant1.read().range(22, 0) != 0) &&
                 (esp2.read() == 0 && mant2.read().range(22, 0) != 0))
            NEXT_STATE = ST_ADJ3;

        else if (esp2.read() == 0 && mant2.read().range(22, 0) != 0)
            NEXT_STATE = ST_ADJ2;

        else if (esp1.read() == 0 && mant1.read().range(22, 0) != 0)
            NEXT_STATE = ST_ADJ1;

        else
            NEXT_STATE = ST_ELAB;
        break;

    case ST_QNAN:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_SNAN2:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_SNAN1:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_ZERO:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_INF:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_ADJ3:
        NEXT_STATE = ST_ELAB;
        break;

    case ST_ADJ2:
        NEXT_STATE = ST_ELAB;
        break;

    case ST_ADJ1:
        NEXT_STATE = ST_ELAB;
        break;

    case ST_ELAB:
        if (mant_tmp.read()[47] == 1)
            NEXT_STATE = ST_SHIFTR;

        else if (mant_tmp.read()[46] == 1)
            NEXT_STATE = ST_CHECK;

        else if (mant_tmp.read()[46] == 1)
            NEXT_STATE = ST_CHECK;

        else
            NEXT_STATE = ST_SHIFTL;
        break;

    case ST_SHIFTR:
        NEXT_STATE = ST_CHECK;
        break;

    case ST_SHIFTL:
        NEXT_STATE = ST_NORM;
        break;

    case ST_NORM:
        if ((esp_tmp.read()[9] == 1) || (esp_tmp.read() == 0))
            NEXT_STATE = ST_CHECK;

        else if (mant_tmp.read()[46] == 1)
            NEXT_STATE = ST_CHECK;

        else
            NEXT_STATE = ST_SHIFTL;
        break;

    case ST_CHECK:
        if (esp_tmp.read().range(9, 8) == "01")
            NEXT_STATE = ST_INF;

        else if (esp_tmp.read() == 0)
            NEXT_STATE = ST_SUBNORM;

        else if ((esp_tmp.read().range(9, 8) == "00") &&
                 (mant_tmp.read()[22] == 0))
            NEXT_STATE = ST_WRITE;

        else if ((esp_tmp.read().range(9, 8) == "00") &&
                 (mant_tmp.read()[22] == 1))
            NEXT_STATE = ST_ROUND;

        else if (((esp_tmp.read().to_int() + 48) >= 0))
            NEXT_STATE = ST_SHIFTR;

        else
            NEXT_STATE = ST_ZERO;
        break;

    case ST_SUBNORM:
        NEXT_STATE = ST_WRITE;
        break;

    case ST_ROUND:
        if (mant_tmp.read()[47] == 1)
            NEXT_STATE = ST_SHIFTR;
        else
            NEXT_STATE = ST_WRITE;
        break;

    case ST_WRITE:
        NEXT_STATE = ST_FINISH;
        break;

    case ST_FINISH:
        NEXT_STATE = ST_START;
        break;

    default:
        std::cout << "ERRRRRRRRRRRRRRRRRRRRRRRRRRRRR" << std::endl;
        NEXT_STATE = STATE;
        break;
    }
}

void MultiplierModule::datapath() {
    if (rst.read() == 1) {
        STATE = ST_START;
        done.write(sc_logic_0);
        res.write(0);
        esp_tmp.write(0);
        mant_tmp.write(0);

        sign1.write(sc_logic_0);
        esp1.write(0);
        mant1.write(0);

        sign2.write(sc_logic_0);
        esp2.write(0);
        mant2.write(0);

    } else if (clk.read() == 1) {
        STATE = NEXT_STATE;

        switch (NEXT_STATE) {
        case ST_START:
            done.write(sc_logic_0);
            esp_tmp.write(0);
            mant_tmp.write(0);

            sign1.write(sc_logic_0);
            esp1.write(0);
            mant1.write(0);

            sign2.write(sc_logic_0);
            esp2.write(0);
            mant2.write(0);
            break;

        case ST_INIT:
            sign1.write(op1.read()[31]);
            esp1.write((sc_lv<31>(0), op1.read().range(30, 23)));
            mant1.write((sc_logic_1, op1.read().range(22, 0)));

            sign2.write(op2.read()[31]);
            esp2.write((sc_lv<31>(0), op2.read().range(30, 23)));
            mant2.write((sc_logic_1, op2.read().range(22, 0)));
            break;

        case ST_QNAN:
            res.write("11111111110000000000000000000000");
            break;

        case ST_SNAN2:
            res.write((sign2.read(), esp2.read(), sc_logic_1, mant2.read().range(21,0)));
            break;

        case ST_SNAN1:
            res.write((sign1.read(), esp1.read(), sc_logic_1, mant1.read().range(21,0)));
            break;

        case ST_ZERO:            
            res.write(((sign1.read() xor sign2.read()), sc_lv<31>(0)));
            break;

        case ST_INF:
            res.write(((sign1.read() xor sign2.read()), sc_lv<8>(255), sc_lv<23>(0)));
            break;

        case ST_ADJ3:
            mant1.write((sc_logic_0, mant2.read().range(22, 0)));
            esp1.write("00000001");
            mant2.write((sc_logic_0, mant2.read().range(22, 0)));
            esp2.write("00000001");
            break;

        case ST_ADJ2:
            mant2.write((sc_logic_0, mant2.read().range(22, 0)));
            esp2.write("00000001");
            break;

        case ST_ADJ1:
            mant1.write((sc_logic_0, mant2.read().range(22, 0)));
            esp1.write("00000001");
            break;

        case ST_ELAB:
            esp_tmp.write(esp1.read().to_uint() + esp2.read().to_uint() - 127);
            mant_tmp.write(mant1.read().to_uint64() * mant2.read().to_uint64());
            break;

        case ST_SHIFTR:
            mant_tmp.write(mant_tmp.read() >> 1);
            esp_tmp.write(esp_tmp.read().to_int() + 1);
            break;

        case ST_SHIFTL:
            mant_tmp.write(mant_tmp.read() << 1);
            esp_tmp.write(esp_tmp.read().to_int() - 1);
            break;

        case ST_NORM:
            // Do nothing
            break;

        case ST_CHECK:
            // Do nothing
            break;

        case ST_SUBNORM:
            mant_tmp.write((mant_tmp.read() >> 1));
            break;

        case ST_ROUND:
            mant_tmp.write((sc_lv<25>(mant_tmp.read().range(47, 23).to_uint() + 1),
                            mant_tmp.read().range(22, 0)));
            break;

        case ST_WRITE:
            res.write(((sign1.read() xor sign2.read()), esp_tmp.read().range(7, 0),
                       mant_tmp.read().range(45, 23)));
            break;

        case ST_FINISH:
            done.write(sc_logic_1);
            break;

        default:
            // Do nothing
            break;
        }
    }
}