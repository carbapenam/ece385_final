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
void display_text(char text[], volatile alt_u32 *FONT_BASE_ADDRESS, alt_up_pixel_buffer_dma_dev * pixel_buf_dev);
void copy_str(char* dest, alt_u32 *src, int length_in_32_bits);

static alt_u8 n_background = 0;
static alt_u8 n_chara = 0;
static alt_u8 n_font = 0;
static alt_u8 n_text = 0;

struct scene
{
	alt_8 scene_id;
	alt_8 background_id;
	alt_8 character_id;
	alt_16 text_id_begin;
	alt_16 text_id_end;
} scenes[60];

struct character
{
	alt_8 character_id;
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
	alt_8 background_id;
	alt_u16 width;
	alt_u16 height;
	alt_u32 address;
} backgrounds[10];

struct text
{
	alt_8 text_id;
	alt_u8 length;
	alt_u32 address;
} texts[180];

void init_scenes();

#endif /* HELPER_FUNCTIONS_H_ */
