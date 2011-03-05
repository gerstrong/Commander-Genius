//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__

#include <string>

/*
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

enum AudioType
{
	IMF_MUSIC,
	IMF_ADLIBSOUND
};

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


#define ORIG_ADLIBSOUND_SIZE (ORIG_SOUNDCOMMON_SIZE + ORIG_INSTRUMENT_SIZE + 2)
*/
// Function prototypes

extern  void    SD_Startup(int mixer_rate),
                SD_Shutdown(void);


extern  void    SD_StartIMF(const std::string& filename, int imf_clock_rate, int opl_rate);
/*extern  void    SD_ContinueMusic(int chunk, int startoffs);
extern  void    SD_MusicOn(void);
extern  int     SD_MusicOff(void);*/

#endif
