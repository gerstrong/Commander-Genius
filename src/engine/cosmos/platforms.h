//
// cosmo_engine created on 27/Nov/2017.
//
#ifndef COSMO_ENGINE_PLATFORMS_H
#define COSMO_ENGINE_PLATFORMS_H

#include "defines.h"

typedef struct MovingPlatform
{
    uint16 x;
    uint16 y;
    uint16 map_tiles[5];
} MovingPlatform;

typedef struct MudFountain
{
    uint16 x;
    uint16 y;
    uint16 direction;
    uint16 length_counter;
    uint16 current_height;
    uint16 max_height;
    uint16 pause_counter;
} MudFountain;

extern uint16 num_moving_platforms;
extern uint16 num_mud_fountains;
extern MovingPlatform moving_platform_tbl[10];
extern MudFountain mud_fountain_tbl[10];

void update_mud_fountains();
void display_mud_fountains();

void update_moving_platforms();

#endif //COSMO_ENGINE_PLATFORMS_H
