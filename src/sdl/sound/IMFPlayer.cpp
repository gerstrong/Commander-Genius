#include "IMFPlayer.h"
#include "FindFile.h"
#include "hardware/dbopl.h"
#include "fileio/TypeDefinitions.h"
#include <SDL.h>
#include <string>
#include <vector>

/****
 * NOTE: PLEASE DON'T EDIT THIS FILE!
 *
 * It will be removed very soon! Instead check out CIMFPlayer.cpp
 *
 */

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t longword;
typedef void * memptr;

const int IMF_CLOCK_RATE = 560;

//      Internal variables
static  word                    SoundPriority;


//      AdLib variables
static  byte * volatile         alSound;
static  byte                    alBlock;
static  longword                alLengthLeft;
static  longword                alTimeCount;

//      Sequencer variables
static  volatile bool        	sqActive;
static  word                   *sqStartPtr;
static  word                   *sqCurPtr;
static  int		                sqLen;
static  int            		    sqTotalLen;
static  longword                sqHackTime;

Uint32 numreadysamples = 0;
byte *curAlSound = NULL;
byte *curAlSoundPtr = NULL;
longword curAlLengthLeft = 0;
Uint32 soundTimeCounter = 5;
Uint32 samplesPerMusicTick;

static Chip opl_chip;

static Bit32s *mix_buffer = NULL;

SDL_AudioSpec l_AudioSpec;

static  Instrument              alZeroInst;

byte *opl_waveform = NULL;
byte *opl_waveform_ptr = NULL;
Uint32 opl_waveform_size = 0;


///////////////////////////////////////////////////////////////////////////
//
//      SD_Startup() - starts up the Sound Mgr
//              Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
OPL_Startup(const SDL_AudioSpec &AudioSpec)
{
    // Init music
    samplesPerMusicTick = AudioSpec.freq / IMF_CLOCK_RATE;    // SDL_t0FastAsmService played at imf_clock_rate Hz
    l_AudioSpec = AudioSpec;

    mix_buffer = (int32_t *) malloc (samplesPerMusicTick * sizeof(uint32_t));

    DBOPL_InitTables();
    Chip__Chip(&opl_chip);
    Chip__Setup(&opl_chip, AudioSpec.freq);

    alTimeCount = 0;
}

/**
 * \brief Reads the IMF data block and loads it into the emulators memory
 */
bool
readIMFData( byte *imfdata, const uint32_t binsize, const SDL_AudioSpec& AudioSpec )
{
    byte* imf_data_ptr = imfdata;

	l_AudioSpec = AudioSpec;

	word size = 0;

	memcpy( &size, imf_data_ptr, sizeof(word) );

    if (size == 0) // Is the IMF file of Type-0?
    	size = binsize;
    else
    	imf_data_ptr += sizeof(Uint16);

    byte* IMFBuffer = (byte*) malloc(size);

    memcpy(IMFBuffer, imf_data_ptr,size*sizeof(byte));

    sqLen = sqTotalLen = size;
    sqCurPtr = sqStartPtr = (word *)(void *) IMFBuffer;
    sqHackTime = 0;
    alTimeCount = 0;

    sqActive = true;

    return true;
}

static void
SDL_AlSetFXInst(Instrument *inst)
{
    byte c,m;

    m = 0;      // modulator cell for channel 0
    c = 3;      // carrier cell for channel 0

    Chip__WriteReg(&opl_chip, m + alChar, inst->mChar);
    Chip__WriteReg(&opl_chip, m + alScale,inst->mScale);
    Chip__WriteReg(&opl_chip, m + alAttack,inst->mAttack);
    Chip__WriteReg(&opl_chip, m + alSus,inst->mSus);
    Chip__WriteReg(&opl_chip, m + alWave,inst->mWave);

    Chip__WriteReg(&opl_chip, c + alChar,inst->cChar);
    Chip__WriteReg(&opl_chip, c + alScale,inst->cScale);
    Chip__WriteReg(&opl_chip, c + alAttack,inst->cAttack);
    Chip__WriteReg(&opl_chip, c + alSus,inst->cSus);
    Chip__WriteReg(&opl_chip, c + alWave,inst->cWave);

    // Note: Switch commenting on these lines for old MUSE compatibility
//    alOutInIRQ(alFeedCon,inst->nConn);
    Chip__WriteReg(&opl_chip, alFeedCon, 0);
}

//      AdLib Code

///////////////////////////////////////////////////////////////////////////
//
//      SDL_ALStopSound() - Turns off any sound effects playing through the
//              AdLib card
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ALStopSound(void)
{
    alSound = 0;
    Chip__WriteReg(&opl_chip, alFreqH + 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
//      SDL_StartAL() - Starts up the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_StartAL(void)
{
	Chip__WriteReg(&opl_chip, alEffects, 0);
    SDL_AlSetFXInst(&alZeroInst);
}

///////////////////////////////////////////////////////////////////////////
//
//      SDL_ShutAL() - Shuts down the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ShutAL(void)
{
    alSound = 0;
    Chip__WriteReg(&opl_chip, alEffects,0);
    Chip__WriteReg(&opl_chip, alFreqH + 0,0);
    SDL_AlSetFXInst(&alZeroInst);
}

///////////////////////////////////////////////////////////////////////////
//
//      SDL_CleanAL() - Totally shuts down the AdLib card
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_CleanAL(void)
{
    int     i;

    Chip__WriteReg(&opl_chip, alEffects,0);
    for (i = 1; i < 0xf5; i++)
    	Chip__WriteReg(&opl_chip, i, 0);
}

///////////////////////////////////////////////////////////////////////////
//
//      SDL_DetectAdLib() - Determines if there's an AdLib (or SoundBlaster
//              emulating an AdLib) present
//
///////////////////////////////////////////////////////////////////////////
static boolean
SDL_DetectAdLib(void)
{
    for (int i = 1; i <= 0xf5; i++)       // Zero all the registers
    	Chip__WriteReg(&opl_chip, i, 0);

    Chip__WriteReg(&opl_chip, 1, 0x20);             // Set WSE=1
//    alOut(8, 0);                // Set CSM=0 & SEL=0

    return true;
}


void freeISFData()
{
	if(opl_waveform)
		delete [] opl_waveform;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Shutdown() - shuts down the Sound Mgr
//              Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
OPL_Shutdown(void)
{
	if(mix_buffer)
		free(mix_buffer);

	if(sqStartPtr)
		free(sqStartPtr);

	mix_buffer = NULL;
	sqStartPtr = NULL;
}


void LoadWaveform()
{
    std::list<Sint16> wavelist;

    while(1)
    {
        if(numreadysamples)
        {
        	Sint16 sample[numreadysamples*l_AudioSpec.channels];
        	OPLUpdate(sample, numreadysamples);
        	for(unsigned int ctr=0 ; ctr<numreadysamples*l_AudioSpec.channels ; ctr++)
        		wavelist.push_back(sample[ctr]);
        }

        soundTimeCounter--;

        if( soundTimeCounter==0 )
        {
            soundTimeCounter = 5;
            if(curAlSound != alSound)
            {
                curAlSound = curAlSoundPtr = alSound;
                curAlLengthLeft = alLengthLeft;
            }
            if(curAlSound)
            {
                if(*curAlSoundPtr)
                {
                	Chip__WriteReg(&opl_chip, alFreqL, *curAlSoundPtr );
                	Chip__WriteReg(&opl_chip, alFreqH, alBlock );
                }
                else Chip__WriteReg(&opl_chip, alFreqH, 0 );

                curAlSoundPtr++;
                curAlLengthLeft--;
                if(!curAlLengthLeft)
                {
                    curAlSound = alSound = 0;
                    SoundPriority = 0;
                	Chip__WriteReg(&opl_chip, alFreqH, 0 );
                }
            }
        }

        if(sqActive)
        {
        	// This cylce takes the loaded data and writes the value to the Emulator
        	// It's waveform is read back later...
            do
            {
                if(sqHackTime > alTimeCount) break;
                sqHackTime = alTimeCount + *(sqCurPtr+1);
            	Chip__WriteReg(&opl_chip, *(byte *) sqCurPtr, *(((byte *) sqCurPtr)+1) );
            	sqCurPtr += 2;
            	sqLen -= 4;
            }
            while(sqLen>0);

            alTimeCount++;

            if(sqLen<0)
            {
            	sqCurPtr = sqStartPtr;
                sqLen = sqTotalLen;
                sqHackTime = 0;
                alTimeCount = 0;
            	break;
            }
        }

        numreadysamples = samplesPerMusicTick;
    }

    opl_waveform_size = wavelist.size();
    opl_waveform = new byte[opl_waveform_size*sizeof(Sint16)];

    std::list<Sint16>::iterator it = wavelist.begin();
	for( ; it != wavelist.end() ; it++)
	{
		memcpy( opl_waveform, &(*it), sizeof(Sint16));
    }

	opl_waveform_ptr = opl_waveform;
}


///////////////////////////////////////////////////////////////////////////
//
//      SD_PlaySound() - plays the specified sound on the appropriate hardware
//
///////////////////////////////////////////////////////////////////////////
bool LoadSound()
{
    Instrument      *inst;
    byte            *data;

    SDL_ALStopSound();

    AdLibSound *sound = (AdLibSound *) sqStartPtr;

    alLengthLeft = sqLen;
    data = sound->data;
    alBlock = ((sound->block & 7) << 2) | 0x20;
    inst = &sound->inst;

    if (!(inst->mSus | inst->cSus))
    {
        // Bad instrument
    	return false;
    }

    SDL_AlSetFXInst(inst);
    alSound = (byte *)data;

    return true;
}

/*
 * Read the IMF Data and return a new array of a waveform which has the complete sound we look for...
 */
byte *readISFData(const byte *imfdata, const uint32_t &insize, const SDL_AudioSpec &AudioSpec, uint32_t &outsize)
{
	OPL_Startup(AudioSpec);

	SDL_DetectAdLib();
	SDL_StartAL();

	Uint32 size = insize;
	Uint16 priority;

	byte *imfdata_ptr = (byte*)imfdata;

	// Get the ISF size
	size = READLONGWORD(imfdata_ptr);

	// Get the priority
	priority = READWORD(imfdata_ptr);

	byte* IMFBuffer = imfdata_ptr;

	sqStartPtr = (word *) IMFBuffer;

	sqLen = sqTotalLen = size;
	sqCurPtr = sqStartPtr;
	sqHackTime = 0;
	alTimeCount = 0;

	sqActive = true;

	LoadSound();
	LoadWaveform();
	outsize = opl_waveform_size;

	SDL_ShutAL();
	SDL_CleanAL();

	sqStartPtr = NULL;
	sqCurPtr = NULL;

	OPL_Shutdown();

	return opl_waveform;
}
