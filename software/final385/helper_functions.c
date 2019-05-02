/*
 * helper_functions.c
 *
 *  Created on: Apr 28, 2019
 *      Author: wsy
 */

#include <string.h>
#include "system.h"
#include "alt_types.h"
#include <unistd.h>  // usleep
#include "sys/alt_irq.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "helper_functions.h"
#include <stdio.h>

#define SPRITE_BASE_ADDR 0  //the address the spritesheet is stored

#define WIDTH_OF_SPRITESHEET 256

void copy_str(char* dest, alt_u32 *src, int length_in_32_bits)
{
	int i = 0;
	char* current = dest;
	while(i < length_in_32_bits)
	{
		*(current) = *src & (0x000000FF);
		*(current+1) = *src & (0x0000FF00) >> 8;
		*(current+2) = *src & (0x00FF0000) >> 16;
		*(current+3) = *src & (0xFF000000) >> 24;
		i++;
		current += 4;
	}
}

void test_assets(int offset)
{
	alt_u32 *SDRAM_PTR = SDRAM_BASE;
	alt_up_pixel_buffer_dma_dev * pixel_buf_dev;

	// open the Pixel Buffer port
	pixel_buf_dev = alt_up_pixel_buffer_dma_open_dev ("/dev/video_pixel_buffer_dma_0");
	if ( pixel_buf_dev == NULL)
		printf ("Error: could not open pixel buffer device \n");
	else
		printf ("Opened pixel buffer device \n");

	/* Clear the screen */
	alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dev,0);
	alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dev,1);
	printf("cleared screen\n");
	printf("offset: %d\n", offset);
	printf("n_backgrounds : %d\n", n_background);

	for (int i=0; i<n_background; i++)
	{
		int width, height;
		alt_u32 addr;

		width = backgrounds[i].width;
		height = backgrounds[i].height;
		addr = backgrounds[i].address;
		printf("drawing an background by %d x %d at %d\n", width, height, addr);
		for(int y=0; y<height; y++)
		{
			for (int x=0; x<width; x++)
			{
				alt_u32 data = SDRAM_PTR[offset + addr + (width*y) + x];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				//printf("color: %x\n",color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}

		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	for (int i=0; i<n_chara; i++)
	{
		int width, height;
		alt_u32 addr;

		width = charas[i].width;
		height = charas[i].height;
		addr = charas[i].address;
		printf("drawing a character by %d x %d at %d\n", width, height, addr);
		for(int y=0; y<height; y++)
		{
			for (int x=0; x<width; x++)
			{
				alt_u32 data = SDRAM_PTR[offset + addr + (width*y) + x];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
		alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dev,1);

	}

	while(1)
	{
		int width, height;
		alt_u32 addr;

		width = font.width;
		height = font.height;
		addr = font.address;
		printf("drawing an font by %d x %d at %d\n", width, height, addr);
		for(int y=0; y<height; y++)
		{
			for (int x=0; x<width; x++)
			{
				alt_u32 data = SDRAM_PTR[offset + addr + (width*y) + x];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
		break;
	}

	/*
	for (int i=0; i<n_text; i++)
	{
		memcpy()
		printf("%d: %s\n", i, SDRAM_PTR+offset+texts[i].address]);
	}*/
}

alt_u32 populate_structs()
{
	//Reset the counter for assets.

	n_background = 0;
	n_chara = 0;
	n_font = 0;
	n_text = 0;

	alt_u32 content = 0;
	alt_u32 addr = 0;

	alt_u32 *SDRAM_PTR = SDRAM_BASE;

	//Start with just reading the sdram
	char reading_mode = 0;

	while (1)
	{
		content = SDRAM_PTR[addr];
		//printf("%x", content);

		if ((char) content == 'D')
		{
			printf("Reached the end of the memory.\n");
			addr++;
			return addr;
			break;
		}

		if ((char) content == 'B')
		{
			printf("Populating background\n");
			reading_mode = 'B';
			addr++;
			continue;
		}

		if ((char) content == 'C')
		{
			printf("Populating character\n");
			reading_mode = 'C';
			addr++;
			continue;
		}

		if ((char) content == 'F')
		{
			printf("Populating font\n");
			reading_mode = 'F';
			addr++;
			continue;
		}

		if ((char) content == 'T')
		{
			printf("Populating text\n");
			reading_mode = 'T';
			addr++;
			continue;
		}

		switch (reading_mode)
		{
			case 'B':
				content = SDRAM_PTR[addr];
				backgrounds[n_background].width = (alt_u16) content;

				content = SDRAM_PTR[addr+1];
				backgrounds[n_background].height = (alt_u16) content;

				content = SDRAM_PTR[addr+2];
				backgrounds[n_background].address = content;

				printf("read a background with %d x %d at %d\n", backgrounds[n_background].width,
						backgrounds[n_background].height, backgrounds[n_background].address);

				addr += 3;
				n_background++;
				break;

			case 'C':
				charas[n_chara].character_id = n_chara;

				content = SDRAM_PTR[addr];
				charas[n_chara].width = (alt_u16) content;

				content = SDRAM_PTR[addr+1];
				charas[n_chara].height = (alt_u16) content;

				content = SDRAM_PTR[addr+2];
				charas[n_chara].offset_x = (alt_u16) content;

				content = SDRAM_PTR[addr+3];
				charas[n_chara].offset_y = (alt_u16) content;

				content = SDRAM_PTR[addr+4];
				charas[n_chara].address = content;

				printf("read a chara with %d x %d with offset (%d, %d) at %d\n", charas[n_chara].width,
						charas[n_chara].height, charas[n_chara].offset_x, charas[n_chara].offset_y,
						charas[n_chara].address);

				addr += 5;
				n_chara++;
				break;

			case 'F':
				content = SDRAM_PTR[addr];
				font.width = (alt_u16) content;

				content = SDRAM_PTR[addr+1];
				font.height = (alt_u16) content;

				content = SDRAM_PTR[addr+2];
				font.address = content;

				printf("read a font with %d x %d at %d\n", font.width,
						font.height, font.address);

				addr += 3;
				break;

			case 'T':
				texts[n_text].text_id = n_text;

				content = SDRAM_PTR[addr];
				texts[n_text].length = (alt_u8) content;

				content = SDRAM_PTR[addr+1];
				texts[n_text].address = content;

				printf("read a text that's %d long at %d\n", texts[n_text].length,
							texts[n_text].address);

				addr += 2;
				n_text++;
				break;
		}
	}

	printf("data loading complete.\n");
}

//font spritesheet is 10 x 10 and 24 x 24 for each font, whole pic size is 256 * 256

void display_text(char text[], volatile alt_u32 * SDRAM_PTR){
	int pos_x = 0;
	int pos_y = 360;
	int x, y;
	int i = 0;
	alt_up_pixel_buffer_dma_dev * pixel_buf_dev;
	char cur = text[i];//cur contains the current character to be displayed

	while (cur!='\0'){
		if (cur == '^'){
			pos_y = pos_y + 20;
			pos_x = 0;
			continue;
		}
		int index = cur - ' ';
		int sprite_x = index % 16;
		int sprite_y = index / 16;
		for (int row = 0; x < 16; x++)
			for (int col = 0; y < 16; y++){
				int address =(sprite_y + row) * WIDTH_OF_SPRITESHEET + sprite_x * 16 + col;
				alt_u32 data = SDRAM_PTR[SPRITE_BASE_ADDR+address];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				alt_u8 alpha = (data & 0x0000FF00) >> 8;
				if (alpha == 0) continue;
				x = pos_x + col;
				y = pos_y + row;
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
		pos_x = pos_x + 16;
		/*if (pos_x >= 640){
			pos_x = 0;
			pos_y = pos_y + 25;
		}*/
		if (pos_x >= 640 && pos_y >= 480)
			return;
		i++;
		cur = text[i];
	}
}

void init_scenes(){
	scenes[0].scene_id = 0;
	scenes[0].background_id = -1;
	scenes[0].character_id = -1;
	scenes[0].text_id_begin = 0;
	scenes[0].text_id_end = 1;

	scenes[1].scene_id = 1;
	scenes[1].background_id = 0;
	scenes[1].character_id = -1;
	scenes[1].text_id_begin = 2;
	scenes[1].text_id_end = 4;

	scenes[2].scene_id = 2;
	scenes[2].background_id = 0;
	scenes[2].character_id = 0;
	scenes[2].text_id_begin = 5;
	scenes[2].text_id_end = 8;

	scenes[3].scene_id = 3;
	scenes[3].background_id = 1;
	scenes[3].character_id = -1;
	scenes[3].text_id_begin = 9;
	scenes[3].text_id_end = 11;

	scenes[4].scene_id = 4;
	scenes[4].background_id = 2;
	scenes[4].character_id = -1;
	scenes[4].text_id_begin = 12;
	scenes[4].text_id_end = 16;

	scenes[5].scene_id = 5;
	scenes[5].background_id = 2;
	scenes[5].character_id = 4;
	scenes[5].text_id_begin = 17;
	scenes[5].text_id_end = 21;

	scenes[6].scene_id = 6;
	scenes[6].background_id = 2;
	scenes[6].character_id = 3;
	scenes[6].text_id_begin = 22;
	scenes[6].text_id_end = 34;

	scenes[7].scene_id = 7;
	scenes[7].background_id = 4;
	scenes[7].character_id = 4;
	scenes[7].text_id_begin = 35;
	scenes[7].text_id_end = 40;

	scenes[8].scene_id = 8;
	scenes[8].background_id = 4;
	scenes[8].character_id = 3;
	scenes[8].text_id_begin = 41;
	scenes[8].text_id_end = 44;

	scenes[9].scene_id = 9;
	scenes[9].background_id = 4;
	scenes[9].character_id = -1;
	scenes[9].text_id_begin = 45;
	scenes[9].text_id_end = 49;

	scenes[10].scene_id = 10;
	scenes[10].background_id = 4;
	scenes[10].character_id = 1;
	scenes[10].text_id_begin = 50;
	scenes[10].text_id_end = 51;

	scenes[11].scene_id = 11;
	scenes[11].background_id = 4;
	scenes[11].character_id = 1;
	scenes[11].text_id_begin = 52;
	scenes[11].text_id_end = 52;

	scenes[12].scene_id = 12;
	scenes[12].background_id = 4;
	scenes[12].character_id = -1;
	scenes[12].text_id_begin = 53;
	scenes[12].text_id_end = 54;

	scenes[13].scene_id = 13;
	scenes[13].background_id = 2;
	scenes[13].character_id = -1;
	scenes[13].text_id_begin = 55;
	scenes[13].text_id_end = 55;

	scenes[14].scene_id = 14;
	scenes[14].background_id = 2;
	scenes[14].character_id = 7;
	scenes[14].text_id_begin = 56;
	scenes[14].text_id_end = 59;

	scenes[15].scene_id = 15;
	scenes[15].background_id = 2;
	scenes[15].character_id = 7;
	scenes[15].text_id_begin = 60;
	scenes[15].text_id_end = 61;

	scenes[16].scene_id = 16;
	scenes[16].background_id = 2;
	scenes[16].character_id = 6;
	scenes[16].text_id_begin = 62;
	scenes[16].text_id_end = 63;

	scenes[17].scene_id = 17;
	scenes[17].background_id = 2;
	scenes[17].character_id = 7;
	scenes[17].text_id_begin = 64;
	scenes[17].text_id_end = 67;

	scenes[18].scene_id = 18;
	scenes[18].background_id = 2;
	scenes[18].character_id = -1;
	scenes[18].text_id_begin = 68;
	scenes[18].text_id_end = 68;

	scenes[19].scene_id = 19;
	scenes[19].background_id = 3;
	scenes[19].character_id = -1;
	scenes[19].text_id_begin = 69;
	scenes[19].text_id_end = 71;

	scenes[20].scene_id = 20;
	scenes[20].background_id = 3;
	scenes[20].character_id = 3;
	scenes[20].text_id_begin = 72;
	scenes[20].text_id_end = 73;

	scenes[21].scene_id = 21;
	scenes[21].background_id = 3;
	scenes[21].character_id = 3;
	scenes[21].text_id_begin = 74;
	scenes[21].text_id_end = 74;

	scenes[22].scene_id = 22;
	scenes[22].background_id = 3;
	scenes[22].character_id = 3;
	scenes[22].text_id_begin = 75;
	scenes[22].text_id_end = 77;

	scenes[23].scene_id = 23;
	scenes[23].background_id = 3;
	scenes[23].character_id = -1;
	scenes[23].text_id_begin = 78;
	scenes[23].text_id_end = 87;

	scenes[24].scene_id = 24;
	scenes[24].background_id = 3;
	scenes[24].character_id = 4;
	scenes[24].text_id_begin = 78;
	scenes[24].text_id_end = 87;

	scenes[25].scene_id = 25;
	scenes[25].background_id = 3;
	scenes[25].character_id = -1;
	scenes[25].text_id_begin = 88;
	scenes[25].text_id_end = 97;

	scenes[26].scene_id = 26;
	scenes[26].background_id = 3;
	scenes[26].character_id = -1;
	scenes[26].text_id_begin = 98;
	scenes[26].text_id_end = 98;

	scenes[27].scene_id = 27;
	scenes[27].background_id = 3;
	scenes[27].character_id = 5;
	scenes[27].text_id_begin = 99;
	scenes[27].text_id_end = 100;

	scenes[28].scene_id = 28;
	scenes[28].background_id = 2;
	scenes[28].character_id = 8;
	scenes[28].text_id_begin = 101;
	scenes[28].text_id_end = 103;

	scenes[29].scene_id = 29;
	scenes[29].background_id = 2;
	scenes[29].character_id = -1;
	scenes[29].text_id_begin = 104;
	scenes[29].text_id_end = 105;

	scenes[30].scene_id = 30;
	scenes[30].background_id = 2;
	scenes[30].character_id = 9;
	scenes[30].text_id_begin = 106;
	scenes[30].text_id_end = 108;

	scenes[31].scene_id = 31;
	scenes[31].background_id = 2;
	scenes[31].character_id = -1;
	scenes[31].text_id_begin = 109;
	scenes[31].text_id_end = 110;

	scenes[32].scene_id = 32;
	scenes[32].background_id = 2;
	scenes[32].character_id = 5;
	scenes[32].text_id_begin = 111;
	scenes[32].text_id_end = 117;

	scenes[33].scene_id = 33;
	scenes[33].background_id = 2;
	scenes[33].character_id = -1;
	scenes[33].text_id_begin = 118;
	scenes[33].text_id_end = 118;

	scenes[34].scene_id = 34;
	scenes[34].background_id = -1;
	scenes[34].character_id = -1;
	scenes[34].text_id_begin = -1;
	scenes[34].text_id_end = -1;

	scenes[35].scene_id = 35;
	scenes[35].background_id = 6;
	scenes[35].character_id = -1;
	scenes[35].text_id_begin = -1;
	scenes[35].text_id_end = -1;

	scenes[36].scene_id = 36;
	scenes[36].background_id = 7;
	scenes[36].character_id = -1;
	scenes[36].text_id_begin = -1;
	scenes[36].text_id_end = -1;

	scenes[37].scene_id = 37;
	scenes[37].background_id = 4;
	scenes[37].character_id = -1;
	scenes[37].text_id_begin = 119;
	scenes[37].text_id_end = 119;

	scenes[38].scene_id = 38;
	scenes[38].background_id = 2;
	scenes[38].character_id = -1;
	scenes[38].text_id_begin = 120;
	scenes[38].text_id_end = 129;

	scenes[39].scene_id = 39;
	scenes[39].background_id = 3;
	scenes[39].character_id = 3;
	scenes[39].text_id_begin = 130;
	scenes[39].text_id_end = 130;

	scenes[40].scene_id = 40;
	scenes[40].background_id = 3;
	scenes[40].character_id = 2;
	scenes[40].text_id_begin = 131;
	scenes[40].text_id_end = 133;

	scenes[41].scene_id = 41;
	scenes[41].background_id = 3;
	scenes[41].character_id = -1;
	scenes[41].text_id_begin = 134;
	scenes[41].text_id_end = 136;

	scenes[42].scene_id = 42;
	scenes[42].background_id = 3;
	scenes[42].character_id = 3;
	scenes[42].text_id_begin = 137;
	scenes[42].text_id_end = 139;

	scenes[43].scene_id = 43;
	scenes[43].background_id = 3;
	scenes[43].character_id = -1;
	scenes[43].text_id_begin = 140;
	scenes[43].text_id_end = 141;

	scenes[44].scene_id = 44;
	scenes[44].background_id = -1;
	scenes[44].character_id = -1;
	scenes[44].text_id_begin = 142;
	scenes[44].text_id_end = 145;



	scenes[45].scene_id = 45;
	scenes[45].background_id = 3;
	scenes[45].character_id = 1;
	scenes[45].text_id_begin = 146;
	scenes[45].text_id_end = 146;

	scenes[46].scene_id = 46;
	scenes[46].background_id = 3;
	scenes[46].character_id = -1;
	scenes[46].text_id_begin = 147;
	scenes[46].text_id_end = 148;

	scenes[47].scene_id = 47;
	scenes[47].background_id = 3;
	scenes[47].character_id = 1;
	scenes[47].text_id_begin = 149;
	scenes[47].text_id_end = 149;

	scenes[48].scene_id = 48;
	scenes[48].background_id = 3;
	scenes[48].character_id = 1;
	scenes[48].text_id_begin = 150;
	scenes[48].text_id_end = 152;

	scenes[49].scene_id = 49;
	scenes[49].background_id = 3;
	scenes[49].character_id = -1;
	scenes[49].text_id_begin = 153;
	scenes[49].text_id_end = 154;

	scenes[50].scene_id = 50;
	scenes[50].background_id = 3;
	scenes[50].character_id = -1;
	scenes[50].text_id_begin = 155;
	scenes[50].text_id_end = 155;

	scenes[51].scene_id = 51;
	scenes[51].background_id = 3;
	scenes[51].character_id = 2;
	scenes[51].text_id_begin = 156;
	scenes[51].text_id_end = 157;

	scenes[52].scene_id = 52;
	scenes[52].background_id = 3;
	scenes[52].character_id = -1;
	scenes[52].text_id_begin = 158;
	scenes[52].text_id_end = 158;

	scenes[53].scene_id = 53;
	scenes[53].background_id = 3;
	scenes[53].character_id = 3;
	scenes[53].text_id_begin = 159;
	scenes[53].text_id_end = 159;

	scenes[54].scene_id = 54;
	scenes[54].background_id = 3;
	scenes[54].character_id = -1;
	scenes[54].text_id_begin = 160;
	scenes[54].text_id_end = 160;
}

