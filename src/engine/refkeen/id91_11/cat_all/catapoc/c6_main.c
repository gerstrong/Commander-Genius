/* Catacomb Apocalypse Source Code
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

// C3_MAIN.C

#define CATALOG

#include <time.h>
#include <stdarg.h>

#include "def.h"
#include "gelib.h"
//#pragma hdrstop
//#include <dir.h>

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

typedef enum demo_screens {sc_logo,sc_title,sc_credits1,sc_credits2,sc_credits3,sc_credits4,sc_end} demo_screens;
struct Shape   shape,
					SdLogoShp,
					TitleShp,
					CreditBKShp,
					Credit1Shp,
					Credit2Shp,
					Credit3Shp,
					Credit4Shp,
					Credit5Shp,
					Credit6Shp,
					Credit7Shp,
					Credit8Shp,
					Credit9Shp,
					Credit10Shp;


PresenterInfo MainHelpText;

GameDiff restartgame;
id0_boolean_t loadedgame,abortgame,ingame;


memptr		scalesegs[NUMPICS];
id0_char_t		str[80],str2[20];
id0_unsigned_t	tedlevelnum;
id0_boolean_t		tedlevel;
gametype	gamestate;
exittype	playstate;
id0_char_t	SlowMode = 0;
id0_int_t starting_level;

//id0_short_t NumGames=0; // REFKEEN - Already defined in gelib.c
id0_unsigned_t Flags=0;

id0_boolean_t LoadShapes = true;
id0_boolean_t EASYMODEON = false;

void DisplayIntroText(void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



//===========================================================================

#if 0
// JAB Hack begin
#define	MyInterrupt	0x60
void interrupt (*intaddr)();
void interrupt (*oldintaddr)();
	id0_char_t	*JHParmStrings[] = {"no386",nil};

void
jabhack(void)
{
extern void id0_far jabhack2(void);
extern id0_int_t id0_far	CheckIs386(void);

	id0_int_t	i;

	oldintaddr = getvect(MyInterrupt);

	for (i = 1;i < id0_argc;i++)
		if (US_CheckParm(id0_argv[i],JHParmStrings) == 0)
			return;

	if (CheckIs386())
	{
		jabhack2();
		setvect(MyInterrupt,intaddr);
	}
}

void
jabunhack(void)
{
	setvect(MyInterrupt,oldintaddr);
}
//	JAB Hack end
#endif

//===========================================================================


// REFKEEN - New cross-platform methods for converting pointers
// to sky/ground colors (saved games compatibility)

extern id0_unsigned_t scolor,gcolor;
extern id0_unsigned_t debug_sky,debug_gnd;
extern id0_unsigned_t sky_lightning[6];

id0_unsigned_t GetSkyGndColorDOSPtrFromNativePointer(id0_unsigned_t *colorPtr)
{
	if (colorPtr == &scolor)
		return 0xB79B;
	if (colorPtr == &gcolor)
		return 0xB79D;
	if (colorPtr == &debug_gnd)
		return 0xB9D9;
	if (colorPtr == &debug_sky)
		return 0xB9DB;
	// REFKEEN - Let's assume here the pointer is properly aligned...
	if ((colorPtr >= sky_lightning) && (colorPtr < sky_lightning+sizeof(sky_lightning)/sizeof(*sky_lightning)))
		return 0x18B4+(colorPtr-sky_lightning)*sizeof(*sky_lightning);
	return 0;
}

id0_unsigned_t* GetSkyGndColorPtrFromDOSPointer(id0_unsigned_t dosOffset)
{
	switch (dosOffset)
	{
	case 0xB79B: return &scolor;
	case 0xB79D: return &gcolor;
	case 0xB9D9: return &debug_gnd;
	case 0xB9DB: return &debug_sky;
	default:
	{
		id0_unsigned_t checkOff;
		// sky_daytonight array - unused, so not implemented

		// sky_lightning array
		checkOff = 0x18B4;
		if ((dosOffset >= checkOff) && (dosOffset < checkOff + sizeof(sky_lightning)) && ((dosOffset-checkOff) % sizeof(*sky_lightning) == 0))
		{
			return sky_lightning + (dosOffset-checkOff)/sizeof(*sky_lightning);
		}

		// sky_colors array - unused
		// gnd_colors array - unused
		return NULL;
	}
	}
}

// REFKEEN - New cross-platform methods for reading/writing objects from/to saved games
static id0_boolean_t SaveObject(BE_FILE_T file, objtype *o)
{
	id0_int_t dummy = 0;
	// for active enum (anonymous type)
	id0_int_t activeint = (id0_int_t)(o->active);
	// BACKWARD COMPATIBILITY
	id0_longword_t statedosfarptr = o->state ? o->state->compatdospointer : 0;
	// Just tells if "o->next" is zero or not
	id0_int_t isnext = o->next ? 1 : 0;
	// Now writing
	size_t BE_Cross_write_classtype_To16LE(BE_FILE_T fp, const classtype *ptr);
	size_t BE_Cross_write_dirtype_To16LE(BE_FILE_T fp, const dirtype *ptr);
	return ((BE_Cross_writeInt16LE(file, &o->ticcount) == 2)
	        && (BE_Cross_writeInt32LE(file, &statedosfarptr) == 4) // BACKWARD COMPATIBILITY
	        && (BE_Cross_writeInt32LE(file, &o->x) == 4)
	        && (BE_Cross_writeInt32LE(file, &o->y) == 4)
	        && (BE_Cross_writeInt16LE(file, &o->viewx) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tilex) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tiley) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->viewheight) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->size) == 2)
	        // No need to write prev pointer as-is,
	        // this is ignored on loading. So write dummy value.
	        // Furthermore, all we need to know about next on loading is
	        // if it's zero or not.
	        && (BE_Cross_writeInt16LE(file, &isnext) == 2) // next
	        && (BE_Cross_writeInt8LEBuffer(file, &dummy, 2) == 2) // prev
		//
		&& (BE_Cross_writeInt16LE(file, &activeint) == 2)
	        && (BE_Cross_write_classtype_To16LE(file, &o->obclass) == 2)
	        && (BE_Cross_writeInt8LE(file, &o->flags) == 1)
	        && (BE_Cross_writeInt8LE(file, &dummy) == 1) // Padding due to word alignment in original code
	        && (BE_Cross_writeInt32LE(file, &o->distance) == 4)
	        && (BE_Cross_write_dirtype_To16LE(file, &o->dir) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->angle) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->hitpoints) == 2)
	        && (BE_Cross_writeInt32LE(file, &o->speed) == 4)
	        && (BE_Cross_writeInt32LE(file, &o->xl) == 4)
	        && (BE_Cross_writeInt32LE(file, &o->xh) == 4)
	        && (BE_Cross_writeInt32LE(file, &o->yl) == 4)
	        && (BE_Cross_writeInt32LE(file, &o->yh) == 4)
	        && (BE_Cross_writeInt16LE(file, &o->temp1) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->temp2) == 2)
	);
}

static id0_boolean_t LoadObject(BE_FILE_T file, objtype *o)
{
	id0_int_t dummy;
	// for active enum (anonymous type)
	id0_int_t activeint;
	// BACKWARD COMPATIBILITY
	id0_longword_t statedosfarptr;
	// Just tells if "o->next" is zero or not
	id0_int_t isnext;
	// Now reading
	size_t BE_Cross_read_classtype_From16LE(BE_FILE_T fp, classtype *ptr);
	size_t BE_Cross_read_dirtype_From16LE(BE_FILE_T fp, dirtype *ptr);
	if ((BE_Cross_readInt16LE(file, &o->ticcount) != 2)
	    || (BE_Cross_readInt32LE(file, &statedosfarptr) != 4) // BACKWARD COMPATIBILITY
	    || (BE_Cross_readInt32LE(file, &o->x) != 4)
	    || (BE_Cross_readInt32LE(file, &o->y) != 4)
	    || (BE_Cross_readInt16LE(file, &o->viewx) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tilex) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tiley) != 2)
	    || (BE_Cross_readInt16LE(file, &o->viewheight) != 2)
	    || (BE_Cross_readInt16LE(file, &o->size) != 2)
	    // No need to read prev pointer as-is,
	    // this is ignored on loading. So read dummy value.
	    // Furthermore, all we need to know about next on loading is
	    // if it's zero or not.
	    || (BE_Cross_readInt16LE(file, &isnext) != 2) // next
	    || (BE_Cross_readInt8LEBuffer(file, &dummy, 2) != 2) // prev
	    //
	    || (BE_Cross_readInt16LE(file, &activeint) != 2)
	    || (BE_Cross_read_classtype_From16LE(file, &o->obclass) != 2)
	    || (BE_Cross_readInt8LE(file, &o->flags) != 1)
	    || (BE_Cross_readInt8LE(file, &dummy) != 1) // Padding due to word alignment in original code
	    || (BE_Cross_readInt32LE(file, &o->distance) != 4)
	    || (BE_Cross_read_dirtype_From16LE(file, &o->dir) != 2)
	    || (BE_Cross_readInt16LE(file, &o->angle) != 2)
	    || (BE_Cross_readInt16LE(file, &o->hitpoints) != 2)
	    || (BE_Cross_readInt32LE(file, &o->speed) != 4)
	    || (BE_Cross_readInt32LE(file, &o->xl) != 4)
	    || (BE_Cross_readInt32LE(file, &o->xh) != 4)
	    || (BE_Cross_readInt32LE(file, &o->yl) != 4)
	    || (BE_Cross_readInt32LE(file, &o->yh) != 4)
	    || (BE_Cross_readInt16LE(file, &o->temp1) != 2)
	    || (BE_Cross_readInt16LE(file, &o->temp2) != 2)
	)
	{
		return false;
	}
	o->active = (activetype)activeint;
	o->state = RefKeen_GetObjStatePtrFromDOSPointer(statedosfarptr);
	// HACK: All we need to know is if next was originally NULL or not
	o->next = isnext ? o : NULL;
	return true;
}

// Similar new methods for writing/reading game state
static id0_boolean_t SaveGameState(BE_FILE_T file, gametype *state)
{
	return ((BE_Cross_writeInt16LE(file, &state->difficulty) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->mapon) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->bolts) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->nukes) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->potions) == 2)
	        && (BE_Cross_writeInt16LEBuffer(file, state->keys, sizeof(state->keys)) == sizeof(state->keys))
	        && (BE_Cross_writeInt16LEBuffer(file, state->scrolls, sizeof(state->scrolls)) == sizeof(state->scrolls))
	        && (BE_Cross_writeInt16LEBuffer(file, state->gems, sizeof(state->gems)) == sizeof(state->gems))
	        && (BE_Cross_writeInt32LE(file, &state->score) == 4)
	        && (BE_Cross_writeInt16LE(file, &state->body) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->shotpower) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->mapwidth) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->mapheight) == 2)
	);
}

static id0_boolean_t LoadGameState(BE_FILE_T file, gametype *state)
{
	return ((BE_Cross_readInt16LE(file, &state->difficulty) == 2)
	        && (BE_Cross_readInt16LE(file, &state->mapon) == 2)
	        && (BE_Cross_readInt16LE(file, &state->bolts) == 2)
	        && (BE_Cross_readInt16LE(file, &state->nukes) == 2)
	        && (BE_Cross_readInt16LE(file, &state->potions) == 2)
	        && (BE_Cross_readInt16LEBuffer(file, state->keys, sizeof(state->keys)) == sizeof(state->keys))
	        && (BE_Cross_readInt16LEBuffer(file, state->scrolls, sizeof(state->scrolls)) == sizeof(state->scrolls))
	        && (BE_Cross_readInt16LEBuffer(file, state->gems, sizeof(state->gems)) == sizeof(state->gems))
	        && (BE_Cross_readInt32LE(file, &state->score) == 4)
	        && (BE_Cross_readInt16LE(file, &state->body) == 2)
	        && (BE_Cross_readInt16LE(file, &state->shotpower) == 2)
	        && (BE_Cross_readInt16LE(file, &state->mapwidth) == 2)
	        && (BE_Cross_readInt16LE(file, &state->mapheight) == 2)
	);
}


/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (void)
{
	if (!loadedgame)
	{
		memset (&gamestate,0,sizeof(gamestate));
		gamestate.mapon = starting_level;
		gamestate.body = MAXBODY;
	}

	BGFLAGS = BGF_NOT_LIGHTNING;
	Flags &= FL_CLEAR;

	boltsleft = bolttimer = 0;

//	memset (gamestate.levels,-1,sizeof(gamestate.levels));
}

//===========================================================================

#define RLETAG	0xABCD

/*
==================
=
= SaveTheGame
=
==================
*/

id0_boolean_t	SaveTheGame(BE_FILE_T file)
{
	id0_word_t	i,compressed,expanded;
	objtype	*o;
	memptr	bigbuffer;

	// save the sky and ground colors
	// REFKEEN - But not before converting to original 16-bit pointers (reusing i variable)
	i = GetSkyGndColorDOSPtrFromNativePointer(skycolor);
	if (BE_Cross_writeInt16LE(file, &i) != 2)
	//if (!CA_FarWrite(file,(void id0_far *)&skycolor,sizeof(skycolor)))
		return(false);
	i = GetSkyGndColorDOSPtrFromNativePointer(groundcolor);
	if (BE_Cross_writeInt16LE(file, &i) != 2)
	//if (!CA_FarWrite(file,(void id0_far *)&groundcolor,sizeof(groundcolor)))
		return(false);

	if (BE_Cross_writeInt16LE(file, &FreezeTime) != 2)
	//if (!CA_FarWrite(file,(void id0_far *)&FreezeTime,sizeof(FreezeTime)))
		return(false);

	// (REFKEEN) Writing fields one-by-one in a cross-platform manner
	if (!SaveGameState(file, &gamestate))
	//if (!CA_FarWrite(file,(void id0_far *)&gamestate,sizeof(gamestate)))
		return(false);

	if (BE_Cross_write_boolean_To16LE(file, &EASYMODEON) != 2)
	//if (!CA_FarWrite(file,(void id0_far *)&EASYMODEON,sizeof(EASYMODEON)))
		return(false);

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i+=2)	// Write planes 0 and 2
	{
//
// leave a word at start of compressed data for compressed length
//
		compressed = (id0_unsigned_t)CA_RLEWCompress ((id0_unsigned_t id0_huge *)mapsegs[i]
			,expanded,((id0_unsigned_t id0_huge *)bigbuffer)+1,RLETAG);

		*(id0_unsigned_t id0_huge *)bigbuffer = compressed;

		if (BE_Cross_writeInt16LEBuffer(file, bigbuffer, compressed+2) != (id0_word_t)(compressed+2))
		//if (!CA_FarWrite(file,(id0_byte_t id0_far *)bigbuffer,compressed+2) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}
	}

	for (o = player;o;o = o->next)
		// (REFKEEN) Writing fields one-by-one in a cross-platform manner
		if (!SaveObject(file, o))
		//if (!CA_FarWrite(file,(void id0_far *)o,sizeof(objtype)))
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

	MM_FreePtr (&bigbuffer);

	return(true);
}

//===========================================================================


/*
==================
=
= LoadTheGame
=
==================
*/

id0_boolean_t	LoadTheGame(BE_FILE_T file)
{
	id0_unsigned_t	i,x,y;
	objtype		/**obj,*/*prev,*next,*followed;
	id0_unsigned_t	compressed,expanded;
	id0_unsigned_t	id0_far *map,tile;
	memptr		bigbuffer;

	screenpage = 0;
	FreeUpMemory();

	playstate = ex_loadedgame;
	// load the sky and ground colors
	// REFKEEN - But not before converting from original 16-bit pointers (reusing i variable)
	if (BE_Cross_readInt16LE(file, &i) != 2)
	//if (!CA_FarRead(file,(void id0_far *)&skycolor,sizeof(skycolor)))
		return(false);
	skycolor = GetSkyGndColorPtrFromDOSPointer(i);
	if (BE_Cross_readInt16LE(file, &i) != 2)
	//if (!CA_FarRead(file,(void id0_far *)&groundcolor,sizeof(groundcolor)))
		return(false);
	groundcolor = GetSkyGndColorPtrFromDOSPointer(i);

	if (BE_Cross_readInt16LE(file, &FreezeTime) != 2)
	//if (!CA_FarRead(file,(void id0_far *)&FreezeTime,sizeof(FreezeTime)))
		return(false);

	// (REFKEEN) Reading fields one-by-one in a cross-platform manner
	if (!LoadGameState(file, &gamestate))
	//if (!CA_FarRead(file,(void id0_far *)&gamestate,sizeof(gamestate)))
		return(false);

	if (BE_Cross_read_boolean_From16LE(file, &EASYMODEON) != 2)
	//if (!CA_FarRead(file,(void id0_far *)&EASYMODEON,sizeof(EASYMODEON)))
		return(false);

	SetupGameLevel ();		// load in and cache the base old level

	// (REFKEEN) DIFFERENCE FROM VANILLA CATACOMB ADVENTURES:
	// Don't do this check, we've already opened the file anyway
	// and this can lead to unexpected behaviors!
#if 0
	if (!FindFile(Filename,"SAVE GAME",-1))
		Quit("Error: Can't find saved game file!");
#endif

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i+=2)	// Read planes 0 and 2
	{
		if (BE_Cross_readInt16LE(file, &compressed) != 2)
		//if (!CA_FarRead(file,(id0_byte_t id0_far *)&compressed,sizeof(compressed)) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		if (BE_Cross_readInt16LEBuffer(file, bigbuffer, compressed) != compressed)
		//if (!CA_FarRead(file,(id0_byte_t id0_far *)bigbuffer,compressed) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		CA_RLEWexpand ((id0_unsigned_t id0_huge *)bigbuffer,
			(id0_unsigned_t id0_huge *)mapsegs[i],expanded,RLETAG);
	}

	MM_FreePtr (&bigbuffer);
//
// copy the wall data to a data segment array again, to handle doors and
// bomb walls that are allready opened
//
	memset (tilemap,0,sizeof(tilemap));
	memset (actorat,0,sizeof(actorat));
	map = mapsegs[0];
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *map++;
			if (tile<NUMFLOORS)
			{
				if (tile != INVISIBLEWALL)
					tilemap[x][y] = tile;
				if (tile>0)
					actorat[x][y] = tile;
					//(id0_unsigned_t)actorat[x][y] = tile;
			}
		}


	// Read the object list back in - assumes at least one object in list

	InitObjList ();
	newobj = player;
	while (true)
	{
		prev = newobj->prev;
		next = newobj->next;
		// (REFKEEN) Reading fields one-by-one in a cross-platform manner
		if (!LoadObject(file, newobj))
		//if (!CA_FarRead(file,(void id0_far *)newobj,sizeof(objtype)))
			return(false);
		followed = newobj->next;
		newobj->prev = prev;
		newobj->next = next;
		actorat[newobj->tilex][newobj->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(newobj);	// drop a new marker
		//actorat[newobj->tilex][newobj->tiley] = newobj;	// drop a new marker

		if (followed)
			GetNewObj (false);
		else
			break;
	}

	return(true);
}

//===========================================================================

/*
==================
=
= ResetGame
=
==================
*/

void ResetGame(void)
{
	NewGame ();

	ca_levelnum--;
	ca_levelbit>>=1;
	CA_ClearMarks();
	ca_levelbit<<=1;
	ca_levelnum++;
}

//===========================================================================


/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
  US_Shutdown ();
#ifndef PROFILE
  SD_Shutdown ();
  IN_Shutdown ();
#endif
  VW_Shutdown ();
  CA_Shutdown ();
  MM_Shutdown ();
}


//===========================================================================

/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

void InitGame (void)
{
	//id0_unsigned_t	segstart,seglength;
	id0_int_t			i,x,y;
	id0_unsigned_t	*blockstart;

//	US_TextScreen();

	MM_Startup ();
	VW_Startup ();
#ifndef PROFILE
	IN_Startup ();
	SD_Startup ();
#endif
	US_Startup ();

	CA_Startup ();

	US_Setup ();

	US_SetLoadSaveHooks(LoadTheGame,SaveTheGame,ResetGame);


//
// load in and lock down some basic chunks
//

	CA_ClearMarks ();

	CA_MarkGrChunk(STARTFONT);
	CA_MarkGrChunk(STARTTILE8);
	CA_MarkGrChunk(STARTTILE8M);
	CA_MarkGrChunk(HAND1PICM);

	CA_MarkGrChunk(NORTHICONSPR);
	CA_CacheMarks (NULL);

	MM_SetLock (&grsegs[STARTFONT],true);
	MM_SetLock (&grsegs[STARTTILE8],true);
	MM_SetLock (&grsegs[STARTTILE8M],true);
	MM_SetLock (&grsegs[HAND1PICM],true);

	fontcolor = WHITE;


//
// build some tables
//
	for (i=0;i<MAPSIZE;i++)
		nearmapylookup[i] = &tilemap[0][0]+MAPSIZE*i;

	for (i=0;i<PORTTILESHIGH;i++)
		uwidthtable[i] = UPDATEWIDE*i;

	blockstart = &blockstarts[0];
	for (y=0;y<UPDATEHIGH;y++)
		for (x=0;x<UPDATEWIDE;x++)
			*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;

	BuildTables ();			// 3-d tables

	SetupScaling ();

#ifndef PROFILE
//	US_FinishTextScreen();
#endif

#if 0
//
// reclaim the memory from the linked in text screen
//
	segstart = FP_SEG(&introscn);
	seglength = 4000/16;
	if (FP_OFF(&introscn))
	{
		segstart++;
		seglength--;
	}
	MML_UseSpace (segstart,seglength);
#endif

	VW_SetScreenMode (GRMODE);
	ge_textmode = false;
//	VW_ColorBorder (3);
	VW_ClearVideo (BLACK);

//
// initialize variables
//
	updateptr = &update[0];
	*(id0_unsigned_t *)(updateptr + UPDATEWIDE*PORTTILESHIGH) = UPDATETERMINATE;
	bufferofs = 0;
	displayofs = 0;
	VW_SetLineWidth(SCREENWIDTH);
}

//===========================================================================

void clrscr (void);		// can't include CONIO.H because of name conflicts...

/*
==========================
=
= Quit
=
==========================
*/

void Quit (const id0_char_t *error, ...)
{
	id0_short_t exit_code=0;
#ifndef CATALOG
	void *finscreen;
	//id0_unsigned_t	finscreen;
#endif

	va_list ap;

	va_start(ap,error);

#ifndef CATALOG
	if (!error)
	{
		CA_SetAllPurge ();
		CA_CacheGrChunk (PIRACY);
		finscreen = grsegs[PIRACY];
		//finscreen = (id0_unsigned_t)grsegs[PIRACY];
	}
#endif

	ShutdownId ();

	if (error && *error)
	{
		BE_ST_vprintf(error,ap);
		exit_code = 1;
	}
#ifndef CATALOG
	else
	if (!NoWait)
	{
		memcpy(BE_ST_GetTextModeMemoryPtr(), finscreen, 4000);
		BE_ST_MarkGfxForUpdate();
		//movedata (finscreen,0,0xb800,0,4000);
		BE_ST_BiosScanCode(0);
	}
#endif

	va_end(ap);

#ifndef CATALOG
	if (!error)
	{
		id0_argc = 2;
		id0_argv[1] = "LAST.SHL";
		id0_argv[2] = "ENDSCN.SCN";
		id0_argv[3] = NULL;
#if 0
		if (execv("LOADSCN.EXE", id0_argv) == -1)
		{
			BE_ST_clrscr();
			BE_ST_puts("Couldn't find executable LOADSCN.EXE.\n");
			BE_ST_HandleExit(1);
		}
#endif
		id0_loadscn_exe_main(id0_argc+1, id0_argv);
	}
#endif

	BE_ST_HandleExit(exit_code);
}

//===========================================================================

/*
==================
=
= TEDDeath
=
==================
*/

void	TEDDeath(void)
{
	ShutdownId();
	// REFKEEN - DISABLED
	BE_ST_ExitWithErrorMsg("Sorry, but TED5.EXE cannot be launched from game in this source port.");
	//execlp("TED5.EXE","TED5.EXE","/LAUNCH",NULL);
}

//===========================================================================

/*
====================
=
= DisplayDepartment
=
====================
*/
void DisplayDepartment(id0_char_t *text)
{
	id0_short_t temp;

//	bufferofs = 0;
	PrintY = 5;
	WindowX = 17;
	WindowW = 168;

	VW_Bar(WindowX,PrintY+1,WindowW,7,0);
	temp = fontcolor;
	fontcolor = 10;
	US_CPrintLine (text, NULL);
	fontcolor = temp;
}



/*
=====================
=
= DemoLoop
=
=====================
*/

//static	const id0_char_t *ParmStrings[] = {"easy","normal","hard",""};

void	DemoLoop (void)
{

/////////////////////////////////////////////////////////////////////////////
// main game cycle
/////////////////////////////////////////////////////////////////////////////

	displayofs = bufferofs = 0;
	VW_Bar (0,0,320,200,0);
	VW_SetScreen(0,0);

//
// Read in all the graphic images needed for the title sequence
//
		VW_WaitVBL(1);
		IN_ReadControl(0,&control);

//	set EASYMODE
//
	if (BE_Cross_strcasecmp(id0_argv[2], "1") == 0)
		EASYMODEON = true;
	else
		EASYMODEON = false;

// restore game
//
	if (BE_Cross_strcasecmp(id0_argv[3], "1") == 0)
	{
		VW_FadeOut();
		bufferofs = displayofs = 0;
		VW_Bar(0,0,320,200,0);
		if (GE_LoadGame())
		{
			loadedgame = true;
			playstate = ex_loadedgame;
			Keyboard[sc_Enter] = true;
			VW_Bar(0,0,320,200,0);
			ColoredPalette();
		}
		VW_Bar(0,0,320,200,0);
		VW_FadeIn();
	}

	// Play a game
	//
		restartgame = gd_Normal;
		NewGame();
		GameLoop();
}

//-------------------------------------------------------------------------
// DisplayIntroText()
//-------------------------------------------------------------------------
void DisplayIntroText()
{
#ifdef TEXT_PRESENTER
	PresenterInfo pi;

	const id0_char_t *toptext = "You stand before the gate leading into the Towne "
						 "of Morbidity.... "
						 "^XX";

	// REFKEEN - Fix compiler warning ("" instead of "), although this isn't actually compiled
	const id0_char_t *bottomtext = "Enter now boldly to defeat the evil Nemesis "
							 "deep inside the catacombs."
							 ""
							 "^XX";
#endif

	id0_char_t oldfontcolor=fontcolor;

	fontcolor = 14;


#ifdef TEXT_PRESENTER
	pi.xl = 0;
	pi.yl = 0;
	pi.xh = 319;
	pi.yh = 1;
	pi.bgcolor = 0;
	pi.script[0] = (id0_char_t id0_far *)toptext;
	Presenter(&pi);

	pi.yl = 160;
	pi.yh = 161;
	pi.script[0] = (id0_char_t id0_far *)bottomtext;
	Presenter(&pi);

#else
	PrintY = 1;
	PrintX = 0;
	WindowX = 0;
	WindowW = 320;


	US_Print ("      A chilling wind greets you at the entrance\n");
	US_Print ("            to the Sanctuary of the Dead.\n");

	PrintY = 180;

	fontcolor = 9;
	US_Print ("                   Shall you proceed as\n");
	fontcolor = 14;
	US_Print ("                  N");
	fontcolor = 9;
	US_Print ("ovice   or");
	fontcolor = 14;
	US_Print ("   W");
	fontcolor = 9;
	US_Print ("arrior ?");

#endif

	fontcolor = oldfontcolor;
}

#if 0
id0_boolean_t ChooseGameLevel()
{
	id0_char_t choices[] = {sc_Escape,sc_E,sc_N,sc_H,0},ch;

	CenterWindow(20,10);

	US_Print("\n   Choose difficulty level:\n");
	US_Print("       (E)asy\n");
	US_Print("       (N)ormal\n");
	US_Print("       (H)ard\n");
	US_Print("\n      (ESC)ape aborts\n");

//	VW_UpdateScreen();
	if ((ch=GetKeyChoice(choices)) == sc_Escape)
	{
		while (Keyboard[sc_Escape]);
		LastScan = 0;
		return(false);
	}

	if (ch == sc_E)
		restartgame = gd_Easy;
	else
	if (ch == sc_N)
		restartgame = gd_Normal;
	else
		restartgame = gd_Hard;

	return(true);
}
#endif


//===========================================================================

/*
==========================
=
= SetupScalePic
=
==========================
*/

void SetupScalePic (id0_unsigned_t picnum)
{
	id0_unsigned_t	scnum;

	if (picnum == 1)
		return;

	scnum = picnum-FIRSTSCALEPIC;

	if (shapedirectory[scnum])
	{
		MM_SetPurge ((memptr *)&shapedirectory[scnum],0);
		return;					// allready in memory
	}

	CA_CacheGrChunk (picnum);
	DeplanePic (picnum);
	shapesize[scnum] = BuildCompShape (&shapedirectory[scnum]);
	grneeded[picnum]&= ~ca_levelbit;
	MM_FreePtr (&grsegs[picnum]);
}

//===========================================================================

/*
==========================
=
= SetupScaleWall
=
==========================
*/

void SetupScaleWall (id0_unsigned_t picnum)
{
	id0_int_t		x,y;
	id0_unsigned_t	scnum;
	id0_byte_t	id0_far *dest;

	if (picnum == 1)
		return;

	scnum = picnum-FIRSTWALLPIC;

	if (walldirectory[scnum])
	{
		MM_SetPurge (&walldirectory[scnum],0);
		return;					// allready in memory
	}

	CA_CacheGrChunk (picnum);
	DeplanePic (picnum);
	MM_GetPtr(&walldirectory[scnum],64*64);
	dest = (id0_byte_t id0_far *)walldirectory[scnum];
	for (x=0;x<64;x++)
		for (y=0;y<64;y++)
			*dest++ = spotvis[y][x];
	grneeded[picnum]&= ~ca_levelbit;
	MM_FreePtr (&grsegs[picnum]);
}

//===========================================================================

/*
==========================
=
= SetupScaling
=
==========================
*/

void SetupScaling (void)
{
	id0_int_t		i/*,x,y*/;
	//id0_byte_t	id0_far *dest;

//
// build the compiled scalers
//
	for (i=1;i<=VIEWWIDTH/2;i++)
		// REFKEEN - Possibly-working-but-bad cast for C++
		BuildCompScale (i*2,(void **)&scaledirectory[i]);
}

//===========================================================================

id0_int_t	showscorebox;

void RF_FixOfs (void)
{
}

void HelpScreens (void)
{
}


#if 0
/*
==================
=
= CheckMemory
=
==================
*/

#define MINMEMORY	400000l

void	CheckMemory(void)
{
	id0_unsigned_t	finscreen;

	if (Flags & FL_NOMEMCHECK)
		return;

	if (mminfo.nearheap+mminfo.farheap+mminfo.EMSmem+mminfo.XMSmem
		>= MINMEMORY)
		return;

	CA_CacheGrChunk (OUTOFMEM);
	finscreen = (id0_unsigned_t)grsegs[OUTOFMEM];
	ShutdownId ();
	movedata (finscreen,7,0xb800,0,4000);
	gotoxy (1,24);
	exit(1);
}
#endif

//===========================================================================


/*
==========================
=
= main
=
==========================
*/

// REFKEEN - Param strings buffer should be const and may further be static
static const id0_char_t			*MainParmStrings[] = {"q","l","ver","nomemcheck","helptest",id0_nil_t};

// The original starting point of the game EXE
void apocgame_exe_main (void)
{
	id0_short_t i;

	starting_level = 0;

	for (i = 1;i < id0_argc;i++)
	{
		switch (US_CheckParm(id0_argv[i],MainParmStrings))
		{
			case 0:
				Flags |= FL_QUICK;
			break;

			case 1:
				starting_level = atoi(id0_argv[i]+1);
				if ((starting_level < 0) || (starting_level > LASTMAP-1))
					starting_level = 0;
			break;

			case 2:
				BE_ST_printf("%s\n", GAMENAME);
				BE_ST_printf("Copyright 1992-93 Softdisk Publishing\n");
				BE_ST_printf("%s %s\n",VERSION,REVISION);
				BE_ST_printf("\n");
				BE_ST_HandleExit(0);
			break;

			case 3:
				Flags |= FL_NOMEMCHECK;
			break;

			case 4:
				Flags |= (FL_HELPTEST|FL_QUICK);
			break;

		}
	}

	// REFKEEN difference from vanilla Catacomb Adventures:
	// Role of ^(a@&r` for game EXE has been flipped. No need to pass it
	// (or use start/intro EXE), but if ^(a@&r` is added then you may get some message.
	if (!BE_Cross_strcasecmp(id0_argv[1], "^(a@&r`"))
	//if (BE_Cross_strcasecmp(id0_argv[1], "^(a@&r`"))
		Quit("You must type CATAPOC to run CATACOMB APOCALYPSE\n");


#if 0
	MainHelpText.xl = 0;
	MainHelpText.yl = 0;
	MainHelpText.xh = 639;
	MainHelpText.yh = 199;
	MainHelpText.bgcolor = 7;
	MainHelpText.ltcolor = 15;
	MainHelpText.dkcolor = 8;
#endif

//	jabhack();

	BE_Cross_Brandomize();

	InitGame ();
//	CheckMemory ();
	LoadLatchMem ();

//	if (!LoadTextFile("MAINHELP."EXT,&MainHelpText))
//		Quit("Can't load MAINHELP."EXT);

#ifdef PROFILE
	NewGame ();
	GameLoop ();
#endif

	DemoLoop();
	Quit(NULL);
}

//-------------------------------------------------------------------------
// Display640()
//-------------------------------------------------------------------------
void Display640(void)
{
// Can you believe it takes all this just to change to 640 mode!!???!
//
	VW_ScreenToScreen(0,FREESTART-STATUSLEN,40,80);
	VW_SetLineWidth(80);
	MoveScreen(0,0);
	VW_Bar (0,0,640,200,0);
	VW_SetScreenMode(EGA640GR);
	VW_SetLineWidth(80);
	BlackPalette();
}

//-------------------------------------------------------------------------
// Display320()
//-------------------------------------------------------------------------
void Display320(void)
{
// Can you believe it takes all this just to change to 320 mode!!???!
//
	VW_ColorBorder(0);
	VW_FadeOut();
	VW_SetLineWidth(40);
	MoveScreen(0,0);
	VW_Bar (0,0,320,200,0);
	VW_SetScreenMode(EGA320GR);
	VW_SetLineWidth(40);
	BlackPalette();
	VW_ScreenToScreen(FREESTART-STATUSLEN,0,40,80);
}

void PrintHelp(void)
{
	id0_char_t oldfontcolor = fontcolor;
	PrintY = 1;
	WindowX = 135;
	WindowW = 640;

	VW_FadeOut();
	bufferofs = displayofs = screenloc[0];
	VW_Bar(0,0,320,200,0);

	Display640();

	VW_Bar(0, 0, 640, 200, 7);

	fontcolor = (7 ^ 1);
	US_Print ("\n\n                    SUMMARY OF GAME CONTROLS\n\n");

	fontcolor = (7 ^ 4);
	US_Print ("         ACTION\n\n");

	US_Print ("Arrow keys, joystick, or mouse\n");
	US_Print ("TAB or V while turning\n");
	US_Print ("ALT or Button 2 while turning\n");
	US_Print ("CTRL or Button 1\n");
	US_Print ("Z\n");
	US_Print ("X or Enter\n");
	US_Print ("F1\n");
	US_Print ("F2\n");
	US_Print ("F3\n");
	US_Print ("F4\n");
	US_Print ("F5\n");
	US_Print ("ESC\n\n");
	fontcolor = (7 ^ 0);
#ifndef CATALOG
	US_Print ("          (See complete Instructions for more info)\n");
#endif
	US_Print ("\n           copyright (c) 1992-93 Softdisk Publishing\n");

	fontcolor = (7 ^ 8);
	PrintX = 400;
	PrintY = 37;
	WindowX = 400;
	US_Print ("   REACTION\n\n");
	US_Print ("Move and turn\n");
	US_Print ("Turn quickly (Quick Turn)\n");
	US_Print ("Move sideways\n");
	US_Print ("Shoot a Missile\n");
	US_Print ("Shoot a Zapper\n");
	US_Print ("Shoot an Xterminator\n");
	US_Print ("Help (this screen)\n");
	US_Print ("Sound control\n");
	US_Print ("Save game position\n");
	US_Print ("Restore a saved game\n");
	US_Print ("Joystick control\n");
	US_Print ("System options\n\n\n");

	VW_UpdateScreen();
	VW_FadeIn();
	VW_ColorBorder(8 | 56);
	IN_Ack();
	Display320();
	fontcolor = oldfontcolor;
}
