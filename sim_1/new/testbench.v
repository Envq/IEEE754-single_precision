`timescale 1ns / 1ps

module testbench();
    parameter TIME = 50;
    reg clk, rst;
    reg ready;
    reg[31:0] op1, op2;
    wire[31:0] res;
    wire done;


double_multipler top_level(
    .clk (clk),
    .rst (rst),
    .ready (ready),
    .op1 (op1),
    .op2 (op2),
    .res (res),
    .done (done)
);


always #TIME clk  <= !clk; // Set clock


initial 
begin
    // Init
    clk <= 1'b0;
    ready <= 1'b0;
    
    // Reset
    rst <= #50 1'b1;
    rst <= #90 1'b0;
    
    // Ready
    ready <= #100 1'b1;
    ready <= #190 1'b0;
    
    // Mul1  
    op1 <= #200 32'b01000000000000000000000000000000; //2.0
    op2 <= #200 32'b01000000001000000000000000000000; //2.5
    
    // Mul2
    op1 <= #300 32'b00111111101000000000000000000000; //1.25
    op2 <= #300 32'b00111111100000000000000000000000; //1.0
    #3500
    
    // Mul1  
    op1 <= #200 32'b01000010110010000110011001100110; //100.2
    op2 <= #200 32'b01000010100000001000101000111101; //64.27
    
    // Mul2
    op1 <= #300 32'b01000101000110000101101101110101; //2437.716
    op2 <= #300 32'b01000100001101011010000000000000; //726.5
    #1500;
    
end


endmodule