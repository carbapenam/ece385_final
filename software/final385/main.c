#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "system.h"
#include "alt_types.h"
#include <unistd.h>  // usleep
#include "sys/alt_irq.h"

#define pi 3.14159

volatile unsigned int * VGA_PTR = (unsigned int *) 0x00000020;

int main()
{
	int x;
	int y;
	int sec = 1;
	double dx,dy,dv;
	int counter = 0;

	int color = 0;

	while(1)
	{
		x = VGA_PTR[3];
		y = VGA_PTR[4];

		if (y % 40 == 0)
		{
			color++;
			color = color % 3;
		}

		if (color == 0)
		{
			VGA_PTR[0] = 255;
			VGA_PTR[1] = 0;
			VGA_PTR[2] = 0;
		}

		else if (color == 1)
		{
			VGA_PTR[0] = 0;
			VGA_PTR[1] = 255;
			VGA_PTR[2] = 0;
		}


		else if (color == 2)
		{
			VGA_PTR[0] = 0;
			VGA_PTR[1] = 0;
			VGA_PTR[2] = 255;
		}
	}
}
