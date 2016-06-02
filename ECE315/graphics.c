#include "graphics.h"
#include <stdio.h>

void write_image_to_screen(image* img, uint8_t x, uint8_t y, bool reversed) {
	uint8_t i, j;
	
	for( i = 0; i < img->height; i++ ) {
	
		ece315_lcdSetPage( i + y );
		for( j = 0; j < img->width; j++ ) {
		
			if ( reversed )
			  ece315_lcdSetColumn(x - j);
			else
			  ece315_lcdSetColumn(x + j);
			
			// ERROR HERE WITH WORLD MAP PRINTING
			//printf("Img width = %d, height = %d, index = %d, ptr at %d\n\r", img->width, img->height, j + img->width * i, (uint32_t)(img->pixels));
			//printf("Img width = %d, height = %d, index = %d, ptr at %d\n\r", img->width, img->height, j + img->width * i, (uint32_t)(img->pixels));
			//printf("Img width = %d, height = %d, index = %d, ptr at %d\n\r", img->width, img->height, j + img->width * i, (uint32_t)(img->pixels));
			//printf("Img width = %d, height = %d, index = %d, ptr at %d\n\r", img->width, img->height, j + img->width * i, (uint32_t)(img->pixels));
			ece315_lcdWriteData(img->pixels[j + img->width * i]);
		}
	}
}

void write_offset_image_to_screen( offset_image* img, uint8_t x, uint8_t y, bool reversed ) {
	if ( reversed )
		write_image_to_screen( img->image, x - img->off_x, y + img->off_y, reversed );
	else
		write_image_to_screen( img->image, x + img->off_x, y + img->off_y, reversed );
}

void clear_image( image* img ) {
	uint8_t i, j;
	for( i = 0; i < img->width; i++ )
	  for( j = 0; j < img->height; j++ )
	    img->pixels[j * img->width + i] = 0x00;
}

void clear_offset_image( offset_image* img ) {
	clear_image( img->image );
}

const uint8_t LCD_CHAR_PIXELS[][4] = {
	// A f
	{ 0xF0, 0x48, 0x48, 0xF0 },
	// B f
	{ 0xF8, 0xA8, 0xA8, 0x50 },
	// C f
	{ 0x70, 0x88, 0x88, 0x50 },
	// D f
	{ 0xF8, 0x88, 0x88, 0x70 },
	// E f
	{ 0xF8, 0xA8, 0xA8, 0x88 },
	// F f
	{ 0xF8, 0x28, 0x28, 0x08 },
	// G f
	{ 0x70, 0x88, 0xA8, 0xE8 },
	// H f
	{ 0xF8, 0x20, 0x20, 0xF8 },
	// I f
	{ 0x88, 0xF8, 0x88, 0x00 },
	// J f
	{ 0x88, 0x88, 0x88, 0x78 },
	// K f
	{ 0xF8, 0x20, 0x50, 0x88 },
	// L f
	{ 0xF8, 0x80, 0x80, 0x00 },
	// M f
	{ 0xF8, 0x10, 0x10, 0xF8 },
	// N f
	{ 0xF8, 0x10, 0x20, 0xF8 },
	// O f
	{ 0x70, 0x88, 0x88, 0x70 },
	// P f
	{ 0xF8, 0x48, 0x48, 0x30 },
	// Q f
	{ 0x70, 0x88, 0x48, 0xB0 },
	// R f
	{ 0xF8, 0x48, 0x48, 0xB0 },
	// S f
	{ 0x90, 0xA8, 0xA8, 0x48 },
	// T f
	{ 0x08, 0xF8, 0x08, 0x00 },
	// U f
	{ 0x78, 0x80, 0x80, 0x78 },
	// V f
	{ 0x78, 0x80, 0x60, 0x18 },
	// W f
	{ 0xF8, 0x40, 0x40, 0xF8 },
	// X f
	{ 0xD8, 0x20, 0x20, 0xD8 },
	// Y f
	{ 0x98, 0xA0, 0xA0, 0x78 },
	// Z f
	{ 0xC8, 0xA8, 0xA8, 0x98 },
	//   f
	{ 0x00, 0x00, 0x00, 0x00 },
	// . f
	{ 0x80, 0x00, 0x00, 0x00 },
	// ? f
	{ 0x10, 0x08, 0xA8, 0x10 },
	// ! f
	{ 0xB8, 0x00, 0x00, 0x00 },
	// , f
	{ 0xC0, 0x00, 0x00, 0x00 }
	
};

void write_char_to_image( image* img, uint8_t* pix, uint8_t* left, uint8_t* top ) {
	uint8_t w, i;
	uint8_t uPage, lPage, uPageH, lPageH;
	for( w = 0; w < 4 && pix[w] != 0; w++ );
	if ( *left + w >= img->width ) {
		(*left) = 0;
		(*top) += 6;
		if ( *top + 5 > 8 * img->height )
			img->height++;
	}
	
	uPage = (*top) >> 3;
	uPageH = (*top) % 8;
	lPage = uPage + 1;
	lPageH = 11 - uPageH;
	
	for( i = 0; i < w ; i++ )
	  if ( uPageH <= 3 )
		 img->pixels[*left + i + img->width * uPage] |= (uint8_t)( pix[i] >> (3 - uPageH) );
		else
			img->pixels[*left + i + img->width * uPage] |= (uint8_t)( pix[i] << (uPageH - 3) );
	if ( lPageH < 8 ) {
		for( i = 0; i < w ; i++ )
			img->pixels[*left + i + img->width * lPage] |= (uint8_t)( pix[i] >> lPageH );
	}
	
	(*left) += (w + 1);
	
}


void write_string_to_image( char* str, image* img ) {
	uint8_t curTop = 0;
	uint8_t curLeft = 0;
	char* c;
	
	for( c = str; *c; c++ ) {
		if ( *c >= 'A' && *c <= 'Z' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[(*c) - 'A'], &curLeft, &curTop );
		else if ( *c >= 'a' && *c <= 'z' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[(*c) - 'a'], &curLeft, &curTop );
		else if ( *c == ' ' ) {
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[26], &curLeft, &curTop );
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[26], &curLeft, &curTop );
		} else if ( *c == '.' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[27], &curLeft, &curTop );
		else if ( *c == '?' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[28], &curLeft, &curTop );
		else if ( *c == '!' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[29], &curLeft, &curTop );
		else if ( *c == ',' )
			write_char_to_image( img, (uint8_t*)&LCD_CHAR_PIXELS[30], &curLeft, &curTop );
	}
	
}
