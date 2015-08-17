/* Catacomb Abyss Source Code
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

//#include <dos.h>
//#include <conio.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
//#include <dir.h>
//#include "mem.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>
//#include "io.h"

#include "def.h"
#include "gelib.h"

#define MAX_GAMELIST_NAMES 20
#define FNAME_LEN				9

////////////////////////////////////////////////////////////////////////////
//
// Global variables
//
id0_boolean_t InLoadSaveGame = false;
//AudioDeviceType ge_DigiMode;
id0_boolean_t ConserveMemory = false;
id0_char_t GameListNames[MAX_GAMELIST_NAMES+1][FNAME_LEN],current_disk=1;
id0_short_t NumGames = 0; // REFKEEN - Set to 0 here rather than in c*_main.c
id0_short_t PPT_LeftEdge=0,PPT_RightEdge=320;
id0_boolean_t LeaveDriveOn=false,ge_textmode=true;
// REFKEEN - GAMENAME depends on the version now so put some good max.,
// which is checked in RefKeen_Patch_gelib
id0_char_t Filename[FILENAME_LEN+1], ID[19/*sizeof(GAMENAME)*/], VER[sizeof(SAVEVER_DATA)];


////////////////////////////////////////////////////////////////////////////
//
// CalibrateJoystick()
//
void CalibrateJoystick(id0_short_t joynum)
{
	id0_word_t	minx,maxx,
			miny,maxy;

	IN_ClearKeysDown();

	VW_HideCursor();

	VW_FixRefreshBuffer();
	CenterWindow(30,8);

	US_Print("\n");
	US_CPrintLine("Move joystick to the upper-left", NULL);
	US_CPrintLine("and press one of the buttons.", NULL);
	VW_UpdateScreen();

	while ((LastScan != sc_Escape) && !IN_GetJoyButtonsDB(joynum))
	{
		BE_ST_ShortSleep();
	}
	if (LastScan == sc_Escape)
		return;

	IN_GetJoyAbs(joynum,&minx,&miny);
	while (IN_GetJoyButtonsDB(joynum))
	{
		BE_ST_ShortSleep();
	}

	US_Print("\n");
	US_CPrintLine("Move joystick to the lower-right", NULL);
	US_CPrintLine("and press one of the buttons.", NULL);
	VW_UpdateScreen();

	while ((LastScan != sc_Escape) && !IN_GetJoyButtonsDB(joynum))
	{
		BE_ST_ShortSleep();
	}
	if (LastScan == sc_Escape)
		return;

	IN_GetJoyAbs(joynum,&maxx,&maxy);
	if ((minx == maxx) && (miny == maxy))
		return;

	IN_SetupJoy(joynum,minx,maxx,miny,maxy);

	while (IN_GetJoyButtonsDB(joynum))
	{
		BE_ST_ShortSleep();
	}
	if (LastScan)
		IN_ClearKeysDown();

	JoystickCalibrated = true;
}

// (REFKEEN) UNUSED FUNCTION
#if 0
////////////////////////////////////////////////////////////////////////////
//
// WaitKeyVBL()
//
void WaitKeyVBL(id0_short_t key, id0_short_t vbls)
{
	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){key, 0});

	while (vbls--)
	{
		VW_WaitVBL(1);
		IN_ReadControl(0,&control);
		if ((control.button0|control.button1)||(Keyboard[key]))
			break;
	}
	// REFKEEN - Alternative controllers support
	BE_ST_AltControlScheme_Pop();
}
#endif

////////////////////////////////////////////////////////////////////////////
//
// MoveScreen()
//
// panadjust must be saved and restored if MoveScreen is being called from
// inside a level.
//
void MoveScreen(id0_short_t x, id0_short_t y)
{
	id0_unsigned_t address;

	address = (y*linewidth)+(x/8);
	VW_SetScreen(address,0);
	bufferofs = displayofs = address;
	panadjust=0;
}

////////////////////////////////////////////////////////////////////////////
//
// MoveGfxDst()
//
void MoveGfxDst(id0_short_t x, id0_short_t y)
{
	id0_unsigned_t address;

	address = (y*linewidth)+(x/8);
	bufferofs = displayofs = address;
}

#if 0

#if GRAPHIC_PIRATE

///////////////////////////////////////////////////////////////////////////
//
// DoPiracy() - Graphics piracy code...
//
void DoPiracy()
{
	struct Shape Pirate1Shp;
	struct Shape Pirate2Shp;

	VW_SetScreenMode (EGA320GR);
	VW_ClearVideo(BLACK);

	// Load shapes...
	//
	if (LoadShape("PIRATE1E."EXT,&Pirate1Shp))
		TrashProg("Can't load PIRATE1E.BIO");

	if (LoadShape("PIRATE2E."EXT,&Pirate2Shp))
		TrashProg("Can't load PIRATE2E.BIO");

	// Deal with shapes...
	//
	VW_SetLineWidth(40);

	VW_FadeOut();

	MoveScreen(0,0);
	UnpackEGAShapeToScreen(&Pirate1Shp,(linewidth-Pirate1Shp.BPR)<<2,0);

	MoveScreen(0,200);
	UnpackEGAShapeToScreen(&Pirate2Shp,(linewidth-Pirate2Shp.BPR)<<2,0);

	MoveScreen(0,0);
	VW_FadeIn();
	WaitKeyVBL(57,200);
	while (Keyboard[57])
	{
		BE_ST_ShortSleep();
	}


	SD_PlaySound(GOOD_PICKSND);

	MoveScreen(0,200);
	WaitKeyVBL(57,300);
	while (Keyboard[57])
	{
		BE_ST_ShortSleep();
	}

	VW_FadeOut();

	FreeShape(&Pirate1Shp);
	FreeShape(&Pirate2Shp);
}

#else

///////////////////////////////////////////////////////////////////////////
//
// DoPiracy() - Text-based piracy code...
//
void DoPiracy()
{
}

#endif
#endif


//--------------------------------------------------------------------------
// PrintPropText()
//--------------------------------------------------------------------------

// THE FOLLOWING MUST BE INITIALIZED BEFORE CALLING THIS ROUTINE:
//

// WindowX, WindowW, PrintY - These are ID global variables and are
// automatically initialized when using their window routines.
//
// WindowX is the left edge of the window.
// WindowW is the width of the window.
// PrintY is the top edge of the window area.
//
// All values are represented in unshifted pixels.

// PPT_LeftEdge, PPT_RightEdge - These are globals used by PrintPropText().
// They define the left and right edge of the text area in pixels.

/* REFKEEN: As with functions like US_Print, a few modifications are required
 * here so text can point to a const char buffer. Useful e.g., in case the
 * input is a C string literal.
 */

void PrintPropText(const id0_char_t id0_far *text)
{
	#define RETURN_CHAR '\n'

	id0_char_t pb[200];

	fontstruct id0_seg *font = (fontstruct id0_seg *)grsegs[STARTFONT];
	//id0_char_t savech; // REFKEEN - No need for savech with constified text
	id0_short_t	length,maxend,maxx/*,loop*/,curx;
	id0_boolean_t centerit/*,lastcharcr*/; // REFKEEN - No need for lastcharcr  with constified text

	while (*text)
	{
		if (*text == '^')
			centerit=true,text++;
		else
			centerit=false;

		/* Search forward for the last possible character in the line. This
		** character is:  1) RETURN (end of line)  2) ZERO (end of buffer)
		** and  3) the character at "WP->vWidth" bytes from the start of the line.
		*/
		curx=PPT_LeftEdge;
		length=0;
		while ((curx+font->width[text[length+1]] < PPT_RightEdge) &&
				(text[length]) &&
				(text[length] != RETURN_CHAR))
						curx+=font->width[text[length++]];

		/* Search backward from the point we just found for a SPACE (for word
		** wrapping).
		*/
		if ((text[length]) && (text[length] != RETURN_CHAR))
		{
			maxx = curx;
			maxend = length;
			while ((length) && (text[length] != ' '))
				curx-=font->width[text[length--]];

			/* Were there any SPACES on this line? If not, take the MAX!
			*/
			if (!length)
				length=maxend,curx=maxx;
		}

		/* If we can, lets keep the SPACE or RETURN that follows a line at
		** the end of that line.
		*/
		if (((text[length] == ' ') || (text[length] == RETURN_CHAR)) && (length < PPT_RightEdge))
			length++;

		// All of this is kludged to work with ID _Print routines...
		//

		// REFKEEN - Constified text
		memcpy(pb,text,length);
		pb[length] = 0;
		if (pb[length-1] == RETURN_CHAR)
		{
			pb[length-1] = 0;
		}
#if 0
		savech=text[length];
		text[length]=0;
		if (text[length-1] == RETURN_CHAR)
		{
			lastcharcr=true;
			text[length-1]=0;
		}
		else
			lastcharcr=false;
		memcpy(pb,text,length+1);
		//_fmemcpy(pb,text,length+1);
#endif
		if (centerit)
		{
			US_CPrintLine(pb, NULL);
		}
		else
		{
			PrintX = PPT_LeftEdge;
			US_Print(pb);
			US_Print("\n");
		}
		// REFKEEN - Constified text
#if 0
		if (lastcharcr)
			text[length-1]=RETURN_CHAR;
		text[length]=savech;
#endif
		//
		// end of ID _Print kludge...

		text += length;
	}
}

////////////////////////////////////////////////////////////////////////////
//
// DisplayText()
//
void DisplayText(textinfo *textinfo)
{
	// REFKEEN - Alternative controllers support	
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PreparePageScrollingControls(sc_PgUp, sc_PgDn);

	#define PAGE_WIDTH 	78

	id0_int_t /*loop,*/ PageNum, LastNum/*,num*/;
	id0_boolean_t InHelp = true,faded_in = false;
	//id0_unsigned_t holddisp,holdpan,holdbuffer,holdaddress;

// Can you believe it takes all this just to change to 640 mode!!???!
//
	VW_ScreenToScreen(0,FREESTART-STATUSLEN,40,80);
	VW_SetLineWidth(80);
	MoveScreen(0,0);
	VW_Bar (0,0,640,200,0);
	VW_SetScreenMode(EGA640GR);
	VW_SetLineWidth(80);
	BlackPalette();

// Now, let's display some text...
//
	PPT_RightEdge=PAGE_WIDTH*8;
	PPT_LeftEdge=16;
	PrintY= 30;
	WindowX=WindowY=0;

	LastNum = -1;
	PageNum = 1;
	while (InHelp)
	{
		BE_ST_ShortSleep();
		// Display new page of text.
		//
		if (PageNum != LastNum)
		{
			US_DrawWindow(1,1,PAGE_WIDTH,23);
			PrintPropText(textinfo->pages[PageNum-1]);
			LastNum = PageNum;
		}

		VW_UpdateScreen();
		if (!faded_in)
		{
			VW_FadeIn();
			faded_in = true;
		}

		// Scroll through text / exit.
		//
		IN_ReadControl(0,&control);
		if (control.button1 || Keyboard[1])
			InHelp=false;
		else
		{
			if (ControlTypeUsed != ctrl_Keyboard)
				control.dir = dir_None;

			if (((control.dir == dir_North) || (control.dir == dir_West)) && (PageNum > 1))
			{
				PageNum--;
				while ((control.dir == dir_North) || (control.dir == dir_West))
				{
					BE_ST_ShortSleep();
					IN_ReadControl(0,&control);
				}
			}
			else
				if (((control.dir == dir_South) || (control.dir == dir_East)) && (PageNum < textinfo->totalpages))
				{
					PageNum++;
					while ((control.dir == dir_South) || (control.dir == dir_East))
					{
						BE_ST_ShortSleep();
						IN_ReadControl(0,&control);
					}
				}
		}
	}

	// Wait for 'exit key' to be released.
	//
	while (control.button1 || Keyboard[1])
	{
		BE_ST_ShortSleep();
		IN_ReadControl(0,&control);
	}

// Can you believe it takes all this just to change to 320 mode!!???!
//
	VW_FadeOut();
	VW_SetLineWidth(40);
	MoveScreen(0,0);
	VW_Bar (0,0,320,200,0);
	VW_SetScreenMode(EGA320GR);
	BlackPalette();
	VW_ScreenToScreen(FREESTART-STATUSLEN,0,40,80);

	// REFKEEN - Alternative controllers support	
	BE_ST_AltControlScheme_Pop();
}

//--------------------------------------------------------------------------
// BlackPalette()
//--------------------------------------------------------------------------
void BlackPalette()
{
	extern id0_char_t colors[7][17];

	BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[0]);
	screenfaded = true;
}

//--------------------------------------------------------------------------
// ColoredPalette()
//--------------------------------------------------------------------------
void ColoredPalette()
{
	extern id0_char_t colors[7][17];

	BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[3]);
	screenfaded = false;
}

////////////////////////////////////////////////////////////////////////////
//
// Verify()
//
id0_long_t Verify(const id0_char_t *filename)
{
	BE_FILE_T handle;
	id0_long_t size;

	if (!BE_Cross_IsFileValid(handle=BE_Cross_open_for_reading(filename)))
	//if ((handle=open(filename,O_BINARY))==-1)
		return (0);
	size=BE_Cross_FileLengthFromHandle(handle);
	BE_Cross_close(handle);
	return(size);
}

///////////////////////////////////////////////////////////////////////////
//
//	GE_SaveGame
//
//	Handles user i/o for saving a game
//
///////////////////////////////////////////////////////////////////////////

void GE_SaveGame()
{
	id0_boolean_t GettingFilename=true;
	//id0_char_t drive; // REFKEEN - Removed
//	id0_char_t Filename[FILENAME_LEN+1],drive; //, ID[sizeof(GAMENAME)], VER[sizeof(SAVEVER_DATA)];
	BE_FILE_T handle;
	//struct dfree dfree; // REFKEEN - Removed
	//id0_long_t davail; // REFKEEN - Removed

	VW_FixRefreshBuffer();
	ReadGameList();
	while (GettingFilename)
	{
		DisplayGameList(2,7,3,10);
		US_DrawWindow(5,1,30,3);
		memset(Filename,0,sizeof(Filename));
		US_CPrint("Enter name to SAVE this game:");
		VW_UpdateScreen();
		if (screenfaded)
			VW_FadeIn();
		if (!US_LineInput((linewidth<<2)-32,20,Filename,"",true,8,0))
			goto EXIT_FUNC;
		if (!strlen(Filename))
			goto EXIT_FUNC;
		// REFKEEN - Remove disk space check
#if 0
		drive = getdisk();
		getdfree(drive+1,&dfree);
		davail = (id0_long_t)dfree.df_avail*(id0_long_t)dfree.df_bsec*(id0_long_t)dfree.df_sclus;

		if (davail < 10000l)
		{
			id0_char_t status[40] = "\nDrive:     Free: ";

			US_CenterWindow(30,6);
			US_Print("\n");
			US_CPrintLine("Disk Full: Can't save game.", NULL);
			US_CPrintLine("Try inserting another disk.", NULL);
			status[8] = drive+'A';
			BE_Cross_itoa_dec(davail,&status[18]);
			//itoa(davail,&status[18],10);
			US_CPrint(status);
			VW_UpdateScreen();

			IN_Ack();
		}
		else
#endif
		{
			// REFKEEN - Convert to uppercase for case-sensitive filesystems
			id0_char_t *chptr = Filename;
			for (; *chptr; ++chptr)
			{
				*chptr = BE_Cross_toupper(*chptr);
			}
			memcpy(chptr, ".SAV", strlen(".SAV")); // Minor optimization...
			//strcat(Filename,".SAV");
			GettingFilename = false;
			if (Verify(Filename))								// FILE EXISTS
			{
				US_CenterWindow(22,4);
				US_CPrintLine("That file already exists...", NULL);
				US_CPrintLine("Overwrite it ????", NULL);
				US_CPrintLine("(Y)es or (N)o?", NULL);
				VW_UpdateScreen();

				// REFKEEN - Alternative controllers support
				BE_ST_AltControlScheme_Push();
				BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){21, 49, 27, 0});
				while((!Keyboard[21]) && (!Keyboard[49]) && !Keyboard[27])
				{
					BE_ST_ShortSleep();
				}

				// REFKEEN - Alternative controllers support
				BE_ST_AltControlScheme_Pop();

				if (Keyboard[27])
					goto EXIT_FUNC;
				if (Keyboard[49])
				{
					GettingFilename = true;
					VW_UpdateScreen();
				}
			}
		}
	}

	handle = BE_Cross_open_for_overwriting(Filename);
	//handle = open(Filename,O_RDWR|O_CREAT|O_BINARY,S_IREAD|S_IWRITE);

	/* REFKEEN - Refactoring: EXIT_FUNC label relocated below error
	 * handling, not using label at all if execution arrives this.
	 */
	if (BE_Cross_IsFileValid(handle))
	//if (handle!=-1)
	{
		if ((BE_Cross_writeInt8LEBuffer(handle, refkeen_compat_gelib_gamename, refkeen_compat_gelib_gamename_strbufflen) != refkeen_compat_gelib_gamename_strbufflen) ||  (BE_Cross_writeInt8LEBuffer(handle, SAVEVER_DATA, sizeof(SAVEVER_DATA)) != sizeof(SAVEVER_DATA)))
		//if ((!CA_FarWrite(handle,(void far *)GAMENAME,sizeof(GAMENAME))) || (!CA_FarWrite(handle,(void far *)SAVEVER_DATA,sizeof(SAVEVER_DATA))))
		{
			if (!screenfaded)
				VW_FadeOut();

			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			return;
		}

		if (!USL_SaveGame(handle))
		{
			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			Quit("Save game error");
		}

		BE_Cross_close(handle); // REFKEEN - Originally preceded by valid handle check
	}
	else // REFKEEN - Originally preceded by valid handle check
	{
		remove(Filename);
		US_CenterWindow(22,6);
		US_CPrintLine("DISK ERROR", NULL);
		US_CPrintLine("Check: Write protect...", NULL);
		US_CPrintLine("File name...", NULL);
		US_CPrintLine("Bytes free on disk...", NULL);
		US_CPrintLine("Press SPACE to continue.", NULL);
		VW_UpdateScreen();
		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Push();
		BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){57, 0});
		while (!Keyboard[57])
		{
			BE_ST_ShortSleep();
		}
		while (Keyboard[57])
		{
			BE_ST_ShortSleep();
		}
		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Pop();
	}

EXIT_FUNC:;

	while (Keyboard[1])
	{
		BE_ST_ShortSleep();
	}

	if (!screenfaded)
		VW_FadeOut();
}


///////////////////////////////////////////////////////////////////////////
//
//	GE_LoadGame
//
//	Handles user i/o for loading a game
//
///////////////////////////////////////////////////////////////////////////

id0_boolean_t GE_LoadGame()
{
	id0_boolean_t GettingFilename=true,rt_code=false;
	BE_FILE_T handle;

	IN_ClearKeysDown();
	memset(ID,0,refkeen_compat_gelib_gamename_strbufflen);
	//memset(ID,0,sizeof(ID));
	memset(VER,0,sizeof(VER));
	VW_FixRefreshBuffer();
	ReadGameList();
	while (GettingFilename)
	{
		DisplayGameList(2,7,3,10);
		US_DrawWindow(5,1,30,3);
		memset(Filename,0,sizeof(Filename));
		US_CPrint("Enter name of game to RESTORE:");
		VW_UpdateScreen();
		if (screenfaded)
			VW_FadeIn();
		if (!US_LineInput((linewidth<<2)-32,20,Filename,"",true,8,0))
			goto EXIT_FUNC;
		// REFKEEN - Convert to uppercase for case-sensitive filesystems
		id0_char_t *chptr = Filename;
		for (; *chptr; ++chptr)
		{
			*chptr = BE_Cross_toupper(*chptr);
		}
		memcpy(chptr, ".SAV", strlen(".SAV")); // Minor optimization...
		//strcat(Filename,".SAV");
		GettingFilename = false;

		if (!Verify(Filename))								// FILE DOESN'T EXIST
		{
			US_CenterWindow(22,3);
			US_CPrintLine(" That file doesn't exist....", NULL);
			US_CPrintLine("Press SPACE to try again.", NULL);
			VW_UpdateScreen();
			// REFKEEN - Alternative controllers support
			BE_ST_AltControlScheme_Push();
			BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){57, 0});

			while (!Keyboard[57])
			{
				BE_ST_ShortSleep();
			}
			while (Keyboard[57])
			{
				BE_ST_ShortSleep();
			}
			// REFKEEN - Alternative controllers support
			BE_ST_AltControlScheme_Pop();

			GettingFilename = true;
		}
	}

	handle = BE_Cross_open_for_reading(Filename);
	//handle = open(Filename,O_RDWR|O_BINARY);

	/* REFKEEN - Refactoring: EXIT_FUNC label relocated below error
	 * handling, not using label at all if execution arrives this.
	 */
	if (BE_Cross_IsFileValid(handle))
	//if (handle==-1)
	{
		if ((!CA_FarRead(handle,(id0_byte_t id0_far *)&ID,refkeen_compat_gelib_gamename_strbufflen)) || (!CA_FarRead(handle,(id0_byte_t id0_far *)&VER,sizeof(VER))))
		//if ((!CA_FarRead(handle,(id0_byte_t id0_far *)&ID,sizeof(ID))) || (!CA_FarRead(handle,(id0_byte_t id0_far *)&VER,sizeof(VER))))
		{
			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			return(false);
		}

		if ((strcmp(ID,GAMENAME)) || (strcmp(VER,SAVEVER_DATA)))
		{
			US_CenterWindow(32,4);
			US_CPrintLine(refkeen_compat_gelib_str_with_gamename, NULL);
			//US_CPrintLine("That isn't a "GAMENAME, NULL);
			US_CPrintLine(".SAV file.", NULL);
			US_CPrintLine("Press SPACE to continue.", NULL);
			VW_UpdateScreen();
			// REFKEEN - Alternative controllers support
			BE_ST_AltControlScheme_Push();
			BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){57, 0});
			while (!Keyboard[57])
			{
				BE_ST_ShortSleep();
			}
			while (Keyboard[57])
			{
				BE_ST_ShortSleep();
			}
			// REFKEEN - Alternative controllers support
			BE_ST_AltControlScheme_Pop();

			if (!screenfaded)
				VW_FadeOut();

			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			return(false);
		}

		if (!USL_LoadGame(handle))
		{
			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			Quit("Load game error.");
		}

		rt_code = true;

		BE_Cross_close(handle); // REFKEEN - Originally preceded by valid handle check
	}
	else // REFKEEN - Originally preceded by valid handle check
	{
		US_CenterWindow(22,3);
		US_CPrintLine("DISK ERROR ** LOAD **", NULL);
		US_CPrintLine("Press SPACE to continue.", NULL);
		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Push();
		BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes((const char []){57, 0});
		while (!Keyboard[57])
		{
			BE_ST_ShortSleep();
		}
		while (Keyboard[57])
		{
			BE_ST_ShortSleep();
		}
		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Pop();
	}

EXIT_FUNC:;

	if (!screenfaded)
		VW_FadeOut();

	return(rt_code);
}


// (REFKEEN) UNUSED FUNCTION

#if 0
///////////////////////////////////////////////////////////////////////////
//
//	GE_HardError() - Handles the Abort/Retry/Fail sort of errors passed
//			from DOS. Hard coded to ignore if during Load/Save Game.
//
///////////////////////////////////////////////////////////////////////////
//#pragma	warn	-par
//#pragma	warn	-rch
id0_int_t GE_HardError(id0_word_t errval,id0_int_t ax,id0_int_t bp,id0_int_t si)
{
#define IGNORE  0
#define RETRY   1
#define	ABORT   2
extern	void	ShutdownId(void);

static	id0_char_t		buf[32];
static	WindowRec	wr;
static	id0_boolean_t		oldleavedriveon;
		id0_int_t			di;
		id0_char_t		c,*s,*t;
id0_boolean_t holdscreenfaded;

	if (InLoadSaveGame)
		hardresume(IGNORE);


	di = _DI;

	oldleavedriveon = LeaveDriveOn;
	LeaveDriveOn = false;

	if (ax < 0)
		s = "Device Error";
	else
	{
		if ((di & 0x00ff) == 0)
			s = "Drive ~ is Write Protected";
		else
			s = "Error on Drive ~";
		for (t = buf;*s;s++,t++)	// Can't use sprintf()
			if ((*t = *s) == '~')
				*t = (ax & 0x00ff) + 'A';
		*t = '\0';
		s = buf;
	}

	c = peekb(0x40,0x49);	// Get the current screen mode
	if ((c < 4) || (c == 7))
		goto oh_kill_me;

	// DEBUG - handle screen cleanup
	holdscreenfaded=screenfaded;

	US_SaveWindow(&wr);
	VW_ClearVideo(0);            ////////////// added for exiting
	US_CenterWindow(30,3);
	US_CPrint(s);
	US_CPrint("(R)etry or (A)bort?");
	VW_UpdateScreen();
	if (holdscreenfaded)
		VW_FadeIn();
	IN_ClearKeysDown();

asm	sti	// Let the keyboard interrupts come through

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
			if (holdscreenfaded)
				VW_FadeOut();
			US_ClearWindow();
			VW_UpdateScreen();
			US_RestoreWindow(&wr);
			LeaveDriveOn = oldleavedriveon;
			return(RETRY);
			break;
		}
		BE_ST_ShortSleep();
	}

oh_kill_me:
	abortprogram = s;
	TrashProg("Terminal Error: %s\n",s);
//	if (tedlevel)
//		fprintf(stderr,"You launched from TED. I suggest that you reboot...\n");

	return(ABORT);
#undef	IGNORE
#undef	RETRY
#undef	ABORT
}
//#pragma	warn	+par
//#pragma	warn	+rch

#endif

//--------------------------------------------------------------------------
//
//
//                          B O B   ROUTINES
//
//
//--------------------------------------------------------------------------



#ifdef BOBLIST

////////////////////////////////////////////////////////////////////////////
//
// UpdateBOBList() - Adds a sprite to an objects BOBlist.  The BOB List
//						 	must already be allocated and have an available slot.
//
//	RETURNS : true = Success adding Sprite / false = Failure.
//
// NOTE : This also sets the users 'needtoreact' flag to true.
//
id0_boolean_t UpdateBOBList(objtype *obj,struct Simple_Shape *Shape,shapeclass Class, id0_short_t priority, spriteflags sprflags)
{
	struct BOB_Shape *CurBOBShape = NULL;

//#pragma warn -pia

	if (CurBOBShape = obj->nextshape)
	{
		// Treverse down BOBList looking for a sprite with the same class
		// OR an empty shape struct to store the new shape.

		while ((CurBOBShape->class != Class) && (CurBOBShape->class) && CurBOBShape)
		{
			CurBOBShape = CurBOBShape->nextshape;
		}

		if (CurBOBShape)
		{
			RF_RemoveSprite(&CurBOBShape->sprite);
			CurBOBShape->shapenum = Shape->shapenum;
			CurBOBShape->x_offset = Shape->x_offset;
			CurBOBShape->y_offset = Shape->y_offset;
			CurBOBShape->priority = priority;
			CurBOBShape->sprflags = sprflags;
			CurBOBShape->class = Class;
			return(true);
		}
	}
	return(false);

//#pragma warn +pia

}

/////////////////////////////////////////////////////////////////////////////
//
// RemoveBOBShape() - Removes a sprite from a BOBList.
//
// RETURNS : true = Success / false = Failure (shape not found)
//
id0_boolean_t RemoveBOBShape(objtype *obj, shapeclass Class)
{
	struct BOB_Shape *CurBOBShape = NULL;

//#pragma warn -pia

	if (CurBOBShape = obj->nextshape)
	{
		while ((CurBOBShape->class != Class) && (!CurBOBShape->class) && CurBOBShape)
		{
			CurBOBShape = CurBOBShape->nextshape;
		}

		if (CurBOBShape)
		{
			CurBOBShape->class = noshape;
			return(true);
		}
	}
	return(false);

//#pragma warn +pia

}


/////////////////////////////////////////////////////////////////////////////
//
// RemoveBOBList() - Removes an entire BOBList attached to an object.
//
//
void RemoveBOBList(objtype *obj)
{
	struct BOB_Shape *CurBOBShape;

//#pragma warn -pia

	if (CurBOBShape = obj->nextshape)
	{
		// Treverse down BOBList looking for a sprite with the same class
		// OR an empty shape struct to store the new shape.

		while (CurBOBShape)
		{
			if (CurBOBShape->class)
			{
				CurBOBShape->class = noshape;
				RF_RemoveSprite (&CurBOBShape->sprite);
			}
			CurBOBShape = CurBOBShape->nextshape;
		}
	}

//#pragma warn +pia

}



/////////////////////////////////////////////////////////////////////////////
//
// InitBOBList() -- This initializes a BOB list for all the possible shapes
//						  attached at one time.  This is done with an array of
//						  BOB_Shape structs and links the 'nextshape' pointer to
//						  to the next element.
//
//
void InitBOBList(objtype *obj, struct BOB_Shape *BOB_Shape, id0_short_t NumElements)
{
	struct BOB_Shape *CurShape;
	id0_short_t loop;

	obj->nextshape = BOB_Shape;

	for (loop=1;loop<NumElements;loop++)
	{
		CurShape = BOB_Shape++;
		CurShape->nextshape = BOB_Shape;
	}

	BOB_Shape->nextshape = NULL;
}


////////////////////////////////////////////////////////////////////////////
//
// RefreshBOBList() -- This routine updates all sprites attached to the
//							  BOBList and refreshes there position in the sprite
//							  list.
//
void RefreshBOBList(objtype *obj)
{
	struct BOB_Shape *Shape;

	Shape = obj->nextshape;

	while (Shape)
	{
		if (Shape->class)
			RF_PlaceSprite(&Shape->sprite,obj->x+Shape->x_offset,obj->y+Shape->y_offset, Shape->shapenum, spritedraw,Shape->priority,Shape->sprflags);
		Shape = Shape->nextshape;
	}
}
#endif










//==========================================================================
// JAMPAK routines
//==========================================================================
#define N		4096
#define F		18

// THRESHOLD : encode string into position and length if match_length is
// greater than this

#define THRESHOLD				2

// index for root of binary search trees
//

#define NIL       			N
#define COMP					"COMP"

id0_unsigned_long_t 	textsize = 0,  		// text size counter
					codesize = 0,			// code size counter
					printcount = 0;     	// counter for reporting progress every 1K bytes

id0_unsigned_char_t id0_far text_buf[N + F - 1];

memptr segptr;

BufferedIO lzwBIO;

//--------------------------------------------------------------------------
// BLoad()
//--------------------------------------------------------------------------
id0_unsigned_long_t BLoad(const id0_char_t *SourceFile, memptr *DstPtr)
{
	BE_FILE_T handle;

	memptr SrcPtr;
	//id0_longword_t i, j, k, r, c;
	//id0_word_t flags;
	id0_byte_t Buffer[8];
	id0_longword_t DstLen, SrcLen;
	id0_boolean_t comp;

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(SourceFile)))
	//if ((handle = open(SourceFile, O_RDONLY|O_BINARY)) == -1)
		return(0);

	// Look for 'COMP' header
	//
	BE_Cross_readInt8LEBuffer(handle,Buffer,4);
	//read(handle,Buffer,4);
	comp = !strncmp((char *)Buffer,COMP,4);

	// Get source and destination length.
	//
	if (comp)
	{
		SrcLen = Verify(SourceFile);
		BE_Cross_readInt32LE(handle, &DstLen);
		//read(handle,(void *)&DstLen,4);
		MM_GetPtr(DstPtr,DstLen);
		if (!*DstPtr)
			return(0);
	}
	else
		DstLen = Verify(SourceFile);

	// LZW decompress OR simply load the file.
	//
	if (comp)
	{

		// REFKEEN - Looks like this is an unsigned comparison in original EXE
		if ((id0_longword_t)MM_TotalFree() < SrcLen)
		{
			if (!InitBufferedIO(handle,&lzwBIO))
				TrashProg("No memory for buffered I/O.");
			//lzwDecompressFromFile(&lzwBIO,MK_FP(*DstPtr,0),SrcLen+8);
			lzwDecompressFromFile(&lzwBIO,(id0_byte_t *)(*DstPtr),SrcLen+8);
			FreeBufferedIO(&lzwBIO);
		}
		else
		{
			// REFKEEN - Better close the current file handle before re-opening here
			BE_Cross_close(handle);
			//
			CA_LoadFile(SourceFile,&SrcPtr);
			//lzwDecompressFromRAM(MK_FP(SrcPtr,8),MK_FP(*DstPtr,0),SrcLen+8);
			lzwDecompressFromRAM((id0_byte_t *)SrcPtr+8,(id0_byte_t *)(*DstPtr),SrcLen+8);
			MM_FreePtr(&SrcPtr);
			// REFKEEN - File handle already closed
			return(DstLen);
		}
	}
	else
	{
		// REFKEEN - Again we close the current file handle first, then load and finally return DstLen without re-closing file handle
		BE_Cross_close(handle);
		CA_LoadFile(SourceFile,DstPtr);
		return(DstLen);
	}

	BE_Cross_close(handle);
	return(DstLen);
}

//--------------------------------------------------------------------------
// lzwDecompressFromRAM()
//
// SrcPtr - pointer to first byte of compressed data.
// DstPtr - pointer to decompress memory area.
// SrcLen - length of compressed data.
//
//--------------------------------------------------------------------------

#undef nextch
//#undef nextch()
#define nextch(ptr)	*ptr++

void lzwDecompressFromRAM(id0_byte_t id0_far *SrcPtr, id0_byte_t id0_far *DstPtr, id0_longword_t SrcLen)
{
	id0_longword_t i, j, k, r, c;
	id0_word_t flags;
	//id0_byte_t ch;


	for (i = 0; i < N - F; i++)
		text_buf[i] = ' ';

	 r = N - F;
	 flags = 0;

	 for ( ; ; )
	 {
			if (((flags >>= 1) & 256) == 0)
			{
				if (!(--SrcLen))
					break;
				c=nextch(SrcPtr);

				flags = c | 0xff00;      /* uses higher byte cleverly */
			}                                  /* to count eight */

			if (flags & 1)
			{
				if (!(--SrcLen))
					break;
				c=nextch(SrcPtr);

				*DstPtr++ = c;
				text_buf[r++] = c;
				r &= (N - 1);
			}
			else
			{
				if (!(--SrcLen))
					break;
				i=nextch(SrcPtr);

				if (!(--SrcLen))
					break;
				j=nextch(SrcPtr);

				i |= ((j & 0xf0) << 4);
				j = (j & 0x0f) + THRESHOLD;

				for (k = 0; k <= j; k++)
				{
						 c = text_buf[(i + k) & (N - 1)];
						 *DstPtr++ = c;
						 text_buf[r++] = c;
						 r &= (N - 1);
				}
			}
	 }
}

//--------------------------------------------------------------------------
// lzwDecompressFromHandle()
//
// SrcPtr - pointer to first byte of compressed data.
// DstPtr - pointer to decompress memory area.
// SrcLen - length of compressed data.
//
//--------------------------------------------------------------------------

#undef nextch
//#undef nextch()
#define nextch(handle)	bio_readch(&lzwBIO)

void lzwDecompressFromFile(BufferedIO *SrcPtr, id0_byte_t id0_far *DstPtr, id0_longword_t SrcLen)
{
	id0_longword_t i, j, k, r, c;
	id0_word_t flags;
	//id0_byte_t ch;


	for (i = 0; i < N - F; i++)
		text_buf[i] = ' ';

	 r = N - F;
	 flags = 0;

	 for ( ; ; )
	 {
			if (((flags >>= 1) & 256) == 0)
			{
				if (!(--SrcLen))
					break;
				c=nextch(SrcPtr);

				flags = c | 0xff00;      /* uses higher byte cleverly */
			}                                  /* to count eight */

			if (flags & 1)
			{
				if (!(--SrcLen))
					break;
				c=nextch(SrcPtr);

				*DstPtr++ = c;
				text_buf[r++] = c;
				r &= (N - 1);
			}
			else
			{
				if (!(--SrcLen))
					break;
				i=nextch(SrcPtr);

				if (!(--SrcLen))
					break;
				j=nextch(SrcPtr);

				i |= ((j & 0xf0) << 4);
				j = (j & 0x0f) + THRESHOLD;

				for (k = 0; k <= j; k++)
				{
						 c = text_buf[(i + k) & (N - 1)];
						 *DstPtr++ = c;
						 text_buf[r++] = c;
						 r &= (N - 1);
				}
			}
	 }
}

//--------------------------------------------------------------------------
// InitBufferedIO()
//--------------------------------------------------------------------------
memptr InitBufferedIO(BE_FILE_T handle, BufferedIO *bio)
{
	bio->handle = handle;
	bio->offset = BIO_BUFFER_LEN;
	bio->status = 0;
	MM_GetPtr(&bio->buffer,BIO_BUFFER_LEN);

	return(bio->buffer);
}

//--------------------------------------------------------------------------
// FreeBufferedIO()
//--------------------------------------------------------------------------
void FreeBufferedIO(BufferedIO *bio)
{
	if (bio->buffer)
		MM_FreePtr(&bio->buffer);
}

//--------------------------------------------------------------------------
// bio_readch()
//--------------------------------------------------------------------------
id0_byte_t bio_readch(BufferedIO *bio)
{
	id0_byte_t id0_far *buffer;

	if (bio->offset == BIO_BUFFER_LEN)
	{
		bio->offset = 0;
		bio_fillbuffer(bio);
	}

	buffer = (id0_byte_t *)(bio->buffer) + (bio->offset++);
	//buffer = MK_FP(bio->buffer,bio->offset++);

	return(*buffer);
}

//--------------------------------------------------------------------------
// bio_fillbuffer()
//
// BUGS (Not really bugs... More like RULES!)
//
//    1) This code assumes BIO_BUFFER_LEN is no smaller than
//       NEAR_BUFFER_LEN!!
//
//    2) BufferedIO.status should be altered by this code to report
//       read errors, end of file, etc... If you know how big the file
//       is you're reading, determining EOF should be no problem.
//
//--------------------------------------------------------------------------
void bio_fillbuffer(BufferedIO *bio)
{
	#define NEAR_BUFFER_LEN	(64)
	id0_byte_t near_buffer[NEAR_BUFFER_LEN];
	id0_short_t bio_length,bytes_read,bytes_requested;

	bytes_read = 0;
	bio_length = BIO_BUFFER_LEN;
	while (bio_length)
	{
		if (bio_length > NEAR_BUFFER_LEN-1)
			bytes_requested = NEAR_BUFFER_LEN;
		else
			bytes_requested = bio_length;

		BE_Cross_readInt8LEBuffer(bio->handle,near_buffer,bytes_requested);
		//read(bio->handle,near_buffer,bytes_requested);
		memcpy((id0_byte_t *)(bio->buffer) + bytes_read,near_buffer,bytes_requested);
		//_fmemcpy(MK_FP(bio->buffer,bytes_read),near_buffer,bytes_requested);

		bio_length -= bytes_requested;
		bytes_read += bytes_requested;
	}
}

// REFKEEN - UNUSED (cross-platform replacements may be used, depending on arch)
#if 0
///////////////////////////////////////////////////////////////////////////
//
// SwapLong()
//
void SwapLong(id0_long_t id0_far *Var)
{
	*Var = ((id0_longword_t)(*Var) >> 24) |
	       (((id0_longword_t)(*Var) >> 8) & 0xFF00) |
	       (((id0_longword_t)(*Var) << 8) & 0xFF0000) |
	       ((id0_longword_t)(*Var) << 24);
}

///////////////////////////////////////////////////////////////////////////
//
// SwapWord()
//
void SwapWord(id0_unsigned_int_t id0_far *Var)
{
	*Var = ((*Var) >> 8) | ((*Var) << 8);
}
#endif


#if 0 // REFKEEN - UNUSED

////////////////////////////////////////////////////////////////////////////
//
// LoadShape()
//
id0_int_t LoadShape(const id0_char_t *Filename,struct Shape *SHP)
{
	#define CHUNK(Name)	(*ptr == *Name) &&			\
								(*(ptr+1) == *(Name+1)) &&	\
								(*(ptr+2) == *(Name+2)) &&	\
								(*(ptr+3) == *(Name+3))


	id0_int_t RT_CODE;
//	struct ffblk ffblk;
	//FILE *fp;
	//id0_char_t CHUNK[5];
	id0_char_t id0_far *ptr;
	memptr IFFfile = NULL;
	id0_unsigned_long_t FileLen, size, ChunkLen;
	//id0_int_t loop;


	RT_CODE = 1;

	// Decompress to ram and return ptr to data and return len of data in
	//	passed variable...

	if (!(FileLen = BLoad(Filename,&IFFfile)))
		TrashProg("Can't load Compressed Shape - Possibly corrupt file!");

	// Evaluate the file
	//
	ptr = (id0_char_t *)IFFfile;
	//ptr = MK_FP(IFFfile,0);
	if (!CHUNK("FORM"))
		goto EXIT_FUNC;
	ptr += 4;

	FileLen = *(id0_long_t id0_far *)ptr;
	SwapLong((id0_long_t id0_far *)&FileLen);
	ptr += 4;

	if (!CHUNK("ILBM"))
		goto EXIT_FUNC;
	ptr += 4;

	FileLen += 4;
	while (FileLen)
	{
		ChunkLen = *(id0_long_t id0_far *)(ptr+4);
		SwapLong((id0_long_t id0_far *)&ChunkLen);
		ChunkLen = (ChunkLen+1) & 0xFFFFFFFE;

		if (CHUNK("BMHD"))
		{
			ptr += 8;
			SHP->bmHdr.w = ((struct BitMapHeader id0_far *)ptr)->w;
			SHP->bmHdr.h = ((struct BitMapHeader id0_far *)ptr)->h;
			SHP->bmHdr.x = ((struct BitMapHeader id0_far *)ptr)->x;
			SHP->bmHdr.y = ((struct BitMapHeader id0_far *)ptr)->y;
			SHP->bmHdr.d = ((struct BitMapHeader id0_far *)ptr)->d;
			SHP->bmHdr.trans = ((struct BitMapHeader id0_far *)ptr)->trans;
			SHP->bmHdr.comp = ((struct BitMapHeader id0_far *)ptr)->comp;
			SHP->bmHdr.pad = ((struct BitMapHeader id0_far *)ptr)->pad;
			SwapWord(&SHP->bmHdr.w);
			SwapWord(&SHP->bmHdr.h);
			SwapWord(&SHP->bmHdr.x);
			SwapWord(&SHP->bmHdr.y);
			ptr += ChunkLen;
		}
		else
		if (CHUNK("BODY"))
		{
			ptr += 4;
			size = *((id0_long_t id0_far *)ptr);
			ptr += 4;
			SwapLong((id0_long_t id0_far *)&size);
			SHP->BPR = (SHP->bmHdr.w+7) >> 3;
			MM_GetPtr(&SHP->Data,size);
			if (!SHP->Data)
				goto EXIT_FUNC;
			//movedata(FP_SEG(ptr),FP_OFF(ptr),FP_SEG(SHP->Data),0,size);
			memcpy(SHP->Data,ptr,size);
			ptr += ChunkLen;

			break;
		}
		else
			ptr += ChunkLen+8;

		FileLen -= ChunkLen+8;
	}

	RT_CODE = 0;

EXIT_FUNC:;
	if (IFFfile)
	{
//		segptr = (memptr)FP_SEG(IFFfile);
		MM_FreePtr(&IFFfile);
	}

	return (RT_CODE);
}

#endif

////////////////////////////////////////////////////////////////////////////
//
// FreeShape()
//
void FreeShape(struct Shape *shape)
{
	if (shape->Data)
		MM_FreePtr(&shape->Data);
}

////////////////////////////////////////////////////////////////////////////
//
// UnpackEGAShapeToScreen()
//
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty)
{
	id0_int_t currenty;
	id0_signed_char_t n, Rep, id0_far *Src/*, id0_far *Dst[8]*/, loop, Plane;
	id0_unsigned_int_t DstOff[8];
	id0_unsigned_int_t BPR, Height;
	id0_boolean_t NotWordAligned;

	NotWordAligned = SHP->BPR & 1;
	startx>>=3;
	Src = (id0_signed_char_t *)(SHP->Data);
	//Src = MK_FP(SHP->Data,0);
	currenty = starty;
	Plane = 0;
	Height = SHP->bmHdr.h;
	while (Height--)
	{
#if 0
		Dst[0] = (MK_FP(0xA000,displayofs));
		Dst[0] += ylookup[currenty];
		Dst[0] += startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			Dst[loop] = Dst[0];
#endif
		DstOff[0] = displayofs + ylookup[currenty] + startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			DstOff[loop] = DstOff[0];

		for (Plane=0; Plane<SHP->bmHdr.d; Plane++)
		{
			//outport(0x3c4,((1<<Plane)<<8)|2);

			BPR = ((SHP->BPR+1) >> 1) << 1;               // IGNORE WORD ALIGN
			while (BPR)
			{
				if (SHP->bmHdr.comp)
					n = *Src++;
				else
					n = BPR-1;

				if (n < 0)
				{
					if (n != -128)
					{
						n = (-n)+1;
						BPR -= n;
						Rep = *Src++;
						if ((!BPR) && (NotWordAligned))   // IGNORE WORD ALIGN
							n--;

						while (n--)
							BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, Rep, 1<<Plane);
							//*Dst[Plane]++ = Rep;
					}
					else
						BPR--;
				}
				else
				{
					n++;
					BPR -= n;
					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						n--;

					while (n--)
						BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, *Src++, 1<<Plane);
						//*Dst[Plane]++ = *Src++;

					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						Src++;
				}
			}
		}
		currenty++;
	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////
//
// GetKeyChoice()
//
id0_char_t GetKeyChoice(const id0_char_t *choices,id0_boolean_t clear)
{
	extern void DoEvents(void);

	id0_boolean_t waiting;
	const id0_char_t *s/*,*ss*/;

	// REFKEEN - Alternative controllers support	
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes(choices);

	IN_ClearKeysDown();

	waiting = true;
	while (waiting)
	{
		s = choices;
		while (*s)
		{
			if (Keyboard[*s++])
			{
				waiting=false;
				break;
			}
		}
		BE_ST_ShortSleep();
	}

	BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support

	IN_ClearKeysDown();

	return(*(--s));
}

#if 0

////////////////////////////////////////////////////////////////////////////
//
// AnimateObj()
//
id0_boolean_t AnimateObj(objtype *obj)
{
	id0_boolean_t Done;

	Done = false;

	if (obj->animtype == at_NONE)		// Animation finished?
		return(true);						// YEP!

	if (obj->animdelay)					// Check animation delay.
	{
		obj->animdelay -= tics;
		if (obj->animdelay < 0)
			obj->animdelay = 0;
		return(false);
	}

	switch (obj->animtype)				// Animate this object!
	{
		case at_ONCE:
		case at_CYCLE:
			switch (obj->animdir)
			{
				case at_FWD:
					if (obj->curframe < obj->maxframe)
						AdvanceAnimFWD(obj);
					else
						if (obj->animtype == at_CYCLE)
						{
							obj->curframe = 0;		 // RESET CYCLE ANIMATION
							obj->animdelay=1;
						}
						else
						{
							obj->animtype = at_NONE; // TERMINATE ONCE ANIM
							Done = true;
						}
					break;

				case at_REV:
					if (obj->curframe > 0)
						AdvanceAnimREV(obj);
					else
						if (obj->animtype == at_CYCLE)
						{
							obj->curframe = obj->maxframe;	// RESET CYCLE ANIMATION
							obj->animdelay = 1;
						}
						else
						{
							obj->animtype = at_NONE;   // TERMINATE ONCE ANIM
							Done = true;
						}
					break; // REV
			}
			break;

		case at_REBOUND:
			switch (obj->animdir)
			{
				case at_FWD:
					if (obj->curframe < obj->maxframe)
						AdvanceAnimFWD(obj);
					else
					{
						obj->animdir = at_REV;
						obj->animdelay = 1;
					}
					break;

				case at_REV:
					if (obj->curframe > 0)
						AdvanceAnimREV(obj);
					else
					{
						obj->animdir = at_FWD;
						obj->animdelay = 1;
						Done = true;
					}
					break;
			}
			break; /* REBOUND */

		case at_WAIT:
			Done = true;
			break;
	}

	return(Done);
}

void AdvanceAnimFWD(objtype *obj)	// Advances a Frame of ANIM for ONCE,CYCLE, REBOUND
{
	obj->curframe++; // INC frames
	obj->animdelay = obj->maxdelay;		 // Init Delay Counter.
	obj->needtoreact = true;
}


void AdvanceAnimREV(objtype *obj)  // Advances a Frame of ANIM for ONCE,CYCLE, REBOUND
{
	obj->curframe--; // DEC frames
	obj->animdelay = obj->maxdelay;		 // Init Delay Counter.
	obj->needtoreact = true;
}
#endif

#if 0
///////////////////////////////////////////////////////////////////////////
//
// LoadASArray()  - Load an array of audio samples in FAR memory.
//
void LoadASArray(struct Sample *ASArray)
{
	id0_int_t loop = 0;

	while (ASArray[loop].filename)
	{
	  if (!BLoad(ASArray[loop].filename,(memptr *)&ASArray[loop].data))
		  TrashProg("Unable to load sample in LoadASArray()");
	  loop++;
	}
}



////////////////////////////////////////////////////////////////////////////
//
// FreeASArray() - Frees an ASArray from memory that has been loaded with
//						 LoadASArray()
//
void FreeASArray(struct Sample *ASArray)
{
	id0_unsigned_t loop = 0;

	while (ASArray[loop].data)
	{
		MM_SetPurge((memptr *)&ASArray[loop++].data,3);
		MM_FreePtr((memptr *)&ASArray[loop++].data);
	}
}

///////////////////////////////////////////////////////////////////////////
//
// GE_LoadAllDigiSounds()	- This is a hook that CA_LoadAllSounds()
//									  calls to load all of the Digitized sounds for
//									  Sound Blaster & Sound Source.
//
// NOTE : This stub would do any other necessary routines for DigiSounds
//			 specific to GAMERS EDGE code. (Keeping seperate from ID's)
//
void GE_LoadAllDigiSounds()
{
	LoadASArray(DigiSounds);
}



/////////////////////////////////////////////////////////////////////////
//
// GE_FreeAllDigiSounds() -- This is a hook that CA_LoadAllSounds()
//									  calls to free all digitized sounds for
//									  which ever hardware and allow for any necessary
//									  clean up.
//
//
// NOTE : This stub would do any other necessary routines for DigiSounds
//			 specific to GAMERS EDGE code. (Keeping seperate from ID's)
//
void GE_FreeAllDigiSounds()
{
	FreeASArray(DigiSounds);
}



///////////////////////////////////////////////////////////////////////////
//
// GE_LoadAllSounds()	- Loads ALL sounds needed for detected hardware.
//
void GE_LoadAllSounds()
{
	id0_unsigned_t i,start;

	start = STARTPCSOUNDS;
	for (i=0;i<NUMSOUNDS;i++,start++)
		CA_CacheAudioChunk (start);

	if (AdLibPresent)
	{
		start = STARTADLIBSOUNDS;
		for (i=0;i<NUMSOUNDS;i++,start++)
			CA_CacheAudioChunk (start);
	}

	if (SoundBlasterPresent)
		LoadASArray(DigiSounds);
}


//////////////////////////////////////////////////////////////////////////
//
// GE_PurgeAllSounds() - Frees all sounds that were loaded.
//
void GE_PurgeAllSounds()
{
	id0_unsigned_t start,i;

	start = STARTPCSOUNDS;
	for (i=0;i<NUMSOUNDS;i++,start++)
		if (audiosegs[start])
			MM_SetPurge ((memptr *)&audiosegs[start],3);		// make purgable


	if (AdLibPresent)
	{
		start = STARTADLIBSOUNDS;
		for (i=0;i<NUMSOUNDS;i++,start++)
			if (audiosegs[start])
				MM_SetPurge ((memptr *)&audiosegs[start],3);		// make purgable
	}

	if (SoundBlasterPresent)
		GE_FreeAllDigiSounds();
}


/////////////////////////////////////////////////////////////////////////////
//
// PlaySample() -- Plays a DIGITIZED sample using SoundBlaster OR SoundSource
//
// PARAMETERS : Sample Number (Corresponding to ASArray "DigiSounds[]".
//
void PlaySample(id0_unsigned_t SampleNum)
{

	if (!DigiSounds[SampleNum].data)
		TrashProg("PlaySample - Trying to play an unloaded digi sound!");


	switch (SoundMode)				// external variable in ID_SD for sound mode..
	{
		case sdm_SoundBlaster:
		case sdm_SoundBlasterAdLib:
			SDL_SBPlaySample(MK_FP(DigiSounds[SampleNum].data,0));
			break;

		default:
			TrashProg("PlaySample() - incorrect SoundMode for PlaySample()");
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
// SelectDigiAudio() -- This routine intergrates multi sound hardware with
//								id's routines.
//
void SelectDigiAudio(AudioDeviceType Device)
{
	switch (Device)
	{
		case ged_SoundSource:
		case ged_SoundBlaster:
			break;
	}
}
#endif

///////////////////////////////////////////////////////////////////////////
//
// DisplayGameList()
//
void DisplayGameList(id0_short_t winx, id0_short_t winy, id0_short_t list_width, id0_short_t list_height)
{
	#define SPACES 2

	id0_short_t width,col,row,orgcol,games_printed=0,h;

	// Possibly shrink window.
	//
	h = (NumGames / list_width) + ((NumGames % list_width) > 0);
	if (h < list_height)
		list_height = h;

	// Open window and print header...
	//
	US_DrawWindow(winx,winy,list_width*(8+SPACES*2),list_height+3);
	US_CPrintLine("LIST OF SAVED GAMES", NULL);
	US_Print("\n");

	col = orgcol = PrintX;
	row = PrintY;

	// Display as many 'save game' files as can fit in the window.
	//
	width = list_width;
	while ((games_printed<NumGames) && (list_height))
	{
		// Print filename and padding spaces.
		//
		void US_Printxy(id0_word_t x, id0_word_t y, id0_char_t *text);

		US_Printxy(col+(SPACES*8),row,GameListNames[games_printed]);
		col += 8*((SPACES*2)+8);

		// Check for end-of-line or end-of-window.
		//
		width--;
		if (!width)
		{
			col = orgcol;
			row += 8;
			width = list_width;
			list_height--;
			US_Print("\n");
		}

		games_printed++;
	}
}

////////////////////////////////////////////////////////////////////////////
//
// ReadGameList()
//
void ReadGameList()
{
	NumGames = BE_Cross_GetSortedRewritableFilenames_AsUpperCase((id0_char_t *)GameListNames, MAX_GAMELIST_NAMES+1, FNAME_LEN, ".sav");
#if 0
	struct ffblk ffblk;
	id0_short_t done,len;

	NumGames = -1;
	done = findfirst("*.sav",&ffblk,0);

	while (!done)
	{
		if (NumGames == MAX_GAMELIST_NAMES)
			memcpy(GameListNames,GameListNames[1],MAX_GAMELIST_NAMES*sizeof(GameListNames[0]));
		else
			NumGames++;

		fnsplit(ffblk.ff_name,NULL,NULL,GameListNames[NumGames],NULL);

		done=findnext(&ffblk);
	}

	NumGames++;
#endif
}

////////////////////////////////////////////////////////////////////////////
//
// LoadTextFile()
//
id0_long_t LoadTextFile(const id0_char_t *filename,textinfo *textinfo)
{
//#pragma warn -pia
	id0_long_t size;

	if (!(size=BLoad(filename,&textinfo->textptr)))
		return(0);
	InitTextFile(textinfo);

	return(size);
//#pragma warn +pia
}

//-------------------------------------------------------------------------
// FreeTextFile()
//-------------------------------------------------------------------------
void FreeTextFile(textinfo *textinfo)
{
	if (textinfo->textptr)
		MM_FreePtr(&textinfo->textptr);
}

////////////////////////////////////////////////////////////////////////////
//
// InitTextFile()
//
void InitTextFile(textinfo *textinfo)
{
	#define END_PAGE  '@'

	id0_char_t id0_far *text = (id0_char_t *)(textinfo->textptr);
	//id0_char_t id0_far *text = MK_FP(textinfo->textptr,0);

	textinfo->totalpages = 0;
	while (*text != END_PAGE)
	{
		if (textinfo->totalpages < MAX_TEXT_PAGES)
			textinfo->pages[textinfo->totalpages++] = text;
		else
			TrashProg("GE ERROR: Too many text pages. --> %d",textinfo->totalpages);

		while (*text != END_PAGE)
		{
			if ((*text == '\r') && (*(text+1) == '\n'))
			{
				*text = 32;
				*(text+1) = '\n';
				text+=2;
			}
			else
				text++;
		}
		*text = 0;
		text += 3;
	}
	*text = 0;
}

#if 0
////////////////////////////////////////////////////////////////////////////
//
// CenterObj()
//
void CenterObj(objtype *obj, id0_unsigned_t x, id0_unsigned_t y)
{
	spritetabletype id0_far *sprite;
	id0_unsigned_t width, height;

	sprite=&spritetable[obj->baseshape+obj->curframe-STARTSPRITES];

	width = (sprite->xh-sprite->xl+(1<<G_P_SHIFT)) >> 1;
	if (obj->sprflags&sf_vertflip)
	{
		height = (sprite->yl-(sprite->height<<G_P_SHIFT) + sprite->yh+(1<<G_P_SHIFT)) >> 1;
	}
	else
		height = (sprite->yh-sprite->yl+(1<<G_P_SHIFT)) >> 1;

	obj->x = x-width;
	obj->y = y-height;
}
#endif

//-------------------------------------------------------------------------
// cacheout()
//-------------------------------------------------------------------------
void cacheout(id0_short_t s,id0_short_t e)
{
	id0_short_t i;

	for(i=(s);i<=(e);i++)
	{
		grneeded[i]&=~ca_levelbit;
		if (grsegs[i])
			MM_SetPurge(&grsegs[i],3);
	}

}

//-------------------------------------------------------------------------
// cachein()
//-------------------------------------------------------------------------
void cachein(id0_short_t s,id0_short_t e)
{
	id0_short_t i;

	for(i=(s);i<=(e);i++)
	{
		CA_MarkGrChunk(i);
		if (grsegs[i])
			MM_SetPurge(&grsegs[i],0);
	}
}

#if 0
////////////////////////////////////////////////////////////////////////////
//
// SetUpdateBlock()
//
void SetUpdateBlock(id0_unsigned_t x,id0_unsigned_t y,id0_unsigned_t width,id0_unsigned_t height,id0_char_t refresh)
{
	eraseblocktype *erase;

#if 0 //SP unsure if this is needed
	x = (x+((MAPBORDER+MAPXLEFTOFFSET)<<4))>>3;
	y += ((MAPBORDER+MAPYTOPOFFSET)<<4);
#else
	x = (x+(MAPBORDER<<4))>>3;
	y += (MAPBORDER<<4);
#endif
	width >>= 3;

	if (refresh & 1)
	{
		erase = eraselistptr[0]++;
		erase->screenx=x;
		erase->screeny=y;
		erase->width=width;
		erase->height=height;
	}

	if (refresh & 2)
	{
		erase = eraselistptr[1]++;
		erase->screenx=x;
		erase->screeny=y;
		erase->width=width;
		erase->height=height;
	}
}


////////////////////////////////////////////////////////////////////////////
//
// ObjHeight
//
id0_unsigned_t ObjHeight(objtype *obj)
{
	spritetabletype id0_far *sprite;

	sprite=&spritetable[obj->baseshape+obj->curframe-STARTSPRITES];

	if (obj->sprflags&sf_vertflip)
	{
		return((sprite->yl-(sprite->height<<G_P_SHIFT) + sprite->yh+(1<<G_P_SHIFT)) >> 1);
	}
	else
		return((sprite->yh-sprite->yl+(1<<G_P_SHIFT)) >> 1);
}


////////////////////////////////////////////////////////////////////////////
//
// ObjWidth
//
id0_unsigned_t ObjWidth(objtype *obj)
{
	spritetabletype id0_far *sprite;

	sprite=&spritetable[obj->baseshape+obj->curframe-STARTSPRITES];

	return((sprite->xh-sprite->xl+(1<<G_P_SHIFT)) >> 1);
}
#endif

#if 0
//--------------------------------------------------------------------------
// visible_on()
//--------------------------------------------------------------------------
id0_boolean_t visible_on(objtype *obj)
{
	if (!(obj->flags & of_visible))
	{
		obj->needtoreact=true;
		obj->flags |= of_visible;
		return(true);
	}

	return(false);
}

//--------------------------------------------------------------------------
// visible_off()
//--------------------------------------------------------------------------
id0_boolean_t visible_off(objtype *obj)
{
	if (obj->flags & of_visible)
	{
		obj->needtoreact=true;
		obj->flags &= ~of_visible;
		return(true);
	}

	return(false);
}
#endif


/*
===================
=
= FizzleFade
=
===================
*/

#define PIXPERFRAME     10000	//1600

void FizzleFade (id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t width,id0_unsigned_t height, id0_boolean_t abortable)
{
	id0_unsigned_t        drawofs,pagedelta;
	id0_unsigned_char_t   maskb[8] = {1,2,4,8,16,32,64,128};
	id0_unsigned_t        x,y,p/*,frame*/;
	id0_longword_t        rndval; // REFKEEN Now unsigned (so right shifts are well-defined)
	//id0_long_t            rndval;
	ScanCode			 lastLastScan=LastScan=0;

	width--;
	height--;

	pagedelta = dest-source;
//	VW_SetScreen (dest,0);
	rndval = 1;
	y = 0;

#if 0
asm     mov     es,[screenseg]
asm     mov     dx,SC_INDEX
asm     mov     al,SC_MAPMASK
asm     out     dx,al
#endif

	//frame=0;
	SD_SetTimeCount(0);
	//TimeCount=frame=0;
	do      // while (1)
	{
		if ((abortable) || (Flags & FL_QUICK))
		{
			IN_ReadControl(0,&control);
			if (control.button0 || control.button1 || (lastLastScan != LastScan)
			|| Keyboard[sc_Escape] || (Flags & FL_QUICK))
			{
				VW_ScreenToScreen (source,dest,(width+1)/8,height+1);
				goto exitfunc;
			}
		}

		for (p=0;p<PIXPERFRAME;p++)
		{
			//
			// seperate random value into x/y pair
			//
			y = (rndval-1)&0xFF; // low 8 bits - 1 = y xoordinate
			x = (rndval&0x1FF00)>>8; // next 9 bits = x xoordinate
#if 0
			asm     mov     ax,[WORD PTR rndval]
			asm     mov     dx,[WORD PTR rndval+2]
			asm     mov     bx,ax
			asm     dec     bl
			asm     mov     [BYTE PTR y],bl                 // low 8 bits - 1 = y xoordinate
			asm     mov     bx,ax
			asm     mov     cx,dx
			asm     shr     cx,1
			asm     rcr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     shr     bx,1
			asm     mov     [x],bx                                  // next 9 bits = x xoordinate
#endif
			//
			// advance to next random element
			//
			if (rndval & 1)
			{
				rndval = (rndval >> 1) ^ 0x00012000;
			}
			else
			{
				rndval >>= 1;
			}
#if 0
			asm     shr     dx,1
			asm     rcr     ax,1
			asm     jnc     noxor
			asm     xor     dx,0x0001
			asm     xor     ax,0x2000
noxor:
			asm     mov     [WORD PTR rndval],ax
			asm     mov     [WORD PTR rndval+2],dx
#endif

			if (x>width || y>height)
				continue;
			drawofs = source+ylookup[y];

			// PORTING FROM ASM:
			// - maskb[x&7] picks pixel(s) to update out of 8 in byte
			// - Source offset is drawofs+(x>>3), dest is source_offset+pagedelta
			// - The byte is updated for each plane separately
			//
			// In ported code we update all planes at once

			BE_ST_EGAUpdateGFXBitsScrToScr((drawofs+(x>>3))+pagedelta, drawofs+(x>>3), maskb[x&7]);
#if 0
			asm     mov     cx,[x]
			asm     mov     si,cx
			asm     and     si,7
			asm     mov dx,GC_INDEX
			asm     mov al,GC_BITMASK
			asm     mov     ah,BYTE PTR [maskb+si]
			asm     out dx,ax

			asm     mov     si,[drawofs]
			asm     shr     cx,1
			asm     shr     cx,1
			asm     shr     cx,1
			asm     add     si,cx
			asm     mov     di,si
			asm     add     di,[pagedelta]

			asm     mov     dx,GC_INDEX
			asm     mov     al,GC_READMAP                   // leave GC_INDEX set to READMAP
			asm     out     dx,al

			asm     mov     dx,SC_INDEX+1
			asm     mov     al,1
			asm     out     dx,al
			asm     mov     dx,GC_INDEX+1
			asm     mov     al,0
			asm     out     dx,al

			asm     mov     bl,[es:si]
			asm     xchg [es:di],bl

			asm     mov     dx,SC_INDEX+1
			asm     mov     al,2
			asm     out     dx,al
			asm     mov     dx,GC_INDEX+1
			asm     mov     al,1
			asm     out     dx,al

			asm     mov     bl,[es:si]
			asm     xchg [es:di],bl

			asm     mov     dx,SC_INDEX+1
			asm     mov     al,4
			asm     out     dx,al
			asm     mov     dx,GC_INDEX+1
			asm     mov     al,2
			asm     out     dx,al

			asm     mov     bl,[es:si]
			asm     xchg [es:di],bl

			asm     mov     dx,SC_INDEX+1
			asm     mov     al,8
			asm     out     dx,al
			asm     mov     dx,GC_INDEX+1
			asm     mov     al,3
			asm     out     dx,al

			asm     mov     bl,[es:si]
			asm     xchg [es:di],bl
#endif

			if (rndval == 1)                // entire sequence has been completed
				goto exitfunc;
		}
		// REFKEEN - Code is commented out as in the original sources
		// (of C4, not C3), but better have *some* delay at the least
		BE_ST_TimeCountWaitFromSrc(SD_GetTimeCount(), 1);
		// REFKEEN - No need to increment frame, comment it out as well
#if 0
		frame++;
		while (TimeCount<frame)         // don't go too fast
		;
#endif
	} while (1);

exitfunc:;
	EGABITMASK(255);
	EGAMAPMASK(15);
	return;
}

// (REFKEEN) UNUSED FUNCTION (Was probably written for monochrome displays)

#if 0
//-------------------------------------------------------------------------
// mprintf()
//-------------------------------------------------------------------------
void mprintf(id0_char_t *msg, ...)
{
	static id0_char_t x=0;
	static id0_char_t y=0;
	static id0_char_t id0_far *video = MK_FP(0xb000,0x0000);
	id0_char_t buffer[100],*ptr;

	va_list(ap);

	va_start(ap,msg);

	vsprintf(buffer,msg,ap);

	ptr = buffer;
	while (*ptr)
	{
		switch (*ptr)
		{
			case '\n':
				if (y >= 23)
				{
					video -= (x<<1);
					_fmemcpy(MK_FP(0xb000,0x0000),MK_FP(0xb000,0x00a0),3840);
				}
				else
				{
					y++;
					video += ((80-x)<<1);
				}
				x=0;
			break;

			default:
				*video = *ptr;
				video[1] = 15;
				video += 2;
				x++;
			break;
		}
		ptr++;
	}

	va_end(ap);
}
#endif

#if 0

//--------------------------------------------------------------------------
//								 FULL SCREEN REFRESH/ANIM MANAGERS
//--------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//
//  InitLatchRefresh() -- Loads an ILBM (JAMPAK'd) into the Master Latch
//								  to be used as the background refresh screen...
//
void InitLatchRefresh(id0_char_t *filename)
{
	struct Shape RefreshShp;
	id0_short_t yofs;

	VW_ClearVideo(0);

	if (LoadShape(filename,&RefreshShp))
		TrashProg("Can't load %s",filename);

	VW_SetLineWidth(RefreshShp.BPR);

	yofs = masterswap/SCREENWIDTH;
	MoveGfxDst(0,yofs);								// Handle title screen
	UnpackEGAShapeToScreen(&RefreshShp,0,0);
	FreeShape(&RefreshShp);

	MoveScreen(0,0);
	VW_InitDoubleBuffer();

	RF_NewPosition(0,0);

	RF_Refresh();

	SetUpdateBlock(0,0,RefreshShp.bmHdr.w,RefreshShp.bmHdr.h,3);
}


////////////////////////////////////////////////////////////////////////////
//
// InitFullScreenAnim() -- Initialize ALL necessary functions for full screen
//					  				animation types.
//
// filename - filename for background lbm.
// SpawnAll - spawn function to call to spawn all inital objects..
//
void InitFullScreenAnim(id0_char_t *filename, void (*SpawnAll)())
{
	id0_unsigned_t old_flags;

	old_flags = GE_SystemFlags.flags;
	GE_SystemFlags.flags &= ~(GEsf_Tiles | GEsf_Panning | GEsf_RefreshVec);

	CA_ClearMarks();

	RFL_InitSpriteList();
	InitObjArray();

	if (SpawnAll)
		SpawnAll();

	CA_CacheMarks(NULL);

	CalcInactivate();
	CalcSprites();

	InitLatchRefresh(filename);

	RF_ForceRefresh();
	RF_ForceRefresh();

	GE_SystemFlags.flags = old_flags;
}



///////////////////////////////////////////////////////////////////////////
//
// DoFullScreenAnim()	-- a General "Do-Every-Thing" function that
//									displays a full screen animation...
//
// filename - Filename of background lbm
//	SpawnAll	- Function to call to spawn all inital objects (REQUIRED)
//	CheckKey - Function to call every cycle - The function called must
//				  return a value of greater than zero (0) to terminate the
//				  animation cycle.									  (REQUIRED)
//	CleanUp	- Function to call upon exiting the animation. (OPTIONAL)
//
void DoFullScreenAnim(id0_char_t *filename, void (*SpawnAll)(), id0_short_t (*CheckKey)(),void (*CleanUp)())
{
	id0_unsigned_t old_flags;
	id0_boolean_t ExitAnim = false;

	// Save off the current system flags....

	old_flags = GE_SystemFlags.flags;
	GE_SystemFlags.flags &= ~(GEsf_Tiles | GEsf_Panning);

//	randomize();

	// Init refresh latch screen, initalize all object, and setup video mode.

	InitFullScreenAnim(filename,SpawnAll);

	VW_FadeIn();

	while (!ExitAnim)
	{
		CalcInactivate();
		CalcSprites();
		RF_Refresh();

		ExitAnim = (id0_boolean_t)CheckKey();
	}

//	RemoveBOBList(player);
//	CalcInactivate();

	if (CleanUp)
		CleanUp();

	// Restore old system flags...

	GE_SystemFlags.flags = old_flags;
}

#endif

//--------------------------------------------------------------------------
// FindFile()
//--------------------------------------------------------------------------
id0_boolean_t FindFile(const id0_char_t *filename,const id0_char_t *disktext,id0_char_t disknum)
{
	extern id0_boolean_t splitscreen;

	id0_char_t command[100];
	id0_char_t choices[]={sc_Escape,sc_Space,0},drive[2];
	// (REFKEEN) rt_code should *not* be a boolean before returning
	id0_boolean_t fadeitout=false/*,rt_code=2*/;
	int rt_code = 2;

	if (!disktext)
		disktext = GAMENAME;
	// REFKEEN - Let's just call the drive 'A' (floppy disk)
	drive[0] = 'A';
	//drive[0] = getdisk() + 'A';
	drive[1] = 0;
	while (rt_code == 2)
	{
		if (Verify(filename))
			rt_code = true;
		else
		{
			if (ge_textmode)
			{
				BE_ST_clrscr();
				BE_ST_MoveTextCursorTo(0, 0); // gotoxy(1,1);
				// (REFKEEN) Because our printf is simplified... (also re-using command buffer as in gfx case)
				strcpy(command,"\nInsert ");
				strcat(command,disktext);
				strcat(command," disk ");
				BE_Cross_itoa_dec(disknum,command+strlen(command));
				strcat(command," into drive ");
				strcat(command,drive);
				strcat(command,".");
				BE_ST_puts(command);
				BE_ST_puts("Press SPACE to continue, ESC to abort.\n");
				//printf("\nInsert %s disk %d into drive %s.\n",disktext,disknum,drive);
				//printf("Press SPACE to continue, ESC to abort.\n");
			}
			else
			{
				if (splitscreen)
				{
					bufferofs = displayofs = screenloc[screenpage];
					CenterWindow(38,5);
				}
				else
				{
					bufferofs = displayofs = 0;
					US_CenterWindow(38,5);
				}

				strcpy(command,"\nInsert ");
				strcat(command,disktext);
				strcat(command," disk");
				if (disknum != -1)
				{
					strcat(command," ");
					BE_Cross_itoa_dec(disknum,command+strlen(command));
					//itoa(disknum,command+strlen(command),10);
				}
				strcat(command," into drive ");
				strcat(command,drive);
				strcat(command,".");
				US_CPrint(command);
				US_CPrint("Press SPACE to continue, ESC to abort.");
			}

			BE_ST_BSound(300);
			//sound(300);
			VW_WaitVBL(20);
			BE_ST_BNoSound();
			//nosound();

			if (!ge_textmode)
			{
				if (screenfaded)
				{
					VW_FadeIn();
					fadeitout=true;
				}
			}
			if (GetKeyChoice(choices,true) == sc_Escape)
				rt_code = false;
		}
	}

	if (!ge_textmode)
		if (fadeitout)
			VW_FadeOut();

	return(rt_code);
}

#if 0
//--------------------------------------------------------------------------
// CacheAll()
//--------------------------------------------------------------------------
void CacheAV(id0_char_t *title)
{
	if (Verify("EGAGRAPH."EXT))
	{
		CA_CacheMarks(title);
		if (!FindFile("EGAGRAPH."EXT,AUDIO_DISK))
			TrashProg("Can't find graphics files.");

// cache in audio

		current_disk = AUDIO_DISK;
	}
	else
	{

// cache in audio

		if (!FindFile("EGAGRAPH."EXT,VIDEO_DISK))
			TrashProg("Can't find audio files.");
		CA_CacheMarks(title);

		current_disk = VIDEO_DISK;
	}
}

#endif














#if 0

/////////////////////////////////////////////////////////////////////////////
//
//  GE_DecompressToRAM() -- This Decompression routine uses normal memory
//									 allocation conventions..
//
id0_unsigned_char_t id0_huge *GE_DecompressToRAM(id0_char_t *SourceFile, id0_unsigned_long_t *DataSize)  /* Just the reverse of Encode(). */
{
	FILE *infile;  /* input & output files */

	id0_unsigned_long_t i, j, k, r, c;
	id0_unsigned_char_t id0_huge *DataPtr;
	id0_unsigned_char_t id0_huge *CurPtr;
	id0_unsigned_char_t Buffer[8];
	id0_unsigned_long_t DstLen;

	if (!(infile = fopen(SourceFile, "rb")))
		return(0);

	// Read Header....

	fread(Buffer,1,4,infile);

	if (strncmp(Buffer,COMP,4))
	{
		fclose(infile);
		printf("NOT a JAM Compressed FILE!\n");
		return(0);
	}

	fread((void *)&DstLen,1,4,infile);

	if (!(DataPtr = farmalloc(DstLen)))
		 return(0);

	 fclose(infile);

	*DataSize = DstLen;

	DecompressToRAMLocation(SourceFile,(id0_unsigned_char_t id0_far *)DataPtr);

	return(DataPtr);
}

#endif

// (REFKEEN) Used for patching version-specific stuff
const char *refkeen_compat_gelib_gamename;
const char *refkeen_compat_gelib_version;
const char *refkeen_compat_gelib_revision;
size_t refkeen_compat_gelib_gamename_strbufflen;
char refkeen_compat_gelib_c4_debug_str_with_gamename[20];
char refkeen_compat_gelib_str_with_gamename[32];

void RefKeen_Patch_gelib(void)
{
	switch (refkeen_current_gamever)
	{
	case BE_GAMEVER_CATABYSS113:
		refkeen_compat_gelib_gamename = "CATACOMB ABYSS  ";
		refkeen_compat_gelib_version = "V1.13   ";
		refkeen_compat_gelib_revision = "QA [0]";
		break;
	case BE_GAMEVER_CATABYSS124:
		refkeen_compat_gelib_gamename = "CATACOMB ABYSS 3-D";
		refkeen_compat_gelib_version = "V1.24   ";
		refkeen_compat_gelib_revision = "1";
		break;
	}
	refkeen_compat_gelib_gamename_strbufflen = 1+strlen(refkeen_compat_gelib_gamename);
	// Make this sanity checks, because the size of the ID array was originally sizeof(GAMENAME)
	if (sizeof(ID) < refkeen_compat_gelib_gamename_strbufflen)
		// Don't use Quit; It may fail to work as expected.
		BE_ST_ExitWithErrorMsg("RefKeen_Patch_gelib - ID string buffer is too small.");

	if ((snprintf(refkeen_compat_gelib_c4_debug_str_with_gamename, sizeof(refkeen_compat_gelib_c4_debug_str_with_gamename), "\n%s", refkeen_compat_gelib_gamename) >= (int)sizeof(refkeen_compat_gelib_c4_debug_str_with_gamename)) ||
	    (snprintf(refkeen_compat_gelib_str_with_gamename, sizeof(refkeen_compat_gelib_str_with_gamename), "That isn't a %s", refkeen_compat_gelib_gamename) >= (int)sizeof(refkeen_compat_gelib_str_with_gamename))
	)
		BE_ST_ExitWithErrorMsg("RefKeen_Patch_gelib - String buffer containing GAMENAME is too small.");
}
