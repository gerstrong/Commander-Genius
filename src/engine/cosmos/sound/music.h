//
// Created by efry on 3/11/2017.
//

#ifndef COSMO_ENGINE_MUSIC_H
#define COSMO_ENGINE_MUSIC_H

#include "defines.h"

//Data
extern uint8 music_on_flag;

void music_init();
void load_music(uint16 new_music_index);
void stop_music();
void play_music();


#endif //COSMO_ENGINE_MUSIC_H
