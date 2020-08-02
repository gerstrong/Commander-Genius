
#ifndef COSMO_ENGINE_PLAYER_H
#define COSMO_ENGINE_PLAYER_H

extern const sint16 player_x_offset_tbl[];
extern const sint16 player_y_offset_tbl[];

//Data
extern bool player_is_grabbing_wall_flag;
extern uint16 player_death_counter;
extern int player_walk_anim_index;
extern int player_is_being_pushed_flag;
extern int player_hanging_on_wall_direction;

extern int player_y_pos;
extern int player_x_pos;
extern int player_direction;
extern int player_direction_related_32E98;

extern uint16 player_sprite_dir_frame_offset;
extern int player_input_jump_related_flag;

extern int player_bounce_height_counter;
extern int player_bounce_flag_maybe;

extern int is_standing_slipry_slope_left_flg;
extern int is_standing_slipry_slope_right_flg;

extern uint8 health;
extern uint8 num_health_bars;

extern uint16 num_bombs;
extern uint16 has_had_bomb_flag;

extern int teleporter_state_maybe;
extern int player_is_teleporting_flag;
extern int teleporter_counter;

extern uint8 player_in_pneumatic_tube_flag;

extern uint16 player_invincibility_counter;

extern int player_fall_off_map_bottom_counter;

extern int num_hits_since_touching_ground;
extern uint8 show_monster_attack_hint;

extern uint8 player_has_shown_ouch_bubble_flag;
extern int player_idle_counter;
extern uint16 word_28F94;
extern int word_2E180;
extern int word_2E1E8;
extern int hide_player_sprite;
extern int player_spring_jump_flag;
extern int word_2E214;
extern int player_hoverboard_counter;
extern int word_32EAC;
extern int word_32EB2;

extern unsigned char byte_2E182;
extern unsigned char byte_2E2E4;

extern bool god_mode_flag;

extern bool cheat_hack_mover_enabled;

typedef enum SpeechBubbleType {
    WHOA = 0xf4,
    UMPH = 0xf5,
    POINTS_50000 = 0xf6,
    OUCH = 0xeb
} SpeechBubbleType;

void handle_player_input_maybe();
void player_hoverboard_update();

void sub_11062(); //resets variables

void player_reset_push_variables();

void player_load_tiles();

void display_player_sprite(uint8 frame_num, int x_pos, int y_pos, int tile_display_func_index);

int player_update_sprite();

void player_update_walk_anim();

int player_check_collision_with_actor(int actorInfoIndex, int frame_num, int x_pos, int y_pos);

int player_bounce_in_the_air(int bounce_height);

void player_add_score_for_actor(int actorInfoIndex);
void player_add_to_score(uint32 amount_to_add_low);

void player_decrease_health();

void push_player_around(int push_direction, int push_anim_duration, int push_duration, int player_frame_num, uint8 dont_push_while_jumping_flag, int check_for_blocking_flag);

void player_move_on_platform(int platform_x_left, int platform_x_right, int x_offset_tbl_index, int y_offset_tbl_index);

void player_update_idle_anim();

void player_add_speech_bubble(SpeechBubbleType type);

void player_hack_mover_update();
#endif //COSMO_ENGINE_PLAYER_H
