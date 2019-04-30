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
#include "script.c" //stores the script

#define pi 3.14159

#define SIZE 50
#define CHOICE1_ID 11
#define CHOICE2_ID 21
#define BAD_ENDING1 0
#define BAD_ENDING2 0


volatile alt_u32 *SDRAM_PTR = SDRAM_BASE;

static int scene_id = 0;
int backgrounds[SIZE] = {-1, 1, 2, 3, 4,-1};
int characters1[SIZE] = {-1,-1,-1,-1,-1, 1};
int characters2[SIZE] = {-1,-1,-1,-1,-1,-1};
int texts[SIZE] = {0, 1, 2, 5, 7, 11};
static int text_id = 0; //count the texts in a single scene

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
	/* Clear and draw a blue box on the screen */
	alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dev,0);
	alt_up_pixel_buffer_dma_clear_screen (pixel_buf_dev,1);
	printf("cleared screen\n");

	int address = 0;

	//draw background
	if (backgrounds[scene_id]!= -1){
		int bg_addr = backgrounds[scene_id]; //address of the background
		for (y=0; y<480; y++)
		{
			for (x =0; x<640; x++)
			{
				address = bg_addr + x + 640*y;
				alt_u32 data = SDRAM_PTR[address];
				alt_u16 color = (data | 0xFFFF00) >> 16;
				//printf("%d, %d, %x, %x\n", x,y,data,color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	//draw 1st character
	if (characters1[scene_id] != -1){
		int ch1_addr = characters1[scene_id];
		for (y = 50; y < 350; y++){ //range need to be changed
			for (x = 120; x < 300; x++){  //range need to be changed
				address = ch1_addr + x + 640*y;
				alt_u32 data = SDRAM_PTR[address];
				alt_u16 color = (data | 0xFFFF00) >> 16;
				//printf("%d, %d, %x, %x\n", x,y,data,color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	//draw 2nd character
	if (characters2[scene_id] != -1){
		int ch2_addr = characters2[scene_id];
		for (y = 50; y < 350; y++){
			for (x = 320; x < 500; x++){ //range needs change
				address = ch2_addr + x + 640*y;
				alt_u32 data = SDRAM_PTR[address];
				alt_u16 color = (data | 0xFFFF00) >> 16;
				//printf("%d, %d, %x, %x\n", x,y,data,color);
				alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
			}
		}
		alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
	}

	//draw text
	text_id = texts[scene_id];
	char cur_text[100];
	strcpy(cur_text, script[text_id]);
	display_text(cur_text, SDRAM_PTR);
	//read from keyboard
	alt_u16 keycode = keyboard();
	printf("\ncode = %x\n", keycode);

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
		text_id++;
		if (text_id >= texts[scene_id]){
			scene_id ++;
		}
	}

}

