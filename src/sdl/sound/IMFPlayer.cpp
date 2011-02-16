#include "IMFPlayer.h"
#include "hardware/dbopl.h"
#include "FindFile.h"
#include "fileio/TypeDefinitions.h"
#include <SDL.h>
#include <string>
#include <vector>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t longword;
typedef void * memptr;


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
static  longword                sqLen;
static  longword                sqTotalLen;
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

template<typename T>
void OPLUpdate(T *buffer, unsigned int length)
{
	Chip__GenerateBlock2(&opl_chip, length, mix_buffer );

    // Mix into the destination buffer, doubling up into stereo.

    for (unsigned int i=0; i<length; ++i)
    {
    	//int pulse = (T) mix_buffer[i];
    	int pulse = (Sint16) mix_buffer[i];

    	if(sizeof(T) != sizeof(Sint16))
    		pulse = (pulse<<(sizeof(T)<<3))>>(sizeof(Sint16)<<3);

    	pulse += l_AudioSpec.silence;

    	for (unsigned int j=0; j<l_AudioSpec.channels; j++)
    		buffer[i * l_AudioSpec.channels + j] = pulse;
    }
}

template<typename T>
void SDL_IMFMusicPlayer(T *stream, unsigned int len)
{
    Uint32 sampleslen = len/(l_AudioSpec.channels*sizeof(T));

    while(1)
    {
        if(numreadysamples)
        {
            if(numreadysamples<sampleslen)
            {
            	OPLUpdate(stream, numreadysamples);
                stream += numreadysamples*l_AudioSpec.channels;
                sampleslen -= numreadysamples;
            }
            else
            {
            	OPLUpdate(stream, sampleslen);
                numreadysamples -= sampleslen;
                return;
            }
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
        	// This cycle takes the loaded data and writes the value to the Emulator
        	// It's waveform is read back later...
            do
            {
                if(sqHackTime > alTimeCount) break;
                const Bit32u reg = *(byte *) sqCurPtr;
                const Bit8u val = *(((byte *) sqCurPtr)+1);
                Chip__WriteReg(&opl_chip, reg, val );
                sqCurPtr++;
                sqHackTime = alTimeCount + *sqCurPtr;
                sqCurPtr++;
                sqLen -= 4;
            }
            while(sqLen>0);

            alTimeCount++;

            if(!sqLen)
            {
            	sqCurPtr = sqStartPtr;
                sqLen = sqTotalLen;
                sqHackTime = 0;
                alTimeCount = 0;
            }
        }

        numreadysamples = samplesPerMusicTick;
    }
}

void PlayIMF(Uint8* buffer, unsigned int length)
{
	if(l_AudioSpec.format == AUDIO_S16)
		SDL_IMFMusicPlayer((Sint16*) buffer, length);
	else if(l_AudioSpec.format == AUDIO_U8)
		SDL_IMFMusicPlayer((Uint8*) buffer, length);
	else if(l_AudioSpec.format == AUDIO_S8)
		SDL_IMFMusicPlayer((Sint8*) buffer, length);
	else if(l_AudioSpec.format == AUDIO_U16)
		SDL_IMFMusicPlayer((Uint16*) buffer, length);
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Startup() - starts up the Sound Mgr
//              Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_Startup(int imf_clock_rate, int mixer_rate, int opl_rate)
{
    // Init music
    samplesPerMusicTick = mixer_rate / imf_clock_rate;    // SDL_t0FastAsmService played at imf_clock_rate Hz

    mix_buffer = (int32_t *) malloc (samplesPerMusicTick * sizeof(uint32_t));

    DBOPL_InitTables();
    Chip__Chip(&opl_chip);
    Chip__Setup(&opl_chip, opl_rate);

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

	SD_Startup (560, AudioSpec.freq, AudioSpec.freq);

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

/**
 * \brief Opens an external IMF File and calls readIMFData to pass the data of the file to the OPL Emulator
 */
bool
openIMFFile(const std::string& filename, const SDL_AudioSpec& AudioSpec)
{
    // Load the IMF File here!
	FILE *fp;

	if( ( fp = OpenGameFile(filename, "rb") ) == NULL )
    	return false;

    // Read the whole binary file into the memory
    fseek(fp, 0, SEEK_END);
    const uint32_t binsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    byte imfdata[binsize];

    if( binsize == fread( imfdata, sizeof(byte), binsize, fp ) )
    {
    	fclose(fp);
    	return readIMFData( imfdata, binsize, AudioSpec );
    }
    else
    {
    	fclose(fp);
    	return false;
    }
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_Shutdown() - shuts down the Sound Mgr
//              Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
SD_Shutdown(void)
{
	if(mix_buffer)
		free(mix_buffer);

	if(sqStartPtr)
		free(sqStartPtr);

	mix_buffer = NULL;
	sqStartPtr = NULL;
}
