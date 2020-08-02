//
// cosmo_engine created on 12/Nov/2017.
//
#ifndef COSMO_ENGINE_EFFECTS_H
#define COSMO_ENGINE_EFFECTS_H


#include "defines.h"

void struct6_add_sprite(int x_pos, int y_pos);
int struct6_1B4FC(int actorInfoIndex, int frame_num, int x_pos, int y_pos);
void struct6_update_sprites();
void struct6_clear_sprites();

int blow_up_actor_with_bomb(int actorInfoIndex, int frame_num, int x_pos, int y_pos);
void effect_add_sprite(int actorInfoIndex, int frame_num, int x_pos, int y_pos, int arg_8, int counter);

void effect_update_sprites();
void effect_clear_sprites();

void explode_effect_add_sprite(int actorInfoIndex, int frame_num, int x_pos, int y_pos);
void explode_effect_update_sprites();
void explode_effect_clear_sprites();

void exploding_balls_effect(int x_pos, int y_pos);

void update_rain_effect();

void add_brightness_obj(uint8 type, int x_pos, int y_pos);

void update_brightness_objs();
void clear_brightness_objs();

#endif //COSMO_ENGINE_EFFECTS_H
