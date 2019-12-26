`timescale 1ns / 1ps

module testbench();
    parameter TIME = 50;
    reg clk, rst;
    reg ready;
    reg[31:0] op1, op2;
    wire[31:0] res1, res2;
    wire done;


double_multipler top_level(
    .clk (clk),
    .rst (rst),
    .ready (ready),
    .op1 (op1),
    .op2 (op2),
    .res1 (res1),
    .res2 (res2),
    .done (done)
);


always #TIME clk  <= !clk; // Set clock


initial 
begin
    clk <= 1'b0;
    ready <= 1'b0;
    
    rst <= #50 1'b1;
    
    rst <= #100 1'b0;
    
    ready <= #200 1'b1;  
    op1 <= #200 32'b01000000000000000000000000000000; //2.0
    op2 <= #200 32'b01000000001000000000000000000000; //2.5
    
    ready <= #300 1'b0;
    op1 <= #400 32'b00111111101000000000000000000000; //1.25
    op2 <= #400 32'b00111111100000000000000000000000; //1.0
    
end


endmodule