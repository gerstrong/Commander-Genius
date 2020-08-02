//
// cosmo_engine created on 22/Nov/2017.
//
#ifndef COSMO_ENGINE_AUDIO_H
#define COSMO_ENGINE_AUDIO_H

#include <defines.h>

typedef enum {
    AUDIO_INT16_SIGNED_LSB,
    AUDIO_FLOAT32_SIGNED_LSB
} AudioFormat;

typedef struct AudioConfig {
    int sampleRate;
    int numChannels;
    AudioFormat format;
    int bytesPerSample;
    bool enabled;
} AudioConfig;

extern struct AudioConfig audioConfig;

void audio_init();
void audio_shutdown();

#endif //COSMO_ENGINE_AUDIO_H
