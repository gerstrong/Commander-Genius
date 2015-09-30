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

// C3_PLAY.C

#include "c3_def.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define POINTTICS	6


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

ControlInfo	c;
id0_boolean_t		running,slowturn;

id0_int_t			bordertime;
// (REFKEEN) new has been renamed newobj since new is a reserved C++ keyword
objtype objlist[MAXACTORS],*newobj,*obj,*player,*lastobj,*objfreelist;

id0_unsigned_t	farmapylookup[MAPSIZE];
id0_byte_t		*nearmapylookup[MAPSIZE];

id0_boolean_t		singlestep,godmode;
id0_int_t			extravbls;

//
// replacing refresh manager
//
id0_unsigned_t	mapwidth,mapheight,tics;
id0_boolean_t		compatability;
id0_byte_t		*updateptr;
id0_unsigned_t	mapwidthtable[64];
id0_unsigned_t	uwidthtable[UPDATEHIGH];
id0_unsigned_t	blockstarts[UPDATEWIDE*UPDATEHIGH];
#define	UPDATESCREENSIZE	(UPDATEWIDE*PORTTILESHIGH+2)
#define	UPDATESPARESIZE		(UPDATEWIDE*2+4)
#define UPDATESIZE			(UPDATESCREENSIZE+2*UPDATESPARESIZE)
id0_byte_t		update[UPDATESIZE];

id0_int_t		mousexmove,mouseymove;
id0_int_t		pointcount,pointsleft;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void CalcBounds (objtype *ob);
void DrawPlayScreen (void);


//
// near data map array (wall values only, get text number from far data)
//
id0_byte_t		tilemap[MAPSIZE][MAPSIZE];
id0_byte_t		spotvis[MAPSIZE][MAPSIZE];
// (REFKEEN) BACKWARDS COMPATIBILITY: Originally used to store objtype
// pointers, as well as 16-bit unsigned integers. We now store just integers
// and convert with a macro when required.
id0_unsigned_t actorat[MAPSIZE][MAPSIZE];
//objtype		*actorat[MAPSIZE][MAPSIZE];

objtype dummyobj;

//id0_int_t bordertime;
id0_int_t	objectcount;

void StopMusic(void);
void StartMusic(void);


//==========================================================================

///////////////////////////////////////////////////////////////////////////
//
//	CenterWindow() - Generates a window of a given width & height in the
//		middle of the screen
//
///////////////////////////////////////////////////////////////////////////

#define MAXX	264
#define MAXY	146

void	CenterWindow(id0_word_t w,id0_word_t h)
{
	US_DrawWindow(((MAXX / 8) - w) / 2,((MAXY / 8) - h) / 2,w,h);
}

//===========================================================================


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
	if (screenfaded)			// don't do anything with a faded screen
		return;

//
// pause key wierdness can't be checked as a scan code
//
	if (Paused)
	{
		CenterWindow (8,3);
		US_PrintCentered ("PAUSED");
		VW_UpdateScreen ();
		SD_MusicOff();
		IN_Ack();
		SD_MusicOn();
		Paused = false;
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL); // Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}

//
// F1-F7/ESC to enter control panel
//
	if ( (LastScan >= sc_F1 && LastScan <= sc_F7) || LastScan == sc_Escape)
	{
		StopMusic ();
		NormalScreen ();
		FreeUpMemory ();
		US_CenterWindow (20,8);
		US_CPrint ("Loading");
		VW_UpdateScreen ();
		US_ControlPanel();
		// REFKEEN - Alternative controllers support (maybe user has changed some keys which may currently have an effect)
		BE_ST_AltControlScheme_PrepareInGameControls(KbdDefs[0].button0, KbdDefs[0].button1, KbdDefs[0].up, KbdDefs[0].down, KbdDefs[0].left, KbdDefs[0].right);
		//
		if (abortgame)
		{
			playstate = ex_abort;
			return;
		}
		StartMusic ();
		IN_ClearKeysDown();
		if (restartgame)
			playstate = ex_resetgame;
		if (loadedgame)
			playstate = ex_loadedgame;
		DrawPlayScreen ();
		CacheScaleds ();
		lasttimecount = SD_GetTimeCount();
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL); // Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}

//
// F10-? debug keys
//
	if (Keyboard[sc_F10])
	{
		DebugKeys();
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL); // Clear accumulated mouse 			// if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
		lasttimecount = SD_GetTimeCount();
	}

}


//===========================================================================

/*
#############################################################################

				  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitObjList
=
= Call to clear out the entire object list, returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

void InitObjList (void)
{
	id0_int_t	i;

	for (i=0;i<MAXACTORS;i++)
	{
		objlist[i].prev = &objlist[i+1];
		objlist[i].next = NULL;
	}

	objlist[MAXACTORS-1].prev = NULL;

	objfreelist = &objlist[0];
	lastobj = NULL;

	objectcount = 0;

//
// give the player and score the first free spots
//
	GetNewObj (false);
	player = newobj;
}

//===========================================================================

/*
=========================
=
= GetNewObj
=
= Sets the global variable newobj to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewObj (id0_boolean_t usedummy)
{
	if (!objfreelist)
	{
		if (usedummy)
		{
			newobj = &dummyobj;
			return;
		}
		Quit ("GetNewObj: No free spots in objlist!");
	}

	newobj = objfreelist;
	objfreelist = newobj->prev;
	memset (newobj,0,sizeof(*newobj));

	if (lastobj)
		lastobj->next = newobj;
	newobj->prev = lastobj;	// newobj->next is already NULL from memset

	newobj->active = no/*false*/;
	lastobj = newobj;

	objectcount++;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
	//objtype **spotat;

	if (gone == player)
		Quit ("RemoveObj: Tried to remove the player!");

//
// fix the next object's back link
//
	if (gone == lastobj)
		lastobj = (objtype *)gone->prev;
	else
		gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
	gone->prev->next = gone->next;

//
// add it back in to the free list
//
	gone->prev = objfreelist;
	objfreelist = gone;
}

//==========================================================================

/*
===================
=
= PollControls
=
===================
*/

void PollControls (void)
{
	id0_unsigned_t buttons;

	IN_ReadControl(0,&c);

	if (MousePresent)
	{
		buttons = BE_ST_GetMouseButtons();
		BE_ST_GetMouseDelta(&mousexmove, &mouseymove);		
#if 0
		Mouse(MButtons);
		buttons = _BX;
		Mouse(MDelta);
		mousexmove = _CX;
		mouseymove = _DX;
#endif

		if (buttons&1)
			c.button0 = 1;
		if (buttons&2)
			c.button1 = 1;

	}

	if (Controls[0]==ctrl_Joystick)
	{
		if (c.x>120 || c.x <-120 || c.y>120 || c.y<-120)
			running = true;
		else
			running = false;
		if (c.x>-48 && c.x<48)
			slowturn = true;
		else
			slowturn = false;
	}
	else
	{
		if (Keyboard[sc_RShift])
			running = true;
		else
			running = false;
		if (c.button0)
			slowturn = true;
		else
			slowturn = false;
	}
}

//==========================================================================

/*
=================
=
= StopMusic
=
=================
*/

void StopMusic(void)
{
	id0_int_t	i;

	SD_MusicOff();
	for (i = 0;i < LASTMUSIC;i++)
		if (audiosegs[STARTMUSIC + i])
		{
			MM_SetPurge((memptr *)&audiosegs[STARTMUSIC + i],3);
			MM_SetLock((memptr *)&audiosegs[STARTMUSIC + i],false);
		}
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

// JAB - Cache & start the appropriate music for this level
void StartMusic(void)
{
	musicnames	chunk;

	SD_MusicOff();
	chunk =	TOOHOT_MUS;
//	if ((chunk == -1) || (MusicMode != smm_AdLib))
//DEBUG control panel		return;

	MM_BombOnError (false);
	CA_CacheAudioChunk(STARTMUSIC + chunk);
	MM_BombOnError (true);
	if (mmerror)
		mmerror = false;
	else
	{
		MM_SetLock((memptr *)&audiosegs[STARTMUSIC + chunk],true);
		SD_StartMusic((MusicGroup id0_far *)audiosegs[STARTMUSIC + chunk]);
	}
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/

void PlayLoop (void)
{
	// REFKEEN - Alternative controllers support	
	BE_ST_AltControlScheme_Push();
	BE_ST_AltControlScheme_PrepareInGameControls(KbdDefs[0].button0, KbdDefs[0].button1, KbdDefs[0].up, KbdDefs[0].down, KbdDefs[0].left, KbdDefs[0].right);

	id0_int_t		give;

	void (*think)(struct objstruct *); // REFKEEN: C++ patch

	ingame = true;
	SD_SetTimeCount(0);
	playstate = (exittype)0;
	//playstate = TimeCount = 0;
	gamestate.shotpower = handheight = 0;
	pointcount = pointsleft = 0;

	DrawLevelNumber (gamestate.mapon);
	DrawBars ();

#ifndef PROFILE
	fizzlein = true;				// fizzle fade in the first refresh
#endif
	/*TimeCount = */lasttimecount = lastnuke = 0;

	PollControls ();				// center mouse
	StartMusic ();
	do
	{
#ifndef PROFILE
		PollControls();
#else
		c.xaxis = 1;
//		if (++TimeCount == 300)
//			return;
		SD_SetTimeCount(SD_GetTimeCount()+1);
		if (SD_GetTimeCount() == 300)
		{
			BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
			return;
		}
#endif

		for (obj = player;obj;obj = obj->next)
			if (obj->active)
			{
				if (obj->ticcount)
				{
					obj->ticcount-=tics;
					while ( obj->ticcount <= 0)
					{
						think = obj->state->thinkptr;
						if (think)
						{
							think (obj);
							if (!obj->state)
							{
								RemoveObj (obj);
								goto nextactor;
							}
						}

						obj->state = obj->state->next;
						if (!obj->state)
						{
							RemoveObj (obj);
							goto nextactor;
						}
						if (!obj->state->tictime)
						{
							obj->ticcount = 0;
							goto nextactor;
						}
						if (obj->state->tictime>0)
							obj->ticcount += obj->state->tictime;
					}
				}
				think =	obj->state->thinkptr;
				if (think)
				{
					think (obj);
					if (!obj->state)
						RemoveObj (obj);
				}
nextactor:;
			}


		if (bordertime)
		{
			bordertime -= tics;
			if (bordertime<=0)
			{
				bordertime = 0;
				VW_ColorBorder (3);
			}
		}

		if (pointcount)
		{
			pointcount -= tics;
			if (pointcount <= 0)
			{
				pointcount += POINTTICS;
				give = (pointsleft > 1000)? 1000 :
						(
							(pointsleft > 100)? 100 :
								((pointsleft < 20)? pointsleft : 20)
						);
				SD_PlaySound (GETPOINTSSND);
				AddPoints (give);
				pointsleft -= give;
				if (!pointsleft)
					pointcount = 0;
			}
		}

		ThreeDRefresh ();

		CheckKeys();
		// (REFKEEN) SPECIAL - Without this the game
		// can run very fast, even if it's not noticeable
		// (a lot of PlayLoop iterations and consumed CPU power)
		//
		// Notes:
		// 1. Should NOT be called from ThreeDRefresh/CalcTics,
		// because we don't always want that to be done
		// (e.g., FizzleFade effect right after loading C4 saved game).
		// 2. SHOULD be called AFTER CheckKeys. That function resets
		// lasttimecount (just like CalcTics) if the debug key modifier
		// (F10 in Cat. 3-D/Abyss, Backspace in Armageddon/Apocalypse)
		// is held. As a consequence, if the wait is done before the
		// call to CheckKeys then the game may seem to get stuck while
		// the debug key modifier is held.
		BE_ST_TimeCountWaitFromSrc(SD_GetTimeCount(), 1);
		//
		if (singlestep)
		{
			VW_WaitVBL(14);
			lasttimecount = SD_GetTimeCount();
		}
		if (extravbls)
			VW_WaitVBL(extravbls);

	}while (!playstate);
	StopMusic ();

	ingame = false;
	if (bordertime)
	{
		bordertime = 0;
		VW_ColorBorder (3);
	}

	if (!abortgame)
		AddPoints (pointsleft);
	else
		abortgame = false;

	BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
}

// (REFKEEN) Used for patching version-specific stuff
id0_word_t refkeen_compat_c3_play_objoffset;

void RefKeen_Patch_c3_play(void)
{
	switch (refkeen_current_gamever)
	{
	case BE_GAMEVER_CAT3D100:
		refkeen_compat_c3_play_objoffset = 0xAAF5;
		break;
	case BE_GAMEVER_CAT3D122:
		refkeen_compat_c3_play_objoffset = 0xADB3;
		break;
	}
}
