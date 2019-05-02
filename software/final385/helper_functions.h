/*
 * helper_functions.h
 *
 *  Created on: Apr 29, 2019
 *      Author: wsy
 */

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

void test_assets(int offset);
alt_u32 populate_structs();
void display_text(char text[], volatile alt_u32 * SDRAM_PTR);

static alt_u8 n_background = 0;
static alt_u8 n_chara = 0;
static alt_u8 n_font = 0;
static alt_u8 n_text = 0;

struct scene
{
	alt_u8 scene_id;
	alt_u8 background_id;
	alt_u8 character_id;
	alt_u8 text_id_begin;
	alt_u8 text_id_end;
} scenes[60];

struct character
{
	alt_u8 character_id;
	alt_u16 width;
	alt_u16 height;
	alt_u16 offset_x;
	alt_u16 offset_y;
	alt_u32 address;
} charas[10];

struct font
{
	alt_u16 width;
	alt_u16 height;
	alt_u32 address;
} font;

struct background
{
	alt_u8 background_id;
	alt_u16 width;
	alt_u16 height;
	alt_u32 address;
} backgrounds[10];

struct text
{
	alt_u8 text_id;
	alt_u8 length;
	alt_u32 address;
} texts[180];

void init_scenes();

#endif /* HELPER_FUNCTIONS_H_ */
