//
// Created by efry on 30/10/2017.
//

#ifndef COSMO_ENGINE_MAP_H
#define COSMO_ENGINE_MAP_H

#include "tile.h"

#define MAP_WINDOW_WIDTH 38
#define MAP_WINDOW_HEIGHT 18

//Data
extern uint16 current_level;
extern int map_width_in_tiles;
extern int map_height_in_tiles;
extern int map_stride_bit_shift_amt;

extern int mapwindow_x_offset;
extern int mapwindow_y_offset;

extern int map_max_y_offset;
extern uint8 rain_flag;

uint16 map_get_tile_cell(int x, int y);
void map_write_tile_cell(uint16 map_tile_cell, int x, int y);
void map_write_row_of_tiles(uint16 map_tile_cell, uint16 length_in_tiles, int x, int y);
void write_tile_row_to_tilemap(uint16 map_tile_cell, uint16 map_tile_cell2, uint16 map_tile_cell3, uint16 map_tile_cell4, int x, int y);

void load_level(int level_number);

void map_load_tiles();

void map_display();

Tile *map_get_bg_tile(uint16 tile_num);

uint8 map_get_tile_attr(int x, int y);

bool move_map_window(int dx, int dy);

#endif //COSMO_ENGINE_MAP_H
