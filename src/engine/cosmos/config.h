//
// cosmo_engine created on 31/Dec/2017.
//
#ifndef COSMO_ENGINE_CONFIG_H
#define COSMO_ENGINE_CONFIG_H

#include <SDL_keycode.h>
#include "defines.h"

void load_config_file();
void write_config_file();

uint8 keycode_to_scancode(SDL_Keycode keycode);
const char *scancode_to_string(uint8 scan_code);

void load_config_from_command_line(int argc, char *argv[]);

char *get_data_dir_full_path(const char *filename);
char *get_save_dir_full_path(const char *filename);
char *get_game_dir_full_path(const char *filename);

void config_cleanup();

#endif //COSMO_ENGINE_CONFIG_H
