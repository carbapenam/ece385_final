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
                                     VGA_HS       //VGA horizontal sync signal					  
);

	 logic Reset_h, Clk;
	 logic [31:0] VGA_EXPORT_DATA;
	 //logic [7:0] keycode;
	 
	 assign Clk = CLOCK_50;
	 
	//VGA_EXPORT_DATA has following structure:
	//0:7 - Red
	//8:15 - Green
	//16:23 - Blue
	//24 - VGA_HS
	//25 - VGA_VS
	//26 - VGA_BLANK_N
	//27 - VGA_SYNC_N
	//28 - VGA_CLK
	//29:31 - 0
		 
	 assign VGA_R[7:0] = VGA_EXPORT_DATA[7:0];
	 assign VGA_G[7:0] = VGA_EXPORT_DATA[15:8];
	 assign VGA_B[7:0] = VGA_EXPORT_DATA[23:16];
	 assign VGA_HS = VGA_EXPORT_DATA[24]; 
	 assign VGA_VS = VGA_EXPORT_DATA[25]; 
	 assign VGA_BLANK_N = VGA_EXPORT_DATA[26];
	 assign VGA_SYNC_N = VGA_EXPORT_DATA[27];    		 
	 assign VGA_CLK = VGA_EXPORT_DATA[28];     

	 
	 always_ff @ (posedge Clk) begin
		  Reset_h <= ~(KEY[0]);        // The push buttons are active low
	 end	

	  // You need to make sure that the port names here are identical to the port names at 
	  // the interface in lab7_soc.v
	  final385_soc final385_soc_instance(.clk_clk(CLOCK_50),
								 .reset_reset_n(KEY[0]), 
								 .vga_export_new_signal(VGA_EXPORT_DATA),  	// Exported data
								 .sdram_wire_addr(DRAM_ADDR),    //  sdram_wire.addr
								 .sdram_wire_ba(DRAM_BA),      	//  .ba
								 .sdram_wire_cas_n(DRAM_CAS_N),    //  .cas_n
								 .sdram_wire_cke(DRAM_CKE),     	//  .cke
								 .sdram_wire_cs_n(DRAM_CS_N),      //  .cs_n
								 .sdram_wire_dq(DRAM_DQ),      	//  .dq
								 .sdram_wire_dqm(DRAM_DQM),     	//  .dqm
								 .sdram_wire_ras_n(DRAM_RAS_N),    //  .ras_n
								 .sdram_wire_we_n(DRAM_WE_N),      //  .we_n
								 .sdram_clk_clk(DRAM_CLK)			//  clock out to SDRAM from other PLL port
								 );
								 
								 
								 								 
endmodule