//
// Created by efry on 3/11/2017.
//

#include "defines.h"

#ifndef COSMO_ENGINE_BACKDROP_H
#define COSMO_ENGINE_BACKDROP_H

//Data
extern uint8 background_x_scroll_flag;
extern uint8 background_y_scroll_flag;

bool set_backdrop(uint16 backdrop_index);

void backdrop_display();

#endif //COSMO_ENGINE_BACKDROP_H
