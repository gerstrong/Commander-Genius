//
// Created by efry on 12/11/2017.
//

#include <stdlib.h>
#include "effects.h"
#include "defines.h"
#include "actor.h"
#include "player.h"
#include "map.h"
#include "sound/sfx.h"
#include "tile.h"
#include "actor_toss.h"
#include "video.h"

#define MAX_EFFECT_SPRITES 10
#define MAX_EXPLODE_EFFECT_SPRITES 16
#define MAX_STRUCT6_SPRITES 7
#define MAX_BRIGHTNESS_OBJS 0xc7

typedef struct effect_sprite
{
    int is_active_flag;
    int actorInfoIndex;
    int totalNumFrames;
    int currentFrameNum;
    int x;
    int y;
    int field_A;
    int counter;
} effect_sprite;

typedef struct explode_effect_sprite
{
    int actorInfoIndex;
    int x;
    int y;
    int frame_num;
    int counter;
    int direction;
    int fall_through_floor;
} explode_effect_sprite;

typedef struct struc_6
{
    int counter;
    int x;
    int y;
} struc_6;

typedef struct
{
    int type;
    int x;
    int y;
} Brightness;

uint16 num_brightness_objs = 0;
Brightness brightness_tbl[MAX_BRIGHTNESS_OBJS + 1];

effect_sprite static_effect_sprites[MAX_EFFECT_SPRITES];

explode_effect_sprite explode_effect_sprites[MAX_EXPLODE_EFFECT_SPRITES];

struc_6 struc6_sprites[MAX_STRUCT6_SPRITES];

int blow_up_actor_with_bomb(int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    switch (actorInfoIndex)
    {
        case 3:
        case 4:
        case 17:
        case 18:
        case 20:
        case 25:
        case 41:
        case 46:
        case 47:
        case 51:
        case 54:
        case 55:
        case 65:
        case 69:
        case 74:
        case 75:
        case 78:
        case 80:
        case 83:
        case 84:
        case 86:
        case 87:
        case 88:
        case 89:
        case 92:
        case 95:
        case 96:
        case 101:
        case 106:
        case 111:
        case 112:
        case 113:
        case 118:
        case 124:
        case 125:
        case 126:
        case 127:
        case 128:
        case 129:
        case 187:
        case 188:
            if(actorInfoIndex == 0x7d)
            {
                actor_add_new(0xb8, x_pos, y_pos);
            }
            if((actorInfoIndex == 0x12 || actorInfoIndex == 0x58) && frame_num == 2)
            {
                return 0;
            }
            else
            {
                explode_effect_add_sprite(actorInfoIndex, frame_num, x_pos, y_pos);
                player_add_score_for_actor(actorInfoIndex);
                if(actorInfoIndex == 0x5f)
                {
                    if(num_eye_plants_remaining_to_blow_up == 1)
                    {
                        player_add_speech_bubble(POINTS_50000);
                    }
                    effect_add_sprite(0x17, 8, x_pos, y_pos, 0, 1);
                    actor_toss_add_new(0x39, x_pos, y_pos);
                    num_eye_plants_remaining_to_blow_up--;
                }
            }
            return 1;

        default : break;
    }

    return 0;
}

void effect_add_sprite(int actorInfoIndex, int frame_num, int x_pos, int y_pos, int arg_8, int counter)
{
    for(int i=0;i<MAX_EFFECT_SPRITES;i++)
    {
        effect_sprite *sprite = &static_effect_sprites[i];
        if(!sprite->is_active_flag)
        {
            sprite->is_active_flag = 1;
            sprite->actorInfoIndex = actorInfoIndex;
            sprite->totalNumFrames = frame_num;
            sprite->x = x_pos;
            sprite->y = y_pos;
            sprite->field_A = arg_8;
            sprite->counter = counter;
            sprite->currentFrameNum = 0;
            return;
        }
    }
}

void effect_update_sprites()
{
    for(int i=0; i < MAX_EFFECT_SPRITES; i++)
    {
        effect_sprite *sprite = &static_effect_sprites[i];

        if(sprite->is_active_flag)
        {
            if(is_sprite_on_screen(sprite->actorInfoIndex, sprite->currentFrameNum, sprite->x, sprite->y))
            {

                if(sprite->actorInfoIndex == 0x63)
                {
                    
                    display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->currentFrameNum, sprite->x, sprite->y, 5);
                }
                else
                {
                    display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->currentFrameNum, sprite->x, sprite->y, 0);
                }
                
                if(sprite->actorInfoIndex == 0x1b)
                {
                    sprite->x--;
                    sprite->y = sprite->y + (rand() % 3);
                }
                sprite->x = sprite->x + player_x_offset_tbl[sprite->field_A];
                sprite->y = sprite->y + player_y_offset_tbl[sprite->field_A];
                sprite->currentFrameNum++;
                if(sprite->currentFrameNum == sprite->totalNumFrames)
                {
                    sprite->currentFrameNum = 0;
                    if(sprite->counter != 0)
                    {
                        sprite->counter--;
                        if(sprite->counter == 0)
                        {
                            sprite->is_active_flag = 0;
                        }
                    }
                }
            }
            else
            {
                sprite->is_active_flag = 0;
            }
        }
    }
}

void effect_clear_sprites()
{
    for(int i=0; i < MAX_EFFECT_SPRITES; i++)
    {
        static_effect_sprites[i].is_active_flag = 0;
    }
}

void exploding_balls_effect(int x_pos, int y_pos)
{
    effect_add_sprite(0x15, 6, x_pos + 1, y_pos, 6, 2);
    effect_add_sprite(0x15, 6, x_pos + 3, y_pos, 4, 2);
    effect_add_sprite(0x15, 6, x_pos + 4, y_pos - 2, 3, 2);
    effect_add_sprite(0x15, 6, x_pos + 3, y_pos - 4, 2, 2);
    effect_add_sprite(0x15, 6, x_pos + 1, y_pos - 4, 8, 2);
    effect_add_sprite(0x15, 6, x_pos, y_pos - 2, 7, 2);
}

void update_rain_effect() //FIXME this rain doesn't look quite right. The lightning palette anim might need tweaking.
{

    int x = (rand() % 38) + mapwindow_x_offset;
    int y = (rand() % 18) + mapwindow_y_offset;

    if((rand() & 1) != 0)
    {
        if((map_get_tile_attr(x, y) & TILE_ATTR_SLIPPERY) != 0)
        {
            effect_add_sprite(0x63, 5, x, y, 0, 1);
        }
    }
    if(rain_flag != 0)
    {
        y = mapwindow_y_offset + 1;

        if(map_get_tile_cell(x, y) == 0)
        {
            effect_add_sprite(0x1b, 1, x, y, 6, 0x14);
        }
    }
    return;
}

void explode_effect_add_sprite(int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    static uint8 explode_direction = 0;

    explode_direction++;
    if(explode_direction == 5)
    {
        explode_direction = 0;
    }

    for(int i=0;i < MAX_EXPLODE_EFFECT_SPRITES; i++)
    {
        explode_effect_sprite *sprite = &explode_effect_sprites[i];
        
        if(sprite->counter == 0)
        {
            sprite->actorInfoIndex = actorInfoIndex;
            sprite->x = x_pos;
            sprite->y = y_pos;
            sprite->frame_num = frame_num;
            sprite->counter = 1;
            sprite->direction = explode_direction;
            sprite->fall_through_floor = 0;
            return;
        }
    }

    return ;
}

void explode_effect_clear_sprites()
{
    for(int i=0;i < MAX_EXPLODE_EFFECT_SPRITES; i++)
    {
        explode_effect_sprites[i].counter = 0;
    }
}

void explode_effect_update_sprites()
{
    for(int i=0;i < MAX_EXPLODE_EFFECT_SPRITES; i++)
    {
        explode_effect_sprite *sprite = &explode_effect_sprites[i];
        if (sprite->counter == 0)
        {
            continue;
        }

        if(sprite->direction == 0 || sprite->direction == 3)
        {
            if(sprite_blocking_check(RIGHT, sprite->actorInfoIndex, sprite->frame_num, sprite->x + 1, sprite->y + 1) == NOT_BLOCKED)
            {
                sprite->x++;
                if(sprite->direction == 3)
                {
                    sprite->x++;
                }
            }
        }
        else
        {
            if(sprite->direction == 1 || sprite->direction == 4)
            {
                if(sprite_blocking_check(LEFT, sprite->actorInfoIndex, sprite->frame_num, sprite->x - 1, sprite->y + 1) == NOT_BLOCKED)
                {
                    sprite->x--;
                    if(sprite->direction == 4)
                    {
                        sprite->x--;
                    }
                }
            }
        }

        for(;;)
        {
            if(sprite->counter < 5)
            {
                sprite->y -= 2;
            }

            if(sprite->counter == 5)
            {
                sprite->y--;
            }
            else if(sprite->counter == 8)
            {
                if (sprite_blocking_check(DOWN, sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y + 1) ==
                    NOT_BLOCKED)
                {
                    sprite->y++;
                }
                else
                {
                    sprite->counter = 3;
                    sprite->y += 2;
                    continue;
                }
            }

            if(sprite->counter < 9)
            {
                if(sprite->counter == 1)
                {
                    display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y, 2);
                }
                else
                {
                    display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y, 4);
                }
                sprite->counter++;
                if(sprite->counter > 40)
                {
                    sprite->counter = 0;
                }
                break;
            }

            if(sprite->counter > 16 && is_sprite_on_screen(sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y) == 0)
            {
                sprite->counter = 0;
                break;
            }

            if(sprite->fall_through_floor != 0 || sprite_blocking_check(DOWN, sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y + 1) == NOT_BLOCKED)
            {
                sprite->y++;
                if(sprite->fall_through_floor != 0 || sprite_blocking_check(DOWN, sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y + 1) == NOT_BLOCKED)
                {
                    if(sprite->counter == 1)
                    {
                        display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y, 2);
                    }
                    else
                    {
                        display_actor_sprite_maybe(sprite->actorInfoIndex, sprite->frame_num, sprite->x, sprite->y, 4);
                    }
                    sprite->counter++;
                    if(sprite->counter > 40)
                    {
                        sprite->counter = 0;
                    }
                    break;
                }
            }
            sprite->counter = 3;
            sprite->fall_through_floor = 1;
            play_sfx(0x2e);
        }
    }
}

uint8 struct6_collision_check_maybe(int actorInfoIndex1, int actor_frame_num1, int actor1_x, int actor1_y, int actorInfoIndex2, int actor_frame_num2, int actor2_x, int actor2_y)
{
    TileInfo *actor1_info = actor_get_tile_info(actorInfoIndex1, actor_frame_num1);
    uint16 sprite1_width = actor1_info->width;
    uint16 si = actor1_info->height;

    TileInfo *actor2_info = actor_get_tile_info(actorInfoIndex2, actor_frame_num2);
    uint16 sprite2_width = actor2_info->width;
    uint16 di = actor2_info->height;

    if(actor1_x > map_width_in_tiles && actor1_x <= -1)
    {
        sprite1_width = actor1_x + sprite1_width;
        actor1_x = 0;
    }

    if(((actor2_x > actor1_x || actor2_x + sprite2_width <= actor1_x) && (actor2_x < actor1_x || actor1_x + sprite1_width <= actor2_x)) ||
            ((actor1_y - si >= actor2_y || actor2_y > actor1_y) && (actor2_y - di >= actor1_y || actor1_y > actor2_y)))
    {
        return 0;
    }

    return 1;
}

int struct6_1B4FC(int actorInfoIndex, int frame_num, int x_pos, int y_pos)
{
    for(int i=0; i < MAX_STRUCT6_SPRITES; i++)
    {
        struc_6 *sprite = &struc6_sprites[i];

        if(sprite->counter != 0)
        {
            if(struct6_collision_check_maybe(0x1a, 0, sprite->x, sprite->y, actorInfoIndex, frame_num, x_pos, y_pos) != 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

void struct6_update_sprites()
{
    for(int i=0; i < MAX_STRUCT6_SPRITES; i++)
    {
        struc_6 *sprite = &struc6_sprites[i];
        if(sprite->counter != 0)
        {
            if(sprite->counter == 1)
            {
                effect_add_sprite(0x17, 8, sprite->x + 1 + 1, sprite->y - 2, 0, 1);
            }

            display_actor_sprite_maybe(0x1a, sprite->counter - 1 & 3, sprite->x, sprite->y, 0);

            if(player_check_collision_with_actor(0x1a, sprite->counter - 1 & 3, sprite->x, sprite->y) != 0)
            {
                player_decrease_health();
            }

            sprite->counter++;
            if(sprite->counter == 9)
            {
                sprite->counter = 0;
                effect_add_sprite(0x62, 6, sprite->x + 1, sprite->y - 1, 1, 1);
            }
        }
    }
}

void struct6_add_sprite(int x_pos, int y_pos)
{
    for(int i=0; i < MAX_STRUCT6_SPRITES; i++)
    {
        struc_6 *sprite = &struc6_sprites[i];
        if (sprite->counter == 0)
        {
            sprite->counter = 1;
            sprite->x = x_pos;
            sprite->y = y_pos + 2;
            play_sfx(0xa);
            return;
        }
    }
}

void struct6_clear_sprites()
{
    for(int i=0; i < MAX_STRUCT6_SPRITES; i++)
    {
        struc6_sprites[i].counter = 0;
    }
}

void update_brightness_objs()
{
    if(brightness_effect_enabled_flag == 0)
        return;

    for(int i=0;i<num_brightness_objs;i++)
    {
        Brightness *brightness = &brightness_tbl[i];
        if(brightness->x >= mapwindow_x_offset && brightness->x < mapwindow_x_offset + MAP_WINDOW_WIDTH &&
           brightness->y >= mapwindow_y_offset && brightness->y < mapwindow_y_offset + MAP_WINDOW_HEIGHT)
        {
            video_draw_highlight_effect((brightness->x - mapwindow_x_offset + 1) * TILE_WIDTH, (brightness->y - mapwindow_y_offset + 1) * TILE_HEIGHT, brightness->type);

            uint16 tile_attr = map_get_tile_attr(brightness->x,brightness->y + 1);
            for(int j = 1; !(tile_attr & TILE_ATTR_BLOCK_DOWN);)
            {
                if(brightness->x >= mapwindow_x_offset && brightness->x < mapwindow_x_offset + MAP_WINDOW_WIDTH &&
                   brightness->y + j >= mapwindow_y_offset && brightness->y + j < mapwindow_y_offset + MAP_WINDOW_HEIGHT)
                {
                    video_draw_highlight_effect((brightness->x - mapwindow_x_offset + 1) * TILE_WIDTH,
                                                (brightness->y + j - mapwindow_y_offset + 1) * TILE_HEIGHT, 1);
                }
                j++;
                tile_attr = map_get_tile_attr(brightness->x,brightness->y + j);
            }
        }
    }
}

void add_brightness_obj(uint8 type, int x_pos, int y_pos)
{
    if (num_brightness_objs != MAX_BRIGHTNESS_OBJS)
    {
        brightness_tbl[num_brightness_objs].type = type;
        brightness_tbl[num_brightness_objs].x = x_pos;
        brightness_tbl[num_brightness_objs].y = y_pos;
        num_brightness_objs++;
    }
}

void clear_brightness_objs()
{
    num_brightness_objs = 0;
}

