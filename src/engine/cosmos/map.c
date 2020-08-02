//
// Created by efry on 30/10/2017.
//

#include <stdio.h>
#include "files/file.h"
#include <assert.h>
#include "input.h"
#include "backdrop.h"
#include "game.h"
#include "video.h"
#include "map.h"
#include "sound/music.h"
#include "dialog.h"
#include "palette.h"
#include "player.h"
#include "save.h"
#include "actor.h"
#include "tile.h"
#include "actor_toss.h"
#include "effects.h"
#include "status.h"
#include "platforms.h"
#include "fullscreen_image.h"

#define MAX_MAP_TILES 32768
//Data
uint16 current_level;

int map_width_in_tiles = 0;
int map_height_in_tiles = 0;
int map_stride_bit_shift_amt;
int mapwindow_x_offset = 0;
int mapwindow_y_offset = 0;
int map_max_y_offset = 0;

uint8 rain_flag = 0;

uint16 map_data[MAX_MAP_TILES];

Tile *map_bg_tiles;
Tile *map_fg_tiles;

uint16 map_get_tile_cell(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);

    if (x + y * map_width_in_tiles >= MAX_MAP_TILES)
    {
        return 0;
    }

    return map_data[x + y * map_width_in_tiles];
}

const char level_filename_ep1_tbl[][13] = {
        "A1.MNI",
        "A2.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A3.MNI",
        "A4.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A5.MNI",
        "A6.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A7.MNI",
        "A8.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A9.MNI",
        "A10.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A11.MNI",
        "A12.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A13.MNI",
        "A14.MNI",
        "BONUS1.MNI",
        "BONUS2.MNI",
        "A15.MNI",
        "A16.MNI"
};

const char level_filename_ep2_tbl[][13] = {
        "B1.MNI",
        "B2.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B3.MNI",
        "B4.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B5.MNI",
        "B6.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B7.MNI",
        "B8.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B9.MNI",
        "B10.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B11.MNI",
        "B12.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B13.MNI",
        "B14.MNI",
        "BONUS3.MNI",
        "BONUS4.MNI",
        "B15.MNI",
        "B16.MNI"
};

const char level_filename_ep3_tbl[][13] = {
        "C1.MNI",
        "C2.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C3.MNI",
        "C4.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C5.MNI",
        "C6.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C7.MNI",
        "C8.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C9.MNI",
        "C10.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C11.MNI",
        "C12.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C13.MNI",
        "C14.MNI",
        "BONUS5.MNI",
        "BONUS6.MNI",
        "C15.MNI",
        "C16.MNI"
};

void load_level_data(int level_number);

const char *get_level_filename(int level_number)
{
    switch(get_episode_number())
    {
        case 1 : return level_filename_ep1_tbl[level_number];
        case 2 : return level_filename_ep2_tbl[level_number];
        case 3 : return level_filename_ep3_tbl[level_number];
        default : break;
    }
    return NULL;
}

void load_level(int level_number)
{
    if (level_number != 0 || show_one_moment_screen_flag == 0) {
        fade_to_black_speed_3();
    } else {
        display_fullscreen_image(5);
        wait_for_time_or_key(0x12c);
    }

    File map_file;
    if(!open_file(get_level_filename(level_number), &map_file))
    {
        printf("Error: loading level data. %s\n", get_level_filename(level_number));
        return;
    }

    uint16 level_flags = file_read2(&map_file);
    file_close(&map_file);

    stop_music();
    rain_flag = (uint8)(level_flags & 0x20);
    uint16 backdrop_index = (uint16)(level_flags & 0x1f);
    background_x_scroll_flag = (uint8)(level_flags & 0x40);
    background_y_scroll_flag = (uint8)(level_flags & 0x80);

    palette_anim_type = (uint8)(level_flags >> 8 & 7);
    uint16 music_index = (uint16)((level_flags >> 11) & 0x1f);

    printf("Level %d: rain=%d, backdrop_index=%d, bg_x_scroll=%d, bg_y_scroll=%d, pal_anim_type=%d, music_index=%d\n", current_level, rain_flag, backdrop_index, background_x_scroll_flag, background_y_scroll_flag, palette_anim_type, music_index);
    reset_game_state();

    set_backdrop(backdrop_index);

    load_level_data(level_number);

    if (level_number == 0 && show_one_moment_screen_flag != 0) {
        fade_to_black_speed_3();
        show_one_moment_screen_flag = 0;
    }
    if (game_play_mode == 0) {
        switch (level_number) {
            case 0:
            case 1:
            case 4:
            case 5:
            case 8:
            case 9:
            case 12:
            case 13:
            case 16:
            case 17:
                video_fill_screen_with_black();
                fade_in_from_black_with_delay_3();
                now_entering_level_n_dialog(level_number);
                wait_for_time_or_key(0x96);
                fade_to_black_speed_3();
                break;
            default:
                break;
        }
    }
    explode_effect_clear_sprites();
    struct6_clear_sprites();
    effect_clear_sprites();
    player_reset_push_variables();
    actor_toss_clear_all();
    status_panel_init();

    write_savegame_file('T');
    load_music(music_index);

    //I Don't think this will be needed.

//    if (byte_32FEA == 0) {
//        gvar_32D12 = REGISTER_29;
//        tileattr_mni_data = &ptr + 0x1388;
//        word_28BE4 = 0x1111;
//        load_tileattr_mni("TILEATTR.MNI", REGISTER_32);
//    }

    fade_in_from_black_with_delay_3();
}

void load_level_data(int level_number)
{
    byte_28BE3 = 0;
    File map_file;
    if(!open_file(get_level_filename(level_number), &map_file))
    {
        printf("Error: loading level data. %s\n", get_level_filename(level_number));
        return;
    }

    file_seek(&map_file, 2);
    map_width_in_tiles = file_read2(&map_file);
    map_height_in_tiles = 32768 / map_width_in_tiles - 1;

    printf("map width (in tiles): %d\n", map_width_in_tiles);

    switch(map_width_in_tiles)
    {
        case 32: map_stride_bit_shift_amt = 5; break;
        case 64: map_stride_bit_shift_amt = 6; break;
        case 128: map_stride_bit_shift_amt = 7; break;
        case 256: map_stride_bit_shift_amt = 8; break;
        case 512: map_stride_bit_shift_amt = 9; break;
        case 1024: map_stride_bit_shift_amt = 10; break;
        case 2048: map_stride_bit_shift_amt = 11; break;
        default: break;
    }


    uint16 actor_data_size_in_words = file_read2(&map_file);

    total_num_actors = 0;
    num_moving_platforms = 0;
    num_mud_fountains = 0;
    clear_brightness_objs();
    brightness_effect_enabled_flag = 1;
    obj_switch_151_flag = 0;

    for(int i=0;i< actor_data_size_in_words/3;i++)
    {
        uint16 actor_type = file_read2(&map_file);
        uint16 x = file_read2(&map_file);
        uint16 y = file_read2(&map_file);
        if(total_num_actors < MAX_ACTORS)
        {
            load_actor(total_num_actors, actor_type, x, y);
        }
    }

    for(int i=0;i < MAX_MAP_TILES - 4; i++)
    {
        map_data[i] = file_read2(&map_file);
    }

    for(int i=0;i<num_moving_platforms;i++)
    {
        for(int j = 2;j < 7;j++)
        {
            moving_platform_tbl[i].map_tiles[j-2] = map_data[moving_platform_tbl[i].x + moving_platform_tbl[i].y * map_width_in_tiles + j - 4];
        }

    }

    current_level = (uint16)level_number;
    map_max_y_offset = 0x10000 / (map_width_in_tiles * 2) - 19;
}

void map_load_tiles()
{
    uint16 num_tiles;
    map_bg_tiles = load_tiles("TILES.MNI", SOLID, &num_tiles);
    printf("Loading %d map bg tiles.\n", num_tiles);
    map_fg_tiles = load_tiles("MASKTILE.MNI", TRANSPARENT, &num_tiles);
    printf("Loading %d map fg tiles.\n", num_tiles);
}

void map_display() {
    if(mapwindow_y_offset > map_max_y_offset)
        mapwindow_y_offset = map_max_y_offset;

    backdrop_display();
    for(int y=0; y < MAP_WINDOW_HEIGHT; y++)
    {
        for(int x=0; x < MAP_WINDOW_WIDTH; x++)
        {
            uint16 map_cell = map_data[(x+mapwindow_x_offset) + (y+mapwindow_y_offset) * map_width_in_tiles];
            if(map_cell < 16000)
            {
                uint16 tile = map_cell/8;
                if(tile > 9) // The first 10 tiles aren't shown. They are arrows for moving platform debug.
                {
                    video_draw_tile(&map_bg_tiles[tile], (x+1)*8, (y+1)*8);
                }
            }
            else
            {
                uint16 tile =  ((map_cell/8) - 2000) / 5;
                video_draw_tile(&map_fg_tiles[tile], (x+1)*8, (y+1)*8);
            }
        }
    }
}

void map_write_tile_cell(uint16 map_tile_cell, int x, int y)
{
    map_data[x + y * map_width_in_tiles] = map_tile_cell;
}

void map_write_row_of_tiles(uint16 map_tile_cell, uint16 length_in_tiles, int x, int y) {
    assert(x + length_in_tiles < map_width_in_tiles);
    for(int i = 0; i < length_in_tiles; i++)
    {
        map_write_tile_cell(map_tile_cell, x + i, y);
    }
}

void write_tile_row_to_tilemap(uint16 map_tile_cell,
                               uint16 map_tile_cell2,
                               uint16 map_tile_cell3,
                               uint16 map_tile_cell4,
                               int x,
                               int y)
{
    map_write_tile_cell(map_tile_cell, x, y);
    map_write_tile_cell(map_tile_cell2, x + 1, y);
    map_write_tile_cell(map_tile_cell3, x + 2, y);
    map_write_tile_cell(map_tile_cell4, x + 3, y);
}

Tile *map_get_bg_tile(uint16 tile_num) {
    return &map_bg_tiles[tile_num];
}

uint8 map_get_tile_attr(int x, int y) {

    if (x < 0 || y < 0 || x >= map_width_in_tiles || x + y * map_width_in_tiles >= MAX_MAP_TILES) {
        return TILE_ATTR_NONE;
    }

    return tileattr_mni_data[map_get_tile_cell(x, y) / 8];
}

bool move_map_window(int dx, int dy) {
    int new_x = mapwindow_x_offset + dx;
    int new_y = mapwindow_y_offset + dy;

    if (new_x < 0 || new_y < 0
    || new_x + MAP_WINDOW_WIDTH >= map_width_in_tiles
    || new_y + MAP_WINDOW_HEIGHT >= map_height_in_tiles) {
        return false;
    }

    mapwindow_x_offset = new_x;
    mapwindow_y_offset = new_y;

    return true;
}
