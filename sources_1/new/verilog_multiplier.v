`timescale 1ns / 1ps

module verilog_multiplier(clk, rst, ready, op1, op2, res, done);
    // Interface
    input clk, rst;
    input ready;
    input[31:0] op1, op2;
    output reg [31:0] res;
    output reg done;    
    
    // states
    parameter ST_START=0, ST_EVAL1=1, ST_EVAL2=2, ST_EVAL3=3, ST_CHECK1=4, ST_ELAB=5, ST_UNDERF=6, ST_CHECK2=7, ST_NORM1=8, ST_ROUND=9, ST_CHECK3=10, ST_NORM2=11, ST_OVERF=12, ST_FINISH=13;
    reg[3:0] STATE, NEXT_STATE;
    // types num
    parameter T_NUM=0, T_NAN=1, T_ZER=2, T_INF=3; 
    reg[2:0] op1_type, op2_type, res_type;
    // op1 and op2 informations
    reg sign1, sign2;
    reg[9:0] esp1, esp2;
    reg[23:0] mant1, mant2;
    // res intermediate parts
    reg[9:0] esp_tmp;
    reg[47:0] mant_tmp;
    reg norm_again;
    


// FSM
always @(STATE, ready, res_type, norm_again)
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
            NEXT_STATE <= ST_EVAL3;
        end
        
        ST_EVAL3: begin
            NEXT_STATE <= ST_CHECK1;
        end
        
        ST_CHECK1: begin
            if (res_type == T_NUM)
                NEXT_STATE <= ST_ELAB;
            else
                NEXT_STATE <= ST_FINISH;
        end
        
        ST_ELAB: begin
            NEXT_STATE <= ST_UNDERF;
        end
        
        ST_UNDERF: begin
            NEXT_STATE <= ST_CHECK2;
        end
        
        ST_CHECK2: begin
            if (res_type == T_NUM)
                NEXT_STATE <= ST_NORM1;
            else
                NEXT_STATE <= ST_FINISH;
        end
        
        ST_NORM1: begin
            NEXT_STATE <= ST_ROUND;
        end
        
        ST_ROUND: begin
            NEXT_STATE <= ST_CHECK3;
        end
        
        ST_CHECK3: begin
            if (norm_again == 1'b1)
                NEXT_STATE <= ST_NORM2;
            else
                NEXT_STATE <= ST_OVERF;
        end
        
        ST_NORM2: begin
            NEXT_STATE <= ST_OVERF;
        end
        
        ST_OVERF: begin
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
                norm_again <= 1'b0;
                // Get informations of op1
                sign1 <= op1[31];             
                esp1 <= {2'b00, op1[30:23]};
                mant1[23:0] <= {1'b1, op1[22:0]};
                // Get informations of op2
                sign2 <= op2[31];             
                esp2 <= {2'b00, op2[30:23]};
                mant2[23:0] <= {1'b1, op2[22:0]};
            end
            
            // Special case op1 check
            ST_EVAL1: begin
                if (esp1[7:0] == 8'b11111111)
                    if (mant1[22:0] == 23'b00000000000000000000000)
                        op1_type <= T_INF;
                    else
                        op1_type <= T_NAN;
                else
                    if (esp1[7:0] == 8'b00000000 && (mant1[22:0] == 23'b00000000000000000000000))
                        op1_type <= T_ZER;
                    else
                        op1_type <= T_NUM;
            end
            
            // Special case op2 check
            ST_EVAL2: begin            
                if (esp2[7:0] == 8'b11111111)
                    if (mant2[22:0] == 23'b00000000000000000000000)
                        op2_type <= T_INF;
                    else
                        op2_type <= T_NAN;
                else
                    if (esp2[7:0] == 8'b00000000 && (mant2[22:0] == 23'b00000000000000000000000))
                        op2_type <= T_ZER;
                    else
                        op2_type <= T_NUM;
            end
         
            // Special case for res
            ST_EVAL3: begin
                if (op1_type == T_NAN || op2_type == T_NAN ||
                    (op1_type == T_ZER && op2_type == T_INF) ||
                    (op2_type == T_ZER && op1_type == T_INF))
                        res_type <= T_NAN;
                else
                    if (op1_type == T_ZER || op2_type == T_ZER)
                        res_type <= T_ZER;
                    else
                        if (op1_type == T_INF || op2_type == T_INF)
                            res_type <= T_INF;
                        else
                            res_type <= T_NUM;
            end
            
            // Next status check
            ST_CHECK1: begin
                // Do nothing
            end
            
            // Process esp and mant
            ST_ELAB: begin
                esp_tmp <= esp1 + esp2 - 10'd127;
                mant_tmp <= mant1 * mant2;
            end
            
            // Underflow check
            ST_UNDERF: begin
                if (esp_tmp[9] == 1'b1)                 //undeflow check
                    res_type <= T_ZER;
                else
                    res_type <= T_NUM;
            end
            
            // Next status check
            ST_CHECK2: begin            
                // Do nothing
            end
            
            // Normalize result
            ST_NORM1: begin
                if (mant_tmp[47] == 1'b1) 
                    esp_tmp <= esp_tmp + 10'd1;          //increment esp
                else begin
                    mant_tmp <= mant_tmp << 1'b1;        //norm mant_tmp
                end 
            end
            
            // Round result
            ST_ROUND: begin
                if (mant_tmp[23] == 1'b1 || (mant_tmp[22:0] == 23'b01111111111111111111111))
                    norm_again <= 1'b1;
                else
                    norm_again <= 1'b0;
            end
            
            // Next status check
            ST_CHECK3: begin
                // Do nothing
            end
            
            // Normalize result after rounding
            ST_NORM2: begin
                if (mant_tmp[46:24] == 23'b11111111111111111111111) begin
                    esp_tmp <= esp_tmp + 10'd1;                 //increment esp
                end
                mant_tmp[46:24] <= mant_tmp[46:24] + 1'b1;      //icrement mant
            end
            
            // Overflow check and store 
            ST_OVERF: begin
                if (esp_tmp[8] == 1'b1)                 //overflow check
                    res_type <= T_INF;
                else begin
                    res_type <= T_NUM;
                end
            end
            
            // Finish
            ST_FINISH: begin
                case (res_type)
                    T_ZER: begin
                        res[30:0] <= 31'b0000000000000000000000000000000;
                    end
                    
                    T_INF: begin
                        res[30:0] <= 31'b1111111100000000000000000000000;
                    end
                    
                    T_NAN: begin
                        res[30:0] <= 31'b1111111111111111111111111111111;
                    end
                    
                    T_NUM: begin                    
                        res[30:23] <= esp_tmp[7:0];         //store esp
                        res[22:0] <= mant_tmp[46:24];       //store mant
                    end
                    
                    default: begin
                        // Do nothing
                    end
                endcase
                
                res[31] <= sign1 ^ sign2;    //get sign
                done <= 1'b1;
            end
                
            
            default : begin
                // Do nothing
            end
        endcase
    end    
end
    

endmodule
