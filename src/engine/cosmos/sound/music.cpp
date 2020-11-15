//
// Created by efry on 3/11/2017.
//

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
#endif
#include <game.h>
#include "music.h"
#include "opl.h"
#include "audio.h"

#define MUSIC_INSTRUCTION_RATE 560 //Hz
#define ADLIB_OP_SIZE 4

//Data
sint8 music_index = -1;

uint8 *music_data;
uint32 music_data_length;

uint32 adlib_instruction_position = 0;
uint32 delay_counter = 0;

uint8 music_on_flag = 1;

//Get delay between adlib commands. Measured in audio samples.
uint32 get_delay(uint32 instruction_num)
{
    return (audioConfig.sampleRate / MUSIC_INSTRUCTION_RATE) * (uint32)((uint16)music_data[instruction_num*ADLIB_OP_SIZE+2] + ((uint16)music_data[instruction_num*ADLIB_OP_SIZE+3] << 8));
}

void music_player_function(void *udata, Uint8 *stream, int len)
{
    int num_samples = len / audioConfig.numChannels / audioConfig.bytesPerSample;
    uint8 is_stereo = audioConfig.numChannels == 2 ? 1 : 0;

    for(int i=num_samples;i > 0;)
    {
        if(delay_counter == 0)
        {
            adlib_write(music_data[adlib_instruction_position*ADLIB_OP_SIZE], music_data[adlib_instruction_position*ADLIB_OP_SIZE+1]);
            delay_counter = get_delay(adlib_instruction_position);
            adlib_instruction_position++;
            if(adlib_instruction_position * ADLIB_OP_SIZE >= music_data_length)
            {
                adlib_instruction_position = 0;
            }
        }
        if(delay_counter > i)
        {
            delay_counter -= i;
            adlib_getsample(stream, i, is_stereo, audioConfig.format);
            return;
        }
        if(delay_counter <= i)
        {
            i -= delay_counter;
            adlib_getsample(stream, delay_counter, is_stereo, audioConfig.format);
            stream += delay_counter * audioConfig.numChannels * audioConfig.bytesPerSample;
            delay_counter = 0;
        }
    }
}

const char music_filename_tbl[][13] = {
        "MCAVES.MNI",
        "MSCARRY.MNI",
        "MBOSS.MNI",
        "MRUNAWAY.MNI",
        "MCIRCUS.MNI",
        "MTEKWRD.MNI",
        "MEASYLEV.MNI",
        "MROCKIT.MNI",
        "MHAPPY.MNI",
        "MDEVO.MNI",
        "MDADODA.MNI",
        "MBELLS.MNI",
        "MDRUMS.MNI",
        "MBANJO.MNI",
        "MEASY2.MNI",
        "MTECK2.MNI",
        "MTECK3.MNI",
        "MTECK4.MNI",
        "MZZTOP.MNI"
};

void load_music(uint16 new_music_index)
{
    if (!audioConfig.enabled) {
        return;
    }

    adlib_instruction_position = 0;
    delay_counter = 0;

    if(music_index == new_music_index)
    {
        play_music();
        return;
    }

    if(music_index != -1)
    {
        Mix_HookMusic(NULL, NULL);
        free(music_data);
    }

    music_index = new_music_index;

    music_data = load_file_in_new_buf(music_filename_tbl[music_index], &music_data_length);

    play_music();
}

void music_init()
{
    adlib_init(audioConfig.sampleRate);
}

void stop_music()
{
    if (audioConfig.enabled) {
        Mix_HookMusic(NULL, NULL);
    }
}

void play_music()
{
    if(audioConfig.enabled && music_index != -1 && music_on_flag)
    {
        Mix_HookMusic(music_player_function, NULL);
    }
}
