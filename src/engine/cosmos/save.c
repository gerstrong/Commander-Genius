//
// Created by efry on 4/11/2017.
//

#include <stdlib.h>
#include "save.h"
#include "defines.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "dialog.h"
#include "config.h"

int cleanup_and_exit();

static SaveGameData temp_save;

uint16 cheats_used_flag = 0;

void write_savegame_file(char suffix)
{
    if(suffix == 'T')
    {
        temp_save.health = health;
        temp_save.score = score;
        temp_save.num_stars_collected = num_stars_collected;
        temp_save.current_level = current_level;
        temp_save.num_bombs = num_bombs;
        temp_save.num_health_bars = num_health_bars;
        temp_save.cheats_used_flag = cheats_used_flag;
        temp_save.has_had_bomb_flag = has_had_bomb_flag;
        temp_save.show_monster_attack_hint = show_monster_attack_hint;
        temp_save.knows_about_powerups_flag = knows_about_powerups_flag;
    }
    else
    {
        char filename[13];
        char *path;
        sprintf(filename, "COSMO%d.SV%c", get_episode_number(), suffix);
        path = get_save_dir_full_path(filename);
        File savefile;
        if(!file_open(path, "wb", &savefile))
        {
            printf("Error: saving file %s\n", path);
            free(path);
            return;
        }
        free(path);

        file_write2(health, &savefile);
        file_write4(score, &savefile);
        file_write2(num_stars_collected, &savefile);
        file_write2(current_level, &savefile);
        file_write2(num_bombs, &savefile);
        file_write2(num_health_bars, &savefile);
        file_write2(cheats_used_flag, &savefile);
        file_write2(has_had_bomb_flag, &savefile);
        file_write2(show_monster_attack_hint, &savefile);
        file_write2(knows_about_powerups_flag, &savefile);
        uint16 checksum = (uint16)(health + num_stars_collected + current_level + num_bombs + num_health_bars);
        file_write2(checksum, &savefile);

        file_close(&savefile);
    }
}

bool load_savegame_file(char suffix)
{
    if(suffix == 'T')
    {
         health = temp_save.health;
         score = temp_save.score;
         num_stars_collected = temp_save.num_stars_collected;
         current_level = temp_save.current_level;
         num_bombs = temp_save.num_bombs;
         num_health_bars = temp_save.num_health_bars;
         cheats_used_flag = temp_save.cheats_used_flag;
//         has_had_bomb_flag = temp_save.has_had_bomb_flag;
//         show_monster_attack_hint = temp_save.show_monster_attack_hint;
//         knows_about_powerups_flag = temp_save.knows_about_powerups_flag;
    }
    else
    {
        SaveGameData data;
        SaveStatus status = load_savegame_data_from_file(suffix, &data);

        if(status == FILE_IO_ERROR)
        {
            return false;
        }

        if(status == CRC_ERROR)
        {
            malformed_savegame_dialog();
            exit(cleanup_and_exit());
        }

        health = data.health;
        score = data.score;
        num_stars_collected = data.num_stars_collected;
        current_level = data.current_level;
        num_bombs = data.num_bombs;
        num_health_bars = data.num_health_bars;
        cheats_used_flag = data.cheats_used_flag;
        has_had_bomb_flag = data.has_had_bomb_flag;
        show_monster_attack_hint = data.show_monster_attack_hint;
        knows_about_powerups_flag = data.knows_about_powerups_flag;
    }

    return true;
}

SaveStatus load_savegame_data_from_file(char suffix, SaveGameData *data) {
    File file;
    char filename[13];
    char *path;
    sprintf(filename, "COSMO%d.SV%c", get_episode_number(), suffix);

    path = get_save_dir_full_path(filename);
    int status = file_open(path, "rb", &file);
    free(path);

    if(!status)
    {
        return FILE_IO_ERROR;
    }

    data->health = (uint8)file_read2(&file);
    data->score = file_read4(&file);
    data->num_stars_collected = file_read2(&file);
    data->current_level = file_read2(&file);
    data->num_bombs = file_read2(&file);
    data->num_health_bars = (uint8)file_read2(&file);
    data->cheats_used_flag = file_read2(&file);
    data->has_had_bomb_flag = file_read2(&file);;
    data->show_monster_attack_hint = (uint8)file_read2(&file);;
    data->knows_about_powerups_flag = (uint8)file_read2(&file);;

    uint16 checksum = file_read2(&file);

    file_close(&file);

    if(checksum != data->health + data->num_stars_collected + data->current_level + data->num_bombs + data->num_health_bars)
    {
        return CRC_ERROR;
    }

    return LOADED;
}
