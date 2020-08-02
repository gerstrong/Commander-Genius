//
// Created by efry on 4/11/2017.
//

#ifndef COSMO_ENGINE_SAVE_H
#define COSMO_ENGINE_SAVE_H

#include "defines.h"

typedef struct SaveGameData {
    uint8 health;
    uint32 score;
    uint16 num_stars_collected;
    uint16 current_level;
    uint16 num_bombs;
    uint8 num_health_bars;
    uint16 cheats_used_flag;
    uint16 has_had_bomb_flag;
    uint8 show_monster_attack_hint;
    uint8 knows_about_powerups_flag;
} SaveGameData;

typedef enum SaveStatus {
    LOADED,
    FILE_IO_ERROR,
    CRC_ERROR
} SaveStatus;

//Data
extern uint16 cheats_used_flag;

void write_savegame_file(char suffix);
bool load_savegame_file(char suffix);
SaveStatus load_savegame_data_from_file(char suffix, SaveGameData *data);
#endif //COSMO_ENGINE_SAVE_H
