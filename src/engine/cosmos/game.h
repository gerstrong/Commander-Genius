//
// Created by efry on 25/10/2017.
//

#ifndef COSMO_ENGINE_GAME_H
#define COSMO_ENGINE_GAME_H

#include <stdbool.h>
#include "defines.h"
#include "files/file.h"

typedef enum {
    PLAY_GAME=0,
    PLAY_DEMO=1,
    RECORD_DEMO=2,
    QUIT_GAME=3
} game_play_mode_enum;

extern game_play_mode_enum game_play_mode;
extern uint8 finished_level_flag_maybe;
extern uint8 finished_game_flag_maybe;
extern uint32 num_stars_collected;
extern uint8 knows_about_powerups_flag;
extern uint32 score;
extern uint8 cheat_mode_flag;

unsigned char *load_file(const char *filename, unsigned char *buf, uint32 buf_size);
unsigned char *load_file_in_new_buf(const char *filename, uint32 *file_size);
bool open_file(const char *filename, File *file);

void game_init();
void game_loop();
void set_initial_game_state();
void reset_game_state();

uint8 get_episode_number();
void set_episode_number(uint8 episode);

void enable_quick_start_mode();
bool is_quick_start();

#endif //COSMO_ENGINE_GAME_H
