module verilog_multiplier(clk, rst, ready, op1, op2, res, done);
    // Init port
    input clk, rst;
    input ready;
    input[31:0] op1, op2;
    output reg [31:0] res;
    output reg done;    
    
    // states
    parameter ST_START=0, ST_EVAL1=1, ST_EVAL2=2, ST_CHECK=3, ST_ELAB=4, ST_NORM1=5, ST_ROUND=6, ST_NORM2=7, ST_FINISH=8;
    reg[3:0] STATE, NEXT_STATE;
    // op1 and op2 informations
    reg sign1, sign2;
    reg[7:0] esp1, esp2;
    reg[23:0] mant1, mant2;
    // res intermediate parts
    reg[9:0] esp_tmp;
    reg[47:0] mant_tmp;
    reg special, norm_again;
    //parameter T_ZER = 2'b00, T_INF = 2'b01, T_NAN = 2'b10, T_NUM = 2'b11;
    parameter T_ZER=0, T_INF=1, T_NAN=2, T_NUM=3;
    reg[3:0] op1_type, op2_type;
    parameter   U_ZER = 31'b0000000000000000000000000000000,
                U_INF = 31'b1111111100000000000000000000000,
                U_NAN = 31'b1111111111111111111111111111111;


// FSM
always @(STATE, ready, special)
begin
    case (STATE)        
        ST_START: begin
            if (ready == 1'b1)
                NEXT_STATE <= ST_EVAL1;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_EVAL1: begin
            NEXT_STATE <= ST_EVAL2;
        end
        
        ST_EVAL2: begin
            NEXT_STATE <= ST_CHECK;
        end
        
        ST_CHECK: begin
            if (special == 1'b1)
                NEXT_STATE <= ST_FINISH;
            else
                NEXT_STATE <= ST_ELAB;
        end
        
        ST_ELAB: begin
            if (special == 1'b1)
                NEXT_STATE <= ST_FINISH;
            else
                NEXT_STATE <= ST_NORM1;
        end
        
        ST_NORM1: begin
            NEXT_STATE <= ST_ROUND;
        end
        
        ST_ROUND: begin
            if (norm_again == 1'b1)
                NEXT_STATE <= ST_NORM2;
            else
                NEXT_STATE <= ST_FINISH;
        end
        
        ST_NORM2: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_FINISH: begin
            NEXT_STATE <= ST_START;
        end
        
        // Catch wrong state
        default : begin
            NEXT_STATE <= STATE;
        end
    endcase
end


// DATA PATH
always @(posedge clk, posedge rst)
begin
    if (rst == 1'b1) begin
        STATE <= ST_START;                   //Reset state
    end 
    else begin
        STATE <= NEXT_STATE;                 //Update STATE
        case (STATE)
            // Reset register
            ST_START: begin
                // Reset signals
                done <= 1'b0;                
                // Reset internal comunication signals
                special <= 1'b0;             
                norm_again <= 1'b0;
                // Get informations of op1
                sign1 <= op1[31];             
                esp1 <= op1[30:23];
                mant1[23] <= 1'b1;
                mant1[22:0] = op1[22:0];
                // Get informations of op2
                sign2 <= op2[31];             
                esp2 <= op2[30:23];
                mant2[23] <= 1'b1;
                mant2[22:0] <= op2[22:0];
            end
            
            // Special case op1 check
            ST_EVAL1: begin
                if (esp1 == 8'd255)
                    if (mant1 == 0)
                        op1_type <= T_INF;
                    else
                        op1_type <= T_NAN;
                else
                    if (esp1 == 8'd0 && mant1 == 23'd0)
                        op1_type <= T_ZER;
                    else
                        op1_type <= T_NUM;
            end
            
            // Special case op2 check
            ST_EVAL2: begin
                if (esp2 == 8'd255)
                    if (mant2 == 0)
                        op2_type <= T_INF;
                    else
                        op2_type <= T_NAN;
                else
                    if (esp2 == 8'd0 && mant2 == 23'd0)
                        op2_type <= T_ZER;
                    else
                        op2_type <= T_NUM;
            end
         
            // Check special case for res
            ST_CHECK : begin
                if (op1_type == T_NAN || op2_type == T_NAN ||
                    (op1_type == T_ZER && op2_type == T_INF) ||
                    (op2_type == T_ZER && op1_type == T_INF)) begin
                        res[30:0] <= U_NAN;
                        special <= 1'b1;
                end
                else
                    if (op1_type == T_ZER || op2_type == T_ZER) begin
                            res[30:0] = U_ZER;
                            special <= 1'b1;
                    end
                    else
                        if (op1_type == T_INF || op2_type == T_INF) begin
                                res[30:0] = U_INF;
                                special <= 1'b1;
                        end
            end
            
            // process esp and mant
            ST_ELAB : begin
                mant_tmp = mant1 * mant2;
                esp_tmp = esp1 + esp2 - 10'd126;        // 127 - 1 for norm
                if (esp_tmp[9] == 1'b1) begin           // undeflow check
                    res[30:0] <= U_ZER;
                    special <= 1'b1;
                end
                else
                    if (esp_tmp[8] == 1'b1) begin       // overflow check
                        res[30:0] <= U_INF;
                        special <= 1'b1;
                    end
                    else
                        special <= 1'b0;                 // is a number
            end
            
            //Normalize result
            ST_NORM1 : begin
                res[30:23] <= esp_tmp[7:0];             // store esp
                if (mant_tmp[47] == 1'b0) begin
                    mant_tmp <= mant_tmp << 1'b1;       // norm mant
                end 
            end
            
            //Round result
            ST_ROUND : begin
                res[22:0] <= mant_tmp[46:24];         // store mant
                if (mant_tmp[23] == 1'b1 || (mant_tmp[22:0] == 23'b01111111111111111111111))
                    norm_again <= 1'b1;
                else
                    norm_again <= 1'b0;
            end
            
            //Normalize result after rounding
            ST_NORM2 : begin
                if (res[22:0] == 23'b11111111111111111111111) begin
                    if (res[31:23] == 8'b11111111) begin       // overflow check
                        res[30:0] <= U_INF;
                        special <= 1'b1;
                    end
                    else begin
                        res[22:0] <= 23'b00000000000000000000000;
                        res[31:23] <= res[31:23] + 1;
                    end
                end
                else
                    res[22:0] = res[22:0] + 23'd1;
            end
            
            ST_FINISH : begin
                res[31] = sign1 ^ sign2;
                done <= 1'b1;
            end
                
            // Do nothing
            default : begin
            end
        endcase
    end    
end


endmodule