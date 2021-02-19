#ifndef COSMO_ENGINE_ACTOR_H
#define COSMO_ENGINE_ACTOR_H

#include <memory>
#include <base/Singleton.h>
#include <engine/core/CMap.h>
#include "defines.h"
#include "tile.h"

#define MAX_ACTORS 410

typedef struct ActorData
{
    int actorInfoIndex;
    int frame_num;
    int x;
    int y;
    int update_while_off_screen_flag;
    int can_update_if_goes_off_screen_flag;
    int non_blocking_flag_maybe;
    int can_fall_down_flag;
    int has_moved_left_flag;
    int has_moved_right_flag;
    uint16 data_1;
    uint16 data_2;
    uint16 data_3;
    uint16 data_4;
    uint16 data_5;
    int is_deactivated_flag_maybe;
    int falling_counter;
    int count_down_timer;
    void (*update_function)(struct ActorData *);
} ActorData;

//Data
extern int total_num_actors;
extern uint16 question_mark_code;
extern bool speech_bubble_hamburger_shown_flag;
extern bool speech_bubble_red_plant_shown_flag;
extern bool speech_bubble_special_switch_shown_flag;
extern bool speech_bubble_clam_trap_shown_flag;
extern bool speech_bubble_silver_robot_shown_flag;
extern bool speech_bubble_purple_boss_shown_flag;
extern bool speech_bubble_pipe_shown_flag;
extern bool speech_bubble_teleporter_shown_flag;
extern bool speech_bubble_hoverboard_shown_flag;
extern bool speech_bubble_rubber_wall_shown_flag;
extern bool speech_bubble_floor_spring_shown_flag;

extern uint8 byte_32EB8;

extern uint16 rnd_index;

extern uint8 energy_beam_enabled_flag;

extern uint16 num_containers;

extern uint16 num_eye_plants_remaining_to_blow_up;

extern uint16 brightness_effect_enabled_flag;
extern uint16 obj_switch_151_flag;


extern uint8 move_platform_flag;
extern DrawMode actor_tile_display_func_index;

int is_sprite_on_screen(int actorInfoIndex, int frame_num, int x_pos, int y_pos);
BlockingType sprite_blocking_check(int blocking_dir, int actorInfoIndex, int frame_num, int x_pos, int y_pos);

void actor_add_new(int image_index, int x_pos, int y_pos);
int actor_init(int actor_num, int image_index, int x_pos1, int y_pos1);
void load_actor(int actor_num, int actorType, int x_pos, int y_pos);
void actor_load_tiles();


#define gActorMan cosmos_engine::ActorManager::get()

namespace cosmos_engine
{


class ActorManager : public GsSingleton<ActorManager>
{
public:

    void setMapPtr(std::shared_ptr<CMap> &mapPtr);


    void display_sprite_maybe(const int actorInfoIndex,
                              const int frame_num,
                              const float x_pos,
                              const float y_pos,
                              const DrawMode draw_mode);

private:

    std::shared_ptr<CMap> mpMap;
};

};

void display_sprite_maybe(const int actorInfoIndex,
                                const int frame_num,
                                const float x_pos,
                                const float y_pos,
                                const int tile_display_func_index);

uint16 cosmo_random();

void actor_update_all(const bool draw_only);

TileInfo *actor_get_tile_info(int actorInfoIndex, int frame_num);

ActorData *get_actor(uint16 actor_num);

#endif //COSMO_ENGINE_ACTOR_H
