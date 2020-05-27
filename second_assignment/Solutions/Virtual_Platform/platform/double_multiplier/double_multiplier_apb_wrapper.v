`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company: UniVr
// Engineer: Sgarbanti Enrico
//////////////////////////////////////////////////////////////////////////////////


module double_multiplier_apb_wrapper(pclk, presetn, paddr, psel, penable, pwrite, pwdata, pready, prdata);
    // Interface   
    input wire pclk;
    input wire presetn;
    input wire[31:0] paddr;
    input wire psel;
    input wire penable;
    input wire pwrite;
    input wire[31:0] pwdata;
    output reg pready;
    output reg[31:0] prdata;
    
    // states
    parameter ST_WAIT1=0, ST_READ1=1, ST_WAIT2=2, ST_READ2=3, ST_WAIT3=4, ST_READ3=5, ST_WAIT4=6, ST_READ4=7, ST_ELAB1=8, ST_ELAB2=9, ST_RET0=10, ST_RET1=11, ST_ACK=12, ST_RET2=13;
    reg[3:0] STATE;
    reg[3:0] NEXT_STATE = ST_WAIT1;
    
    // Signals
    reg [31:0] op1, op2;
    reg [31:0] op1_tmp, op2_tmp, op3_tmp, op4_tmp, res_tmp;
    reg ready;
    wire done;
    wire [31:0] res;
    
    double_multiplier dm(
        .clk(pclk),
        .rst(presetn),
        .ready(ready),
        .op1(op1),
        .op2(op2),
        .done(done),
        .res(res)
    );
    
    
// FSM
always @(STATE, penable, done)
begin
    case (STATE)        
        ST_WAIT1: begin
            if (penable == 1'b1)
                NEXT_STATE <= ST_READ1;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_READ1: begin
            if (penable == 1'b0)
                NEXT_STATE <= ST_WAIT2;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_WAIT2: begin
            if (penable == 1'b1)
                NEXT_STATE <= ST_READ2;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_READ2: begin
            if (penable == 1'b0)
                NEXT_STATE <= ST_WAIT3;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_WAIT3: begin
            if (penable == 1'b1)
                NEXT_STATE <= ST_READ3;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_READ3: begin
            if (penable == 1'b0)
                NEXT_STATE <= ST_WAIT4;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_WAIT4: begin
            if (penable == 1'b1)
                NEXT_STATE <= ST_READ4;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_READ4: begin
            NEXT_STATE <= ST_ELAB1;
        end
        
        ST_ELAB1: begin
            NEXT_STATE <= ST_ELAB2;
        end
        
        ST_ELAB2: begin
            if (done == 1'b1)
                NEXT_STATE <= ST_RET0;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_RET0: begin
            NEXT_STATE <= ST_RET1;
        end
        
        ST_RET1: begin
            if (penable == 1'b0)
                NEXT_STATE <= ST_ACK;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_ACK: begin
            if (penable == 1'b1)
                NEXT_STATE <= ST_RET2;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_RET2: begin
            if (penable == 1'b0)
                NEXT_STATE <= ST_WAIT1;
            else
                NEXT_STATE <= STATE;
        end
        
        // Catch wrong state
        default : begin
            NEXT_STATE <= STATE;
        end
    endcase
end


// DATA PATH
always @(posedge pclk, posedge presetn)
begin
    if (presetn == 1'b1) begin
        STATE <= ST_WAIT1;
        op1 <= 32'd0;
        op2 <= 32'd0;
        op1_tmp <= 32'd0;
        op2_tmp <= 32'd0;
        op3_tmp <= 32'd0;
        op4_tmp <= 32'd0;
        ready <= 1'b0;
        pready <= 1'b0;
        prdata <= 32'd0;
    end 
    else begin
        STATE <= NEXT_STATE;
        case (NEXT_STATE)
            ST_WAIT1: begin
                op1 <= 32'd0;
                op2 <= 32'd0;
                op1_tmp <= 32'd0;
                op2_tmp <= 32'd0;
                op3_tmp <= 32'd0;
                op4_tmp <= 32'd0;
                ready <= 1'b0;
                pready <= 1'b0;
                prdata <= 32'd0;
            end
            
            ST_READ1: begin
                op1_tmp <= pwdata;
            end
            
            ST_WAIT2: begin
                // Do nothing
            end
            
            ST_READ2: begin
                op2_tmp <= pwdata;
            end
            
            ST_WAIT3: begin
                // Do nothing
            end
            
            ST_READ3: begin
                op3_tmp <= pwdata;
            end
            
            ST_WAIT4: begin
                // Do nothing
            end
            
            ST_READ4: begin
                op4_tmp <= pwdata;
                ready <= 1'b1;
            end
            
            ST_ELAB1: begin
                op1 <= op1_tmp;
                op2 <= op2_tmp;
            end
            
            ST_ELAB2: begin
                op1 <= op3_tmp;
                op2 <= op4_tmp;
            end
            
            ST_RET0: begin
                ready <= 1'b0;
                pready <= 1'b1;
                prdata <= res;
            end
            
            ST_RET1: begin
                res_tmp <= res;
            end
            
            ST_ACK: begin
                pready <= 1'b0;
            end
            
            ST_RET2: begin
                pready <= 1'b1;
                prdata <= res_tmp;
            end
            
            // Other cases
            default : begin
                // Do nothing
            end
        endcase
    end    
end

   
endmodule