//
// Created by Eric Fry on 8/11/2017.
//

#ifndef COSMO_ENGINE_STATUS_H
#define COSMO_ENGINE_STATUS_H

#include "defines.h"

#define STATUS_BAR_HEIGHT 6

#define STATUS_BAR_WIDTH 38

void status_load_tiles();

void status_panel_init();

void status_display();

void add_to_score_update_on_display(uint32 amount_to_add_low, int x_pos, int y_pos);

void update_health_bar_display();

void display_num_stars_collected();

void display_num_bombs_left();

#endif //COSMO_ENGINE_STATUS_H
