//
// Created by Eric Fry on 31/10/2017.
//


#include <SDL_events.h>
#include "sound/sfx.h"
#include <SDL_timer.h>
#include <SDL.h>
#include "input.h"
#include "game.h"
#include "player.h"
#include "status.h"
#include "dialog.h"
#include "demo.h"
#include "config.h"

SDL_Keycode cfg_up_key;
SDL_Keycode cfg_down_key;
SDL_Keycode cfg_left_key;
SDL_Keycode cfg_right_key;
SDL_Keycode cfg_jump_key;
SDL_Keycode cfg_bomb_key;

uint8 bomb_key_pressed = 0;
uint8 jump_key_pressed = 0;
uint8 up_key_pressed = 0;
uint8 down_key_pressed = 0;
uint8 left_key_pressed = 0;
uint8 right_key_pressed = 0;

//This is needed because the game manipulates up_key_pressed as part of the hover board logic. This is the actual
//key pressed state.
uint8 input_up_key_pressed = 0;

uint8 byte_2E17C; //modifies the left, right and jump key presses TODO this isn't wired up yet. It might disable player input.

SDL_GameController *game_controller = NULL;
Sint16 controller_axis_prev_value[SDL_CONTROLLER_AXIS_MAX];

bool input_init() {
    if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        printf("ERROR: intialising game controller subsystem!\n");
        return false;
    }

    char * mappingFilename = get_data_dir_full_path("gamecontrollerdb.txt");
    int numMappings = SDL_GameControllerAddMappingsFromFile(mappingFilename);
    SDL_Log("Game controllers found: %d Mappings: \'%s\' Count: %d", SDL_NumJoysticks(), mappingFilename, numMappings);
    free(mappingFilename);

    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            char *mapping;
            SDL_Log("Index \'%i\' is a compatible controller, named \'%s\'", i, SDL_GameControllerNameForIndex(i));
            game_controller = SDL_GameControllerOpen(i);
            mapping = SDL_GameControllerMapping(game_controller);
            SDL_Log("Controller %i is mapped as \"%s\".", i, mapping);
            SDL_free(mapping);

            for (int j = 0;j < SDL_CONTROLLER_AXIS_MAX; j++) {
                controller_axis_prev_value[j] = SDL_GameControllerGetAxis(game_controller, j);
            }

            return true;
        } else {
            SDL_Log("Index \'%i\' is not a compatible controller.", i);
        }
    }

    return false;
}

void input_shutdown() {
    if (game_controller) {
        SDL_GameControllerClose(game_controller);
    }

    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void wait_for_time_or_key(int delay_in_game_cycles)
{
    reset_player_control_inputs();
    for(int i=0;i < delay_in_game_cycles; i++)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN && !event.key.repeat)
            {
                return;
            }
        }
        cosmo_wait(1);
    }
}

void cosmo_wait(int delay)
{
    SDL_Delay((Uint32)(8 * delay));
}

input_state_enum handle_demo_input()
{
    if(poll_for_key_press(false) != SDLK_UNKNOWN)
    {
        return QUIT;
    }

    if(read_input_from_demo())
    {
        return QUIT;
    }

    return CONTINUE;
}

InputCommand get_input_command_from_keycode(SDL_Keycode keycode)
{
    if (keycode == cfg_up_key)
    {
        return CMD_KEY_UP;
    }
    if (keycode == cfg_down_key)
    {
        return CMD_KEY_DOWN;
    }
    if (keycode == cfg_left_key)
    {
        return CMD_KEY_LEFT;
    }
    if (keycode == cfg_right_key)
    {
        return CMD_KEY_RIGHT;
    }
    if (keycode == cfg_jump_key)
    {
        return CMD_KEY_JUMP;
    }
    if (keycode == cfg_bomb_key)
    {
        return CMD_KEY_BOMB;
    }
    if (keycode == SDLK_ESCAPE || keycode == SDLK_F1)
    {
        return CMD_KEY_MENU;
    }

    return CMD_KEY_OTHER;
}

InputCommand get_input_command_from_controller(SDL_Event *event)
{
    if (event->type == SDL_CONTROLLERBUTTONDOWN || event->type == SDL_CONTROLLERBUTTONUP) {
        switch (event->cbutton.button)
        {
            case SDL_CONTROLLER_BUTTON_A :
                return CMD_KEY_JUMP;
            case SDL_CONTROLLER_BUTTON_B :
                return CMD_KEY_BOMB;
            case SDL_CONTROLLER_BUTTON_START :
                return CMD_KEY_MENU;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT :
                return CMD_KEY_LEFT;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT :
                return CMD_KEY_RIGHT;
            case SDL_CONTROLLER_BUTTON_DPAD_UP :
                return CMD_KEY_UP;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN :
                return CMD_KEY_DOWN;
            default :
                break;
        }
    }

    return CMD_KEY_OTHER;
}

input_state_enum handleInputPressed(InputCommand command)
{
    switch(command)
    {
        case CMD_KEY_UP :
            input_up_key_pressed = 1;
            break;
        case CMD_KEY_DOWN :
            down_key_pressed = 1;
            break;
        case CMD_KEY_LEFT :
            left_key_pressed = 1;
            break;
        case CMD_KEY_RIGHT :
            right_key_pressed = 1;
            break;
        case CMD_KEY_JUMP :
            jump_key_pressed = 1;
            break;
        case CMD_KEY_BOMB :
            bomb_key_pressed = 1;
            break;
        case CMD_KEY_MENU :
            switch(help_menu_dialog())
            {
                case 0 : break;
                case 1 : break;
                case 2 : return QUIT;
                default : break;
            }
            break;
        default :
            break;
    }

    up_key_pressed = input_up_key_pressed;
    return CONTINUE;
}

void handleInputReleased(InputCommand command) {
    switch(command)
    {
        case CMD_KEY_UP :
            input_up_key_pressed = 0;
            break;
        case CMD_KEY_DOWN :
            down_key_pressed = 0;
            break;
        case CMD_KEY_LEFT :
            left_key_pressed = 0;
            break;
        case CMD_KEY_RIGHT :
            right_key_pressed = 0;
            break;
        case CMD_KEY_JUMP :
            jump_key_pressed = 0;
            break;
        case CMD_KEY_BOMB :
            bomb_key_pressed = 0;
            break;
        default : break;
    }
    up_key_pressed = input_up_key_pressed;
}

input_state_enum handle_key_down(SDL_KeyboardEvent event)
{
    InputCommand command = get_input_command_from_keycode(event.keysym.sym);

    if (handleInputPressed(command) == QUIT) {
        return QUIT;
    }

    switch(event.keysym.sym)
    {
        case SDLK_b : //FIXME testing code
            num_bombs++;
            display_num_bombs_left();
            break;
        case SDLK_g :
            god_mode_toggle_dialog(); //TODO should be protected by cheat mode
            break;
        case SDLK_m :
            music_toggle_dialog();
            break;
        case SDLK_w :
            warp_mode_dialog(); //FIXME remove this. Only here for testing.
            break;
        case SDLK_s :
            sound_toggle_dialog();
            break;
        case SDLK_q :
            if (quit_game_dialog())
            {
                return QUIT;
            }
            break;
        case SDLK_h :
            hack_mover_toggle_dialog();
            break;
        case SDLK_i : //FIXME testing code
            printf("player info x_pos =%d; y_pos = %d;\n", player_x_pos, player_y_pos);
            break;
        default : break;
    }

    return CONTINUE;
}

input_state_enum handle_key_up(SDL_KeyboardEvent event)
{
    InputCommand command = get_input_command_from_keycode(event.keysym.sym);
    handleInputReleased(command);

    return CONTINUE;
}

input_state_enum handle_controller_button_down(SDL_Event *event) {
    InputCommand command = get_input_command_from_controller(event);
    return handleInputPressed(command);
}

input_state_enum handle_controller_button_up(SDL_Event *event) {
    InputCommand command = get_input_command_from_controller(event);
    handleInputReleased(command);

    return CONTINUE;
}

input_state_enum handle_controller_axis_motion(SDL_Event *event) {
    if (event->type == SDL_CONTROLLERAXISMOTION) {
        switch (event->caxis.axis) {
            case SDL_CONTROLLER_AXIS_LEFTX:
            case SDL_CONTROLLER_AXIS_RIGHTX:
                if (event->caxis.value < -16384) {
                    left_key_pressed = 1;
                } else {
                    if (controller_axis_prev_value[event->caxis.axis] < -16384) {
                        left_key_pressed = 0;
                    }
                }
                if (event->caxis.value > 16384) {
                    right_key_pressed = 1;
                } else {
                    if (controller_axis_prev_value[event->caxis.axis] > 16384) {
                        right_key_pressed = 0;
                    }
                }
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
            case SDL_CONTROLLER_AXIS_RIGHTY:
                if (event->caxis.value < -16384) {
                    input_up_key_pressed = 1;
                } else {
                    if (controller_axis_prev_value[event->caxis.axis] < -16384) {
                        input_up_key_pressed = 0;
                    }
                }
                if (event->caxis.value > 16384) {
                    down_key_pressed = 1;
                } else {
                    if (controller_axis_prev_value[event->caxis.axis] > 16384) {
                        down_key_pressed = 0;
                    }
                }
                break;
        }
        controller_axis_prev_value[event->caxis.axis] = event->caxis.value;
    }
    up_key_pressed = input_up_key_pressed;
    return CONTINUE;
}

input_state_enum read_input()
{
    if(game_play_mode ==  PLAY_DEMO)
    {
        return handle_demo_input();
    }

    //FIXME handle cheats.
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return QUIT;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if(handle_key_down(event.key) == QUIT)
                return QUIT;
        }
        else if (event.type == SDL_KEYUP)
        {
            handle_key_up(event.key);
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            if (handle_controller_button_down(&event) == QUIT) {
                return QUIT;
            }
        }
        else if (event.type == SDL_CONTROLLERBUTTONUP)
        {
            handle_controller_button_up(&event);
        }
        else if (event.type == SDL_CONTROLLERAXISMOTION)
        {
            handle_controller_axis_motion(&event);
        }
    }

    up_key_pressed = input_up_key_pressed;

    return CONTINUE;
}

void reset_player_control_inputs()
{
    up_key_pressed = 0;
    input_up_key_pressed = 0;
    down_key_pressed = 0;
    left_key_pressed = 0;
    right_key_pressed = 0;
    bomb_key_pressed = 0;
    jump_key_pressed = 0;
}

bool is_controller_event(SDL_Event *event) {
    return event->type == SDL_CONTROLLERBUTTONDOWN
    || event->type == SDL_CONTROLLERBUTTONUP
    || event->type == SDL_CONTROLLERAXISMOTION;
}

SDL_Keycode poll_for_key_press(bool allow_key_repeat)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        if (is_controller_event(&event)) {
            if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                InputCommand inputCommand = get_input_command_from_controller(&event);
                if (inputCommand == CMD_KEY_UP) {
                    return SDLK_UP;
                }
                if (inputCommand == CMD_KEY_DOWN) {
                    return SDLK_DOWN;
                }
                if (inputCommand == CMD_KEY_LEFT) {
                    return SDLK_LEFT;
                }
                if (inputCommand == CMD_KEY_RIGHT) {
                    return SDLK_RIGHT;
                }
                if (inputCommand == CMD_KEY_JUMP) {
                    return SDLK_RETURN;
                }
                if (inputCommand == CMD_KEY_BOMB) {
                    return SDLK_ESCAPE;
                }
            }
            //TODO add support for analog stick directional events here.
        }
        if (event.type == SDL_KEYDOWN && (allow_key_repeat || !event.key.repeat)) {
            return event.key.keysym.sym;
        }
    }
    return SDLK_UNKNOWN;
}

void set_input_command_key(InputCommand command, SDL_Keycode keycode)
{
    switch (command)
    {
        case CMD_KEY_UP : cfg_up_key = keycode; break;
        case CMD_KEY_DOWN : cfg_down_key = keycode; break;
        case CMD_KEY_LEFT : cfg_left_key = keycode; break;
        case CMD_KEY_RIGHT : cfg_right_key = keycode; break;
        case CMD_KEY_JUMP : cfg_jump_key = keycode; break;
        case CMD_KEY_BOMB : cfg_bomb_key = keycode; break;
        case CMD_KEY_OTHER : break;
        default : break;
    }
}

SDL_Keycode get_input_command_key(InputCommand command)
{
    switch (command)
    {
        case CMD_KEY_UP : return cfg_up_key;
        case CMD_KEY_DOWN : return cfg_down_key;
        case CMD_KEY_LEFT : return cfg_left_key;
        case CMD_KEY_RIGHT : return cfg_right_key;
        case CMD_KEY_JUMP : return cfg_jump_key;
        case CMD_KEY_BOMB : return cfg_bomb_key;
        case CMD_KEY_OTHER : break;
        default : break;
    }

    return SDLK_UNKNOWN;
}

const char *get_command_key_string(InputCommand command)
{
    switch (command)
    {
        case CMD_KEY_UP : return scancode_to_string(keycode_to_scancode(cfg_up_key));
        case CMD_KEY_DOWN : return scancode_to_string(keycode_to_scancode(cfg_down_key));
        case CMD_KEY_LEFT : return scancode_to_string(keycode_to_scancode(cfg_left_key));
        case CMD_KEY_RIGHT : return scancode_to_string(keycode_to_scancode(cfg_right_key));
        case CMD_KEY_JUMP : return scancode_to_string(keycode_to_scancode(cfg_jump_key));
        case CMD_KEY_BOMB : return scancode_to_string(keycode_to_scancode(cfg_bomb_key));
        case CMD_KEY_OTHER : break;
        default : break;
    }

    return scancode_to_string(0);
}

void flush_input()
{
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP);
}

bool is_return_key(SDL_Keycode key)
{
    return (key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_RETURN2);
}

HintDialogInput hint_dialog_get_input(HintDialogInput input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_SPACE)
            {
                return FAST_FORWARD;
            }
            else
            {
                return EXIT;
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                return NO_INPUT;
            }
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) {
                return EXIT;
            } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
                return FAST_FORWARD;
            }
            return NO_INPUT;
        }
        else if (event.type == SDL_CONTROLLERBUTTONUP)
        {
            if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
                return NO_INPUT;
            }
        }
    }

    return input;
}
