//
// cosmo_engine created on 09/Jan/2018.
//

#include <stdlib.h>
#include "demo.h"
#include "defines.h"
#include "game.h"
#include "input.h"

uint16 demo_input_index = 0;
uint16 demo_input_length = 0;

uint8 *demo_data = NULL;

void load_demo()
{
    demo_input_index = 0;
    demo_input_length = 0;

    if(demo_data)
    {
        free(demo_data);
    }

    uint32 file_size;
    demo_data = load_file_in_new_buf("PREVDEMO.MNI", &file_size);
    if(!demo_data)
    {
        printf("Error: Loading demo data.\n");
        return;
    }

    demo_input_length = (uint16)(file_size - 2);
}

uint8 read_input_from_demo()
{
    if(demo_input_index >= demo_input_length)
    {
        return 1;
    }

    uint8 input = demo_data[demo_input_index + 2];
    demo_input_index++;

    left_key_pressed = (uint8)(input & 1);
    right_key_pressed = (uint8)((input >> 1) & 1);
    up_key_pressed = (uint8)((input >> 2) & 1);
    down_key_pressed = (uint8)((input >> 3) & 1);
    jump_key_pressed = (uint8)((input >> 4) & 1);
    bomb_key_pressed = (uint8)((input >> 5) & 1);
    finished_level_flag_maybe = (uint8)((input >> 6) & 1);

    return 0;
}
