//
// Created by efry on 14/11/2017.
//

#include "actor_collision.h"
#include "sound/sfx.h"
#include "player.h"
#include "effects.h"
#include "game.h"
#include "status.h"
#include "input.h"
#include "dialog.h"
#include "actor_toss.h"

const int green_roamer_worm_tbl[] = {0xAC, 0x22, 0xB0, 0xAE};

int actor_update_impl(ActorData *actor, int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    if (is_sprite_on_screen(actorInfoIndex, frame_num, x_pos, y_pos) == 0 || actorInfoIndex == 0xdd) //duke nukum
    {
        return 1;
    }

    uint16 sprite_height = actor_get_tile_info(actorInfoIndex, frame_num)->height;
    uint16 sprite_width = actor_get_tile_info(actorInfoIndex, frame_num)->width;

    word_2E1E8 = 0;
    if (actorInfoIndex != 0x66)
    {
        int ax = word_2E180 <= 3 ? 0 : 1;

        if (ax + y_pos - sprite_height + 1 >= player_y_pos && y_pos - sprite_height <= player_y_pos &&
            player_x_pos + 1 + 1 >= x_pos && x_pos + sprite_width - 1 >= player_x_pos && player_hoverboard_counter == 0)
        {
            word_2E1E8 = 1;
        }
    }
    else
    {
        sprite_height = 7;
        if (y_pos - sprite_height + 5 >= player_y_pos && y_pos - sprite_height <= player_y_pos &&
            player_x_pos + 1 + 1 >= x_pos && x_pos + sprite_width - 1 >= player_x_pos)
        {
            word_2E1E8 = 1;
        }
    }

    switch (actorInfoIndex)
    {
        case 2: //33 PLAT:   Floor Spring
            if (actor->data_5 == 0)
            {
                if (actor->count_down_timer == 0 && player_bounce_in_the_air(0x28) != 0)
                {
                    play_sfx(6);
                    if (!speech_bubble_floor_spring_shown_flag)
                    {
                        speech_bubble_floor_spring_shown_flag = true;
                        player_add_speech_bubble(WHOA);
                    }
                    actor->data_1 = 3;
                }
                return 0;
            }
            break;

        case 16: //47 OBJECT: Blue Mobile Trampoline Car
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(0x14) != 0)
            {
                play_sfx(0x2b);
                actor->data_1 = 3;
            }
            return 0;

        case 25: //56 HAZARD: Green Pruny Cabbage Ball
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                actor->count_down_timer = 5;
                play_sfx(6);
                actor_tile_display_func_index = 2;

                actor->data_1 = actor->data_1 - 1;
                if (actor->data_1 == 0)
                {
                    actor->is_deactivated_flag_maybe = 1;
                    player_add_score_for_actor(0x19);

                    exploding_balls_effect(actor->x, actor->y);
                    return 1;
                }
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 0:  //31 Unknown.
        case 29: //60 BARREL: Power Up    (health/12800)
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(5) != 0)
            {
                actor_explode_container(actor);
                player_add_to_score(0x64);

                actor_add_new(0xb1, actor->x, actor->y);
                return 1;
            }
            return 0;

        case 51: // 82 ENEMY:  Ghost
        case 54: // 85 ENEMY:  Angry Moon (blue floating characters)
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                actor->count_down_timer = 3;
                play_sfx(6);

                actor->data_5 = actor->data_5 - 1;
                actor_tile_display_func_index = 2;
                if (actor->data_5 != 0)
                {
                    return 0;
                }
                actor->is_deactivated_flag_maybe = 1;
                if (actorInfoIndex == 0x33)
                {
                    actor_add_new(0x41, actor->x, actor->y);
                }

                exploding_balls_effect(actor->x - 1, actor->y + 1);
                player_add_score_for_actor(0x33);
                return 1;
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 65: // 96 ENEMY:  Mini Ghost (jumps)
        case 106: // 137 ENEMY:  Suction-Cup-Legged Alien
        case 187: // 218 ENEMY:  Blue Bird
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                play_sfx(6);

                actor->is_deactivated_flag_maybe = 1;
                exploding_balls_effect(actor->x, actor->y);
                player_add_score_for_actor(actor->actorInfoIndex);
                return 1;
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 74: // 105 ENEMY:  Egg Head (special?) -- only 1x: in B2.MNI
        case 75: // 106 ENEMY:  Egg Head
            if (actor->count_down_timer != 0)
            {
                return 0;
            }
            if (player_bounce_in_the_air(7) == 0)
            {
                return 0;
            }
            play_sfx(0x21);

            if (actor->data_2 != 0)
            {
                actor->data_2 = 1;
            }
            else
            {
                actor->data_2 = 10;
            }
            return 0;

        case 86: // 117 ENEMY:  Blue Ball
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                play_sfx(6);

                actor->data_3 = 0;
                actor->count_down_timer = 3;
                actor->data_5 = actor->data_5 - 1;
                if (actor->data_1 != 0 || actor->falling_counter != 0)
                {
                    actor->data_5 = 0;
                }

                if (actor->data_5 == 0)
                {
                    exploding_balls_effect(actor->x, actor->y);

                    actor->is_deactivated_flag_maybe = 1;
                    if (actor->data_1 > 0)
                    {
                        player_add_to_score(0xc80);

                        actor_add_new(0xb6, actor->x, actor->y);
                        return 0;
                    }
                    if (actor->falling_counter == 0)
                    {
                        player_add_to_score(0x320);
                    }
                    else
                    {
                        player_add_to_score(0x3200);

                        actor_add_new(0xb8, actor->x, actor->y);
                    }
                }
                else
                {
                    actor_tile_display_func_index = 2;

                    if (actor->data_1 == 0)
                    {
                        actor->data_2 = 0;
                        actor->data_1 = (sub_1106F() & 1) + 1;
                    }
                }
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 101: // 132 ENEMY:  Big Red Jumper FIXME might need new logic from COSMO2 or 3 EXEs
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(15) != 0)
            {
                play_sfx(6);

                actor->count_down_timer = 6;
                actor->data_5 = actor->data_5 - 1;
                if (actor->data_5 != 0)
                {
                    actor_tile_display_func_index = 2;
                }
                else
                {
                    actor_add_new(1, actor->x, actor->y);

                    exploding_balls_effect(actor->x, actor->y);
                    actor->is_deactivated_flag_maybe = 1;
                    return 1;
                }
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 113: // 144 ENEMY:  Blue Turret Alien
        case 118: // 149 ENEMY:  Red Chomper Alien
        case 126: // 157 ENEMY:  Silver Robot (pushes player around)
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                actor->count_down_timer = 3;
                play_sfx(6);
                actor_tile_display_func_index = 2;
                if (actorInfoIndex != 0x76)
                {
                    actor->data_5 = actor->data_5 - 1;
                }
                if (actor->data_5 == 0 || actorInfoIndex == 0x76)
                {

                    actor->is_deactivated_flag_maybe = 1;
                    player_add_score_for_actor(actor->actorInfoIndex);

                    exploding_balls_effect(actor->x, actor->y);
                    return 1;
                }
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 124: // 155 CREATURE/HAZARD:    Pink slug/Worm
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                player_add_score_for_actor(0x7c);
                play_sfx(6);

                exploding_balls_effect(actor->x, actor->y);

                actor->is_deactivated_flag_maybe = 1;
                actor_add_new(0x80, actor->x, actor->y);
                return 1;
            }
            return 0;

        case 127: // 158 ENEMY:  Security Robot (shoots flashing bullet, can act as moving floor spring)
            if (((brightness_effect_enabled_flag == 0 && obj_switch_151_flag != 0) || (brightness_effect_enabled_flag != 0 && obj_switch_151_flag == 0)) &&
                actor->count_down_timer == 0 && player_bounce_in_the_air(15) != 0)
            {
                actor->count_down_timer = 3;
                play_sfx(6);

                if (actor->data_1 == 0)
                {
                    actor->frame_num = 8;
                }
                else
                {
                    actor->frame_num = 7;
                }
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 129: // 160 PLAT/HAZARD:    Dragonfly
        case 145: // 176 ENEMY:  Green Plant
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                num_hits_since_touching_ground = 0;
                play_sfx(6);
                actor->count_down_timer = 5;
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 0;

        case 188: // 219 OBJECT: Rocket
            if (actor->x == player_x_pos && actor->count_down_timer == 0 && player_bounce_in_the_air(5) != 0)
            {
                play_sfx(6);
            }
            return 0;

        case 152: // 183 ENEMY:  Big Red Plant (spitting Blue Balls/catapult)

            if (actor->has_moved_right_flag != 0)
            {
                actor->has_moved_right_flag = actor->has_moved_right_flag - 1;
                if (actor->has_moved_right_flag != 0)
                {
                    return 0;
                }
                byte_2E2E4 = 1;
                word_2E1E8 = 1;
                if (actor->count_down_timer != 0 || player_bounce_in_the_air(0x14) == 0)
                {//FIXME is this correct?
                }
                play_sfx(6);
                byte_2E17C = 0;
                hide_player_sprite = 0;
                word_2E180 = 0;

                actor->has_moved_left_flag = 1;
                actor->data_2 = 0;
                actor->data_1 = 1;
                player_y_pos = player_y_pos - 2;
                if (!speech_bubble_red_plant_shown_flag)
                {
                    speech_bubble_red_plant_shown_flag = true;
                    player_add_speech_bubble(WHOA);
                }
                return 0;
            }

            if (actor->has_moved_left_flag != 0)
            {
                return 0;
            }
            if (actor->x + 1 > player_x_pos)
            {
                return 0;
            }
            if (actor->x + 5 < player_x_pos + 1 + 1)
            {
                return 0;
            }
            if (actor->y - 1 != player_y_pos && actor->y - 2 != player_y_pos)
            {
                return 0;
            }
            if (byte_2E2E4 != 0)
            {

                actor->has_moved_right_flag = 0x14;
                word_2E1E8 = 0;
                player_bounce_height_counter = 0;
                byte_2E2E4 = 0;
                byte_2E17C = 1;
                hide_player_sprite = 1;
                actor->has_moved_left_flag = 1;
                actor->data_2 = 0;
                actor->data_1 = 1;
                play_sfx(0x3e);
            }
            return 0;


        case 102: // 133 ENEMY:  BOSS (Purple Alien in Spike-bottomed ship)

            if (actor->has_moved_right_flag != 0)
            {
                return 1;
            }
            if (actor->data_5 == 12)
            {
                return 1;
            }
            if (actor->count_down_timer == 0 && player_bounce_in_the_air(7) != 0)
            {
                play_sfx(6);

                actor->data_5 = actor->data_5 + 1;
                actor->has_moved_left_flag = 10;
                actor->count_down_timer = 7;
                if (actor->data_1 != 2)
                {
                    actor->data_1 = 2;
                    actor->data_2 = 0x1f;
                    actor->data_3 = 0;
                    actor->data_4 = 1;
                    actor->can_fall_down_flag = 0;
                    actor->falling_counter = 0;
                }

                if (actor->data_5 == 4)
                {
                    explode_effect_add_sprite(0x66, 1, actor->x, actor->y - 4);
                    play_sfx(0x37);
                }

                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);

                effect_add_sprite(0x61, 6, actor->x + 3, actor->y, 2, 1);
            }
            else
            {
                if (actor->count_down_timer == 0 &&
                    player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) != 0)
                {
                    player_decrease_health();
                }
            }
            return 1;
    }


    if (player_check_collision_with_actor(actorInfoIndex, frame_num, x_pos, y_pos) == 0)
    {
        return 0;
    }


    switch (actorInfoIndex - 1)
    {
        case 2:
        case 3:
        case 4:
        case 5:
        case 19:
        case 40:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 67:
        case 77:
        case 79:
        case 91:
        case 150:player_decrease_health();

            if (actor->actorInfoIndex == 0x44)
            {
                actor->is_deactivated_flag_maybe = 1;

            }
            return 0;


        case 232: // 264 DANGER: Flame (<-)
        case 233: // 265 DANGER: Flame (->)
            if (actor->frame_num > 1)
            {
                player_decrease_health();
            }
            return 0;


        case 42: // 74 HAZARD: Dripping Green Acid
        case 236:// 268 HAZARD: Dripping Red Acid

            if (actor->data_5 != 0)
            {
                actor->y = actor->data_2;
                actor->data_4 = 0;
                if (actor->y > player_y_pos - 4 || actor->frame_num == 6)
                {
                    player_decrease_health();
                }

                actor->frame_num = 0;
                return 0;
            }
            if (actor->y > player_y_pos - 4)
            {
                player_decrease_health();
            }
            return 0;

        case 82: // 114 ENEMY:  Blue/Red plant
        case 83: // 115 Unknown
            if (actor->frame_num != 0)
            {
                player_decrease_health();
            }
            return 0;

        case 6: // 38 Blue Switch (on ceiling
        case 7: // 39 Red Switch (on ceiling)
        case 8: // 40 Green Switch
        case 9: // 41 Yellow Switch

            if (actor->frame_num == 0)
            {
                actor->y = actor->y - 1;
                actor->frame_num = 1;
            }
            return 0;

        case 16: // 48 HAZARD: Non-Retracting Spikes
        case 17: // 49 HAZARD: Retracting Spikes
        case 86: // 118 ENEMY:  Spikes (on left wall)
        case 87: // 119 ENEMY:  Retracting Spikes (on left wall)
        case 88: // 120 ENEMY:  Spikes (on right wall)
            if (actor->frame_num <= 1)
            {
                player_decrease_health();
                return 0;
            }
            return 1;

        case 27: // 59  BONUS:  Power Up    (health/12800)
            actor->is_deactivated_flag_maybe = 1;
            play_sfx(1);

            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            if (knows_about_powerups_flag == 0)
            {
                knows_about_powerups_flag = 1;
                power_up_module_dialog();
            }
            if (health > num_health_bars)
            {
                player_add_to_score(0x3200);

                actor_add_new(0xb8, actor->x, actor->y);
            }
            else
            {
                health = health + 1;
                update_health_bar_display();

                player_add_to_score(0x64);

                actor_add_new(0xb1, actor->x, actor->y);
            }
            return 1;

        case 84:
        case 93:
        case 133:
        case 134:
        case 135:
        case 136:
        case 137:
        case 138:
        case 139:
        case 140:
        case 145:
        case 146:
        case 167:
        case 169:
        case 171:
        case 222:
        case 225:
        case 228:
        case 231:actor->is_deactivated_flag_maybe = 1;
            if (actorInfoIndex != 0x92 && actorInfoIndex != 0xdf && actorInfoIndex != 0x55 && actorInfoIndex != 0x8d)
            {
                player_add_to_score(0x190);
                actor_add_new(0xb3, x_pos, y_pos);
            }
            else
            {
                player_add_to_score(0x320);
                actor_add_new(0xb4, x_pos, y_pos);
            }
            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            play_sfx(13);
            return 1;

        case 56: // 88: bonus bomb
            if (num_bombs <= 8)
            {
                actor->is_deactivated_flag_maybe = 1;
                num_bombs++;
                has_had_bomb_flag = 1;
                player_add_to_score(0x64);

                actor_add_new(0xb1, actor->x, actor->y);
                display_num_bombs_left();

                effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
                play_sfx(13);
                return 1;
            }
            return 0;

        case 68: // 100 CREATURE:   Green Roamer Worm
            if (actor->count_down_timer != 0)
            {
                return 0;
            }

            actor->count_down_timer = 10;
            if (player_bounce_in_the_air(7) == 0)
            {
                player_hanging_on_wall_direction = 0;
            }
            else
            {
                play_sfx(6);
            }

            actor_toss_add_new(green_roamer_worm_tbl[sub_1106F() & 3], actor->x, actor->y + 1);
            play_sfx(0x11);
            actor_tile_display_func_index = 2;

            actor->data_2 = actor->data_2 - 1;
            if (actor->data_2 == 0)
            {
                actor->is_deactivated_flag_maybe = 1;
                exploding_balls_effect(actor->x - 1, actor->y + 1);
            }
            return 0;

        case 69: // 101 PIPE TRANSIT DIRECTION: Arrow Up
        case 70: // 102 PIPE TRANSIT DIRECTION: Arrow Down
        case 71: // 103 PIPE TRANSIT DIRECTION: Arrow Left
        case 72: // 104 PIPE TRANSIT DIRECTION: Arrow Right
            if (player_in_pneumatic_tube_flag == 0)
            {
                return 1;
            }

            switch (actorInfoIndex - 0x46)
            {
                case 0:push_player_around(1, 0x64, 2, 0xff, 0, 0);
                    break;

                case 1:push_player_around(5, 0x64, 2, 0xff, 0, 0);
                    break;

                case 2:push_player_around(7, 0x64, 2, 0xff, 0, 0);
                    break;

                case 3:push_player_around(3, 0x64, 2, 0xff, 0, 0);
                    break;
            }

            play_sfx(0x16);
            return 1;

        case 104: // 136: pneumatic pipe entrance

            if (actor->data_2 == 0 && (actor->y + 3 == player_y_pos || actor->y + 2 == player_y_pos))
            {
                if (player_is_being_pushed_flag == 0)
                {
                    return 0;
                }
                player_x_pos = actor->x;
                word_32EB2 = 1;
                player_in_pneumatic_tube_flag = 0;
                player_reset_push_variables();
                if (!speech_bubble_pipe_shown_flag)
                {
                    speech_bubble_pipe_shown_flag = true;
                    player_add_speech_bubble(WHOA);
                }
                return 0;
            }
            if (byte_2E2E4 != 0 && player_bounce_flag_maybe == 0)
            {
                return 0;
            }
            if (jump_key_pressed == 0 && player_bounce_flag_maybe == 0)
            {
                return 0;
            }

            if (actor->x != player_x_pos)
            {
                return 0;
            }
            if (actor->y + 3 == player_y_pos || actor->y + 2 == player_y_pos)
            {
                player_in_pneumatic_tube_flag = 1;
            }
            return 0;

        case 107: // 139 OBJECT: Teleporter B
            if (teleporter_counter != 0)
            {
                return 1;
            }

            if (actor->x > player_x_pos || actor->x + 4 < player_x_pos + 1 + 1 || actor->y != player_y_pos)
            {
                player_is_teleporting_flag = 0;
            }
            else
            {
                if (up_key_pressed != 0)
                {
                    teleporter_state_maybe = actor->data_5;
                    teleporter_counter = 15;
                    byte_2E2E4 = 0;
                }
                player_is_teleporting_flag = 1;
            }
            return 1;

        case 113: // 145 PLAT:   Hoverboard
            if (byte_2E2E4 == 0)
            {
                return 0;
            }

            if (actor->y != player_y_pos && actor->y + 1 != player_y_pos)
            {
                return 0;
            }
            player_hoverboard_counter = 4;
            play_sfx(3);
            player_reset_push_variables();
            byte_2E2E4 = 0;
            word_2E180 = 0;
            player_bounce_flag_maybe = 0;
            word_2E1E8 = 0;
            player_bounce_height_counter = 0;
            num_hits_since_touching_ground = 0;
            if (!speech_bubble_hoverboard_shown_flag)
            {
                speech_bubble_hoverboard_shown_flag = true;
                player_add_speech_bubble(WHOA);
            }
            return 0;

        case 148: // 180 DECO:   Jaws & Tongue

            if (actor->data_4 != 0)
            {
                actor->data_4 = actor->data_4 - 1;
                if (actor->data_4 != 0)
                {
                    actor->frame_num = 0;
                    return 1;
                }
                else
                {
                    finished_level_flag_maybe = 1;
                    actor->frame_num = 0;
                }
                return 0;
            }

            if (actor->data_1 == 0)
            {
                return 1;
            }
            if (actor->y != player_y_pos)
            {
                return 1;
            }
            if (actor->x <= player_x_pos)
            {
                actor->frame_num = 0;
                actor->data_5 = 0;
                actor->data_4 = 5;
                hide_player_sprite = 1;
                byte_2E17C = 1;
                play_sfx(0x27);
            }
            return 1;

        case 161: // 193 HAZARD: Clamp Trap

            if (actor->data_2 == 0 && actor->x == player_x_pos && actor->y == player_y_pos)
            {
                actor->data_2 = 1;
                byte_2E17C = 1;
                if (!speech_bubble_clam_trap_shown_flag)
                {
                    speech_bubble_clam_trap_shown_flag = true;
                    player_add_speech_bubble(UMPH);
                }
                return 0;
            }
            //NOTE deliberate fall through here. :)

        case 185: // 217 ENEMY:  Alien-Eating Space Plant (Exits the current level)

            if (actor->frame_num != 0)
            {
                return 0;
            }
            if (actor->x >= player_x_pos)
            {
                return 0;
            }
            if (actor->x + 5 <= player_x_pos)
            {
                return 0;
            }
            if (actor->y - 2 <= player_y_pos)
            {
                return 0;
            }
            if (actor->y - 5 >= player_y_pos)
            {
                return 0;
            }
            if (byte_2E2E4 != 0)
            {
                actor->data_5 = 1;
                byte_2E17C = 1;
                hide_player_sprite = 1;
                actor->frame_num = 1;
                play_sfx(0x27);
            }
            return 0;

        case 0: // 32: bonus star
            effect_add_sprite(0x17, 8, x_pos, y_pos, 0, 1);
            num_stars_collected++;
            actor->is_deactivated_flag_maybe = 1;
            play_sfx(1);
            player_add_score_for_actor(actorInfoIndex);
            actor_add_new(0xb2, x_pos, y_pos);
            display_num_stars_collected();
            return 1;

        case 81: // 113 Hamburger
            actor->is_deactivated_flag_maybe = 1;
            player_add_to_score(0x3200);
            actor_add_new(0xb8, x_pos, y_pos);

            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            play_sfx(13);
            if (num_health_bars < 5)
            {
                num_health_bars++;
            }
            if (!speech_bubble_hamburger_shown_flag)
            {
                player_add_speech_bubble(WHOA);
                speech_bubble_hamburger_shown_flag = true;
            }
            update_health_bar_display();
            return 1;

        case 31: // 63 BONUS:  Green Tomato (falling)
        case 33: // 65 BONUS:  Tomato (falling)
        case 35: // 67 BONUS:  Yellow/Cyan Fruit (falling)
        case 37: // 69 Unknown
            actor->is_deactivated_flag_maybe = 1;
            player_add_to_score(0xc8);
            actor_add_new(0xb2, x_pos, y_pos);

            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            play_sfx(13);
            return 1;

        case 188: // 220 BONUS:  Invincibility Cube
            actor->is_deactivated_flag_maybe = 1;
            actor_add_new(0xc9, player_x_pos - 1, player_y_pos + 1);
            effect_add_sprite(0x17, 8, x_pos, y_pos, 0, 1);
            actor_add_new(0xb8, x_pos, y_pos);
            play_sfx(1);
            return 1;

        case 152: // 184 unknown
        case 173: // 205 BONUS:  Green Gem
        case 175: // 207 BONUS:  Diamond
            actor->is_deactivated_flag_maybe = 1;
            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            player_add_to_score(0xc80);
            actor_add_new(0xb6, x_pos, y_pos);
            play_sfx(13);
            return 1;

        case 153: // 185 BONUS:  Blue Crystal (on ground)
        case 154: // 186 BONUS:  Red Crystal (on ground)
            actor->is_deactivated_flag_maybe = 1;
            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            player_add_to_score(0x640);
            actor_add_new(0xb5, x_pos, y_pos);
            play_sfx(13);
            return 1;

        case 193: // 225 BONUS:  Diamond 2 (falling)
        case 195: // 227 BONUS:  Red Berry (falling)
        case 197: // 229 BONUS:  Crystal (falling)
        case 199: // 231 BONUS:  Blue Gem (falling)
        case 219: // 251 BONUS:  Headphones (falling)
            actor->is_deactivated_flag_maybe = 1;
            effect_add_sprite(15, 4, actor->x, actor->y, 0, 3);
            player_add_to_score(0x320);
            actor_add_new(0xb4, x_pos, y_pos);
            play_sfx(13);
            return 1;

        case 1: // 33: floor spring
            if (actor->data_5 != 0 && actor->count_down_timer == 0 && player_hoverboard_counter == 0 &&
                (byte_2E2E4 == 0 || player_bounce_flag_maybe != 0))
            {
                actor->count_down_timer = 2;
                play_sfx(6);
                actor->data_1 = 3;
                player_bounce_height_counter = 0;
                player_bounce_flag_maybe = 0;
                byte_2E2E4 = 1;
                word_2E180 = 4;
                byte_2E182 = 0;
            }
            return 0;

        case 38: // 70  OBJECT: Exit Sign (ends level on touch)
            finished_level_flag_maybe = 1;
            return 0;

        case 59: // 91 unknown.

            if (actor->data_1 >= 4 || actor->data_4 != 0)
            {
                return 0;
            }
            byte_2E2E4 = 1;
            sub_11062();
            player_bounce_in_the_air(3);

            actor->data_1 = actor->data_1 + 1;
            if (actor->data_2 != 0)
            {
                actor->data_3 = 0;
            }
            else
            {
                actor->data_3 = 0x40;
                actor->data_2 = 1;
            }
            actor->data_4 = 1;
            return 0;

        case 63: // 95  OBJECT: Rubber Wall (can blow it up, throws Cosmo backwards when touched)
            if (!speech_bubble_rubber_wall_shown_flag)
            {
                speech_bubble_rubber_wall_shown_flag = true;
                player_add_speech_bubble(UMPH);
            }
            if (actor->x != player_x_pos + 2)
            {
                if (actor->x + 2 != player_x_pos)
                {
                    return 0;
                }
                push_player_around(3, 5, 2, 0x11, 0, 1);
            }
            else
            {
                push_player_around(7, 5, 2, 0x28, 0, 1);
            }
            play_sfx(0x14);
            return 0;

        case 62:
        case 110:
        case 111:
        case 127:
        case 201:player_decrease_health();
            return 0;

        case 54: // 86  ENEMY:  Small Red Plant
            actor->data_1 = 1;
            player_decrease_health();
            return 0;

        case 246:
            if (get_episode_number() != 1) //FIXME is this ok to leave in EP1
            {
                hide_player_sprite = 1;
                byte_2E17C = 1;
                actor->data_1++;
                if (actor->frame_num == 0)
                {
                    if (actor->data_1 == 3)
                    {
                        actor->frame_num++;
                    }
                }
                else
                {
                    finished_level_flag_maybe = 1;
                }
                if (actor->data_1 > 1)
                {
                    player_y_pos = actor->y;
                    byte_2E2E4 = 0;
                }
                return 0;
            }
            break;

        default :break;
    }

    return 0;
}

void actor_explode_container(ActorData *actor)
{
    actor->is_deactivated_flag_maybe = 1;
    explode_effect_add_sprite(actor->data_2, 0, actor->x - 1, actor->y);

    explode_effect_add_sprite(actor->data_2, 1, actor->x + 1, actor->y - 1);

    explode_effect_add_sprite(actor->data_2, 2, actor->x + 3, actor->y);

    explode_effect_add_sprite(actor->data_2, 3, actor->x + 1 + 1, actor->y + 2);
    if((sub_1106F() & 1) == 0)
    {
        play_sfx(0x3d);
    }
    else
    {
        play_sfx(12);
    }

    actor_toss_add_new(actor->data_1, actor->x + 1, actor->y);
    if(num_containers == 1)
    {
        player_add_speech_bubble(POINTS_50000);
    }
    num_containers--;
    return ;
}
