#include "system.h"
#include "alt_types.h"
#include <unistd.h>  // usleep
#include "sys/alt_irq.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

#include "io_handler.h"
#include "cy7c67200.h"
#include "helper_functions.h" //display given text on screen
#include "usb.h"
#include "lcp_cmd.h"
#include "lcp_data.h"
#include "keyboard.h" //keyboard routine
//#include "script.c" //stores the script

#define pi 3.14159

#define CHOICE1_ID 11
#define CHOICE2_ID 21
#define BAD_ENDING1 37
#define BAD_ENDING2 49
int bad_end2 = 0; //flag

volatile alt_u32 *SDRAM_PTR = SDRAM_BASE;

int bg_id, chara_id, text_begin, text_end;

extern alt_u8 n_background;
extern alt_u8 n_chara;
extern alt_u8 n_font;
extern alt_u8 n_text;

alt_u16 offset;
//int backgrounds[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //address for each bg
//int characters[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//address for each character

static int scene_id = 0;

int main()
{
	int x, y;
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

	init_scenes();
	offset = populate_structs();
	printf("memory offset: %d\n", offset);
	test_assets(offset);

	printf("loop begins\n");

	int address = 0;
	bg_id = scenes[scene_id].background_id;
	chara_id = scenes[scene_id].character_id;
	text_begin = scenes[scene_id].text_id_begin;
	text_end = scenes[scene_id].text_id_end;

	//draw background

	if (bg_id!= -1) {
		int bg_addr = backgrounds[bg_id].address + offset; //address of the background
		for (y=0; y<480; y++)
		{
			for (x =0; x<640; x++)
			{
				address = bg_addr + x + 640*y;
				alt_u32 data = SDRAM_PTR[address];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				alt_u8 alpha = (data & 0x0000FF00) >> 8;
				if (alpha == 0) continue;
				//printf("%d, %d, %x, %x\n", x,y,data,color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	//draw 1st character
	if (chara_id != -1){
		int ch1_addr = charas[chara_id].address + offset;
		int y_offset = charas[chara_id].offset_y;
		int x_offset = charas[chara_id].offset_x;
		int w = charas[chara_id].width;
		int h = charas[chara_id].height;
		for (y = y_offset; y < y_offset+h; y++) { //range need to be changed
			for (x = x_offset; x < x_offset+w; x++) {  //range need to be changed
				address = ch1_addr + x + 640*y;
				alt_u32 data = SDRAM_PTR[address];
				alt_u16 color = (data & 0xFFFF0000) >> 16;
				alt_u8 alpha = (data & 0x0000FF00) >> 8;
				if (alpha == 0) continue;
				//printf("%d, %d, %x, %x\n", x,y,data,color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	//draw text
	int text_count = text_begin;

	char cur_text[100];
	while (text_count < text_end && text_count != -1){
		printf("text_address: %d, offset: %d\n", texts[text_count].address, offset);
		copy_str(&cur_text[0], SDRAM_PTR + texts[text_count].address + offset, texts[text_count].length);
		printf("%s\n", cur_text);
		//read the text[text_count] into cur_text
		//need to be done
		display_text(cur_text, font.address + offset);
		alt_u16 keycode = keyboard();
		//printf("\ncode = %x\n", keycode);
		if (keycode!= 0)
			text_count ++;
	}

	//read from keyboard
	int keycode = keyboard();
	printf("\nkey = %d\n", keycode);

	//update scene id based on keycode
	if (scene_id == CHOICE1_ID){  //check for 1st choosing scene
		if (keycode == 'a')   // a is the good choice
			scene_id++;
		if (keycode == 'b')
			scene_id = BAD_ENDING1;
	}
	else if (scene_id == CHOICE2_ID){ //check for 2nd choosing scene
		if (keycode == 'a')   // a is the good choice
			scene_id++;
		if (keycode == 'b')
			scene_id = BAD_ENDING2;
	}

	else if (keycode != 0){
		//reuse common ending
		if (scene_id == 48)
			scene_id = 34;

		//reuse scene 23-26
		if (scene_id == 49){
			bad_end2 = 1;
			scene_id = 23;
		}
		if ((bad_end2 == 1) && (scene_id == 26)){
			bad_end2 = 0;
			scene_id = 50;
		}

		//reuse bad ending
		if (scene_id == 54)
			scene_id = 43;

		//at the last scene, press r will restart
		if (scene_id == 36)
			if (keycode == 'r')
				scene_id = 0;

		//update scene id
		scene_id++;
	}
}

