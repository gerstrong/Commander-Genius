//
// Created on 9/3/18.
//

#include <SDL_keycode.h>
#include "b800.h"
#include "video.h"
#include "input.h"
#include "game.h"

void display_exit_text()
{
    switch(get_episode_number())
    {
        case 1 : display_b800_screen("COSMO1.MNI");
        case 2 : display_b800_screen("COSMO2.MNI");
        case 3 : display_b800_screen("COSMO3.MNI");
    }
}

void display_b800_screen(const char *filename)
{
    uint32 length;
    unsigned char *buf = load_file_in_new_buf(filename, &length);
    if(buf == NULL)
    {
        return;
    }

    set_text_mode();

    unsigned char *ptr = buf;

    for(int y=0; y < TEXT_SCREEN_HEIGHT; y++)
    {
        for(int x=0; x < TEXT_SCREEN_WIDTH; x++)
        {
            video_draw_text(*ptr, ptr[1] & 0xf, (ptr[1]>>4) & 7, x, y);
            ptr+=2;
        }
    }
    video_update();
    flush_input();

    SDL_Keycode keycode = poll_for_key_press(false);
    while(keycode == SDLK_UNKNOWN)
    {
        video_update();
        keycode = poll_for_key_press(false);
    }

    free(buf);
}
