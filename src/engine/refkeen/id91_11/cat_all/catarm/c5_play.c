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

// C3_PLAY.C

#include "def.h"
#include "gelib.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define POINTTICS	6
#define PAUSE 300

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_byte_t bcolor;
id0_short_t skytimer=-1,skytimer_reset;
id0_short_t groundtimer=-1,groundtimer_reset;

id0_unsigned_t scolor,gcolor;
id0_unsigned_t *skycolor,*groundcolor,debug_sky,debug_gnd;

id0_unsigned_t nocolorchange=0xFFFF;
id0_byte_t BGFLAGS,				// global that holds all current flags
	  bgflag;				// used by BG changer, this flag is set when done


id0_unsigned_t sky_daytonight[]={0x0909,0x0101,0x0808,0x0000,0xFFFF};
//unsigned gnd_daytonight[]={0x0202,0xFFFF};

id0_unsigned_t sky_lightning[]={0x0101,0x0909,0x0f0f,0x0808,0x0000,0xFFFF};

id0_unsigned_t sky_colors[NUMLEVELS]={0x0000,0x0000,0x0000,0x0000,0x0808,
										  0x0404,0x0000,0x0000,0x0000,0x0000,
										  0x0000,0x0000,0x0000,0x0000,0x0606,
										  0x0000,0x0000,0x0000,0x0000,0x0000,
										  0x0000};
id0_unsigned_t gnd_colors[NUMLEVELS]={0x0202,0x0202,0x0606,0x0202,0x0707,
										  0x0505,0x0808,0x0606,0x0101,0x0808,
										  0x0606,0x0404,0x0808,0x0c0c,0x0e0e,
										  0x0808,0x0808,0x0c0c,0x0000,0x0707,
										  0x0808};


ControlInfo	control;
id0_boolean_t		running=false; //,slowturn;

id0_int_t			bordertime;
// (REFKEEN) new has been renamed newobj since new is a reserved C++ keyword
objtype objlist[MAXACTORS],*newobj,*obj,*player,*lastobj,*objfreelist;

#if USE_INERT_LIST
inertobjtype inertobjlist[MAXINERTOBJ],*inert;
#endif

id0_unsigned_t	farmapylookup[MAPSIZE];
id0_byte_t		*nearmapylookup[MAPSIZE];

id0_boolean_t		singlestep,godmode;
id0_int_t			extravbls;
status_flags    status_flag;
id0_int_t             status_delay;

//
// replacing refresh manager
//
id0_unsigned_t	mapwidth,mapheight,tics,realtics;
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

id0_short_t BeepTime = 0;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void CalcBounds (objtype *ob);
void DrawPlayScreen (void);
void PreFullDisplay(void);
void PostFullDisplay(id0_boolean_t draw_view);


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

void CalibrateJoystick(id0_short_t joynum);

//==========================================================================

///////////////////////////////////////////////////////////////////////////
//
//	CenterWindow() - Generates a window of a given width & height in the
//		middle of the screen
//
///////////////////////////////////////////////////////////////////////////

#define MAXX	320
#define MAXY	120

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
	//extern id0_boolean_t autofire;

	if (screenfaded)			// don't do anything with a faded screen
		return;

#if 0
//
// pause key wierdness can't be checked as a scan code
//
	if (Paused)
	{
		CenterWindow (8,3);
		US_PrintCentered ("PAUSED");
		VW_UpdateScreen ();
//		SD_MusicOff();
		IN_Ack();
//		SD_MusicOn();
		Paused = false;
		// (REFKEEN) Minor difference from vanilla Catacomb
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL);	// Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}
	else
	if (Keyboard[sc_Enter])			// P = pause with no screen disruptioon
	{
//		SD_MusicOff();
		DisplaySMsg("PAUSED",NULL);
		IN_Ack();
//		SD_MusicOn();
		// (REFKEEN) Minor difference from vanilla Catacomb
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL);	// Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}
	else
	if (Keyboard[sc_S])
	{
		id0_char_t *Text[] = {{"Slow Mode ON"},{"Slow Mode OFF"}};

		SlowMode ^= 1;
		extravbls = SlowMode << 3;
		CenterWindow (8,3);
		US_PrintCentered (Text[SlowMode]);
		VW_UpdateScreen ();
//		SD_MusicOff();
		IN_Ack();
//		SD_MusicOn();
		// (REFKEEN) Minor difference from vanilla Catacomb
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL);	// Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}
#endif


// F2 - SOUND OPTIONS
//
	if (Keyboard[sc_F2])
	{
		id0_int_t height=7;
		id0_boolean_t ChoiceMade = false;

		if (AdLibPresent)
			height++;

		VW_FixRefreshBuffer();
		CenterWindow(22,height);
		US_Print( "\n        1 )  NO SOUND \n");
		US_Print(   "        2 )  PC  AUDIO \n");

		if (AdLibPresent)
			US_Print("        3 ) ADLIB AUDIO\n");

		US_Print( "\n       ESC)    EXIT    ");
		VW_UpdateScreen();

		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Push();
		BE_ST_AltControlScheme_PrepareFaceButtonsDOSScancodes(AdLibPresent ? (const char []){2, 3, 4, sc_Escape, 0} : (const char []){2, 3, sc_Escape, 0});

		// Switch audio device ON/OFF & load sounds if there
		// was a change in the device.

		do {

			if (Keyboard[1]) 								// ESC - Exit
				ChoiceMade = true;
			else
			if (Keyboard[2]) 							 	// 1 - No Sound
			{
				SD_SetSoundMode(sdm_Off);
				ChoiceMade = true;
			}
			else
			if (Keyboard[3])  							// 2 - PC Audio
			{
				SD_SetSoundMode(sdm_PC);
//				if (oldsoundmode != sdm_PC)
					CA_LoadAllSounds();
				ChoiceMade = true;
			}
			else
			if ((Keyboard[4]) &&	AdLibPresent)		// 3 - AdLib Audio
			{
				SD_SetSoundMode(sdm_AdLib);
//				if (oldsoundmode != sdm_AdLib)
					CA_LoadAllSounds();
				ChoiceMade = true;
			}

			BE_ST_ShortSleep();

		} while (!ChoiceMade);

		// REFKEEN - Alternative controllers support
		BE_ST_AltControlScheme_Pop();

		tics = realtics = 1;
		IN_ClearKeysDown();
	}

// F5 - CALIBRATE JOYSTICK
//
	if (Keyboard[sc_F5])
	{
		CalibrateJoystick(0);
		tics = realtics = 1;
		IN_ClearKeysDown();
	}

deadloop:;
// ESCAPE - quits game
//
	if ((Keyboard[sc_Escape]) || (Flags & FL_DEAD))
	{
		id0_char_t ch;

		DisplaySMsg("Options", NULL);
		status_flag = S_NONE;


		if (Flags & FL_DEAD)
		{
			id0_char_t choices[] = {sc_Escape,sc_R,sc_N,sc_Q,0};
			ch = DisplayMsg("Restore          New          Quit",choices);
		}
		else
		{
			id0_char_t choices[] = {sc_Escape,sc_S,sc_R,sc_N,sc_Q,0};
			ch = DisplayMsg("Save       Restore       New       Quit",choices);
		}
		DrawText(true);

		switch (ch)
		{
			case sc_S:
				if (!(Flags & FL_DEAD))
					Keyboard[sc_F3] = true;
			break;

			case sc_R:
				Keyboard[sc_F4] = true;
			break;

			case sc_N:
				DisplaySMsg("Starting anew", NULL);
				VW_WaitVBL(60);
				playstate = ex_resetgame;
				Flags &= ~FL_DEAD;
			break;

			case sc_Q:
				DisplaySMsg("FARE THEE WELL!", NULL);
				VW_WaitVBL(120);
				if (!Flags & FL_QUICK)
					VW_FadeOut();
				NormalScreen();
				FreeUpMemory();
				Quit(NULL);
			break;
		}
		tics = realtics = 1;
	}

// F1 - DISPLAY HELP
//
	if (Keyboard[sc_F1])
	{
		PrintHelp();

#ifdef TEXT_PRESENTER

		extern PresenterInfo MainHelpText;

		VW_FadeOut();

		FreeUpMemory();
		if (!LoadPresenterScript("HELP.TXT",&MainHelpText))
		{
			VW_FadeIn();
			CenterWindow(30,5);
			US_CPrint("\nError loading HELP file.\n");
			US_CPrint("Press any key.");
			IN_Ack();
			VW_FadeOut();
		}
		else
		{
			VW_SetSplitScreen(200);
			bufferofs = displayofs = screenloc[0];
			VW_Bar(0,0,320,200,0);

			Display640();
			Presenter(&MainHelpText);
			Display320();
		}
		FreePresenterScript(&MainHelpText);
#endif
		VW_SetSplitScreen(120);
		VW_SetScreen(screenloc[0],0);
		screenpage = 0;
		CacheScaleds();

		bufferofs = 0;
		RedrawStatusWindow();
		ThreeDRefresh();
		VW_FadeIn();
		Keyboard[sc_F1] = false;
		tics = realtics = 1;
		IN_ClearKeysDown();
	}

// F3 - SAVE GAME
//
	if ((Keyboard[sc_F3]) && (!(Flags & FL_DEAD)))
	{
		PreFullDisplay();
		GE_SaveGame();
		PostFullDisplay(true);
		tics = realtics = 1;
		IN_ClearKeysDown();
	}

// F4 - LOAD GAME
//
	if (Keyboard[sc_F4])
	{
		PreFullDisplay();
		if (GE_LoadGame())
		{
			loadedgame = true;
			playstate = ex_loadedgame;
			Flags &= ~FL_DEAD;
			lasttext = -1;
			PostFullDisplay(false);
		}
		else
		if (playstate == ex_victorious)
		{
			PostFullDisplay(false);
			Victory(false);
			IN_Ack();
//			gamestate.mapon++;
		}
		else
			PostFullDisplay(true);
		Keyboard[sc_F5] = false;
		tics = realtics = 1;
		IN_ClearKeysDown();
	}

	if (Flags & FL_DEAD)
		goto deadloop;

//
// F10-? debug keys
//
	if (Keyboard[sc_BackSpace])
	{
		DebugKeys();
		// (REFKEEN) Minor difference from vanilla Catacomb
		if (MousePresent) BE_ST_GetMouseDelta(NULL, NULL);	// Clear accumulated mouse movement
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
		lasttimecount = SD_GetTimeCount();
	}
}

//-------------------------------------------------------------------------
// PreFullDisplay()
//-------------------------------------------------------------------------
void PreFullDisplay()
{
	VW_FadeOut();
	VW_SetSplitScreen(200);
	bufferofs = displayofs = screenloc[0];
	VW_Bar(0,0,320,200,0);
}

//-------------------------------------------------------------------------
// PostFullDisplay()
//-------------------------------------------------------------------------
void PostFullDisplay(id0_boolean_t draw_view)
{
	VW_SetSplitScreen(120);
	bufferofs = 0;
	RedrawStatusWindow();
	if (draw_view)
	{
		ThreeDRefresh();
		VW_FadeIn();
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

#if USE_INERT_LIST
	inert = inertobjlist;
#endif

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
	newobj->prev = lastobj;	// newobj->next is allready NULL from memset

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

	objectcount--;
}

#if USE_INERT_LIST

//--------------------------------------------------------------------------
// MoveObjToInert()
//--------------------------------------------------------------------------
void MoveObjToInert(objtype *obj)
{

	if (inert == &inertobjlist[MAXINERTOBJ])
		return;

// Transfer info needed by inert objtype
//
	inert->x = obj->x;
	inert->y = obj->y;
	inert->size = obj->size;
	inert->viewx = obj->viewx;
	inert->tilex = obj->tilex;
	inert->tiley = obj->tiley;
	inert->state = obj->state;
	inert->ticcount = obj->ticcount;

// Setup links between inert objects
//
	if (inert != inertobjlist)
		(inert-1)->next = inert;
	inert->next = NULL;
	inert++;

// Free 'real' object from list.
//
	RemoveObj(obj);
}

#endif

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

	IN_ReadControl(0,&control);

	if (MousePresent)
	{
		// (REFKEEN) Minor difference from vanilla Catacomb
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
			control.button0 = 1;
		if (buttons&2)
			control.button1 = 1;

	}

	if (Keyboard[sc_V] || Keyboard[sc_Tab])
		running = true;
	else
		running = false;
}

//==========================================================================

#if 0
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
			MM_SetPurge(&((memptr)audiosegs[STARTMUSIC + i]),3);
			MM_SetLock(&((memptr)audiosegs[STARTMUSIC + i]),false);
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
		MM_SetLock(&((memptr)audiosegs[STARTMUSIC + chunk]),true);
		SD_StartMusic((MusicGroup far *)audiosegs[STARTMUSIC + chunk]);
	}
}
#endif

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

	id0_char_t shot_color[3] = {4,9,14};

	id0_int_t allgems[5]={GEM_DELAY_TIME,		// used for Q & D comparison
						 GEM_DELAY_TIME,		// for having all gems...
						 GEM_DELAY_TIME,		// the "allgems" declaration MUST
						 GEM_DELAY_TIME,		// match the "gems" declaration in
						 GEM_DELAY_TIME		// the gametype structure!
						};

//	id0_int_t originx=0;
//	id0_int_t i=100;
	id0_signed_long_t dx,dy/*,radius*/,psin,pcos,newx,newy;
	//id0_int_t		give;
	id0_short_t objnum;
	id0_signed_long_t ox,oy,xl,xh,yl,yh,px,py,norm_dx,norm_dy;
	id0_short_t o_radius;

	void (*think)(struct objstruct *); // REFKEEN: C++ patch

	ingame = true;
	SD_SetTimeCount(0);
	playstate = (exittype)0;
	//playstate = TimeCount = 0;
	gamestate.shotpower = handheight = 0;
	pointcount = pointsleft = 0;

	status_flag = S_NONE;

#if 0
	// setup sky/ground colors and effects (based on level)
	//
	switch (gamestate.mapon)
	{
		case 255:
			if (!(BGFLAGS & BGF_NIGHT))
			{
				InitBgChange(3*60,sky_daytonight,-1,NULL,BGF_NIGHT);
				groundcolor = &gnd_colors[0];
			}
			else
			{
				skycolor = &sky_colors[0];
				groundcolor = &gnd_colors[0];
			}
		break;

		default:
			skycolor = &sky_colors[gamestate.mapon];
			groundcolor = &gnd_colors[gamestate.mapon];
			skytimer = groundtimer = -1;
		break;
	}
#endif

	BGFLAGS |= BGF_NOT_LIGHTNING;
	skytimer = groundtimer = -1;

	debug_gnd = *groundcolor;
	debug_sky = *skycolor;
	RedrawStatusWindow();
	ThreeDRefresh();
	if (screenfaded)
		VW_FadeIn();

#ifndef PROFILE
	fizzlein = true;				// fizzle fade in the first refresh
#endif
	/*TimeCount = */lasttimecount = lastnuke = 0;
	SD_SetTimeCount(0);

	PollControls ();				// center mouse
//	StartMusic ();
	do
	{
#ifndef PROFILE
		PollControls();
#else
		control.xaxis = 1;
		//if (++TimeCount == 300)
		//	return;
		SD_SetTimeCount(SD_GetTimeCount()+1);
		if (SD_GetTimeCount() == 300)
		{
			BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
			return;
		}
#endif
		DisplayStatus(&status_flag);

		objnum=0;
		for (obj = player;obj;obj = obj->next)
		{
			if ((obj->active >= yes) && (!(FreezeTime && (obj!=player))))
			{
				if (obj->ticcount)
				{
					obj->ticcount-=realtics;
					while ( obj->ticcount <= 0)
					{
						think = obj->state->thinkptr;
						if (think)
						{
							statetype *oldstate=obj->state;

							think (obj);
							if (!obj->state)
							{
								RemoveObj (obj);
								goto nextactor;
							}
							if (obj->state != oldstate)
								break;
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

			// keep a list of objects around the player for radar updates
			//
				if (obj == player)
				{
					px = player->x;
					py = player->y;
					psin = sintable[player->angle];
					pcos = costable[player->angle];
					xl = px-((id0_long_t)RADAR_WIDTH<<TILESHIFT)/2;
					xh = px+((id0_long_t)RADAR_WIDTH<<TILESHIFT)/2-1;
					yl = py-((id0_long_t)RADAR_HEIGHT<<TILESHIFT)/2;
					yh = py+((id0_long_t)RADAR_HEIGHT<<TILESHIFT)/2;
				}

				if (objnum > MAX_RADAR_BLIPS-2)
					objnum = MAX_RADAR_BLIPS-2;

				ox = obj->x;
				oy = obj->y;


				if ((ox >= xl) && (ox <= xh) && (oy >= yl) && (oy <= yh))
				{
					norm_dx = (dx = px-ox)>>TILESHIFT;
					norm_dy = (dy = oy-py)>>TILESHIFT;

					id0_int_t IntSqrt(id0_long_t va);

					o_radius = IntSqrt((norm_dx * norm_dx) + (norm_dy * norm_dy));

					if (o_radius < RADAR_RADIUS)
					{
						newx = FixedByFrac(dy,pcos)-FixedByFrac(dx,psin);
						newy = FixedByFrac(dy,psin)+FixedByFrac(dx,pcos);

						RadarXY[objnum][0]=newx>>TILESHIFT;
						RadarXY[objnum][1]=newy>>TILESHIFT;

						// Define color to use for this object...
						//

						switch (obj->obclass)
						{
			// NO GEM NEEDED
			//
					// THE WIZARD! (YOU)
					//
							case playerobj:
								RadarXY[objnum++][2]=15;
							break;

					// WIZARD'S SHOTS
					//
							case pshotobj:
							case bigpshotobj:
								RadarXY[objnum++][2]=shot_color[screenpage];
							break;

					// BATS	    							(DK GRAY)
					//
							case batobj:
								if (obj->active == always)
									RadarXY[objnum++][2]=8;
							break;

					// RABBITS	    						(LT GRAY)
					//
							case bunnyobj:
								if (obj->active == always)
									RadarXY[objnum++][2]=7;
							break;

			// RED GEM
			//
					// EYE, RED DEMON        					(DK RED)
					//
							case eyeobj:
							case reddemonobj:
								if (gamestate.gems[B_RGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=4;
							break;

					// RED MAGE							(LT RED)
					//
							case mageobj:
								if (gamestate.gems[B_RGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=12;
							break;

			// BLUE GEM
			//
					// SUCCUBUS							(LT BLUE)
					//
							case succubusobj:
								if (gamestate.gems[B_BGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=9;
							break;

					// WATER DRAGON							(DK BLUE)
					//
							case wetobj:
								if (gamestate.gems[B_GGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=1;
							break;



			// GREEN GEM
			//
					// GREEN TROLL							(LT GREEN)
					//
							case fatdemonobj:
								if (gamestate.gems[B_GGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=10;
							break;

					// GODESS							(DK GREEN)
					//
							case godessobj:
								if (gamestate.gems[B_GGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=2;
							break;

			// YELLOW GEM
			//
					// ANT								(BROWN)
					//
							case antobj:
							case treeobj:
								if (gamestate.gems[B_YGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=6;
							break;

					// SKELETON							(YELLOW)
					//
							case skeletonobj:
								if (gamestate.gems[B_YGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=14;
							break;

			// PURPLE GEM
			//
					// ZOMBIE
					//
							case zombieobj:
								if (gamestate.gems[B_PGEM-B_RGEM])
									if (obj->active == always)
										RadarXY[objnum++][2]=13;
							break;

			// ALL GEMS NEEDED
			//
					// NEMESIS
					//
							case grelmobj:
								if (!memcmp(gamestate.gems,allgems,sizeof(gamestate.gems)))
									if (obj->active == always)
										RadarXY[objnum++][2]=15;
							break;
						}
					}
				}
		}
		RadarXY[objnum][2]=-1;		// Signals end of RadarXY list...

#if USE_INERT_LIST
		if (inert != inertobjlist)
			for (obj=(objtype *)inertobjlist;obj;obj=obj->next)
				if (obj->ticcount)
				{
					obj->ticcount-=realtics;
					while ( obj->ticcount <= 0)
					{
						obj->state = obj->state->next;
						if (!obj->state)
							Quit("Removable object in INERT list.");

						if (!obj->state->tictime)
						{
							obj->ticcount = 0;
							goto nextactor;
						}

						if (obj->state->tictime>0)
							obj->ticcount += obj->state->tictime;
					}
				}
#endif

		if (bordertime)
		{
			bordertime -= realtics;
			if (bordertime<=0)
			{
				bordertime = 0;
				VW_ColorBorder(0);
			}
		}

#if 1
// random lightning?
//
	if (BGFLAGS & (BGF_NOT_LIGHTNING))
	{
		if ((scolor & 0xe0) && (!(BE_Cross_Brandom(20-realtics))))
		{
			BGFLAGS &= ~BGF_NOT_LIGHTNING;
			InitBgChange(1,sky_lightning,-1,NULL,BGF_NOT_LIGHTNING);
		}
	}

// handle sky/ground color changes
//
		if (skytimer != -1)
		{
			skytimer -= realtics;
			if (skytimer < 0)
			{
				skycolor++;
				if (*skycolor == 0xffff)
				{
					skytimer = -1;
//					skycolor--;
					skycolor = &scolor;
					if (groundtimer == -1)
						BGFLAGS |= bgflag;
				}
				else
					skytimer = skytimer_reset;
			}
		}

		if (groundtimer != -1)
		{
			groundtimer -= realtics;
			if (groundtimer < 0)
			{
				groundcolor++;
				if (*groundcolor == 0xffff)
				{
					groundtimer = -1;
//					groundcolor--;
					groundcolor = &gcolor;
					if (skytimer == -1)
						BGFLAGS |= bgflag;
				}
				else
					groundtimer = groundtimer_reset;
			}
		}
#endif


//
//		Handle FreezeTime counter..
//
		if (FreezeTime)
		{
			if (FreezeTime<20*30)
				if ((BeepTime+=realtics)>=60)
				{
					BeepTime -= 60;
					SD_PlaySound(TICKSND);
				}

			if ((FreezeTime-=realtics)<=0)
			{
				FreezeTime=0;
				SD_PlaySound(TIMERETURNSND);
				DisplaySMsg(NULL,NULL);
				status_flag = S_NONE;
			}
		}


// refresh all
//
		ThreeDRefresh ();

		if (Flags & FL_DEAD)
		{
			SD_PlaySound (GAMEOVERSND);
			DisplaySMsg("DEAD",NULL);
			DrawHealth();
			if (gamestate.potions)
			{
				 bufferofs = displayofs = screenloc[screenpage];
				 CenterWindow(35,3);
				 US_CPrint("\nYou should use your Cure Potions wisely\n");
				 IN_Ack();
			}
		}

// check for win
//
		if (playstate == ex_victorious)
		{
			Victory(true);
//			Flags |= FL_DEAD;
			IN_Ack();
//			gamestate.mapon++;
		}

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

	}while (!playstate);
//	StopMusic ();

	ingame = false;
	if (bordertime)
	{
		bordertime = 0;
		VW_ColorBorder(0);
	}

	if (abortgame)
		abortgame = false;

	BE_ST_AltControlScheme_Pop(); // REFKEEN - Alternative controllers support
}

//--------------------------------------------------------------------------
// IntSqrt() - by Master Programmer, George Leritte!
//--------------------------------------------------------------------------
id0_int_t IntSqrt(id0_long_t va)
{
	// TODO (REFKEEN): Does all of that behave the same way as the original??
	if (!va)
	{
		return 0;
	}
	id0_word_t va_lo = va&0xFFFF, va_hi = ((id0_longword_t)va)>>16;
	id0_word_t /* not id0_int_t on purpose */ result = (va_hi<<1) | (va_lo>>8) | (va_lo&0xFF);
	--result;
	result += va_hi; // initial guess
	if (((id0_int_t)result) <= 0) // TODO (REFKEEN): Is that correct?
	{
		++result;
		if (((id0_int_t)result) <= 0) // TODO (REFKEEN): Is that correct?
		{
			return 0x7FFF;
		}
	}
	id0_word_t res_div;
	do
	{
		res_div = ((id0_longword_t)va)/result; // is it close to result?

		res_div -= result;
		if ((res_div <= 1) || (res_div >= 0xFFFF))
		{
			return result;
		}
		// We want to add the result of "sar ax, 1" where ax stores res_div
		result += ((res_div & 0x8000) | (res_div >> 1));
	} while (true);
#if 0
asm     mov     AX, word ptr va
asm     mov     DX, word ptr va+2
asm     mov     bx,dx           // {bx = integer square root of dx:ax}
asm     or      bx,ax           // {if dx:ax=0 then return}
asm     jz      isq01
asm     mov     bx,dx
asm     shl     bx,1
asm     or      bl,ah
asm     or      bl,al
asm     dec     bx
asm     add     bx,dx           // { initial guess}
asm     jg      isq10
asm     inc     bx              // { don't return zero}
asm     jg      isq10
asm     mov     bx,7fffh
isq01:;
		  goto    exitrout;

isq10:;
asm     push    ax
asm     push    dx
asm     div     bx
asm     sub     ax,bx
asm     cmp     ax,1
asm     jbe     isq90
asm     cmp     ax,-1
asm     jae     isq90
asm     sar     ax,1
asm     add     bx,ax
asm     pop     dx
asm     pop     ax
asm     jmp     isq10
isq90:;
asm     pop     dx
asm     pop     ax
exitrout:;
asm     mov     ax,bx
#endif
}

//-------------------------------------------------------------------------
// InitBgChange()
//-------------------------------------------------------------------------
void InitBgChange(id0_short_t stimer, id0_unsigned_t *scolors, id0_short_t gtimer, id0_unsigned_t *gcolors, id0_byte_t flag)
{
	skytimer_reset = skytimer = stimer;
	if (scolors)
		skycolor = scolors;

	groundtimer_reset = groundtimer = gtimer;
	if (gcolors)
		groundcolor = gcolors;

	bgflag = flag;
}

////////////////////////////////////////////////////////
//
// DisplayStatus
//
//  Stat_Flag -  contains the type of status displayed
//  -- also uses status_delay (global variable) will not
//     change display until this variable is zero.
//  -- heirarchy is determined by the series of if statements,
//        to change it, rearrange th if statements.
//
////////////////////////////////////////////////////////

#define MESSAGEDELAY  25
void DisplayStatus (status_flags *stat_flag)
{
	status_flags temp_status;


	if (*stat_flag == S_TIMESTOP)
	  return;

	if (status_delay > 0)
	{
		status_delay -= realtics;
		return;
	}
	else
		status_delay = 0;

	// check for a change in status from previous call

	temp_status = S_VIEWING;                             //precaution

	if (Keyboard[sc_Control] || control.button0)
		temp_status = S_MISSLE;

	if (Keyboard[sc_Z] && !Keyboard[sc_F10])
		temp_status = S_ZAPPER;

	if ((Keyboard[sc_X] && !Keyboard[sc_F10]) || Keyboard[sc_Enter])
		temp_status = S_XTER;

	if (control.x)
		temp_status = S_TURN;

	if ((Keyboard[sc_V] || Keyboard[sc_Tab]) && control.x)
		temp_status = S_QTURN;

	if (Keyboard[sc_Alt] && control.x)
		temp_status = S_SIDESTEP;

	if (control.y < 0)
		temp_status = S_ADVANCE;

	if (control.y > 0)
		temp_status = S_RETREAT;

	if (Keyboard[sc_F5])
		temp_status = S_JOYSTICK;

	if (Keyboard[sc_F4])
		temp_status = S_RESTORING;

	if (Keyboard[sc_F3])
		temp_status = S_SAVING;

	if (Keyboard[sc_F2])
		temp_status = S_SND;

	if (Keyboard[sc_F1])
		temp_status = S_HELP;

	if (temp_status != *stat_flag)
	{
		*stat_flag = temp_status;


		switch (*stat_flag)
		{
			case S_MISSLE:
				DisplaySMsg("Magick Missile", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_ZAPPER:
				if (gamestate.bolts)
				{
					DisplaySMsg("Zapper", NULL);
					status_delay = MESSAGEDELAY+10;
				}
			break;

			case S_XTER:
				if (gamestate.nukes)
				{
					DisplaySMsg("Xterminator", NULL);
					status_delay = MESSAGEDELAY+5;
				}
			break;

			case S_TURN:
				DisplaySMsg("Turning", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_QTURN:
				DisplaySMsg("Quick Turning", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_SIDESTEP:
				DisplaySMsg("Sidestepping", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_ADVANCE:
				DisplaySMsg("Advancing", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_RETREAT:
				DisplaySMsg("Retreating", NULL);
				status_delay = MESSAGEDELAY;
			break;

			case S_JOYSTICK:
				DisplaySMsg("Adjusting Joystick", NULL);
			break;

			case S_RESTORING:
				DisplaySMsg("Restoring", NULL);
			break;

			case S_SAVING:
				DisplaySMsg("Saving", NULL);
			break;

			case S_SND:
				DisplaySMsg("Select Sound", NULL);
			break;

			case S_HELP:
				DisplaySMsg("Getting Help", NULL);
			break;

			case S_VIEWING:
				DisplaySMsg("Viewing", NULL);
			break;
		}
		bufferofs = displayofs = screenloc[screenpage];

	}
}
