/* Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//
//	ID Engine
//	ID_SD.c - Sound Manager
//	v1.1d1
//	By Jason Blochowiak
//

//
//	This module handles dealing with generating sound on the appropriate
//		hardware
//
//	Depends on: User Mgr (for parm checking)
//
//	Globals:
//		For User Mgr:
//			SoundSourcePresent - Sound Source thingie present?
//			SoundBlasterPresent - SoundBlaster card present?
//			AdLibPresent - AdLib card present?
//			SoundMode - What device is used for sound effects
//				(Use SM_SetSoundMode() to set)
//			MusicMode - What device is used for music
//				(Use SM_SetMusicMode() to set)
//		For Cache Mgr:
//			NeedsDigitized - load digitized sounds?
//			NeedsMusic - load music?
//

// REFKEEN - This macro is not even mentioned in the original Catacomb 3-D
// sources, so we can simply define it to be 1 for these. Furthermore, a bit
// more is actually disabled, not just empty function stubs
// disabled (closer to Apocalypse, but more is disabled).

#ifdef REFKEEN_VER_CATADVENTURES
#define USE_MUSIC	0
#else
#define USE_MUSIC	1
#endif

//#pragma hdrstop		// Wierdo thing with MUSE

//#include <dos.h>

#ifdef	_MUSE_      // Will be defined in ID_Types.h
#include "id_sd.h"
#else
#include "id_heads.h"
#endif
//#pragma	hdrstop
//#pragma	warn	-pia

#define	SDL_SoundFinished()	{SoundNumber = SoundPriority = 0;}

// Macros for AdLib stuff
#define	selreg(n)	outportb(0x388,n)
#define	writereg(n)	outportb(0x389,n)
#define	readstat()	inportb(0x388)

//	Global variables
	id0_boolean_t		SoundSourcePresent,SoundBlasterPresent,AdLibPresent,
				NeedsDigitized,NeedsMusic;
	SDMode		SoundMode;
	SMMode		MusicMode;
	// NEVER accessed directly now - done from backend via functions
	//id0_longword_t	TimeCount;
	id0_word_t		HackCount;
	SoundCommon		**SoundTable;
	//id0_word_t		*SoundTable;	// Really * seg *SoundTable, but that don't work
	id0_boolean_t		ssIsTandy;
	id0_word_t		ssPort = 2;

//	Internal variables
static	id0_boolean_t			SD_Started;
/*** (REFKEEN) Unused (used in vanilla Keen Dreams with low-level funcs) ***/
//static	id0_boolean_t			TimerDone;
//static	id0_word_t			TimerVal,TimerDelay10,TimerDelay25,TimerDelay100;
/*** (REFKEEN) Also unused now (and more is unused) ***/
//static	id0_longword_t		TimerDivisor,TimerCount;
static const id0_char_t			*ParmStrings[] =
						{
							"noal",
							id0_nil_t
						};
static	void			(*SoundUserHook)(void);
static	id0_word_t			SoundNumber,SoundPriority;
//static	void interrupt	(*t0OldService)(void);
//static	id0_word_t			t0CountTable[] = {8,8,8,8,40,40};
//static	id0_long_t			LocalTime;

//	PC Sound variables
static	id0_byte_t			pcLastSample,id0_far *pcSound;
static	id0_longword_t		pcLengthLeft;
static	id0_word_t			pcSoundLookup[255];

//	AdLib variables
static	id0_boolean_t			alNoCheck;
static	id0_byte_t			id0_far *alSound;
static	id0_word_t			alBlock;
static	id0_longword_t		alLengthLeft;
static	id0_longword_t		alTimeCount;
static	Instrument		alZeroInst;

// This table maps channel numbers to carrier and modulator op cells
static	id0_byte_t			carriers[9] =  { 3, 4, 5,11,12,13,19,20,21},
						modifiers[9] = { 0, 1, 2, 8, 9,10,16,17,18}
// This table maps percussive voice numbers to op cells
#if 0
						,
						pcarriers[5] = {19,0xff,0xff,0xff,0xff},
						pmodifiers[5] = {16,17,18,20,21}
#endif
						;

//	Sequencer variables
#if USE_MUSIC
static	id0_boolean_t			sqActive;
#endif
static	id0_word_t			alFXReg;
#if 0
static	ActiveTrack		*tracks[sqMaxTracks],
						mytracks[sqMaxTracks];
static	id0_word_t			sqMode,sqFadeStep;
#endif
#if USE_MUSIC
static	id0_word_t			id0_far *sqHack,id0_far *sqHackPtr,sqHackLen,sqHackSeqLen;
static	id0_long_t			sqHackTime;
#endif

//	Internal routines

///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetTimer0() - Sets system timer 0 to the specified speed
//
///////////////////////////////////////////////////////////////////////////
//#pragma	argsused
static void
SDL_SetTimer0(id0_word_t speed)
{
	BE_ST_SetTimer(speed, (MusicMode == smm_AdLib)); // HACK for different timing with music on
#if 0
#ifndef TPROF	// If using Borland's profiling, don't screw with the timer
	outportb(0x43,0x36);				// Change timer 0
	outportb(0x40,speed);
	outportb(0x40,speed >> 8);
	TimerDivisor = speed;
#else
	TimerDivisor = 0x10000;
#endif
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetIntsPerSec() - Uses SDL_SetTimer0() to set the number of
//		interrupts generated by system timer 0 per second
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_SetIntsPerSec(id0_word_t ints)
{
	SDL_SetTimer0(1192030 / ints);
}

/*** REFKEEN - UNUSED FUNCTIONS ***/
#if 0

///////////////////////////////////////////////////////////////////////////
//
//	SDL_TimingService() - Used by SDL_InitDelay() to determine a timing
//		value for the current system that we're running on
//
///////////////////////////////////////////////////////////////////////////
static void interrupt
SDL_TimingService(void)
{
	TimerVal = _CX;
	TimerDone++;

	outportb(0x20,0x20);				// Ack interrupt
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_InitDelay() - Sets up TimerDelay's for SDL_Delay()
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_InitDelay(void)
{
	id0_int_t		i;
	id0_word_t	timer;

	setvect(8,SDL_TimingService);		// Set to my timer 0 ISR

	SDL_SetIntsPerSec(1000);			// Time 1ms

	for (i = 0,timer = 0;i < 10;i++)	// Do timing test 10 times
	{
	asm	xor		dx,dx					// Zero DX
	asm	mov		cx,0xffff				// Put starting value in CX
	asm	mov		[TimerDone],cx			// TimerDone = false - 1
startloop:
	asm	or		[TimerDone],0
	asm	jnz		startloop				// Make sure we're at the start
loop:
	asm	test	[TimerDone],1			// See if TimerDone flag got hit
	asm	jnz		done					// Yep - drop out of the loop
	asm	loop	loop
done:

		if (0xffff - TimerVal > timer)
			timer = 0xffff - TimerVal;
	}
	timer += timer / 2;					// Use some slop
	TimerDelay10 =  timer / (1000 / 10);
	TimerDelay25 =  timer / (1000 / 25);
	TimerDelay100 = timer / (1000 / 100);

	SDL_SetTimer0(0);					// Reset timer 0

	setvect(8,t0OldService);			// Set back to old ISR
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_Delay() - Delays the specified amount of time
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_Delay(id0_word_t delay)
{
	if (!delay)
		return;

asm	mov		cx,[delay]
loop:
asm	test	[TimerDone],0	// Useless code - just for timing equivilency
asm	jnz		done
asm	loop	loop
done:;
}

#endif /* END OF UNUSED FUNCTIONS (REFKEEN) */

//
//	PC Sound code
//

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCPlaySound() - Plays the specified sound on the PC speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCPlaySound(PCSound id0_far *sound)
{
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	pcLastSample = -1;
	pcLengthLeft = sound->common.length;
	pcSound = sound->data;

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCStopSound() - Stops the current sound playing on the PC Speaker
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_PCStopSound(void)
{
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	pcSound = 0;
	BE_ST_PCSpeakerOff();
#if 0
asm	in	al,0x61		  	// Turn the speaker off
asm	and	al,0xfd			// ~2
asm	out	0x61,al
#endif

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_PCService() - Handles playing the next sample in a PC sound
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_PCService(void)
{
	id0_byte_t	s;
	id0_word_t	t;

	if (pcSound)
	{
		s = *pcSound++;
		if (s != pcLastSample)
		{
			BE_ST_LockAudioRecursively();
		//asm	pushf
		//asm	cli

			pcLastSample = s;
			if (s)					// We have a frequency!
			{
				t = pcSoundLookup[s];
				BE_ST_PCSpeakerOn(t);
#if 0
			asm	mov	bx,[t]

			asm	mov	al,0xb6			// Write to channel 2 (speaker) timer
			asm	out	43h,al
			asm	mov	al,bl
			asm	out	42h,al			// Low byte
			asm	mov	al,bh
			asm	out	42h,al			// High byte

			asm	in	al,0x61			// Turn the speaker & gate on
			asm	or	al,3
			asm	out	0x61,al
#endif
			}
			else					// Time for some silence
			{
				BE_ST_PCSpeakerOff();
#if 0
			asm	in	al,0x61		  	// Turn the speaker & gate off
			asm	and	al,0xfc			// ~3
			asm	out	0x61,al
#endif
			}

			BE_ST_UnlockAudioRecursively();
		//asm	popf
		}

		if (!(--pcLengthLeft))
		{
			SDL_PCStopSound();
			SDL_SoundFinished();
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutPC() - Turns off the pc speaker
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ShutPC(void)
{
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	pcSound = 0;

	BE_ST_PCSpeakerOff();
#if 0
asm	in	al,0x61		  	// Turn the speaker & gate off
asm	and	al,0xfc			// ~3
asm	out	0x61,al
#endif

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

// 	AdLib Code

///////////////////////////////////////////////////////////////////////////
//
//	alOut(n,b) - Puts b in AdLib card register n
//
///////////////////////////////////////////////////////////////////////////
void
alOut(id0_byte_t n,id0_byte_t b)
{
	BE_ST_ALOut(n, b);
#if 0
asm	pushf
asm	cli

asm	mov		dx,0x388
asm	mov		al,[n]
asm	out		dx,al
#if 0
	SDL_Delay(TimerDelay10);
#else
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
#endif

asm	mov		dx,0x389
asm	mov		al,[b]
asm	out		dx,al

asm	popf

#if 0
	SDL_Delay(TimerDelay25);
#else
asm	mov	dx,0x388
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx

asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
asm	in	al, dx
#endif
#endif
}

#if 0
///////////////////////////////////////////////////////////////////////////
//
//	SDL_SetInstrument() - Puts an instrument into a generator
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_SetInstrument(id0_int_t track,id0_int_t which,Instrument id0_far *inst,id0_boolean_t percussive)
{
	id0_byte_t		c,m;

	if (percussive)
	{
		c = pcarriers[which];
		m = pmodifiers[which];
	}
	else
	{
		c = carriers[which];
		m = modifiers[which];
	}

	tracks[track - 1]->inst = *inst;
	tracks[track - 1]->percussive = percussive;

	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);

	// Most percussive instruments only use one cell
	if (c != 0xff)
	{
		alOut(c + alChar,inst->cChar);
		alOut(c + alScale,inst->cScale);
		alOut(c + alAttack,inst->cAttack);
		alOut(c + alSus,inst->cSus);
		alOut(c + alWave,inst->cWave);
	}

	alOut(which + alFeedCon,inst->nConn);	// DEBUG - I think this is right
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ALStopSound() - Turns off any sound effects playing through the
//		AdLib card
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_ALStopSound(void)
{
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	alSound = 0;
	alOut(alFreqH + 0,0);

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

static void
SDL_AlSetFXInst(Instrument id0_far *inst)
{
	id0_byte_t		c,m;

	m = modifiers[0];
	c = carriers[0];
	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);
	alOut(c + alChar,inst->cChar);
	alOut(c + alScale,inst->cScale);
	alOut(c + alAttack,inst->cAttack);
	alOut(c + alSus,inst->cSus);
	alOut(c + alWave,inst->cWave);
	// DEBUG!!! - I just put this in
//	alOut(alFeedCon,inst->nConn);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ALPlaySound() - Plays the specified sound on the AdLib card
//
///////////////////////////////////////////////////////////////////////////
#ifdef	_MUSE_
void
#else
static void
#endif
SDL_ALPlaySound(AdLibSound id0_far *sound)
{
	Instrument	id0_far *inst;

	SDL_ALStopSound();

	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	alLengthLeft = sound->common.length;
	alSound = sound->data;
	alBlock = ((sound->block & 7) << 2) | 0x20;
	inst = &sound->inst;

	if (!(inst->mSus | inst->cSus))
	{
		BE_ST_UnlockAudioRecursively();
	//asm	popf
		Quit("SDL_ALPlaySound() - Bad instrument");
	}

	SDL_AlSetFXInst(inst);

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
// 	SDL_ALSoundService() - Plays the next sample out through the AdLib card
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ALSoundService(void)
{
	id0_byte_t	s;

	if (alSound)
	{
		s = *alSound++;
		if (!s)
			alOut(alFreqH + 0,0);
		else
		{
			alOut(alFreqL + 0,s);
			alOut(alFreqH + 0,alBlock);
		}

		if (!(--alLengthLeft))
		{
			alSound = 0;
			alOut(alFreqH + 0,0);
			SDL_SoundFinished();
		}
	}
}

#if 0
///////////////////////////////////////////////////////////////////////////
//
//	SDL_SelectMeasure() - sets up sequencing variables for a given track
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_SelectMeasure(ActiveTrack *track)
{
	track->seq = track->moods[track->mood];
	track->nextevent = 0;
}
#endif

#if USE_MUSIC
static void
SDL_ALService(void)
{
	id0_byte_t	a,v;
	id0_word_t	w;

	if (!sqActive)
		return;

	// REFKEEN - Looks like this the comparison is unsigned in original EXE
	while (sqHackLen && ((id0_longword_t)sqHackTime <= alTimeCount))
	{
		w = *sqHackPtr++;
		sqHackTime = alTimeCount + *sqHackPtr++;
		// REFKEEN - This is the case on Little and Big Endian altogether
		a = *((id0_byte_t *)&w);
		v = *((id0_byte_t *)&w + 1);
#if 0
	asm	mov	dx,[w]
	asm	mov	[a],dl
	asm	mov	[v],dh
#endif
		alOut(a,v);
		sqHackLen -= 4;
	}
	alTimeCount++;
	if (!sqHackLen)
	{
		sqHackPtr = (id0_word_t id0_far *)sqHack;
		sqHackLen = sqHackSeqLen;
		alTimeCount = sqHackTime = 0;
	}
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutAL() - Shuts down the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_ShutAL(void)
{
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	alOut(alEffects,0);
	alOut(alFreqH + 0,0);
	SDL_AlSetFXInst(&alZeroInst);
	alSound = 0;

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_CleanAL() - Totally shuts down the AdLib card
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_CleanAL(void)
{
	id0_int_t	i;

	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	alOut(alEffects,0);
	for (i = 1;i < 0xf5;i++)
		alOut(i,0);

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_StartAL() - Starts up the AdLib card for sound effects
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_StartAL(void)
{
	alFXReg = 0;
	alOut(alEffects,alFXReg);
	SDL_AlSetFXInst(&alZeroInst);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_DetectAdLib() - Determines if there's an AdLib (or SoundBlaster
//		emulating an AdLib) present
//
///////////////////////////////////////////////////////////////////////////
static id0_boolean_t
SDL_DetectAdLib(void)
{
	// REFKEEN - If there's no emulated OPL chip, just return false
	if (!BE_ST_IsEmulatedOPLChipReady())
	{
		return false;
	}

	//id0_byte_t	status1,status2;
	id0_int_t		i;

	alOut(4,0x60);	// Reset T1 & T2
	alOut(4,0x80);	// Reset IRQ
	//status1 = readstat();
	alOut(2,0xff);	// Set timer 1
	alOut(4,0x21);	// Start timer 1

	// We ALWAYS return true here, but maybe it's good
	// to send some commands to the emulated OPL chip

	// TODO (REFKEEN): Anyway to handle this delay (if at all)?
	//SDL_Delay(TimerDelay100);

	//status2 = readstat();
	alOut(4,0x60);
	alOut(4,0x80);

	//if (((status1 & 0xe0) == 0x00) && ((status2 & 0xe0) == 0xc0))
	{
		for (i = 1;i <= 0xf5;i++)	// Zero all the registers
			alOut(i,0);

		alOut(1,0x20);	// Set WSE=1
		alOut(8,0);		// Set CSM=0 & SEL=0

		return(true);
	}
//	else
//		return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_t0Service() - My timer 0 ISR which handles the different timings and
//		dispatches to whatever other routines are appropriate
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_t0Service(void)
{
static	id0_word_t	count = 1;

#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,4	// red
asm	out	dx,al
#endif

	HackCount++;

#if USE_MUSIC
	if (MusicMode == smm_AdLib)
	{
		SDL_ALService();
		if (!(++count & 7))
		{
			//LocalTime++;
			//TimeCount++;
			if (SoundUserHook)
				SoundUserHook();
		}
		if (!(count & 3))
		{
			switch (SoundMode)
			{
			case sdm_PC:
				SDL_PCService();
				break;
			case sdm_AdLib:
				SDL_ALSoundService();
				break;
			}
		}
	}
	else
#endif
	{
		if (!(++count & 1))
		{
			//LocalTime++;
			//TimeCount++;
			if (SoundUserHook)
				SoundUserHook();
		}
		switch (SoundMode)
		{
		case sdm_PC:
			SDL_PCService();
			break;
		case sdm_AdLib:
			SDL_ALSoundService();
			break;
		}
	}

#if 0
asm	mov	ax,[WORD PTR TimerCount]
asm	add	ax,[WORD PTR TimerDivisor]
asm	mov	[WORD PTR TimerCount],ax
asm	jnc	myack
	t0OldService();			// If we overflow a word, time to call old int handler
asm	jmp	olddone
myack:;
	outportb(0x20,0x20);	// Ack the interrupt
olddone:;
#endif

#if 0	// for debugging
asm	mov	dx,STATUS_REGISTER_1
asm	in	al,dx
asm	mov	dx,ATR_INDEX
asm	mov	al,ATR_OVERSCAN
asm	out	dx,al
asm	mov	al,3	// blue
asm	out	dx,al
asm	mov	al,0x20	// normal
asm	out	dx,al
#endif
}

////////////////////////////////////////////////////////////////////////////
//
//	SDL_ShutDevice() - turns off whatever device was being used for sound fx
//
////////////////////////////////////////////////////////////////////////////
static void
SDL_ShutDevice(void)
{
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_ShutPC();
		break;
	case sdm_AdLib:
		SDL_ShutAL();
		break;
	}
	SoundMode = sdm_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_CleanDevice() - totally shuts down all sound devices
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_CleanDevice(void)
{
	if ((SoundMode == sdm_AdLib) || (MusicMode == smm_AdLib))
		SDL_CleanAL();
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_StartDevice() - turns on whatever device is to be used for sound fx
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_StartDevice(void)
{
	switch (SoundMode)
	{
	case sdm_AdLib:
		SDL_StartAL();
		break;
	}
	SoundNumber = SoundPriority = 0;
}

static void
SDL_SetTimerSpeed(void)
{
	id0_word_t	rate;

#if USE_MUSIC
	if (MusicMode == smm_AdLib)
		rate = TickBase * 8;
	else
#endif
		rate = TickBase * 2;
	SDL_SetIntsPerSec(rate);
}

//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	SD_SetSoundMode() - Sets which sound hardware to use for sound effects
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
SD_SetSoundMode(SDMode mode)
{
	id0_boolean_t	result;
	id0_word_t	tableoffset;

	SD_StopSound();

#ifndef	_MUSE_
	switch (mode)
	{
	case sdm_Off:
		NeedsDigitized = false;
		result = true;
		// (REFKEEN) Originally tableoffset wasn't set here at all - undefined behaviors (even if offset is irrelevant)...
		tableoffset = 0;
		break;
	case sdm_PC:
		tableoffset = STARTPCSOUNDS;
		NeedsDigitized = false;
		result = true;
		break;
	case sdm_AdLib:
		if (AdLibPresent)
		{
			tableoffset = STARTADLIBSOUNDS;
			NeedsDigitized = false;
			result = true;
		}
		// (REFKEEN) Originally result was not set here to false, or anything, at all - undefined behaviors...
		else
		{
			result = false;
		}
		break;
	default:
		result = false;
		break;
	}
#endif

	if (result && (mode != SoundMode))
	{
		SDL_ShutDevice();
		SoundMode = mode;
#ifndef	_MUSE_
		SoundTable = (SoundCommon **)&audiosegs[tableoffset];
		//SoundTable = (id0_word_t *)(&audiosegs[tableoffset]);
#endif
		SDL_StartDevice();
	}

	SDL_SetTimerSpeed();

	return(result);
}

#if USE_MUSIC
///////////////////////////////////////////////////////////////////////////
//
//	SD_SetMusicMode() - sets the device to use for background music
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
SD_SetMusicMode(SMMode mode)
{
	id0_boolean_t	result;

	SD_FadeOutMusic();
	while (SD_MusicPlaying())
	{
		BE_ST_ShortSleep();
	}

	switch (mode)
	{
	case smm_Off:
		NeedsMusic = false;
		result = true;
		break;
	case smm_AdLib:
		if (AdLibPresent)
		{
			NeedsMusic = true;
			result = true;
		}
		// (REFKEEN) Originally result was not set here to false, or anything, at all - undefined behaviors...
		else
		{
			result = false;
		}
		break;
	default:
		result = false;
		break;
	}

	if (result)
		MusicMode = mode;

	SDL_SetTimerSpeed();

	return(result);
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	SD_Startup() - starts up the Sound Mgr
//		Detects all additional sound hardware and installs my ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_Startup(void)
{
	id0_int_t	i;

	if (SD_Started)
		return;

	ssIsTandy = false;
	alNoCheck = false;
#ifndef	_MUSE_
	for (i = 1;i < id0_argc;i++)
	{
		switch (US_CheckParm(id0_argv[i],ParmStrings))
		{
		case 0:						// No AdLib detection
			alNoCheck = true;
			break;
		}
	}
#endif

	SoundUserHook = 0;

	//t0OldService = getvect(8);	// Get old timer 0 ISR

	/*** (REFKEEN) UNUSED ***/
	//SDL_InitDelay();			// SDL_InitDelay() uses t0OldService

	BE_ST_StartAudioSDService(&SDL_t0Service);
	//setvect(8,SDL_t0Service);	// Set to my timer 0 ISR
	/*LocalTime = TimeCount =*/ alTimeCount = 0;

	SD_SetSoundMode(sdm_Off);
#if USE_MUSIC
	SD_SetMusicMode(smm_Off);
#endif

	if (!alNoCheck)
		AdLibPresent = SDL_DetectAdLib();

	for (i = 0;i < 255;i++)
		pcSoundLookup[i] = i * 60;

	SD_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_Default() - Sets up the default behaviour for the Sound Mgr whether
//		the config file was present or not.
//
///////////////////////////////////////////////////////////////////////////
void
SD_Default(id0_boolean_t gotit,SDMode sd,SMMode sm)
{
	id0_boolean_t	gotsd,gotsm;

	gotsd = gotsm = gotit;

	if (gotsd)	// Make sure requested sound hardware is available
	{
		switch (sd)
		{
		case sdm_AdLib:
			gotsd = AdLibPresent;
			break;
		}
	}
	if (!gotsd)
	{
		if (AdLibPresent)
			sd = sdm_AdLib;
		else
			sd = sdm_PC;
	}
	if (sd != SoundMode)
		SD_SetSoundMode(sd);


	if (gotsm)	// Make sure requested music hardware is available
	{
		switch (sm)
		{
		case sdm_AdLib:
			gotsm = AdLibPresent;
			break;
		}
	}
	if (!gotsm)
	{
		if (AdLibPresent)
			sm = smm_AdLib;
	}
#if USE_MUSIC
	if (sm != MusicMode)
		SD_SetMusicMode(sm);
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_Shutdown() - shuts down the Sound Mgr
//		Removes sound ISR and turns off whatever sound hardware was active
//
///////////////////////////////////////////////////////////////////////////
void
SD_Shutdown(void)
{
	if (!SD_Started)
		return;

	BE_ST_StopAudioSDService();

#if USE_MUSIC
	SD_MusicOff();
#endif
	SDL_ShutDevice();
	SDL_CleanDevice();

	BE_ST_LockAudioRecursively();
	//asm	pushf
	//asm	cli

	SDL_SetTimer0(0);

// Do NOT call this here - A deadlock is a possibility (via recursive lock)
//	BE_ST_StopAudioSDService(void);
//	setvect(8,t0OldService);

	BE_ST_UnlockAudioRecursively();
//	asm	popf

	SD_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_SetUserHook() - sets the routine that the Sound Mgr calls every 1/70th
//		of a second from its timer 0 ISR
//
///////////////////////////////////////////////////////////////////////////
void
SD_SetUserHook(void (* hook)(void))
{
	SoundUserHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_PlaySound() - plays the specified sound on the appropriate hardware
//
///////////////////////////////////////////////////////////////////////////
void
SD_PlaySound(soundnames sound)
{
	SoundCommon	id0_far *s;

	if ((SoundMode == sdm_Off) || (sound == -1))
		return;

	//s = MK_FP(SoundTable[sound],0);
	s = (SoundCommon id0_far *)(SoundTable[sound]);
	if (!s)
		Quit("SD_PlaySound() - Uncached sound");
	if (!s->length)
		Quit("SD_PlaySound() - Zero length sound");
	if (s->priority < SoundPriority)
		return;

	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCPlaySound((PCSound id0_far *)s);
		//SDL_PCPlaySound((void id0_far *)s);
		break;
	case sdm_AdLib:
		SDL_ALPlaySound((AdLibSound id0_far *)s);
		//SDL_ALPlaySound((void id0_far *)s);
		break;
	}

	SoundNumber = sound;
	SoundPriority = s->priority;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_SoundPlaying() - returns the sound number that's playing, or 0 if
//		no sound is playing
//
///////////////////////////////////////////////////////////////////////////
id0_word_t
SD_SoundPlaying(void)
{
	id0_boolean_t	result = false;

	switch (SoundMode)
	{
	case sdm_PC:
		result = pcSound? true : false;
		break;
	case sdm_AdLib:
		result = alSound? true : false;
		break;
	}

	if (result)
		return(SoundNumber);
	else
		return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_StopSound() - if a sound is playing, stops it
//
///////////////////////////////////////////////////////////////////////////
void
SD_StopSound(void)
{
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCStopSound();
		break;
	case sdm_AdLib:
		SDL_ALStopSound();
		break;
	}

	SDL_SoundFinished();
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_WaitSoundDone() - waits until the current sound is done playing
//
///////////////////////////////////////////////////////////////////////////
void
SD_WaitSoundDone(void)
{
	while (SD_SoundPlaying())
	{
		BE_ST_ShortSleep();
	}
}

#if USE_MUSIC // A block of music-related functions

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicOn() - turns on the sequencer
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOn(void)
{
	sqActive = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicOff() - turns off the sequencer and any playing notes
//
///////////////////////////////////////////////////////////////////////////
void
SD_MusicOff(void)
{
	id0_word_t	i;


	switch (MusicMode)
	{
	case smm_AdLib:
		alFXReg = 0;
		alOut(alEffects,0);
		for (i = 0;i < sqMaxTracks;i++)
			alOut(alFreqH + i + 1,0);
		break;
	}
	sqActive = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_StartMusic() - starts playing the music pointed to
//
///////////////////////////////////////////////////////////////////////////
void
SD_StartMusic(MusicGroup id0_far *music)
{
	SD_MusicOff();
	BE_ST_LockAudioRecursively();
//asm	pushf
//asm	cli

	if (MusicMode == smm_AdLib)
	{
		sqHackPtr = sqHack = music->values;
		sqHackSeqLen = sqHackLen = music->length;
		sqHackTime = 0;
		alTimeCount = 0;
		SD_MusicOn();
	}

	BE_ST_UnlockAudioRecursively();
//asm	popf
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_FadeOutMusic() - starts fading out the music. Call SD_MusicPlaying()
//		to see if the fadeout is complete
//
///////////////////////////////////////////////////////////////////////////
void
SD_FadeOutMusic(void)
{
	// REFKEEN - Original code does nothing (also in Keen Dreams and Catacomb 3D)...
#if 0
	switch (MusicMode)
	{
	case smm_AdLib:
		// DEBUG - quick hack to turn the music off
		SD_MusicOff();
		break;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	SD_MusicPlaying() - returns true if music is currently playing, false if
//		not
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
SD_MusicPlaying(void)
{
	// REFKEEN - Original code always returns false...
	return false;
#if 0
	id0_boolean_t	result;

	switch (MusicMode)
	{
	case smm_AdLib:
		result = false;
		// DEBUG - not written
		break;
	default:
		result = false;
	}

	return(result);
#endif
}

#endif // USE_MUSIC

// Replacements for direct accesses to TimeCount variable
// (should be instantiated here even if inline, as of C99)
id0_longword_t SD_GetTimeCount(void);
void SD_SetTimeCount(id0_longword_t newcount);
