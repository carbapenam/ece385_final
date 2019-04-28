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

#include "io_handler.h"
#include "cy7c67200.h"
#include "usb.h"
#include "lcp_cmd.h"
#include "lcp_data.h"

#define pi 3.14159

volatile alt_u32 *SDRAM_PTR = SDRAM_BASE;

void keyboard();

int main()
{
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

for (int y=0; y<480; y++)
{
	for (int x =0; x<640; x++)
	{
		address = x + 640*y;
		alt_u32 data = SDRAM_PTR[address];
		alt_u16 color = (data | 0xFFFF00) >> 16;
		//printf("%d, %d, %x, %x\n", x,y,data,color);
		alt_up_pixel_buffer_dma_draw(pixel_buf_dev, color, x, y);
	}
}
alt_up_pixel_buffer_dma_swap_buffers(pixel_buf_dev);
}

