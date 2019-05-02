module final385  ( input	       CLOCK_50, 
					  input  [3:0]  KEY,
					  input  [7:0]  SW,
					  output [7:0]  LEDG,
					  
					  //SDRAMs
					  output [12:0] DRAM_ADDR,
					  output [1:0]  DRAM_BA,
					  output        DRAM_CAS_N,
					  output		    DRAM_CKE,
					  output		    DRAM_CS_N,
					  inout  [31:0] DRAM_DQ,
					  output  [3:0] DRAM_DQM,
					  output		    DRAM_RAS_N,
					  output		    DRAM_WE_N,
					  output		    DRAM_CLK,
					  
					  //VGAs
                 output logic [7:0]  VGA_R,        //VGA Red
                                     VGA_G,        //VGA Green
                                     VGA_B,        //VGA Blue
                 output logic        VGA_CLK,      //VGA Clock
                                     VGA_SYNC_N,   //VGA Sync signal
                                     VGA_BLANK_N,  //VGA Blank signal
                                     VGA_VS,       //VGA virtical sync signal
                                     VGA_HS,       //VGA horizontal sync signal
						
					  //SRAM
					  	inout  [15:0] SRAM_DQ,           //       sram_wire.DQ
		            output [19:0] SRAM_ADDR,         //                .ADDR
		            output        SRAM_LB_N,         //                .LB_N
		            output        SRAM_UB_N,         //                .UB_N
		            output        SRAM_CE_N,         //                .CE_N
		            output        SRAM_OE_N,         //                .OE_N
		            output        SRAM_WE_N,          //                .WE_N
             
				 // CY7C67200 Interface
             inout  wire  [15:0] OTG_DATA,     //CY7C67200 Data bus 16 Bits
             output logic [1:0]  OTG_ADDR,     //CY7C67200 Address 2 Bits
             output logic        OTG_CS_N,     //CY7C67200 Chip Select
                                 OTG_RD_N,     //CY7C67200 Write
                                 OTG_WR_N,     //CY7C67200 Read
                                 OTG_RST_N,    //CY7C67200 Reset
             input               OTG_INT      //CY7C67200 Interrupt
						);

	 logic Reset_h, Clk;
	 logic [7:0] keycode;
	 
	 assign Clk = CLOCK_50;
 
	 
	 always_ff @ (posedge Clk) begin
		  Reset_h <= ~(KEY[0]);        // The push buttons are active low
	 end	

    logic [1:0] hpi_addr;
    logic [15:0] hpi_data_in, hpi_data_out;
    logic hpi_r, hpi_w, hpi_cs, hpi_reset;
    
    // Interface between NIOS II and EZ-OTG chip
    hpi_io_intf hpi_io_inst(
                            .Clk(Clk),
                            .Reset(Reset_h),
                            // signals connected to NIOS II
                            .from_sw_address(hpi_addr),
                            .from_sw_data_in(hpi_data_in),
                            .from_sw_data_out(hpi_data_out),
                            .from_sw_r(hpi_r),
                            .from_sw_w(hpi_w),
                            .from_sw_cs(hpi_cs),
                            .from_sw_reset(hpi_reset),
                            // signals connected to EZ-OTG chip
                            .OTG_DATA(OTG_DATA),    
                            .OTG_ADDR(OTG_ADDR),    
                            .OTG_RD_N(OTG_RD_N),    
                            .OTG_WR_N(OTG_WR_N),    
                            .OTG_CS_N(OTG_CS_N),
                            .OTG_RST_N(OTG_RST_N)
    );	 
	 
	
	// You need to make sure that the port names here are identical to the port names at 
	  // the interface in lab7_soc.v
	  final385_soc final385_soc_instance(.clk_clk(CLOCK_50),
								 .reset_reset_n(KEY[0]), 
								 .sdram_wire_addr(DRAM_ADDR),    //  sdram_wire.addr
								 .sdram_wire_ba(DRAM_BA),      	//  .ba
								 .sdram_wire_cas_n(DRAM_CAS_N),    //  .cas_n
								 .sdram_wire_cke(DRAM_CKE),     	//  .cke
								 .sdram_wire_cs_n(DRAM_CS_N),      //  .cs_n
								 .sdram_wire_dq(DRAM_DQ),      	//  .dq
								 .sdram_wire_dqm(DRAM_DQM),     	//  .dqm
								 .sdram_wire_ras_n(DRAM_RAS_N),    //  .ras_n
								 .sdram_wire_we_n(DRAM_WE_N),      //  .we_n
								 .sdram_clk_clk(DRAM_CLK),			//  clock out to SDRAM from other PLL port

                             .keycode_export(keycode),  
                             .otg_hpi_address_export(hpi_addr),
                             .otg_hpi_data_in_port(hpi_data_in),
                             .otg_hpi_data_out_port(hpi_data_out),
                             .otg_hpi_cs_export(hpi_cs),
                             .otg_hpi_r_export(hpi_r),
                             .otg_hpi_w_export(hpi_w),
                             .otg_hpi_reset_export(hpi_reset),
								 
								 .sram_wire_DQ(SRAM_DQ),           //       sram_wire.DQ
		                   .sram_wire_ADDR(SRAM_ADDR),         //                .ADDR
		                   .sram_wire_LB_N(SRAM_LB_N),         //                .LB_N
		                   .sram_wire_UB_N(SRAM_UB_N),         //                .UB_N
		                   .sram_wire_CE_N(SRAM_CE_N),         //                .CE_N
		                   .sram_wire_OE_N(SRAM_OE_N),         //                .OE_N
		                   .sram_wire_WE_N(SRAM_WE_N),        //                .WE_N
								 
								 .vga_CLK(VGA_CLK),                //             vga.CLK
                         .vga_HS(VGA_HS),                 //                .HS
.vga_VS(VGA_VS),                 //                .VS
.vga_BLANK(VGA_BLANK_N),              //                .BLANK
.vga_SYNC(VGA_SYNC_N),               //                .SYNC
.vga_R(VGA_R),                  //                .R
.vga_G(VGA_G),                  //                .G
.vga_B(VGA_B)                   //                .B
								 );
								 
								 
								 								 
endmodule