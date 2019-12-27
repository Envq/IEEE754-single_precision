`timescale 1ns / 1ps

module double_multipler(clk, rst, ready, op1, op2, res, done);
    // Interface
    input clk, rst;
    input ready;
    input[31:0] op1, op2;
    output reg[31:0] res;
    output reg done;    
    
    // Statements
    parameter ST_START=0, ST_READ1=1, ST_READ2=2, ST_WAIT=3, ST_WAIT1=4, ST_WAIT2=5, ST_RET1=6, ST_RET2=7;
    reg[2:0] STATE, NEXT_STATE;
    reg ready1, ready2;
    reg[31:0] op1_tmp, op2_tmp;
    wire[31:0] res1, res2;
    wire done1, done2;    
    
    
vhdl_multiplier mul1 (
    .clk (clk),
    .rst (rst),
    .ready (ready1),
    .op1 (op1_tmp),
    .op2 (op2_tmp),
    .res (res1),
    .done (done1)
);

vhdl_multiplier mul2 (
    .clk (clk),
    .rst (rst),
    .ready (ready2),
    .op1 (op1_tmp),
    .op2 (op2_tmp),
    .res (res2),
    .done (done2)
);


// FSM
always @(STATE, ready, done1, done2)
begin
    case (STATE)        
        ST_START: begin
            if (ready == 1'b1)
                NEXT_STATE <= ST_READ1;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_READ1: begin
            NEXT_STATE <= ST_READ2;
        end
        
        ST_READ2: begin
            NEXT_STATE <= ST_WAIT;
        end
        
        ST_WAIT: begin
            if (done1 == 1'b1)
                if (done2 == 1'b1)           // done1 &&  done2
                    NEXT_STATE <= ST_RET1;
                else
                    NEXT_STATE <= ST_WAIT2;  // done1 && !done2
            else if (done2 == 1'b1)
                NEXT_STATE <= ST_WAIT1;      //!done1 &&  done2
            else
                NEXT_STATE <= STATE;         //!done1 && !done2
        end
        
        ST_WAIT1: begin
            if (done1 == 1'b1)
                NEXT_STATE <= ST_RET1;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_WAIT2: begin
            if (done2 == 1'b1)
                NEXT_STATE <= ST_RET1;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_RET1: begin
            NEXT_STATE <= ST_RET2;
        end
        
        ST_RET2: begin
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
        STATE <= ST_START;                  //Reset state
    end 
    else begin
        STATE <= NEXT_STATE;                //Update STATE
        case (STATE)
            // Reset register
            ST_START: begin
                done <= 1'b0;
                ready2 <= 1'b0; 
                ready1 <= 1'b0;
            end
            
            // Start first multiplier
            ST_READ1: begin      
                op1_tmp <= op1;
                op2_tmp <= op2;  
                ready1 <= 1'b1;
            end
            
            // Start second multiplier
            ST_READ2: begin   
                ready1 <= 1'b0;             //stop mul1
                op1_tmp <= op1;
                op2_tmp <= op2; 
                ready2 <= 1'b1;
            end
            
            // Wait the end of mults
            ST_WAIT: begin
                ready2 <= 1'b0;             //stop mul2
            end
            
            // Raise the flag of done and return res1
            ST_RET1: begin
                done <= 1'b1;
                res <= res1;
            end
            
            // Return res2
            ST_RET2: begin
                res <= res2;
            end
            
            // Do nothing
            default : begin
            end
        endcase
    end    
end


endmodule