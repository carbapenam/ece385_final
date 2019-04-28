/*
 * helper_functions.c
 *
 *  Created on: Apr 28, 2019
 *      Author: wsy
 */

#include <string.h>

#define SPRITE_BASE_ADDR 0  //the address the spritesheet is stored

//font spritesheet is 10 x 10 and 24 x 24 for each font, whole pic size is 256 * 256

void display_font(string text){
	int pos_x = 0;
	int pos_y = 360;
	int x, y;
	for (int i = 0; i < text.length(); i++){
		char cur = text[i]; //cur contains the current character to be displayed
		int index = cur - ' ';
		int sprite_x = index % 10;
		int sprite_y = index / 10;
		for (int row = 0; x < 24; x++)
			for (int col = 0; y < 24; y++){
				address =(sprite_y + row) * 256 + sprite_x + col;
				alt_u32 data = SDRAM_PTR[SPRITE_BASE_ADDR+address];
				alt_u16 color = (data | 0xFFFF00) >> 16;
				x = pos_x + col;
				y = pos_y + row;
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}
}
