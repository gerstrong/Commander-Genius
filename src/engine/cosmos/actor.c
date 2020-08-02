#include <stdio.h>
#include <assert.h>
#include "actor.h"
#include "defines.h"
#include "map.h"
#include "player.h"
#include "tile.h"
#include "actor_worktype.h"
#include "effects.h"
#include "video.h"
#include "actor_collision.h"
#include "platforms.h"

//Data
ActorData actors[MAX_ACTORS];
int total_num_actors = 0;

uint16 word_2E17E = 0;
bool speech_bubble_hamburger_shown_flag = false;
bool speech_bubble_red_plant_shown_flag = false;
bool speech_bubble_switch_61_shown_flag = false;
bool speech_bubble_clam_trap_shown_flag = false;
bool speech_bubble_silver_robot_shown_flag = false;
bool speech_bubble_purple_boss_shown_flag = false;
bool speech_bubble_pipe_shown_flag = false;
bool speech_bubble_teleporter_shown_flag = false;
bool speech_bubble_hoverboard_shown_flag = false;
bool speech_bubble_rubber_wall_shown_flag = false;
bool speech_bubble_floor_spring_shown_flag = false;

uint8 byte_32EB8 = 0;

uint16 word_32EC2 = 0; //range 0 - 35

uint8 energy_beam_enabled_flag = 0;

uint16 num_containers = 0;

uint16 num_eye_plants_remaining_to_blow_up = 0;

uint16 num_moving_platforms = 0;

uint16 brightness_effect_enabled_flag = 0;
uint16 obj_switch_151_flag = 0;


uint8 move_platform_flag = 0;
uint16 actor_tile_display_func_index = 0;


Tile *actor_tiles;
Sprite *actor_sprites;


const uint16 word_28D9C[] = {
        31, 12, 17, 233,
        99, 8, 64, 12,
        199, 49, 5, 6,
        143, 1, 35, 46,
        52, 5, 8, 21,
        44, 8, 3, 77,
        2, 103, 34, 23,
        78, 2, 67, 2,
        79, 46, 1, 98
};

//TODO work out what this should be called.
uint16 sub_1106F()
{
    word_32EC2++;
    if(word_32EC2 > 0x23)
    {
        word_32EC2 = 0;
    }
    return word_28D9C[word_32EC2] + mapwindow_x_offset + mapwindow_y_offset + word_32EC2 + player_x_pos + player_y_pos;
}

void actor_add_new(int image_index, int x_pos, int y_pos)
{
    for (int i = 0; i < total_num_actors; i++)
    {
        ActorData *actor = &actors[i];
        if (actor->is_deactivated_flag_maybe != 0)
        {
            actor_init(i, image_index, x_pos, y_pos);
            if (image_index == 0x56)
            {
                actor->update_while_off_screen_flag = 1;
            }
            return;
        }
    }

    if (total_num_actors < MAX_ACTORS)
    {
        ActorData *actor = &actors[total_num_actors];
        actor_init(total_num_actors, image_index, x_pos, y_pos);
        if (image_index == 0x56)
        {
            actor->update_while_off_screen_flag = 1;
        }
        total_num_actors = total_num_actors + 1;
    }

    return;
}

void actor_init_struct(int actor_init_cur_actor_num,
                       int actorInfoIndex,
                       int x,
                       int y,
                       int update_while_offscreen,
                       int can_update_if_goes_offscreen,
                       int can_fall_down,
                       int is_non_blocking,
                       void (*update_function)(ActorData *),
                       int data_1,
                       int data_2,
                       int data_3,
                       int data_4,
                       int data_5)
{
    if(data_2 == 0x1e || data_2 == 0xb9)
    {
        num_containers++;
    }

    ActorData *actor = &actors[actor_init_cur_actor_num];

    actor->actorInfoIndex = actorInfoIndex;
    actor->frame_num = 0;
    actor->x = x;
    actor->y = y;
    actor->update_while_off_screen_flag = update_while_offscreen;
    actor->can_update_if_goes_off_screen_flag = can_update_if_goes_offscreen;
    actor->can_fall_down_flag = can_fall_down;
    actor->non_blocking_flag_maybe = is_non_blocking;
    actor->is_deactivated_flag_maybe = 0;
    actor->update_function = update_function;
    actor->has_moved_left_flag = 0;
    actor->has_moved_right_flag = 0;
    actor->falling_counter = 0;
    actor->data_1 = data_1;
    actor->data_2 = data_2;
    actor->data_3 = data_3;
    actor->data_4 = data_4;
    actor->data_5 = data_5;
    actor->count_down_timer = 0;

    return;
}

int actor_init(int actor_num, int image_index, int x_pos, int y_pos)
{
    switch (image_index)
    {
        case 0:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0, 0xb9, 0, 0, 0);
            break;

        case 1:
            actor_init_struct(actor_num, 1, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 2:
            actor_init_struct(actor_num, 2, x_pos, y_pos, 0, 1, 1, 0, actor_wt_spring, 0, 0, 0, 0, 0);
            break;

        case 3:
            actor_init_struct(actor_num, 3, x_pos, y_pos, 0, 1, 0, 0, actor_wt_extending_arrow, 0, 0, 0, 0, 0);
            break;

        case 4:
            actor_init_struct(actor_num, 4, x_pos - 4, y_pos, 0, 1, 0, 0, actor_wt_extending_arrow, 0, 0, 0, 0, 1);
            break;

        case 5:
            actor_init_struct(actor_num, 5, x_pos, y_pos, 1, 0, 0, 0, actor_wt_plasma_fireball, 0, x_pos, y_pos, 0, 0);
            break;

        case 6:
            actor_init_struct(actor_num, 5, x_pos - 1, y_pos, 1, 0, 0, 0, actor_wt_plasma_fireball, 0, x_pos - 1, y_pos, 0, 1);
            break;

        case 7:
            actor_init_struct(actor_num, 7, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch, 0, 0, 0, 0, 11);
            break;

        case 11:
            actor_init_struct(actor_num, 11, x_pos, y_pos, 0, 0, 0, 0, actor_wt_door, 0, 0, 0, 0, 0);
            break;

        case 8:
            actor_init_struct(actor_num, 8, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch, 0, 0, 0, 0, 12);
            break;

        case 12:
            actor_init_struct(actor_num, 12, x_pos, y_pos, 0, 0, 0, 0, actor_wt_door, 0, 0, 0, 0, 0);
            break;

        case 9:
            actor_init_struct(actor_num, 9, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch, 0, 0, 0, 0, 13);
            break;

        case 13:
            actor_init_struct(actor_num, 13, x_pos, y_pos, 0, 0, 0, 0, actor_wt_door, 0, 0, 0, 0, 0);
            break;

        case 10:
            actor_init_struct(actor_num, 10, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch, 0, 0, 0, 0, 14);
            break;

        case 14:
            actor_init_struct(actor_num, 14, x_pos, y_pos, 0, 0, 0, 0, actor_wt_door, 0, 0, 0, 0, 0);
            break;

        case 16:
            actor_init_struct(actor_num, 0x10, x_pos, y_pos, 1, 0, 0, 0, actor_wt_blue_mobile_trampoline_car, 0, 0, 0, 0, 0);
            break;

        case 17:
            actor_init_struct(actor_num, 0x11, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 18:
            actor_init_struct(actor_num, 0x12, x_pos, y_pos, 0, 0, 0, 0, actor_wt_retracting_spikes, 1, 0, 0, 0, 0);
            break;

        case 20:
            actor_init_struct(actor_num, 0x14, x_pos, y_pos, 0, 1, 0, 1, actor_wt_big_saw_blade, 0, 0, 0, 0, 0);
            break;

        case 22:
            actor_init_struct(actor_num, 0x14, x_pos, y_pos, 1, 0, 0, 1, actor_wt_robotic_spike_ground, 0, 0, 0, 0, 1);
            break;

        case 24:
            actor_init_struct(actor_num, 0x18, x_pos, y_pos, 1, 0, 1, 1, actor_wt_bomb, 0, 0, 0, 0, 0);
            break;

        case 25:
            actor_init_struct(actor_num, 0x19, x_pos, y_pos, 0, 1, 1, 1, actor_wt_green_pruny_cabbage_ball, 1, 0, 0, 0, 0);
            break;

        case 28:
            actor_init_struct(actor_num, 0x1c, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 1, 6);
            break;

        case 29:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x1c, 0x1e, 0, 0, 0);
            break;

        case 31:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x20, 0xb9, 0, 0, 0);
            break;

        case 33:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x22, 0xb9, 0, 0, 0);
            break;

        case 35:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x24, 0x1e, 0, 0, 0);
            break;

        case 37:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x26, 0x1e, 0, 0, 0);
            break;

        case 58:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 1, actor_wt_container, 2, 0x1e, 0, 0, 0);
            break;

        case 32:
            actor_init_struct(actor_num, 0x20, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 34:
            actor_init_struct(actor_num, 0x22, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 36:
            actor_init_struct(actor_num, 0x24, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 38:
            actor_init_struct(actor_num, 0x26, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 39:
            actor_init_struct(actor_num, 0x27, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 40:
            actor_init_struct(actor_num, 0x29, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 41:
            actor_init_struct(actor_num, 0x29, x_pos, y_pos, 0, 0, 0, 0, actor_wt_spear_vertical, 0, 0, 0, 0, 0);
            break;

        case 42:
            actor_init_struct(actor_num, 0x2b, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_acid, 0, 0, 0, 0, 0);
            break;

        case 43:
            actor_init_struct(actor_num, 0x2b, x_pos, y_pos + 1, 0, 1, 0, 0, actor_wt_acid, x_pos, y_pos + 1, 0, 0, 1);
            break;

        case 44:
            actor_init_struct(actor_num, 0x2c, x_pos, y_pos, 1, 0, 0, 0, actor_wt_plasma_energy_blue_sprite, 0, 0, 0, 0, 0);
            break;

        case 45:
            actor_init_struct(actor_num, 0x2d, x_pos, y_pos, 0, 1, 0, 0, actor_wt_two_tons, 0, 0, 0, 0, 0);
            break;

        case 46:
            actor_init_struct(actor_num, 0x2e, x_pos, y_pos, 0, 1, 0, 0, actor_wt_jumping_bullet_head, 0, 0, 0, 0, 0);
            break;

        case 47:
            actor_init_struct(actor_num, 0x2f, x_pos, y_pos, 0, 1, 0, 0, actor_wt_stone_head, 0, 0, 0, 0, 0);
            break;

        case 48:
            actor_init_struct(actor_num, 0x31, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 49:
            actor_init_struct(actor_num, 0x31, x_pos, y_pos + 1, 0, 1, 0, 1, actor_wt_big_yellow_spike, 0, 0, 0, 0, 0);
            break;

        case 50:
            actor_init_struct(actor_num, 0x31, x_pos, y_pos, 0, 0, 0, 0, actor_wt_big_yellow_spike, 0, 0, 0, 0, 1);
            break;

        case 51:
            actor_init_struct(actor_num, 0x33, x_pos, y_pos, 0, 1, 0, 0, actor_wt_ghost, 0, 0, 0, 0, 4);
            break;

        case 54:
            actor_init_struct(actor_num, 0x36, x_pos, y_pos, 0, 0, 0, 1, actor_wt_angry_moon, 0, 0, 0, 0, 4);
            break;

        case 55:
            actor_init_struct(actor_num, 0x37, x_pos, y_pos, 0, 0, 0, 0, actor_wt_small_red_plant, 0, 0, 0, 0, 0);
            break;

        case 56:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x39, 0x1e, 0, 0, 0);
            break;

        case 57:
            actor_init_struct(actor_num, 0x39, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_bomb, 0, 0, 0, 0, 0);
            break;

        case 59:
            actor_init_struct(actor_num, 0x3c, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0x3b);
            move_platform_flag = 0;
            break;

        case 61:
            actor_init_struct(actor_num, 0x3c, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0x3d);
            break;

        case 62:
            actor_init_struct(actor_num, 0x3e, x_pos, y_pos, 1, 0, 0, 0, actor_wt_question_mark_block, 0, 0, 0, 0, 0);
            word_2E17E = 0;
            break;

        case 65:
            actor_init_struct(actor_num, 0x41, x_pos, y_pos, 0, 1, 1, 0, actor_wt_mini_ghost, 0, 0, 0, 0, 0);
            break;

        case 66:
            actor_init_struct(actor_num, 0x44, x_pos, y_pos, 1, 0, 0, 1, actor_wt_projectile_flashing_ball, 0, 0, 0, 0, 1);
            break;

        case 67:
            actor_init_struct(actor_num, 0x44, x_pos, y_pos, 1, 0, 0, 1, actor_wt_projectile_flashing_ball, 0, 0, 0, 0, 3);
            break;

        case 68:
            actor_init_struct(actor_num, 0x44, x_pos, y_pos, 1, 0, 0, 1, actor_wt_projectile_flashing_ball, 0, 0, 0, 0, 2);
            break;

        case 69:
            actor_init_struct(actor_num, 0x45, x_pos, y_pos, 0, 1, 0, 0, actor_wt_green_roamer_worm, 0, 3, 0, 0, 0);
            break;

        case 70:
            actor_init_struct(actor_num, 0x46, x_pos, y_pos, 0, 0, 0, 0, actor_wt_pipe_transit_direction, 0, 0, 0, 0, 0);
            break;

        case 71:
            actor_init_struct(actor_num, 0x47, x_pos, y_pos, 0, 0, 0, 0, actor_wt_pipe_transit_direction, 0, 0, 0, 0, 0);
            break;

        case 72:
            actor_init_struct(actor_num, 0x48, x_pos, y_pos, 0, 1, 0, 0, actor_wt_pipe_transit_direction, 0, 0, 0, 0, 0);
            break;

        case 73:
            actor_init_struct(actor_num, 0x49, x_pos, y_pos, 0, 1, 0, 0, actor_wt_pipe_transit_direction, 0, 0, 0, 0, 0);
            break;

        case 74:
            actor_init_struct(actor_num, 0x4b, x_pos, y_pos, 0, 0, 0, 0, actor_wt_egg_head, 0, 0, 0, 0, 0);
            break;

        case 75:
            actor_init_struct(actor_num, 0x4b, x_pos, y_pos, 0, 0, 0, 0, actor_wt_egg_head, 0, 0, 0, 0, 1);
            break;

        case 78:
            actor_init_struct(actor_num, 0x4e, x_pos, y_pos, 0, 1, 0, 0, actor_wt_robotic_spike_ground, 8, 0, 0, 0, 1);
            break;

        case 80:
            actor_init_struct(actor_num, 0x50, x_pos, y_pos + 1 + 1, 0, 1, 0, 0, actor_wt_robotic_spike_ceiling, 0, 0, 0, 0, 0);
            break;

        case 81:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x52, 0xb9, 0, 0, 0);
            break;

        case 82:
            actor_init_struct(actor_num, 0x52, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 83:
            actor_init_struct(actor_num, 0x53, x_pos, y_pos, 0, 0, 0, 0, actor_wt_red_blue_plant, 0, 0, 0, 0, 0);
            break;

        case 84:
            actor_init_struct(actor_num, 0x53, x_pos, y_pos + 1 + 1, 0, 0, 0, 0, actor_wt_red_blue_plant, 0, 0, 0, 0, 4);
            break;

        case 85:
            actor_init_struct(actor_num, 0x55, x_pos, y_pos + 1 + 1, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 86:
            actor_init_struct(actor_num, 0x56, x_pos, y_pos, 0, 1, 1, 1, actor_wt_blue_ball, 0, 0x14, 0, 0, 2);
            break;

        case 87:
            actor_init_struct(actor_num, 0x57, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 88:
            actor_init_struct(actor_num, 0x58, x_pos, y_pos, 0, 0, 0, 0, actor_wt_retracting_spikes, 1, 0, 0, 0, 0);
            break;

        case 89:
            actor_init_struct(actor_num, 0x59, x_pos - 3, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 90:
            actor_init_struct(actor_num, 0x5a, x_pos, y_pos, 1, 0, 0, 0, actor_wt_robot_with_blue_arc, 0, 0, 0, 0, 0);
            break;

        case 91:
            actor_init_struct(actor_num, 0x5b, x_pos, y_pos, 1, 0, 0, 0, actor_wt_blue_platform, 0, 0, 0, 0, 0);
            break;

        case 92:
            actor_init_struct(actor_num, 0x5c, x_pos, y_pos, 0, 1, 0, 0, actor_wt_spark, 0, 0, 0, 0, 0);
            break;

        case 93:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x5e, 0xb9, 0, 0, 0);
            break;

        case 94:
            actor_init_struct(actor_num, 0x5e, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 1, 2);
            break;

        case 95:
            // Eye Plant
            actor_init_struct(actor_num, 0x5f, x_pos, y_pos, 0, 1, 0, 0, actor_wt_pink_eye_plant, 0, 0, 0, 0, 0);
            if(num_eye_plants_remaining_to_blow_up < 15)
            {
                num_eye_plants_remaining_to_blow_up++;
            }
            break;

        case 96:
            actor_init_struct(actor_num, 0x5f, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_pink_eye_plant, 0, 0, 0, 0, 4);
            break;

        case 100:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xfb, 0x1e, 0, 0, 0);
            break;

        case 101:
            actor_init_struct(actor_num, 0x65, x_pos, y_pos, 0, 1, 0, 0, actor_wt_big_red_jumper, 0, 0, 0, 0, 7);
            break;

        case 102:
            actor_init_struct(actor_num, 0x66, x_pos, y_pos, 0, 1, 0, 0, actor_wt_133_boss_purple_15411, 0, 0, 0, 0, 0);
            break;

        case 104:
            actor_init_struct(actor_num, 0x69, x_pos - 1, y_pos + 1 + 1, 1, 0, 0, 0, actor_wt_pneumatic_pipe, 0, 0, 0, 0, 0);
            break;

        case 105:
            actor_init_struct(actor_num, 0x69, x_pos - 1, y_pos + 1 + 1, 0, 1, 0, 0, actor_wt_pneumatic_pipe, 0, 1, 0, 0, 0);
            break;

        case 106:
            actor_init_struct(actor_num, 0x6a, x_pos, y_pos, 0, 1, 0, 0, actor_wt_suction_cup_alien_enemy, 0, 0, 0, 0, 0);
            break;

        case 107:
            actor_init_struct(actor_num, 0x6c, x_pos, y_pos, 1, 0, 0, 0, actor_wt_teleporter, 0, 0, 0, 0, 2);
            break;

        case 108:
            actor_init_struct(actor_num, 0x6c, x_pos, y_pos, 1, 0, 0, 0, actor_wt_teleporter, 0, 0, 0, 0, 1);
            break;

        case 109:
            actor_init_struct(actor_num, 0x44, x_pos, y_pos, 1, 0, 0, 0, actor_wt_projectile_flashing_ball, 0, 0, 0, 0, 0);
            break;

        case 110:
            actor_init_struct(actor_num, 0x44, x_pos, y_pos, 1, 0, 0, 0, actor_wt_projectile_flashing_ball, 0, 0, 0, 0, 4);
            break;

        case 112:
            actor_init_struct(actor_num, 0x70, x_pos - 3, y_pos, 0, 0, 0, 0, actor_wt_cyan_spitting_plant, 0, 0, 0, 0, 2);
            break;

        case 111:
            actor_init_struct(actor_num, 0x6f, x_pos, y_pos, 0, 0, 0, 0, actor_wt_cyan_spitting_plant, 0, 0, 0, 0, 3);
            break;

        case 113:
            actor_init_struct(actor_num, 0x71, x_pos, y_pos, 0, 1, 0, 0, actor_wt_blue_turret_alien, 0, 10, x_pos, 0, 3);
            break;

        case 114:
            actor_init_struct(actor_num, 0x72, x_pos, y_pos, 0, 1, 0, 0, actor_wt_hoverboard, 0, 0, 0, 0, 0);
            break;

        case 118:
            actor_init_struct(actor_num, 0x76, x_pos, y_pos, 0, 1, 1, 0, actor_wt_red_chomper_alien, 0, 0, 0, 0, 0);
            break;

        case 120:
            actor_init_struct(actor_num, 0x3c, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0x78);
            brightness_effect_enabled_flag = 0;
            obj_switch_151_flag = 1;
            break;

        case 121:
            actor_init_struct(actor_num, 0x3c, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0x79);
            break;

        case 122:
            actor_init_struct(actor_num, 0x7a, x_pos, y_pos, 1, 0, 0, 0, actor_wt_energy_beam, 0, 0, 0, 0, 0);
            break;

        case 123:
            actor_init_struct(actor_num, 0x7b, x_pos, y_pos, 1, 0, 0, 0, actor_wt_energy_beam, 0, 0, 0, 0, 1);
            break;

        case 124:
            actor_init_struct(actor_num, 0x7c, x_pos, y_pos, 0, 1, 1, 0, actor_wt_pink_slug, 0, 0, 0, 0, 0);
            break;

        case 125:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0);
            break;

        case 126:
            actor_init_struct(actor_num, 0x7e, x_pos, y_pos, 0, 1, 0, 0, actor_wt_silver_robot, 0, 0, 0, 0, 4);
            break;

        case 127:
            actor_init_struct(actor_num, 0x7f, x_pos, y_pos, 0, 1, 0, 0, actor_wt_security_robot, 0, 0, 0, 0, 4);
            break;

        case 128:
            actor_init_struct(actor_num, 0x80, x_pos, y_pos, 0, 0, 1, 0, actor_wt_159_unknown, 0, 0, 0, 0, 3);
            break;

        case 129:
            actor_init_struct(actor_num, 0x81, x_pos, y_pos, 0, 1, 0, 0, actor_wt_dragonfly, 0, 0, 0, 0, 0);
            break;

        case 130:
            actor_init_struct(actor_num, 0x82, x_pos, y_pos, 1, 0, 0, 0, actor_wt_crate_bomb_box, 0, 0, 0, 0, (sub_1106F() % 0x14) * 5 + 0x32);
            break;

        case 134:
            actor_init_struct(actor_num, 0x86, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 135:
            actor_init_struct(actor_num, 0x87, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 136:
            actor_init_struct(actor_num, 0x88, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 137:
            actor_init_struct(actor_num, 0x89, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 138:
            actor_init_struct(actor_num, 0x8a, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 139:
            actor_init_struct(actor_num, 0x8b, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 140:
            actor_init_struct(actor_num, 0x8c, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 141:
            actor_init_struct(actor_num, 0x8d, x_pos, y_pos + 1 + 1, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 142:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x86, 0x1e, 0, 0, 0);
            break;

        case 52:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x87, 0xb9, 0, 0, 0);
            break;

        case 53:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x88, 0xb9, 0, 0, 0);
            break;

        case 119:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x89, 0xb9, 0, 0, 0);
            break;

        case 115:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x8a, 0xb9, 0, 0, 0);
            break;

        case 116:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 0, 0, actor_wt_container, 0x8b, 0xb9, 0, 0, 0);
            break;

        case 117:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x8c, 0x1e, 0, 0, 0);
            break;

        case 143:
            actor_init_struct(actor_num, 0x8f, x_pos, y_pos, 0, 0, 0, 0, actor_wt_satellite, 0, 0, 0, 0, 0);
            break;

        case 145:
            actor_init_struct(actor_num, 0x91, x_pos, y_pos + 7, 0, 1, 0, 0, actor_wt_green_plant, 5, 0, 0, 7, 0);
            break;

        case 146:
            actor_init_struct(actor_num, 0x92, x_pos, y_pos + 1 + 1, 1, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 147:
            actor_init_struct(actor_num, 0x93, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 148:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x93, 0xb9, 0, 0, 0);
            break;

        case 149:
            actor_init_struct(actor_num, 0x95, x_pos - 4, y_pos, 0, 1, 0, 0, actor_wt_jaws_and_tongue, 0, 0, 0, 0, 0);
            break;

        case 150:
            actor_init_struct(actor_num, 0x96, x_pos, y_pos, 1, 0, 0, 0, actor_wt_invisible_exit_marker_right, 0, 0, 0, 0, 0);
            break;

        case 151:
            actor_init_struct(actor_num, 0x97, x_pos, y_pos, 0, 0, 0, 0, actor_wt_small_flame, 0, 0, 0, 0, 0);
            break;

        case 153:
            actor_init_struct(actor_num, 0x99, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 154:
            actor_init_struct(actor_num, 0x9a, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 5);
            break;

        case 155:
            actor_init_struct(actor_num, 0x9b, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 6);
            break;

        case 156:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x99, 0x1e, 0, 0, 0);
            break;

        case 157:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x9a, 0x1e, 0, 0, 0);
            break;

        case 158:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0x9b, 0x1e, 0, 0, 0);
            break;

        case 159:
            actor_init_struct(actor_num, 0x20, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 160:
            actor_init_struct(actor_num, 0x22, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 161:
            actor_init_struct(actor_num, 0x24, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 162:
            actor_init_struct(actor_num, 0xa2, x_pos, y_pos, 0, 0, 0, 0, actor_wt_clam_trap, 0, 0, 0, 0, 0);
            break;

        case 163:
            actor_init_struct(actor_num, 0xa3, x_pos, y_pos, 0, 1, 0, 0, actor_wt_blue_cube_platform, 0, 0, 0, 0, 0);
            break;

        case 164:
        case 165:
        case 166:
            actor_init_struct(actor_num, 0xa4, x_pos, y_pos, 1, 0, 0, 0, actor_wt_short_dialog, image_index, 0, 0, 0, 0);
            break;

        case 168:
            actor_init_struct(actor_num, 0xa8, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 167:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xa8, 0xb9, 0, 0, 0);
            break;

        case 170:
            actor_init_struct(actor_num, 0xaa, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 169:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xaa, 0xb9, 0, 0, 0);
            break;

        case 172:
            actor_init_struct(actor_num, 0xac, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 171:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xac, 0xb9, 0, 0, 0);
            break;

        case 174:
            actor_init_struct(actor_num, 0xae, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 5);
            break;

        case 173:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xae, 0x1e, 0, 0, 0);
            break;

        case 176:
            actor_init_struct(actor_num, 0xb0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 175:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xb0, 0x1e, 0, 0, 0);
            break;

        case 177:
            actor_init_struct(actor_num, 0xb1, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 178:
            actor_init_struct(actor_num, 0xb2, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 179:
            actor_init_struct(actor_num, 0xb3, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 180:
            actor_init_struct(actor_num, 0xb4, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 181:
            actor_init_struct(actor_num, 0xb5, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 182:
            actor_init_struct(actor_num, 0xb6, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 183:
            actor_init_struct(actor_num, 0xb7, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 184:
            actor_init_struct(actor_num, 0xb8, x_pos, y_pos, 0, 1, 0, 0, actor_wt_floating_score_effect, 0, 0, 0, 0, 0);
            break;

        case 186:
            actor_init_struct(actor_num, 0xba, x_pos, y_pos, 0, 1, 0, 0, actor_wt_alien_eating_space_plant, 0, 0, 0x1e, 0, 0);
            break;

        case 187:
            actor_init_struct(actor_num, 0xbb, x_pos, y_pos, 0, 1, 0, 0, actor_wt_blue_bird, 0, 0, 0, 0, 0);
            break;

        case 188:
            actor_init_struct(actor_num, 0xbc, x_pos, y_pos, 0, 1, 0, 0, actor_wt_rocket, 0x3c, 10, 0, 0, 0);
            break;

        case 189:
            actor_init_struct(actor_num, 0xbd, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 190:
            actor_init_struct(actor_num, 0xc0, x_pos, y_pos, 1, 0, 0, 0, actor_wt_destructable_pedestal, 13, 0, 0, 0, 0);
            break;

        case 191:
            actor_init_struct(actor_num, 0xc0, x_pos, y_pos, 1, 0, 0, 0, actor_wt_destructable_pedestal, 0x13, 0, 0, 0, 0);
            break;

        case 192:
            actor_init_struct(actor_num, 0xc0, x_pos, y_pos, 1, 0, 0, 0, actor_wt_destructable_pedestal, 0x19, 0, 0, 0, 0);
            break;

        case 201:
            actor_init_struct(actor_num, 0xc9, x_pos, y_pos, 0, 0, 0, 0, actor_wt_unknown_232, 0, 0, 0, 0, 0);
            break;

        case 193:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xc2, 0x1e, 0, 0, 0);
            break;

        case 194:
            actor_init_struct(actor_num, 0xc2, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 3, 2, 0, 0, 1);
            break;

        case 195:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xc4, 0x1e, 0, 0, 0);
            break;

        case 196:
            actor_init_struct(actor_num, 0xc4, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 197:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xc6, 0x1e, 0, 0, 0);
            break;

        case 198:
            actor_init_struct(actor_num, 0xc6, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 199:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xc8, 0x1e, 0, 0, 0);
            break;

        case 200:
            actor_init_struct(actor_num, 0xc8, x_pos, y_pos, 1, 0, 1, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 202:
            actor_init_struct(actor_num, 0xca, x_pos, y_pos + 1 + 1, 0, 0, 0, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 203:
            actor_init_struct(actor_num, 0x6c, x_pos, y_pos, 1, 0, 0, 0, actor_wt_teleporter, 0, 0, 0, 0, 3);
            break;

        case 204:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 1);
            break;

        case 205:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 2);
            break;

        case 206:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 3);
            break;

        case 207:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 4);
            break;

        case 208:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 5);
            break;

        case 209:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 6);
            break;

        case 210:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 7);
            break;

        case 211:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 8);
            break;

        case 212:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 9);
            break;

        case 63:
            actor_init_struct(actor_num, 0x3f, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 64:
            actor_init_struct(actor_num, 0x40, x_pos, y_pos, 0, 0, 0, 0, actor_wt_rubber_wall, 0, 0, 0, 0, 0);
            break;

        case 213:
            actor_init_struct(actor_num, 0xc2, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 3, 2, 0, 0, 1);
            break;

        case 214:
            actor_init_struct(actor_num, 0xc4, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 215:
            actor_init_struct(actor_num, 0xc6, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 216:
            actor_init_struct(actor_num, 0xc8, x_pos, y_pos, 0, 0, 0, 0, actor_wt_bonus_item, 2, 2, 0, 0, 1);
            break;

        case 152:
            actor_init_struct(actor_num, 0x98, x_pos, y_pos, 0, 0, 0, 0, actor_wt_big_red_plant, 0, 0x1e, 0, 0, 0);
            break;

        case 217:
            actor_init_struct(actor_num, 2, x_pos, y_pos, 1, 0, 0, 0, actor_wt_spring, 0, 0, y_pos + 1, y_pos + 3, 1);
            break;

        case 218:
            actor_init_struct(actor_num, 0x1d, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xdc, 0x1e, 0, 0, 0);
            break;

        case 219:
            actor_init_struct(actor_num, 0xdc, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 220:
            actor_init_struct(actor_num, 0xdc, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 221:
            actor_init_struct(actor_num, 0xdd, x_pos, y_pos, 0, 0, 0, 0, actor_wt_frozen_duke_nukum, 0, 0, 0, 0, 0);
            break;

        case 223:
            actor_init_struct(actor_num, 0xdf, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 224:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xe2, 0xb9, 0, 0, 0);
            break;

        case 225:
            actor_init_struct(actor_num, 0xe2, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 226:
            actor_init_struct(actor_num, 0xe2, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 227:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xe5, 0xb9, 0, 0, 0);
            break;

        case 228:
            actor_init_struct(actor_num, 0xe5, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 229:
            actor_init_struct(actor_num, 0xe5, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 230:
            actor_init_struct(actor_num, 0, x_pos, y_pos, 1, 0, 1, 0, actor_wt_container, 0xe8, 0xb9, 0, 0, 0);
            break;

        case 231:
            actor_init_struct(actor_num, 0xe8, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 232:
            actor_init_struct(actor_num, 0xe8, x_pos, y_pos, 1, 0, 1, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 233:
            actor_init_struct(actor_num, 0xe9, x_pos - 1, y_pos, 0, 0, 0, 0, actor_wt_horizontal_flame, 0, 0, 0, 0, 1);
            break;

        case 234:
            actor_init_struct(actor_num, 0xea, x_pos, y_pos, 0, 0, 0, 0, actor_wt_horizontal_flame, 0, 0, 0, 0, 0);
            break;

        case 236:
            actor_init_struct(actor_num, 0xed, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_acid, 0, 0, 0, 0, 0);
            break;

        case 237:
            actor_init_struct(actor_num, 0xed, x_pos, y_pos + 1, 0, 1, 0, 0, actor_wt_acid, x_pos, y_pos + 1, 0, 0, 1);
            break;

        case 238:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 10);
            break;

        case 239:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 11);
            break;

        case 240:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 12);
            break;

        case 241:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 13);
            break;

        case 242:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 14);
            break;

        case 243:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 15);
            break;

        case 235:actor_init_struct(actor_num, 0xeb, x_pos, y_pos, 1, 0, 0, 0, actor_wt_speech_bubble, 0, 0, 0, 0, 0);
            break;

        case 244:actor_init_struct(actor_num, 0xf4, x_pos, y_pos, 1, 0, 0, 0, actor_wt_speech_bubble, 0, 0, 0, 0, 0);
            break;

        case 245:actor_init_struct(actor_num, 0xf5, x_pos, y_pos, 1, 0, 0, 0, actor_wt_speech_bubble, 0, 0, 0, 0, 0);
            break;

        case 246:actor_init_struct(actor_num, 0xf6, x_pos, y_pos, 1, 0, 0, 0, actor_wt_speech_bubble, 0, 0, 0, 0, 0);
            break;

        case 247:
            actor_init_struct(actor_num, 0xf7, x_pos, y_pos, 0, 0, 0, 0, actor_wt_switch_multi_use, 0, 0, 0, 0, 0);
            break;

        case 248:
            actor_init_struct(actor_num, 0xf8, x_pos, y_pos, 0, 0, 0, 0, actor_wt_smoke_rising, 0, 0, 0, 0, 1);
            break;

        case 249:
            actor_init_struct(actor_num, 0xf9, x_pos, y_pos, 0, 0, 0, 0, actor_wt_smoke_rising, 1, 0, 0, 0, 0);
            break;

        case 250:
            actor_init_struct(actor_num, 0xfa, x_pos, y_pos, 1, 0, 0, 0, actor_wt_end_of_level_marker, 0, 0, 0, 0, 0);
            break;

        case 251:
            actor_init_struct(actor_num, 0x19, x_pos, y_pos, 1, 0, 1, 1, actor_wt_green_pruny_cabbage_ball, 2, 0, 0, 0, 0);
            break;

        case 252:
            actor_init_struct(actor_num, 0x9b, x_pos, y_pos + 1, 0, 0, 0, 0, actor_wt_bonus_item, 1, 0, 0, 0, 6);
            break;

        case 253:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x10);
            break;

        case 254:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x11);
            break;

        case 255:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x12);
            break;

        case 256:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x13);
            break;

        case 257:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x14);
            break;

        case 258:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x15);
            break;

        case 259:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x16);
            break;

        case 260:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x17);
            break;

        case 261:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x18);
            break;

        case 262:
            actor_init_struct(actor_num, 0x7d, x_pos, y_pos, 0, 0, 0, 0, actor_wt_hint_dialog, 0, 0, 0, 0, 0x19);
            break;

        case 263:
            actor_init_struct(actor_num, 0x1c, x_pos, y_pos, 0, 1, 1, 0, actor_wt_bonus_item, 0, 0, 0, 1, 6);
            break;

        case 264:
            actor_init_struct(actor_num, 1, x_pos, y_pos, 0, 1, 1, 0, actor_wt_bonus_item, 0, 0, 0, 0, 4);
            break;

        case 265:
            actor_init_struct(actor_num, 0x109, x_pos, y_pos + 3, 1, 0, 0, 0, actor_wt_end_of_level_marker, 1, 0, 0, 0, 0);
            break;

        default :
            return 0;
    }

    return 1;
}

void load_actor(int actor_num, int actorType, int x_pos, int y_pos)
{
    //printf("Loading Actor: %d, (%d,%d)\n", actorType, x_pos, y_pos);
    if (actorType < 0x20)
    {
        switch (actorType)
        {
            case 0:
                if (map_width_in_tiles - 15 >= x_pos)
                {
                    if (map_stride_bit_shift_amt <= 5 || x_pos - 15 < 0)
                    {
                        mapwindow_x_offset = 0;
                    }
                    else
                    {
                        mapwindow_x_offset = x_pos - 15;
                    }
                }
                else
                {
                    mapwindow_x_offset = map_width_in_tiles - 38;
                }

                if (y_pos - 10 < 0)
                {
                    mapwindow_y_offset = 0;
                }
                else
                {
                    mapwindow_y_offset = y_pos - 10;
                }

                player_x_pos = x_pos;
                player_y_pos = y_pos;
                break;

            case 1:
                moving_platform_tbl[num_moving_platforms].x = x_pos;
                moving_platform_tbl[num_moving_platforms].y = y_pos;
                num_moving_platforms++;
                break;

            case 2:
            case 3:
            case 4:
            case 5:
                mud_fountain_tbl[num_mud_fountains].x = x_pos - 1;
                mud_fountain_tbl[num_mud_fountains].y = y_pos - 1;
                mud_fountain_tbl[num_mud_fountains].direction = 0;
                mud_fountain_tbl[num_mud_fountains].length_counter = 0;
                mud_fountain_tbl[num_mud_fountains].current_height = 0;
                mud_fountain_tbl[num_mud_fountains].max_height = actorType * 3;
                mud_fountain_tbl[num_mud_fountains].pause_counter = 0;
                num_mud_fountains++;
                break;

            case 6:
            case 7:
            case 8:
                add_brightness_obj(actorType - 6, x_pos, y_pos);
                break;
            default: break;
        }
    }

    if (actorType >= 0x1f)
    {
        if (actor_init(actor_num, actorType - 31, x_pos, y_pos) != 0)
        {
            total_num_actors = total_num_actors + 1;
        }
    }
    return;
}

void actor_load_tiles()
{
    uint16 num_tile_info_records;
    uint16 num_tiles;
    actor_tiles = load_tiles("ACTORS.MNI", TRANSPARENT, &num_tiles);
    printf("Loaded %d actor tiles.\n", num_tiles);

    actor_sprites = load_tile_info("ACTRINFO.MNI", &num_tile_info_records);
    printf("Loading %d, actor tile info records.\n", num_tile_info_records);
}

void display_actor_sprite_flipped(TileInfo *info, int x_pos, int y_pos)
{
    Tile *tile = &actor_tiles[info->tile_num];
    for(int y=0;y < info->height;y++)
    {
        for (int x = 0; x < info->width; x++)
        {
            uint16 screen_x = (x_pos - mapwindow_x_offset + x + 1) * 8;
            uint16 screen_y = (y_pos - info->height + 1 - mapwindow_y_offset + (info->height-y-1) + 1) * 8;
            uint16 tile_attr = map_get_tile_attr(x_pos+x,y_pos - info->height + 1 + (info->height-y-1));
            if(screen_x >= 8 && screen_x <= 304 && //FIXME need a better way of making sure we draw in the borders.
               screen_y >= 8 && screen_y < 152 &&
               !(tile_attr & TILE_ATTR_IN_FRONT))
            {
                video_draw_tile_flipped(tile, screen_x, screen_y);
            }
            tile++;
        }
    }
}

void display_actor_sprite_maybe(int actorInfoIndex, int frame_num, int x_pos, int y_pos, int tile_display_func_index)
{
    //FIXME

    if(actor_sprites[actorInfoIndex].num_frames==0)
    {
        //printf("WARN: actorInfoIndex %d has no frames!. Wanted frame_num: %d\n", actorInfoIndex, frame_num);
        return;
    }

    TileInfo *info = &actor_sprites[actorInfoIndex].frames[frame_num];
    Tile *tile = &actor_tiles[info->tile_num];

    if(tile_display_func_index == 4)
    {
        display_actor_sprite_flipped(info, x_pos, y_pos);
        return;
    }

    for(int y=0;y < info->height;y++)
    {
        for(int x=0;x < info->width; x++)
        {
            uint16 screen_x = (x_pos - mapwindow_x_offset + x + 1) * 8;
            uint16 screen_y = (y_pos - info->height + 1 - mapwindow_y_offset + y + 1) * 8;
            uint16 tile_attr = map_get_tile_attr(x_pos+x,y_pos - info->height + y + 1);
            if(tile_display_func_index == 6) //FIXME
            {
                screen_x = (x_pos + x + 1) * 8;
                screen_y = (y_pos - info->height + y + 1) * 8;
                video_draw_tile(tile, screen_x, screen_y);
            }
            else
            {
                if(screen_x >= 8 && screen_x <= 304 && //FIXME need a better way of making sure we draw in the borders.
                   screen_y >= 8 && screen_y < 152 &&
                   (!(tile_attr & TILE_ATTR_IN_FRONT) || tile_display_func_index == 5 || tile_display_func_index == 6))
                {
                    if (tile_display_func_index == 2)
                    {
                        video_draw_tile_solid_white(tile, screen_x, screen_y);
                    }
                    else if(tile_display_func_index == 3)
                    {
                        video_draw_tile_mode3(tile, screen_x, screen_y);
                    }
                    else
                    {
                        video_draw_tile(tile, screen_x, screen_y);
                    }
                }
            }
            tile++;
        }
    }
}

void actor_update(ActorData *actor)
{
    if (actor->is_deactivated_flag_maybe != 0)
    {
        return;
    }

    if (actor->y > map_max_y_offset + 0x15)
    {
        actor->is_deactivated_flag_maybe = 1;
        return;
    }

    actor_tile_display_func_index = 0;
    if (actor->count_down_timer != 0)
    {
        actor->count_down_timer = actor->count_down_timer - 1;
    }

    if (is_sprite_on_screen(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) == 0)
    {
        if (actor->update_while_off_screen_flag == 0)
        {
            return;
        }
        actor_tile_display_func_index = 1;
    }
    else
    {

        if (actor->can_update_if_goes_off_screen_flag != 0)
        {
            actor->update_while_off_screen_flag = 1;
        }
    }

    if (actor->can_fall_down_flag != 0)
    {
        if (sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y) != 0)
        {

            actor->y = actor->y - 1;
            actor->falling_counter = 0;
        }

        if (sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) != 0)
        {
            actor->falling_counter = 0;
        }
        else
        {

            if (actor->falling_counter < 5)
            {
                actor->falling_counter = actor->falling_counter + 1;
            }

            if (actor->falling_counter > 1 && actor->falling_counter < 6)
            {
                actor->y = actor->y + 1;
            }

            if (actor->falling_counter == 5)
            {
                if (sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) != 0)
                {
                    actor->falling_counter = 0;
                }
                else
                {
                    actor->y = actor->y + 1;
                }
            }
        }
    }

    if (is_sprite_on_screen(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
    {
        actor_tile_display_func_index = 0;
    }

    actor->update_function(actor);

    if (struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0 &&
            blow_up_actor_with_bomb(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        return;
    }

    if (actor_update_impl(actor, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) == 0 && actor_tile_display_func_index != 1)
    {

        display_actor_sprite_maybe(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y, actor_tile_display_func_index);
    }

    return;
}

void actor_update_all()
{
    word_32EAC = 0;

    for(int i=0;i<total_num_actors;i++)
    {
        actor_update(&actors[i]);
    }

    if (word_2E17E != 0)
    {
        word_2E17E = 0;
    }
    return;
}

BlockingType sprite_blocking_check(int blocking_dir, int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    uint16 sprite_height = actor_sprites[actorInfoIndex].frames[frame_num].height;
    uint16 sprite_width = actor_sprites[actorInfoIndex].frames[frame_num].width;

    switch (blocking_dir)
    {
        case 0:
            for (int i = 0; i < sprite_width; i++)
            {
                if(map_get_tile_attr(x_pos + i, y_pos - sprite_height + 1) & TILE_ATTR_BLOCK_UP)
                {
                    return BLOCKED;
                }
            }
            break;

        case 1:
            for (int i = 0; i < sprite_width; i++)
            {
                uint16 tile_attr = map_get_tile_attr(x_pos + i, y_pos);
                if(tile_attr & TILE_ATTR_SLOPED)
                {
                    return SLOPE;
                }
                if(tile_attr & TILE_ATTR_BLOCK_DOWN)
                {
                    return BLOCKED;
                }
            }
            break;

        case 2:
            if(x_pos == 0)
            {
                return BLOCKED;
            }

            for(int i=0;i<sprite_height;i++)
            {
                uint16 tile_attr = map_get_tile_attr(x_pos, y_pos - i);
                if(i == 0 &&
                        (tile_attr & TILE_ATTR_SLOPED) &&
                        (map_get_tile_attr(x_pos, y_pos - 1) & TILE_ATTR_BLOCK_LEFT) == 0)
                {
                    return SLOPE;
                }

                if(tile_attr & TILE_ATTR_BLOCK_LEFT)
                {
                    return BLOCKED;
                }
            }
            break;

        case 3:
            if (x_pos + sprite_width == map_width_in_tiles)
            {
                return BLOCKED;
            }

            for(int i=0;i<sprite_height;i++)
            {
                uint16 tile_attr = map_get_tile_attr(x_pos + sprite_width - 1, y_pos - i);
                if(i == 0 &&
                   tile_attr & TILE_ATTR_SLOPED &&
                   (map_get_tile_attr(x_pos + sprite_width - 1, y_pos - 1) & TILE_ATTR_BLOCK_RIGHT) == 0)
                {
                    return SLOPE;
                }

                if(tile_attr & TILE_ATTR_BLOCK_RIGHT)
                {
                    return BLOCKED;
                }
            }
            break;

        default: break;
    }

    return NOT_BLOCKED;
}

int is_sprite_on_screen(int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    uint16 sprite_height = actor_sprites[actorInfoIndex].frames[frame_num].height;
    uint16 sprite_width = actor_sprites[actorInfoIndex].frames[frame_num].width;

    if(((mapwindow_x_offset > x_pos || mapwindow_x_offset + 0x26 <= x_pos) && (mapwindow_x_offset < x_pos || x_pos + sprite_width <= mapwindow_x_offset)) ||
            ((mapwindow_y_offset + 0x12 <= y_pos - sprite_height + 1 || mapwindow_y_offset + 0x12 > y_pos) && (y_pos < mapwindow_y_offset || mapwindow_y_offset + 0x12 <= y_pos)))
    {
        return false;
    }

    return true;
}

TileInfo *actor_get_tile_info(int actorInfoIndex, int frame_num) {
    return &actor_sprites[actorInfoIndex].frames[frame_num];
}

ActorData *get_actor(uint16 actor_num) {
    assert(actor_num < MAX_ACTORS);
    return &actors[actor_num];
}
