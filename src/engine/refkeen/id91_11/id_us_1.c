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
//      ID Engine
//      ID_US_1.c - User Manager - General routines
//      v1.1d1
//      By Jason Blochowiak
//      Hacked up for Catacomb 3D
//

//
//      This module handles dealing with user input & feedback
//
//      Depends on: Input Mgr, View Mgr, some variables from the Sound, Caching,
//              and Refresh Mgrs, Memory Mgr for background save/restore
//
//      Globals:
//              ingame - Flag set by game indicating if a game is in progress
//      abortgame - Flag set if the current game should be aborted (if a load
//                      game fails)
//              loadedgame - Flag set if a game was loaded
//              abortprogram - Normally nil, this points to a terminal error message
//                      if the program needs to abort
//              restartgame - Normally set to gd_Continue, this is set to one of the
//                      difficulty levels if a new game should be started
//              PrintX, PrintY - Where the User Mgr will print (global coords)
//              WindowX,WindowY,WindowW,WindowH - The dimensions of the current
//                      window
//

#include "id_heads.h"

//#pragma hdrstop

//#pragma warn    -pia


//      Special imports
extern  id0_boolean_t         showscorebox;
#ifdef  KEEN
extern  id0_boolean_t         oldshooting;
extern  ScanCode        firescan;
#else
		ScanCode        firescan;
#endif

//      Global variables
		id0_char_t            *abortprogram;
		id0_boolean_t         NoWait,
					HighScoresDirty;
		id0_word_t            PrintX,PrintY;
		id0_word_t            WindowX,WindowY,WindowW,WindowH;

#ifdef REFKEEN_VER_CATADVENTURES
		id0_word_t	MaxX=320,MaxY=200;	// MDM (GAMERS EDGE)
#endif

//      Internal variables
#define ConfigVersion   1

// TODO (REFKEEN)
// 1. Originally ParmStrings, ParmStrings2 were not terminated...
// Any better way to emulate this? (Probably insignificant)
// 2. ParmStrings isn't always used, for now don't compile in such cases.
#ifndef REFKEEN_VER_CATADVENTURES
static const id0_char_t *ParmStrings[] = {"TEDLEVEL","NOWAIT",NULL};
#endif
static const id0_char_t *ParmStrings2[] = {"COMP","NOCOMP",NULL};
//static  const id0_char_t            *ParmStrings[] = {"TEDLEVEL","NOWAIT"},
//					*ParmStrings2[] = {"COMP","NOCOMP"};
static  id0_boolean_t         US_Started;

		id0_boolean_t         Button0,Button1,
					CursorBad;
		id0_int_t                     CursorX,CursorY;

		void            (*USL_MeasureString)(const id0_char_t id0_far *,const id0_char_t id0_far *,id0_word_t *,id0_word_t *) = VW_MeasurePropString,
					(*USL_DrawString)(const id0_char_t id0_far *,const id0_char_t id0_far *) = VWB_DrawPropString;

		id0_boolean_t         (*USL_SaveGame)(BE_FILE_T),(*USL_LoadGame)(BE_FILE_T);
		void            (*USL_ResetGame)(void);
		SaveGame        Games[MaxSaveGames];
		HighScore       Scores[MaxScores] =
					{
						{"Sir Lancelot",500,3},
						{"",0},
						{"",0},
						{"",0},
						{"",0},
						{"",0},
						{"",0},
					};

//      Internal routines

//      Public routines

#if 0 // USL_HardError IS UNUSED NOW (TODO REFKEEN: Restore?)

///////////////////////////////////////////////////////////////////////////
//
//      USL_HardError() - Handles the Abort/Retry/Fail sort of errors passed
//                      from DOS.
//
///////////////////////////////////////////////////////////////////////////
//#pragma warn    -par
//#pragma warn    -rch
id0_int_t
USL_HardError(id0_word_t errval,id0_int_t ax,id0_int_t bp,id0_int_t si)
{
#define IGNORE  0
#define RETRY   1
#define ABORT   2
extern  void    ShutdownId(void);

static  id0_char_t            buf[32];
static  WindowRec       wr;
		id0_int_t                     di;
		id0_char_t            c,*s,*t;


	di = _DI;

	if (ax < 0)
		s = "Device Error";
	else
	{
		if ((di & 0x00ff) == 0)
			s = "Drive ~ is Write Protected";
		else
			s = "Error on Drive ~";
		for (t = buf;*s;s++,t++)        // Can't use sprintf()
			if ((*t = *s) == '~')
				*t = (ax & 0x00ff) + 'A';
		*t = '\0';
		s = buf;
	}

	c = peekb(0x40,0x49);   // Get the current screen mode
	if ((c < 4) || (c == 7))
		goto oh_kill_me;

	// DEBUG - handle screen cleanup

	US_SaveWindow(&wr);
	US_CenterWindow(30,3);
	US_CPrint(s);
	US_CPrint("(R)etry or (A)bort?");
	VW_UpdateScreen();
	IN_ClearKeysDown();

asm     sti     // Let the keyboard interrupts come through

	while (true)
	{
		switch (IN_WaitForASCII())
		{
		case key_Escape:
		case 'a':
		case 'A':
			goto oh_kill_me;
			break;
		case key_Return:
		case key_Space:
		case 'r':
		case 'R':
			US_ClearWindow();
			VW_UpdateScreen();
			US_RestoreWindow(&wr);
			return(RETRY);
			break;
		}
		BE_ST_ShortSleep();
	}

oh_kill_me:
	abortprogram = s;
	ShutdownId();
	fprintf(stderr,"Terminal Error: %s\n",s);
	if (tedlevel)
		fprintf(stderr,"You launched from TED. I suggest that you reboot...\n");

	return(ABORT);
#undef  IGNORE
#undef  RETRY
#undef  ABORT
}
//#pragma warn    +par
//#pragma warn    +rch

#endif // USL_HardError IS UNUSED NOW

///////////////////////////////////////////////////////////////////////////
//
//      USL_GiveSaveName() - Returns a pointer to a static buffer that contains
//              the filename to use for the specified save game
//
///////////////////////////////////////////////////////////////////////////
id0_char_t *
USL_GiveSaveName(id0_word_t game)
{
static  id0_char_t    name[] = "SAVEGAMx."EXTENSION;

	name[7] = '0' + game;
	return(name);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_SetLoadSaveHooks() - Sets the routines that the User Mgr calls after
//              reading or writing the save game headers
//
///////////////////////////////////////////////////////////////////////////
void
US_SetLoadSaveHooks(id0_boolean_t (*load)(BE_FILE_T),id0_boolean_t (*save)(BE_FILE_T),void (*reset)(void))
{
	USL_LoadGame = load;
	USL_SaveGame = save;
	USL_ResetGame = reset;
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ReadConfig() - Reads the configuration file, if present, and sets
//              things up accordingly. If it's not present, uses defaults. This file
//              includes the high scores.
//
///////////////////////////////////////////////////////////////////////////
static void
USL_ReadConfig(void)
{
	id0_boolean_t         gotit;
	id0_char_t            sig[sizeof(EXTENSION)];
	id0_word_t            version;
	BE_FILE_T                     file;
	SDMode          sd;
	SMMode          sm;
	ControlType     ctl;

	if (BE_Cross_IsFileValid(file = BE_Cross_open_for_reading("CONFIG."EXTENSION)))
	//if ((file = open("CONFIG."EXTENSION,O_BINARY | O_RDONLY)) != -1)
	{
		// REFKEEN Cross Platform file I/O
		BE_Cross_readInt8LEBuffer(file, sig, sizeof(sig));
		BE_Cross_readInt16LE(file, &version);
		if (strcmp(sig,EXTENSION) || (version != ConfigVersion))
		{
			BE_Cross_close(file);
			goto rcfailed;
		}
		for (int i = 0; i < MaxScores; ++i)
		{
			BE_Cross_readInt8LEBuffer(file, Scores[i].name, sizeof(Scores[i].name));
			BE_Cross_readInt32LE(file, &Scores[i].score);
			BE_Cross_readInt16LE(file, &Scores[i].completed);
		}
		size_t BE_Cross_read_SDMode_From16LE(BE_FILE_T handle, SDMode *ptr);
		BE_Cross_read_SDMode_From16LE(file, &sd);
		size_t BE_Cross_read_SMMode_From16LE(BE_FILE_T handle, SMMode *ptr);
		BE_Cross_read_SMMode_From16LE(file, &sm);
		size_t BE_Cross_read_ControlType_From16LE(BE_FILE_T handle, ControlType *ptr);
		BE_Cross_read_ControlType_From16LE(file, &ctl);
		// KeyboardDef is a ScanCode array, and ScanCode is simply typedef-ed to be a byte
		BE_Cross_readInt8LEBuffer(file, &(KbdDefs[0]),sizeof(KbdDefs[0]));
		BE_Cross_read_boolean_From16LE(file, &showscorebox);
		BE_Cross_read_boolean_From16LE(file, &compatability);
#ifdef KEEN
		BE_Cross_read_boolean_From16LE(file, &oldshooting);
		BE_Cross_readInt8LE(file, &firescan);
#endif
#if 0
		read(file,sig,sizeof(EXTENSION));
		read(file,&version,sizeof(version));
		if (strcmp(sig,EXTENSION) || (version != ConfigVersion))
		{
			close(file);
			goto rcfailed;
		}
		read(file,Scores,sizeof(HighScore) * MaxScores);
		read(file,&sd,sizeof(sd));
		read(file,&sm,sizeof(sm));
		read(file,&ctl,sizeof(ctl));
		read(file,&(KbdDefs[0]),sizeof(KbdDefs[0]));
		read(file,&showscorebox,sizeof(showscorebox));
		read(file,&compatability,sizeof(compatability));
#ifdef KEEN
		read(file,&oldshooting,sizeof(oldshooting));
		read(file,&firescan,sizeof(firescan));
#endif
#endif
		BE_Cross_close(file);

		HighScoresDirty = false;
		gotit = true;
	}
	else
	{
rcfailed:
		sd = sdm_Off;
		sm = smm_Off;
		ctl = ctrl_Keyboard;
		showscorebox = true;
#ifdef KEEN
		oldshooting = false;
#endif

		gotit = false;
		HighScoresDirty = true;
	}

	SD_Default(gotit,sd,sm);
	IN_Default(gotit,ctl);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_WriteConfig() - Writes out the current configuration, including the
//              high scores.
//
///////////////////////////////////////////////////////////////////////////
static void
USL_WriteConfig(void)
{
	id0_word_t    version;
	BE_FILE_T             file;

	version = ConfigVersion;
	file = BE_Cross_open_for_overwriting("CONFIG."EXTENSION);
	//file = open("CONFIG."EXTENSION,O_CREAT | O_BINARY | O_WRONLY,
	//			S_IREAD | S_IWRITE | S_IFREG);
	if (BE_Cross_IsFileValid(file))
	{
		BE_Cross_writeInt8LEBuffer(file, EXTENSION, sizeof(EXTENSION));
		BE_Cross_writeInt16LE(file, &version);
		// REFKEEN Cross Platform file I/O
		for (int i = 0; i < MaxScores; ++i)
		{
			BE_Cross_writeInt8LEBuffer(file, Scores[i].name, sizeof(Scores[i].name));
			BE_Cross_writeInt32LE(file, &Scores[i].score);
			BE_Cross_writeInt16LE(file, &Scores[i].completed);
		}
		size_t BE_Cross_write_SDMode_To16LE(BE_FILE_T handle, const SDMode *ptr);
		BE_Cross_write_SDMode_To16LE(file, &SoundMode);
		size_t BE_Cross_write_SMMode_To16LE(BE_FILE_T handle, const SMMode *ptr);
		BE_Cross_write_SMMode_To16LE(file, &MusicMode);
		if      // Hack
		(
			(Controls[0] == ctrl_Joystick1)
		||      (Controls[0] == ctrl_Joystick2)
		)
			Controls[0] = ctrl_Keyboard;
		size_t BE_Cross_write_ControlType_To16LE(BE_FILE_T handle, const ControlType *ptr);
		BE_Cross_write_ControlType_To16LE(file, &(Controls[0]));
		BE_Cross_writeInt8LEBuffer(file, &(KbdDefs[0]),sizeof(KbdDefs[0]));
		BE_Cross_write_boolean_To16LE(file, &showscorebox);
		BE_Cross_write_boolean_To16LE(file, &compatability);
#ifdef KEEN
		BE_Cross_write_boolean_To16LE(file, &oldshooting);
		BE_Cross_writeInt8LE(file, &firescan);
#endif
#if 0
		write(file,EXTENSION,sizeof(EXTENSION));
		write(file,&version,sizeof(version));
		write(file,Scores,sizeof(HighScore) * MaxScores);
		write(file,&SoundMode,sizeof(SoundMode));
		write(file,&MusicMode,sizeof(MusicMode));
		if      // Hack
		(
			(Controls[0] == ctrl_Joystick1)
		||      (Controls[0] == ctrl_Joystick2)
		)
			Controls[0] = ctrl_Keyboard;
		write(file,&(Controls[0]),sizeof(Controls[0]));
		write(file,&(KbdDefs[0]),sizeof(KbdDefs[0]));
		write(file,&showscorebox,sizeof(showscorebox));
		write(file,&compatability,sizeof(compatability));
#ifdef KEEN
		write(file,&oldshooting,sizeof(oldshooting));
		write(file,&firescan,sizeof(firescan));
#endif
#endif
		BE_Cross_close(file);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_CheckSavedGames() - Checks to see which saved games are present
//              & valid
//
///////////////////////////////////////////////////////////////////////////
static void
USL_CheckSavedGames(void)
{
	id0_boolean_t         ok;
	id0_char_t            *filename;
	id0_word_t            i;
	BE_FILE_T                     file;
	SaveGame        *game;

	USL_SaveGame = 0;
	USL_LoadGame = 0;

	for (i = 0,game = Games;i < MaxSaveGames;i++,game++)
	{
		filename = USL_GiveSaveName(i);
		ok = false;
		if (BE_Cross_IsFileValid(file = BE_Cross_open_for_reading(filename)))
		//if ((file = open(filename,O_BINARY | O_RDONLY)) != -1)
		{
			// REFKEEN Cross Platform file I/O
			id0_byte_t padding; // Apparently one byte of struct padding
			if
			(
				(BE_Cross_readInt8LEBuffer(file, game->signature, sizeof(game->signature)) == sizeof(game->signature))
			&&	(BE_Cross_readInt16LE(file, &game->oldtestoffset) == 2)
			&&	(BE_Cross_read_boolean_From16LE(file, &(game->present)) == 2)
			&&	(BE_Cross_readInt8LEBuffer(file, game->name, sizeof(game->name)) == sizeof(game->name))
			&&	(BE_Cross_readInt8LE(file, &padding) == 1)

				//(read(file,game,sizeof(*game)) == sizeof(*game))
			&&      (!strcmp(game->signature,EXTENSION))
			&&	(game->oldtestoffset == refkeen_compat_id_us_printx_offset)
			//&&      (game->oldtest == &PrintX)
			)
				ok = true;

			BE_Cross_close(file);
		}

		if (ok)
			game->present = true;
		else
		{
			strcpy(game->signature,EXTENSION);
			game->present = false;
			strcpy(game->name,"Empty");
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Startup() - Starts the User Mgr
//
///////////////////////////////////////////////////////////////////////////
void
US_Startup(void)
{
	id0_int_t     i;

	if (US_Started)
		return;

	// (REFKEEN) UNUSED (TODO restore?)
	// harderr(USL_HardError); // Install the fatal error handler

	US_InitRndT(true);              // Initialize the random number generator

	USL_ReadConfig();               // Read config file

	for (i = 1;i < id0_argc;i++)
	{
		switch (US_CheckParm(id0_argv[i],ParmStrings2))
		{
		case 0:
			if (grmode == EGAGR)
				compatability = true;
			break;
		case 1:
			compatability = false;
			break;
		}
	}

	US_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Setup() - Does the disk access part of the User Mgr's startup
//
///////////////////////////////////////////////////////////////////////////
void
US_Setup(void)
{
	USL_CheckSavedGames();  // Check which saved games are present
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Shutdown() - Shuts down the User Mgr
//
///////////////////////////////////////////////////////////////////////////
void
US_Shutdown(void)
{
	if (!US_Started)
		return;

	if (!abortprogram)
		USL_WriteConfig();

	US_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CheckParm() - checks to see if a string matches one of a set of
//              strings. The check is case insensitive. The routine returns the
//              index of the string that matched, or -1 if no matches were found
//
///////////////////////////////////////////////////////////////////////////
id0_int_t
US_CheckParm(const id0_char_t *parm,const id0_char_t **strings)
{
	id0_char_t    cp,cs;
	const id0_char_t *p,*s;
	id0_int_t             i;

	while (!isalpha(*parm)) // Skip non-alphas
		parm++;

	for (i = 0;*strings && **strings;i++)
	{
		for (s = *strings++,p = parm,cs = cp = 0;cs == cp;)
		{
			cs = *s++;
			if (!cs)
				return(i);
			cp = *p++;

			if (BE_Cross_isupper(cs))
				cs = BE_Cross_tolower(cs);
			if (BE_Cross_isupper(cp))
				cp = BE_Cross_tolower(cp);
		}
	}
	return(-1);
}

// REFKEEN - Disable for The Catacomb 3-D Adventures, but not for Catacomb 3-D
#ifndef REFKEEN_VER_CATADVENTURES

///////////////////////////////////////////////////////////////////////////
//
//      USL_ScreenDraw() - Draws a chunk of the text screen (called only by
//              US_TextScreen())
//
///////////////////////////////////////////////////////////////////////////
static void
USL_ScreenDraw(id0_word_t x,id0_word_t y,const id0_char_t *s,id0_byte_t attr)
{
	id0_byte_t    id0_far *screen,id0_far *oscreen;

	screen = BE_ST_GetTextModeMemoryPtr() + (x * 2) + (y * 80 * 2);
	//screen = MK_FP(0xb800,(x * 2) + (y * 80 * 2));
	oscreen = (/*&*/(id0_byte_t *)introscn + 7) + ((x - 1) * 2) + (y * 80 * 2) + 1;
	while (*s)
	{
		*screen++ = *s++;
		if (attr != 0xff)
		{
			*screen++ = (attr & 0x8f) | (*oscreen & 0x70);
			oscreen += 2;
		}
		else
			screen++;
	}
	BE_ST_MarkGfxForUpdate();
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ClearTextScreen() - Makes sure the screen is in text mode, clears it,
//              and moves the cursor to the leftmost column of the bottom line
//
///////////////////////////////////////////////////////////////////////////
static void
USL_ClearTextScreen(void)
{
	// Set to 80x25 color text mode
	BE_ST_SetScreenMode(3); // Mode 3
	// Move the cursor to the bottom of the screen
	BE_ST_MoveTextCursorTo(0/*Lefthand side of the screen*/, 24/*Bottom row*/);
#if 0
	// Set to 80x25 color text mode
	_AL = 3;                                // Mode 3
	_AH = 0x00;
	geninterrupt(0x10);

	// Use BIOS to move the cursor to the bottom of the screen
	_AH = 0x0f;
	geninterrupt(0x10);             // Get current video mode into _BH
	_DL = 0;                                // Lefthand side of the screen
	_DH = 24;                               // Bottom row
	_AH = 0x02;
	geninterrupt(0x10);
#endif
}

///////////////////////////////////////////////////////////////////////////
//
//      US_TextScreen() - Puts up the startup text screen
//      Note: These are the only User Manager functions that can be safely called
//              before the User Mgr has been started up
//
///////////////////////////////////////////////////////////////////////////
void
US_TextScreen(void)
{
	id0_word_t    i,n;

	USL_ClearTextScreen();

	memcpy(BE_ST_GetTextModeMemoryPtr(), 7 + introscn, 80 * 25 * 2);
	BE_ST_MarkGfxForUpdate();
	//_fmemcpy(MK_FP(0xb800,0),7 + &introscn,80 * 25 * 2);

	// Check for TED launching here
	for (i = 1;i < id0_argc;i++)
	{
		n = US_CheckParm(id0_argv[i],ParmStrings);
		if (n == 0)
		{
			tedlevelnum = atoi(id0_argv[i + 1]);
			if (tedlevelnum >= 0)
			{
				tedlevel = true;
				return;
			}
			else
				break;
		}
		else if (n == 1)
		{
			NoWait = true;
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_Show() - Changes the appearance of one of the fields on the text
//              screen. Possibly adds a checkmark in front of it and highlights it
//
///////////////////////////////////////////////////////////////////////////
static void
USL_Show(id0_word_t x,id0_word_t y,id0_word_t w,id0_boolean_t show,id0_boolean_t hilight)
{
	id0_byte_t    id0_far *screen,id0_far *oscreen;

	screen = BE_ST_GetTextModeMemoryPtr() + ((x - 1) * 2) + (y * 80 * 2);
	//screen = MK_FP(0xb800,((x - 1) * 2) + (y * 80 * 2));
	oscreen = (/*&*/(id0_byte_t *)introscn + 7) + ((x - 1) * 2) + (y * 80 * 2) - 1;
	*screen++ = show? 251 : ' ';    // Checkmark char or space
//      *screen = 0x48;
//      *screen = (*oscreen & 0xf0) | 8;
	oscreen += 2;
	if (show && hilight)
	{
		for (w++;w--;screen += 2,oscreen += 2)
			*screen = (*oscreen & 0xf0) | 0x0f;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ShowMem() - Right justifies a longword in one of the memory fields on
//              the text screen
//
///////////////////////////////////////////////////////////////////////////
static void
USL_ShowMem(id0_word_t x,id0_word_t y,id0_long_t mem)
{
	id0_char_t    buf[16];
	id0_word_t    i;

	//for (i = strlen(ltoa(mem,buf,10));i < 5;i++)
	for (i = strlen(BE_Cross_ltoa_dec(mem,buf));i < 5;i++)
		USL_ScreenDraw(x++,y," ",0xff);
	USL_ScreenDraw(x,y,buf,0xff);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_UpdateTextScreen() - Called after the ID libraries are started up.
//              Displays what hardware is present.
//
///////////////////////////////////////////////////////////////////////////
void
US_UpdateTextScreen(void)
{
	id0_boolean_t         b;
	id0_longword_t        totalmem;

	// Show video card info
	b = (grmode == CGAGR);
	USL_Show(21,7,4,(videocard >= CGAcard) && (videocard <= VGAcard),b);
	b = (grmode == EGAGR);
	USL_Show(21,8,4,(videocard >= EGAcard) && (videocard <= VGAcard),b);
	b = (grmode == VGAGR);
	USL_Show(21,9,4,videocard == VGAcard,b);
	if (compatability)
		USL_ScreenDraw(5,10,"SVGA Compatibility Mode Enabled.",0x4f);

	// Show input device info
	USL_Show(60,7,8,true,true);
	USL_Show(60,8,11,JoysPresent[0],true);
	USL_Show(60,9,11,JoysPresent[1],true);
	USL_Show(60,10,5,MousePresent,true);

	// Show sound hardware info
	USL_Show(21,14,11,true,SoundMode == sdm_PC);
	b = (SoundMode == sdm_AdLib) || (MusicMode == smm_AdLib);
	USL_Show(21,15,14,AdLibPresent,b);
	if (b && AdLibPresent)  // Hack because of two lines
	{
		id0_byte_t    id0_far *screen,id0_far *oscreen;
		id0_word_t    x,y,w;

		x = 21;
		y = 16;
		w = 14;
		screen = BE_ST_GetTextModeMemoryPtr() + (x * 2) + (y * 80 * 2) - 1;
		//screen = MK_FP(0xb800,(x * 2) + (y * 80 * 2) - 1);
		oscreen = (/*&*/(id0_byte_t *)introscn + 7) + (x * 2) + (y * 80 * 2) - 1;
		oscreen += 2;
		for (w++;w--;screen += 2,oscreen += 2)
			*screen = (*oscreen & 0xf0) | 0x0f;
	}

	// Show memory available/used
	USL_ShowMem(63,15,mminfo.mainmem / 1024);
	USL_Show(53,15,23,true,true);
	USL_ShowMem(63,16,mminfo.EMSmem / 1024);
	USL_Show(53,16,23,mminfo.EMSmem? true : false,true);
	USL_ShowMem(63,17,mminfo.XMSmem / 1024);
	USL_Show(53,17,23,mminfo.XMSmem? true : false,true);
	totalmem = mminfo.mainmem + mminfo.EMSmem + mminfo.XMSmem;
	USL_ShowMem(63,18,totalmem / 1024);
	USL_Show(53,18,23,true,true);   // DEBUG
	USL_ScreenDraw(52,18," ",0xff);

	// Change Initializing... to Loading...
	USL_ScreenDraw(27,22,"  Loading...   ",0x9c);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_FinishTextScreen() - After the main program has finished its initial
//              loading, this routine waits for a keypress and then clears the screen
//
///////////////////////////////////////////////////////////////////////////
void
US_FinishTextScreen(void)
{
	// REFKEEN - A few bits of code were commented out for Cat3D v1.22,
	// but this function is never called in v1.22 anyway.
static  id0_byte_t    colors[] = {4,6,13,15,15,15,15,15,15};
		id0_boolean_t up;
		id0_int_t             i,c;

	// Change Loading... to Press a Key

	if (!(tedlevel || NoWait))
	{
//#ifndef REFKEEN_VER_CAT3D_122
		IN_ClearKeysDown();
//#endif
		for (i = 0,up = true;!IN_UserInput(4,true);)
		{
			c = colors[i];
			if (up)
			{
				if (++i == 9)
					i = 8,up = false;
			}
			else
			{
				if (--i < 0)
					i = 1,up = true;
			}

//#ifndef REFKEEN_VER_CAT3D_122
			USL_ScreenDraw(29,22," Ready - Press a Key     ",0x00 + c);
//#endif
		}
	}
//#ifndef REFKEEN_VER_CAT3D_122
	else
		USL_ScreenDraw(29,22," Ready - Press a Key     ",0x9a);
	IN_ClearKeysDown();
//#endif

	USL_ClearTextScreen();
}

#endif // REFKEEN_VER_CATADVENTURES

//      Window/Printing routines

///////////////////////////////////////////////////////////////////////////
//
//      US_SetPrintRoutines() - Sets the routines used to measure and print
//              from within the User Mgr. Primarily provided to allow switching
//              between masked and non-masked fonts
//
///////////////////////////////////////////////////////////////////////////
void
US_SetPrintRoutines(void (*measure)(const id0_char_t id0_far *,const id0_char_t id0_far *,id0_word_t *,id0_word_t *),void (*print)(const id0_char_t id0_far *,const id0_char_t id0_far *))
{
	USL_MeasureString = measure;
	USL_DrawString = print;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Print() - Prints a string in the current window. Newlines are
//              supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_Print(const id0_char_t *s)
{
	id0_char_t c;
	const id0_char_t *se;
	//id0_char_t	c,*se;
	id0_word_t    w,h;

	// (REFKEEN) Modifications from vanilla Keen:
	// - Input is now const and US_Print does not temporarily modify it.
	// - Reason is the input is often a C string literal. Modification of
	// any such string leads to undefined behaviors (or at least a crash).

	while (*s)
	{
		se = s;
		while ((c = *se) && (c != '\n'))
			se++;
		//*se = '\0'; Constified

		USL_MeasureString(s,se,&w,&h); // Instead of "*se = '\0';"
		//USL_MeasureString(s,&w,&h);
		px = PrintX;
		py = PrintY;
		USL_DrawString(s,se); // Instead of "*se = '\0';"
		//USL_DrawString(s);

		s = se;
		if (c)
		{
			//*se = c; // Constified
			s++;

			PrintX = WindowX;
			PrintY += h;
		}
		else
			PrintX += w;
	}
}

#ifdef REFKEEN_VER_CATADVENTURES
// MDM - (GAMERS EDGE)	begin

///////////////////////////////////////////////////////////////////////////
//
// US_Printxy()
//
void US_Printxy(id0_word_t x, id0_word_t y, id0_char_t *text)
{
	id0_word_t orgx, orgy;

	orgx = PrintX;
	orgy = PrintY;

//	PrintX = WindowX+x;
//	PrintY = WindowY+y;
	PrintX = x;
	PrintY = y;
	US_Print(text);

	PrintX = orgx;
	PrintY = orgy;
}

// MDM - (GAMERS EDGE)	end
#endif

///////////////////////////////////////////////////////////////////////////
//
//      US_PrintUnsigned() - Prints an unsigned long
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintUnsigned(id0_longword_t n)
{
	id0_char_t    buffer[32];

	US_Print(BE_Cross_ultoa_dec(n,buffer));
	//US_Print(ultoa(n,buffer,10));
}

///////////////////////////////////////////////////////////////////////////
//
//      US_PrintSigned() - Prints a signed long
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintSigned(id0_long_t n)
{
	id0_char_t    buffer[32];

	US_Print(BE_Cross_ltoa_dec(n,buffer));
	//US_Print(ltoa(n,buffer,10));
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_PrintInCenter() - Prints a string in the center of the given rect
//
///////////////////////////////////////////////////////////////////////////
void
USL_PrintInCenter(const id0_char_t *s,Rect r)
{
	id0_word_t    w,h,
			rw,rh;

	USL_MeasureString(s,NULL,&w,&h);
	rw = r.lr.x - r.ul.x;
	rh = r.lr.y - r.ul.y;

	px = r.ul.x + ((rw - w) / 2);
	py = r.ul.y + ((rh - h) / 2);
	USL_DrawString(s,NULL);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_PrintCentered() - Prints a string centered in the current window.
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintCentered(const id0_char_t *s)
{
	Rect    r;

	r.ul.x = WindowX;
	r.ul.y = WindowY;
	r.lr.x = r.ul.x + WindowW;
	r.lr.y = r.ul.y + WindowH;

	USL_PrintInCenter(s,r);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CPrintLine() - Prints a string centered on the current line and
//              advances to the next line. Newlines are not supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_CPrintLine(const id0_char_t *s, const id0_char_t *optse)
{
	id0_word_t    w,h;

	// (REFKEEN) WORKAROUND for s=NULL: Treat strings a empty
	// (looks like the original behaviors for Catacomb Abyss,
	// c4_play.c:PlayLoop, DisplaySMsg(NULL,NULL))
	if (!s)
	{
		s = "";
	}
	// (REFKEEN) Modifications from vanilla Keen:
	// - All input strings are now const.
	// - An additional "optse" argument marking one char past end
	// of string. Set to NULL for original behaviors.
	// - Related to modifcation to US_CPrint, properly taking care of
	// C string literals as inputs.
	// - The functions pointed by USL_MeasureString and USL_DrawString are
	// similarly modified.

	USL_MeasureString(s,optse,&w,&h);
	//USL_MeasureString(s,&w,&h);

	if (w > WindowW)
#ifdef REFKEEN_VER_CATABYSS
		Quit("US_CPrintLine() - String exceeds width\n-->%s",s);
#else // Yes, this applies to Catacomb 3-D and non-Abyss adventures!
		Quit("US_CPrintLine() - String exceeds width");
#endif
	px = WindowX + ((WindowW - w) / 2);
	py = PrintY;
	USL_DrawString(s,optse);
	//USL_DrawString(s);
	PrintY += h;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CPrint() - Prints a string in the current window. Newlines are
//              supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_CPrint(const id0_char_t *s)
{
	id0_char_t	c;
	const id0_char_t	*se;
	//id0_char_t	c,*se;

	// (REFKEEN) Modifications from vanilla Keen:
	// - Input is now const and US_Print does not temporarily modify it.
	// - Reason is the input is often a C string literal. Modification of
	// any such string leads to undefined behaviors (or at least a crash).

	while (*s)
	{
		se = s;
		while ((c = *se) && (c != '\n'))
			se++;
		//*se = '\0'; // Constified

		US_CPrintLine(s,se); // Instead of "*se = '\0';"

		s = se;
		if (c)
		{
			//*se = c; // Constified
			s++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ClearWindow() - Clears the current window to white and homes the
//              cursor
//
///////////////////////////////////////////////////////////////////////////
void
US_ClearWindow(void)
{
#ifdef REFKEEN_VER_CATADVENTURES
	VWB_Bar(WindowX,WindowY,WindowW,WindowH,LT_GREY);
#else
	VWB_Bar(WindowX,WindowY,WindowW,WindowH,WHITE);
#endif
	PrintX = WindowX;
	PrintY = WindowY;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_DrawWindow() - Draws a frame and sets the current window parms
//
///////////////////////////////////////////////////////////////////////////
void
US_DrawWindow(id0_word_t x,id0_word_t y,id0_word_t w,id0_word_t h)
{
	id0_word_t    i,
			sx,sy,sw,sh;

	WindowX = x * 8;
	WindowY = y * 8;
	WindowW = w * 8;
	WindowH = h * 8;

	PrintX = WindowX;
	PrintY = WindowY;

	sx = (x - 1) * 8;
	sy = (y - 1) * 8;
	sw = (w + 1) * 8;
	sh = (h + 1) * 8;

	US_ClearWindow();

	VWB_DrawTile8M(sx,sy,0),VWB_DrawTile8M(sx,sy + sh,6);
	for (i = sx + 8;i <= sx + sw - 8;i += 8)
		VWB_DrawTile8M(i,sy,1),VWB_DrawTile8M(i,sy + sh,7);
	VWB_DrawTile8M(i,sy,2),VWB_DrawTile8M(i,sy + sh,8);

	for (i = sy + 8;i <= sy + sh - 8;i += 8)
		VWB_DrawTile8M(sx,i,3),VWB_DrawTile8M(sx + sw,i,5);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CenterWindow() - Generates a window of a given width & height in the
//              middle of the screen
//
///////////////////////////////////////////////////////////////////////////
void
US_CenterWindow(id0_word_t w,id0_word_t h)
{
	US_DrawWindow(((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CenterSaveWindow() - Generates a window of a given width & height in
//              the middle of the screen, saving the background
//
///////////////////////////////////////////////////////////////////////////
void
US_CenterSaveWindow(id0_word_t w,id0_word_t h,memptr *save)
{
	id0_word_t    x,y,
			screen;

	x = ((MaxX / 8) - w) / 2;
	y = ((MaxY / 8) - h) / 2;
	MM_GetPtr(save,(w * h) * CHARWIDTH);
	screen = bufferofs + panadjust + ylookup[y] + (x * CHARWIDTH);
	VW_ScreenToMem(screen,*save,w * CHARWIDTH,h);
	US_DrawWindow(((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_RestoreSaveWindow() - Restores the background of the size of the
//              current window from the memory specified by save
//
///////////////////////////////////////////////////////////////////////////
void
US_RestoreSaveWindow(memptr *save)
{
	id0_word_t    screen;

	screen = bufferofs + panadjust + ylookup[WindowY] + (WindowX * CHARWIDTH);
	VW_MemToScreen(*save,screen,WindowW * CHARWIDTH,WindowH);
	MM_FreePtr(save);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_SaveWindow() - Saves the current window parms into a record for
//              later restoration
//
///////////////////////////////////////////////////////////////////////////
void
US_SaveWindow(WindowRec *win)
{
	win->x = WindowX;
	win->y = WindowY;
	win->w = WindowW;
	win->h = WindowH;

	win->px = PrintX;
	win->py = PrintY;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_RestoreWindow() - Sets the current window parms to those held in the
//              record
//
///////////////////////////////////////////////////////////////////////////
void
US_RestoreWindow(WindowRec *win)
{
	WindowX = win->x;
	WindowY = win->y;
	WindowW = win->w;
	WindowH = win->h;

	PrintX = win->px;
	PrintY = win->py;
}

//      Cursor routines

#if 0
///////////////////////////////////////////////////////////////////////////
//
//      US_StartCursor() - Sets up the cursor for User Mgr use
//
///////////////////////////////////////////////////////////////////////////
void
US_StartCursor(void)
{
	CursorInfo      info;

	VW_SetCursor(CURSORARROWSPR);
	CursorX = MaxX / 2;
	CursorY = MaxY / 2;
	VW_MoveCursor(CursorX,CursorY);
	VW_ShowCursor();

	IN_ReadCursor(&info);   // Dispose of any accumulated movement
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ShutCursor() - Cleans up after US_StartCursor()
//
///////////////////////////////////////////////////////////////////////////
void
US_ShutCursor(void)
{
	VW_HideCursor();
}

///////////////////////////////////////////////////////////////////////////
//
//      US_UpdateCursor() - Gets the new cursor position & button states from
//              the Input Mgr and tells the View Mgr where the cursor is
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
US_UpdateCursor(void)
{
	CursorInfo      info;

	IN_ReadCursor(&info);
	if (info.x || info.y || CursorBad)
	{
		CursorX += info.x;
		if (CursorX >= MaxX)
			CursorX = MaxX - 1;
		else if (CursorX < 0)
			CursorX = 0;

		CursorY += info.y;
		if (CursorY >= MaxY)
			CursorY = MaxY - 1;
		else if (CursorY < 0)
			CursorY = 0;

		VW_MoveCursor(CursorX,CursorY);
		CursorBad = false;
	}
	Button0 = info.button0;
	Button1 = info.button1;
	return(Button0 || Button1);
}
#endif

//      Input routines

///////////////////////////////////////////////////////////////////////////
//
//      USL_XORICursor() - XORs the I-bar text cursor. Used by US_LineInput()
//
///////////////////////////////////////////////////////////////////////////
static void
USL_XORICursor(id0_int_t x,id0_int_t y,const id0_char_t *s,id0_word_t cursor)
{
	id0_char_t    buf[MaxString];
	id0_word_t    w,h;

	strcpy(buf,s);
	buf[cursor] = '\0';
	USL_MeasureString(buf,NULL,&w,&h);

	px = x + w - 1;
	py = y;
	USL_DrawString("\x80",NULL);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_LineInput() - Gets a line of user input at (x,y), the string defaults
//              to whatever is pointed at by def. Input is restricted to maxchars
//              chars or maxwidth pixels wide. If the user hits escape (and escok is
//              true), nothing is copied into buf, and false is returned. If the
//              user hits return, the current string is copied into buf, and true is
//              returned
//
///////////////////////////////////////////////////////////////////////////
id0_boolean_t
US_LineInput(id0_int_t x,id0_int_t y,id0_char_t *buf,const id0_char_t *def,id0_boolean_t escok,
				id0_int_t maxchars,id0_int_t maxwidth)
{
	id0_boolean_t         redraw,
				cursorvis,cursormoved,
				done,result;
	ScanCode        sc;
	id0_char_t            c,
				s[MaxString],olds[MaxString];
	id0_word_t            i,
				cursor,
				w,h,
				len;
	id0_longword_t        lasttime;

	VW_HideCursor();

	if (def)
		strcpy(s,def);
	else
		*s = '\0';
	*olds = '\0';
	cursor = strlen(s);
	cursormoved = redraw = true;

	cursorvis = done = false;
	lasttime = SD_GetTimeCount();
	LastASCII = key_None;
	LastScan = sc_None;

	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareTextInput();

	while (!done)
	{
		if (cursorvis)
			USL_XORICursor(x,y,s,cursor);

	//asm     pushf
	//asm     cli
		// For buggy blinking cursor in Catacomb Abyss, should be called here and later
		BE_ST_ShortSleep();

		sc = LastScan;
		LastScan = sc_None;
		c = LastASCII;
		LastASCII = key_None;

	//asm     popf

		switch (sc)
		{
		case sc_LeftArrow:
			if (cursor)
				cursor--;
			c = key_None;
			cursormoved = true;
			break;
		case sc_RightArrow:
			if (s[cursor])
				cursor++;
			c = key_None;
			cursormoved = true;
			break;
		case sc_Home:
			cursor = 0;
			c = key_None;
			cursormoved = true;
			break;
		case sc_End:
			cursor = strlen(s);
			c = key_None;
			cursormoved = true;
			break;

		case sc_Return:
			strcpy(buf,s);
			done = true;
			result = true;
			c = key_None;
			break;
		case sc_Escape:
			if (escok)
			{
				done = true;
				result = false;
			}
			c = key_None;
			break;

		case sc_BackSpace:
			if (cursor)
			{
				strcpy(s + cursor - 1,s + cursor);
				cursor--;
				redraw = true;
			}
			c = key_None;
			cursormoved = true;
			break;
		case sc_Delete:
			if (s[cursor])
			{
				strcpy(s + cursor,s + cursor + 1);
				redraw = true;
			}
			c = key_None;
			cursormoved = true;
			break;

		case 0x4c:      // Keypad 5
		case sc_UpArrow:
		case sc_DownArrow:
		case sc_PgUp:
		case sc_PgDn:
		case sc_Insert:
			c = key_None;
			break;
		}

		if (c)
		{
			len = strlen(s);
			USL_MeasureString(s,NULL,&w,&h);

			if
			(
				isprint(c)
			&&      (len < MaxString - 1)
			&&      ((!maxchars) || (len < maxchars))
			&&      ((!maxwidth) || (w < maxwidth))
			)
			{
				for (i = len + 1;i > cursor;i--)
					s[i] = s[i - 1];
				s[cursor++] = c;
				redraw = true;
			}
		}

		if (redraw)
		{
			px = x;
			py = y;
			USL_DrawString(olds,NULL);
			strcpy(olds,s);

			px = x;
			py = y;
			USL_DrawString(s,NULL);

			redraw = false;
		}

		if (cursormoved)
		{
			cursorvis = false;
			lasttime = SD_GetTimeCount() - TickBase;

			cursormoved = false;
		}
		if (SD_GetTimeCount() - lasttime > TickBase / 2)
		{
			lasttime = SD_GetTimeCount();

			cursorvis ^= true;
		}
		if (cursorvis)
			USL_XORICursor(x,y,s,cursor);

		VW_UpdateScreen();

		// For buggy blinking cursor in Catacomb Abyss, should be called here and earlier
		BE_ST_ShortSleep();
	}

	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Pop();

	if (cursorvis)
		USL_XORICursor(x,y,s,cursor);
	if (!result)
	{
		px = x;
		py = y;
		USL_DrawString(olds,NULL);
	}
	VW_ShowCursor();
	VW_UpdateScreen();

	IN_ClearKeysDown();
	return(result);
}

// (REFKEEN) Used for patching version-specific stuff
id0_char_t *introscn;
id0_word_t refkeen_compat_id_us_printx_offset;

void RefKeen_Patch_id_us(void)
{
#ifdef REFKEEN_VER_CAT3D
	// Just in case this may ever be reloaded
	BE_Cross_free_mem_loaded_embedded_rsrc(introscn);
	// Don't use CA_LoadFile for (sort-of) compatibility; It also doesn't work!
	if (BE_Cross_load_embedded_rsrc_to_mem("INTROSCN.SCN", (memptr *)&introscn) < 0)
		// Similarly we don't use Quit
		BE_ST_ExitWithErrorMsg("RefKeen_Patch_id_us - Failed to load INTROSCN.SCN.");
#endif

	switch (refkeen_current_gamever)
	{
#ifdef REFKEEN_VER_CAT3D
	case BE_GAMEVER_CAT3D100:
		refkeen_compat_id_us_printx_offset = 0xA24E;
		break;
	case BE_GAMEVER_CAT3D122:
		refkeen_compat_id_us_printx_offset = 0xA50C;
		break;
#endif
#ifdef REFKEEN_VER_CATABYSS
	case BE_GAMEVER_CATABYSS113:
		refkeen_compat_id_us_printx_offset = 0xEB23;
		break;
	case BE_GAMEVER_CATABYSS124:
		refkeen_compat_id_us_printx_offset = 0xEAE1;
		break;
#endif
#ifdef REFKEEN_VER_CATARM
	case BE_GAMEVER_CATARM102:
		refkeen_compat_id_us_printx_offset = 0x5B14;
		break;
#endif
#ifdef REFKEEN_VER_CATAPOC
	case BE_GAMEVER_CATAPOC101:
		refkeen_compat_id_us_printx_offset = 0x5639;
		break;
#endif
	}
}
