#include "id_sd.h"
//#include "fmopl.h"
#include "dbopl.h"
#include <SDL.h>
#include <string>

//      Internal variables
static  word                    SoundPriority;


//      AdLib variables
static  byte * volatile         alSound;
static  byte                    alBlock;
static  longword                alLengthLeft;
static  longword                alTimeCount;

//      Sequencer variables
static  volatile boolean        sqActive;
static  word                   *sqHack;
static  word                   *sqHackPtr;
static  int                     sqHackLen;
static  int                     sqHackSeqLen;
static  longword                sqHackTime;

static  Instrument              alZeroInst;


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

///////////////////////////////////////////////////////////////////////////
//
//      SD_PlaySound() - plays the specified sound on the appropriate hardware
//
///////////////////////////////////////////////////////////////////////////
bool SD_PlaySound()
{
printf("Play the sound!\n");
    Instrument      *inst;
    byte            *data;

    SDL_ALStopSound();

    AdLibSound *sound = (AdLibSound *) sqHack;

    alLengthLeft = sqHackLen;
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

void SDL_IMFMusicPlayer(void *udata, Uint8 *stream, int len)
{
    int stereolen = len>>1;
    int sampleslen = stereolen>>1;
    Sint16 *stream16 = (Sint16 *) (void *) stream;    // expect correct alignment

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
        if(!soundTimeCounter)
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
            do
            {
                if(sqHackTime > alTimeCount) break;
                sqHackTime = alTimeCount + *(sqHackPtr+1);
            	Chip__WriteReg(&opl_chip, *(byte *) sqHackPtr, *(((byte *) sqHackPtr)+1) );
                sqHackPtr += 2;
                sqHackLen -= 4;
            }
            while(sqHackLen>0);
            alTimeCount++;
            if(!sqHackLen)
            {
                sqHackPtr = sqHack;
                sqHackLen = sqHackSeqLen;
                sqHackTime = 0;
                alTimeCount = 0;
            }
        }
        numreadysamples = samplesPerMusicTick;
    }

    SDL_MixAudio(stream, 0, len, SDL_MIX_MAXVOLUME);
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
    // Open the audio device
    SDL_AudioSpec *desired, *obtained;

    // Allocate a desired SDL_AudioSpec
    desired = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    obtained = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
    desired->freq=mixer_rate;
    desired->format=AUDIO_S16SYS;
    desired->channels=2;
    desired->samples = (mixer_rate * 2048) / 44100;
    desired->callback=SDL_IMFMusicPlayer;
    desired->userdata=NULL;

    // Open the audio device
    if ( SDL_OpenAudio(desired, obtained) < 0 ){
      fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
      return;
    }

    // desired spec is no longer needed
    free(desired);

    // Init music

    samplesPerMusicTick = mixer_rate / imf_clock_rate;    // SDL_t0FastAsmService played at imf_clock_rate Hz

    mix_buffer = (int32_t *) malloc(mixer_rate * sizeof(uint32_t));

    DBOPL_InitTables();
    Chip__Chip(&opl_chip);
    Chip__Setup(&opl_chip, opl_rate);

    SDL_PauseAudio(0);
    alTimeCount = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//      SD_StartMusic() - starts playing the music pointed to
//
///////////////////////////////////////////////////////////////////////////
void
SD_StartIMF(const std::string& filename, AudioType IMFAudioType)
{
    // Load the IMF File here!
    FILE *fp=fopen(filename.c_str(), "rb");

    if(IMFAudioType == IMF_ADLIBSOUND)
    {
	printf("Performing in Adlib Sound mode!\n");
    	SDL_DetectAdLib();
    	SDL_StartAL();

    	Uint32 size;
    	Uint16 priority;

    	// Get the file size
    	fread(&size,sizeof(Uint32),1,fp);

    	fread(&priority,sizeof(Uint16),1,fp);

    	byte* IMFBuffer = (byte*) malloc(size);

    	fread(IMFBuffer,sizeof(byte),size,fp);

    	fclose(fp);

    	sqHack = (word *)(void *) IMFBuffer;

    	sqHackLen = sqHackSeqLen = size;
    	sqHackPtr = sqHack;
    	sqHackTime = 0;
    	alTimeCount = 0;

    	sqActive = true;

    	SD_PlaySound();
    }
    else if(IMFAudioType == IMF_MUSIC)
    {
    	Uint16 size;

    	fread(&size,sizeof(Uint16),1,fp);
    	if (size == 0) // Is the IMF file of Type-0?
    	{
    		fseek(fp, 0L, SEEK_END);
    		size = ftell(fp);
    		fseek(fp, 0L, SEEK_SET);
    	}

    	byte* IMFBuffer = (byte*) malloc(size);

    	fread(IMFBuffer,sizeof(byte),size,fp);

    	fclose(fp);

    	sqHack = (word *)(void *) IMFBuffer;

    	sqHackLen = sqHackSeqLen = size;
    	sqHackPtr = sqHack;
    	sqHackTime = 0;
    	alTimeCount = 0;

    	sqActive = true;
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

	if(sqHack)
		free(sqHack);
}
