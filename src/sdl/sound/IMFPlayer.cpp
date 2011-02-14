#include "IMFPlayer.h"
#include "hardware/dbopl.h"
#include "FindFile.h"
#include "fileio/TypeDefinitions.h"
#include <SDL.h>
#include <string>

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
static  volatile bool        sqActive;
static  word                   *sqStartPtr;
static  word                   *sqCurPtr;
static  int                     sqLen;
static  int                     sqTotalLen;
static  longword                sqHackTime;

int numreadysamples = 0;
byte *curAlSound = 0;
byte *curAlSoundPtr = 0;
longword curAlLengthLeft = 0;
int soundTimeCounter = 5;
int samplesPerMusicTick;

static Chip opl_chip;

static int32_t *mix_buffer = NULL;

void OPLUpdate(Sint16 *buffer, int length)
{
	Chip__GenerateBlock2(&opl_chip, length, mix_buffer );

    // Mix into the destination buffer, doubling up into stereo.
    for (int i=0; i<length; ++i)
    {
        buffer[i * 2] = (int16_t) mix_buffer[i];
        buffer[i * 2 + 1] = (int16_t) mix_buffer[i];
    }
}


void SDL_IMFMusicPlayer(Uint8 *stream, int len)
{
    const int stereolen = len>>1;
    int sampleslen = stereolen>>1;
    Sint16 *stream16 = (Sint16 *) stream;    // expect correct alignment

    while(1)
    {
        if(numreadysamples)
        {
            if(numreadysamples<sampleslen)
            {
            	OPLUpdate(stream16, numreadysamples);
                stream16 += numreadysamples*2;
                sampleslen -= numreadysamples;
            }
            else
            {
            	OPLUpdate(stream16, sampleslen);
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
        	// This cylce takes the loaded data and writes the value to the Emulator
        	// It's waveform is read back later...
            do
            {
                if(sqHackTime > alTimeCount) break;
                sqHackTime = alTimeCount + *(sqCurPtr+1);
                const Bit32u reg = *(byte *) sqCurPtr;
                const Bit8u val = *(((byte *) sqCurPtr)+1);
                Chip__WriteReg(&opl_chip, reg, val );
            	sqCurPtr += 2;
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

    mix_buffer = (int32_t *) malloc(mixer_rate * sizeof(uint32_t));

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
