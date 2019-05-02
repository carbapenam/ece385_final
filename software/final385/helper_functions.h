/*
 * helper_functions.h
 *
 *  Created on: Apr 29, 2019
 *      Author: wsy
 */

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_



void display_text(char text[], volatile alt_u32 * SDRAM_PTR);

struct scene
{
	alt_u8 scene_id;
	alt_u8 background_id;
	alt_u8 character_id;
	alt_u8 text_id_begin;
	alt_u8 text_id_end;
};

struct character
{
	alt_u8 character_id;
	alt_u16 width;
	alt_u16 height;
	alt_u16 offset_x;
	alt_u16 offset_y;
};

struct scene scenes[60];

struct character charas[10];

void init_scenes();


#endif /* HELPER_FUNCTIONS_H_ */
