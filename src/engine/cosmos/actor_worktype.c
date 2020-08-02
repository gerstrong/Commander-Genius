//
// Created by efry on 12/11/2017.
//

#include <stdlib.h>
#include "sound/music.h"
#include "actor_worktype.h"
#include "player.h"
#include "effects.h"
#include "game.h"
#include "dialog.h"
#include "input.h"
#include "sound/sfx.h"
#include "tile.h"
#include "map.h"
#include "actor_toss.h"
#include "actor_collision.h"
#include "util.h"

static const sint16 purple_boss_y_offset_tbl[] = {2, 2, 1, 0, -1, -2, -2, -2, -2, -1, 0, 1, 2, 2};
void actor_wt_133_boss_purple_15411(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    if(!speech_bubble_purple_boss_shown_flag)
    {
        speech_bubble_purple_boss_shown_flag = true;
        player_add_speech_bubble(WHOA);
        stop_music();
        load_music(2);
    }
    
    if (actor->has_moved_right_flag > 0)
    {
        actor->has_moved_right_flag = actor->has_moved_right_flag - 1;
        if(actor->has_moved_right_flag < 0x28)
        {
            actor->y = actor->y - 1;
        }

        actor->can_fall_down_flag = 0;
        actor->falling_counter = 0;
        if(actor->has_moved_right_flag == 1 || actor->y == 0 || (is_sprite_on_screen(0x66, 0, actor->x, actor->y) == 0 && actor->has_moved_right_flag < 0x1e))
        {
            if (get_episode_number() == 3) {
                finished_game_flag_maybe = 1;
            } else {
                finished_level_flag_maybe = 1;
            }
            player_add_to_score(0x186a0);
        }

        if(actor->has_moved_right_flag < 0x28 && actor->has_moved_right_flag != 0)
        {
            if(actor->has_moved_right_flag % 3 == 0)
            {
                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
                effect_add_sprite(0x61, 6, actor->x + 3, actor->y, 2, 1);
                play_sfx(0x38);
            }
        }

        if((actor->has_moved_right_flag & 1) == 0)
        {
            display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 0);
            display_actor_sprite_maybe(0x66, 5, actor->x, actor->y - 4, 0);
        }
        else
        {
            display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 2);
            display_actor_sprite_maybe(0x66, 5, actor->x, actor->y - 4, 2);

            if(actor->has_moved_right_flag > 0x27)
            {
                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
                effect_add_sprite(0x61, 6, actor->x + 3, actor->y, 2, 1);
            }
        }
        return;
    }

    if (actor->data_5 == 12)
    {
        if(sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + 1) == 0)
        {
            actor->y = actor->y + 1;
            if((actor->y & 1) == 0)
            {
                display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 0);
                display_actor_sprite_maybe(0x66, 5, actor->x, actor->y - 4, 0);
            }
            else
            {
                display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 2);
                display_actor_sprite_maybe(0x66, 5, actor->x, actor->y - 4, 2);
            }
        }

        if(sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
        {
            actor->has_moved_right_flag = 0x50;
        }
        return;
    }

    if(actor->has_moved_left_flag != 0)
    {
        int frame_num = actor->data_5 <= 3 ? 1 : 5;

        actor->has_moved_left_flag = actor->has_moved_left_flag - 1;
        if((actor->has_moved_left_flag & 1) == 0)
        {
            display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 0);
            display_actor_sprite_maybe(0x66, frame_num, actor->x, actor->y - 4, 0);
        }
        else
        {
            display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 2);
            display_actor_sprite_maybe(0x66, frame_num, actor->x, actor->y - 4, 2);
        }
    }
    
    if (actor->data_1 != 0)
    {
        if (actor->data_1 != 1)
        {
            if (actor->data_1 == 2)
            {
                if(sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + purple_boss_y_offset_tbl[actor->data_3 % 14]) != NOT_BLOCKED)
                {
                    if(purple_boss_y_offset_tbl[actor->data_3 % 14] == 2)
                    {
                        actor->y = actor->y - 2;
                    }
                }

                if (sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + purple_boss_y_offset_tbl[actor->data_3 % 14]) == NOT_BLOCKED || purple_boss_y_offset_tbl[actor->data_3 % 14] != 1)
                {
                    actor->y += purple_boss_y_offset_tbl[actor->data_3 % 14];
                }
                else
                {
                    actor->y = actor->y - 1;
                }

                actor->data_3 = actor->data_3 + 1;
                if(actor->data_3 % 14 == 1)
                {
                    play_sfx(0x38);
                }

                actor->data_2 = actor->data_2 + 1;
                if(actor->data_2 > 0x1e && actor->data_2 < 0xc9)
                {
                    if(actor->data_4 == 0)
                    {
                        if(sprite_blocking_check(2, 0x66, 0, actor->x - 1, actor->y) != NOT_BLOCKED)
                        {
                            actor->data_4 = 1;
                            play_sfx(0x25);

                            effect_add_sprite(0x61, 6, actor->x, actor->y - 2, 5, 1);
                        }
                        else
                        {
                            actor->x = actor->x - 1;
                        }
                    }
                    else
                    {
                        if(sprite_blocking_check(3, 0x66, 0, actor->x + 1, actor->y) == NOT_BLOCKED)
                        {
                            actor->x = actor->x + 1;
                        }
                        else
                        {
                            actor->data_4 = 0;
                            play_sfx(0x25);

                            effect_add_sprite(0x61, 6, actor->x + 3, actor->y - 2, 5, 1);
                        }
                    }
                }
                else
                {

                    if(actor->data_2 > 0xc7)
                    {
                        actor->data_1 = 3;
                        actor->data_2 = 0;
                        actor->data_3 = 8;
                    }
                }
            }
            else
            {
                if (actor->data_1 == 3)
                {
                    actor->data_2 = actor->data_2 + 1;
                    if (actor->data_3 >= 6)
                    {
                        if (actor->data_2 < 0x66)
                        {
                            actor->can_fall_down_flag = 1;
                            if(sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + 1) == 0)
                            {
                                if(actor->x + 1 <= player_x_pos)
                                {
                                    if(actor->x + 3 < player_x_pos)
                                    {
                                        if(sprite_blocking_check(3, 0x66, 0, actor->x + 1, actor->y) == NOT_BLOCKED)
                                        {
                                            actor->x = actor->x + 1;
                                        }
                                    }
                                }
                                else
                                {
                                    if(sprite_blocking_check(2, 0x66, 0, actor->x - 1, actor->y) == NOT_BLOCKED)
                                    {
                                        actor->x = actor->x - 1;
                                    }
                                }
                            }
                            else
                            {
                                actor->data_3 = 0;
                                actor->can_fall_down_flag = 0;
                                actor->falling_counter = 0;
                                play_sfx(0x32);

                                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
                                effect_add_sprite(0x61, 6, actor->x + 3, actor->y, 2, 1);
                            }
                        }
                        else
                        {
                            if (sprite_blocking_check(1, 0x66, 0, actor->x, actor->y + 1) != 0 || sprite_blocking_check(1, 0x66, 0, actor->x, actor->y) != 0)
                            {
                                actor->data_1 = 4;
                                actor->data_2 = 0;
                                actor->data_3 = 0;
                                actor->can_fall_down_flag = 0;
                                actor->falling_counter = 0;
                                play_sfx(0x25);

                                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
                                effect_add_sprite(0x61, 6, actor->x + 3, actor->y, 2, 1);
                            }
                            else
                            {
                                actor->y = actor->y + 1;
                            }
                        }
                    }
                    else
                    {
                        actor->data_3 = actor->data_3 + 1;
                        actor->y = actor->y - 2;
                    }
                }
                else
                {
                    if(actor->data_1 == 4)
                    {
                        actor->can_fall_down_flag = 0;
                        actor->falling_counter = 0;
                        actor->y = actor->y - 1;
                        actor->data_2 = actor->data_2 + 1;
                        if(actor->data_2 == 6)
                        {
                            actor->data_1 = 2;
                            actor->data_3 = 0;
                            actor->data_2 = 0;
                        }
                    }
                }
            }
        }
        else
        {
            if(actor->data_2 == 0)
            {
                actor->data_1 = 2;
            }
            else
            {
                actor->data_2 = actor->data_2 - 1;
            }
        }
    }
    else
    {
        actor->y = actor->y - 2;
        actor->data_2 = actor->data_2 + 1;
        if(actor->data_2 == 6)
        {
            actor->data_1 = 1;
        }
    }

    if(actor->has_moved_left_flag != 0)
    {
        return;
    }
    display_actor_sprite_maybe(0x66, 0, actor->x, actor->y, 0);

    if(actor->data_5 < 4)
    {
        display_actor_sprite_maybe(0x66, 1, actor->x, actor->y - 4, 0);
        return;
    }

    if(actor->x + 1 > player_x_pos)
    {
        display_actor_sprite_maybe(0x66, 2, actor->x + 1, actor->y - 4, 0);
        return;
    }

    if(actor->x + 1 + 1 >= player_x_pos)
    {
        display_actor_sprite_maybe(0x66, 3, actor->x + 1, actor->y - 4, 0);
    }
    else
    {
        display_actor_sprite_maybe(0x66, 4, actor->x + 1, actor->y - 4, 0);
    }
}

void actor_wt_159_unknown(ActorData *actor)
{
    if(actor->data_5 != 0)
    {
        actor->data_5--;
        return;
    }

    if(actor->frame_num == 8)
    {
        actor->frame_num = 1;
        return;
    }

    actor->frame_num++;
    return;
}

const static uint8  acid_frame_num_tbl[] = {0, 1, 2, 3, 2, 1, 0};
void actor_wt_acid(ActorData *actor)
{
    if(actor->data_5 != 0)
    {
        if(actor->data_4 == 0)
        {
            actor->frame_num = acid_frame_num_tbl[actor->data_3 % 6];
            actor->data_3++;
            if(actor->data_3 != 15)
            {
                return;
            }
            actor->data_4 = 1;
            actor->data_3 = 0;
            actor->frame_num = 4;
            if(is_sprite_on_screen(0x2b, 6, actor->x, actor->data_2) != 0)
            {
                play_sfx(0x15);
            }
            return;
        }
        
        if(actor->frame_num < 6)
        {
            actor->frame_num = actor->frame_num + 1;
            return;
        }
        
        actor->y = actor->y + 1;
        if(is_sprite_on_screen(0x2b, 6, actor->x, actor->y) == 0)
        {
            actor->y = actor->data_2;
            actor->data_4 = 0;
            actor->frame_num = 0;
            return;
        }
    }
    else
    {
        actor->frame_num = acid_frame_num_tbl[actor->data_3];
        actor->data_3++;
        if(actor->data_3 == 6)
        {
            actor->data_3 = 0;
        }
    }
    return;
}

const static uint8 alien_eating_space_plant_f_num_tbl[] = {5, 6, 7, 8};
const static uint8 alien_eating_space_plant_f_num_2_tbl[] = {
        1, 1, 1, 1, 1, 1, 1, 2,
        3, 4, 1, 1, 1, 1, 1, 1
};
void actor_wt_alien_eating_space_plant(ActorData *actor)
{
    if(actor->data_3 != 0)
    {
        actor->data_3--;
        actor->frame_num = 1;
        if(actor->data_3 != 0)
        {
            return;
        }
        actor->frame_num = 0;
    }
    
    if(actor->frame_num == 0 && actor->data_5 == 0)
    {
        display_actor_sprite_maybe(0xba, alien_eating_space_plant_f_num_tbl[actor->data_1 & 3], actor->x + 1 + 1, actor->y - 3, 0);
        actor->data_1++;
    }
    
    if(actor->data_5 != 0)
    {
        actor->frame_num = alien_eating_space_plant_f_num_2_tbl[actor->data_5 - 1];
        if(actor->data_5 != 0x10)
        {
            actor->data_5++;
        }
        else
        {
            finished_level_flag_maybe = 1;
        }
    }
    
    if(is_sprite_on_screen(0xba, 1, actor->x, actor->y) == 0)
    {
        actor->data_3 = 0x1e;
        actor->data_5 = 0;
        actor->frame_num = 1;
    }
    return;
}

void actor_wt_angry_moon(ActorData *actor)
{
    actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
    if(actor->data_3 == 0)
    {
        actor->data_2++;
        if(actor->x >= player_x_pos)
        {
            actor->frame_num = actor->data_2 & 1;
        }
        else
        {
            actor->frame_num = (actor->data_2 & 1) + 2;
        }
    }
    return;
}

const static sint8 red_jumper_tbl[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1,
        0, 1, -2, 2, -2, 2, -2, 2,
        -2, 2, -1, 2, -1, 2, -1, 2,
        0, 2, 0, 2, 1, 1, 1, 1
};

void actor_wt_big_red_jumper(ActorData *actor)
{
    if(actor->data_2 >= 5)
    {
        if(actor->data_2 != 14 || is_sprite_on_screen(0x65, 0, actor->x, actor->y) == 0)
        {
            if(actor->data_2 > 0x10 && actor->data_2 < 0x27)
            {
                if(actor->data_1 != 0 || sprite_blocking_check(2, 0x65, 0, actor->x - 1, actor->y) != NOT_BLOCKED)
                {
                    if(actor->data_1 == 3 && sprite_blocking_check(3, 0x65, 0, actor->x + 1, actor->y) == NOT_BLOCKED)
                    {
                        actor->x = actor->x + 1;
                    }
                }
                else
                {
                    actor->x = actor->x - 1;
                }
            }
        }
        else
        {
            play_sfx(0x1f);
        }
    }
    else
    {
        if(actor->x <= player_x_pos)
        {
            actor->data_1 = 3;
        }
        else
        {
            actor->data_1 = 0;
        }
    }
    
    if (actor->data_2 <= 0x27) //goto loc_15546;
    {
// node 00015546-0001561d #insn=6 use={} def={} in={} out={} pred={ 153F7} CONDJUMP target=0001566d follow=0001561f
        //loc_15546:
        //si = * ((actor->data_2 << 1) + word_28356);
        sint8 si = red_jumper_tbl[actor->data_2];
        if(si == -1)
        {
            if(sprite_blocking_check(0, 0x65, 0, actor->x, actor->y - 1) != 0)
            {
                actor->data_2 = 0x22;
            }
            else
            {
                actor->y = actor->y - 1;
            }
        }
        if(si == -2)
        {

            if(sprite_blocking_check(0, 0x65, 0, actor->x, actor->y - 1) != 0)
            {
                actor->data_2 = 0x22;
            }
            else
            {
                actor->y = actor->y - 1;
            }

            if(sprite_blocking_check(0, 0x65, 0, actor->x, actor->y - 1) != 0)
            {
                actor->data_2 = 0x22;
            }
            else
            {
                actor->y = actor->y - 1;
            }
        }
        if(si == 1)
        {

            if(sprite_blocking_check(1, 0x65, 0, actor->x, actor->y + 1) == 0)
            {
                actor->y = actor->y + 1;
            }
        }
        if (si != 2) //goto loc_1566D;
        {
            // node 0001566d-0001568e #insn=6 use={} def={} in={} out={} pred={  15546} FALLTHROUGH follow=00015698
            //loc_1566D:

            actor->frame_num = actor->data_1 + red_jumper_tbl[actor->data_2 + 1];
            if(actor->data_2 < 0x27)
            {
                actor->data_2 = actor->data_2 + 2;
            }
            return;
        }
        else
        {
            //0001561f
            // node 0001561f-0001563b #insn=3 use={} def={ax, bx} in={} out={ax} pred={ 15546} CONDJUMP target=00015662 follow=0001563d

            if (sprite_blocking_check(1, 0x65, 0, actor->x, actor->y - 1) != NOT_BLOCKED) //goto loc_15662;
            {
                // node 00015662-0001566b #insn=3 use={} def={bx} in={ax} out={ax} pred={ 1561F} JUMP target=00015698
                //loc_15662:
                actor->data_2 = 0;
                //goto loc_15698;
                return;
            }
            else
            {
                // 0001563d
                // node 0001563d-0001565d #insn=4 use={} def={ax, bx} in={} out={ax} pred={ 1561F} CONDJUMP target=00015662 follow=0001550f

                actor->y = actor->y + 1;
                if (sprite_blocking_check(1, 0x65, 0, actor->x, actor->y - 1) != NOT_BLOCKED) //goto loc_15662;
                {
                    // node 00015662-0001566b #insn=3 use={} def={bx} in={ax} out={ax} pred={ 1563D} JUMP target=00015698
                   // loc_15662:
                    actor->data_2 = 0;
                    //goto loc_15698;
                    return;
                }
                else
                {
                    // 0001550f
                    // node 0001550f-00015516 #insn=3 use={} def={bx} in={} out={} pred={ 154ED 1563D} JUMP target=0001566d
                   // loc_1550F:
                    actor->y = actor->y + 1;
                 //   goto loc_1566D;
                    // node 0001566d-0001568e #insn=6 use={} def={} in={} out={} pred={ 1550F } FALLTHROUGH follow=00015698
                  //  loc_1566D:

                    actor->frame_num = actor->data_1 + red_jumper_tbl[actor->data_2 + 1];
                    if(actor->data_2 < 0x27)
                    {
                        actor->data_2 = actor->data_2 + 2;
                    }
                    return;
                }
            }
        }
    }
    else
    {
        //﻿000154d2
        // node 000154d2-000154eb #insn=1 use={bx} def={ax} in={bx} out={} pred={ 153F7} CONDJUMP target=00015519 follow=000154ed
        if (sprite_blocking_check(1, 0x65, 0, actor->x, actor->y + 1) != NOT_BLOCKED) //goto loc_15519;
        {
            // node 00015519-00015537 #insn=5 use={} def={} in={} out={} pred={  154ED} FALLTHROUGH follow=00015698
           // loc_15519:

            actor->data_2 = 0;
            if(is_sprite_on_screen(0x65, 0, actor->x, actor->y) != 0)
            {
                play_sfx(0x20);
            }
            return;
        }
        else
        {
            // 00154ed
            // node 000154ed-0001550d #insn=3 use={} def={ax, bx} in={} out={} pred={ 154D2} CONDJUMP target=00015519 follow=0001550f

            actor->y = actor->y + 1;
            if (sprite_blocking_check(1, 0x65, 0, actor->x, actor->y + 1) != NOT_BLOCKED) //goto loc_15519;
            {
                // node 00015519-00015537 #insn=5 use={} def={} in={} out={} pred={  154ED} FALLTHROUGH follow=00015698
              //  loc_15519:

                actor->data_2 = 0;
                if(is_sprite_on_screen(0x65, 0, actor->x, actor->y) != 0)
                {
                    play_sfx(0x20);
                }
            }
            else
            {
                // 0001550f
                // node 0001550f-00015516 #insn=3 use={} def={bx} in={} out={} pred={ 154ED } JUMP target=0001566d
             //   loc_1550F:
                actor->y = actor->y + 1;
             //   goto loc_1566D;
                // node 0001566d-0001568e #insn=6 use={} def={} in={} out={} pred={ 1550F } FALLTHROUGH follow=00015698
             //   loc_1566D:

                actor->frame_num = actor->data_1 + red_jumper_tbl[actor->data_2 + 1];
                if(actor->data_2 < 0x27)
                {
                    actor->data_2 = actor->data_2 + 2;
                }
                return;
            }
        }
    }

// node 00015698-0001569a #insn=3 use={ax} def={si} in={ax} out={} pred={ 15519 15662 15662 1566D} RETURN
   // loc_15698:
    /* pop  */
}

const static uint8 big_red_plant_frame_num_tbl[] ={0, 2, 1, 0, 1};
void actor_wt_big_red_plant(ActorData *actor)
{
    if(actor->has_moved_right_flag > 0 && actor->has_moved_right_flag < 7)
    {
        return;
    }
    
    if(actor->data_3 != 0)
    {
        actor->data_3--;
        if((actor->data_3 & 1) != 0)
        {
            actor_tile_display_func_index = 2;
        }
        return;
    }
    
    if (struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) == 0)
    {
        if(actor->data_2 != 0)
        {
            actor->frame_num = 1;
            actor->data_2--;
        }
        else
        {
            actor->frame_num = big_red_plant_frame_num_tbl[actor->data_1];
            actor->data_1++;
            if(actor->data_1 == 2 && actor->has_moved_left_flag == 0)
            {
                actor_toss_add_new(0x56, actor->x + 2, actor->y - 5);
                play_sfx(0x2f);
            }

            if(actor->data_1 == 5)
            {
                actor->data_2 = 0x64;
                actor->data_1 = 0;
                actor->has_moved_left_flag = 0;
            }
        }
    }
    else
    {
        actor->data_3 = 15;
        actor->data_5++;
        if (actor->data_5 == 2)
        {
            actor->is_deactivated_flag_maybe = 1;
            explode_effect_add_sprite(0x56, 0, actor->x + 2, actor->y - 5);
            explode_effect_add_sprite(0x56, 2, actor->x + 2, actor->y - 5);
            explode_effect_add_sprite(0x56, 4, actor->x + 2, actor->y - 5);
            explode_effect_add_sprite(0x56, 9, actor->x + 2, actor->y - 5);
            explode_effect_add_sprite(0x56, 3, actor->x + 2, actor->y - 5);
            explode_effect_add_sprite(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y);
        }
        else
        {
            if(actor->data_2 != 0)
            {
                actor->frame_num = 1;
                actor->data_2--;
            }
            else
            {
                actor->frame_num = big_red_plant_frame_num_tbl[actor->data_1];
                actor->data_1++;
                if(actor->data_1 == 2 && actor->has_moved_left_flag == 0)
                {
                    actor_toss_add_new(0x56, actor->x + 2, actor->y - 5);
                    play_sfx(0x2f);
                }

                if(actor->data_1 == 5)
                {
                    actor->data_2 = 0x64;
                    actor->data_1 = 0;
                    actor->has_moved_left_flag = 0;
                }
            }
        }
    }
    return;
}

void actor_wt_big_saw_blade(ActorData *actor)
{
    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    if(is_sprite_on_screen(actor->actorInfoIndex, 0, actor->x, actor->y) != 0)
    {
        play_sfx(0x23);
    }
    
    if(actor->data_1 != 0)
    {
        if(sprite_blocking_check(0, actor->actorInfoIndex, 0, actor->x, actor->y - 1) == NOT_BLOCKED)
        {
            actor->y = actor->y - 1;
        }
        else
        {
            actor->data_1 = 0;
        }
        return;
    }
    
    if(sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) == NOT_BLOCKED)
    {
        actor->y = actor->y + 1;
    }
    else
    {
        actor->data_1 = 1;
    }
    return;
}

void actor_wt_big_yellow_spike(ActorData *actor)
{
    if(actor->data_5 == 0)
    {
        if(actor->data_1 != 0)
        {
            if(sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
            {
                actor->is_deactivated_flag_maybe = 1;
                effect_add_sprite(0x61, 6, actor->x, actor->y, 1, 3);
                play_sfx(0x1b);
                actor_tile_display_func_index = 1;
            }
        }
        else
        {
            if(actor->y < player_y_pos)
            {
                if(actor->x <= player_x_pos + 6)
                {
                    if(actor->x + 5 > player_x_pos)
                    {
                        actor->data_1 = 1;
                        actor->can_fall_down_flag = 1;
                    }
                }
            }
        }
    }
    else
    {
        actor_tile_display_func_index = 4;
    }
    
    if(actor->is_deactivated_flag_maybe != 0)
    {
        return;
    }
    if(struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
    {
        actor->data_2 = 3;
    }
    
    if(actor->data_2 != 0)
    {
        actor->data_2--;
        if(actor->data_2 == 0)
        {
            struct6_add_sprite(actor->x - 1, actor->y + 1);
            actor->is_deactivated_flag_maybe = 1;
            player_add_to_score(0xc8);
            
            explode_effect_add_sprite(actor->actorInfoIndex, 0, actor->x, actor->y);
        }
    }
    return;
}

const static uint8 blue_ball_frame_num_tbl[] = {
        2, 2, 2, 0, 3, 3, 3, 0, 0, 2, 2, 0, 0, 1, 1, 0, 1, 3, 3, 3, 0, 1, 1, 0, 1, 1, 1
};

const static uint8 blue_ball_moving_left_frame_num_tbl[] =  {7, 6, 5, 4};
const static uint8 blue_ball_moving_right_frame_num_tbl[] = {4, 5, 6, 7};

void actor_wt_blue_ball(ActorData *actor)
{
    if(actor->falling_counter != 0)
    {
        actor->data_1 = 0;
        actor->data_2 = 0x14;
        if(actor->falling_counter < 2)
        {
            actor->frame_num = 10;
            return;
        }

        if(actor->falling_counter < 2 || actor->falling_counter > 4)
        {
            actor->y = actor->y - 1;
            display_actor_sprite_maybe(0x56, 9, actor->x, actor->y - 2, 0);
            actor->frame_num = 10;
        }
        else
        {
            display_actor_sprite_maybe(0x56, 8, actor->x, actor->y - 2, 0);
            actor->frame_num = 10;
        }
        return;
    }

    if (actor->data_1 == 0)
    {
        actor->data_2 = actor->data_2 + 1;
        actor->frame_num = blue_ball_frame_num_tbl[actor->data_2];
        if (actor->data_2 == 0x1a)
        {
            actor->data_2 = 0;
            if (actor->y == player_y_pos || (sub_1106F() & 1) == 0)
            {
                if(actor->x < player_x_pos + 1 + 1)
                {
                    if(actor->x + 2 <= player_x_pos)
                    {
                        actor->data_1 = 2;
                        actor->data_2 = 0;
                        actor->frame_num = 3;
                        actor->data_3 = 6;
                    }
                }
                else
                {
                    actor->data_1 = 1;
                    actor->data_2 = 0;
                    actor->frame_num = 2;
                    actor->data_3 = 6;
                }
            }
        }
    }

    if(actor->data_3 != 0)
    {
        actor->data_3 = actor->data_3 - 1;
        return;
    }
    
    if (actor->data_1 == 1)
    {
        actor->x--;
        check_actor_move_left_or_right(actor, LEFT);

        if(actor->has_moved_left_flag != 0)
        {
            actor->frame_num = blue_ball_moving_left_frame_num_tbl[actor->data_2 & 3];
            actor->data_2 = actor->data_2 + 1;
            if(actor->data_2 == 0x10)
            {
                actor->data_1 = 0;
                actor->data_2 = 0;
            }
            return;
        }

        actor->data_1 = 0;
        actor->data_2 = 0;
        actor->frame_num = 0;
        return;
    }
    else
    {
        if (actor->data_1 != 2)
        {
            return;
        }

        actor->x++;
        check_actor_move_left_or_right(actor, RIGHT);
        if (actor->has_moved_right_flag != 0)
        {
            actor->frame_num = blue_ball_moving_right_frame_num_tbl[actor->data_2 & 3];
            actor->data_2 = actor->data_2 + 1;
            if(actor->data_2 == 12)
            {
                actor->data_1 = 0;
                actor->data_2 = 0;
            }
        }
        else
        {
            actor->data_1 = 0;
            actor->data_2 = 0;
            actor->frame_num = 0;
        }
    }

    return;
}

const static sint8 bird_swoop_y_offset_tbl[] = {
        2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2
};

void actor_wt_blue_bird(ActorData *actor)
{
    if(actor->data_1 == 0)
    {
        if(actor->x + 1 <= player_x_pos)
        {
            if(rand() % 10 != 0)
            {
                actor->data_2 = 4;
            }
            else
            {
                actor->data_2 = 5;
            }
        }
        else
        {
            if(rand() % 10 != 0)
            {
                actor->data_2 = 0;
            }
            else
            {
                actor->data_2 = 1;
            }
        }
        actor->frame_num = actor->data_2;
        actor->data_3 = actor->data_3 + 1;
        if(actor->data_3 == 0x1e)
        {
            actor->data_1 = 1;
            actor->data_3 = 0;
        }
    }
    else
    {
        if(actor->data_1 == 1)
        {
            actor->data_3 = actor->data_3 + 1;
            if(actor->data_3 != 0x14)
            {
                if((actor->data_3 & 1) != 0 && actor->data_3 < 10)
                {
                    actor->y = actor->y - 1;
                }
            }
            else
            {
                actor->data_3 = 0;
                actor->data_1 = 2;
                if(actor->x + 1 <= player_x_pos)
                {
                    actor->data_4 = 1;
                }
                else
                {
                    actor->data_4 = 0;
                }
            }

            if(actor->x + 1 <= player_x_pos)
            {
                actor->frame_num = (actor->data_3 & 1) + 6;
            }
            else
            {
                actor->frame_num = (actor->data_3 & 1) + 2;
            }
        }
        else
        {

            if(actor->data_1 == 2)
            {
                actor->data_3 = actor->data_3 + 1;
                if(actor->data_4 != 0)
                {
                    actor->frame_num = (actor->data_3 & 1) + 6;
                    actor->x = actor->x + 1;
                }
                else
                {
                    actor->frame_num = (actor->data_3 & 1) + 1 + 1;
                    actor->x = actor->x - 1;
                }
                actor->y = actor->y + bird_swoop_y_offset_tbl[actor->data_3 - 1];
                if(actor->data_3 == 15)
                {
                    actor->data_1 = 1;
                    actor->data_3 = 10;
                }
            }
        }
    }
    return;
}

void actor_wt_blue_cube_platform(ActorData *actor)
{
    if(sprite_blocking_check(1, 0xa3, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
    {
        actor->is_deactivated_flag_maybe = 1;
        explode_effect_add_sprite(0xa3, 1, actor->x, actor->y);
        
        explode_effect_add_sprite(0xa3, 2, actor->x, actor->y);
        play_sfx(0x19);
        actor_tile_display_func_index = 2;
        return;
    }
    
    if(actor->data_1 == 0)
    {
        actor->has_moved_left_flag = map_get_tile_cell(actor->x, actor->y - 1);
        actor->has_moved_right_flag = map_get_tile_cell(actor->x + 1, actor->y - 1);
        map_write_tile_cell(0x50, actor->x, actor->y - 1);
        map_write_tile_cell(0x50, actor->x + 1, actor->y - 1);
        actor->data_1 = 1;
    }
    
    if(actor->y - 2 == player_y_pos)
    {
        if(actor->x <= player_x_pos + 1 + 1)
        {
            if(actor->x + 1 >= player_x_pos)
            {
                actor->data_2 = 7;
            }
        }
    }
    
    if(actor->data_2 != 0)
    {
        actor->data_2--;
        if(actor->data_2 == 0)
        {
            actor->can_fall_down_flag = 1;
            map_write_tile_cell((uint16)actor->has_moved_left_flag, actor->x, actor->y - 1);
            map_write_tile_cell((uint16)actor->has_moved_right_flag, actor->x + 1, actor->y - 1);
        }
    }
    return;
}

void actor_wt_blue_mobile_trampoline_car(ActorData *actor)
{
    if(actor->data_1 == 0)
    {
        actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
        if(actor->data_2 == 0)
        {
            actor->x = actor->x - 1;
            check_actor_move_left_or_right(actor, LEFT);
            
            if(actor->has_moved_left_flag == 0)
            {
                actor->data_2 = 1;
            }
        }
        else
        {
            actor->x = actor->x + 1;
            check_actor_move_left_or_right(actor, RIGHT);
            
            if(actor->has_moved_right_flag == 0)
            {
                actor->data_2 = 0;
            }
        }
    }
    else
    {
        actor->frame_num = 2;
        actor->data_1--;
    }

    if(is_sprite_on_screen(0x10, 2, actor->x, actor->y) == 0)
    {
        actor->frame_num = 0;
        return;
    }
    return;
}

void actor_wt_blue_platform(ActorData *actor)
{
    actor->has_moved_left_flag = actor->has_moved_left_flag + 1;
    if(actor->data_1 == 0)
    {
        actor->data_1 = 1;
        map_write_row_of_tiles(0x3dd0, 4, actor->x, actor->y - 1);
    }
    else
    {
        if (actor->data_1 != 1 || actor->y - 2 != player_y_pos)
        {
            if (actor->data_1 == 2)
            {
                if ((actor->has_moved_left_flag & 1) != 0)
                {
                    actor->data_2 = actor->data_2 + 1;
                }

                if (actor->data_2 == 5)
                {
                    map_write_row_of_tiles(0, 4, actor->x, actor->y - 1);
                }

                if (actor->data_2 >= 5 && actor->data_2 < 8)
                {
                    actor_tile_display_func_index = 1;
                    display_actor_sprite_maybe(0x5b, 1, actor->x - (actor->data_2 - 5), actor->y, 0);

                    display_actor_sprite_maybe(0x5b, 2, actor->x + actor->data_2 - 3, actor->y, 0);
                }

                if (actor->data_2 == 7)
                {
                    actor->data_1 = 3;
                    actor->data_2 = 0;
                }
            }
        }
        else
        {
            if (actor->x > player_x_pos || actor->x + 3 < player_x_pos)
            {
                if (actor->x <= player_x_pos + 2)
                {
                    if (actor->x + 3 >= player_x_pos + 2)
                    {
                        actor->data_1 = 2;
                        actor->data_2 = 0;
                        sub_11062();
                    }
                }
            }
            else
            {
                actor->data_1 = 2;
                actor->data_2 = 0;
                sub_11062();
            }
        }
    }

    if(actor->data_1 != 3)
    {
        return;
    }
    actor_tile_display_func_index = 1;
    display_actor_sprite_maybe(0x5b, 1, actor->x + actor->data_2 - 2, actor->y, 0);
    
    display_actor_sprite_maybe(0x5b, 2, actor->x + 4 - actor->data_2, actor->y, 0);
    
    if((actor->has_moved_left_flag & 1) != 0)
    {
        actor->data_2 = actor->data_2 + 1;
    }

    if(actor->data_2 == 3)
    {
        actor_tile_display_func_index = 0;
        map_write_row_of_tiles(0, 4, actor->x, actor->y - 1);
        actor->data_1 = 0;
    }
}

void actor_wt_blue_turret_alien(ActorData *actor)
{
    actor->data_2--;
    if(actor->data_2 == 0)
    {
        actor->data_1++;
        actor->data_2 = 3;
        if(actor->data_1 != 3)
        {
            actor->frame_num++;
            switch (actor->frame_num - 2)
            {
                case 0:
                    actor_add_new(0x6d, actor->x - 1, actor->y - 1);
                    break;

                case 3:
                    actor_add_new(0x42, actor->x - 1, actor->y + 1);
                    break;

                case 6:
                    actor_add_new(0x44, actor->x + 1, actor->y + 1);
                    break;

                case 9:
                    actor_add_new(0x43, actor->x + 5, actor->y + 1);
                    break;

                case 12:
                    actor_add_new(0x6e, actor->x + 5, actor->y - 1);
                    break;

                default : break;
            }
        }
    }

    if(actor->data_1 == 0)
    {
        if(actor->y < player_y_pos - 2)
        {
            if(actor->y < player_y_pos - 2)
            {
                if(actor->x - 2 <= player_x_pos)
                {
                    if(actor->x + 3 >= player_x_pos)
                    {
                        if(actor->x - 2 < player_x_pos && actor->x + 3 >= player_x_pos)
                        {
                            actor->frame_num = 6;
                            actor->x = actor->data_3 + 1;
                        }
                    }
                    else
                    {
                        actor->frame_num = 9;
                        actor->x = actor->data_3 + 1;
                    }
                }
                else
                {
                    actor->frame_num = 3;
                    actor->x = actor->data_3;
                }

                if(actor->x - 2 == player_x_pos)
                {
                    actor->frame_num = 6;
                    actor->x = actor->data_3 + 1;
                }
            }
        }
        else
        {
            if(actor->x + 1 <= player_x_pos)
            {
                if(actor->x + 2 <= player_x_pos)
                {
                    actor->frame_num = 12;
                    actor->x = actor->data_3 + 1;
                }
            }
            else
            {
                actor->frame_num = 0;
                actor->x = actor->data_3;
            }
        }
    }

    if(actor->data_1 == 3)
    {
        actor->data_2 = 0x1b;
        actor->data_1 = 0;
    }

    if(actor->frame_num > 14)
    {
        actor->frame_num = 14;
    }

    return;
}

void actor_wt_bomb(ActorData *actor)
{
    if(actor->frame_num == 3)
    {
        actor->data_2 = actor->data_2 + 1;
        actor->data_1 = actor->data_1 + 1;
        if((actor->data_1 & 1) != 0 && actor->frame_num == 3)
        {
            actor_tile_display_func_index = 2;
        }

        if(actor->data_2 == 10)
        {
            actor->is_deactivated_flag_maybe = 1;
            exploding_balls_effect(actor->x - 2, actor->y + 1 + 1);
            actor_tile_display_func_index = 1;

            struct6_add_sprite(actor->x - 2, actor->y);

            if((actor->data_1 & 1) != 0 && actor->frame_num == 3)
            {
                display_actor_sprite_maybe(0x18, actor->frame_num, actor->x, actor->y, 2);
            }
        }
    }
    else
    {
        actor->data_1 = actor->data_1 + 1;
        if(actor->data_1 == 5)
        {
            actor->data_1 = 0;
            actor->frame_num++;
        }
    }

    if(sprite_blocking_check(1, 0x18, 0, actor->x, actor->y) != NOT_BLOCKED)
    {
        actor->y--;
    }
    return;

}

void actor_wt_bonus_bomb(ActorData *actor)
{
    if(actor->data_1 == 2)
    {
        struct6_add_sprite(actor->x - 2, actor->y);
        actor->is_deactivated_flag_maybe = 1;
        return;
    }
    
    if(actor->data_1 != 0)
    {
        actor->data_1 = actor->data_1 + 1;
    }
    
    if(actor->data_1 == 0)
    {
        if(struct6_1B4FC(0x39, 0, actor->x, actor->y) != 0)
        {
            actor->data_1 = 1;
        }
    }
}

void actor_wt_bonus_item(ActorData *actor)
{
    if(actor->data_1 != 0)
    {
        actor_tile_display_func_index = 4;
    }

    if (actor->data_4 == 0)
    {
        actor->frame_num = actor->frame_num + 1;
    }
    else
    {
        actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
        if (actor->data_3 != 0)
        {
            actor->frame_num = actor->frame_num + 1;
        }
    }

    if(actor->frame_num == actor->data_5)
    {
        actor->frame_num = 0;
    }

    if(actor->data_5 == 1 && actor->actorInfoIndex != 0xca && actor->data_4 == 0)
    {
        if((rand() & 0x3f) == 0)
        {
            effect_add_sprite(0x17, 8, rand() % actor->data_1 + actor->x, rand() % actor->data_2 + actor->y, 0, 1);
        }
    }
}

const static uint8  clam_trap_frame_num_tbl[] = {
        0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0
};

void actor_wt_clam_trap(ActorData *actor)
{
    if(actor->data_2 != 0)
    {
        if(actor->data_3 == 1)
        {
            play_sfx(0x28);
        }
        actor->frame_num = clam_trap_frame_num_tbl[actor->data_3];
        actor->data_3++;
        if(actor->data_3 >= 0x18)
        {
            byte_2E17C = 0;
        }
        
        if(actor->data_3 == 0x1b)
        {
            actor->data_3 = 0;
            actor->data_2 = 0;
            byte_2E17C = 0;
        }
        
        if(struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
        {
            player_add_to_score(0xfa);
            
            explode_effect_add_sprite(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y);
            actor->is_deactivated_flag_maybe = 1;
            byte_2E17C = 0;
        }
    }
    else
    {
        if(struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
        {
            player_add_to_score(0xfa);
            
            explode_effect_add_sprite(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y);
            actor->is_deactivated_flag_maybe = 1;
        }
    }
}

void actor_wt_container(ActorData *actor)
{
    if(struct6_1B4FC(0x1d, 0, actor->x, actor->y) != 0)
    {
        actor_explode_container(actor);
        player_add_to_score(0x640);
        
        actor_add_new(0xb5, actor->x, actor->y);
    }
}

void actor_wt_crate_bomb_box(ActorData *actor)
{
    if(actor->data_4 == 0)
    {
        map_write_row_of_tiles(0x50, 4, actor->x, actor->y - 2);
        actor->data_4 = 1;
        return;
    }
    
    if(sprite_blocking_check(1, 0x82, 0, actor->x, actor->y + 1) == 0)
    {
        
        map_write_row_of_tiles(0, 4, actor->x, actor->y - 2);
        
        actor->y = actor->y + 1;
        if(sprite_blocking_check(1, 0x82, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
        {
            
            map_write_row_of_tiles(0x50, 4, actor->x, actor->y - 2);
        }
        return;
    }
    
    if(is_sprite_on_screen(0x82, 0, actor->x, actor->y) == 0)
    {
        return;
    }
    
    if(struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
    {
        
        actor->data_5 = 1;
        actor->has_moved_right_flag = 0xf1f1;
    }
    
    if(actor->data_5 != 0)
    {
        actor->data_5--;
        return;
    }
    
    actor->is_deactivated_flag_maybe = 1;
    if(actor->has_moved_right_flag == 0xf1f1)
    {
        struct6_add_sprite(actor->x - 1, actor->y - 1);
        return;
    }
    
    map_write_row_of_tiles(0, 4, actor->x, actor->y - 2);
    
    actor_add_new(0x7c, actor->x, actor->y);
    actor_tile_display_func_index = 2;
    
    explode_effect_add_sprite(0x83, 0, actor->x - 1, actor->y + 3);
    explode_effect_add_sprite(0x83, 1, actor->x, actor->y - 1);
    explode_effect_add_sprite(0x83, 2, actor->x + 1, actor->y);
    explode_effect_add_sprite(0x83, 3, actor->x, actor->y);
    explode_effect_add_sprite(0x83, 4, actor->x + 3, actor->y + 1 + 1);
    explode_effect_add_sprite(0x83, 5, actor->x, actor->y);
    explode_effect_add_sprite(0x83, 6, actor->x + 5, actor->y + 5);

    play_sfx(0x19);
}

void actor_wt_cyan_spitting_plant(ActorData *actor)
{
    actor->data_4++;
    if(actor->data_4 == 0x32)
    {
        actor->data_4 = 0;
        actor->frame_num = 0;
    }

    if(actor->data_4 == 0x2a)
    {
        actor->frame_num = 1;
    }

    if(actor->data_4 == 0x2d)
    {
        actor->frame_num = 2;
        if(actor->data_5 != 2)
        {
            actor_add_new(0x6e, actor->x + 4, actor->y - 1);
        }
        else
        {
            actor_add_new(0x6d, actor->x - 1, actor->y - 1);
        }
    }
}

void actor_wt_destructable_pedestal(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    int i=0;
    for(; i < actor->data_1;i++)
    {
        display_actor_sprite_maybe(0xc0, 1, actor->x, actor->y - i, 0);
    }

    display_actor_sprite_maybe(0xc0, 0, actor->x - 2, actor->y - i, 0);

    map_write_row_of_tiles(0x48, 5, actor->x - 2, actor->y - i);

    if(actor->data_2 == 0)
    {
        if(struct6_1B4FC(0xc0, 1, actor->x, actor->y) != 0)
        {
            actor->data_2 = 3;
        }
    }

    if(actor->data_2 > 1)
    {
        actor->data_2--;
    }

    if(actor->data_2 != 1)
    {
        return;
    }
    actor->data_2 = 3;
    map_write_row_of_tiles(0, 5, actor->x - 2, actor->y - i);

    actor->data_1--;
    if(actor->data_1 != 1)
    {
        explode_effect_add_sprite(0xc0, 1, actor->x, actor->y);
        effect_add_sprite(0x61, 6, actor->x - 1, actor->y + 1, 1, 1);
    }
    else
    {
        actor->is_deactivated_flag_maybe = 1;
        explode_effect_add_sprite(0xc0, 0, actor->x, actor->y);
    }
}

void actor_wt_door(ActorData *actor)
{
    if (actor->has_moved_left_flag != 0)
    {
        return;
    }

    actor->has_moved_left_flag = 1;

    actor->data_1 = map_get_tile_cell(actor->x + 1, actor->y);
    actor->data_2 = map_get_tile_cell(actor->x + 1, actor->y - 1);
    actor->data_3 = map_get_tile_cell(actor->x + 1, actor->y - 2);
    actor->data_4 = map_get_tile_cell(actor->x + 1, actor->y - 3);
    actor->data_5 = map_get_tile_cell(actor->x + 1, actor->y - 4);

    for(int si=0; si < 5; si++)
    {
        map_write_tile_cell(0x3dc8, actor->x + 1, actor->y - si);
    }

    return;
}

void actor_wt_dragonfly(ActorData *actor)
{
    if(actor->data_1 != 0)
    {
        if(sprite_blocking_check(3, 0x81, 0, actor->x + 1, actor->y) == NOT_BLOCKED)
        {
            actor->x = actor->x + 1;
            actor->data_2 = (actor->data_2 ? -1 : 0) + 1;
            actor->frame_num = actor->data_2 + 2;
        }
        else
        {
            actor->data_1 = 0;
        }
        return;
    }

    if(sprite_blocking_check(2, 0x81, 0, actor->x - 1, actor->y) == NOT_BLOCKED)
    {
        actor->x = actor->x - 1;
        actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    }
    else
    {
        actor->data_1 = 1;
    }
}

void actor_wt_egg_head(ActorData *actor)
{
    if(actor->data_2 == 0)
    {
        if(sub_1106F() % 0x46 == 0 && actor->data_3 == 0)
        {
            actor->data_3 = 2;
        }
        else
        {
            actor->frame_num = 0;
        }
    }
    else
    {
        actor->frame_num = 2;
    }

    if(actor->data_3 != 0)
    {
        actor->data_3--;
        actor->frame_num = 1;
    }

    if(actor->data_5 == 0 && actor->data_1 == 0)
    {
        if(actor->y <= player_y_pos)
        {
            if(actor->x - 6 < player_x_pos)
            {
                if(actor->x + 4 > player_x_pos)
                {
                    actor->data_1 = 1;
                    actor->data_2 = 0x14;
                    play_sfx(0x21);
                }
            }
        }
    }

    if(actor->data_2 > 1)
    {
        actor->data_2--;
        return;
    }

    if(actor->data_2 == 1)
    {
        actor->is_deactivated_flag_maybe = 1;
        actor_tile_display_func_index = 1;
        actor_add_new(0x41, actor->x, actor->y);

        effect_add_sprite(0x4c, 1, actor->x, actor->y - 1, 8, 5);
        effect_add_sprite(0x4d, 1, actor->x + 1, actor->y - 1, 2, 5);
        effect_add_sprite(0x84, 1, actor->x, actor->y, 3, 5);
        effect_add_sprite(0x85, 1, actor->x + 1, actor->y, 7, 5);
        play_sfx(0x22);
    }
}

void actor_wt_end_of_level_marker(ActorData *actor)
{
    if(actor->y <= player_y_pos && actor->data_1 == 0)
    {
        finished_level_flag_maybe = 1;
    }
    else
    {
        if(actor->y >= player_y_pos && actor->data_1 != 0)
        {
            finished_game_flag_maybe = 1;
        }
    }

    actor_tile_display_func_index = 1;
    return;
}

void actor_wt_energy_beam(ActorData *actor)
{
    actor->data_1 = 0;
    actor->data_4++;
    if(actor->data_4 == 3)
    {
        actor->data_4 = 0;
    }
    actor_tile_display_func_index = 1;
    if(energy_beam_enabled_flag == 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        return;
    }
    if (actor->data_5 != 0)
    {
        for(;;actor->data_1++)
        {
            if(player_check_collision_with_actor(actor->actorInfoIndex, 0, actor->x + actor->data_1, actor->y) != 0)
            {
                player_decrease_health();
                break;
            }

            if((map_get_tile_attr(actor->x + actor->data_1, actor->y) & TILE_ATTR_BLOCK_RIGHT) != 0)
            {
                break;
            }

            display_actor_sprite_maybe(actor->actorInfoIndex, actor->data_4, actor->x + actor->data_1, actor->y, 0);
        }
    }
    else
    {
        for(;;actor->data_1++)
        {
            if (player_check_collision_with_actor(actor->actorInfoIndex, 0, actor->x, actor->y - actor->data_1) != 0)
            {
                player_decrease_health();
                break;
            }

            if((map_get_tile_attr(actor->x, actor->y - actor->data_1) & TILE_ATTR_BLOCK_UP) != 0)
            {
                break;
            }

            display_actor_sprite_maybe(actor->actorInfoIndex, actor->data_4, actor->x, actor->y - actor->data_1, 0);
        }
    }
}

void actor_wt_extending_arrow(ActorData *actor)
{
    if(actor->data_1 >= 0x1f)
    {
        actor->data_1 = 0;
    }
    else
    {
        actor->data_1++;
    }
    
    if(actor->data_1 == 0x1d || actor->data_1 == 0x1a)
    {
        if(is_sprite_on_screen(actor->actorInfoIndex, 0, actor->x, actor->y) != 0)
        {
            play_sfx(9);
        }
    }
    
    if(actor->data_5 == 0)
    {
        if(actor->data_1 > 0x1c)
        {
            actor->x = actor->x + 1;
        }
        else
        {
            if(actor->data_1 > 0x19)
            {
                actor->x = actor->x - 1;
                return;
            }
        }
        return;
    }
    if(actor->data_1 > 0x1c)
    {
        actor->x = actor->x - 1;
        return;
    }
    if(actor->data_1 > 0x19)
    {
        actor->x = actor->x + 1;
        return;
    }
    return;
}

void actor_wt_frozen_duke_nukum(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    switch (actor->data_1) {
        case 0:
            if (!struct6_1B4FC(0xdd, 0, actor->x, actor->y)) {
                display_actor_sprite_maybe(0xdd, 0, actor->x, actor->y, 0);
            } else {
                explode_effect_add_sprite(0xdd, 6, actor->x, actor->y - 6);
                explode_effect_add_sprite(0xdd, 7, actor->x + 4, actor->y);
                explode_effect_add_sprite(0xdd, 8, actor->x, actor->y - 5);
                explode_effect_add_sprite(0xdd, 9, actor->x, actor->y - 4);
                explode_effect_add_sprite(0xdd, 10, actor->x + 5, actor->y - 6);
                explode_effect_add_sprite(0xdd, 11, actor->x + 5, actor->y - 4);
                play_sfx(0x32);
                actor->data_1 = 1;
                actor->x++;
            }
            break;
        case 1:
            actor->data_2++;
            if (actor->data_2 & 1) {
                actor->y--;
            }
            int data_5 = actor->data_5;
            actor->data_5++;
            display_actor_sprite_maybe(0xdd, (data_5 & 1) + 4, actor->x, actor->y + 5, 0);
            display_actor_sprite_maybe(0xdd, 2, actor->x, actor->y, 0);
            effect_add_sprite(0x61, 6, actor->x, actor->y + 6, 5, 1);

            if (actor->data_2 == 10) {
                actor->data_1 = 2;
                actor->data_2 = 0;
            }
            break;
        case 2:
            display_actor_sprite_maybe(0xdd, (actor->data_5 & 1) + 4, actor->x, actor->y + 5, 0);
            display_actor_sprite_maybe(0xdd, 1, actor->x, actor->y, 0);
            actor->data_5++;
            actor->data_2++;
            if (actor->data_2 == 0x1e) {
                cosmo_duke_dialog();
                actor->data_1 = 3;
                actor->data_2 = 0;
            }
            break;
        case 3:
            display_actor_sprite_maybe(0xdd, (actor->data_5 & 1) + 4, actor->x, actor->y + 5, 0);
            actor->data_5++;
            actor->data_2++;
            if (actor->data_2 < 10) {
                display_actor_sprite_maybe(0xdd, 1, actor->x, actor->y, 0);
            } else {
                display_actor_sprite_maybe(0xdd, 2, actor->x, actor->y, 0);
                effect_add_sprite(0x61, 6, actor->x, actor->y + 6, 5, 1);
            }
            if (actor->data_2 == 0xf) {
                actor->data_1 = 4;
                actor->data_2 = 0;
            }
            break;
        case 4:
            actor->data_2++;
            if (actor->data_2 == 1) {
                actor_add_new(0x52, actor->x, actor->y); //TODO make hamburger drop only after duke leaves the screen.
            }
            actor->y--;
            if (actor->data_2 < 0x33 && is_sprite_on_screen(0xdd, 2, actor->x, actor->y)) {
                int data_5 = actor->data_5;
                actor->data_5++;
                display_actor_sprite_maybe(0xdd, (data_5 & 1) + 4, actor->x, actor->y + 5, 0);
                display_actor_sprite_maybe(0xdd, 2, actor->x, actor->y, 0);
                effect_add_sprite(0x61, 6, actor->x, actor->y + 6, 5, 1);
                play_sfx(0x31);
            } else {
                actor->is_deactivated_flag_maybe = 1;
            }
            break;
        default:
            actor_tile_display_func_index = 1;
            break;
    }
}

void actor_wt_ghost(ActorData *actor)
{
    actor->data_4++;
    if(actor->data_4 % 3 == 0)
    {
        actor->data_1++;
    }
    
    if(actor->data_1 == 4)
    {
        actor->data_1 = 0;
    }
    if (player_direction == 0)
    {
        if (actor->x <= player_x_pos + 1 + 1 || player_hanging_on_wall_direction != 2 || right_key_pressed == 0)
        {
            if (actor->x <= player_x_pos)
            {
                if(cosmo_rand() % 0x23 != 0)
                {
                    actor->frame_num = 5;
                }
                else
                {
                    actor->frame_num = 7;
                }
                return;
            }
            else
            {
                actor->frame_num = actor->data_1 & 1;
                if(actor->data_1 != 0)
                {
                    return;
                }
                actor->x = actor->x - 1;
                if(actor->y < player_y_pos)
                {
                    actor->y = actor->y + 1;
                    return;
                }

                if(actor->y > player_y_pos)
                {
                    actor->y = actor->y - 1;
                }
                return;
            }
        }
        else
        {
            if(cosmo_rand() % 0x23 != 0)
            {
                actor->frame_num = 2;
            }
            else
            {
                actor->frame_num = 6;
            }
            return;
        }
    }
    else
    {
        if (actor->x >= player_x_pos || player_hanging_on_wall_direction != 3 || left_key_pressed == 0)
        {
            if (actor->x >= player_x_pos)
            {
                if(cosmo_rand() % 0x23 != 0)
                {
                    actor->frame_num = 2;
                }
                else
                {
                    actor->frame_num = 6;
                }
            }
            else
            {
                actor->frame_num = (actor->data_1 & 1) + 3;
                if (actor->data_1 == 0)
                {
                    actor->x = actor->x + 1;
                    if(actor->y < player_y_pos)
                    {
                        actor->y = actor->y + 1;
                        return;
                    }

                    if(actor->y > player_y_pos)
                    {
                        actor->y = actor->y - 1;
                    }
                }
                return;
            }
        }
        else
        {
            if(cosmo_rand() % 0x23 != 0)
            {
                actor->frame_num = 5;
            }
            else
            {
                actor->frame_num = 7;
            }
        }
    }
}

void actor_wt_green_plant(ActorData *actor)
{
    if(actor->data_2 != 0)
    {
        actor->y = actor->y + 1;
        actor->data_4 = actor->data_4 + 1;
        if(actor->data_4 == 7)
        {
            actor->data_2 = 0;
            actor->data_3 = 0;
            actor->data_1 = 12;
        }
        return;
    }

    if(actor->data_3 < actor->data_1)
    {
        actor->data_3 = actor->data_3 + 1;
        return;
    }

    actor->data_5 = (actor->data_5 ? -1 : 0) + 1;
    actor->frame_num = actor->frame_num + 1;
    if(actor->frame_num == 4)
    {
        actor->frame_num = 0;
    }

    if(actor->data_4 != 0)
    {
        if(actor->data_4 == 7)
        {
            play_sfx(0x35);
        }

        actor->data_4 = actor->data_4 - 1;
        actor->y = actor->y - 1;
    }

    if(struct6_1B4FC(0x91, 0, actor->x, actor->y) != 0)
    {
        actor->data_2 = 1;
    }
}

const static sint8 cabbage_ball_y_tbl[] = {-1, -1, 0, 0};

void actor_wt_green_pruny_cabbage_ball(ActorData *actor)
{
    if(actor->data_2 == 10 && actor->data_3 == 3)
    {
        if(sprite_blocking_check(1, 0x19, 0, actor->x, actor->y + 1) == NOT_BLOCKED)
        {
            if(actor->data_4 == 0)
            {
                actor->frame_num = 1;
            }
            else
            {
                actor->frame_num = 3;
            }
            return;
        }
    }

    if(actor->data_2 < 10)
    {
        if(sprite_blocking_check(1, 0x19, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
        {

            actor->data_2 = actor->data_2 + 1;
            if(actor->x <= player_x_pos)
            {
                actor->frame_num = 2;
                actor->data_4 = 2;
            }
            else
            {
                actor->frame_num = 0;
                actor->data_4 = 0;
            }
            return;
        }
    }

    if(actor->data_3 < 3)
    {
        actor->y = actor->y + cabbage_ball_y_tbl[actor->data_3];
        if(actor->data_4 == 0)
        {
            actor->x = actor->x - 1;
            check_actor_move_left_or_right(actor, LEFT);
        }
        else
        {
            actor->x = actor->x + 1;
            check_actor_move_left_or_right(actor, RIGHT);
        }

        actor->data_3++;
        if(actor->data_4 == 0)
        {
            actor->frame_num = 1;
        }
        else
        {
            actor->frame_num = 3;
        }
        return;
    }

    actor->data_2 = 0;
    actor->data_3 = 0;
    if(actor->x <= player_x_pos)
    {
        actor->frame_num = 2;
        actor->data_4 = 2;
    }
    else
    {
        actor->frame_num = 0;
        actor->data_4 = 0;
    }
}

void actor_wt_green_roamer_worm(ActorData *actor)
{
    if(actor->data_5 != 0)
    {
        int si = sub_1106F() & 3;
        if(si == 0)
        {
            if(sprite_blocking_check(0, 0x45, 0, actor->x, actor->y - 1) == 0)
            {
                actor->data_5 = 0;
                actor->data_1 = 0;
            }
        }
        if(si == 1)
        {
            if(sprite_blocking_check(1, 0x45, 0, actor->x, actor->y + 1) == 0)
            {
                actor->data_5 = 0;
                actor->data_1 = 1;
            }
        }
        if(si == 2)
        {
            if(sprite_blocking_check(2, 0x45, 0, actor->x - 1, actor->y) == 0)
            {
                actor->data_5 = 0;
                actor->data_1 = 2;
            }
        }
        if(si == 3)
        {
            if(sprite_blocking_check(3, 0x45, 0, actor->x + 1, actor->y) == 0)
            {
                actor->data_5 = 0;
                actor->data_1 = 3;
            }
        }
    }
    else
    {
        switch (actor->data_1)
        {
            case 0:

                if(sprite_blocking_check(0, 0x45, 0, actor->x, actor->y - 1) == NOT_BLOCKED)
                {
                    actor->y = actor->y - 1;
                }
                else
                {
                    actor->data_5 = 1;
                }
                actor->data_3 = 0;
                break;

            case 1:

                if(sprite_blocking_check(1, 0x45, 0, actor->x, actor->y + 1) == 0)
                {
                    actor->y = actor->y + 1;
                }
                else
                {
                    actor->data_5 = 1;
                }
                actor->data_3 = 4;
                break;

            case 2:

                if(sprite_blocking_check(2, 0x45, 0, actor->x - 1, actor->y) == 0)
                {
                    actor->x = actor->x - 1;
                }
                else
                {
                    actor->data_5 = 1;
                }
                actor->data_3 = 6;
                break;

            case 3:

                if(sprite_blocking_check(3, 0x45, 0, actor->x + 1, actor->y) == 0)
                {
                    actor->x = actor->x + 1;
                }
                else
                {
                    actor->data_5 = 1;
                }
                actor->data_3 = 2;
                break;
        }
    }

    actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
    actor->frame_num = actor->data_3 + actor->data_4;
}

const uint8 byte_28EFE[] = { 0, 4, 5, 6, 5, 4};

void actor_wt_hint_dialog(ActorData *actor)
{
    actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
    if(actor->data_4 != 0)
    {
        actor->data_3 = actor->data_3 + 1;
    }

    display_actor_sprite_maybe(0x7d, byte_28EFE[actor->data_3 % 6], actor->x, actor->y - 2, 0);

    actor->data_2 = actor->data_2 + 1;
    if(actor->data_2 == 4)
    {
        actor->data_2 = 1;
    }

    display_actor_sprite_maybe(0x7d, actor->data_2, actor->x, actor->y, 0);
    actor_tile_display_func_index = 1;

    if(player_check_collision_with_actor(0x7d, 0, actor->x, actor->y - 2) != 0)
    {
        word_32EAC = 1;
        if(game_play_mode != 0)
        {
            byte_2E21C = 1;
        }
        if((up_key_pressed != 0 && player_hoverboard_counter == 0) || byte_2E21C == 0)
        {
            play_sfx(0x1e);
            show_ingame_hint_dialog(actor->data_5);
        }
        byte_2E21C = 1;
    }
    return;
}

const static uint8 flame_frame_num_tbl[] = {
        0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 1, 0, 0
};
void actor_wt_horizontal_flame(ActorData *actor)
{
    if(actor->data_1 != 0)
    {
        actor->data_1--;
        actor_tile_display_func_index = 1;
    }
    else
    {
        actor->frame_num = flame_frame_num_tbl[actor->data_2];
        if(actor->frame_num == 2)
        {
            effect_add_sprite(0x61, 6, actor->x - actor->data_5, actor->y - 3, 1, 1);
            play_sfx(0x36);
        }
        actor->data_2++;
        if(actor->data_2 == 0x10)
        {
            actor->data_1 = 0x1e;
            actor->data_2 = 0;
        }
    }
    return;
}

void actor_wt_hoverboard(ActorData *actor)
{
    actor->frame_num = actor->frame_num + 1;
    actor->frame_num = actor->frame_num & 3;
    if(player_hoverboard_counter != 0)
    {
        actor->x = player_x_pos;
        actor->y = player_y_pos + 1;
        return;
    }
    
    actor->data_2++;
    if(actor->data_2 % 10 == 0)
    {
        if(sprite_blocking_check(1, 0x72, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
        {
            actor->y = actor->y - 1;
        }
        else
        {
            actor->y = actor->y + 1;
            if(sprite_blocking_check(1, 0x72, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
            {
                actor->y = actor->y - 1;
                return;
            }
        }
        return;
    }
    return;
}

void actor_wt_invisible_exit_marker_right(ActorData *actor)
{
    if(actor->x <= player_x_pos + 3)
    {
        finished_level_flag_maybe = 1;
        return;
    }
    actor_tile_display_func_index = 1;
    return;
}

const static uint8 jaws_and_tongue_frame_num_tbl[] = {2, 3, 4, 3};
void actor_wt_jaws_and_tongue(ActorData *actor)
{
    if(actor->data_1 == 0)
    {
        actor->data_2++;
    }

    if(actor->data_2 == 10)
    {
        actor->data_1 = 1;
        actor->data_2 = 11;
        actor->frame_num = 1;
        actor->data_5 = 1;
        play_sfx(0x26);
    }

    if(actor->frame_num != 0)
    {
        display_actor_sprite_maybe(0x95, jaws_and_tongue_frame_num_tbl[actor->data_3 & 3], actor->x + 6 - actor->data_5, actor->y - 3, 0);
        actor->data_3++;
    }

    if(is_sprite_on_screen(0x95, 1, actor->x, actor->y) == 0)
    {
        
        actor->frame_num = 0;
        actor->data_2 = 0;
        actor->data_1 = 0;
        actor->data_5 = 0;
    }
    actor_tile_display_func_index = 1;
    
    display_actor_sprite_maybe(actor->actorInfoIndex, 1, actor->x, actor->y, 0);
    
    if(actor->data_5 != 0 && actor->data_5 < 4)
    {
        actor->data_5++;
        return;
    }

    display_actor_sprite_maybe(actor->actorInfoIndex, 0, actor->x, actor->y - 1 - actor->data_5, 0);
    return;
}
const static sint16 bullet_head_y_tbl[] = {-2, -2, -2, -2, -1, -1, -1, 0, 0, 1, 1, 1, 2, 2, 2, 2};
void actor_wt_jumping_bullet_head(ActorData *actor)
{
    if(actor->data_2 != 0)
    {
        actor->x = actor->x + 1;
    }
    else
    {
        actor->x = actor->x - 1;
    }
    actor->y = actor->y + bullet_head_y_tbl[actor->data_3];
    actor->data_3++;
    if(actor->data_3 == 0x10)
    {
        actor->data_2 = (actor->data_2 ? -1 : 0) + 1;
        if(is_sprite_on_screen(0x2e, 0, actor->x, actor->y) != 0)
        {
            play_sfx(0x25);
        }
        actor->data_3 = 0;
    }
}

void actor_wt_mini_ghost(ActorData *actor)
{
    if(actor->data_4 != 0)
    {
        actor->data_4--;
        return;
    }
    
    if(actor->data_1 == 0)
    {
        if(sprite_blocking_check(1, 0x41, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
        {
            actor->can_fall_down_flag = 0;
            actor->data_1 = 1;
            actor->data_4 = 3;
            actor->data_2 = 4;
            actor->frame_num = 1;
            actor->data_3 = 1;
            if(is_sprite_on_screen(0x41, 0, actor->x, actor->y) != 0)
            {
                play_sfx(0x3b);
            }
            return;
        }
        
        if(actor->data_5 != 0)
        {
            actor->data_5--;
        }
        else
        {
            actor->frame_num = 1;
            if(actor->data_3 == 0)
            {
                actor->data_4++;
                return;
            }
        }
    }
    else
    {
        
        if(actor->data_1 == 1)
        {
            actor->y = actor->y - 1;
            actor->frame_num = 0;
            if(actor->data_2 == 4)
            {
                if(is_sprite_on_screen(0x41, 0, actor->x, actor->y) != 0)
                {
                    play_sfx(0x3a);
                }
            }
            
            actor->data_2--;
            if(actor->data_2 == 0)
            {
                actor->data_1 = 0;
                actor->data_5 = 3;
                actor->can_fall_down_flag = 1;
            }
        }
    }
    return;
}

void actor_wt_pink_eye_plant(ActorData *actor)
{
    actor_tile_display_func_index = actor->data_5;
    actor->data_2 = rand() % 0x28;
    if(actor->data_2 <= 0x25)
    {
        actor->data_2 = 0;
    }
    else
    {
        actor->data_2 = 3;
    }

    if(actor->x - 2 > player_x_pos)
    {
        actor->frame_num = actor->data_2;
        return;
    }

    if(actor->x + 1 >= player_x_pos)
    {
        actor->frame_num = actor->data_2 + 1;
    }
    else
    {
        actor->frame_num = actor->data_2 + 2;
    }

    return;
}

void actor_wt_pink_slug(ActorData *actor)
{
    if(actor->data_5 == 0)
    {
        actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
        if(actor->data_4 != 0)
        {
            return;
        }
    }
    if(rand() % 0x28 > 0x25)
    {

        if(actor->data_3 == 0 && actor->data_2 == 0)
        {
            actor->data_3 = 4;
        }
    }

    if(actor->data_3 != 0)
    {
        actor->data_3--;
        if(actor->data_3 == 2)
        {
            if(actor->data_1 == 0)
            {
                actor->frame_num = 2;
                return;
            }

            if(actor->data_2 == 0)
            {
                actor->frame_num = 5;
            }
            return;
        }

        if(actor->data_1 != 0)
        {
            actor->frame_num = 3;
        }
        else
        {
            actor->frame_num = 0;
        }
        return;
    }

    if(actor->data_1 == 0)
    {
        actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
        if(actor->frame_num != 0)
        {
            actor->x--;
            check_actor_move_left_or_right(actor, LEFT);

            if(actor->has_moved_left_flag == 0)
            {
                actor->data_1 = 1;
            }
        }
        return;
    }

    actor->data_2 = (actor->data_2 ? -1 : 0) + 1;
    if(actor->data_2 == 0)
    {
        actor->x = actor->x + 1;
        actor->frame_num = 1;
        check_actor_move_left_or_right(actor, RIGHT);

        if(actor->has_moved_right_flag == 0)
        {
            actor->data_1 = 0;
        }
        return;
    }

    actor->frame_num = actor->data_2 + 3;
}

void actor_wt_pipe_transit_direction(ActorData *actor)
{
    actor_tile_display_func_index = 1;
}

void actor_wt_plasma_energy_blue_sprite(ActorData *actor)
{
    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    if(actor->data_1 >= 0x3f)
    {
        actor->data_1 = 0;
    }
    else
    {
        actor->data_1++;
    }

    if(actor->data_1 > 0x32)
    {
        actor->y = actor->y + 2;
        if(actor->data_1 < 0x37)
        {
            actor->y = actor->y - 1;
        }
        actor_tile_display_func_index = 4;
        return;
    }

    if(actor->data_1 > 0x22)
    {
        if(actor->data_1 < 0x2f)
        {
            actor->y = actor->y - 1;
        }

        if(actor->data_1 < 0x2d)
        {
            actor->y = actor->y - 1;
        }
    }
}

void actor_wt_plasma_fireball(ActorData *actor)
{
    if(actor->data_1 == 0x1d)
    {
        play_sfx(0x24);
    }
    
    if(actor->data_1 >= 0x1e)
    {
        
        if(actor->data_5 != 0)
        {
            
            actor->x = actor->x + 1;
            
            actor->has_moved_right_flag = (sprite_blocking_check(3, actor->actorInfoIndex, 0, actor->x, actor->y) != NOT_BLOCKED ? -1 : 0) + 1;
            if(actor->has_moved_right_flag == 0)
            {
                actor->data_1 = 0;
                effect_add_sprite(0x61, 6, actor->x - 2, actor->y, 1, 1);
                
                actor->x = actor->data_2;
                actor->y = actor->data_3;
                play_sfx(0x1b);
            }
        }
        else
        {
            actor->x = actor->x - 1;
            
            actor->has_moved_left_flag = (sprite_blocking_check(2, actor->actorInfoIndex, 0, actor->x, actor->y) != NOT_BLOCKED ? -1 : 0) + 1;
            if(actor->has_moved_left_flag == 0)
            {
                actor->data_1 = 0;
                effect_add_sprite(0x61, 6, actor->x + 1, actor->y, 1, 1);
                
                actor->x = actor->data_2;
                actor->y = actor->data_3;
                play_sfx(0x1b);
            }
        }
    }
    else
    {
        actor->data_1++;
    }

    if(is_sprite_on_screen(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) == 0)
    {
        
        actor->data_1 = 0;
        actor->x = actor->data_2;
        actor->y = actor->data_3;
        return;
    }

    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    return;
}

void actor_wt_pneumatic_pipe(ActorData *actor)
{
    if(actor->data_2 != 0)
    {
        actor->data_1++;
        actor->data_3++;
        if((actor->data_3 & 1) == 0)
        {
            actor->frame_num = 0;
        }
        else
        {
            actor->frame_num = 4;
        }
        
        if(actor->data_1 == 4)
        {
            actor->data_1 = 1;
        }
        
        display_actor_sprite_maybe(0x69, actor->data_1, actor->x, actor->y + 3, 0);
        return;
    }
    return;
}

void actor_wt_projectile_flashing_ball(ActorData *actor)
{
    if(is_sprite_on_screen(0x44, 0, actor->x, actor->y) == 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        return;
    }

    if(actor->data_1 == 0)
    {
        actor->data_1 = 1;
        play_sfx(0x1a);
    }

    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;

    switch (actor->data_5)
    {
        case 0:
            actor->x = actor->x - 1;
            break;

        case 1:
            actor->x = actor->x - 1;
            actor->y = actor->y + 1;
            break;

        case 2:
            actor->y = actor->y + 1;
            break;

        case 3:
            actor->x = actor->x + 1;
            actor->y = actor->y + 1;
            break;

        case 4:
            actor->x = actor->x + 1;
            break;

        default : break;
    }
}

void actor_wt_question_mark_block(ActorData *actor)
{
    if(word_2E17E != 0)
    {
        actor->data_1 = 1;
        actor->update_while_off_screen_flag = 1;
    }
    
    if(actor->data_1 == 0)
    {
        return;
    }
    if((actor->data_1 & 1) != 0)
    {
        map_write_tile_cell(0x3df8, actor->x, actor->y - 1);
        
        map_write_tile_cell(0x3e00, actor->x + 1, actor->y - 1);
        
        map_write_tile_cell(0x3e08, actor->x, actor->y);
        
        map_write_tile_cell(0x3e10, actor->x + 1, actor->y);
    }
    
    if(sprite_blocking_check(0, actor->actorInfoIndex, 0, actor->x, actor->y - 1) != NOT_BLOCKED)
    {
        
        if((actor->data_1 & 1) == 0)
        {
            map_write_tile_cell(0x3e08, actor->x, actor->y - 1);
            map_write_tile_cell(0x3e10, actor->x + 1, actor->y - 1);
        }
        actor->is_deactivated_flag_maybe = 1;
    }
    else
    {
        if((actor->data_1 & 1) == 0)
        {
            effect_add_sprite(15, 4, actor->x - 1, actor->y - 1, 0, 1);
        }

        actor->data_1++;
        actor->y--;
    }
}

void actor_wt_red_blue_plant(ActorData *actor)
{
    actor_tile_display_func_index = actor->data_5;
    if(actor->data_2 == 1)
    {
        actor->frame_num = actor->frame_num + 1;
        if(actor->frame_num == 1)
        {
            play_sfx(0x3f);
        }
        
        if(actor->frame_num == 4)
        {
            actor->data_2 = 2;
        }
        return;
    }
    
    if(actor->data_2 == 2)
    {
        actor->frame_num = actor->frame_num - 1;
        if(actor->frame_num == 1)
        {
            actor->data_2 = 0;
            actor->data_1 = 1;
        }
        return;
    }
    
    if(actor->data_1 >= 0x10)
    {
        actor->data_1 = 0;
    }
    else
    {
        actor->data_1++;
    }
    
    if(actor->data_1 != 0)
    {
        actor->frame_num = 0;
    }
    else
    {
        actor->data_2 = 1;
    }
    return;
}

const uint8 word_28EE6[] = { 8, 9, 10, 10, 9, 8};
const uint8 word_28EF2[] = { 10, 9, 8, 8, 9, 10};

void actor_wt_red_chomper_alien(ActorData *actor)
{
    actor->data_4 = (actor->data_4 ? -1 : 0) + 1;

    if(sub_1106F() % 0x5f != 0)
    {
        if(sub_1106F() % 0x64 == 0)
        {
            actor->data_5 = 11;
        }
    }
    else
    {
        actor->data_5 = 10;
    }

    if(actor->data_5 < 11 && actor->data_5 != 0)
    {
        actor->data_5--;
        if(actor->data_5 <= 8)
        {

            if(actor->data_5 != 8)
            {

                actor->data_2 = (actor->data_2 ? -1 : 0) + 1;

                actor->frame_num = actor->data_2 + 6;
            }
            else
            {
                actor->frame_num = 5;
            }
        }
        else
        {
            actor->frame_num = 6;
        }
        if(actor->data_5 != 0)
        {
            return;
        }

        if((sub_1106F() & 1) == 0)
        {
            return;
        }

        if(actor->x < player_x_pos)
        {
            actor->data_1 = 1;
        }
        else
        {
            actor->data_1 = 0;
        }
    }
    else
    {

        if(actor->data_5 > 10)
        {
            if(actor->data_1 != 0)
            {
                actor->frame_num = word_28EF2[actor->data_5 - 11];
            }
            else
            {
                actor->frame_num = word_28EE6[actor->data_5 - 11];
            }

            actor->data_5++;
            if(actor->data_5 == 0x11)
            {
                actor->data_5 = 0;
            }
            return;
        }

        if(actor->data_1 == 0)
        {
            if(actor->data_4 != 0)
            {
                actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
                actor->x = actor->x - 1;
                check_actor_move_left_or_right(actor, LEFT);

                if(actor->has_moved_left_flag == 0)
                {
                    actor->data_1 = 1;
                    actor->frame_num = 4;
                }
            }
            return;
        }

        if(actor->data_4 != 0)
        {
            actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
            actor->frame_num = actor->data_3 + 2;
            actor->x++;
            check_actor_move_left_or_right(actor, RIGHT);

            if(actor->has_moved_right_flag == 0)
            {
                actor->data_1 = 0;
                actor->frame_num = 4;
            }
        }
    }
    return;
}

void actor_wt_retracting_spikes(ActorData *actor)
{
    actor->data_2++;
    if(actor->data_2 == 0x14)
    {
        actor->data_2 = 0;
    }
    
    if(actor->frame_num != 0 || actor->data_2 != 0)
    {
        
        if(actor->frame_num != 2 || actor->data_2 != 0)
        {
            
            if(actor->data_1 == 0)
            {
                
                if(actor->frame_num < 2)
                {
                    actor->frame_num = actor->frame_num + 1;
                }
            }
            else
            {
                if(actor->frame_num > 0)
                {
                    actor->frame_num = actor->frame_num - 1;
                }
            }
        }
        else
        {
            actor->data_1 = 1;
            play_sfx(9);
            actor_tile_display_func_index = 1;
        }
    }
    else
    {
        actor->data_1 = 0;
        play_sfx(9);
    }
    if(actor->frame_num == 2)
    {
        actor_tile_display_func_index = 1;
        return;
    }
    return;
}

void actor_wt_robot_with_blue_arc(ActorData *actor)
{
    static uint16 spark_frame_num = 0;

    actor_tile_display_func_index = 1;
    if (actor->data_2 == 0)
    {
        actor->data_5 = (actor->data_5 ? -1 : 0) + 1;
        actor->data_4++;
        if(actor->data_1 == 0)
        {

            if((actor->data_4 & 1) != 0)
            {
                actor->x++;
            }
            check_actor_move_left_or_right(actor, RIGHT);

            if(actor->has_moved_right_flag == 0)
            {
                actor->data_1 = 1;
            }
        }
        else
        {
            if((actor->data_4 & 1) != 0)
            {
                actor->x--;
            }
            check_actor_move_left_or_right(actor, LEFT);

            if(actor->has_moved_left_flag == 0)
            {
                actor->data_1 = 0;
            }
        }

        display_actor_sprite_maybe(0x5a, actor->data_5, actor->x, actor->y, 0);

        if(player_check_collision_with_actor(0x5a, 0, actor->x, actor->y) != 0)
        {
            player_decrease_health();
        }
        spark_frame_num++;

        int si;
        for(si=2; si < 21 && sprite_blocking_check(0, 0x5a, 2, actor->x + 1, actor->y - si) == NOT_BLOCKED; si++)
        {
            display_actor_sprite_maybe(0x5a, (spark_frame_num & 3) + 4, actor->x + 1, actor->y - si, 0);

            if(player_check_collision_with_actor(0x5a, 4, actor->x + 1, actor->y - si) != 0)
            {
                player_decrease_health();
            }
        }
        display_actor_sprite_maybe(0x5a, actor->data_5 + 1 + 1, actor->x + 1, actor->y - si + 1, 0);

        if(player_check_collision_with_actor(0x5a, 0, actor->x, actor->y + 1) != 0)
        {
            player_decrease_health();
        }

        if(struct6_1B4FC(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) != 0)
        {
            actor->data_2 = si;
        }
    }
    else
    {
        for(int si = 0; si < actor->data_2; si += 4)
        {
            struct6_add_sprite(actor->x, actor->y - si);
            actor_add_new(1, actor->x, actor->y - si);
        }

        actor->is_deactivated_flag_maybe = 1;
    }
}

void actor_wt_robotic_spike_ceiling(ActorData *actor)
{
    actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
    if(actor->data_3 == 0)
    {
        return;
    }
    if(actor->data_4 != 0)
    {
        actor->data_4--;
        return;
    }
    
    if(actor->data_2 == 1)
    {
        if(sprite_blocking_check(3, 0x50, 0, actor->x + 1, actor->y) != 0)
        {
            actor->data_4 = 4;
            actor->data_2 = 0;
        }
        else
        {
            if(sprite_blocking_check(3, 0x50, 0, actor->x + 1, actor->y - 1) != 0)
            {
                actor->x = actor->x + 1;
            }
            else
            {
                actor->data_4 = 4;
                actor->data_2 = 0;
            }
        }
        return;
    }
    
    if(sprite_blocking_check(2, 0x50, 0, actor->x - 1, actor->y) != 0)
    {
        actor->data_4 = 4;
        actor->data_2 = 1;
    }
    else
    {
        if(sprite_blocking_check(2, 0x50, 0, actor->x - 1, actor->y - 1) != 0)
        {
            actor->x = actor->x - 1;
        }
        else
        {
            actor->data_4 = 4;
            actor->data_2 = 1;
        }
    }
    
    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    return;
}

void actor_wt_robotic_spike_ground(ActorData *actor)
{
    actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
    if(actor->actorInfoIndex == 0x14)
    {
        actor->data_3 = 1;
        if(is_sprite_on_screen(actor->actorInfoIndex, 0, actor->x, actor->y) != 0)
        {
            play_sfx(0x23);
        }
    }
    
    if(actor->data_4 != 0)
    {
        actor->data_4--;
    }
    
    if(actor->data_3 == 0)
    {
        return;
    }
    if(actor->data_4 == 0)
    {
        if(actor->data_2 == 0)
        {
            actor->x = actor->x - 1;
            check_actor_move_left_or_right(actor, LEFT);
            
            if(actor->has_moved_left_flag == 0)
            {
                actor->data_2 = 1;
                actor->data_4 = actor->data_1;
            }
        }
        else
        {
            actor->x = actor->x + 1;
            check_actor_move_left_or_right(actor, RIGHT);
            
            if(actor->has_moved_right_flag == 0)
            {
                actor->data_2 = 0;
                actor->data_4 = actor->data_1;
            }
        }
    }
    
    actor->frame_num++;
    if(actor->frame_num > actor->data_5)
    {
        actor->frame_num = 0;
    }
    return;
}

void actor_wt_rocket(ActorData *actor)
{
    if(actor->data_1 != 0)
    {
        actor->data_1--;
        if(actor->data_1 >= 0x1e)
        {
            return;
        }
        if((actor->data_1 & 1) == 0)
        {

            effect_add_sprite(0x61, 6, actor->x + 1, actor->y + 1, 2, 1);
        }
        else
        {
            effect_add_sprite(0x61, 6, actor->x - 1, actor->y + 1, 8, 1);
        }
        return;
    }

    if(actor->data_2 != 0)
    {
        if(actor->data_2 > 7)
        {
            effect_add_sprite(0x61, 6, actor->x - 1, actor->y + 1, 7, 1);

            effect_add_sprite(0x61, 6, actor->x + 1, actor->y + 1, 3, 1);
            play_sfx(0x31);
        }

        if(actor->data_2 > 1)
        {
            actor->data_2--;
        }

        if(actor->data_2 < 10)
        {
            if(sprite_blocking_check(0, 0xbc, 0, actor->x, actor->y - 1) != 0)
            {
                actor->data_5 = 1;
            }
            else
            {
                actor->y = actor->y - 1;
            }

            if(is_sprite_on_screen(actor->actorInfoIndex, 0, actor->x, actor->y) != 0)
            {
                play_sfx(0x31);
            }
        }

        if(actor->data_2 < 5)
        {
            if(sprite_blocking_check(0, 0xbc, 0, actor->x, actor->y - 1) != 0)
            {
                actor->data_5 = 1;
            }
            else
            {
                actor->y = actor->y - 1;
            }

            actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
            display_actor_sprite_maybe(0xbc, actor->data_4 + 4, actor->x, actor->y + 6, 0);

            if(player_check_collision_with_actor(0xbc, 4, actor->x, actor->y + 6) != 0)
            {
                player_decrease_health();
            }

            if(actor->data_4 != 0)
            {
                effect_add_sprite(0x61, 6, actor->x, actor->y + 6, 5, 1);
            }
        }

        if(actor->x == player_x_pos)
        {
            if(actor->y - 7 <= player_y_pos)
            {
                if(actor->y - 4 >= player_y_pos)
                {
                    player_bounce_height_counter = 0x10;
                    player_bounce_flag_maybe = 1;
                    sub_11062();
                    player_spring_jump_flag = 0;
                    if(actor->y - 7 == player_y_pos)
                    {
                        player_y_pos = player_y_pos + 1;
                    }
                    if(actor->y - 6 == player_y_pos)
                    {
                        player_y_pos = player_y_pos + 1;
                    }
                    if(actor->y - 4 == player_y_pos)
                    {
                        player_y_pos = player_y_pos - 1;
                    }
                }
            }
        }

        if(actor->data_2 > 4 && (actor->data_2 & 1) != 0)
        {
            effect_add_sprite(0x61, 6, actor->x, actor->y + 1 + 1, 5, 1);
        }
    }

    if(actor->data_5 != 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        explode_effect_add_sprite(0xbc, 1, actor->x, actor->y);

        explode_effect_add_sprite(0xbc, 2, actor->x + 1, actor->y);

        explode_effect_add_sprite(0xbc, 3, actor->x + 1 + 1, actor->y);

        struct6_add_sprite(actor->x - 4, actor->y);

        struct6_add_sprite(actor->x + 1, actor->y);
        actor_tile_display_func_index = 2;
    }
    return;
}

void actor_wt_rubber_wall(ActorData *actor)
{
    if (actor->data_2 != 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        actor_tile_display_func_index = 1;
        explode_effect_add_sprite(0x40, 3, actor->x, actor->y - 8);

        explode_effect_add_sprite(0x40, 3, actor->x, actor->y - 7);

        explode_effect_add_sprite(0x40, 3, actor->x, actor->y - 6);

        explode_effect_add_sprite(0x40, 3, actor->x, actor->y);

        explode_effect_add_sprite(0x40, 3, actor->x + 1, actor->y);

        explode_effect_add_sprite(0x40, 3, actor->x + 1 + 1, actor->y);

        effect_add_sprite(0x61, 6, actor->x, actor->y, 1, 2);

        effect_add_sprite(0x61, 6, actor->x, actor->y, 2, 2);

        effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 2);

        effect_add_sprite(0x61, 6, actor->x, actor->y - 4, 1, 3);
        player_add_to_score(0x6400);

        actor_add_new(0xb8, actor->x - 2, actor->y - 9);

        actor_add_new(0xb8, actor->x + 1 + 1, actor->y - 9);
        play_sfx(0x19);
        return;
    }

    if (actor->has_moved_left_flag == 0)
    {
        actor->has_moved_left_flag = 1;
        for(int i=0; i < 9; i++)
        {
            map_write_tile_cell(0x3d88, actor->x + 1, actor->y - i);
        }
    }

    if(actor->data_1 != 0)
    {
        actor->data_1--;
        if((actor->data_1 & 1) != 0)
        {
            actor_tile_display_func_index = 2;
        }
    }
    
    if (struct6_1B4FC(0x40, 0, actor->x, actor->y) != 0 && actor->data_1 == 0)
    {
        actor->data_1 = 10;
        actor->frame_num++;

        if (actor->frame_num == 3)
        {
            actor->frame_num = 2;
            actor->data_2 = 1;
            for(int i=0; i < 9; i ++)
            {
                map_write_tile_cell(0, actor->x + 1, actor->y - i);
            }
        }
    }

    return;
}

void actor_wt_satellite(ActorData *actor)
{
    if(actor->data_2 != 0)
    {
        actor->data_2--;
        if(actor->data_2 != 0)
        {
            if((actor->data_2 & 1) != 0)
            {
                actor_tile_display_func_index = 2;
            }
            return;
        }
    }

    if(struct6_1B4FC(0x8f, 0, actor->x, actor->y) == 0)
    {
        return;
    }

    if(actor->data_1 == 0)
    {
        actor->data_1 = 1;
        actor->data_2 = 15;
        return;
    }

    actor->is_deactivated_flag_maybe = 1;
    actor_tile_display_func_index = 2;
    play_sfx(0x12);
    actor->data_1 = 1;

    for(; actor->data_1 < 9; actor->data_1++)
    {
        effect_add_sprite(0x61, 6, actor->x + 3, actor->y - 3, actor->data_1, 3);
    }

    exploding_balls_effect(actor->x, actor->y + 5);
    explode_effect_add_sprite(0x90, 0, actor->x, actor->y - 2);
    explode_effect_add_sprite(0x90, 1, actor->x + 1, actor->y - 2);
    explode_effect_add_sprite(0x90, 2, actor->x + 7, actor->y + 1 + 1);
    explode_effect_add_sprite(0x90, 3, actor->x + 3, actor->y - 2);
    explode_effect_add_sprite(0x90, 4, actor->x - 1, actor->y - 8);
    explode_effect_add_sprite(0x90, 5, actor->x + 1 + 1, actor->y + 3);
    explode_effect_add_sprite(0x90, 6, actor->x + 6, actor->y - 2);
    explode_effect_add_sprite(0x90, 7, actor->x - 4, actor->y + 1);

    actor_toss_add_new(0x52, actor->x + 4, actor->y);
}

void actor_wt_security_robot(ActorData *actor)
{
    if(actor->count_down_timer != 0)
    {
        return;
    }
    actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
    if(actor->data_3 != 0)
    {
        return;
    }
    if(brightness_effect_enabled_flag != 0)
    {
        if(sub_1106F() % 0x32 > 0x30)
        {
            if(actor->data_4 == 0)
            {
                actor->data_4 = 10;
            }
        }
    }
    
    if(actor->data_4 != 0)
    {
        actor->data_2 = (actor->data_2 ? -1 : 0) + 1;
        actor->data_4--;
        if(actor->data_4 == 1)
        {
            if(actor->x + 1 <= player_x_pos)
            {
                actor->data_1 = 1;
            }
            else
            {
                actor->data_1 = 0;
            }
            
            if(actor->data_1 == 0)
            {
                actor_add_new(0x6d, actor->x - 1, actor->y - 1);
            }
            else
            {
                actor_add_new(0x6e, actor->x + 3, actor->y - 1);
            }
        }
        
        if(actor->data_1 != 0)
        {
            if(actor->data_2 == 0)
            {
                actor->frame_num = 0;
            }
            else
            {
                actor->frame_num = 5;
            }
            return;
        }
        
        if(actor->data_2 == 0)
        {
            actor->frame_num = 2;
        }
        else
        {
            actor->frame_num = 6;
        }
    }
    else
    {
        
        if(actor->data_1 == 0)
        {
            actor->x = actor->x - 1;
            check_actor_move_left_or_right(actor, LEFT);
            
            if(actor->has_moved_left_flag != 0)
            {
                
                actor->data_2 = (actor->data_2 ? -1 : 0) + 1;
                actor->frame_num = actor->data_2 + 2;
            }
            else
            {
                actor->data_1 = 1;
                actor->frame_num = 4;
            }
            return;
        }
        actor->x = actor->x + 1;
        check_actor_move_left_or_right(actor, RIGHT);
        
        if(actor->has_moved_right_flag != 0)
        {
            actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
        }
        else
        {
            actor->data_1 = 0;
            actor->frame_num = 4;
        }
    }
}

void actor_wt_short_dialog(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    if(actor->data_2 == 0)
    {
        if(actor->y <= player_y_pos)
        {
            if(actor->y >= player_y_pos - 4)
            {
                end_game_dialog(actor->data_1);
                actor->data_2 = 1;
            }
        }
    }
}

void actor_wt_silver_robot(ActorData *actor)
{
    actor_tile_display_func_index = 3;
    if(actor->data_5 == 1)
    {
        actor_tile_display_func_index = 0;
    }
    
    if(actor->data_2 != 0)
    {
        actor->data_2--;
        actor_tile_display_func_index = 0;
        return;
    }
    
    if(actor->data_4 != 0)
    {
        actor->data_4--;
    }
    
    actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
    if(actor->data_1 == 0)
    {
        if(actor->y == player_y_pos)
        {
            if(actor->x - 3 == player_x_pos && actor->data_4 == 0)
            {
                actor->frame_num = 2;
                actor->data_2 = 8;
                push_player_around(7, 5, 2, 0x28, 0, 1);
                play_sfx(0x14);
                player_direction = 0x17;
                actor->data_4 = 3;
                actor_tile_display_func_index = 0;
                if(!speech_bubble_silver_robot_shown_flag)
                {
                    speech_bubble_silver_robot_shown_flag = true;
                    player_add_speech_bubble(UMPH);
                }
                return;
            }
        }
        
        if(actor->data_3 == 0)
        {
            return;
        }
        actor->x = actor->x - 1;
        check_actor_move_left_or_right(actor, LEFT);
        
        if(actor->has_moved_left_flag != 0)
        {
            actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
        }
        else
        {
            actor->data_1 = 1;
            actor->frame_num = (actor->x & 1) + 3;
        }
    }
    else
    {
        
        if(actor->y == player_y_pos)
        {
            if(actor->x + 4 == player_x_pos && actor->data_4 == 0)
            {
                actor->frame_num = 5;
                actor->data_2 = 8;
                push_player_around(3, 5, 2, 0x11, 0, 1);
                play_sfx(0x14);
                player_direction = 0;
                actor->data_4 = 3;
                actor_tile_display_func_index = 0;
                if(!speech_bubble_silver_robot_shown_flag)
                {
                    speech_bubble_silver_robot_shown_flag = true;
                    player_add_speech_bubble(UMPH);
                }
                return;
            }
        }
        
        if(actor->data_3 != 0)
        {
            actor->x++;
            check_actor_move_left_or_right(actor, RIGHT);
            
            if(actor->has_moved_right_flag != 0)
            {
                actor->frame_num = (actor->x & 1) + 3;
            }
            else
            {
                actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
                actor->data_1 = 0;
            }
        }
    }
    return;
}

void actor_wt_small_flame(ActorData *actor)
{
    actor->frame_num = actor->frame_num + 1;
    if(actor->frame_num == 6)
    {
        actor->frame_num = 0;
    }
    return;
}

void actor_wt_small_red_plant(ActorData *actor)
{
    if(actor->data_1 == 0)
    {
        if(actor->y > player_y_pos)
        {
            if(actor->x == player_x_pos)
            {
                actor->data_1 = 1;
            }
        }
    }
    
    if(actor->data_1 == 1)
    {
        actor->data_2++;
        if(actor->data_2 == 2)
        {
            actor->data_2 = 0;
            actor->frame_num = actor->frame_num + 1;
            if(actor->frame_num == 3)
            {
                actor->data_1 = 0;
                actor->frame_num = 0;
            }
            
            if(actor->frame_num == 1)
            {
                actor->x = actor->x - 1;
                play_sfx(0x3f);
            }
            
            if(actor->frame_num == 2)
            {
                actor->x = actor->x + 1;
            }
        }
    }
    return;
}

void actor_wt_smoke_rising(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    
    actor->data_1 = (uint16)(sub_1106F() & 0x1f);
    if(actor->data_1 == 0)
    {
        if(actor->data_5 == 0)
        {
            
            effect_add_sprite(0x62, 6, actor->x - 2, actor->y, 1, 1);
        }
        else
        {
            effect_add_sprite(0x61, 6, actor->x - 1, actor->y, 1, 1);
        }
    }
    return;
}

void actor_wt_spark(ActorData *actor)
{
    actor->data_5++;
    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    if((actor->data_5 & 1) != 0)
    {
        return;
    }
    if(actor->data_1 == 0)
    {
        actor->x = actor->x - 1;
        if(sprite_blocking_check(2, actor->actorInfoIndex, 0, actor->x - 1, actor->y) != NOT_BLOCKED)
        {
            actor->data_1 = 2;
            return;
        }
        
        if(sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) == NOT_BLOCKED)
        {
            actor->data_1 = 3;
        }
        return;
    }
    
    if (actor->data_1 != 1)
    {
        if(actor->data_1 == 2)
        {
            actor->y = actor->y - 1;
            if(sprite_blocking_check(0, actor->actorInfoIndex, 0, actor->x, actor->y - 1) != NOT_BLOCKED)
            {
                actor->data_1 = 1;
                return;
            }

            if(sprite_blocking_check(2, actor->actorInfoIndex, 0, actor->x - 1, actor->y) == NOT_BLOCKED)
            {
                actor->data_1 = 0;
                return;
            }
            return;
        }

        if(actor->data_1 == 3)
        {
            actor->y = actor->y + 1;
            if(sprite_blocking_check(1, actor->actorInfoIndex, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
            {
                actor->data_1 = 0;
                return;
            }

            if(sprite_blocking_check(3, actor->actorInfoIndex, 0, actor->x + 1, actor->y) == NOT_BLOCKED)
            {
                actor->data_1 = 1;
            }
            return;
        }
    }
    else
    {
        actor->x = actor->x + 1;
        if(sprite_blocking_check(3, actor->actorInfoIndex, 0, actor->x + 1, actor->y) != NOT_BLOCKED)
        {
            actor->data_1 = 3;
            return;
        }

        if(sprite_blocking_check(0, actor->actorInfoIndex, 0, actor->x, actor->y - 1) == NOT_BLOCKED)
        {
            actor->data_1 = 2;
        }
    }
}

void actor_wt_spear_vertical(ActorData *actor)
{
    if(actor->data_1 >= 0x1e)
    {
        actor->data_1 = 0;
    }
    else
    {
        actor->data_1++;
    }
    
    if(actor->data_1 > 0x16)
    {
        actor->y = actor->y - 1;
        return;
    }
    
    if(actor->data_1 > 14)
    {
        actor->y = actor->y + 1;
        return;
    }
    return;
}

void actor_wt_spring(ActorData *actor)
{
    if(actor->data_1 <= 0)
    {
        actor->frame_num = 0;
    }
    else
    {
        actor->frame_num = 1;
        actor->data_1--;
    }

    if(actor->data_5 != 0)
    {
        actor_tile_display_func_index = 4;
        if(actor->frame_num != 0)
        {
            actor->y = actor->data_4;
        }
        else
        {
            actor->y = actor->data_3;
        }
    }
    return;
}

void actor_wt_stone_head(ActorData *actor)
{
    actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
    if(actor->data_1 == 0)
    {
        if(actor->y < player_y_pos)
        {
            if(actor->x <= player_x_pos + 6)
            {
                if(actor->x + 7 > player_x_pos)
                {
                    actor->data_1 = 1;
                    actor->data_2 = actor->y;
                    actor->frame_num = 1;
                    return;
                }
            }
        }
        actor->frame_num = 0;
        return;
    }
    
    if(actor->data_1 == 1)
    {
        actor->frame_num = 1;
        actor->y = actor->y + 1;
        if(sprite_blocking_check(1, 0x2f, 0, actor->x, actor->y) != 0)
        {
            actor->data_1 = 2;
            if(is_sprite_on_screen(0x2f, 0, actor->x, actor->y) != 0)
            {
                play_sfx(0x25);
                
                effect_add_sprite(0x61, 6, actor->x + 1, actor->y, 2, 1);
                effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
                actor->y = actor->y - 1;
            }
            else
            {
                actor->y = actor->y - 1;
            }
            return;
        }
        
        actor->y = actor->y + 1;
        if(sprite_blocking_check(1, 0x2f, 0, actor->x, actor->y) != NOT_BLOCKED)
        {
            actor->data_1 = 2;
            play_sfx(0x25);
            
            effect_add_sprite(0x61, 6, actor->x + 1, actor->y, 2, 1);
            
            effect_add_sprite(0x61, 6, actor->x, actor->y, 8, 1);
            actor->y = actor->y - 1;
            return;
        }
    }
    else
    {
        
        if(actor->data_1 == 2)
        {
            actor->frame_num = 0;
            if(actor->y == actor->data_2)
            {
                actor->data_1 = 0;
            }
            else if(actor->data_4 != 0)
            {
                actor->y = actor->y - 1;
            }
        }
    }
    return;
}

uint8 suction_cup_enemy_check_if_blocked(ActorData *actor, uint8 blocking_check)
{
    if ((sub_1106F() & 1) == 0)
    {
        return 0;
    }

    if (blocking_check == 0)
    {
        for(int si=0;si < 15; si++)
        {
            if ((map_get_tile_attr(actor->x, actor->y - si - 4) & TILE_ATTR_BLOCK_LEFT) != 0 &&
                (map_get_tile_attr(actor->x + 1 + 1, actor->y - si - 4) & TILE_ATTR_BLOCK_LEFT) != 0)
            {
                return 1;
            }
        }
    }
    else if(blocking_check == 1)
    {
        for(int si=0; si < 15; si++)
        {
            if((map_get_tile_attr(actor->x, actor->y + si) & TILE_ATTR_BLOCK_DOWN) != 0 &&
               (map_get_tile_attr(actor->x + 1 + 1, actor->y + si) & TILE_ATTR_BLOCK_DOWN) != 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

void actor_wt_suction_cup_alien_enemy(ActorData *actor)
{
    actor->data_4 = (actor->data_4 ? -1 : 0) + 1;
    if (actor->data_1 == 0)
    {
        if (actor->data_2 == 0)
        {
            if(actor->data_4 != 0)
            {
                actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
                actor->frame_num = actor->data_3;
            }

            uint8 tile_attr = map_get_tile_attr(actor->x - 1, actor->y + 1);
            if (sprite_blocking_check(2, 0x6a, 0, actor->x - 1, actor->y) != NOT_BLOCKED
            || ((tile_attr & 1u) ? -1 : 0) + 1 != 0)
            {
                if (suction_cup_enemy_check_if_blocked(actor, 0) == 0)
                {
                    actor->data_1 = 1;
                    actor->data_2 = 0;
                }
                else
                {
                    actor->data_2 = 2;
                    actor->frame_num = 9;
                }
                return;
            }
            else
            {
                if (sub_1106F() % 0x32 == 0)
                {
                    if (suction_cup_enemy_check_if_blocked(actor, 0) == 0)
                    {
                        actor->data_1 = 1;
                        actor->data_2 = 0;
                    }
                    else
                    {
                        actor->data_2 = 2;
                        actor->frame_num = 9;
                    }
                }
                else
                {
                    if(actor->data_4 != 0)
                    {
                        actor->x = actor->x - 1;
                    }
                }
                return;
            }
        }
        else
        {
            if(actor->data_2 != 1)
            {
                if(actor->data_2 == 2)
                {
                    if(sprite_blocking_check(0, 0x6a, 0, actor->x, actor->y - 1) == 0)
                    {
                        actor->y = actor->y - 1;
                    }
                    else
                    {
                        actor->data_2 = 1;
                    }

                    if(sprite_blocking_check(0, 0x6a, 0, actor->x, actor->y - 1) != NOT_BLOCKED)
                    {
                        actor->data_2 = 1;
                    }
                    else
                    {
                        actor->y = actor->y - 1;
                    }
                    return;
                }

                if(actor->data_2 != 3)
                {
                    return;
                }
                if(sprite_blocking_check(1, 0x6a, 0, actor->x, actor->y + 1) == 0)
                {
                    actor->y = actor->y + 1;
                }
                else
                {
                    actor->data_2 = 0;
                }

                if(sprite_blocking_check(1, 0x6a, 0, actor->x, actor->y + 1) != NOT_BLOCKED)
                {
                    actor->data_2 = 0;
                }
                else
                {
                    actor->y = actor->y + 1;
                }
                return;
            }
            if(actor->data_4 != 0)
            {
                actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
                actor->frame_num = actor->data_3 + 4;
            }

            BlockingType blocking_check = sprite_blocking_check(2, 0x6a, 0, actor->x - 1, actor->y);
            uint8 tile_attr = map_get_tile_attr(actor->x - 1, actor->y - 4);

            if(blocking_check == SLOPE)
            {

                if(actor->data_4 != 0)
                {
                    actor->y = actor->y - 1;
                    actor->x = actor->x - 1;
                    return;
                }
            }
            if (blocking_check != NOT_BLOCKED || ((tile_attr & 4) ? -1 : 0) + 1 != 0)
            {
                if (suction_cup_enemy_check_if_blocked(actor, 1) == 0)
                {
                    actor->data_1 = 1;
                    actor->data_2 = 1;
                }
                else
                {
                    actor->data_2 = 3;
                    actor->frame_num = 9;
                }
            }
            else
            {
                if (sub_1106F() % 0x32 != 0)
                {
                    if(actor->data_4 != 0)
                    {
                        actor->x = actor->x - 1;
                    }
                }
                else
                {
                    if (suction_cup_enemy_check_if_blocked(actor, 1) == 0)
                    {
                        actor->data_1 = 1;
                        actor->data_2 = 1;
                    }
                    else
                    {
                        actor->data_2 = 3;
                        actor->frame_num = 9;
                    }
                }
            }
        }
    }
    else
    {
        if(actor->data_1 != 1)
        {
            return;
        }
        if (actor->data_2 == 0)
        {
            if(actor->data_4 != 0)
            {
                actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
                actor->frame_num = actor->data_3 + 1 + 1;
            }

            uint8 tile_attr = map_get_tile_attr(actor->x + 3, actor->y + 1);
            if (sprite_blocking_check(3, 0x6a, 0, actor->x + 1, actor->y) != NOT_BLOCKED || ((tile_attr & 1u) ? -1 : 0) + 1 != 0)
            {
                if(suction_cup_enemy_check_if_blocked(actor, 0) == 0)
                {
                    actor->data_1 = 0;
                    actor->data_2 = 0;
                }
                else
                {
                    actor->data_2 = 2;
                    actor->frame_num = 8;
                }
            }
            else
            {
                if (sub_1106F() % 0x32 != 0)
                {
                    if(actor->data_4 != 0)
                    {
                        actor->x = actor->x + 1;
                    }
                }
                else
                {
                    if(suction_cup_enemy_check_if_blocked(actor, 0) == 0)
                    {
                        actor->data_1 = 0;
                        actor->data_2 = 0;
                    }
                    else
                    {
                        actor->data_2 = 2;
                        actor->frame_num = 8;
                    }
                }
            }
        }
        else
        {
            if(actor->data_2 != 1)
            {
                if(actor->data_2 == 2)
                {
                    if(sprite_blocking_check(0, 0x6a, 0, actor->x, actor->y - 1) == NOT_BLOCKED)
                    {
                        actor->y = actor->y - 1;
                    }
                    else
                    {
                        actor->data_2 = 1;
                    }

                    if(sprite_blocking_check(0, 0x6a, 0, actor->x, actor->y - 1) != NOT_BLOCKED)
                    {
                        actor->data_2 = 1;
                    }
                    else
                    {
                        actor->y = actor->y - 1;
                    }
                    return;
                }

                if(actor->data_2 == 3)
                {
                    if(sprite_blocking_check(1, 0x6a, 0, actor->x, actor->y + 1) == NOT_BLOCKED)
                    {
                        actor->y = actor->y + 1;
                    }
                    else
                    {
                        actor->data_2 = 0;
                    }

                    if(sprite_blocking_check(1, 0x6a, 0, actor->x, actor->y + 1) == NOT_BLOCKED)
                    {
                        actor->y = actor->y + 1;
                    }
                    else
                    {
                        actor->data_2 = 0;
                    }
                }
                return;
            }
            if(actor->data_4 != 0)
            {
                actor->data_3 = (actor->data_3 ? -1 : 0) + 1;
                actor->frame_num = actor->data_3 + 6;
            }

            uint8 tile_attr = map_get_tile_attr(actor->x + 3, actor->y - 4);
            if (sprite_blocking_check(3, 0x6a, 0, actor->x + 1, actor->y) != NOT_BLOCKED || ((tile_attr & 4u) ? -1 : 0) + 1 != 0)
            {
                if(suction_cup_enemy_check_if_blocked(actor, 1) == 0)
                {
                    actor->data_1 = 0;
                    actor->data_2 = 1;
                }
                else
                {
                    actor->data_2 = 3;
                    actor->frame_num = 8;
                }
            }
            else
            {
                if (sub_1106F() % 0x32 == 0)
                {
                    if(suction_cup_enemy_check_if_blocked(actor, 1) == 0)
                    {
                        actor->data_1 = 0;
                        actor->data_2 = 1;
                    }
                    else
                    {
                        actor->data_2 = 3;
                        actor->frame_num = 8;
                    }
                }
                else
                {
                    if(actor->data_4 != 0)
                    {
                        actor->x = actor->x + 1;
                    }
                }
            }
        }
    }
}

void activate_switch_maybe(int actorInfoIndex, ActorData *switch_actor)
{
    for(int di=0; di < total_num_actors; di++)
    {
        ActorData *actor = get_actor(di);
        if (actor->actorInfoIndex != actorInfoIndex)
        {
            continue;
        }

        if (switch_actor->data_1 != 2)
        {
            if (switch_actor->data_1 != 1)
            {
                break;
            }

            map_write_tile_cell(actor->data_1, actor->x + 1, actor->y - 0);
            map_write_tile_cell(actor->data_2, actor->x + 1, actor->y - 1);
            map_write_tile_cell(actor->data_3, actor->x + 1, actor->y - 2);
            map_write_tile_cell(actor->data_4, actor->x + 1, actor->y - 3);
            map_write_tile_cell(actor->data_5, actor->x + 1, actor->y - 4);
        }
        else
        {
            actor->is_deactivated_flag_maybe = 1;
            play_sfx(8);
            effect_add_sprite(actorInfoIndex, 1, actor->x, actor->y, 5, 5);
        }
    }
}
void actor_wt_switch(ActorData *actor)
{
    if(actor->frame_num == 1)
    {
        if(actor->data_1 < 3)
        {
            actor->data_1++;
        }
        activate_switch_maybe(actor->data_5, actor);
    }
}

void actor_wt_switch_multi_use(ActorData *actor)
{
    if(actor->actorInfoIndex != 0x3c)
    {
        return;
    }
    if(actor->has_moved_left_flag == 0)
    {
        actor->has_moved_left_flag = 1;
        write_tile_row_to_tilemap(0x3d88, 0x3d90, 0x3d98, 0x3da0, actor->x, actor->y);
    }

    if(actor->data_4 != 0)
    {
        actor->data_4 = 0;
        write_tile_row_to_tilemap(0x3da8 - actor->data_3, 0x3da8 - actor->data_3 + 8, 0x3da8 - actor->data_3 + 0x10, 0x3da8 - actor->data_3 + 0x18, actor->x, actor->y);

        actor->y = actor->y + 1;
        write_tile_row_to_tilemap(0x3d88, 0x3d90, 0x3d98, 0x3da0, actor->x, actor->y);
        if(actor->data_1 == 4)
        {
            play_sfx(0x10);
            switch (actor->data_5)
            {
                case 59:
                    move_platform_flag = 1;
                    break;

                case 61:
                    word_2E17E = 4;
                    if(!speech_bubble_switch_61_shown_flag)
                    {
                        speech_bubble_switch_61_shown_flag = true;
                        player_add_speech_bubble(WHOA);
                    }
                    break;

                case 120:
                    brightness_effect_enabled_flag = 1;
                    break;

                case 121:
                    energy_beam_enabled_flag = 0;
                    break;
            }
        }
        else
        {
            play_sfx(15);
        }
    }

    if(actor->data_1 < 4 && actor->data_4 == 0)
    {
        if(struct6_1B4FC(0x3c, 0, actor->x, actor->y) != 0)
        {
            actor->data_1 = actor->data_1 + 1;
            if(actor->data_2 != 0)
            {
                actor->data_3 = 0;
            }
            else
            {
                actor->data_3 = 0x40;
                actor->data_2 = 1;
            }
            actor->data_4 = 1;
        }
    }
}

void actor_wt_teleporter(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    if(teleporter_counter == 0 || (cosmo_rand() & 1) == 0)
    {
        
        display_actor_sprite_maybe(0x6b, 0, actor->x, actor->y, 0);
    }
    else
    {
        
        display_actor_sprite_maybe(0x6b, 0, actor->x, actor->y, 2);
    }
    if((sub_1106F() & 1) != 0)
    {
        display_actor_sprite_maybe(0x6b, (cosmo_rand() & 1) + 1, actor->x, actor->y, 0);
    }
    if(teleporter_counter == 15)
    {
        effect_add_sprite(15, 4, player_x_pos - 1, player_y_pos, 0, 1);
        effect_add_sprite(15, 4, player_x_pos + 1, player_y_pos, 0, 1);
        effect_add_sprite(15, 4, player_x_pos - 1, player_y_pos - 3, 0, 2);
        effect_add_sprite(15, 4, player_x_pos, player_y_pos - 2, 0, 3);
        effect_add_sprite(15, 4, player_x_pos + 1, player_y_pos - 3, 0, 3);
        play_sfx(0x17);
    }
    if(teleporter_counter > 1)
    {
        teleporter_counter = teleporter_counter - 1;
        return;
    }
    if(teleporter_state_maybe == 3)
    {
        finished_level_flag_maybe = 1;
        return;
    }
    if(teleporter_state_maybe == 0)
    {
        return;
    }
    
    if(actor->data_5 == teleporter_state_maybe || actor->data_5 == 3)
    {
        return;
    }
    player_x_pos = actor->x + 1;
    player_y_pos = actor->y;
    if(player_x_pos - 14 >= 0)
    {
        if(player_x_pos - 14 <= map_width_in_tiles - 38)
        {
            mapwindow_x_offset = player_x_pos - 14;
        }
        else
        {
            mapwindow_x_offset = map_width_in_tiles - 38;
        }
    }
    else
    {
        mapwindow_x_offset = 0;
    }
    if(player_y_pos - 12 >= 0)
    {
        if(player_y_pos - 12 <= map_max_y_offset)
        {
            mapwindow_y_offset = player_y_pos - 12;
        }
        else
        {
            mapwindow_y_offset = map_max_y_offset;
        }
    }
    else
    {
        mapwindow_y_offset = 0;
    }
    teleporter_state_maybe = 0;
    teleporter_counter = 0;
    player_bounce_flag_maybe = 0;
    if(!speech_bubble_teleporter_shown_flag)
    {
        speech_bubble_teleporter_shown_flag = true;
        player_add_speech_bubble(WHOA);
    }
    return;
}

void actor_wt_two_tons(ActorData *actor)
{
    if(actor->data_1 < 0x14)
    {
        actor->data_1++;
    }
    
    if(actor->data_1 == 0x13)
    {
        actor->data_2 = 1;
    }
    
    if(actor->data_2 == 1)
    {
        if(actor->frame_num >= 3)
        {
            
            actor->data_2 = 2;
            if(is_sprite_on_screen(0x2d, 4, actor->x - 1, actor->y + 3) != 0)
            {
                play_sfx(0x25);
            }
        }
        else
        {
            actor->frame_num = actor->frame_num + 1;
            if(actor->frame_num == 1)
            {
                actor->data_3 = 1;
            }
            else
            {
                if(actor->frame_num == 2)
                {
                    actor->data_3 = 2;
                }
                else
                {
                    if(actor->frame_num == 3)
                    {
                        actor->data_3 = 4;
                    }
                }
            }
            
            actor->y = actor->y + actor->data_3;
        }
    }
    
    if(actor->data_2 == 2)
    {
        if(actor->frame_num <= 0)
        {
            
            actor->data_2 = 0;
            actor->data_1 = 0;
            actor->data_3 = 0;
        }
        else
        {
            actor->frame_num--;
            if(actor->frame_num == 0)
            {
                actor->data_3 = 1;
            }
            else
            {
                if(actor->frame_num == 1)
                {
                    actor->data_3 = 2;
                }
                else
                {
                    if(actor->frame_num == 2)
                    {
                        actor->data_3 = 4;
                    }
                }
            }
            
            actor->y = actor->y - actor->data_3;
        }
    }
    
    if(player_check_collision_with_actor(0x2d, 4, actor->x - 1, actor->y + 3) != 0)
    {
        player_decrease_health();
        return;
    }
    
    display_actor_sprite_maybe(0x2d, 4, actor->x - 1, actor->y + 3, 0);
    return;
}

const sint8 score_effect_x_tbl[] = {
        -2, -1, 0, 1, 2, 2, 1, 0, -1, -2
};
void actor_wt_floating_score_effect(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    actor->data_1++;
    actor->frame_num = (actor->frame_num ? -1 : 0) + 1;
    if(actor->data_1 > 0x1f)
    {
        actor->y = actor->y - 1;
        actor->x = actor->x + score_effect_x_tbl[(actor->data_1 - 32) % 10];
    }

    if(actor->data_1 < 4)
    {
        actor->y = actor->y - 1;
    }

    if(actor->data_1 == 0x64 || is_sprite_on_screen(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y) == 0)
    {
        actor->is_deactivated_flag_maybe = 1;
        actor_tile_display_func_index = 1;
    }

    display_actor_sprite_maybe(actor->actorInfoIndex, actor->frame_num, actor->x, actor->y, 5);
    return;
}

void actor_wt_speech_bubble(ActorData *actor)
{
    actor_tile_display_func_index = 1;
    if(actor->data_1 == 0)
    {
        play_sfx(0x39);
        if(actor->actorInfoIndex == 0xf6)
        {
            player_add_to_score(0xc350);
        }
    }

    actor->data_1++;
    if(actor->data_1 != 0x14)
    {
        display_actor_sprite_maybe(actor->actorInfoIndex, 0, player_x_pos - 1, player_y_pos - 5, 5);
    }
    else
    {
        actor->is_deactivated_flag_maybe = 1;
    }

    return;
}

const static uint8 unk_232_frame_num_tbl[] = {0, 1, 2, 1};
void actor_wt_unknown_232(ActorData *actor)
{
    byte_32EB8 = 1;
    
    actor->data_1++;
    actor->frame_num = unk_232_frame_num_tbl[actor->data_1 & 3];
    if(actor->data_1 > 0xc8 && (actor->data_1 & 1) != 0)
    {
        actor_tile_display_func_index = 1;
    }
    
    if(actor->data_1 != 0xf0)
    {
        
        actor->x = player_x_pos - 1;
        actor->y = player_y_pos + 1;
    }
    else
    {
        actor->is_deactivated_flag_maybe = 1;
        actor_tile_display_func_index = 1;
        byte_32EB8 = 0;
    }
}

void check_actor_move_left_or_right(ActorData *actor, Direction direction_of_movement)
{
    uint16 sprite_width = actor_get_tile_info(actor->actorInfoIndex, actor->frame_num)->width;

    if(direction_of_movement == LEFT)
    {
        BlockingType block_status = sprite_blocking_check(2, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y);

        actor->has_moved_left_flag = (block_status != NOT_BLOCKED ? -1 : 0) + 1;
        if(actor->has_moved_left_flag == 0 && block_status != SLOPE)
        {
            actor->x = actor->x + 1;
            return;
        }
        if(block_status != SLOPE)
        {
            if(sprite_blocking_check(1, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y + 1) == NOT_BLOCKED)
            {
                uint8 tile_attr = map_get_tile_attr(actor->x + sprite_width, actor->y + 1);
                if((tile_attr & TILE_ATTR_SLOPED) != 0)
                {

                    tile_attr = map_get_tile_attr(actor->x + sprite_width - 1, actor->y + 1 + 1);
                    if((tile_attr & TILE_ATTR_SLOPED) != 0)
                    {

                        tile_attr = map_get_tile_attr(actor->x + sprite_width - 1, actor->y + 1);
                        if((tile_attr & TILE_ATTR_BLOCK_DOWN) != 0)
                        {
                            return;
                        }

                        actor->has_moved_left_flag = 1;

                        if((tile_attr & TILE_ATTR_SLOPED) == 0)
                        {
                            actor->y = actor->y + 1;
                        }
                        return;
                    }
                }
                if(actor->has_moved_left_flag == 0)
                {
                    actor->x = actor->x + 1;
                    return;
                }

                if(actor->non_blocking_flag_maybe != 0 || sprite_blocking_check(2, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y + 1) != 0)
                {
                    return;
                }

                tile_attr = map_get_tile_attr(actor->x + sprite_width - 1, actor->y + 1);
                if((tile_attr & TILE_ATTR_SLOPED) == 0)
                {
                    actor->x = actor->x + 1;
                    actor->has_moved_left_flag = 0;
                }
            }
            else
            {
                actor->has_moved_left_flag = 1;
            }
        }
        else
        {
            actor->has_moved_left_flag = 1;
            actor->y = actor->y - 1;
        }
    }
    else
    {
        BlockingType block_status = sprite_blocking_check(3, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y);

        actor->has_moved_right_flag = (block_status != NOT_BLOCKED ? -1 : 0) + 1;
        if(actor->has_moved_right_flag == 0 && block_status != SLOPE)
        {
            actor->x = actor->x - 1;
            return;
        }
        if(block_status != SLOPE)
        {
            if(sprite_blocking_check(1, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y + 1) == NOT_BLOCKED)
            {
                uint8 tile_attr = map_get_tile_attr(actor->x - 1, actor->y + 1);
                if((tile_attr & TILE_ATTR_SLOPED) != 0)
                {

                    tile_attr = map_get_tile_attr(actor->x, actor->y + 1 + 1);
                    if((tile_attr & TILE_ATTR_SLOPED) != 0)
                    {

                        tile_attr = map_get_tile_attr(actor->x, actor->y + 1);
                        if((tile_attr & TILE_ATTR_BLOCK_DOWN) != 0)
                        {
                            return;
                        }

                        actor->has_moved_right_flag = 1;
                        tile_attr = map_get_tile_attr(actor->x, actor->y + 1);
                        if((tile_attr & TILE_ATTR_SLOPED) == 0)
                        {
                            actor->y = actor->y + 1;
                            return;
                        }
                        return;
                    }
                }
                if(actor->has_moved_right_flag == 0)
                {
                    actor->x = actor->x - 1;
                    return;
                }

                if(actor->non_blocking_flag_maybe == 0 && sprite_blocking_check(3, actor->actorInfoIndex, actor->frame_num, actor->x, actor->y + 1) == NOT_BLOCKED)
                {

                    tile_attr = map_get_tile_attr(actor->x, actor->y + 1);
                    if((tile_attr & TILE_ATTR_SLOPED) == 0)
                    {

                        actor->x = actor->x - 1;
                        actor->has_moved_right_flag = 0;
                        return;
                    }
                    return;
                }
            }
            else
            {
                actor->has_moved_right_flag = 1;
            }
        }
        else
        {
            actor->has_moved_right_flag = 1;
            actor->y = actor->y - 1;
        }
    }

    return;
}
