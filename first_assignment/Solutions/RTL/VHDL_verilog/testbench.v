`timescale 1ns / 1ps

module testbench();
    reg clk, rst;
    reg ready;
    reg[31:0] op1, op2;
    wire[31:0] res;
    wire done;
    parameter PERIOD = 20;
    parameter NUM_TEST = 20;
    reg [31:0] mem [0:NUM_TEST*3-1]; 
    integer i;
    
    wire[2:0] STATE, NEXT_STATE;


double_multiplier dut(
    .clk (clk),
    .rst (rst),
    .ready (ready),
    .op1 (op1),
    .op2 (op2),
    .res (res),
    .done (done),
    .STATE (STATE),
    .NEXT_STATE (NEXT_STATE)
);


initial begin    
    //Test1
    mem[0] = 32'b01000001100000100000000000000000; //16.25
    mem[1] = 32'b01000001000100000000000000000000; //9.0
    mem[2] = 32'b01000011000100100100000000000000; //146.25
    
    //Test2
    mem[3] = 32'b01000001101100000000000000000000; //22.0
    mem[4] = 32'b11000000101000000000000000000000; //-5.0
    mem[5] = 32'b11000010110111000000000000000000; //-110.0
    
    //Test3
    mem[6] = 32'b01000000010110011001100110011010; //3.400000095367431640625
    mem[7] = 32'b01000000010110011001100110011010; //3.400000095367431640625
    mem[8] = 32'b01000001001110001111010111000011; //11.56000041961669921875
    
    //Test4
    mem[9] = 32'b01000010101001111010100011110110;  //83.8300018310546875
    mem[10] = 32'b00111110000011110101110000101001; //0.14000000059604644775390625
    mem[11] = 32'b01000001001110111100011101111010; //11.7362003326416015625
    
    //Test5 Verilog
    mem[12] = 32'b11111111100000000000000000000000; //-inf
    mem[13] = 32'b01000000000000000000000000000000; //2.0
    mem[14] = 32'b11111111100000000000000000000000; //-inf
    
    //Test6 VHDL
    mem[15] = 32'b11111111100000000000000000000000; //-inf
    mem[16] = 32'b01000000000000000000000000000000; //2.0
    mem[17] = 32'b11111111100000000000000000000000; //-inf
    
    //Test7 Verilog
    mem[18] = 32'b00111111101000000000000000000000; //1.25
    mem[19] = 32'b00000000000000000000000000000000; //0.0
    mem[20] = 32'b00000000000000000000000000000000; //0.0
    
    //Test8 VHDL
    mem[21] = 32'b00111111101000000000000000000000; //1.25
    mem[22] = 32'b00000000000000000000000000000000; //0.0
    mem[23] = 32'b00000000000000000000000000000000; //0.0
    
    //Test9 Verilog
    mem[24] = 32'b11111111100000000000000000000000; //inf
    mem[25] = 32'b00000000000000000000000000000000; //0.0
    mem[26] = 32'b11111111110000000000000000000000; //nan quiet
    
    //Test10 VHDL
    mem[27] = 32'b11111111100000000000000000000000; //inf
    mem[28] = 32'b00000000000000000000000000000000; //0.0
    mem[29] = 32'b11111111110000000000000000000000; //nan quiet
    
    //Test11 Verilog
    mem[30] = 32'b11111111100000000000000000000110; //nan signal
    mem[31] = 32'b00111111101000000000000000000000; //1.25
    mem[32] = 32'b11111111100000000000000000000110; //nan signal
    
    //Test12 VHDL
    mem[33] = 32'b11111111100000000000000000000110; //nan signal
    mem[34] = 32'b00111111101000000000000000000000; //1.25
    mem[35] = 32'b11111111100000000000000000000110; //nan signal
    
    //Test13 Verilog
    mem[36] = 32'b01000010111101110111100011110010; //123.7362213134765625
    mem[37] = 32'b00111010101000010011011111110100; //0.0012300000526010990142822265625
    mem[38] = 32'b00111110000110111101100100100111; //0.15219555795192718505859375
    
    //Test14 VHDL
    mem[39] = 32'b01000010111101110111100011110010; //123.7362213134765625
    mem[40] = 32'b00111010101000010011011111110100; //0.0012300000526010990142822265625
    mem[41] = 32'b00111110000110111101100100100111; //0.15219555795192718505859375
    
    //Test15 Verilog
    mem[42] = 32'b01000010100100011100110011001101; //72.90000152587890625
    mem[43] = 32'b01000001000100000000000000000000; //9.0
    mem[44] = 32'b01000100001001000000011001100111; //656.10003662109375
    
    //Test16 VHDL
    mem[45] = 32'b01000010100100011100110011001101; //72.90000152587890625
    mem[46] = 32'b01000001000100000000000000000000; //9.0
    mem[47] = 32'b01000100001001000000011001100111; //656.10003662109375
    
    //Test17 Verilog
    mem[48] = 32'b01001001011101000010010000000000; //1000000.0
    mem[49] = 32'b01001001010110010000001110001110; //888888.875
    mem[50] = 32'b01010011010011101111010111101000; //888888885248
    
    //Test18 VHDL
    mem[51] = 32'b01001001011101000010010000000000; //1000000.0
    mem[52] = 32'b01001001010110010000001110001110; //888888.875
    mem[53] = 32'b01010011010011101111010111101000; //888888885248
    
    //Test19 Verilog
    mem[54] = 32'b01000010100100011100110011001101; //72.9
    mem[55] = 32'b01000001000100000000000000000000; //9.0
    mem[56] = 32'b01000100001001000000011001100111; //656.1
    
    //Test20 VHDL
    mem[57] = 32'b01000010100100011100110011001101; //72.9
    mem[58] = 32'b01000001000100000000000000000000; //9.0
    mem[59] = 32'b01000100001001000000011001100111; //656.1
end


always #(PERIOD/2) clk  <= !clk; // Set clock

initial 
begin    
    // Init
    clk <= 1'b1;
    ready <= 1'b0;
    rst <= 1'b0;
    i <= 0;
    
    // Wait for startup FPGA
    #(PERIOD*20);
    
    // Reset
    #PERIOD;
    rst <= 1'b1;
    #(PERIOD/2);
    #PERIOD;
    rst <= 1'b0;    
    
    // Test2
    while (i < NUM_TEST * 3) begin        
        #PERIOD;
        ready <= 1'b1;
        #PERIOD;
        ready <= 1'b0;
        op1 <= mem[i];
        op2 <= mem[i+1];
        #PERIOD;
        op1 <= mem[i+3];
        op2 <= mem[i+4];
        #PERIOD;
        @(posedge done) #(PERIOD/2);
        if (res == mem[i+2])
            $display("TEST VERILOG %d OK", 1+i/3);
        else
            $display("TEST VERILOG %d FAILED -> res= %b", 1+i/3, res);
        #PERIOD;
        if (res == mem[i+5])
            $display("TEST VHDL    %d OK", 2+i/3);
        else
            $display("TEST VHDL    %d FAILED -> res= %b", 2+i/3, res);        
        i = i + 6;
    end
end


endmodule