`timescale 1ns / 1ps

module verilog_multiplier(clk, rst, ready, op1, op2, res, done);
    // Interface
    input wire clk, rst;
    input wire ready;
    input wire [31:0] op1, op2;
    output reg [31:0] res;
    output reg done;
    
    // states
    parameter ST_START=0, ST_INIT=1, ST_SNAN1=2, ST_SNAN2=3, ST_QNAN=4, ST_ZERO=5, ST_INF=6, ST_ADJ3=7, ST_ADJ2=8, ST_ADJ1=9, ST_ELAB=10, ST_SHIFTR=11, ST_SHIFTL=12, ST_NORM=13, ST_CHECK=14, ST_SUBNORM=15, ST_ROUND=16, ST_WRITE=17, ST_FINISH=18;
    reg[4:0] STATE, NEXT_STATE;
    // op1 and op2 informations
    reg sign1, sign2;
    reg[7:0] esp1, esp2;
    reg[23:0] mant1, mant2;
    // res intermediate parts
    reg[9:0] esp_tmp;
    reg[47:0] mant_tmp;


// FSM
always @(STATE, ready, esp1, mant1, esp2, mant2, esp_tmp, mant_tmp)
begin
    case (STATE)        
        ST_START: begin
            if (ready == 1'b1)
                NEXT_STATE <= ST_INIT;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_INIT: begin        
            if (((esp1 == 8'd0 && mant1[22:0] == 23'd0) && (esp2 == 8'd255 && mant2[22:0] == 23'd0)) || 
               ((esp1 == 8'd255 && mant1[22:0] == 23'd0) && (esp2 == 8'd0 && mant2[22:0] == 23'd0)))
                NEXT_STATE <= ST_QNAN;
                
            else if (esp2 == 8'd255 && mant2[22:0] != 23'd0)
                NEXT_STATE <= ST_SNAN2;
                
            else if (esp1 == 8'd255 && mant1[22:0] != 23'd0)
                NEXT_STATE <= ST_SNAN1;
                
            else if ((esp1 == 8'd0 && mant1[22:0] == 23'd0) || (esp2 == 8'd0 && mant2[22:0] == 23'd0))
                NEXT_STATE <= ST_ZERO;
                
            else if ((esp1 == 8'd255 && mant1[22:0] == 23'd0) || (esp2 == 8'd255 && mant2[22:0] == 23'd0))
                NEXT_STATE <= ST_INF;
                
            else if ((esp1 == 8'd0 && mant1[22:0] != 23'd0) && (esp2 == 8'd0 && mant2[22:0] != 23'd0))
                NEXT_STATE <= ST_ADJ3;
                
            else if (esp2 == 8'd0 && mant2[22:0] != 23'd0)
                NEXT_STATE <= ST_ADJ2;
                
            else if (esp1 == 8'd0 && mant1[22:0] != 23'd0)
                NEXT_STATE <= ST_ADJ1;
            
            else
                NEXT_STATE <= ST_ELAB;            
        end
        
        ST_QNAN: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_SNAN2: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_SNAN1: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_ZERO: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_INF: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_ADJ3: begin
            NEXT_STATE <= ST_ELAB;
        end
        
        ST_ADJ2: begin
            NEXT_STATE <= ST_ELAB;
        end
        
        ST_ADJ1: begin
            NEXT_STATE <= ST_ELAB;
        end
        
        ST_ELAB: begin
            if (mant_tmp[47] == 1'b1)                               // mant_tmp == 1x.x..
                NEXT_STATE <= ST_SHIFTR;
                
            else if (mant_tmp[46] == 1'b1)                          // mant_tmp == 01.x..
                NEXT_STATE <= ST_CHECK;
                
            else if (mant_tmp[46] == 1'b1)                          // mant_tmp == 01.x..
                NEXT_STATE <= ST_CHECK;
                        
            else                                                    // mant_tmp == 00.x.. && mant_tmp != 0
                NEXT_STATE <= ST_SHIFTL;                            // num or subnorm or underflow
        end
        
        ST_SHIFTR: begin
            NEXT_STATE <= ST_CHECK;
        end
        
        ST_SHIFTL: begin
            NEXT_STATE <= ST_NORM;
        end
        
        ST_NORM: begin                
            if ((esp_tmp[9] == 10'd1) || (esp_tmp == 10'd0))        // esp_tmp <= 0
                NEXT_STATE <= ST_CHECK;                             // case underflow or subnorm
                        
            else if (mant_tmp[46] == 10'd1)
                NEXT_STATE <= ST_CHECK;                             // case num
                
            else
                NEXT_STATE <= ST_SHIFTL;                            // norm again
        end
        
        ST_CHECK: begin           
            if (esp_tmp[9:8] == 2'b01)                                              // esp_tmp == 01..
                NEXT_STATE <= ST_INF;                                               // overflow
                
            else if (esp_tmp == 10'd0)                                              // esp_tmp == 0  
                NEXT_STATE <= ST_SUBNORM;                                           // subnorm
                
            else if ((esp_tmp[9:8] == 2'b00) && (mant_tmp[22] == 1'b0))             // esp_tmp == 00..  && esp_tmp != 0
                NEXT_STATE <= ST_WRITE;                                             // ok
                
            else if ((esp_tmp[9:8] == 2'b00) && (mant_tmp[22] == 1'b1))             // esp_tmp == 00..  && esp_tmp != 0
                NEXT_STATE <= ST_ROUND;                                             // need to round   
                
            else if (((esp_tmp + 10'd48) < 10'b1000000000))                         // esp_tmp == 1x.. && esp_tmp+48 >= 0
                NEXT_STATE <= ST_SHIFTR;                                            // create subnorm if possible
        
            else                                                                    // esp_tmp == 1x.. && esp_tmp+48 < 0
                NEXT_STATE <= ST_ZERO;                                              // underflow
        end
        
        ST_SUBNORM: begin
            NEXT_STATE <= ST_WRITE;
        end
        
        ST_ROUND: begin
            if (mant_tmp[47] == 1)
                NEXT_STATE <= ST_SHIFTR;     // need to norm again
            else
                NEXT_STATE <= ST_WRITE;
        end
        
        ST_WRITE: begin
            NEXT_STATE <= ST_FINISH;
        end
        
        ST_FINISH: begin
            NEXT_STATE <= ST_START;
        end
        
        // Catch state
        default : begin
            NEXT_STATE <= STATE;
        end
    endcase
end


// DATA PATH
always @(posedge clk, posedge rst)
begin
    if (rst == 1'b1) begin                  // Reset all
        STATE <= ST_START;
        done <= 1'b0;
        res <= 32'd0;                
        mant_tmp <= 48'd0;
        esp_tmp <= 24'd0;                  
        sign1 <= 1'b0;             
        esp1 <= 10'd0;
        mant1 <= 24'd0;        
        sign2 <= 1'b0;             
        esp2 <= 10'd0;
        mant2 <= 24'd0;
end 
else begin
        STATE <= NEXT_STATE;                // Update STATE
        case (NEXT_STATE)
            // Reset register 
            ST_START: begin 
                done <= 1'b0;
                
                mant_tmp <= 48'd0;
                esp_tmp <= 24'd0;
                          
                sign1 <= 1'b0;             
                esp1 <= 10'd0;
                mant1 <= 24'd0;
                
                sign2 <= 1'b0;             
                esp2 <= 10'd0;
                mant2 <= 24'd0;
            end       
            
            // Get info
            ST_INIT: begin
                // Get informations of op1
                sign1 <= op1[31];             
                esp1 <= op1[30:23];
                mant1 <= {1'b1, op1[22:0]};
                // Get informations of op2
                sign2 <= op2[31];             
                esp2 <= op2[30:23];
                mant2 <= {1'b1, op2[22:0]};
            end
            
            // Quiet NAN from 0*inf
            ST_QNAN: begin
                res[31] <= 1'b1;
                res[30:23] <= 8'd255;
                res[22] <= 1'b1;
                res[21:0] <= 22'd0;
            end
            
            // Signal NAN from op2
            ST_SNAN2: begin
                res[31] <= sign2;
                res[30:23] <= esp2;
                res[22:0] <= mant2;
            end
            
            // Signal NAN from op1
            ST_SNAN1: begin
                res[31] <= sign1;
                res[30:23] <= esp1;
                res[22:0] <= mant1;
            end
            
            // ZERO
            ST_ZERO: begin
                res[31] = sign1 ^ sign2;
                res[30:23] = 8'd0;
                res[22:0] = 31'd0;
            end
            
            // INF
            ST_INF: begin
                res[31] = sign1 ^ sign2;
                res[30:23] = 8'd255;
                res[22:0] = 31'd0;
            end
            
            // both input are SUBNORM
            ST_ADJ3: begin
                mant1[23] = 1'b0;
                mant2[23] = 1'b0;
                esp1 = 10'd1;   //(-bias + 1)+127  because: from (-bias+1) to (esp-bias)
                esp2 = 10'd1;   //(-bias + 1)+127  because: from (-bias+1) to (esp-bias)
            end
            
            // op2 is SUBNORM
            ST_ADJ2: begin
                mant2[23] = 1'b0;
                esp2 = 10'd1;   //(-bias + 1)+127  because: from (-bias+1) to (esp-bias)
            end
            
            // op1 is SUBNORM
            ST_ADJ1: begin
                mant1[23] = 1'b0;
                esp1 = 10'd1;   //(-bias + 1)+127
            end
            
            // Process esp and mant parts
            ST_ELAB: begin
                esp_tmp <= esp1 + esp2 - 10'd127;
                mant_tmp <= mant1 * mant2;
            end
            
            // Norm (right) from 1x. 
            ST_SHIFTR: begin
                mant_tmp <= mant_tmp >> 1'b1;
                esp_tmp <= esp_tmp + 10'd1;
            end
            
            // Norm (left) from 00.
            ST_SHIFTL: begin
                mant_tmp <= mant_tmp << 1'b1;
                esp_tmp <= esp_tmp - 10'd1;
            end
            
            // Check if norm
            ST_NORM: begin
                // Do nothing
            end
            
            // Check overflow/undeflow
            ST_CHECK: begin
                // Do nothing
            end
            
            // Subnorm
            ST_SUBNORM: begin
                mant_tmp <= mant_tmp >> 10'd1; //from (esp-bias) to (-bias+1) implicit
            end
            
            // Rounding
            ST_ROUND: begin
                mant_tmp[47:23] <= mant_tmp[47:23] + 24'd1;          
            end
            
            // Write res
            ST_WRITE: begin
                res[31] = sign1 ^ sign2;
                res[30:23] = esp_tmp[7:0];
                res[22:0] = mant_tmp[45:23];
            end
            
            // Set done
            ST_FINISH: begin
                done = 1'b1;
            end                
            
            default : begin
                // Do nothing
            end
        endcase
    end    
end

endmodule