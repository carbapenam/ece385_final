/*
Based on Avalon-MM Interface for AES Decryption IP Core by 
Dong Kai Wang, Fall 2017

For use with ECE 385 Experiment 9
University of Illinois ECE Department
*/

module avalon_vga_interface (
	// Avalon Clock Input
	input logic CLK,
	
	// Avalon Reset Input
	input logic RESET,
	
	// Avalon-MM Slave Signals
	input  logic AVL_READ,					// Avalon-MM Read
	input  logic AVL_WRITE,					// Avalon-MM Write
	input  logic AVL_CS,						// Avalon-MM Chip Select
	input  logic [3:0] AVL_BYTE_EN,		// Avalon-MM Byte Enable
	input  logic [2:0] AVL_ADDR,			// Avalon-MM Address
	input  logic [31:0] AVL_WRITEDATA,	// Avalon-MM Write Data
	output logic [31:0] AVL_READDATA,	// Avalon-MM Read Data
	
	// Exported Conduit
	output wire [31:0] EXPORT_DATA		// Exported Conduit Signals to VGA DACs
);

//Todo:
// If the buffer is set to 0, then write the images to 1 and output images from 0.
// If the buffer is set to 1, then write the images to 0 and output images from 1.


// Register map for R, G, B
// 0 - Red
// 1 - Green
// 2 - Blue
// 3 - DrawX
// 4 - DrawY
// 5 - Buffer_Number

logic [31:0] reg_file[5];

logic VGA_CLK,      //VGA Clock
		VGA_SYNC_N,   //VGA Sync signal
		VGA_BLANK_N,  //VGA Blank signal
		VGA_VS,       //VGA virtical sync signal
		VGA_HS;       //VGA horizontal sync signal
logic [9:0]DrawX, DrawY;
		
logic [7:0] R_out, G_out, B_out;
	

//Ignore Byte Enable hahaha.
always_ff @ (posedge CLK)
begin
	if (RESET)
	begin
		for (int i=0; i<5; i++)
		begin
			reg_file[i] = 32'b0;
		end
	end

	if (AVL_CS && AVL_WRITE)
	begin
		// Only allow Red, Green, Blue to be written
		// Don't let it write DrawX, DrawY
		if (AVL_ADDR < 3'd3)
			reg_file[AVL_ADDR][31:0] <= AVL_WRITEDATA[31:0];	
	end
	
	reg_file[3][9:0] = DrawX;
	reg_file[4][9:0] = DrawY;
end


vga_clk vga_clk_instance(.inclk0(CLK), .c0(VGA_CLK));								 
VGA_controller vga_controller_instance(.Clk(CLK),
                                       .Reset(RESET),
                                       .R_in(reg_file[0][7:0]), 
													.G_in(reg_file[1][7:0]),
												   .B_in(reg_file[2][7:0]),
													.VGA_R(R_out),
													.VGA_G(G_out),
													.VGA_B(B_out),
													.DrawX,
													.DrawY,
													.VGA_CLK,
													.VGA_BLANK_N,  //VGA Blank signal
                                       .VGA_VS,       //VGA virtical sync signal
                                       .VGA_HS
);
// EXPORT_DATA has following structure:
//0:7 - Red
//8:15 - Green
//16:23 - Blue
//24 - VGA_HS
//25 - VGA_VS
//26 - VGA_BLANK_N
//27 - VGA_SYNC_N
//28 - VGA_CLK
//29:31 - 0


//Check if i fucked this part up.
//as in do it in reverse

assign EXPORT_DATA[7:0] = R_out;
assign EXPORT_DATA[15:8] = G_out; 
assign EXPORT_DATA[23:16] = B_out;
assign EXPORT_DATA[24] = VGA_HS;
assign EXPORT_DATA[25] = VGA_VS;
assign EXPORT_DATA[26] = VGA_BLANK_N;
assign EXPORT_DATA[27] = VGA_SYNC_N;
assign EXPORT_DATA[28] = VGA_CLK;
assign EXPORT_DATA[31:29] = 3'b0;
							 
/*
assign EXPORT_DATA = {3'b0,
                      VGA_CLK,
							 VGA_SYNC_N,
							 VGA_BLANK_N,
							 VGA_VS,
							 VGA_HS,
							 B_out,
							 G_out,
							 R_out
							 };

*/							 
assign AVL_READDATA = (AVL_READ && AVL_CS) ? reg_file[AVL_ADDR] : 32'b0;



endmodule
