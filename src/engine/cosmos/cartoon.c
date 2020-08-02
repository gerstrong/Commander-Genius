//
// Created by efry on 27/12/2017.
//

#include<stdio.h>
#include "cartoon.h"
#include "tile.h"
#include "video.h"

Tile *cartoon_tiles;
Sprite *sprites;

void load_cartoon_images()
{
    uint16 num_tile_info_records;
    uint16 num_tiles;
    cartoon_tiles = load_tiles("CARTOON.MNI", TRANSPARENT, &num_tiles);
    printf("Loading %d cartoon tiles.\n", num_tiles);

    sprites = load_tile_info("CARTINFO.MNI", &num_tile_info_records);
    printf("Loading %d, cartoon tile info records.\n", num_tile_info_records);
}

void display_cartoon_sequence(uint16 frame_num, uint16 x_pos, uint16 y_pos)
{
    TileInfo *info = &sprites[0].frames[frame_num];
    Tile *tile = &cartoon_tiles[info->tile_num];

    for(int y=0; y < info->height; y++)
    {
        for(int x=0; x < info->width; x++)
        {
            video_draw_tile(tile, (x_pos + x) * 8, (y_pos + y - info->height + 1) * 8);
            tile++;
        }
    }
}