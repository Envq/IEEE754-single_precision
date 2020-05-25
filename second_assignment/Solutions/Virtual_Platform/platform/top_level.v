`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Create Date: 09.11.2019 15:06:45
// Design Name: 
// Module Name: top_level
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
//   Splatters Top Level module
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module top_level(

    // SW interface for ROM loading 
    input           clk,
    input           rst,
    input           mode,
    input [7:0]     sw_din,
    input [13:0]    sw_addr,
    input           we_n_in,
    
    //Data input/output for/from IO Module
    input   [31:0]  din,
    input           din_rdy,
    output  [31:0]  dout,
    output          dout_rdy,
    output          din_req
     
);

    //CPU Memory
    wire [7:0]  datai,datao;
    wire [15:0] addr;   // Address bus
    wire        oeb; 			//  data bus negtive enable
    wire        irq_n;         //  interrupt request
    wire        nmi_n;         //  Non-makable interrupt request
    wire        res_n; 		   //  reset
    wire        rdy;			//  ready
    wire        sob_n; 		   //  Set Overflow bit
    wire        sync;			//  indicates an opcode fetch
    wire        vpab; 			//  Vector Pull Address
    wire        we_n; 			//  1=read,0=write
    wire        clk_div4;       // Clock divider 4x
    
    
    //Memory Bus Interface
    wire [7:0] psel;
    wire [31:0] pwdata,paddr,prdata;
    wire penable,pready,pwrite,presetn;
    
    wire        apb_1_pclk;
    wire        apb_1_presetn;
    wire [31:0] apb_1_paddr;
    wire        apb_1_psel;
    wire        apb_1_penable;
    wire        apb_1_pwrite;
    wire [31:0] apb_1_pwdata;
    wire        apb_1_pready;
    wire [31:0] apb_1_prdata;
    
    wire        apb_2_pclk;
    wire        apb_2_presetn;
    wire [31:0] apb_2_paddr;
    wire        apb_2_psel;
    wire        apb_2_penable;
    wire        apb_2_pwrite;
    wire [31:0] apb_2_pwdata;
    wire        apb_2_pready;
    wire [31:0] apb_2_prdata;
    
    //add new wire for the bus
    
    //-----------------------------------
    // Memory
    //----------------------------------- 
    memory_top memory(  .clk(clk),
                        .rst(rst),
                        .clk_div4(clk_div4),
                        .datai(datai),
                        .datao(datao),
                        .addr(addr),
                        .oeb(oeb),
                        .irq_n(irq_n),
                        .nmi_n(nmi_n),
                        .res_n(res_n),
                        .rdy(rdy),
                        .sob_n(sob_n),
                        .sync(sync),
                        .vpab(vpab),
                        .we_n(we_n),
                        .mode(mode),
                        .sw_din(sw_din),
                        .sw_addr(sw_addr),
                        .we_n_in(we_n_in),
                        .psel(psel),
                        .pwrite(pwrite),
                        .penable(penable),
                        .presetn(presetn),
                        .pwdata(pwdata),
                        .paddr(paddr),
                        .pready(pready),
                        .prdata(prdata));
    
    //-----------------------------------
    // m6502 CPU
    //-----------------------------------                    
    m6502 cpu(  .clk(clk_div4),
                .datai(datai),
                .datao(datao),
                .addr(addr),
                .oeb(oeb),
                .irq_n(irq_n),
                .nmi_n(nmi_n),
                .res_n(res_n),
                .rdy(rdy),
                .sob_n(sob_n),
                .sync(sync),
                .vpab(vpab),
                .we_n(we_n));
    
    
    //------------------------------------------------------
    // APB Bus
    //------------------------------------------------------
    // 8 peripheral ports available! Only 2 used 
    // Peripheral 1 =  multiplier
    // Peripheral 2 =  I/O module
    // peripheral 3 = Your Multiplier!
    amba_apb_bus bus(   .clk(clk_div4),
                        .apb_master_presetn(presetn),
                        .apb_master_paddr(paddr),
                        .apb_master_psel1(psel[0]),
                        .apb_master_psel2(psel[1]),
                        .apb_master_psel3(psel[2]),
                        .apb_master_psel4(psel[3]),
                        .apb_master_psel5(psel[4]),
                        .apb_master_psel6(psel[5]),
                        .apb_master_psel7(psel[6]),
                        .apb_master_psel8(psel[7]),
                        .apb_master_penable(penable),
                        .apb_master_pwrite(pwrite),
                        .apb_master_pwdata(pwdata),
                        .apb_master_pready(pready),
                        .apb_master_prdata(prdata),
                        .apb_1_pclk(apb_1_pclk),
                        .apb_1_presetn(apb_1_presetn),
                        .apb_1_paddr(apb_1_paddr),
                        .apb_1_psel(apb_1_psel),
                        .apb_1_penable(apb_1_penable),
                        .apb_1_pwrite(apb_1_pwrite),
                        .apb_1_pwdata(apb_1_pwdata),
                        .apb_1_pready(apb_1_pready),
                        .apb_1_prdata(apb_1_prdata),
                        .apb_2_pclk(apb_2_pclk),
                        .apb_2_presetn(apb_2_presetn),
                        .apb_2_paddr(apb_2_paddr),
                        .apb_2_psel(apb_2_psel),
                        .apb_2_penable(apb_2_penable),
                        .apb_2_pwrite(apb_2_pwrite),
                        .apb_2_pwdata(apb_2_pwdata),
                        .apb_2_pready(apb_2_pready),
                        .apb_2_prdata(apb_2_prdata)
                        );
    
    //------------------------------------------------------
    // Multiplier
    //------------------------------------------------------                    
    multiplier_apb_wrapper multiplier(  .pclk(apb_1_pclk),
                                        .presetn(apb_1_presetn),
                                        .paddr(apb_1_paddr),
                                        .psel(apb_1_psel),
                                        .penable(apb_1_penable),
                                        .pwrite(apb_1_pwrite),  
                                        .pwdata(apb_1_pwdata),
                                        .pready(apb_1_pready),
                                        .prdata(apb_1_prdata));
   
   //------------------------------------------------------
   // I/O Module
   //------------------------------------------------------                     
    IO_module io(
		.pclk(apb_2_pclk),	
		.presetn(apb_2_presetn), 
		.paddr(apb_2_paddr), 	
		.psel(apb_2_psel), 	
		.penable(apb_2_penable), 
		.pwrite(apb_2_pwrite), 	
		.pwdata(apb_2_pwdata), 	
		.pready(apb_2_pready), 
		.prdata(apb_2_prdata),
		.din(din),
		.din_rdy(din_rdy),
		.dout(dout),
		.dout_rdy(dout_rdy),
		.din_req(din_req)
    );                       
     
     //------------------------------------------------------
     // ADD your floating point Multiplier here! 
     // Pay attention on clock.... ;)  
     //------------------------------------------------------       
               
endmodule