/* Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// KD_DEMO.C

#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
//#include <dir.h>
#endif
#include "kd_def.h"

//#pragma hdrstop

#define RLETAG  0xABCD

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


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

//===========================================================================

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
	id0_word_t    i;

	gamestate.worldx = 0;           // spawn keen at starting spot

	gamestate.mapon = 0;
	gamestate.score = 0;
	gamestate.nextextra = 20000;
	gamestate.lives = 3;
	gamestate.flowerpowers = gamestate.boobusbombs = 0;
	for (i = 0;i < GAMELEVELS;i++)
		gamestate.leveldone[i] = false;
}

//===========================================================================

/*
=====================
=
= WaitOrKey
=
=====================
*/

id0_int_t WaitOrKey (id0_int_t vbls)
{
	while (vbls--)
	{
		IN_ReadControl(0,&c);           // get player input
		if (LastScan || c.button0 || c.button1)
		{
			IN_ClearKeysDown ();
			return 1;
		}
		VW_WaitVBL(1);
	}
	return 0;
}

//===========================================================================

/*
=====================
=
= GameOver
=
=====================
*/

void
GameOver (void)
{
	VW_InitDoubleBuffer ();
	US_CenterWindow (16,3);

	US_PrintCentered("Game Over!");

	VW_UpdateScreen ();
	IN_ClearKeysDown ();
	IN_Ack ();

}


//===========================================================================

/*
==================
=
= StatusWindow
=
==================
*/

void StatusWindow (void)
{
	id0_word_t    x;

	// DEBUG - make this look better

	US_CenterWindow(22,7);
	US_CPrint("Status Window");

	WindowX += 8;
	WindowW -= 8;
	WindowY += 20;
	WindowH -= 20;
	PrintX = WindowX;
	PrintY = WindowY;

	VWB_DrawTile8(PrintX,PrintY,26);
	VWB_DrawTile8(PrintX + 8,PrintY,27);
	PrintX += 24;
	US_PrintUnsigned(gamestate.lives);
	US_Print("\n");

	VWB_DrawTile8(PrintX,PrintY,32);
	VWB_DrawTile8(PrintX + 8,PrintY,33);
	VWB_DrawTile8(PrintX,PrintY + 8,34);
	VWB_DrawTile8(PrintX + 8,PrintY + 8,35);
	PrintX += 24;
	US_PrintUnsigned(gamestate.boobusbombs);
	US_Print("\n");

	WindowX += 50;
	WindowW -= 50;
	PrintX = WindowX;
	PrintY = WindowY;

	fontcolor = F_FIRSTCOLOR;
	US_Print("Next ");
	fontcolor = F_BLACK;
	x = PrintX;
	VWB_DrawTile8(PrintX,PrintY,26);
	VWB_DrawTile8(PrintX + 8,PrintY,27);
	PrintX += 24;
	US_PrintUnsigned(gamestate.nextextra);
	US_Print("\n");

	PrintX = x;
	VWB_DrawTile8(PrintX,PrintY,24);
	VWB_DrawTile8(PrintX + 8,PrintY,25);
	PrintX += 24;
	US_PrintUnsigned(gamestate.keys);
	US_Print("\n");

	// DEBUG - add flower powers (#36)

	VW_UpdateScreen();
	IN_Ack();
}

// REFKEEN - New cross-platform methods for reading/writing objects from/to saved games
static id0_boolean_t SaveObject(BE_FILE_T file, objtype *o)
{
	id0_int_t dummy = 0;
	// for active enum (anonymous type)
	id0_int_t activeint = (id0_int_t)(o->active);
	// BACKWARD COMPATIBILITY
	id0_word_t statedosoffset = o->state ? o->state->compatdospointer : 0;
	// Just tells if "o->next" is zero or not
	id0_int_t isnext = o->next ? 1 : 0;
	// Now writing
	size_t BE_Cross_write_classtype_To16LE(BE_FILE_T fp, const classtype *ptr);
	return ((BE_Cross_write_classtype_To16LE(file, &o->obclass) == 2)
	        && (BE_Cross_writeInt16LE(file, &activeint) == 2)
	        && (BE_Cross_write_boolean_To16LE(file, &o->needtoreact) == 2)
	        && (BE_Cross_write_boolean_To16LE(file, &o->needtoclip) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->nothink) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->x) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->y) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->xdir) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->ydir) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->xmove) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->ymove) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->xspeed) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->yspeed) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->ticcount) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->ticadjust) == 2)
	        && (BE_Cross_writeInt16LE(file, &statedosoffset) == 2) // BACKWARD COMPATIBILITY
	        && (BE_Cross_writeInt16LE(file, &o->shapenum) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->left) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->top) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->right) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->bottom) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->midx) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tileleft) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tiletop) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tileright) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tilebottom) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->tilemidx) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->hitnorth) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->hiteast) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->hitsouth) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->hitwest) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->temp1) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->temp2) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->temp3) == 2)
	        && (BE_Cross_writeInt16LE(file, &o->temp4) == 2)
	        // No need to write sprite, prev pointers as-is,
	        // these are ignored on loading. So write dummy value.
	        // Furthermore, all we need to know about next on loading is
	        // if it's zero or not.
	        && (BE_Cross_writeInt16LE(file, &dummy) == 2) // sprite
	        && (BE_Cross_writeInt16LE(file, &isnext) == 2) // next
	        && (BE_Cross_writeInt16LE(file, &dummy) == 2) // prev
	);
}

static id0_boolean_t LoadObject(BE_FILE_T file, objtype *o)
{
	id0_int_t dummy;
	// for active enum (anonymous type)
	id0_int_t activeint;
	// BACKWARD COMPATIBILITY
	id0_word_t statedosoffset;
	// Just tells if "o->next" is zero or not
	id0_int_t isnext;
	// Now reading
	size_t BE_Cross_read_classtype_From16LE(BE_FILE_T fp, classtype *ptr);
	if ((BE_Cross_read_classtype_From16LE(file, &o->obclass) != 2)
	    || (BE_Cross_readInt16LE(file, &activeint) != 2)
	    || (BE_Cross_read_boolean_From16LE(file, &o->needtoreact) != 2)
	    || (BE_Cross_read_boolean_From16LE(file, &o->needtoclip) != 2)
	    || (BE_Cross_readInt16LE(file, &o->nothink) != 2)
	    || (BE_Cross_readInt16LE(file, &o->x) != 2)
	    || (BE_Cross_readInt16LE(file, &o->y) != 2)
	    || (BE_Cross_readInt16LE(file, &o->xdir) != 2)
	    || (BE_Cross_readInt16LE(file, &o->ydir) != 2)
	    || (BE_Cross_readInt16LE(file, &o->xmove) != 2)
	    || (BE_Cross_readInt16LE(file, &o->ymove) != 2)
	    || (BE_Cross_readInt16LE(file, &o->xspeed) != 2)
	    || (BE_Cross_readInt16LE(file, &o->yspeed) != 2)
	    || (BE_Cross_readInt16LE(file, &o->ticcount) != 2)
	    || (BE_Cross_readInt16LE(file, &o->ticadjust) != 2)
	    || (BE_Cross_readInt16LE(file, &statedosoffset) != 2) // BACKWARD COMPATIBILITY
	    || (BE_Cross_readInt16LE(file, &o->shapenum) != 2)
	    || (BE_Cross_readInt16LE(file, &o->left) != 2)
	    || (BE_Cross_readInt16LE(file, &o->top) != 2)
	    || (BE_Cross_readInt16LE(file, &o->right) != 2)
	    || (BE_Cross_readInt16LE(file, &o->bottom) != 2)
	    || (BE_Cross_readInt16LE(file, &o->midx) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tileleft) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tiletop) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tileright) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tilebottom) != 2)
	    || (BE_Cross_readInt16LE(file, &o->tilemidx) != 2)
	    || (BE_Cross_readInt16LE(file, &o->hitnorth) != 2)
	    || (BE_Cross_readInt16LE(file, &o->hiteast) != 2)
	    || (BE_Cross_readInt16LE(file, &o->hitsouth) != 2)
	    || (BE_Cross_readInt16LE(file, &o->hitwest) != 2)
	    || (BE_Cross_readInt16LE(file, &o->temp1) != 2)
	    || (BE_Cross_readInt16LE(file, &o->temp2) != 2)
	    || (BE_Cross_readInt16LE(file, &o->temp3) != 2)
	    || (BE_Cross_readInt16LE(file, &o->temp4) != 2)
	    // No need to read sprite, prev pointers as-is,
	    // these are ignored on loading. So read dummy value.
	    // Furthermore, all we need to know about next on loading is
	    // if it's zero or not.
	    || (BE_Cross_readInt16LE(file, &dummy) != 2) // sprite
	    || (BE_Cross_readInt16LE(file, &isnext) != 2) // next
	    || (BE_Cross_readInt16LE(file, &dummy) != 2) // prev
	)
	{
		return false;
	}
	o->active = (activetype)activeint;
	o->state = RefKeen_GetObjStatePtrFromDOSPointer(statedosoffset);
	// ANOTHER SPECIAL CASE (for almost all creatures as flowers)
	o->temp2stateptr = RefKeen_GetObjStatePtrFromDOSPointer(o->temp2);
	// HACK: All we need to know is if next was originally NULL or not
	o->next = isnext ? o : NULL;
	return true;
}

// Similar new methods for writing/reading game state
static id0_boolean_t SaveGameState(BE_FILE_T file, gametype *state)
{
	return ((BE_Cross_writeInt16LE(file, &state->worldx) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->worldy) == 2)
	        && (BE_Cross_write_booleans_To16LEBuffer(file, state->leveldone, 2*sizeof(state->leveldone)/sizeof(id0_boolean_t)) == 2*sizeof(state->leveldone)/sizeof(id0_boolean_t))
	        && (BE_Cross_writeInt32LE(file, &state->score) == 4)
	        && (BE_Cross_writeInt32LE(file, &state->nextextra) == 4)
	        && (BE_Cross_writeInt16LE(file, &state->flowerpowers) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->boobusbombs) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->bombsthislevel) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->keys) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->mapon) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->lives) == 2)
	        && (BE_Cross_writeInt16LE(file, &state->difficulty) == 2)
	);
}

static id0_boolean_t LoadGameState(BE_FILE_T file, gametype *state)
{
	return ((BE_Cross_readInt16LE(file, &state->worldx) == 2)
	        && (BE_Cross_readInt16LE(file, &state->worldy) == 2)
	        && (BE_Cross_read_booleans_From16LEBuffer(file, state->leveldone, 2*sizeof(state->leveldone)/sizeof(id0_boolean_t)) == 2*sizeof(state->leveldone)/sizeof(id0_boolean_t))
	        && (BE_Cross_readInt32LE(file, &state->score) == 4)
	        && (BE_Cross_readInt32LE(file, &state->nextextra) == 4)
	        && (BE_Cross_readInt16LE(file, &state->flowerpowers) == 2)
	        && (BE_Cross_readInt16LE(file, &state->boobusbombs) == 2)
	        && (BE_Cross_readInt16LE(file, &state->bombsthislevel) == 2)
	        && (BE_Cross_readInt16LE(file, &state->keys) == 2)
	        && (BE_Cross_readInt16LE(file, &state->mapon) == 2)
	        && (BE_Cross_readInt16LE(file, &state->lives) == 2)
	        && (BE_Cross_readInt16LE(file, &state->difficulty) == 2)
	);
}


id0_boolean_t
SaveGame(BE_FILE_T file)
{
	id0_word_t    i/*,size*/,compressed,expanded;
	objtype *o;
	memptr  bigbuffer;

	// (REFKEEN) Writing fields one-by-one in a cross-platform manner
	if (!SaveGameState(file, &gamestate))
	//if (!CA_FarWrite(file,(void id0_far *)&gamestate,sizeof(gamestate)))
		return(false);

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i++)   // Write all three planes of the map
	{
//
// leave a word at start of compressed data for compressed length
//
		compressed = CA_RLEWCompress ((id0_unsigned_t id0_huge *)mapsegs[i]
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


id0_boolean_t
LoadGame(BE_FILE_T file)
{
	id0_word_t    i/*,j,size*/;
	//objtype *o;
	//id0_int_t orgx,orgy;
	objtype         *prev,*next,*followed;
	id0_unsigned_t        compressed,expanded;
	memptr  bigbuffer;

	// (REFKEEN) Reading fields one-by-one in a cross-platform manner
	if (!LoadGameState(file, &gamestate))
	//if (!CA_FarRead(file,(void id0_far *)&gamestate,sizeof(gamestate)))
		return(false);

// drop down a cache level and mark everything, so when the option screen
// is exited it will be cached

	ca_levelbit >>= 1;
	ca_levelnum--;

	SetupGameLevel (false);         // load in and cache the base old level
	titleptr[ca_levelnum] = levelnames[mapon];

	ca_levelbit <<= 1;
	ca_levelnum ++;

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i++)   // Read all three planes of the map
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
			(id0_unsigned_t id0_huge *)mapsegs[i],compressed,RLETAG);
	}

	MM_FreePtr (&bigbuffer);

	// Read the object list back in - assumes at least one object in list

	InitObjArray ();
	newobj = player;
	prev = newobj->prev;
	next = newobj->next;
	// (REFKEEN) Reading fields one-by-one in a cross-platform manner
	if (!LoadObject(file, newobj))
	//if (!CA_FarRead(file,(void id0_far *)newobj,sizeof(objtype)))
		return(false);
	newobj->prev = prev;
	newobj->next = next;
	newobj->needtoreact = true;
	newobj->sprite = NULL;
	newobj = scoreobj;
	while (true)
	{
		prev = newobj->prev;
		next = newobj->next;
		// And again
		if (!LoadObject(file, newobj))
		//if (!CA_FarRead(file,(void id0_far *)newobj,sizeof(objtype)))
			return(false);
		followed = newobj->next;
		newobj->prev = prev;
		newobj->next = next;
		newobj->needtoreact = true;
		newobj->sprite = NULL;

		if (followed)
			GetNewObj (false);
		else
			break;
	}

	scoreobj->temp1 = scoreobj->temp2 = -1;             // force score to be updated
	scoreobj->temp3 = -1;                   // and flower power
	scoreobj->temp4 = -1;                   // and lives
#if 0
	*((id0_long_t *)&(scoreobj->temp1)) = -1;             // force score to be updated
	scoreobj->temp3 = -1;                   // and flower power
	scoreobj->temp4 = -1;                   // and lives
#endif

	return(true);
}

void
ResetGame(void)
{
	NewGame ();

	ca_levelnum--;
	CA_ClearMarks();
	titleptr[ca_levelnum] = NULL;           // don't reload old level
	ca_levelnum++;
}

#if FRILLS
void
TEDDeath(void)
{
	ShutdownId();
	execlp("TED5.EXE","TED5.EXE","/LAUNCH","KDREAMS",NULL);
}
#endif

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
static id0_boolean_t
MoveTitleTo(id0_int_t offset)
{
	id0_boolean_t         done;
	id0_int_t                     dir,
				chunk,
				move;
	id0_longword_t        lasttime,delay;

	if (offset < originxglobal)
		dir = -1;
	else
		dir = +1;

	chunk = dir * PIXGLOBAL;

	done = false;
	delay = 1;
	while (!done)
	{
		lasttime = SD_GetTimeCount();
		move = delay * chunk;
		if (chunk < 0)
			done = originxglobal + move <= offset;
		else
			done = originxglobal + move >= offset;
		if (!done)
		{
			RF_Scroll(move,0);
			RF_Refresh();
		}
		if (IN_IsUserInput())
			return(true);
		delay = SD_GetTimeCount() - lasttime;
	}
	if (originxglobal != offset)
	{
		RF_Scroll(offset - originxglobal,0);
		RF_Refresh();
	}
	return(false);
}

static id0_boolean_t
Wait(id0_longword_t time)
{
	time += SD_GetTimeCount();
	while ((SD_GetTimeCount() < time) && (!IN_IsUserInput()))
	{
		if (!(SD_GetTimeCount() % MINTICS))
			RF_Refresh();
		else
			BE_ST_ShortSleep();
	}
	return(IN_IsUserInput());
}

static id0_boolean_t
ShowText(id0_int_t offset,WindowRec *wr,const id0_char_t *s)
{
	if (MoveTitleTo(offset))
		return(true);

	US_RestoreWindow(wr);
	US_CPrint(s);
	VW_UpdateScreen();

	if (Wait(TickBase * 5))
		return(true);

	US_RestoreWindow(wr);
	US_CPrint(s);
	VW_UpdateScreen();
	return(false);
}
#endif // REFKEEN game versions

/*
=====================
=
= DemoLoop
=
=====================
*/

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
void
DemoLoop (void)
{
	const id0_char_t            *s;
	//id0_word_t            move;
	//id0_longword_t        lasttime;
	WindowRec       mywin;

#if FRILLS
//
// check for launch from ted
//
	if (tedlevel)
	{
		NewGame();
		gamestate.mapon = tedlevelnum;
		GameLoop();
		TEDDeath();
	}
#endif

//
// demo loop
//
	US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
	restartgame = gd_Continue;
	while (true)
	{
		// Load the Title map
		gamestate.mapon = 20;           // title map number
		loadedgame = false;
		SetupGameLevel(true);

		while (!restartgame && !loadedgame)
		{
			VW_InitDoubleBuffer();
			IN_ClearKeysDown();

			while (true)
			{
				// Display the Title map
				RF_NewPosition((5 * TILEGLOBAL) + (TILEGLOBAL / 2),
								(TILEGLOBAL * 2) + (TILEGLOBAL / 2)
								+ (TILEGLOBAL / 4));
				RF_ForceRefresh();
				RF_Refresh();
				RF_Refresh();

				if (Wait(TickBase * 2))
					break;

				mywin.x = (16 * 13) + 4;
				mywin.y = 0;
				mywin.w = 16 * 7;
				mywin.h = 200;
				mywin.px = mywin.x + 0;
				mywin.py = mywin.y + 10;
				s =             "Game\n"
						"John Carmack\n"
						"\n"
						"Utilities\n"
						"John Romero\n"
						"\n"
						"Interface/Sound\n"
						"Jason Blochowiak\n"
						"\n"
						"Creative Director\n"
						"Tom Hall\n"
						"\n"
						"Art\n"
						"Adrian Carmack\n";
				if (ShowText((9 * TILEGLOBAL) - (PIXGLOBAL * 2),&mywin,s))
					break;

				mywin.x = 4;
				mywin.y = 0;
				mywin.w = 16 * 7;
				mywin.h = 200;
				mywin.px = mywin.x + 0;
				mywin.py = mywin.y + 10;
				s =             "\n"
						"\"Keen Dreams\"\n"
						"Copyright 1991-93\n"
						"Softdisk, Inc.\n"
						"\n"
						"\n"
						"\n"
						"\n"
						"Commander Keen\n"
						"Copyright 1990-91\n"
						"Id Software, Inc.\n"
						"\n"
						"Press F1 for Help\n"
						"SPACE to Start\n";
				if (ShowText((2 * TILEGLOBAL) + (PIXGLOBAL * 2),&mywin,s))
					break;

				if (MoveTitleTo((5 * TILEGLOBAL) + (TILEGLOBAL / 2)))
					break;
				if (Wait(TickBase * 3))
					break;

				VWB_Bar(0,0,320,200,FIRSTCOLOR);
				US_DisplayHighScores(-1);

				if (IN_UserInput(TickBase * 8,false))
					break;
			}

			US_ControlPanel ();
		}

		if (!loadedgame)
			NewGame();
		GameLoop();
	}
}
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
void
DemoLoop (void)
{
	//id0_char_t		*s;
	//id0_word_t		move;
	//id0_longword_t	lasttime;
	const id0_char_t *FileName1;
	struct Shape FileShape1;
#if CREDITS
	const id0_char_t *FileName2;
	struct Shape FileShape2;
#endif
	//struct ffblk ffblk;
	//WindowRec	mywin;
	id0_int_t bufsave	= bufferofs;
	id0_int_t dissave	= displayofs;


#if FRILLS
//
// check for launch from ted
//
	if (tedlevel)
	{
		NewGame();
		gamestate.mapon = tedlevelnum;
		GameLoop();
		TEDDeath();
	}
#endif

//
// demo loop
//
	US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
	restartgame = gd_Continue;

	BE_FILE_T handle = BE_Cross_open_for_reading("KDREAMS.CMP");
	//int handle = open("KDREAMS.CMP" ,O_BINARY | O_RDONLY);
	if (!BE_Cross_IsFileValid(handle))
	//if (handle == -1)
		Quit("Couldn't find KDREAMS.CMP");
	BE_Cross_close(handle);
#if 0
	if (findfirst("KDREAMS.CMP", &ffblk, 0) == -1)
		Quit("Couldn't find KDREAMS.CMP");
#endif

	while (true)
	{

		loadedgame = false;

		FileName1 = "TITLESCR.LBM";
		if (LoadLIBShape("KDREAMS.CMP", FileName1, &FileShape1))
			Quit("Can't load TITLE SCREEN");
#if CREDITS
		FileName2 = "CREDITS.LBM";
		if (LoadLIBShape("KDREAMS.CMP", FileName2, &FileShape2))
			Quit("Can't load CREDITS SCREEN");
#endif

		while (!restartgame && !loadedgame)
		{

			VW_InitDoubleBuffer();
			IN_ClearKeysDown();

			while (true)
			{

				VW_SetScreen(0, 0);
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape1, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

#if CREDITS
				if (IN_UserInput(TickBase * 8, false))
					break;
#else
				if (IN_UserInput(TickBase * 4, false))
					break;
#endif

#if CREDITS
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape2, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

				if (IN_UserInput(TickBase * 7, false))
					break;
#else
				MoveGfxDst(0, 200);
				UnpackEGAShapeToScreen(&FileShape1, 0, 0);
				VW_ScreenToScreen (64*200,0,40,200);

				if (IN_UserInput(TickBase * 3, false))
					break;
#endif

				displayofs = 0;
				VWB_Bar(0,0,320,200,FIRSTCOLOR);
				US_DisplayHighScores(-1);

				if (IN_UserInput(TickBase * 6, false))
					break;

			}

			bufferofs = bufsave;
			displayofs = dissave;

			VW_FixRefreshBuffer();
			US_ControlPanel ();
		}

		if (!loadedgame)
			NewGame();

		FreeShape(&FileShape1);
#if CREDITS
		FreeShape(&FileShape2);
#endif
		GameLoop();
	}
}
#endif // VERSION
