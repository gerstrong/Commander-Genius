//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__

#include <string>
#include "fileio/TypeDefinitions.h"
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


typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef int8_t boolean;
typedef void * memptr;


typedef struct
{
    byte    mChar,cChar,
            mScale,cScale,
            mAttack,cAttack,
            mSus,cSus,
            mWave,cWave,
            nConn,

            // These are only for Muse - these bytes are really unused
            voice,
            mode;
    byte    unused[3];
} Instrument;

#define ORIG_INSTRUMENT_SIZE 16

typedef struct
{
    Instrument      inst;
    byte            block;
    byte            data[1];
} AdLibSound;


// Function prototypes
void    OPL_Startup(int imf_clock_rate, int mixer_rate, int opl_rate),
        OPL_Shutdown(void);

void PlayIMF(Uint8* buffer, unsigned int length);

void PlayWaveform(Uint8 *stream, int len);

bool    openIMFFile( const std::string& filename, const SDL_AudioSpec& AudioSpec );
bool	readIMFData( byte *imfdata, const uint32_t binsize, const SDL_AudioSpec& AudioSpec );

byte *readISFData(const byte *imfdata, const uint32_t &insize, const SDL_AudioSpec &AudioSpec, uint32_t &outsize);

void freeISFData();

#endif
