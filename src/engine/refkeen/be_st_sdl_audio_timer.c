#include "SDL.h"

#include "be_cross.h"
#include "be_st.h"
#include "opl/dbopl.h"

#define PC_PIT_RATE 1193182

static SDL_mutex* g_sdlCallbackMutex = NULL;
static SDL_AudioSpec g_sdlAudioSpec;

bool g_sdlAudioSubsystemUp;

static bool g_sdlEmulatedOPLChipReady;
static uint32_t g_sdlSampleOffsetInSound, g_sdlSamplePerPart;
static void (*g_sdlCallbackSDFuncPtr)(void) = 0;

// Used for filling with samples from alOut (alOut_lLw), in addition
// to SD_SDL_CallBack (because waits between/after AdLib writes are expected)

#define OPL_NUM_OF_SAMPLES 512
#define OPL_SAMPLE_RATE 49716
// Use this if the audio subsystem is disabled for most (we want a BYTES rate of 1000Hz, same units as used in values returned by SDL_GetTicks())
#define NUM_OF_SAMPLES_WITH_DISABLED_SUBSYSTEM 500

static int16_t g_sdlALOutSamples[OPL_NUM_OF_SAMPLES];
static uint32_t g_sdlALOutSamplesStart = 0, g_sdlALOutSamplesEnd = 0;

// Sample rate conversion (from the OPL orate of ~49716Hz to mixing rate):
// For each one out of 49716 AdLib samples, tells how many times it should be repeated for the sake of conversion.
static int g_sdlALSampleRateConvTable[OPL_SAMPLE_RATE];
// Current location in the convertsion process;
static int g_sdlALSampleRateConvCurrIndex; // Index to g_sdlALSampleRateConvTable
static int g_sdlALSampleRateConvCounter; // Counter for current cell of g_sdlALSampleRateConvTable

//#define BE_ST_ENABLE_LOW_PASS_FILTERING 1
#ifdef BE_ST_ENABLE_LOW_PASS_FILTERING
// Used for low-pass filtering
static float g_sdlALLastSampleForFiltering;
static float g_sdlALFilteringAlpha;
#endif

// PC Speaker current status
static bool g_sdlPCSpeakerOn = false;
static int16_t g_sdlCurrentBeepSample;
static uint32_t g_sdlBeepHalfCycleCounter, g_sdlBeepHalfCycleCounterUpperBound;

// PIT timer divisor
static uint32_t g_sdlScaledTimerDivisor;

// A variable used for timing measurements
static uint32_t g_sdlLastTicks;


// A PRIVATE TimeCount variable we store
// (SD_GetTimeCount/SD_SetTimeCount should be called instead)
static uint32_t g_sdlTimeCount;

static void BEL_ST_CallBack(void *unused, Uint8 *stream, int len);
static inline bool YM3812Init(int numChips, int clock, int rate);

void BE_ST_InitAudio(void)
{
	g_sdlAudioSubsystemUp = false;
	g_sdlEmulatedOPLChipReady = false;
	if (!g_refKeenCfg.disableSoundSubSystem)
	{
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		{
			BE_Cross_LogMessage(BE_LOG_MSG_WARNING, "SDL audio system initialization failed,\n%s\n", SDL_GetError());
		}
		else
		{
			g_sdlAudioSpec.freq = g_refKeenCfg.sndSampleRate;
			g_sdlAudioSpec.format = AUDIO_S16;
			g_sdlAudioSpec.channels = 1;
			g_sdlAudioSpec.samples = 512;
			g_sdlAudioSpec.callback = BEL_ST_CallBack;
			g_sdlAudioSpec.userdata = NULL;
			if (SDL_OpenAudio(&g_sdlAudioSpec, NULL))
			{
				BE_Cross_LogMessage(BE_LOG_MSG_WARNING, "Cannot open SDL audio device,\n%s\n", SDL_GetError());
				SDL_QuitSubSystem(SDL_INIT_AUDIO);
			}
			else
			{
				g_sdlCallbackMutex = SDL_CreateMutex();
				if (!g_sdlCallbackMutex)
				{
					BE_Cross_LogMessage(BE_LOG_MSG_ERROR, "Cannot create recursive mutex for SDL audio callback,\n%s\nClosing SDL audio subsystem\n", SDL_GetError());
					SDL_CloseAudio();
					SDL_QuitSubSystem(SDL_INIT_AUDIO);
				}
				else
				{
					g_sdlAudioSubsystemUp = true;
				}
			}
		}
	}
	// If the audio subsystem is off, let us simulate a byte rate
	// of 1000Hz (same as SDL_GetTicks() time units)
	if (!g_sdlAudioSubsystemUp)
	{
		g_sdlAudioSpec.freq = NUM_OF_SAMPLES_WITH_DISABLED_SUBSYSTEM;
	}
	else
	{
		if (YM3812Init(1, 3579545, OPL_SAMPLE_RATE))
		{
			BE_Cross_LogMessage(BE_LOG_MSG_WARNING, "Preparation of emulated OPL chip has failed\n");
		}
		else
		{
			g_sdlEmulatedOPLChipReady = true;
			if (g_sdlAudioSpec.freq != OPL_SAMPLE_RATE)
			{
				// The some of all entries should be g_sdlAudioSpec.freq,
				// "uniformly" distributed over g_sdlALSampleRateConvTable
				for (int i = 0; i < OPL_SAMPLE_RATE; ++i)
				{
					// Using uint64_t cause an overflow is possible
					g_sdlALSampleRateConvTable[i] = ((uint64_t)(i+1)*(uint64_t)g_sdlAudioSpec.freq/OPL_SAMPLE_RATE)-(uint64_t)i*(uint64_t)g_sdlAudioSpec.freq/OPL_SAMPLE_RATE;
				}
#ifdef BE_ST_ENABLE_LOW_PASS_FILTERING
				// Low-pass filtering can help in such a case
				if (g_sdlAudioSpec.freq < OPL_SAMPLE_RATE)
				{
					g_sdlALLastSampleForFiltering = 0.0;
					float RC = 1.0f/(2.0f*3.14159265358979f*g_sdlAudioSpec.freq);
					float dt = 1.0f/(float)OPL_SAMPLE_RATE;
					g_sdlALFilteringAlpha = dt/(RC + dt);
				}
#endif
			}
		}
	}
}

void BE_ST_ShutdownAudio(void)
{
	if (g_sdlAudioSubsystemUp)
	{
		SDL_DestroyMutex(g_sdlCallbackMutex);
		g_sdlCallbackMutex = NULL;
		SDL_CloseAudio();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		g_sdlAudioSubsystemUp = false;
	}
	g_sdlCallbackSDFuncPtr = 0; // Just in case this may be called after the audio subsystem was never really started (manual calls to callback)
}

void BE_ST_StartAudioSDService(void (*funcPtr)(void))
{
	g_sdlCallbackSDFuncPtr = funcPtr;
	if (g_sdlAudioSubsystemUp)
	{
		SDL_PauseAudio(0);
	}
}

void BE_ST_StopAudioSDService(void)
{
	if (g_sdlAudioSubsystemUp)
	{
		SDL_PauseAudio(1);
	}
	g_sdlCallbackSDFuncPtr = 0;
}

void BE_ST_LockAudioRecursively(void)
{
	if (g_sdlAudioSubsystemUp)
	{
		SDL_LockMutex(g_sdlCallbackMutex);
	}
}

void BE_ST_UnlockAudioRecursively(void)
{
	if (g_sdlAudioSubsystemUp)
	{
		SDL_UnlockMutex(g_sdlCallbackMutex);
	}
}

// Use this ONLY if audio subsystem isn't properly started up
void BE_ST_PrepareForManualAudioSDServiceCall(void)
{
	// HACK: Rather than using SDL_PauseAudio for deciding if
	// we call, just check if g_sdlCallbackSDFuncPtr is non-NULL
	// (not necessarily the same behaviors, but "good enough")
	if (g_sdlCallbackSDFuncPtr)
	{
		static uint32_t s_lastTicks;

		uint32_t currTicks = SDL_GetTicks();
		if (currTicks != s_lastTicks)
		{
			uint8_t buff[2*NUM_OF_SAMPLES_WITH_DISABLED_SUBSYSTEM];
			uint32_t bytesPassed = currTicks-s_lastTicks; // Using a sample rate of 500Hz here, or equivalenty the byte rate of 1000Hz
			bytesPassed &= ~1; // Ensure value is even (16 bits per sample)
			BEL_ST_CallBack(NULL, buff, (bytesPassed <= sizeof(buff)) ? bytesPassed : sizeof(buff));
			s_lastTicks = currTicks;
		}
	}
}

bool BE_ST_IsEmulatedOPLChipReady(void)
{
	return g_sdlEmulatedOPLChipReady;
}

// Frequency is about 1193182Hz/spkVal
void BE_ST_PCSpeakerOn(uint16_t spkVal)
{
	g_sdlPCSpeakerOn = true;
	g_sdlCurrentBeepSample = 0;
	g_sdlBeepHalfCycleCounter = 0;
	g_sdlBeepHalfCycleCounterUpperBound = g_sdlAudioSpec.freq * spkVal;
}

void BE_ST_PCSpeakerOff(void)
{
	g_sdlPCSpeakerOn = false;
}

void BE_ST_BSound(uint16_t frequency)
{
	BE_ST_LockAudioRecursively();
	BE_ST_PCSpeakerOn(PC_PIT_RATE/(uint32_t)frequency);
	BE_ST_UnlockAudioRecursively();
}

void BE_ST_BNoSound(void)
{
	BE_ST_LockAudioRecursively();
	BE_ST_PCSpeakerOff();
	BE_ST_UnlockAudioRecursively();
}

/*******************************************************************************
OPL emulation, powered by dbopl from DOSBox and using bits of code from Wolf4SDL
*******************************************************************************/

Chip oplChip;

static inline bool YM3812Init(int numChips, int clock, int rate)
{
	DBOPL_InitTables();
	Chip__Chip(&oplChip);
	Chip__Setup(&oplChip, rate);
	return false;
}

static inline void YM3812Write(Chip *which, Bit32u reg, Bit8u val)
{
	Chip__WriteReg(which, reg, val);
}

static inline void YM3812UpdateOne(Chip *which, int16_t *stream, int length)
{
	Bit32s buffer[512 * 2];
	int i;

	// length is at maximum samplesPerMusicTick = param_samplerate / 700
	// so 512 is sufficient for a sample rate of 358.4 kHz (default 44.1 kHz)
	if(length > 512)
		length = 512;
#if 0
	if(which->opl3Active)
	{
		Chip__GenerateBlock3(which, length, buffer);

		// GenerateBlock3 generates a number of "length" 32-bit stereo samples
		// so we need to convert them to 16-bit mono samples
		for(i = 0; i < length; i++)
		{
			// Scale volume and pick one channel
			Bit32s sample = 2*buffer[2*i];
			if(sample > 16383) sample = 16383;
			else if(sample < -16384) sample = -16384;
			stream[i] = sample;
		}
	}
	else
#endif
	{
		Chip__GenerateBlock2(which, length, buffer);

		// GenerateBlock2 generates a number of "length" 32-bit mono samples
		// so we only need to convert them to 16-bit mono samples
		for(i = 0; i < length; i++)
		{
			// Scale volume
			Bit32s sample = 2*buffer[i];
			if(sample > 16383) sample = 16383;
			else if(sample < -16384) sample = -16384;
			stream[i] = (int16_t) sample;
		}
	}
}

// Drop-in replacement for id_sd.c:alOut
void BE_ST_ALOut(uint8_t reg,uint8_t val)
{
	BE_ST_LockAudioRecursively(); // RECURSIVE lock

	// FIXME: The original code for alOut adds 6 reads of the
	// register port after writing to it (3.3 microseconds), and
	// then 35 more reads of register port after writing to the
	// data port (23 microseconds).
	//
	// It is apparently important for a portion of the fuse
	// breakage sound at the least. For now a hack is implied.
	YM3812Write(&oplChip, reg, val);
	// Hack comes with a "magic number"
	// that appears to make it work better
	unsigned int length = OPL_SAMPLE_RATE / 10000;
	// Circular buffer, shouldn't be too long...
	if (length > OPL_NUM_OF_SAMPLES - g_sdlALOutSamplesEnd)
	{
		YM3812UpdateOne(&oplChip, &g_sdlALOutSamples[g_sdlALOutSamplesEnd], OPL_NUM_OF_SAMPLES - g_sdlALOutSamplesEnd);
		length -= (OPL_NUM_OF_SAMPLES - g_sdlALOutSamplesEnd);
		g_sdlALOutSamplesEnd = 0;
	}
	if (length)
	{
		YM3812UpdateOne(&oplChip, &g_sdlALOutSamples[g_sdlALOutSamplesEnd], length);
		g_sdlALOutSamplesEnd += length;
	}

	BE_ST_UnlockAudioRecursively(); // RECURSIVE unlock
}

/************************************************************************
PC Speaker emulation; The function mixes audio
into an EXISTING stream (of OPL sound data)
ASSUMPTION: The speaker is outputting sound (PCSpeakerUpdateOne == true).
************************************************************************/
static inline void PCSpeakerUpdateOne(int16_t *stream, int length)
{
	for (int loopVar = 0; loopVar < length; loopVar++, stream++)
	{
		*stream = (*stream + g_sdlCurrentBeepSample) / 2; // Mix
		g_sdlBeepHalfCycleCounter += 2 * PC_PIT_RATE;
		if (g_sdlBeepHalfCycleCounter >= g_sdlBeepHalfCycleCounterUpperBound)
		{
			g_sdlBeepHalfCycleCounter %= g_sdlBeepHalfCycleCounterUpperBound;
			// 32767 - too loud
			g_sdlCurrentBeepSample = 24575-g_sdlCurrentBeepSample;
		}
	}
}

#ifdef BE_ST_ENABLE_LOW_PASS_FILTERING
// Within g_sdlALOutSamplesStart
static void BEL_ST_ALDoLowPassFilter(uint32_t start, int len)
{
	uint32_t end = (start+len)%OPL_NUM_OF_SAMPLES;
	if (start <= end)
	{
		for (; start != end; ++start)
		{
			g_sdlALLastSampleForFiltering = (1.0f-g_sdlALFilteringAlpha)*g_sdlALLastSampleForFiltering + g_sdlALFilteringAlpha*(float)(g_sdlALOutSamples[start]);
			g_sdlALOutSamples[start] = g_sdlALLastSampleForFiltering;
		}
	}
	else
	{
		for (; start != OPL_NUM_OF_SAMPLES; ++start)
		{
			g_sdlALLastSampleForFiltering = (1.0f-g_sdlALFilteringAlpha)*g_sdlALLastSampleForFiltering + g_sdlALFilteringAlpha*(float)(g_sdlALOutSamples[start]);
			g_sdlALOutSamples[start] = g_sdlALLastSampleForFiltering;
		}
		// Wrap around
		for (start = 0; start != end; ++start)
		{
			g_sdlALLastSampleForFiltering = (1.0f-g_sdlALFilteringAlpha)*g_sdlALLastSampleForFiltering + g_sdlALFilteringAlpha*(float)(g_sdlALOutSamples[start]);
			g_sdlALOutSamples[start] = g_sdlALLastSampleForFiltering;
		}

	}
}
#endif

// BIG BIG FIXME: This is the VERY(?) wrong place to call the OPL emulator, etc!
static void BEL_ST_CallBack(void *unused, Uint8 *stream, int len)
{
	int16_t *currSamplePtr = (int16_t *)stream;
	uint32_t currNumOfSamples;
	bool isPartCompleted;
#if SDL_VERSION_ATLEAST(1,3,0)
	memset(stream, 0, len);
#endif
	/////////////////////////////
	BE_ST_LockAudioRecursively(); // RECURSIVE lock
	/////////////////////////////
	
	while (len)
	{
		if (!g_sdlSampleOffsetInSound)
		{
			// FUNCTION VARIABLE (We should use this and we want to kind-of separate what we have here from original code.)
			g_sdlCallbackSDFuncPtr();
		}
		// Now generate sound
		isPartCompleted = ((unsigned)len >= 2*(g_sdlSamplePerPart-g_sdlSampleOffsetInSound));
		currNumOfSamples = isPartCompleted ? (g_sdlSamplePerPart-g_sdlSampleOffsetInSound) : (len/2);
		/*** AdLib (including hack for alOut delays) ***/
		if (g_sdlEmulatedOPLChipReady)
		{
			// Relatively simple case
			if (g_sdlAudioSpec.freq == OPL_SAMPLE_RATE)
			{
				uint32_t noOfALSamplesAvailable = (g_sdlALOutSamplesEnd-g_sdlALOutSamplesStart) % OPL_NUM_OF_SAMPLES; // Circular buffer
				if (noOfALSamplesAvailable <= currNumOfSamples)
				{
					// Copy sound generated by alOut
					if (noOfALSamplesAvailable > 0)
					{
						if (g_sdlALOutSamplesStart < g_sdlALOutSamplesEnd)
						{
							memcpy(currSamplePtr, &g_sdlALOutSamples[g_sdlALOutSamplesStart], 2*noOfALSamplesAvailable);
						}
						else // Circular buffer wrapping
						{
							memcpy(currSamplePtr, &g_sdlALOutSamples[g_sdlALOutSamplesStart], 2*(OPL_NUM_OF_SAMPLES-g_sdlALOutSamplesStart));
							memcpy(currSamplePtr+(OPL_NUM_OF_SAMPLES-g_sdlALOutSamplesStart), g_sdlALOutSamples, 2*g_sdlALOutSamplesEnd);
						}
					}
					// Generate what's left
					if (currNumOfSamples-noOfALSamplesAvailable > 0)
					{
						YM3812UpdateOne(&oplChip, currSamplePtr+noOfALSamplesAvailable, currNumOfSamples-noOfALSamplesAvailable);
					}
					// Finally update these
					g_sdlALOutSamplesStart = g_sdlALOutSamplesEnd = 0;
				}
				else
				{
					// Already generated enough by alOut, to be copied
					if (g_sdlALOutSamplesStart+currNumOfSamples <= OPL_NUM_OF_SAMPLES)
					{
						memcpy(currSamplePtr, &g_sdlALOutSamples[g_sdlALOutSamplesStart], 2*currNumOfSamples);
					}
					else // Circular buffer wrapping
					{
						memcpy(currSamplePtr, &g_sdlALOutSamples[g_sdlALOutSamplesStart], 2*(OPL_NUM_OF_SAMPLES-g_sdlALOutSamplesStart));
						memcpy(currSamplePtr+(OPL_NUM_OF_SAMPLES-g_sdlALOutSamplesStart), g_sdlALOutSamples, 2*currNumOfSamples-2*(OPL_NUM_OF_SAMPLES-g_sdlALOutSamplesStart));
					}
					g_sdlALOutSamplesStart += currNumOfSamples;
					g_sdlALOutSamplesStart %= OPL_NUM_OF_SAMPLES;
				}
			}
			else // g_sdlAudioSpec.freq != OPL_SAMPLE_RATE so we should interpolate
			{
				int16_t *currCopiedALSamplePtr = currSamplePtr;
				int16_t *copiedALSamplesEnd = currSamplePtr+currNumOfSamples;
#ifdef BE_ST_ENABLE_LOW_PASS_FILTERING
				// If we need to apply a low-pass filter, first calculate how much AL samples should we process
				if (g_sdlAudioSpec.freq < OPL_SAMPLE_RATE)
				{
					int16_t *copiedALStart = currCopiedALSamplePtr;
					int alInputLen = 0;
					// Should still be good to use similar interpolation for silence;
					// But first we want to copy these table counters
					int alSampleRateConvCounterLocal = g_sdlALSampleRateConvCounter;
					int alSampleRateConvCurrIndexLocal = g_sdlALSampleRateConvCurrIndex;
					while (copiedALStart < copiedALSamplesEnd)
					{
						if (!alSampleRateConvCounterLocal)
						{
							++alInputLen;
						}
						// Take AL sample and store in output buffer
						if (alSampleRateConvCounterLocal < g_sdlALSampleRateConvTable[alSampleRateConvCurrIndexLocal])
						{
							++alSampleRateConvCounterLocal;
							++copiedALStart;
						}
						// Time to go to next AL silence sample
						if (alSampleRateConvCounterLocal >= g_sdlALSampleRateConvTable[alSampleRateConvCurrIndexLocal])
						{
							alSampleRateConvCounterLocal = 0;
							++alSampleRateConvCurrIndexLocal;
							alSampleRateConvCurrIndexLocal %= OPL_SAMPLE_RATE;
						}
					}
					BEL_ST_ALDoLowPassFilter(g_sdlALOutSamplesStart, alInputLen);
				}
#endif
				// Go over AL samples generated so far and interpolate
				while ((g_sdlALOutSamplesStart != g_sdlALOutSamplesEnd) && (currCopiedALSamplePtr < copiedALSamplesEnd))
				{
					// Take AL sample and store in output buffer
					if (g_sdlALSampleRateConvCounter < g_sdlALSampleRateConvTable[g_sdlALSampleRateConvCurrIndex])
					{
						++g_sdlALSampleRateConvCounter;
						*(currCopiedALSamplePtr++) = g_sdlALOutSamples[g_sdlALOutSamplesStart];
					}
					// Time to go to next AL sample
					if (g_sdlALSampleRateConvCounter >= g_sdlALSampleRateConvTable[g_sdlALSampleRateConvCurrIndex])
					{
						g_sdlALSampleRateConvCounter = 0;
						++g_sdlALSampleRateConvCurrIndex;
						g_sdlALSampleRateConvCurrIndex %= OPL_SAMPLE_RATE;
						++g_sdlALOutSamplesStart;
						g_sdlALOutSamplesStart %= OPL_NUM_OF_SAMPLES;
					}
				}
				// Need to generate extra AdLib silence?
				if (currCopiedALSamplePtr < copiedALSamplesEnd)
				{
					int16_t *copiedALStart = currCopiedALSamplePtr;
					int alInputLen = 0;
					// Should still be good to use similar interpolation for silence;
					// But first we want to copy these table counters
					int alSampleRateConvCounterLocal = g_sdlALSampleRateConvCounter;
					int alSampleRateConvCurrIndexLocal = g_sdlALSampleRateConvCurrIndex;
					while (copiedALStart < copiedALSamplesEnd)
					{
						if (!alSampleRateConvCounterLocal)
						{
							++alInputLen;
						}
						// Take AL sample and store in output buffer
						if (alSampleRateConvCounterLocal < g_sdlALSampleRateConvTable[alSampleRateConvCurrIndexLocal])
						{
							++alSampleRateConvCounterLocal;
							++copiedALStart;
						}
						// Time to go to next AL silence sample
						if (alSampleRateConvCounterLocal >= g_sdlALSampleRateConvTable[alSampleRateConvCurrIndexLocal])
						{
							alSampleRateConvCounterLocal = 0;
							++alSampleRateConvCurrIndexLocal;
							alSampleRateConvCurrIndexLocal %= OPL_SAMPLE_RATE;
						}
					}
					// Finally we know how many (new) samples should be generated
					// But just to prevent buffer overflow (shouldn't happen in a "proper" situation)...
					g_sdlALOutSamplesStart = 0;
					// Just in case (SHOULDN'T HAPPEN)
					alInputLen = (alInputLen > OPL_NUM_OF_SAMPLES) ? OPL_NUM_OF_SAMPLES : alInputLen;
					//
					YM3812UpdateOne(&oplChip, g_sdlALOutSamples, alInputLen);
#ifdef BE_ST_ENABLE_LOW_PASS_FILTERING
					if (g_sdlAudioSpec.freq < OPL_SAMPLE_RATE)
					{
						BEL_ST_ALDoLowPassFilter(0, alInputLen);
					}
#endif
					// Repeat the above loop, but with the global counter variables and, storing silence
					while (currCopiedALSamplePtr < copiedALSamplesEnd)
					{
						// Take AL sample and store in output buffer
						if (g_sdlALSampleRateConvCounter < g_sdlALSampleRateConvTable[g_sdlALSampleRateConvCurrIndex])
						{
							++g_sdlALSampleRateConvCounter;
							*(currCopiedALSamplePtr++) = g_sdlALOutSamples[g_sdlALOutSamplesStart];
						}
						if (g_sdlALSampleRateConvCounter >= g_sdlALSampleRateConvTable[g_sdlALSampleRateConvCurrIndex])
						// Time to go to next AL silence sample
						{
							g_sdlALSampleRateConvCounter = 0;
							++g_sdlALSampleRateConvCurrIndex;
							g_sdlALSampleRateConvCurrIndex %= OPL_SAMPLE_RATE;
							++g_sdlALOutSamplesStart;
							//No need to do this here now
							//g_sdlALOutSamplesStart %= OPL_NUM_OF_SAMPLES;
						}
					}
					// DON'T FORGET THESE!
					g_sdlALOutSamplesStart = g_sdlALOutSamplesEnd = 0;
				}
			}
		}
		// PC Speaker
		if (g_sdlPCSpeakerOn)
			PCSpeakerUpdateOne(currSamplePtr, currNumOfSamples);
		// We're done for now
		currSamplePtr += currNumOfSamples;
		g_sdlSampleOffsetInSound += currNumOfSamples;
		len -= 2*currNumOfSamples;
		// End of part?
		if (g_sdlSampleOffsetInSound >= g_sdlSamplePerPart)
		{
			g_sdlSampleOffsetInSound = 0;
		}
	}

	///////////////////////////////
	BE_ST_UnlockAudioRecursively(); // RECURSIVE unlock
	///////////////////////////////
}

// Here, the actual rate is about 1193182Hz/speed
// NOTE: isALMusicOn is irrelevant for Keen Dreams (even with its music code)
void BE_ST_SetTimer(uint16_t speed, bool isALMusicOn)
{
	g_sdlSamplePerPart = (int32_t)speed * g_sdlAudioSpec.freq / PC_PIT_RATE;
	// In the original code, the id_sd.c:SDL_t0Service callback
	// is responsible for incrementing TimeCount at a given rate
	// (~70Hz), although the rate in which the service itself is
	// 560Hz with music on and 140Hz otherwise.
	g_sdlScaledTimerDivisor = isALMusicOn ? (speed*8) : (speed*2);
}

static uint32_t g_sdlTicksOffset = 0;
void BEL_ST_UpdateHostDisplay(void);
void BEL_ST_TicksDelayWithOffset(int sdltickstowait);
void BEL_ST_TimeCountWaitByPeriod(int16_t timetowait);

uint32_t BE_ST_GetTimeCount(void)
{
	// FIXME: What happens when SDL_GetTicks() reaches the upper bound?
	// May be challenging to fix... A proper solution should
	// only work with *differences between SDL_GetTicks values*.

	// WARNING: This must have offset subtracted! (So the game "thinks" it gets the correct (but actually delayed) TimeCount value)
	uint32_t currOffsettedSdlTicks = SDL_GetTicks() - g_sdlTicksOffset;
	uint32_t ticksToAdd = (uint64_t)currOffsettedSdlTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor) - (uint64_t)g_sdlLastTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor);
	g_sdlTimeCount += ticksToAdd;
	g_sdlLastTicks = currOffsettedSdlTicks;
	return g_sdlTimeCount;
}

void BE_ST_SetTimeCount(uint32_t newcount)
{
	g_sdlTimeCount = newcount;
}

void BE_ST_TimeCountWaitForDest(uint32_t dsttimecount)
{
	BEL_ST_TimeCountWaitByPeriod((int32_t)dsttimecount-(int32_t)g_sdlTimeCount);
}

void BE_ST_TimeCountWaitFromSrc(uint32_t srctimecount, int16_t timetowait)
{
	BEL_ST_TimeCountWaitByPeriod((srctimecount+timetowait)-g_sdlTimeCount);
}

void BEL_ST_TimeCountWaitByPeriod(int16_t timetowait)
{
	if (timetowait <= 0)
	{
		return;
	}
	// COMMENTED OUT - Do NOT refresh TimeCount and g_sdlLastTicks
	//BE_ST_GetTimeCount();

	// We want to find the minimal currSdlTicks value (mod 2^32) such that...
	// ticksToWait <= (uint64_t)currSdlTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor) - (uint64_t)g_sdlLastTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor)
	// (WARNING: The divisions here are INTEGER divisions!)
	// i.e.,
	// ticksToWait + (uint64_t)g_sdlLastTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor) <= (uint64_t)currSdlTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor)
	// That is, ignoring the types and up to the division by PC_PIT_RATE being an integer:
	// currSdlTicks >= [ticksToWait + (uint64_t)g_sdlLastTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor)]*1000*g_sdlScaledTimerDivisor / PC_PIT_RATE
	// So it may seem like we can replace ">=" with "=" and give it a go.
	//
	// PROBLEM: The last division IS (or at leat should be) integer. Suppose e.g., the condition is
	// currSdlTicks >= (PC_PIT_RATE-100) / PC_PIT_RATE
	// Then we get currSdlTicks = 0, which is not what we want (the minimal integer which is at at least this ratio as a noninteger is 1.)
	//
	// SOLUTION: Add PC_PIC_RATE-1 to the numerator.

	uint32_t nextSdlTicks = ((timetowait + (uint64_t)g_sdlLastTicks * (uint64_t)PC_PIT_RATE / (1000*g_sdlScaledTimerDivisor))*1000*g_sdlScaledTimerDivisor + (PC_PIT_RATE-1)) / PC_PIT_RATE;
	// NOTE: nextSdlTicks is already adjusted in terms of offset, so we can simply reset it here
	g_sdlTicksOffset = 0;
	BEL_ST_TicksDelayWithOffset(nextSdlTicks-SDL_GetTicks());
}


void BE_ST_WaitVBL(int16_t number)
{
	// (REFKEEN) Difference from vanilla: If the input value is not
	// positive then the game practically hangs for quite long
	// (ok, more than 7 minutes). It basically feels like a true hang,
	// which isn't desired anyway. So we assume that number > 0.

	// TODO (REFKEEN) Make a difference based on HW?

	// Simulate waiting while in vertical retrace first, and then
	// waiting while NOT in vertical retrace. In practice, we jump
	// to the very beginning of the next "refresh cycle".
	// This is repeated for a total of 'length' times.

	uint32_t currSdlTicks = SDL_GetTicks();
	uint32_t nextSdlTicks = (int)number*1000000/70086 + currSdlTicks - g_sdlTicksOffset;
	// First iteration takes a bit less time again, so we want
	// the value "mod" about 1000/70.086 (VGA adapter);
	// For the sake of a bit better precision we further multiply by 1000
	nextSdlTicks -= (((uint64_t)1000*(uint64_t)nextSdlTicks) % ((uint64_t)1000000000/70086))/1000;
	g_sdlTicksOffset = 0; // Can reset this, taking g_sdlTicksOffset into account above
	BEL_ST_TicksDelayWithOffset(nextSdlTicks-currSdlTicks);
}

// Call during a busy loop of some unknown duration (e.g., waiting for key press/release)
void BE_ST_ShortSleep(void)
{
	SDL_Delay(1);
	// TODO: Make this more efficient
	BEL_ST_UpdateHostDisplay();
	BE_ST_PollEvents();
}


void BE_ST_Delay(uint16_t msec) // Replacement for delay from dos.h
{
	BEL_ST_TicksDelayWithOffset(msec);
}

void BEL_ST_TicksDelayWithOffset(int sdltickstowait)
{
	if (sdltickstowait <= (int32_t)g_sdlTicksOffset)
	{
		// Already waited for this time earlier, no need to do so now
		if (sdltickstowait > 0)
		{
			g_sdlTicksOffset -= sdltickstowait;
		}
		BE_ST_PollEvents(); // Still safer to do this
		return;
	}
	uint32_t nextSdlTicks = SDL_GetTicks() + sdltickstowait - g_sdlTicksOffset;
	BEL_ST_UpdateHostDisplay();
	BE_ST_PollEvents();
	uint32_t currSdlTicks = SDL_GetTicks();
	while ((int32_t)(currSdlTicks - nextSdlTicks) < 0)
	{
		SDL_Delay(1);
		BE_ST_PollEvents();
		currSdlTicks = SDL_GetTicks();
	} 
	g_sdlTicksOffset = (currSdlTicks - nextSdlTicks);
}
