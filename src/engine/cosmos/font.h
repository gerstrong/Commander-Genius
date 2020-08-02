//
// cosmo_engine created on 18/Nov/2017.
//
#ifndef COSMO_ENGINE_FONT_H
#define COSMO_ENGINE_FONT_H

#include "defines.h"
#include "tile.h"

#define FONT_WHITE 0xf

extern Tile *font_tiles;

void font_load_tiles();

void display_number(int x_pos, int y_pos, uint32 number);
void display_char(int x_pos, int y_pos, char c, uint8 font_color);
void display_text(int x_pos, int y_pos, const char *text);
void display_text_with_color(int x_pos, int y_pos, const char *text, uint8 color);

#endif //COSMO_ENGINE_FONT_H
