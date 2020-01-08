`timescale 1ns / 1ps

module testbench();
    reg clk, rst;
    reg ready;
    reg[31:0] op1, op2;
    wire[31:0] res;
    wire done;
    parameter PERIOD = 20;


double_multipler top_level(
    .clk (clk),
    .rst (rst),
    .ready (ready),
    .op1 (op1),
    .op2 (op2),
    .res (res),
    .done (done)
);


always #(PERIOD/2) clk  <= !clk; // Set clock


initial 
begin
    // Init
    clk <= 1'b1;
    ready <= 1'b0;
    rst <= 1'b0;
    
    // Wait for startup FPGA
    #(PERIOD*10);
    
    // Reset
    rst <= 1'b1;
    #PERIOD;
    rst <= 1'b0;
    #PERIOD;
    
    // Exec1
    ready <= 1'b1;
    op1 <= 32'b01000000000000000000000000000000; //2.0
    op2 <= 32'b01000000001000000000000000000000; //2.5
    #PERIOD;
    op1 <= 32'b00111111101000000000000000000000; //1.25
    op2 <= 32'b00111111100000000000000000000000; //1.0
    ready <= 1'b0;
    #(PERIOD*18);
    
    // Exec2
    ready <= 1'b1;
    op1 <= 32'b01000010110010000110011001100110; //100.2
    op2 <= 32'b10000000000000000000000000000000; //-0.0
    #PERIOD;
    op1 <= 32'b11111111100000000000000000000000; //-inf;
    op2 <= 32'b01000101000110000101101101110101; //2437.716
    ready <= 1'b0;
    #(PERIOD*18);
    
end


endmodule
