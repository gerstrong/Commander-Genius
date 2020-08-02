//
// cosmo_engine created on 31/Dec/2017.
//

#include <SDL_keycode.h>
#include <SDL_filesystem.h>
#include "files/file.h"
#include "sound/music.h"
#include "sound/sfx.h"
#include "config.h"
#include "high_scores.h"
#include "input.h"
#include "game.h"
#include "video.h"

#define MAX_SCORE_STRING_LENGTH 13
#define NUM_SCAN_CODES 89

char *data_directory = NULL;
char *save_directory = NULL;
char *game_data_directory = NULL;
const char working_directory[] = ".";

SDL_Keycode scancode_to_keycode(uint8 scan_code);
uint8 keycode_to_scancode(SDL_Keycode keycode);

uint32 read_score(File *file)
{
    uint32 cur_pos = file_get_current_position(file);
    uint32 file_len = file_get_filesize(file);
    char buf[MAX_SCORE_STRING_LENGTH+1];
    memset(buf, 0, MAX_SCORE_STRING_LENGTH+1);
    for(int i=0; i < MAX_SCORE_STRING_LENGTH && cur_pos + i < file_len; i++)
    {
        char c = file_read1(file);
        if(c == ' ')
        {
            break;
        }
        buf[i] = c;
    }

    return (uint32)strtol(buf, NULL, 10);
}

void read_score_name(File *file, char *buf, uint16 buf_size)
{
    uint32 cur_pos = file_get_current_position(file);
    uint32 file_len = file_get_filesize(file);
    memset(buf, 0, buf_size);
    for(int i=0; i < buf_size-1 && cur_pos + i < file_len; i++)
    {
        char c = file_read1(file);
        if(c == 0xa)
        {
            break;
        }
        buf[i] = c;
    }
}

const char *get_config_filename()
{
    switch(get_episode_number())
    {
        case 1 : return "COSMO1.CFG";
        case 2 : return "COSMO2.CFG";
        case 3 : return "COSMO3.CFG";
        default : break;
    }

    return NULL;
}

void load_config_file()
{
    char *path = get_save_dir_full_path(get_config_filename());
    File file;
    clear_high_score_table();
    int config_file_loaded = file_open(path, "rb", &file);
    free(path);

    if(config_file_loaded)
    {
        set_input_command_key(CMD_KEY_UP, scancode_to_keycode(file_read1(&file)));
        set_input_command_key(CMD_KEY_DOWN, scancode_to_keycode(file_read1(&file)));
        set_input_command_key(CMD_KEY_LEFT, scancode_to_keycode(file_read1(&file)));
        set_input_command_key(CMD_KEY_RIGHT, scancode_to_keycode(file_read1(&file)));
        set_input_command_key(CMD_KEY_JUMP, scancode_to_keycode(file_read1(&file)));
        set_input_command_key(CMD_KEY_BOMB, scancode_to_keycode(file_read1(&file)));

        music_on_flag = file_read1(&file);
        sfx_on_flag = file_read1(&file);

        for(int i=0; i < NUM_HIGH_SCORE_ENTRIES; i++)
        {
            uint32 s = read_score(&file);
            char name_buf[HIGH_SCORE_MAX_NAME_LENGTH+1];
            read_score_name(&file, name_buf, HIGH_SCORE_MAX_NAME_LENGTH+1);

            add_high_score_with_name(s, name_buf);
        }
        file_close(&file);
    }
    else
    {
        set_input_command_key(CMD_KEY_UP, SDLK_UP);
        set_input_command_key(CMD_KEY_DOWN, SDLK_DOWN);
        set_input_command_key(CMD_KEY_LEFT, SDLK_LEFT);
        set_input_command_key(CMD_KEY_RIGHT, SDLK_RIGHT);
        set_input_command_key(CMD_KEY_JUMP, SDLK_LCTRL);
        set_input_command_key(CMD_KEY_BOMB, SDLK_LALT);
        music_on_flag = 1;
        sfx_on_flag = 1;

        add_high_score_with_name(0xf4240, "BART");
        add_high_score_with_name(0xdbba0, "LISA");
        add_high_score_with_name(0xc3500, "MARGE");
        add_high_score_with_name(0xaae60, "ITCHY");
        add_high_score_with_name(0x927c0, "SCRATCHY");
        add_high_score_with_name(0x7a120, "MR. BURNS");
        add_high_score_with_name(0x61a80, "MAGGIE");
        add_high_score_with_name(0x493e0, "KRUSTY");
        add_high_score_with_name(0x30d40, "HOMER");
    }
}

void write_config_file()
{
    char *path = get_save_dir_full_path(get_config_filename());
    File file;
    int status = file_open(path, "wb", &file);
    free(path);

    if(!status)
    {
        return;
    }

    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_UP)), &file);
    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_DOWN)), &file);
    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_LEFT)), &file);
    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_RIGHT)), &file);
    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_JUMP)), &file);
    file_write1(keycode_to_scancode(get_input_command_key(CMD_KEY_BOMB)), &file);
    file_write1(music_on_flag, &file);
    file_write1(sfx_on_flag, &file);

    for(int i=0; i < NUM_HIGH_SCORE_ENTRIES; i++)
    {
        char buf[32];
        HighScore high_score = get_high_score(i);
        sprintf(buf, "%d %s", high_score.score, high_score.name);
        for(int j=0;j < strlen(buf); j++)
        {
            file_write1((uint8)buf[j], &file);
        }
        file_write1(0xa, &file);
    }
    file_close(&file);
}

const char scancode_string_tbl[NUM_SCAN_CODES][6] = {
        "NULL",
        "ESC",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "-",
        "=",
        "BKSP",
        "TAB",
        "Q",
        "W",
        "E",
        "R",
        "T",
        "Y",
        "U",
        "I",
        "O",
        "P",
        " ",
        " ",
        "ENTR",
        "CTRL",
        "A",
        "S",
        "D",
        "F",
        "G",
        "H",
        "J",
        "K",
        "L",
        ";",
        "\"",
        " ",
        "LSHFT",
        " ",
        "Z",
        "X",
        "C",
        "V",
        "B",
        "N",
        "M",
        ",",
        ".",
        "?",
        "RSHFT",
        "PRTSC",
        "ALT",
        "SPACE",
        "CAPLK",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "NUMLK",
        "SCRLK",
        "HOME",
        "\x18",
        "PGUP",
        "-",
        "\x1b",
        "5",
        "\x1c",
        "+",
        "END",
        "\x19",
        "PGDN",
        "INS",
        "DEL",
        "SYSRQ",
        " ",
        " ",
        "F11",
        "F12"
};

const SDL_Keycode scancode_to_keycode_tbl[NUM_SCAN_CODES] = {
        SDLK_UNKNOWN,
        SDLK_ESCAPE,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_4,
        SDLK_5,
        SDLK_6,
        SDLK_7,
        SDLK_8,
        SDLK_9,
        SDLK_0,
        SDLK_MINUS,
        SDLK_EQUALS,
        SDLK_BACKSPACE,
        SDLK_TAB,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_r,
        SDLK_t,
        SDLK_y,
        SDLK_u,
        SDLK_i,
        SDLK_o,
        SDLK_p,
        SDLK_LEFTBRACKET,
        SDLK_RIGHTBRACKET,
        SDLK_RETURN,
        SDLK_LCTRL,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_f,
        SDLK_g,
        SDLK_h,
        SDLK_j,
        SDLK_k,
        SDLK_l,
        SDLK_SEMICOLON,
        SDLK_QUOTEDBL,
        SDLK_QUOTE,
        SDLK_UNKNOWN, // Left Shift
        SDLK_SLASH,
        SDLK_z,
        SDLK_x,
        SDLK_c,
        SDLK_v,
        SDLK_b,
        SDLK_n,
        SDLK_m,
        SDLK_COMMA,
        SDLK_PERIOD,
        SDLK_QUESTION,
        SDLK_UNKNOWN, // right shift
        SDLK_PRINTSCREEN,
        SDLK_LALT,
        SDLK_SPACE,
        SDLK_CAPSLOCK,
        SDLK_F1,
        SDLK_F2,
        SDLK_F3,
        SDLK_F4,
        SDLK_F5,
        SDLK_F6,
        SDLK_F7,
        SDLK_F8,
        SDLK_F9,
        SDLK_F10,
        SDLK_NUMLOCKCLEAR,
        SDLK_SCROLLLOCK,
        SDLK_HOME,
        SDLK_UP,
        SDLK_PAGEUP,
        SDLK_KP_MINUS,
        SDLK_LEFT,
        SDLK_KP_5,
        SDLK_RIGHT,
        SDLK_KP_PLUS,
        SDLK_END,
        SDLK_DOWN,
        SDLK_PAGEDOWN,
        SDLK_INSERT,
        SDLK_DELETE,
        SDLK_SYSREQ,
        SDLK_UNKNOWN,
        SDLK_UNKNOWN,
        SDLK_F11,
        SDLK_F12
};

SDL_Keycode scancode_to_keycode(uint8 scan_code)
{
    if(scan_code >= NUM_SCAN_CODES)
    {
        return SDLK_UNKNOWN;
    }

    return scancode_to_keycode_tbl[scan_code];
}

uint8 keycode_to_scancode(SDL_Keycode keycode)
{
    for(uint8 i = 0; i < NUM_SCAN_CODES; i++)
    {
        if(scancode_to_keycode_tbl[i] == keycode)
        {
            return i;
        }
    }

    return 0;
}

const char *scancode_to_string(uint8 scan_code)
{
    if(scan_code >= NUM_SCAN_CODES)
    {
        scan_code = 0;
    }

    return &scancode_string_tbl[scan_code][0];
}

void load_config_from_command_line(int argc, char **argv)
{
    for(int i=1;i<argc; i++)
    {
        if(!strcmp(argv[i], "-ep1") || !strcmp(argv[i], "1"))
        {
            set_episode_number(1);
        }
        if(!strcmp(argv[i], "-ep2") || !strcmp(argv[i], "2"))
        {
            set_episode_number(2);
        }
        if(!strcmp(argv[i], "-ep3") || !strcmp(argv[i], "3"))
        {
            set_episode_number(3);
        }
        if(!strcmp(argv[i], "-fs"))
        {
            video_set_fullscreen(true);
        }
        if(!strcmp(argv[i], "-q"))
        {
            enable_quick_start_mode();
        }
        if(!strncmp(argv[i], "-x", 2) && strlen(argv[i]) > 2)
        {
            int scale_factor = (int)strtol(&argv[i][2], NULL, 10);
            if(scale_factor)
            {
                video_set_scale_factor(scale_factor);
            }
        }
        if(!strcmp(argv[i], "-datadir") && i + 1 < argc)
        {
            i++;
            data_directory = (char *)SDL_malloc(strlen(argv[i]) + 1);
            strcpy(data_directory, argv[i]);
        }
        if(!strcmp(argv[i], "-savedir") && i + 1 < argc)
        {
            i++;
            save_directory = (char *)SDL_malloc(strlen(argv[i]) + 1);
            strcpy(save_directory, argv[i]);
        }
        if(!strcmp(argv[i], "-gamedir") && i + 1 < argc)
        {
            i++;
            game_data_directory = (char *)SDL_malloc(strlen(argv[i]) + 1);
            strcpy(game_data_directory, argv[i]);
        }
    }

    if(data_directory == NULL)
    {
        data_directory = SDL_GetBasePath();
    }
    if(game_data_directory == NULL)
    {
        game_data_directory = SDL_GetBasePath();
    }
}

char *get_full_path(const char *base_dir, const char *filename)
{
    char *path = (char *)malloc(strlen(base_dir) + strlen(filename) + 2); //2 = path delimiter + \0
    sprintf(path, "%s%c%s", base_dir, '/', filename); //FIXME handle windows path separator
    return path;
}

char *get_data_dir_full_path(const char *filename)
{
    const char *base_dir = data_directory != NULL ? data_directory : working_directory;
    return get_full_path(base_dir, filename);
}

char *get_save_dir_full_path(const char *filename)
{
    const char *base_dir = save_directory != NULL ? save_directory : working_directory;
    return get_full_path(base_dir, filename);
}

char *get_game_dir_full_path(const char *filename)
{
    const char *base_dir = game_data_directory != NULL ? game_data_directory : working_directory;
    return get_full_path(base_dir, filename);
}

void config_cleanup()
{
    if(game_data_directory)
    {
        SDL_free(game_data_directory);
    }

    if(save_directory)
    {
        SDL_free(save_directory);
    }
}
