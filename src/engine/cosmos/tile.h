//
// Created by Eric Fry on 31/10/2017.
//

#ifndef COSMO_ENGINE_TILE_H
#define COSMO_ENGINE_TILE_H

#include "defines.h"
typedef enum {
    SOLID,
    TRANSPARENT,
    FONT
} TileType;

enum TileAttr
{
    BLOCK_DOWN = 0x1,
    BLOCK_UP = 0x2,
    BLOCK_LEFT = 0x4,
    BLOCK_RIGHT = 0x8,
    SLIPPERY = 0x10,
    IN_FRONT = 0x20,
    SLOPED = 0x40,
    CAN_GRAB_WALL = 0x80,
};

typedef struct {
    TileType type;
    uint8 pixels[64];
} Tile;

typedef struct {
    uint16 height;
    uint16 width;
    uint16 tile_num;
} TileInfo;

typedef struct {
    uint16 num_frames;
    TileInfo *frames;
} Sprite;

#define TILE_ATTR_NONE              0u
#define TILE_ATTR_BLOCK_DOWN      0x1u
#define TILE_ATTR_BLOCK_UP        0x2u
#define TILE_ATTR_BLOCK_LEFT      0x4u
#define TILE_ATTR_BLOCK_RIGHT     0x8u
#define TILE_ATTR_SLIPPERY       0x10u
#define TILE_ATTR_IN_FRONT       0x20u
#define TILE_ATTR_SLOPED         0x40u
#define TILE_ATTR_CAN_GRAB_WALL  0x80u

#define TILE_HEIGHT 8
#define TILE_WIDTH 8

#define TRANSPARENT_COLOR 255

extern uint8 tileattr_mni_data[7000];

void tile_attr_load();

Tile *load_tiles(const char *filename, TileType type, uint16 *num_tiles_loaded);

Sprite *load_tile_info(const char *filename, uint16 *num_records_loaded);

#endif //COSMO_ENGINE_TILE_H
