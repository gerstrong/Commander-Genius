/* Catacomb Armageddon Source Code
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
// REFKEEN - Unused
//#include "sl_file.h"


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
id0_char_t Filename[FILENAME_LEN+1], ID[sizeof(GAMENAME)], VER[sizeof(SAVEVER_DATA)];
id0_short_t wall_anim_delay,wall_anim_time = 7;
BufferedIO lzwBIO;





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
	while (vbls--)
	{
		VW_WaitVBL(1);
		IN_ReadControl(0,&control);
		if ((control.button0|control.button1)||(Keyboard[key]))
			break;
	}
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
//	id0_char_t Filename[FILENAME_LEN+1]; //, ID[sizeof(GAMENAME)], VER[sizeof(SAVEVER_DATA)];
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
		getdfree(getdisk()+1,&dfree);
		davail = (id0_long_t)dfree.df_avail*(id0_long_t)dfree.df_bsec*(id0_long_t)dfree.df_sclus;
		if (davail < 10000)
		{
			US_CenterWindow(22,4);
			US_Print("\n");
			US_CPrintLine("Disk Full: Can't save game.", NULL);
			US_CPrintLine("Try inserting another disk.", NULL);
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
		if ((BE_Cross_writeInt8LEBuffer(handle, GAMENAME, sizeof(GAMENAME)) != sizeof(GAMENAME)) ||  (BE_Cross_writeInt8LEBuffer(handle, SAVEVER_DATA, sizeof(SAVEVER_DATA)) != sizeof(SAVEVER_DATA)))
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
	memset(ID,0,sizeof(ID));
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
		if ((!CA_FarRead(handle,(id0_byte_t id0_far *)&ID,sizeof(ID))) || (!CA_FarRead(handle,(id0_byte_t id0_far *)&VER,sizeof(VER))))
		{
			BE_Cross_close(handle); // REFKEEN - Avoid resource leak and implementation-defined behaviors
			return(false);
		}

		if ((strcmp(ID,GAMENAME)) || (strcmp(VER,SAVEVER_DATA)))
		{
			US_CenterWindow(32,4);
			US_CPrintLine("That isn't a "GAMENAME, NULL);
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
#pragma	warn	-par
#pragma	warn	-rch
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
#pragma	warn	+par
#pragma	warn	+rch

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

#pragma warn -pia

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

#pragma warn +pia

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

#pragma warn -pia

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

#pragma warn +pia

}


/////////////////////////////////////////////////////////////////////////////
//
// RemoveBOBList() - Removes an entire BOBList attached to an object.
//
//
void RemoveBOBList(objtype *obj)
{
	struct BOB_Shape *CurBOBShape;

#pragma warn -pia

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

#pragma warn +pia

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


#if 0

////////////////////////////////////////////////////////////////////////////
//
// LoadShape()
//
id0_int_t LoadShape(id0_char_t *Filename,struct Shape *SHP)
{
	#define CHUNK(Name)	(*ptr == *Name) &&			\
								(*(ptr+1) == *(Name+1)) &&	\
								(*(ptr+2) == *(Name+2)) &&	\
								(*(ptr+3) == *(Name+3))


	id0_int_t RT_CODE;
//	struct ffblk ffblk;
	FILE *fp;
	id0_char_t CHUNK[5];
	id0_char_t far *ptr;
	memptr IFFfile = NULL;
	id0_unsigned_long_t FileLen, size, ChunkLen;
	id0_int_t loop;


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

	FileLen = *(id0_long_t far *)ptr;
	SwapLong((id0_long_t far *)&FileLen);
	ptr += 4;

	if (!CHUNK("ILBM"))
		goto EXIT_FUNC;
	ptr += 4;

	FileLen += 4;
	while (FileLen)
	{
		ChunkLen = *(id0_long_t far *)(ptr+4);
		SwapLong((id0_long_t far *)&ChunkLen);
		ChunkLen = (ChunkLen+1) & 0xFFFFFFFE;

		if (CHUNK("BMHD"))
		{
			ptr += 8;
			SHP->bmHdr.w = ((struct BitMapHeader far *)ptr)->w;
			SHP->bmHdr.h = ((struct BitMapHeader far *)ptr)->h;
			SHP->bmHdr.x = ((struct BitMapHeader far *)ptr)->x;
			SHP->bmHdr.y = ((struct BitMapHeader far *)ptr)->y;
			SHP->bmHdr.d = ((struct BitMapHeader far *)ptr)->d;
			SHP->bmHdr.trans = ((struct BitMapHeader far *)ptr)->trans;
			SHP->bmHdr.comp = ((struct BitMapHeader far *)ptr)->comp;
			SHP->bmHdr.pad = ((struct BitMapHeader far *)ptr)->pad;
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
			size = *((id0_long_t far *)ptr);
			ptr += 4;
			SwapLong((id0_long_t far *)&size);
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

#if 0
////////////////////////////////////////////////////////////////////////////
//
// CenterObj()
//
void CenterObj(objtype *obj, id0_unsigned_t x, id0_unsigned_t y)
{
	spritetabletype far *sprite;
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

#if 0
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
#endif

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
	spritetabletype far *sprite;

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
	spritetabletype far *sprite;

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

#define PIXPERFRAME     10000

void FizzleFade (id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t width,id0_unsigned_t height, id0_boolean_t abortable)
{
	id0_unsigned_t        drawofs,pagedelta;
	id0_unsigned_char_t maskb[8] = {1,2,4,8,16,32,64,128};
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
//		frame++;
//		while (TimeCount<frame);         // don't go too fast

	} while (1);

exitfunc:;
	EGABITMASK(255);
	EGAMAPMASK(15);
	return;
}

#if 0
//-------------------------------------------------------------------------
// mprintf()
//-------------------------------------------------------------------------
void mprintf(id0_char_t *msg, ...)
{
	static id0_char_t x=0;
	static id0_char_t y=0;
	static id0_char_t far *video = MK_FP(0xb000,0x0000);
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

#ifdef TEXT_PRESENTER
//--------------------------------------------------------------------------
//
//                         TEXT PRESENTER CODE
//
//--------------------------------------------------------------------------

typedef enum pi_stype {pis_pic2x,pis_latch_pic} pi_stype;


typedef struct {						// 4 bytes
	id0_unsigned_t shapenum;
	pi_stype shape_type;
} pi_shape_info;

#define pia_active	0x01

typedef struct {						// 10 bytes
	id0_char_t baseshape;
	id0_char_t frame;
	id0_char_t maxframes;
	id0_short_t delay;
	id0_short_t maxdelay;
	id0_short_t x,y;
} pi_anim_info;

	#define PI_CASE_SENSITIVE

	#define PI_RETURN_CHAR			'\n'
	#define PI_CONTROL_CHAR			'^'

	#define PI_CNVT_CODE(c1,c2)	((c1)|(c2<<8))

// shape table provides a way for the presenter to access and
// display any shape.
//
pi_shape_info id0_far pi_shape_table[] = {

											{BOLTOBJPIC,pis_pic2x},				// 0
											{NUKEOBJPIC,pis_pic2x},
											{SKELETON_1PIC,pis_pic2x},
											{EYE_WALK1PIC,pis_pic2x},
											{ZOMB_WALK1PIC,pis_pic2x},

											{TIMEOBJ1PIC,pis_pic2x},			// 5
											{POTIONOBJPIC,pis_pic2x},
											{RKEYOBJPIC,pis_pic2x},
											{YKEYOBJPIC,pis_pic2x},
											{GKEYOBJPIC,pis_pic2x},

											{BKEYOBJPIC,pis_pic2x},				// 10
											{RGEM1PIC,pis_pic2x},
											{GGEM1PIC,pis_pic2x},
											{BGEM1PIC,pis_pic2x},
											{YGEM1PIC,pis_pic2x},

											{PGEM1PIC,pis_pic2x},				// 15
											{CHESTOBJPIC,pis_pic2x},
											{PSHOT1PIC,pis_pic2x},
											{RED_DEMON1PIC,pis_pic2x},
											{MAGE1PIC,pis_pic2x},

											{BAT1PIC,pis_pic2x},					// 20
											{GREL1PIC,pis_pic2x},
											{GODESS_WALK1PIC,pis_pic2x},
											{ANT_WALK1PIC,pis_pic2x},
											{FATDEMON_WALK1PIC,pis_pic2x},

											{SUCCUBUS_WALK1PIC,pis_pic2x},	//25
											{TREE_WALK1PIC,pis_pic2x},
											{DRAGON_WALK1PIC,pis_pic2x},
											{BUNNY_LEFT1PIC,pis_pic2x},

};

// anim table holds info about each different animation.
//
pi_anim_info id0_far pi_anim_table[] = {{0,0,3,0,10},		// 0		BOLT
												{1,0,3,0,10},     //       NUKE
												{2,0,4,0,10},     //       SKELETON
												{3,0,3,0,10},     //       EYE
												{4,0,3,0,10},     //       ZOMBIE

												{5,0,2,0,10},     // 5     FREEZE TIME
												{11,0,2,0,10},    //			RED GEM
												{12,0,2,0,10},    //       GREEN GEM
												{13,0,2,0,10},    //       BLUE GEM
												{14,0,2,0,10},    //       YELLOW GEM

												{15,0,2,0,10},    // 10    PURPLE GEM
												{17,0,2,0,10},    //       PLAYER'S SHOT
												{18,0,3,0,10},    //       RED DEMON
												{19,0,2,0,10},    //       MAGE
												{20,0,4,0,10},    //       BAT

												{21,0,2,0,10},    // 15    GRELMINAR
												{22,0,3,0,10},    //       GODESS
												{23,0,3,0,10},    //       ANT
												{24,0,4,0,10},    //       FAT DEMON
												{25,0,4,0,10},    //       SUCCUBUS

												{26,0,2,0,10},    // 20    TREE
												{27,0,3,0,10},    //       DRAGON
												{28,0,2,0,10},    //       BUNNY
};

// anim list is created on the fly from the anim table...
// this allows a single animation to be display in more than
// one place...
//
pi_anim_info id0_far pi_anim_list[PI_MAX_ANIMS];
id0_boolean_t pi_recursing=false;

//--------------------------------------------------------------------------
// Presenter() - DANGEROUS DAVE "Presenter()" is more up-to-date than this.
//
//
// Active control codes:
//
//  ^CE				- center text between 'left margin' and 'right margin'
//  ^FCn				- set font color
//  ^LMnnn			- set left margin (if 'nnn' == "fff" uses current x)
//  ^RMnnn			- set right margin (if 'nnn' == "fff" uses current x)
//  ^EP				- end of page (waits for up/down arrow)
//  ^PXnnn			- move x to coordinate 'n'
//  ^PYnnn			- move y to coordinate 'n'
//  ^XX				- exit presenter
//  ^LJ				- left justify  --\ ^RJ doesn't handle imbedded control
//  ^RJ				- right justify --/ codes properly. Use with caution.
//  ^BGn	 			- set background color
//  ^ANnn			- define animation
//  ^SHnnn			- display shape 'n' at current x,y
//
//
// Future control codes:
//
//  ^OBnnn			- activate object 'n'
//  ^FL				- flush to edges (whatever it's called)
//
//
// Other info:
//
// All 'n' values are hex numbers (0 - f), case insensitive.
// The number of N's listed is the number of digits REQUIRED by that control
// code. (IE: ^LMnnn MUST have 3 values! --> 003, 1a2, 01f, etc...)
//
// If a line consists only of control codes, the cursor is NOT advanced
// to the next line (the ending <CR><LF> is skipped). If just ONE non-control
// code is added, the number "8" for example, then the "8" is displayed
// and the cursor is advanced to the next line.
//
// ^CE must be on the same line as the text it should center!
//
//--------------------------------------------------------------------------
void Presenter(PresenterInfo *pi)
{
#ifdef AMIGA
	XBitMap **font = pi->font;

	#define ch_width(ch) font[ch]->pad
	id0_char_t font_height = font[0]->Rows;
#else
	fontstruct id0_seg *font = (fontstruct id0_seg *)grsegs[STARTFONT];

	#define MAX_PB 150
	#define ch_width(ch) font->width[ch]
	id0_char_t font_height = font->height-1;		// "-1" squeezes font vertically
	id0_char_t pb[MAX_PB];
	id0_short_t length;
#endif

	enum {jm_left,jm_right,jm_flush};
	id0_char_t justify_mode = jm_left;
	id0_boolean_t centering=false;

	id0_short_t bgcolor = pi->bgcolor;
	id0_short_t xl=pi->xl,yl=pi->yl,xh=pi->xh,yh=pi->yh;
	id0_short_t cur_x = xl, cur_y = yl;
	id0_char_t id0_far *first_ch = pi->script[0];

	id0_char_t id0_far *scan_ch,temp;
	id0_short_t scan_x,PageNum=0,numanims=0;
	id0_boolean_t up_released=true,dn_released=true;
	id0_boolean_t presenting=true,start_of_line=true;

// if set background is first thing in file, make sure initial screen
// clear uses this color.
//
	if (!_fstrncmp(first_ch,"^BG",3))
		bgcolor = PI_VALUE(first_ch+3,1);

	if (!pi_recursing)
	{
		PurgeAllGfx();
		CachePage(first_ch);
	}
	VW_Bar(xl,yl,xh-xl+1,yh-yl+1,bgcolor);

	while (presenting)
	{
//
// HANDLE WORD-WRAPPING TEXT
//
		if (*first_ch != PI_CONTROL_CHAR)
		{
			start_of_line = false;

	// Parse script until one of the following:
	//
	// 1) text extends beyond right margin
	// 2) NULL termination is reached
	// 3) PI_RETURN_CHAR is reached
	// 4) PI_CONTROL_CHAR is reached
	//
			scan_x = cur_x;
			scan_ch = first_ch;
			while ((scan_x+ch_width(*scan_ch) <= xh) && (*scan_ch) &&
					 (*scan_ch != PI_RETURN_CHAR) && (*scan_ch != PI_CONTROL_CHAR))
				scan_x += ch_width(*scan_ch++);

	// if 'text extends beyond right margin', scan backwards for
	// a SPACE
	//
			if (scan_x+ch_width(*scan_ch) > xh)
			{
				id0_short_t last_x = scan_x;
				id0_char_t id0_far *last_ch = scan_ch;

				while ((scan_ch != first_ch) && (*scan_ch != ' ') && (*scan_ch != PI_RETURN_CHAR))
					scan_x -= ch_width(*scan_ch--);

				if (scan_ch == first_ch)
					scan_ch = last_ch, scan_x = last_x;
			}

	// print current line
	//
#ifdef AMIGA
			while (first_ch < scan_ch)
			{
				qBlit(font[*first_ch++],pi->dst,cur_x,cur_y);
//				qBlit(font[*first_ch],pi->dst,cur_x,cur_y);
//				cur_x += ch_width(*first_ch++);
			}
#else
			temp = *scan_ch;
			*scan_ch = 0;

			if ((justify_mode == jm_right)&&(!centering))
			{
				id0_unsigned_t width,height;

				VWL_MeasureString(first_ch,&width,&height,font);
				cur_x = xh-width;
				if (cur_x < xl)
					cur_x = xl;
			}

			px = cur_x;
			py = cur_y;

			length = scan_ch-first_ch+1;		// USL_DrawString only works with
			if (length > MAX_PB)
				Quit("Presenter(): Print buffer string too long!");
			_fmemcpy(pb,first_ch,length);    // near pointers...

			if (*first_ch != '\n')
				USL_DrawString(pb);

			*scan_ch = temp;
			first_ch = scan_ch;
#endif
			cur_x = scan_x;
			centering = false;

	// skip SPACES / RETURNS at end of line
	//
			if ((*first_ch==' ') || (*first_ch==PI_RETURN_CHAR))
				first_ch++;

	// PI_CONTROL_CHARs don't advance to next character line
	//
			if (*scan_ch != PI_CONTROL_CHAR)
			{
				cur_x = xl;
				cur_y += font_height;
			}
		}
		else
//
// HANDLE CONTROL CODES
//
		{
			PresenterInfo endmsg;
			pi_anim_info id0_far *anim;
			pi_shape_info id0_far *shape_info;
			id0_unsigned_t shapenum;
			id0_short_t length;
			id0_char_t id0_far *s;

			if (first_ch[-1] == '\n')
				start_of_line = true;

			first_ch++;
#ifndef PI_CASE_SENSITIVE
			*first_ch=BE_Cross_toupper(*first_ch);
			*(first_ch+1)=BE_Cross_toupper(*(first_ch+1));
#endif
			switch (*((id0_unsigned_t id0_far *)first_ch)++)
			{
		// CENTER TEXT ------------------------------------------------------
		//
				case PI_CNVT_CODE('C','E'):
					length = 0;
					s = first_ch;
					while (*s && (*s != PI_RETURN_CHAR))
					{
						switch (*s)
						{
							case PI_CONTROL_CHAR:
								s++;
								switch (*((id0_unsigned_t *)s)++)
								{
#ifndef AMIGA
									case PI_CNVT_CODE('F','C'):
									case PI_CNVT_CODE('B','G'):
										s++;
									break;
#endif

									case PI_CNVT_CODE('L','M'):
									case PI_CNVT_CODE('R','M'):
									case PI_CNVT_CODE('S','H'):
									case PI_CNVT_CODE('P','X'):
									case PI_CNVT_CODE('P','Y'):
										s += 3;
									break;

									case PI_CNVT_CODE('L','J'):
									case PI_CNVT_CODE('R','J'):
										// No parameters to pass over!
									break;
								}
							break;

							default:
								length += ch_width(*s++);
							break;
						}
					}
					cur_x = ((xh-xl+1)-length)/2;
					centering = true;
				break;

		// DRAW SHAPE -------------------------------------------------------
		//
				case PI_CNVT_CODE('S','H'):
					shapenum = PI_VALUE(first_ch,3);
					first_ch += 3;
					shape_info = &pi_shape_table[shapenum];
					switch (shape_info->shape_type)
					{
						id0_short_t width;

						case pis_pic2x:
							cur_x = ((cur_x+2) + 7) & 0xFFF8;
							width=BoxAroundPic(cur_x-2,cur_y-1,shape_info->shapenum,pi);
							VW_DrawPic2x(cur_x>>3,cur_y,shape_info->shapenum);
							cur_x += width;
						break;
					}
				break;

		// INIT ANIMATION ---------------------------------------------------
		//
				case PI_CNVT_CODE('A','N'):
					shapenum = PI_VALUE(first_ch,2);
					first_ch += 2;
					_fmemcpy(&pi_anim_list[numanims],&pi_anim_table[shapenum],sizeof(pi_anim_info));
					anim = &pi_anim_list[numanims++];
					shape_info = &pi_shape_table[anim->baseshape+anim->frame];
					switch (shape_info->shape_type)
					{
						id0_short_t width;

						case pis_pic2x:
							cur_x = ((cur_x+2) + 7) & 0xFFF8;
							width=BoxAroundPic(cur_x-2,cur_y-1,shape_info->shapenum,pi);
							VW_DrawPic2x(cur_x>>3,cur_y,shape_info->shapenum);
							anim->x = cur_x>>3;
							anim->y = cur_y;
							cur_x += width;
						break;
					}
				break;

#ifndef AMIGA
		// FONT COLOR -------------------------------------------------------
		//
				case PI_CNVT_CODE('F','C'):
					fontcolor = bgcolor ^ PI_VALUE(first_ch++,1);
				break;
#endif

		// BACKGROUND COLOR -------------------------------------------------
		//
				case PI_CNVT_CODE('B','G'):
					bgcolor = PI_VALUE(first_ch++,1);
				break;

		// LEFT MARGIN ------------------------------------------------------
		//
				case PI_CNVT_CODE('L','M'):
					shapenum = PI_VALUE(first_ch,3);
					first_ch += 3;
					if (shapenum == 0xfff)
						xl = cur_x;
					else
						xl = shapenum;
				break;

		// RIGHT MARGIN -----------------------------------------------------
		//
				case PI_CNVT_CODE('R','M'):
					shapenum = PI_VALUE(first_ch,3);
					first_ch += 3;
					if (shapenum == 0xfff)
						xh = cur_x;
					else
						xh = shapenum;
				break;

		// SET X COORDINATE -------------------------------------------------
		//
				case PI_CNVT_CODE('P','X'):
					cur_x = PI_VALUE(first_ch,3);
					first_ch += 3;
				break;

		// SET Y COORDINATE -------------------------------------------------
		//
				case PI_CNVT_CODE('P','Y'):
					cur_y = PI_VALUE(first_ch,3);
					first_ch += 3;
				break;

		// LEFT JUSTIFY -----------------------------------------------------
		//
				case PI_CNVT_CODE('L','J'):
					justify_mode = jm_left;
				break;

		// RIGHT JUSTIFY ----------------------------------------------------
		//
				case PI_CNVT_CODE('R','J'):
					justify_mode = jm_right;
				break;

		// END OF PAGE ------------------------------------------------------
		//
				case PI_CNVT_CODE('E','P'):
					if (pi_recursing)
						Quit("Presenter(): Can't use ^EP when recursing!");

					endmsg.xl = cur_x;
					endmsg.yl = yh-(font_height+2);
					endmsg.xh = xh;
					endmsg.yh = yh;
					endmsg.bgcolor = bgcolor;
					endmsg.ltcolor = pi->ltcolor;
					endmsg.dkcolor = pi->dkcolor;
					endmsg.script[0] = (id0_char_t id0_far *)"^CE^FC8-  ^FC0ESC ^FC8to return to play, or ^FC0ARROW KEYS ^FC8to page through more Help  -^XX";

					pi_recursing = true;
					Presenter(&endmsg);
					pi_recursing = false;

#ifndef AMIGA
					if (screenfaded)
						VW_FadeIn();
					VW_ColorBorder(8 | 56);
#endif

					while (1)
					{
#ifndef AMIGA
						id0_long_t newtime;

						VW_WaitVBL(1);
						newtime = SD_GetTimeCount();
						realtics = tics = newtime-lasttimecount;
						lasttimecount = newtime;
#else
						WaitVBL(1);
						CALC_TICS;
#endif
						AnimatePage(numanims);
						IN_ReadControl(0,&control);

						if (control.button1 || Keyboard[1])
						{
							presenting=false;
							break;
						}
						else
						{
							if (ControlTypeUsed != ctrl_Keyboard)
								control.dir = dir_None;

							if (((control.dir == dir_North) || (control.dir == dir_West)) && (PageNum))
							{
								if (up_released)
								{
									PageNum--;
									up_released = false;
									break;
								}
							}
							else
							{
								up_released = true;
								if (((control.dir == dir_South) || (control.dir == dir_East)) && (PageNum < pi->numpages-1))
								{
									if (dn_released)
									{
										PageNum++;
										dn_released = false;
										break;
									}
								}
								else
									dn_released = true;
							}
						}
					}

					cur_x = xl;
					cur_y = yl;
					if (cur_y+font_height > yh)
						cur_y = yh-font_height;
					first_ch = pi->script[PageNum];

					numanims = 0;
					PurgeAllGfx();
					CachePage(first_ch);

					VW_Bar(xl,yl,xh-xl+1,yh-yl+1,bgcolor);
				break;

		// EXIT PRESENTER ---------------------------------------------------
		//
				case PI_CNVT_CODE('X','X'):
					presenting=false;
				break;
			}

			if ((first_ch[0] == ' ') && (first_ch[1] == '\n') && start_of_line)
				first_ch += 2;
		}
	}
}

//--------------------------------------------------------------------------
// ResetAnims()
//--------------------------------------------------------------------------
void ResetAnims()
{
	pi_anim_list[0].baseshape = -1;
}

//--------------------------------------------------------------------------
// AnimatePage()
//--------------------------------------------------------------------------
void AnimatePage(id0_short_t numanims)
{
	pi_anim_info id0_far *anim=pi_anim_list;
	pi_shape_info id0_far *shape_info;

	while (numanims--)
	{
		anim->delay += tics;
		if (anim->delay >= anim->maxdelay)
		{
			anim->delay = 0;
			anim->frame++;
			if (anim->frame == anim->maxframes)
				anim->frame = 0;

#if ANIM_USES_SHAPETABLE
			shape_info = &pi_shape_table[anim->baseshape+anim->frame];
#else
			shape_info = &pi_shape_table[anim->baseshape];
#endif
			switch (shape_info->shape_type)
			{
				case pis_pic2x:
#if ANIM_USES_SHAPETABLE
					VW_DrawPic2x(anim->x,anim->y,shape_info->shapenum);
#else
					VW_DrawPic2x(anim->x,anim->y,shape_info->shapenum+anim->frame);
#endif
				break;
			}
		}
		anim++;
	}
}

//--------------------------------------------------------------------------
// BoxAroundPic()
//--------------------------------------------------------------------------
id0_short_t BoxAroundPic(id0_short_t x1, id0_short_t y1, id0_unsigned_t picnum, PresenterInfo *pi)
{
	id0_short_t x2,y2;

	x2 = x1+(pictable[picnum-STARTPICS].width<<4)+2;
	y2 = y1+(pictable[picnum-STARTPICS].height)+1;
	VWB_Hlin(x1,x2,y1,pi->ltcolor);
	VWB_Hlin(x1,x2,y2,pi->dkcolor);
	VWB_Vlin(y1,y2,x1,pi->ltcolor);
	VWB_Vlin(y1,y2,x1+1,pi->ltcolor);
	VWB_Vlin(y1,y2,x2,pi->dkcolor);
	VWB_Vlin(y1,y2,x2+1,pi->dkcolor);

	return(x2-x1+1);
}

//--------------------------------------------------------------------------
// PurgeAllGfx()
//--------------------------------------------------------------------------
void PurgeAllGfx()
{
	ResetAnims();
	FreeUpMemory();
}

//--------------------------------------------------------------------------
// CachePage()
//--------------------------------------------------------------------------
void CachePage(id0_char_t id0_far *script)
{
	pi_anim_info id0_far *anim;
	id0_short_t loop;
	id0_unsigned_t shapenum;
	id0_boolean_t end_of_page=false;
	id0_short_t numanims=0;

	while (!end_of_page)
	{
		switch (*script++)
		{
			case PI_CONTROL_CHAR:
#ifndef PI_CASE_SENSITIVE
				*script=BE_Cross_toupper(*script);
				*(script+1)=BE_Cross_toupper(*(script+1));
#endif
				switch (*((id0_unsigned_t id0_far *)script)++)
				{
					case PI_CNVT_CODE('S','H'):
						shapenum = PI_VALUE(script,3);
						script += 3;
						CA_MarkGrChunk(pi_shape_table[shapenum].shapenum);
					break;

					case PI_CNVT_CODE('A','N'):
						shapenum = PI_VALUE(script,2);
						script += 2;

						if (numanims++ == PI_MAX_ANIMS)
							Quit("CachePage(): Too many anims on one page.");

						anim = &pi_anim_table[shapenum];
#if ANIM_USES_SHAPETABLE
						for (loop=anim->baseshape;loop < anim->baseshape+anim->maxframes; loop++)
							CA_MarkGrChunk(pi_shape_table[loop].shapenum);
#else
						shapenum = pi_shape_table[anim->baseshape].shapenum;
						for (loop=0; loop<anim->maxframes; loop++)
							CA_MarkGrChunk(shapenum+loop);
#endif
					break;

					case PI_CNVT_CODE('X','X'):
					case PI_CNVT_CODE('E','P'):
						end_of_page = true;
					break;
				}
			break;
		}
	}

	CA_CacheMarks(NULL);
}

//--------------------------------------------------------------------------
// PI_VALUE()
//--------------------------------------------------------------------------
id0_unsigned_t PI_VALUE(id0_char_t id0_far *ptr,id0_char_t num_nybbles)
{
	id0_char_t ch,nybble,shift;
	id0_unsigned_t value=0;

	for (nybble=0; nybble<num_nybbles; nybble++)
	{
		shift = 4*(num_nybbles-nybble-1);

		ch = *ptr++;
		if (isxdigit(ch))
			if (isalpha(ch))
				value |= (BE_Cross_toupper(ch)-'A'+10)<<shift;
			else
				value |= (ch-'0')<<shift;
	}

	return(value);
}

//--------------------------------------------------------------------------
// LoadPresenterScript()
//--------------------------------------------------------------------------
id0_long_t LoadPresenterScript(id0_char_t *filename,PresenterInfo *pi)
{
#pragma warn -pia
	id0_long_t size;

	if (!(size=BLoad(filename,&pi->scriptstart)))
		return(0);
	pi->script[0] = MK_FP(pi->scriptstart,0);
	pi->script[0][size-1] = 0;		 			// Last id0_byte_t is trashed!
	InitPresenterScript(pi);

	return(size);
#pragma warn +pia
}

//-------------------------------------------------------------------------
// FreePresenterScript()
//-------------------------------------------------------------------------
void FreePresenterScript(PresenterInfo *pi)
{
	if (pi->script)
		MM_FreePtr(&pi->scriptstart);
}

//-------------------------------------------------------------------------
// InitPresenterScript()
//-------------------------------------------------------------------------
void InitPresenterScript(PresenterInfo *pi)
{
	id0_char_t id0_far *script = pi->script[0];

	pi->numpages = 1;		// Assume at least 1 page
	while (*script)
	{
		switch (*script++)
		{
			case PI_CONTROL_CHAR:
#ifndef PI_CASE_SENSITIVE
				*script=BE_Cross_toupper(*script);
				*(script+1)=BE_Cross_toupper(*(script+1));
#endif
				switch (*((id0_unsigned_t id0_far *)script)++)
				{
					case PI_CNVT_CODE('E','P'):
						if (pi->numpages < PI_MAX_PAGES)
							pi->script[pi->numpages++] = script;
						else
							TrashProg("GE ERROR: Too many Presenter() pages. --> %d",pi->numpages);
					break;
				}
			break;

			case '\r':
				if (*script == '\n')
				{
					*(script-1) = ' ';
					script++;
				}
			break;
		}
	}

	pi->numpages--;	// Last page defined is not a real page.
}
#endif


//-------------------------------------------------------------------------
// AnimateWallList()
//-------------------------------------------------------------------------
void AnimateWallList(void)
{
	walltype *wall/*, *check*/;
	id0_unsigned_t i;
	id0_int_t tile,org_tile;

	if (wall_anim_delay>0)
	{
		wall_anim_delay-=realtics;
		return;
	}

	//
	// Re-Init our counter...
	//

	wall_anim_delay = wall_anim_time;

	//
	// Clear all previous flags marking animation being DONE.
	//

	for (i=0;i<NUMFLOORS;i++)
		TILE_FLAGS(i) &= ~tf_MARKED;


	//
	// Run though wall list updating only then needed animations
	//

	for (wall=&walls[1];wall<rightwall;wall++)
	{
		org_tile = tile = wall->color + wall_anim_pos[wall->color];

		if (ANIM_FLAGS(tile))
		{
			do
			{
				if (!(TILE_FLAGS(tile) & tf_MARKED))
				{
					//
					// update our offset table (0-NUMANIMS)
					//

					wall_anim_pos[tile] += (id0_char_signed_t)ANIM_FLAGS(tile+(id0_char_signed_t)wall_anim_pos[tile]);

					//
					// Mark tile as being already updated..
					//

					TILE_FLAGS(tile) |= tf_MARKED;
				}

				//
				// Check rest of tiles in this animation string...
				//

				tile += (id0_char_signed_t)ANIM_FLAGS(tile);

			} while (tile != org_tile);
		}
	}
}

