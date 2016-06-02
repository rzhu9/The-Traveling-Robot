#ifndef __ECE353_GRAPHICS__
#define __ECE353_GRAPHICS__

#include "boardUtil.h"
#include "lcd.h"

extern const uint8_t LCD_CHAR_PIXELS[][4];

typedef struct lcd_char{
	uint8_t* pixels;
	uint8_t width;
} lcd_char;

typedef struct image {
	uint8_t* pixels;
	uint8_t width;
	uint8_t height;
} image;

typedef struct offset_image {
  uint8_t off_x, off_y;
  image* image;
} offset_image;


void write_image_to_screen( image* img, uint8_t x, uint8_t y, bool reverse );
void write_offset_image_to_screen( offset_image* img, uint8_t x, uint8_t y, bool reverse );

void clear_image( image* img );
void clear_offset_image( offset_image* img );
void write_string_to_image( char* str, image* img );

#endif
