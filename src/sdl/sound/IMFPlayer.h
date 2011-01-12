//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__

#include <string>
#include <SDL.h>

//      Register addresses
// Operator stuff
#define alChar          0x20
#define alScale         0x40
#define alAttack        0x60
#define alSus           0x80
#define alWave          0xe0
// Channel stuff
#define alFreqL         0xa0
#define alFreqH         0xb0
#define alFeedCon       0xc0
// Global stuff
#define alEffects       0xbd



// Function prototypes
void    SD_Startup(int imf_clock_rate, int mixer_rate, int opl_rate),
        SD_Shutdown(void);

void SDL_IMFMusicPlayer(Uint8 *stream, int len);

bool    openIMFFile(const std::string& filename, const SDL_AudioSpec& AudioSpec);

#endif
