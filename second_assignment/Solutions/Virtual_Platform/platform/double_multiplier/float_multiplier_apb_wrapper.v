`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company: UniVr
// Engineer: Sgarbanti Enrico
//////////////////////////////////////////////////////////////////////////////////


module float_multiplier_apb_wrapper(pclk, presetn, paddr, psel, penable, pwrite, pwdata, pready, prdata);
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
    parameter ST_WAIT1=0, ST_READ1=1, ST_WAIT2=2, ST_ELAB=3, ST_RET=4;
    reg[2:0] STATE;
    reg[2:0] NEXT_STATE = ST_WAIT1;
    
    // Signals
    reg [31:0] tmp;
    reg ready;
    wire done;
    wire [31:0] res;

    verilog_multiplier m(
        .clk(pclk),
        .rst(presetn),
        .ready(ready),
        .op1(tmp),
        .op2(pwdata),
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
                NEXT_STATE <= ST_ELAB;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_ELAB: begin
            if (done == 1'b1)
                NEXT_STATE <= ST_RET;
            else
                NEXT_STATE <= STATE;
        end
        
        ST_RET: begin
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
        tmp <= 32'd0;
        ready <= 1'b0;
        pready <= 1'b0;
        prdata <= 32'd0;
    end 
    else begin
        STATE <= NEXT_STATE;
        case (NEXT_STATE)
            ST_WAIT1: begin
                tmp <= 32'd0;
                ready <= 1'b0;
                pready <= 1'b0;
                prdata <= 32'd0;
            end
            
            ST_READ1: begin
                tmp <= pwdata;
            end
            
            ST_WAIT2: begin
                // Do nothing
            end
            
            ST_ELAB: begin
                ready <= 1'b1;
            end
            
            ST_RET: begin
                pready <= 1'b1;
                prdata <= res;
                ready <= 1'b0;
            end
            
            // Other cases
            default : begin
                // Do nothing
            end
        endcase
    end    
end

   
endmodule