//
// Created by Eric Fry on 22/11/2017.
//

#include <defines.h>
#include "files/file.h"
#include <game.h>
#include <stdlib.h>
#ifdef __EMSCRIPTEN__
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
#endif
#include "sfx.h"
#include "audio.h"

#define SFX_SAMPLE_RATE 140
#define PC_PIT_RATE 1193181
#define WAVE_AMPLITUDE_VALUE 3500

typedef struct Sfx {
    uint8 priority;
    Mix_Chunk *sample;
} Sfx;

Sfx *sfxs;
int num_sfx = 0;
int currently_playing_priority=0;
uint8 sfx_on_flag = 1;

int get_num_sfx(const char *filename)
{
    File file;
    open_file(filename, &file);
    file_seek(&file, 6);
    int count = file_read2(&file);
    file_close(&file);
    return count;
}

int get_num_samples(File *file, int offset, int index, int total)
{
    if(index < total - 1)
    {
        file_seek(file, (index+2)*16);
        int next_offset = file_read2(file);
        return ((next_offset - offset) / 2) - 1;
    }

    return ((file_get_filesize(file) - offset) / 2) - 1;
}

void writeSample(Uint8 *buf, uint16 index, sint16 sample) {
    if (audioConfig.format == AUDIO_FLOAT32_SIGNED_LSB) {
        *(float *)(buf + index * audioConfig.numChannels * audioConfig.bytesPerSample) = (float)sample/234234;
        if (audioConfig.numChannels == 2) {
            *(float *)(buf + index * audioConfig.numChannels * audioConfig.bytesPerSample + audioConfig.bytesPerSample) = (float)sample/234234;
        }
    } else {
        *(sint16 *)(buf + index * audioConfig.numChannels * audioConfig.bytesPerSample) = sample;
        if (audioConfig.numChannels == 2) {
            *(sint16 *)(buf + index * audioConfig.numChannels * audioConfig.bytesPerSample + audioConfig.bytesPerSample) = sample;
        }
    }
}

Mix_Chunk *convert_sfx_to_wave(File *file, int offset, int num_samples)
{
    int sample_length = (audioConfig.sampleRate / SFX_SAMPLE_RATE);
    Mix_Chunk *chunk = (Mix_Chunk *)malloc(sizeof(Mix_Chunk));
    chunk->alen = (Uint32)(num_samples * sample_length * audioConfig.numChannels * audioConfig.bytesPerSample);
    chunk->abuf = (Uint8 *)malloc(chunk->alen);
    chunk->allocated = 0;
    chunk->volume = 128;

    file_seek(file, offset);

    sint16 *wave_data = (sint16 *)chunk->abuf;

    sint16 beepWaveVal = WAVE_AMPLITUDE_VALUE;
//    sint16 velocity = -4;
//    sint16 desiredAmplitude = -WAVE_AMPLITUDE_VALUE;
    uint16 beepHalfCycleCounter = 0;
    for(int i=0; i < num_samples; i++)
    {
        uint16 sample = file_read2(file);
        if (sample)
        {
            float freq = PC_PIT_RATE / (float)sample;
            int half_cycle_length = (int)(audioConfig.sampleRate / (freq * 2));
            //printf("sample %d, freq=%f, half_cycle_len = %d\n", i, freq, half_cycle_length);
            for (int sampleCounter = 0; sampleCounter < sample_length; sampleCounter++) {
                writeSample(chunk->abuf, i*sample_length+sampleCounter, beepWaveVal);

//                beepWaveVal += velocity;
//                if((velocity < 0 && beepWaveVal < desiredAmplitude) || (velocity > 0 && beepWaveVal > desiredAmplitude))
//                {
//                    beepWaveVal = desiredAmplitude;
//                    velocity = 0;
//                }
//                velocity *= 2;

                beepHalfCycleCounter++;
                if (beepHalfCycleCounter >= half_cycle_length) {
                    beepHalfCycleCounter = half_cycle_length != 0 ? (uint16)(beepHalfCycleCounter % half_cycle_length) : (uint16)0;
                    beepWaveVal = -beepWaveVal;
//                    desiredAmplitude = -desiredAmplitude;
//                    if(desiredAmplitude < 0)
//                    {
//                        velocity = -4;
//                    }
//                    else
//                    {
//                        velocity = 4;
//                    }
                }
            }
        }
        else
        {
            memset(&wave_data[i*sample_length*audioConfig.numChannels], 0, sample_length * audioConfig.numChannels * audioConfig.bytesPerSample); //silence
        }
    }

    return chunk;
}

int load_sfx_file(const char *filename, int sfx_offset)
{
    File file;
    open_file(filename, &file);
    file_seek(&file, 6);
    int count = file_read2(&file);
    printf("%s, %d, %d\n", filename, count, sfx_offset);
    for(int i=0; i < MAX_SAMPLES_PER_FILE; i++)
    {
        file_seek(&file, (i+1) * 16); //+1 to skip header.
        int offset = file_read2(&file);
        Sfx *sfx = &sfxs[sfx_offset + i];
        sfx->priority = file_read1(&file);
        int num_samples = get_num_samples(&file, offset, i, count);
//        printf("sfx[%d] samples = %d\n", i+sfx_offset, num_samples);
        sfx->sample = convert_sfx_to_wave(&file, offset, num_samples);
    }
    return MAX_SAMPLES_PER_FILE;
}

void load_sfx()
{
    num_sfx = 0;
    num_sfx += get_num_sfx("SOUNDS.MNI");
    num_sfx += get_num_sfx("SOUNDS2.MNI");
    num_sfx += get_num_sfx("SOUNDS3.MNI");

    printf("Total Sfx %d\n", num_sfx);

    sfxs = (Sfx *)malloc(sizeof(Sfx) * num_sfx);

    int sfx_offset = load_sfx_file("SOUNDS.MNI", 0);
    sfx_offset += load_sfx_file("SOUNDS2.MNI", sfx_offset);
    load_sfx_file("SOUNDS3.MNI", sfx_offset);
}

void play_sfx(int sfx_number)
{
    if(sfx_on_flag)
    {
        sfx_number--;
        if(Mix_Playing(0))
        {
            if(sfxs[sfx_number].priority < currently_playing_priority)
                return;
        }

        currently_playing_priority = sfxs[sfx_number].priority;
        Mix_PlayChannel(0, sfxs[sfx_number].sample, 0);
    }
}
