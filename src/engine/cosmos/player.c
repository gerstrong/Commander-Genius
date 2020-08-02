#include "sound/sfx.h"
#include "actor.h"
#include "dialog.h"
#include "map.h"
#include "input.h"
#include "tile.h"
#include "util.h"
#include "video.h"
#include "save.h"
#include "status.h"
#include "effects.h"
#include "player.h"

const sint16 player_x_offset_tbl[] = { 0, 0, 1, 1, 1, 0, -1, -1, -1 };
const sint16 player_y_offset_tbl[] = { 0, -1, -1, 0, 1, 1, 1, 0, -1 };

bool player_is_grabbing_wall_flag = false;
uint16 player_death_counter = 0;
int player_walk_anim_index = 0;
int player_is_being_pushed_flag = 0;
int player_hanging_on_wall_direction = 0;

int player_y_pos = 0;
int player_x_pos = 0;
int player_direction = 0;
int player_direction_related_32E98 = 0;

uint16 player_sprite_dir_frame_offset = 0;
int player_input_jump_related_flag = 0;

int player_bounce_height_counter = 0;
int player_bounce_flag_maybe = 0;

int is_standing_slipry_slope_left_flg = 0;
int is_standing_slipry_slope_right_flg = 0;

uint8 health = 0;
uint8 num_health_bars = 0;

uint16 num_bombs = 0;
uint16 has_had_bomb_flag = 0;

int teleporter_state_maybe = 0;
int player_is_teleporting_flag = 0;
int teleporter_counter = 0;

uint8 player_in_pneumatic_tube_flag = 0;

uint16 player_invincibility_counter = 0;

int player_fall_off_map_bottom_counter = 0;
int num_hits_since_touching_ground = 0;

uint16 player_push_direction = 0;
uint16 player_push_anim_duration_maybe = 0;
uint16 player_push_anim_counter = 0;
uint16 player_push_duration = 0;
uint16 player_push_frame_num = 0;
uint8 player_dont_push_while_jumping_flag = 0;
uint16 player_push_check_blocking_flag = 0;

uint8 player_has_shown_ouch_bubble_flag;
int player_idle_counter;
uint16 word_28F94;
int word_2E180;
int word_2E1E8;
int hide_player_sprite;
int player_spring_jump_flag;
int word_2E214;
int player_hoverboard_counter;
int word_32EAC;
int word_32EB2;

unsigned char byte_2E182;
unsigned char byte_2E2E4;

bool god_mode_flag = false;
uint8 show_monster_attack_hint = 0;
bool cheat_hack_mover_enabled = false;

Tile *player_tiles;
Sprite *player_sprites;

BlockingType player_check_movement(int direction, int x_pos, int y_pos)
{
    is_standing_slipry_slope_left_flg = 0;
    is_standing_slipry_slope_right_flg = 0;
    uint8 tile_attr;
    switch (direction)
    {
        case 0: //UP
            if (player_y_pos - 3 == 0 || player_y_pos - 2 == 0)
            {
                return BLOCKED;
            }

            for (int i = 0; i < 3; i++)
            {
                if (map_get_tile_attr(x_pos + i, y_pos - 4) & TILE_ATTR_BLOCK_UP)
                {
                    return BLOCKED;
                }
            }
            break;

        case 1: //DOWN
            if (map_max_y_offset + 0x12 == player_y_pos)
            {
                return NOT_BLOCKED;
            }

            tile_attr = map_get_tile_attr(x_pos, y_pos);
            if ((tile_attr & TILE_ATTR_BLOCK_DOWN) == 0 && (tile_attr & TILE_ATTR_SLOPED) != 0 &&
                (tile_attr & TILE_ATTR_SLIPPERY) != 0)
            {
                is_standing_slipry_slope_left_flg = 1;
            }

            tile_attr = map_get_tile_attr(x_pos + 2, y_pos);
            if ((tile_attr & TILE_ATTR_BLOCK_DOWN) == 0 &&
                (tile_attr & TILE_ATTR_SLOPED) != 0 &&
                (tile_attr & TILE_ATTR_SLIPPERY) != 0)
            {
                is_standing_slipry_slope_right_flg = 1;
            }

            for (int i = 0; i < 3; i++)
            {
                tile_attr = map_get_tile_attr(x_pos + i, y_pos);
                if (tile_attr & TILE_ATTR_SLOPED)
                {
                    num_hits_since_touching_ground = 0;
                    return SLOPE;
                }
                if (tile_attr & TILE_ATTR_BLOCK_DOWN)
                {
                    num_hits_since_touching_ground = 0;
                    return BLOCKED;
                }
            }
            break;

        case 2: // LEFT
            tile_attr = map_get_tile_attr(x_pos, y_pos - 2);
            player_is_grabbing_wall_flag = (tile_attr & TILE_ATTR_CAN_GRAB_WALL);

            for (int i = 0; i < 5; i++)
            {
                tile_attr = map_get_tile_attr(x_pos, y_pos - i);
                if (tile_attr & TILE_ATTR_BLOCK_LEFT)
                {
                    return BLOCKED;
                }

                if (i == 0)
                {
                    if (map_get_tile_attr(x_pos, y_pos) & TILE_ATTR_SLOPED &&
                        (map_get_tile_attr(x_pos, y_pos - 1) & TILE_ATTR_BLOCK_LEFT) == 0)
                    {
                        return SLOPE;
                    }
                }
            }
            break;

        case 3: // RIGHT
            tile_attr = map_get_tile_attr(x_pos + 2, y_pos - 2);
            player_is_grabbing_wall_flag = (tile_attr & TILE_ATTR_CAN_GRAB_WALL);

            for (int i = 0; i < 5; i++)
            {
                tile_attr = map_get_tile_attr(x_pos + 2, y_pos - i);
                if (tile_attr & TILE_ATTR_BLOCK_RIGHT)
                {
                    return BLOCKED;
                }

                if (i == 0)
                {
                    if (map_get_tile_attr(x_pos + 2, y_pos) & TILE_ATTR_SLOPED &&
                        (map_get_tile_attr(x_pos + 2, y_pos - 1) & TILE_ATTR_BLOCK_RIGHT) == 0)
                    {
                        return SLOPE;
                    }
                }
            }
            break;

        default : break;
    }

    return NOT_BLOCKED;
}

void push_player()
{
    int di = 0;
    if(player_is_being_pushed_flag == di)
    {
        return;
    }
    if(jump_key_pressed != 0 && player_dont_push_while_jumping_flag != 0)
    {
        player_is_being_pushed_flag = di;
        return;
    }

    for(int i=0; i < player_push_duration; i++)
    {
        if(player_x_offset_tbl[player_push_direction] + player_x_pos > 0 &&
                player_x_offset_tbl[player_push_direction] + player_x_pos + 2 < map_width_in_tiles)
        {
            player_x_pos += player_x_offset_tbl[player_push_direction];
        }

        player_y_pos += player_y_offset_tbl[player_push_direction];

        if(player_x_offset_tbl[player_push_direction] + mapwindow_x_offset > 0 &&
                player_x_offset_tbl[player_push_direction] + mapwindow_x_offset < map_width_in_tiles - 37)
        {
            mapwindow_x_offset = mapwindow_x_offset + player_x_offset_tbl[player_push_direction];
        }
        if(player_y_offset_tbl[player_push_direction] + mapwindow_y_offset > 2)
        {
            mapwindow_y_offset = mapwindow_y_offset + player_y_offset_tbl[player_push_direction];
        }
        if(player_push_check_blocking_flag != 0 && (player_check_movement(2, player_x_pos, player_y_pos) != NOT_BLOCKED || player_check_movement(3, player_x_pos, player_y_pos) != NOT_BLOCKED || player_check_movement(0, player_x_pos, player_y_pos) != NOT_BLOCKED || player_check_movement(1, player_x_pos, player_y_pos) != NOT_BLOCKED))
        {
            di = 1;
            break;
        }
    }

    if(di != 0)
    {
        player_x_pos = player_x_pos - player_x_offset_tbl[player_push_direction];
        player_y_pos = player_y_pos - player_y_offset_tbl[player_push_direction];
        mapwindow_x_offset = mapwindow_x_offset - player_x_offset_tbl[player_push_direction];
        mapwindow_y_offset = mapwindow_y_offset - player_y_offset_tbl[player_push_direction];
        player_reset_push_variables();
        return;
    }
    player_push_anim_counter++;
    if(player_push_anim_counter >= player_push_anim_duration_maybe)
    {
        player_reset_push_variables();
    }
}

sint16 word_28F80[10] = {-2, -1, -1, -1, -1, -1, -1, 0, 0, 0};

void handle_player_input_maybe()
{
    static int local_bomb_key_counter = 0;
    BlockingType player_movement_status = NOT_BLOCKED;

    if (cheat_hack_mover_enabled) {
        player_hack_mover_update();
        return;
    }

    int si = 0;
    player_is_grabbing_wall_flag = false;
    if(player_death_counter != 0 || teleporter_state_maybe != 0 || player_hoverboard_counter != 0 || player_walk_anim_index != 0 || hide_player_sprite != 0)
    {
        return;
    }
    word_28F94 = word_28F94 + 1;
    push_player();
    if(player_is_being_pushed_flag != 0)
    {
        player_hanging_on_wall_direction = 0;
        return;
    }
    if(player_hanging_on_wall_direction != 0)
    {
        uint8 tile_attr = 0;
        if(player_hanging_on_wall_direction != 2)
        {
            tile_attr = map_get_tile_attr(player_x_pos + 3, player_y_pos - 2);
        }
        else
        {
            tile_attr = map_get_tile_attr(player_x_pos - 1, player_y_pos - 2);
        }
        if((tile_attr & TILE_ATTR_SLIPPERY) != 0 && (tile_attr & TILE_ATTR_CAN_GRAB_WALL) != 0)
        {
            if(player_check_movement(1, player_x_pos, player_y_pos + 1) == NOT_BLOCKED)
            {
                player_y_pos = player_y_pos + 1;
                si = 1;
                if(player_hanging_on_wall_direction != 2)
                {
                    tile_attr = map_get_tile_attr(player_x_pos + 3, player_y_pos - 2);
                }
                else
                {
                    tile_attr = map_get_tile_attr(player_x_pos - 1, player_y_pos - 2);
                }

                if((tile_attr & TILE_ATTR_SLIPPERY) == 0)
                {
                    if((tile_attr & TILE_ATTR_CAN_GRAB_WALL) == 0)
                    {
                        player_hanging_on_wall_direction = 0;
                        si = 0;
                        //goto loc_1DD63;
                    }
                }
            }
            else
            {
                player_hanging_on_wall_direction = 0;
            }
        }
        else
        {
            if((tile_attr & TILE_ATTR_CAN_GRAB_WALL) == 0)
            {
                player_hanging_on_wall_direction = 0;
            }
        }
    }
    if(player_hanging_on_wall_direction == 0)
    {
        if(bomb_key_pressed == 0)
        {
            local_bomb_key_counter = 0;
        }
        if(bomb_key_pressed != 0 && local_bomb_key_counter == 0)
        {
            local_bomb_key_counter = 2;
        }
        if(local_bomb_key_counter != 0 && local_bomb_key_counter != 1)
        {
            local_bomb_key_counter = local_bomb_key_counter - 1;
            if(local_bomb_key_counter == 1)
            {
                if(player_direction == 0)
                {
                    uint8 tile_attr = map_get_tile_attr(player_x_pos - 1, player_y_pos - 2);
                    bool top_bomb_check_flag = tile_attr & TILE_ATTR_BLOCK_LEFT;
                    tile_attr = map_get_tile_attr(player_x_pos - 2, player_y_pos - 2);
                    bool bottom_bomb_check_flag = tile_attr & TILE_ATTR_BLOCK_LEFT;

                    if(num_bombs != 0 || has_had_bomb_flag != 0)
                    {
                        if(top_bomb_check_flag)
                        {
                            play_sfx(0x1c);
                        }
                        else
                        {
                            if(bottom_bomb_check_flag)
                            {
                                play_sfx(0x1c);
                            }
                            else
                            {
                                if(num_bombs == 0)
                                {
                                    play_sfx(0x1c);
                                }
                                else
                                {
                                    actor_add_new(0x18, player_x_pos - 2, player_y_pos - 2);
                                    num_bombs = num_bombs - 1;
                                    display_num_bombs_left();
                                    play_sfx(0x1d);
                                }
                            }
                        }
                    }
                    else
                    {
                        has_had_bomb_flag = 1;
                        you_havent_found_any_bombs_dialog();
                    }
                }
                else
                {
                    uint8 tile_attr = map_get_tile_attr(player_x_pos + 3, player_y_pos - 2);
                    bool top_bomb_check_flag = tile_attr & TILE_ATTR_BLOCK_RIGHT;
                    tile_attr = map_get_tile_attr(player_x_pos + 4, player_y_pos - 2);
                    bool bottom_bomb_check_flag = tile_attr & TILE_ATTR_BLOCK_RIGHT;

                    if(num_bombs == 0 && has_had_bomb_flag == 0)
                    {
                        has_had_bomb_flag = 1;
                        you_havent_found_any_bombs_dialog();
                    }
                    if(top_bomb_check_flag)
                    {
                        play_sfx(0x1c);
                    }
                    else
                    {
                        if(!bottom_bomb_check_flag)
                        {
                            if(num_bombs > 0)
                            {
                                actor_add_new(0x18, player_x_pos + 3, player_y_pos - 2);
                                num_bombs = num_bombs - 1;
                                display_num_bombs_left();
                                play_sfx(0x1d);
                            }
                            else
                            {
                                play_sfx(0x1c);
                            }
                        }
                        else
                        {
                            play_sfx(0x1c);
                        }
                    }
                }
            }
        }
        else
        {
            bomb_key_pressed = 0;
        }
    }
    if(byte_2E182 != 0 || bomb_key_pressed == 0 || byte_2E2E4 != 0 || player_hanging_on_wall_direction != 0 || (jump_key_pressed != 0 && player_input_jump_related_flag == 0))
    {
        word_2E214 = 0;
        //ax = player_check_movement(1, player_x_pos, player_y_pos + 1);
        if(is_standing_slipry_slope_left_flg == 0 || is_standing_slipry_slope_right_flg == 0)
        {
            if(is_standing_slipry_slope_right_flg != 0)
            {
                if(player_hanging_on_wall_direction == 0)
                {
                    player_x_pos = player_x_pos - 1;
                }
                if(player_check_movement(1, player_x_pos, player_y_pos + 1) == 0 && player_hanging_on_wall_direction == 0)
                {
                    player_y_pos = player_y_pos + 1;
                }
                if(player_y_pos - mapwindow_y_offset > 14)
                {
                    mapwindow_y_offset = mapwindow_y_offset + 1;
                }
                if(player_x_pos - mapwindow_x_offset < 12 && mapwindow_x_offset > 0)
                {
                    mapwindow_x_offset = mapwindow_x_offset - 1;
                }
                player_hanging_on_wall_direction = 0;
            }
            if(is_standing_slipry_slope_left_flg != 0)
            {
                if(player_hanging_on_wall_direction == 0)
                {
                    player_x_pos = player_x_pos + 1;
                }
                if(player_check_movement(1, player_x_pos, player_y_pos + 1) == 0 && player_hanging_on_wall_direction == 0)
                {
                    player_y_pos = player_y_pos + 1;
                }
                if(player_y_pos - mapwindow_y_offset > 14)
                {
                    mapwindow_y_offset = mapwindow_y_offset + 1;
                }
                if(player_x_pos - mapwindow_x_offset > 0x17)
                {
                    if(map_width_in_tiles - 38 > mapwindow_x_offset)
                    {
                        mapwindow_x_offset = mapwindow_x_offset + 1;
                    }
                }
                player_hanging_on_wall_direction = 0;
            }
        }
        if(left_key_pressed != 0 && player_hanging_on_wall_direction == 0 && right_key_pressed == 0)
        {
            BlockingType di = player_check_movement(1, player_x_pos, player_y_pos + 1);

            if(player_direction_related_32E98 != 2)
            {
                player_direction_related_32E98 = 2;
            }
            else
            {
                player_x_pos = player_x_pos - 1;
            }
            player_direction = 0;
            if(player_x_pos >= 1)
            {
                player_movement_status = player_check_movement(2, player_x_pos, player_y_pos);
                if(player_movement_status == BLOCKED)
                {
                    player_x_pos = player_x_pos + 1;
                    if(player_check_movement(1, player_x_pos, player_y_pos + 1) == NOT_BLOCKED && player_is_grabbing_wall_flag)
                    {
                        player_hanging_on_wall_direction = 2;
                        player_bounce_flag_maybe = 0;
                        player_bounce_height_counter = 0;
                        play_sfx(4);
                        byte_2E2E4 = 0;
                        byte_2E182 = 0;
                        word_2E180 = 0;
                        if(jump_key_pressed == 0)
                        {
                            player_input_jump_related_flag = 0;
                        }
                        else
                        {
                            player_input_jump_related_flag = 1;
                        }
                    }
                }
            }
            else
            {
                player_x_pos = player_x_pos + 1;
            }
            if(player_movement_status != 2)
            {
                if(di == SLOPE)
                {
                    if(player_check_movement(1, player_x_pos, player_y_pos + 1) == NOT_BLOCKED)
                    {
                        byte_2E2E4 = 0;
                        byte_2E182 = 0;
                        player_y_pos = player_y_pos + 1;
                    }
                }
            }
            else
            {
                player_y_pos = player_y_pos - 1;
            }
        }
        if(right_key_pressed != 0 && player_hanging_on_wall_direction == 0 && left_key_pressed == 0)
        {
            BlockingType di = player_check_movement(1, player_x_pos, player_y_pos + 1);
            if(player_direction_related_32E98 != 3)
            {
                player_direction_related_32E98 = 3;
            }
            else
            {
                player_x_pos = player_x_pos + 1;
            }
            player_direction = 0x17;
            if(map_width_in_tiles - 4 >= player_x_pos)
            {
                player_movement_status = player_check_movement(3, player_x_pos, player_y_pos);
                if(player_movement_status == BLOCKED)
                {
                    player_x_pos = player_x_pos - 1;
                    if(player_check_movement(1, player_x_pos, player_y_pos + 1) == NOT_BLOCKED && player_is_grabbing_wall_flag)
                    {
                        player_hanging_on_wall_direction = 3;
                        player_bounce_flag_maybe = 0;
                        player_bounce_height_counter = 0;
                        play_sfx(4);
                        byte_2E182 = 0;
                        byte_2E2E4 = 0;
                        word_2E180 = 0;
                        if(jump_key_pressed == 0)
                        {
                            player_input_jump_related_flag = 0;
                        }
                        else
                        {
                            player_input_jump_related_flag = 1;
                        }
                    }
                }
            }
            else
            {
                player_x_pos = player_x_pos - 1;
            }
            if(player_movement_status != 2)
            {
                if(di == SLOPE)
                {
                    if(player_check_movement(1, player_x_pos, player_y_pos + 1) == NOT_BLOCKED)
                    {
                        byte_2E2E4 = 0;
                        word_2E180 = 0;
                        player_y_pos = player_y_pos + 1;
                    }
                }
            }
            else
            {
                player_y_pos = player_y_pos - 1;
            }
        }
        if(player_hanging_on_wall_direction != 0 && player_input_jump_related_flag != 0 && jump_key_pressed == 0)
        {
            player_input_jump_related_flag = 0;
        }
        if(player_bounce_height_counter != 0 ||
                (jump_key_pressed != 0 && byte_2E2E4 == 0 && player_input_jump_related_flag == 0) ||
                (player_hanging_on_wall_direction != 0 && jump_key_pressed != 0 && player_input_jump_related_flag == 0))
        {
            bool var_4 = false;
            if(player_bounce_flag_maybe != 0 && player_bounce_height_counter > 0)
            {
                player_bounce_height_counter = player_bounce_height_counter - 1;
                if(player_bounce_height_counter < 10)
                {
                    player_spring_jump_flag = 0;
                }
                if(player_bounce_height_counter > 1)
                {
                    player_y_pos = player_y_pos - 1;
                }
                if(player_bounce_height_counter > 13)
                {
                    player_bounce_height_counter = player_bounce_height_counter - 1;
                    if(player_check_movement(0, player_x_pos, player_y_pos) != 0)
                    {
                        player_spring_jump_flag = 0;
                    }
                    else
                    {
                        player_y_pos = player_y_pos - 1;
                    }
                }
                if(player_bounce_height_counter == 0)
                {
                    byte_2E182 = 0;
                    player_bounce_flag_maybe = 0;
                    word_2E180 = 0;
                    player_spring_jump_flag = 0;
                    player_input_jump_related_flag = 1;
                }
            }
            else
            {
                if(player_hanging_on_wall_direction == 2)
                {
                    if(left_key_pressed == 0)
                    {
                        if(right_key_pressed != 0)
                        {
                            player_direction = 0x17;
                        }
                    }
                    else
                    {
                        player_hanging_on_wall_direction = 0;
                    }
                }
                if(player_hanging_on_wall_direction == 3)
                {
                    if(right_key_pressed == 0)
                    {
                        if(left_key_pressed != 0)
                        {
                            player_direction = 0;
                        }
                    }
                    else
                    {
                        player_hanging_on_wall_direction = 0;
                    }
                }
                if(player_hanging_on_wall_direction == 0)
                {
                    player_y_pos = player_y_pos + word_28F80[byte_2E182];
                }
                if(byte_2E182 == 0 && player_check_movement(0, player_x_pos, player_y_pos + 1) != 0)
                {
                    player_y_pos = player_y_pos + 1;
                }
                player_bounce_flag_maybe = 0;
                var_4 = true;
            }
            player_hanging_on_wall_direction = 0;
            BlockingType blockingCheck = player_check_movement(0, player_x_pos, player_y_pos);
            if(blockingCheck == NOT_BLOCKED)
            {
                if(var_4 && byte_2E182 == 0)
                {
                    play_sfx(2);
                }
            }
            else
            {
                if(byte_2E182 > 0 || player_bounce_flag_maybe != 0)
                {
                    //loc_1E420:
                    play_sfx(5);
                }
                player_bounce_height_counter = 0;
                player_bounce_flag_maybe = 0;
                blockingCheck = player_check_movement(0, player_x_pos, player_y_pos + 1);
                if(blockingCheck != NOT_BLOCKED)
                {
                    player_y_pos = player_y_pos + 1;
                }
                player_y_pos = player_y_pos + 1;
                byte_2E2E4 = 1;
                if(jump_key_pressed != 0)
                {
                    player_input_jump_related_flag = 1;
                }
                word_2E180 = 0;
                player_spring_jump_flag = 0;
            }
            if(player_bounce_flag_maybe == 0)
            {
                if(byte_2E182 > 6)
                {
                    byte_2E2E4 = 1;
                    if(jump_key_pressed != 0)
                    {
                        player_input_jump_related_flag = 1;
                    }
                    word_2E180 = 0;
                }
                byte_2E182++;
            }
        }
        if(player_hanging_on_wall_direction == 0)
        {
            if(byte_2E2E4 != 0 && jump_key_pressed != 0)
            {
                player_input_jump_related_flag = 1;
            }
            if((jump_key_pressed == 0 || player_input_jump_related_flag != 0) && byte_2E2E4 == 0)
            {
                byte_2E2E4 = 1;
                word_2E180 = 0;
            }
            if(byte_2E2E4 != 0 && player_bounce_flag_maybe == 0)
            {
                player_y_pos = player_y_pos + 1;
                if(player_check_movement(1, player_x_pos, player_y_pos) != NOT_BLOCKED)
                {
                    if(word_2E180 != 0)
                    {
                        play_sfx(3);
                    }
                    byte_2E2E4 = 0;
                    player_y_pos = player_y_pos - 1;
                    byte_2E182 = 0;
                    if(jump_key_pressed == 0)
                    {
                        player_input_jump_related_flag = 0;
                    }
                    else
                    {
                        player_input_jump_related_flag = 1;
                    }
                    word_2E180 = 0;
                }
                if(word_2E180 > 3)
                {
                    player_y_pos = player_y_pos + 1;
                    mapwindow_y_offset = mapwindow_y_offset + 1;
                    if(player_check_movement(1, player_x_pos, player_y_pos) != NOT_BLOCKED)
                    {
                        play_sfx(3);
                        byte_2E2E4 = 0;
                        player_y_pos = player_y_pos - 1;
                        mapwindow_y_offset = mapwindow_y_offset - 1;
                        byte_2E182 = 0;
                        if(jump_key_pressed == 0)
                        {
                            player_input_jump_related_flag = 0;
                        }
                        else
                        {
                            player_input_jump_related_flag = 1;
                        }
                        word_2E180 = 0;
                    }
                }
                if(word_2E180 < 0x19)
                {
                    word_2E180 = word_2E180 + 1;
                }
            }
            if(byte_2E2E4 != 0 && word_2E180 == 1 && player_bounce_flag_maybe == 0)
            {
                player_y_pos = player_y_pos - 1;
            }
        }
    }
    else
    {
        if(left_key_pressed == 0)
        {
            if(right_key_pressed == 0)
            {
                if(player_direction_related_32E98 != 2)
                {
                    if(player_direction_related_32E98 == 3)
                    {
                        word_2E214 = 3;
                    }
                }
                else
                {
                    word_2E214 = 2;
                }
            }
            else
            {
                player_direction_related_32E98 = 3;
                word_2E214 = 3;
                player_direction = 0x17;
            }
        }
        else
        {
            player_direction_related_32E98 = 2;
            word_2E214 = 2;
            player_direction = 0;
        }
    }
    if (word_2E214 == 0) //goto loc_1E5DC;
    {
// node 0001e5dc-0001e5e1 #insn=2 use={} def={} in={ax, si, al, dl} out={ax, si, al, dl} pred={ 1DC0F} CONDJUMP target=0001e615 follow=0001e6bd
        //loc_1E5DC:
        if ((up_key_pressed != 0 || down_key_pressed != 0) && left_key_pressed == 0 && right_key_pressed == 0 &&
            byte_2E2E4 == 0 && jump_key_pressed == 0)
        {
// node 0001e615-0001e6a9 #insn=6 use={} def={} in={} out={} pred={ 1E5DC} FALLTHROUGH follow=0001e96f
            //loc_1E615:
            player_idle_counter = 0;
            if (up_key_pressed != 0 && player_is_teleporting_flag == 0 && word_32EAC == 0)
            {
                if (mapwindow_y_offset > 0 && player_y_pos - mapwindow_y_offset < 0x11)
                {
                    mapwindow_y_offset = mapwindow_y_offset - 1;
                }
                if (si != 0)
                {
                    mapwindow_y_offset = mapwindow_y_offset + 1;
                }
                if (player_hanging_on_wall_direction == 0)
                {
                    player_sprite_dir_frame_offset = 5;
                }
                else
                {
                    player_sprite_dir_frame_offset = 11;
                }
                return;
            }
            if (down_key_pressed == 0)
            {
                return;
            }
            if (mapwindow_y_offset + 3 < player_y_pos)
            {
                mapwindow_y_offset = mapwindow_y_offset + 1;
                if ((si != 0 || is_standing_slipry_slope_left_flg != 0 || is_standing_slipry_slope_right_flg != 0) &&
                    mapwindow_y_offset + 3 < player_y_pos)
                {
                    mapwindow_y_offset = mapwindow_y_offset + 1;
                }
            }
            if (player_hanging_on_wall_direction == 0)
            {
                player_sprite_dir_frame_offset = 6;
            }
            else
            {
                player_sprite_dir_frame_offset = 12;
            }
            return;
        }

        // node 0001e6bd-0001e6c2 #insn=2 use={} def={} in={} out={} pred={ 1E5DC} FALLTHROUGH follow=0001e8d8
        //loc_1E6BD:
        if(player_hanging_on_wall_direction != 2)
        {
            if(player_hanging_on_wall_direction != 3)
            {
                if((byte_2E2E4 == 0 || player_bounce_flag_maybe != 0) && (byte_2E182 <= 6 || byte_2E2E4 != 0))
                {
                    if((jump_key_pressed == 0 || player_input_jump_related_flag != 0) && player_bounce_flag_maybe == 0)
                    {
                        if(left_key_pressed == right_key_pressed)
                        {
                            uint8 rvalue = (uint8)(cosmo_rand() % 0x32);

                            player_sprite_dir_frame_offset = 4;
                            if(left_key_pressed == 0 && right_key_pressed == 0 && byte_2E2E4 == 0)
                            {
                                player_update_idle_anim();
                            }
                            if(player_sprite_dir_frame_offset != 5 && player_sprite_dir_frame_offset != 6 && (rvalue == 0 || rvalue == 0x1f))
                            {
                                //loc_1E89B:
                                player_sprite_dir_frame_offset = 0x12;
                                //goto loc_1E8D8;
                            }
                        }
                        else
                        {
                            if(byte_2E2E4 == 0)
                            {
                                player_idle_counter = 0;
                                if((word_28F94 & 1u) != 0)
                                {
                                    if((player_sprite_dir_frame_offset & 1u) != 0)
                                    {
                                        play_sfx(0x13);
                                    }
                                    player_sprite_dir_frame_offset = player_sprite_dir_frame_offset + 1;
                                }
                                if(player_sprite_dir_frame_offset > 3)
                                {
                                    player_sprite_dir_frame_offset = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        player_idle_counter = 0;
                        player_sprite_dir_frame_offset = 7;
                        if(player_bounce_flag_maybe != 0 && player_spring_jump_flag != 0)
                        {
                            player_sprite_dir_frame_offset = 0x16;
                        }
                        if(player_bounce_height_counter < 3 && player_bounce_flag_maybe != 0)
                        {
                            player_sprite_dir_frame_offset = 8;
                        }
                    }
                }
                else
                {
                    player_idle_counter = 0;
                    if(player_bounce_flag_maybe != 0 || byte_2E2E4 != 0 || byte_2E182 <= 6)
                    {
                        if(word_2E180 < 10 || word_2E180 >= 0x19)
                        {
                            if(word_2E180 != 0x19)
                            {
                                if(byte_2E2E4 != 0)
                                {
                                    player_sprite_dir_frame_offset = 8;
                                }
                                else
                                {
                                    player_sprite_dir_frame_offset = 7;
                                }
                            }
                            else
                            {
                                player_sprite_dir_frame_offset = 0x10;
                                word_32EB2 = 1;
                            }
                        }
                        else
                        {
                            player_sprite_dir_frame_offset = 13;
                        }
                    }
                    else
                    {
                        player_sprite_dir_frame_offset = 8;
                    }
                }
            }
            else
            {
                player_idle_counter = 0;
                if(left_key_pressed == 0)
                {
                    player_sprite_dir_frame_offset = 9;
                }
                else
                {
                    player_sprite_dir_frame_offset = 10;
                }
            }
        }
        else
        {
            player_idle_counter = 0;
            if(right_key_pressed != 0)
            {
                player_sprite_dir_frame_offset = 10;
            }
            else
            {
                player_sprite_dir_frame_offset = 9;
            }
        }
    }
    else
    {
        // node 0001e5cd-0001e5d9 #insn=3 use={} def={} in={si} out={si} pred={ 1DC0F} JUMP target=0001e8d8
        player_idle_counter = 0;
        player_sprite_dir_frame_offset = 14;
        //goto loc_1E8D8;
    }



// node 0001e8d8-0001e962 #insn=5 use={} def={} in={} out={} pred={ 1E5CD 1E6BD} FALLTHROUGH follow=0001e96f
    //loc_1E8D8:
    if(player_y_pos - mapwindow_y_offset > 14)
    {
        mapwindow_y_offset = mapwindow_y_offset + 1;
    }
    if(si == 0 || player_y_pos - mapwindow_y_offset <= 14)
    {
        if(player_bounce_height_counter > 10 && player_y_pos - mapwindow_y_offset < 7 && mapwindow_y_offset > 0)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
        if(player_y_pos - mapwindow_y_offset < 7 && mapwindow_y_offset > 0)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
    }
    else
    {
        mapwindow_y_offset = mapwindow_y_offset + 1;
    }
    if(player_x_pos - mapwindow_x_offset > 0x17 && map_width_in_tiles - 38 > mapwindow_x_offset && map_stride_bit_shift_amt > 5)
    {
        mapwindow_x_offset = mapwindow_x_offset + 1;
        return;
    }
    if(player_x_pos - mapwindow_x_offset < 12 && mapwindow_x_offset > 0)
    {
        mapwindow_x_offset = mapwindow_x_offset - 1;
    }
}

void sub_11062()
{
    word_32EB2 = 0;
    player_walk_anim_index = 0;
}

void player_reset_push_variables()
{
    player_is_being_pushed_flag = 0;
    player_push_direction = 0;
    player_push_anim_duration_maybe = 0;
    player_push_anim_counter = 0;
    player_push_duration = 0;
    player_push_frame_num = 0;
    player_dont_push_while_jumping_flag = 0;
    player_bounce_flag_maybe = 0;
    player_bounce_height_counter = 0;
    byte_2E2E4 = 1;
    word_2E180 = 0;
}

void player_load_tiles()
{
	uint16 num_tile_info_records;
    uint16 num_tiles;
    player_tiles = load_tiles("PLAYERS.MNI", TRANSPARENT, &num_tiles);
    printf("Loading %d player tiles.\n", num_tiles);

    player_sprites = load_tile_info("PLYRINFO.MNI", &num_tile_info_records);
    printf("Loading %d, player tile info records.\n", num_tile_info_records);
}

void display_player_sprite_mode_6(uint8 frame_num, int x_pos, int y_pos)
{
    TileInfo *info = &player_sprites[0].frames[frame_num];
    Tile *tile = &player_tiles[info->tile_num];

    for(int y=0;y < info->height;y++)
    {
        for (int x = 0; x < info->width; x++)
        {
            video_draw_tile(tile, (x_pos + x) * 8, (y_pos + y - (info->height - 1)) * 8);
            tile++;
        }
    }
}

void display_player_sprite(uint8 frame_num, int x_pos, int y_pos, int tile_display_func_index)
{
    if(tile_display_func_index == 6)
    {
        display_player_sprite_mode_6(frame_num, x_pos, y_pos);
        return;
    }

    if(player_push_frame_num == 0xff || teleporter_state_maybe || (player_invincibility_counter & 1u) || hide_player_sprite)
    {
        return;
    }

    TileInfo *info = &player_sprites[0].frames[frame_num];
    Tile *tile = &player_tiles[info->tile_num];

    for(int y=0;y < info->height;y++)
    {
        for(int x=0;x < info->width; x++)
        {
            uint16 screen_x = (x_pos - mapwindow_x_offset + x + 1) * 8;
            uint16 screen_y = (y_pos - info->height + 1 - mapwindow_y_offset + y + 1) * 8;

            if(screen_x >= 8 && screen_x <= 304 && //FIXME need a better way of making sure we draw in the borders.
                                 screen_y >= 8 && screen_y < 152 &&
                    !(map_get_tile_attr(x_pos+x,y_pos - info->height + y + 1) & TILE_ATTR_IN_FRONT))
            {
                if(tile_display_func_index == 2)
                {
                    video_draw_tile_solid_white(tile, screen_x, screen_y);
                }
                else
                {
                    video_draw_tile(tile, screen_x, screen_y);
                }
            }
            tile++;
        }
    }
}

int player_update_sprite()
{
    static uint8 byte_28FAC = 0;

    if (map_max_y_offset + 0x15 < player_y_pos && player_death_counter == 0)
    {
        player_fall_off_map_bottom_counter = 1;
        player_death_counter = 1;

        if (map_max_y_offset + 0x16 == player_y_pos)
        {
            player_y_pos = player_y_pos + 1;
        }
        byte_28FAC++;
        if (byte_28FAC == 5)
        {
            byte_28FAC = 0;
        }
    }
    if (player_fall_off_map_bottom_counter != 0)
    {
        player_fall_off_map_bottom_counter = player_fall_off_map_bottom_counter + 1;
        if (player_fall_off_map_bottom_counter == 2)
        {
            play_sfx(14);
        }

        while (player_fall_off_map_bottom_counter < 12)
        {
            cosmo_wait(2);
            player_fall_off_map_bottom_counter++;
        }

        if (player_fall_off_map_bottom_counter == 13)
        {
            play_sfx(7);
        }
        if (player_fall_off_map_bottom_counter > 12 && player_fall_off_map_bottom_counter < 0x13)
        {
            display_actor_sprite_maybe(0xde, byte_28FAC, player_x_pos - 1,
                                       player_y_pos - player_fall_off_map_bottom_counter + 13, 5);
        }
        if (player_fall_off_map_bottom_counter > 0x12)
        {
            display_actor_sprite_maybe(0xde, byte_28FAC, player_x_pos - 1, player_y_pos - 6, 5);
        }
        if (player_fall_off_map_bottom_counter > 0x1e)
        {
            load_savegame_file('T');
            load_level(current_level);
            player_fall_off_map_bottom_counter = 0;
            return 1;
        }
    }
    else
    {
        if (player_death_counter == 0)
        {
            if (player_invincibility_counter != 0x2c)
            {
                if (player_invincibility_counter > 0x28)
                {
                    display_player_sprite(player_direction + 15, player_x_pos, player_y_pos, 0);
                }
            }
            else
            {
                display_player_sprite(player_direction + 15, player_x_pos, player_y_pos, 2);
            }
            if (player_invincibility_counter != 0)
            {
                player_invincibility_counter = player_invincibility_counter - 1;
            }
            if (player_invincibility_counter < 0x29)
            {
                if (player_is_being_pushed_flag != 0)
                {
                    display_player_sprite((uint8)player_push_frame_num, player_x_pos, player_y_pos, 0);
                }
                else
                {
                    display_player_sprite(player_direction + player_sprite_dir_frame_offset, player_x_pos, player_y_pos,
                                          0);
                }
            }
        }
        else
        {
            if (player_death_counter >= 10)
            {
                if (player_death_counter > 9)
                {
                    if (mapwindow_y_offset > 0 && player_death_counter < 12)
                    {
                        mapwindow_y_offset = mapwindow_y_offset - 1;
                    }
                    if (player_death_counter == 10)
                    {
                        play_sfx(7);
                    }
                    player_y_pos = player_y_pos - 1;
                    player_death_counter = player_death_counter + 1;

                    display_player_sprite((player_death_counter & 1u) + 0x2e, player_x_pos - 1, player_y_pos, 5);
                    if (player_death_counter > 0x24)
                    {
                        load_savegame_file('T');
                        load_level(current_level);
                        return 1;
                    }
                }
            }
            else
            {
                if (player_death_counter == 1)
                {
                    play_sfx(14);
                }
                player_death_counter = player_death_counter + 1;

                display_player_sprite((player_death_counter & 1u) + 0x2e, player_x_pos - 1, player_y_pos, 5);
            }
        }
    }

    return 0;
}

const uint8 player_walk_frame_tbl_maybe[] = {
        0x13, 0x14, 0x15,
        0x14, 0x13, 0x14,
        0x15, 0x14, 0x13
};

void player_update_walk_anim()
{
    if(player_hanging_on_wall_direction != 0)
    {
        word_32EB2 = 0;
        player_walk_anim_index = 0;
    }
    if(word_32EB2 != 0 && player_check_movement(1, player_x_pos, player_y_pos + 1) != 0)
    {
        word_32EB2 = 0;
        player_walk_anim_index = 8;
        play_sfx(3);
    }
    if(player_walk_anim_index != 0)
    {
        player_sprite_dir_frame_offset = player_walk_frame_tbl_maybe[player_walk_anim_index];// * ((player_walk_anim_index << 1) + player_walk_frame_tbl_maybe);
        player_walk_anim_index = player_walk_anim_index - 1;
        byte_2E2E4 = 0;
        if(player_walk_anim_index > 8)
        {
            sub_11062();
        }
    }
}

int player_check_collision_with_actor(int actorInfoIndex, int frame_num, int x_pos, int y_pos) {
    if(player_death_counter == 0)
    {
        TileInfo *tileInfo = actor_get_tile_info(actorInfoIndex, frame_num);
        uint16 di = tileInfo->height;
        uint16 si = tileInfo->width;

        if(x_pos > map_width_in_tiles && x_pos <= -1 && actorInfoIndex == 0x1a)
        {
            si += x_pos;
            x_pos = 0;
        }
        if(((player_x_pos <= x_pos && player_x_pos + 3 > x_pos) || (player_x_pos >= x_pos && x_pos + si > player_x_pos)) &&
                ((y_pos - di < player_y_pos && player_y_pos <= y_pos) || (player_y_pos - 4 <= y_pos && y_pos <= player_y_pos)))
        {
            return 1;
        }
    }
    return 0;
}

int player_bounce_in_the_air(int bounce_height)
{
    static int word_2CAF6 = 0;

    if(player_death_counter != 0 || player_walk_anim_index != 0)
    {
        return 0;
    }

    if((player_bounce_flag_maybe == 0 || player_bounce_height_counter < 2) &&
            ((byte_2E2E4 != 0 && word_2E180 >= 0) || byte_2E182 > 6) &&
            word_2E1E8 != 0)
    {
        player_bounce_height_counter = bounce_height + 1;
        word_2CAF6 = bounce_height + 1;
        player_bounce_flag_maybe = 1;
        sub_11062();
        if(bounce_height <= 0x12)
        {
            player_spring_jump_flag = 0;
        }
        else
        {
            player_spring_jump_flag = 1;
        }
        show_monster_attack_hint = 2;
        if(bounce_height != 7)
        {
            num_hits_since_touching_ground = 0;
        }
        else
        {
            num_hits_since_touching_ground = num_hits_since_touching_ground + 1;
            if(num_hits_since_touching_ground == 10)
            {
                num_hits_since_touching_ground = 0;
                player_add_speech_bubble(POINTS_50000);
            }
        }
        return 1;
    }

    if(word_2CAF6 - 2 >= player_bounce_height_counter)
    {
        return 0;
    }
    else
    {
        if(word_2E1E8 == 0)
        {
            return 0;
        }
        else
        {
            if(player_bounce_flag_maybe == 0)
            {
                return 0;
            }
            else
            {
                sub_11062();
                if(player_bounce_height_counter <= 0x12)
                {
                    player_spring_jump_flag = 0;
                }
                else
                {
                    player_spring_jump_flag = 1;
                }
                show_monster_attack_hint = 2;
                //return 1;
            }
        }
    }
    return 1;
}

void player_add_to_score(uint32 amount_to_add_low)
{
    add_to_score_update_on_display(amount_to_add_low, 9, 0x16);
}

void player_add_score_for_actor(int actorInfoIndex)
{
    switch (actorInfoIndex)
    {
        case 46:
            player_add_to_score(0x320);
            break;

        case 51:
        case 54:
        case 78:
        case 80:
            player_add_to_score(0x190);
            break;

        case 20:
            player_add_to_score(0xc80);
            break;

        case 41:
        case 47:
        case 86:
            player_add_to_score(0x640);
            break;

        case 92:
        case 101:
            player_add_to_score(0x1900);
            break;

        case 17:
        case 18:
        case 87:
        case 89:
            player_add_to_score(0xfa);
            break;

        case 106:
        case 113:
            player_add_to_score(0x3e8);
            break;

        case 69:
        case 125:
            player_add_to_score(0x3200);
            break;

        case 126:
            player_add_to_score(0x7d0);
            break;

        case 3:
        case 4:
        case 88:
        case 111:
        case 112:
        case 118:
        case 127:
            player_add_to_score(0x1f4);
            break;

        case 129:
            player_add_to_score(0xc350);
            break;

        case 74:
        case 75:
        case 95:
        case 96:
        case 128:
        case 187:
            player_add_to_score(0x64);
            break;

        case 1:
        case 25:
        case 55:
        case 65:
        case 83:
        case 84:
        case 124:
        case 188:
            player_add_to_score(0xc8);
            break;

        default: break;
    }
}

void player_decrease_health()
{
    if(player_death_counter == 0 && !god_mode_flag && hide_player_sprite == 0 && teleporter_state_maybe == 0 && byte_32EB8 == 0 && player_in_pneumatic_tube_flag == 0 && player_invincibility_counter == 0)
    {
        player_hanging_on_wall_direction = 0;
        if(player_has_shown_ouch_bubble_flag == 0)
        {
            player_has_shown_ouch_bubble_flag = 1;
            player_add_speech_bubble(OUCH);
            if(show_monster_attack_hint == 0)
            {
                show_monster_attack_hint = 1;
            }
        }
        health--;
        if(health != 0)
        {
            update_health_bar_display();
            player_invincibility_counter = 0x2c;
            play_sfx(14);
        }
        else
        {
            player_death_counter = 1;
            player_hoverboard_counter = 0;
        }
    }
}

void push_player_around(int push_direction, int push_anim_duration, int push_duration, int player_frame_num,
                        uint8 dont_push_while_jumping_flag, int check_for_blocking_flag)
{
    player_push_direction = push_direction;
    player_push_anim_duration_maybe = push_anim_duration;
    player_push_anim_counter = 0;
    player_push_duration = push_duration;
    player_push_frame_num = player_frame_num;
    player_dont_push_while_jumping_flag = dont_push_while_jumping_flag;
    player_is_being_pushed_flag = 1;
    player_hoverboard_counter = 0;
    player_push_check_blocking_flag = check_for_blocking_flag;
    player_bounce_flag_maybe = 0;
    player_bounce_height_counter = 0;
    sub_11062();
}

void player_hoverboard_update()
{
    static uint16 drop_bomb_state = 0;

    sub_11062();
    word_2E1E8 = 0;
    player_bounce_height_counter = 0;
    byte_2E2E4 = 0;
    if (player_death_counter != 0)
    {
        return;
    }

    if (player_hoverboard_counter <= 1)
    {
        if (jump_key_pressed != 0)
        {
            player_input_jump_related_flag = 1;
            player_hoverboard_counter = 0;
            byte_2E2E4 = 1;
            word_2E180 = 1;
            player_bounce_flag_maybe = 0;
            word_2E1E8 = 1;
            player_bounce_in_the_air(9);
            player_bounce_height_counter -= 2;
            play_sfx(2);
            return;
        }
    }
    else
    {
        up_key_pressed = 1;
        player_hoverboard_counter--;
    }

    if (left_key_pressed != 0 && right_key_pressed == 0)
    {
        if (player_direction == 0)
        {
            player_x_pos--;
        }

        player_direction = 0;
        player_sprite_dir_frame_offset = 4;
        if (player_x_pos < 1)
        {
            player_x_pos++;
        }

        if (player_check_movement(2, player_x_pos, player_y_pos) != 0 ||
            player_check_movement(2, player_x_pos, player_y_pos + 1) != 0)
        {
            player_x_pos++;
        }

        if ((player_x_pos & 1) != 0)
        {
            effect_add_sprite(0x13, 4, player_x_pos + 3, player_y_pos + 1, 3, 1);
            play_sfx(0x18);
        }
    }

    if (right_key_pressed != 0 && left_key_pressed == 0)
    {
        if (player_direction != 0)
        {
            player_x_pos++;
        }

        player_direction = 0x17;
        player_sprite_dir_frame_offset = 4;
        if (map_width_in_tiles - 4 < player_x_pos)
        {
            player_x_pos--;
        }

        if (player_check_movement(3, player_x_pos, player_y_pos) != 0 ||
            player_check_movement(3, player_x_pos, player_y_pos + 1) != 0)
        {
            player_x_pos--;
        }
        if ((player_x_pos & 1) != 0)
        {
            effect_add_sprite(0x13, 4, player_x_pos - 1, player_y_pos + 1, 7, 1);
            play_sfx(0x18);
        }
    }

    if (up_key_pressed == 0 || down_key_pressed != 0)
    {
        if (down_key_pressed == 0 || up_key_pressed != 0)
        {
            player_sprite_dir_frame_offset = 4;
        }
        else
        {
            player_sprite_dir_frame_offset = 6;
            if (map_max_y_offset + 0x11 > player_y_pos)
            {
                player_y_pos++;
            }
            if (player_check_movement(1, player_x_pos, player_y_pos + 1) != 0)
            {
                player_y_pos--;
            }
        }
    }
    else
    {
        player_sprite_dir_frame_offset = 5;
        if (player_y_pos > 4)
        {
            player_y_pos--;
        }
        if (player_check_movement(0, player_x_pos, player_y_pos) != 0)
        {
            player_y_pos++;
        }
        if ((player_y_pos & 1) != 0)
        {
            effect_add_sprite(0x13, 4, player_x_pos + 1, player_y_pos + 1, 5, 1);
            play_sfx(0x18);
        }
    }

    if (bomb_key_pressed == 0)
    {
        drop_bomb_state = 0;
    }
    if (bomb_key_pressed != 0 && drop_bomb_state == 0)
    {
        drop_bomb_state = 1;
        player_sprite_dir_frame_offset = 14;
    }
    if (drop_bomb_state == 0 || drop_bomb_state == 2)
    {
        bomb_key_pressed = 0;
    }
    else
    {
        player_sprite_dir_frame_offset = 14;
        if (drop_bomb_state != 0)
        {
            drop_bomb_state = 2;
            if (player_direction == 0)
            {
                uint8 tr1 = map_get_tile_attr(player_x_pos - 1, player_y_pos - 2);
                uint8 tr2 = map_get_tile_attr(player_x_pos - 2, player_y_pos - 2);
                if ((tr1 & TILE_ATTR_BLOCK_LEFT) == 0 && (tr2 & TILE_ATTR_BLOCK_LEFT) == 0 && num_bombs > 0)
                {
                    actor_add_new(0x18, player_x_pos - 2, player_y_pos - 2);
                    num_bombs = num_bombs - 1;
                    display_num_bombs_left();
                    play_sfx(0x1d);
                }
                else
                {
                    play_sfx(0x1c);
                }
            }
            else
            {
                uint8 tr1 = map_get_tile_attr(player_x_pos + 3, player_y_pos - 2);
                uint8 tr2 = map_get_tile_attr(player_x_pos + 4, player_y_pos - 2);
                if ((tr1 & TILE_ATTR_BLOCK_RIGHT) == 0 && (tr2 & TILE_ATTR_BLOCK_RIGHT) == 0 && num_bombs > 0)
                {
                    actor_add_new(0x18, player_x_pos + 3, player_y_pos - 2);
                    num_bombs = num_bombs - 1;
                    display_num_bombs_left();
                    play_sfx(0x1d);
                }
                else
                {
                    play_sfx(0x1c);
                }
            }
        }
    }

    if (player_y_pos - mapwindow_y_offset <= 14)
    {
        if (player_bounce_height_counter > 10 && player_y_pos - mapwindow_y_offset < 7 && mapwindow_y_offset > 0)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
        if (player_y_pos - mapwindow_y_offset < 7 && mapwindow_y_offset > 0)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
    }
    else
    {
        mapwindow_y_offset = mapwindow_y_offset + 1;
    }

    if (player_x_pos - mapwindow_x_offset > 0x17 && map_width_in_tiles - 38 > mapwindow_x_offset)
    {
        mapwindow_x_offset = mapwindow_x_offset + 1;
    }
    else
    {
        if (player_x_pos - mapwindow_x_offset < 12 && mapwindow_x_offset > 0)
        {
            mapwindow_x_offset = mapwindow_x_offset - 1;
        }
    }
}

void player_move_on_platform(int platform_x_left, int platform_x_right, int x_offset_tbl_index, int y_offset_tbl_index)
{
    if(player_hoverboard_counter != 0)
    {
        return;
    }

    int player_right_x_pos = player_sprites[0].frames[0].width + player_x_pos - 1;

    if(player_hanging_on_wall_direction != 0 && player_check_movement(DOWN, player_x_pos, player_y_pos + 1) != 0)
    {
        player_hanging_on_wall_direction = 0;
    }

    if((player_x_pos < platform_x_left || player_x_pos > platform_x_right) && (player_right_x_pos < platform_x_left || player_right_x_pos > platform_x_right))
    {
        return;
    }

    player_x_pos = player_x_pos + player_x_offset_tbl[x_offset_tbl_index];
    player_y_pos = player_y_pos + player_y_offset_tbl[y_offset_tbl_index];

    if((up_key_pressed != 0 || down_key_pressed != 0) && left_key_pressed == 0 && right_key_pressed == 0)
    {
        if(up_key_pressed != 0 && mapwindow_y_offset > 0 && player_y_pos - mapwindow_y_offset < 0x11)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
        if(down_key_pressed != 0 && (mapwindow_y_offset + 4 < player_y_pos || (player_y_offset_tbl[y_offset_tbl_index] == 1 && mapwindow_y_offset + 3 < player_y_pos)))
        {
            mapwindow_y_offset = mapwindow_y_offset + 1;
        }
    }

    if(player_y_pos - mapwindow_y_offset <= 0x11)
    {
        if(player_y_pos - mapwindow_y_offset < 3)
        {
            mapwindow_y_offset = mapwindow_y_offset - 1;
        }
    }
    else
    {
        mapwindow_y_offset = mapwindow_y_offset + 1;
    }

    if(player_x_pos - mapwindow_x_offset <= 0x17 || map_width_in_tiles - 38 <= mapwindow_x_offset)
    {
        if(player_x_pos - mapwindow_x_offset < 12 && mapwindow_x_offset > 0)
        {
            mapwindow_x_offset = mapwindow_x_offset - 1;
        }
    }
    else
    {
        mapwindow_x_offset = mapwindow_x_offset + 1;
    }

    if(player_y_offset_tbl[y_offset_tbl_index] == 1 && player_y_pos - mapwindow_y_offset > 14)
    {
        mapwindow_y_offset = mapwindow_y_offset + 1;
    }

    if(player_y_offset_tbl[y_offset_tbl_index] == -1 && player_y_pos - mapwindow_y_offset < 3)
    {
        mapwindow_y_offset = mapwindow_y_offset - 1;
    }
}

void player_update_idle_anim() {
    player_idle_counter++;
    if(player_idle_counter <= 0x64 || player_idle_counter >= 0x6e)
    {
        if(player_idle_counter <= 0x8b || player_idle_counter >= 0x96)
        {
            if(player_idle_counter == 0xb4)
            {
                //close eyes
                player_sprite_dir_frame_offset = 0x13;
            }
            else
            {
                if(player_idle_counter == 0xb5)
                {
                    player_sprite_dir_frame_offset = 0x14;
                }
                else
                {
                    if(player_idle_counter != 0xb6)
                    {
                        if(player_idle_counter != 0xb7)
                        {
                            if(player_idle_counter != 0xb8)
                            {
                                if(player_idle_counter == 0xb9)
                                {
                                    player_idle_counter = 0;
                                }
                            }
                            else
                            {
                                player_sprite_dir_frame_offset = 0x13;
                            }
                        }
                        else
                        {
                            player_sprite_dir_frame_offset = 0x14;
                        }
                    }
                    else
                    {
                        player_sprite_dir_frame_offset = 0x15;
                    }
                }
            }
        }
        else
        {
            //look down
            player_sprite_dir_frame_offset = 6;
        }
    }
    else
    {
        //look up.
        player_sprite_dir_frame_offset = 5;
    }
}

void player_add_speech_bubble(SpeechBubbleType type) {
    actor_add_new(type, player_x_pos - 1, player_y_pos - 5);
}

void player_hack_mover_update() {
    if (left_key_pressed) {
        if (player_x_pos > 0) {
            player_x_pos = player_x_pos - 1;
        }
        move_map_window(-1, 0);
    }
    if (right_key_pressed) {
        if (player_x_pos + 3 < map_width_in_tiles - 1) {
            player_x_pos = player_x_pos + 1;
        }
        move_map_window(1, 0);
    }

    if (up_key_pressed) {
        if (player_y_pos > 0) {
            player_y_pos = player_y_pos - 1;
        }
        move_map_window(0, -1);
    }

    if (down_key_pressed) {
        if (player_y_pos < map_height_in_tiles - 1) {
            player_y_pos = player_y_pos + 1;
        }
        move_map_window(0, 1);
    }
}
