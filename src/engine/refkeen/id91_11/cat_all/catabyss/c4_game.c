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

// C3_GAME.C

#include <stdlib.h>

#include "def.h"
#include "gelib.h"
//#pragma hdrstop

#ifdef PROFILE
#include "time.h"
#endif


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define NUMLUMPS        36

#define ORCLUMP         		0
#define TROLLLUMP       		1
#define BOLTLUMP        		2
#define NUKELUMP        		3
#define POTIONLUMP      		4
#define RKEYLUMP        5
#define YKEYLUMP        6
#define GKEYLUMP        7
#define BKEYLUMP        8
#define SCROLLLUMP      9
#define CHESTLUMP       10
#define PLAYERLUMP      11
#define WALL1LUMP       12
#define WALL2LUMP       13
#define BDOORLUMP       14
#define DEMONLUMP               15
#define MAGELUMP                16
#define BATLUMP                 17
#define GRELLUMP                18
#define TOMBSTONESLUMP			  19
#define ZOMBIELUMP				  20
#define SPOOKLUMP					  21
#define SKELETONLUMP				  22
#define RGEMLUMP					  23
#define GGEMLUMP					  24
#define BGEMLUMP					  25
#define YGEMLUMP					  26
#define PGEMLUMP					  27
#define RKEY2LUMP					  28
#define WETMANLUMP				  29
#define OBJ_WARPLUMP				  30
#define EYELUMP					  31
#define REDDEMONLUMP            32
#define PITLUMP					  33
#define FTIMELUMP					  34
#define WATERCHESTLUMP			  35



id0_int_t     lumpstart[NUMLUMPS] = {
ORC_LUMP_START,
TROLL_LUMP_START,
BOLT_LUMP_START,
NUKE_LUMP_START,
POTION_LUMP_START,
RKEY_LUMP_START,
YKEY_LUMP_START,
GKEY_LUMP_START,
BKEY_LUMP_START,
SCROLL_LUMP_START,
CHEST_LUMP_START,
PLAYER_LUMP_START,
//WALL1_LUMP_START,
//WALL2_LUMP_START,
//BDOOR_LUMP_START,
0,0,0,
DEMON_LUMP_START,
MAGE_LUMP_START,
BAT_LUMP_START,
GREL_LUMP_START,
TOMBSTONES_LUMP_START,
ZOMBIE_LUMP_START,
SPOOK_LUMP_START,
SKELDUDE_LUMP_START,
RGEM_LUMP_START,
GGEM_LUMP_START,
BGEM_LUMP_START,
YGEM_LUMP_START,
PGEM_LUMP_START,
RKEY2_LUMP_START,
WETMAN_LUMP_START,
OBJ_WARP_LUMP_START,
EYE_LUMP_START,
REDDEMON_LUMP_START,
PIT_LUMP_START,
TIME_LUMP_START,
O_WATER_CHEST_LUMP_START,
};


id0_int_t     lumpend[NUMLUMPS] = {
ORC_LUMP_END,
TROLL_LUMP_END,
BOLT_LUMP_END,
NUKE_LUMP_END,
POTION_LUMP_END,
RKEY_LUMP_END,
YKEY_LUMP_END,
GKEY_LUMP_END,
BKEY_LUMP_END,
SCROLL_LUMP_END,
CHEST_LUMP_END,
PLAYER_LUMP_END,
//WALL1_LUMP_END,
//WALL2_LUMP_END,
//BDOOR_LUMP_END,
0,0,0,
DEMON_LUMP_END,
MAGE_LUMP_END,
BAT_LUMP_END,
GREL_LUMP_END,
TOMBSTONES_LUMP_END,
ZOMBIE_LUMP_END,
SPOOK_LUMP_END,
SKELDUDE_LUMP_END,
RGEM_LUMP_END,
GGEM_LUMP_END,
BGEM_LUMP_END,
YGEM_LUMP_END,
PGEM_LUMP_END,
RKEY2_LUMP_END,
WETMAN_LUMP_END,
OBJ_WARP_LUMP_END,
EYE_LUMP_END,
REDDEMON_LUMP_END,
PIT_LUMP_END,
TIME_LUMP_END,
O_WATER_CHEST_LUMP_END,
};




/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_unsigned_t        latchpics[NUMLATCHPICS];
id0_unsigned_t        tileoffsets[NUMTILE16];
id0_unsigned_t        textstarts[27];

id0_boolean_t splitscreen=false;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_boolean_t lumpneeded[NUMLUMPS];


//===========================================================================

//==========================================================================
//
//
//							LOCAL PROTOTYPES
//
//
//==========================================================================

void CashPoints(void);



/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

void ScanInfoPlane (void)
{
	extern id0_unsigned_t gnd_colors[];

	//id0_char_t hibyte;
	id0_unsigned_t        x,y/*,i,j*/;
	id0_int_t                     tile;
	id0_unsigned_t        id0_far     *start;

	InitObjList();                  // start spawning things with a clean slate

	memset (lumpneeded,0,sizeof(lumpneeded));

	start = mapsegs[2];
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *start++;
			//hibyte = tile >> 8;
			tile &= 0xff;
			if (!tile)
				continue;

			switch (tile)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				lumpneeded[PLAYERLUMP] = true;
				SpawnPlayer(x,y,NORTH+tile-1);
				break;

			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				lumpneeded[tile-5+BOLTLUMP] = true;
				SpawnBonus(x,y,tile-5);
				break;

			case 29:
				lumpneeded[RKEY2LUMP] = true;
				SpawnBonus(x,y,B_RKEY2);
			break;

			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
				lumpneeded[tile-58+RGEMLUMP] = true;
				SpawnBonus(x,y,tile-58+B_RGEM);
			break;

			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				lumpneeded[SCROLLLUMP] = true;
				SpawnBonus(x,y,B_SCROLL1+tile-12);
				break;

#if 0
			case 20:        // goal
				lumpneeded[GOALLUMP] = true;
				SpawnBonus(x,y,B_GOAL);
				break;
#endif

			case 21:        // chest
				if (gnd_colors[gamestate.mapon] == 0x0101)
					lumpneeded[WATERCHESTLUMP] = true;
				else
					lumpneeded[CHESTLUMP] = true;
				SpawnBonus(x,y,B_CHEST);
			break;

			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
				lumpneeded[OBJ_WARPLUMP] = true;
				SpawnWarp (x,y,tile-30);
				break;

			case 41:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 36:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 22:
				lumpneeded[TROLLLUMP] = true;
				SpawnTroll (x,y);
				break;

			case 42:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 37:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 23:
				lumpneeded[ORCLUMP] = true;
				SpawnOrc (x,y);
				break;

			case 43:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 38:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 25:
				lumpneeded[BATLUMP] = true;
				SpawnBat (x,y);
				break;

			case 44:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 39:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 26:
				lumpneeded[DEMONLUMP] = true;
				SpawnDemon (x,y);
				break;

			case 45:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 40:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 27:
				lumpneeded[MAGELUMP] = true;
				SpawnMage (x,y);
				break;

			case 28:
				lumpneeded[RKEYLUMP] = lumpneeded[GRELLUMP] = true;
				SpawnGrelminar (x,y);
				break;

#if 0
			case 29:
				SpawnBounce (x,y,0);
				break;

			case 30:
				SpawnBounce (x,y,1);
				break;
#endif

			case 46:
			case 47:
			case 48:
				lumpneeded[TOMBSTONESLUMP] = true;
				SpawnTombstone(x,y,tile-46);
			break;

			case 54:
				lumpneeded[PITLUMP]	= true;
				SpawnWarp(x,y,0);
				break;

			case 53:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 52:
				lumpneeded[ZOMBIELUMP] = true;
				SpawnZombie(x,y);
			break;

			case 51:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 50:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 49:
				lumpneeded[SPOOKLUMP] = true;
				SpawnSpook(x,y);
				break;

			case 57:
				lumpneeded[FTIMELUMP] = true;
				SpawnFTime(x,y);
				break;

			case 56:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 55:
				lumpneeded[SKELETONLUMP] = true;
				SpawnSkeleton(x,y);
				break;

			case 65:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 64:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 63:
				lumpneeded[WETMANLUMP] = true;
				SpawnWetMan(x,y);
				break;

			case 68:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 67:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 66:
				lumpneeded[EYELUMP] = true;
				SpawnEye(x,y);
				break;

			case 71:
				if (gamestate.difficulty <gd_Hard)
					break;
			case 70:
				if (gamestate.difficulty <gd_Normal)
					break;
			case 69:
				lumpneeded[SKELETONLUMP] = true;
				SpawnWallSkeleton(x,y);
				break;

			case 20:
			case 24:
			case 30:
				lumpneeded[REDDEMONLUMP] = true;
				SpawnRedDemon (x,y);
				break;
			}
		}

}

//==========================================================================

/*
==================
=
= ScanText
=
==================
*/

void ScanText (void)
{
	id0_int_t     i;
	id0_char_t id0_far *text;

	text = (id0_char_t id0_seg *)grsegs[LEVEL1TEXT+mapon];

	textstarts[0] = 0;

	for (i=1;i<=26;i++)
	{
		while (*text != '\n')
		{
			if (*text == '\r')
				*text = 0;
			text++;
		}
		text++;
		textstarts[i] = text - (id0_char_t id0_seg *)grsegs[LEVEL1TEXT+mapon];
		//textstarts[i] = FP_OFF(text);
	}

}

//==========================================================================

/*
==================
=
= DrawEnterScreen
=
==================
*/

static  const id0_char_t    *levelnames[] =
				{
					"The Towne Cemetery",
					"The Garden of Tears",
					"The Den of Zombies",
					"The Mausoleum Grounds",
					"The Main Floor of the Mausoleum",
					"Mike's Blastable Passage",
					"The Crypt of Nemesis the Undead",
					"The Subterranean Vault",
					"The Ancient Aqueduct",
					"The Orc Mines",
					"The Lair of the Troll",
					"The Demon's Inferno",
					"The Battleground of the Titans",
					"The Coven of Mages",
					"The Inner Sanctum",
					"The Haunt of Nemesis",
					"The Passage to the Surface",
					"Big Jim's Domain",
					"Nolan's Nasty",
				};

void DrawEnterScreen (void)
{
	id0_int_t width;

	bufferofs = displayofs = screenloc[screenpage];
	VW_Bar(0,0,VIEWWIDTH,VIEWHEIGHT,0);
	width = strlen(levelnames[gamestate.mapon]);
	if (width < 20)
		width = 20;
	CenterWindow(width,5);
	US_CPrint("\nYou have arrived at\n");
	US_CPrint(levelnames[gamestate.mapon]);
}

//==========================================================================

id0_boolean_t tileneeded[NUMFLOORS];


/*
==================
=
= CacheScaleds
=
==================
*/

void CacheScaleds (void)
{
	id0_int_t     i,j;
	id0_unsigned_t        source,dest;

	FreeUpMemory ();
	CA_CacheGrChunk(LEVEL1TEXT+mapon);
	ScanText ();

//
// make sure we are displaying screenpage 0
//
	if (screenpage)
	{
		source = screenloc[screenpage];
		dest = screenloc[0];
		VW_ScreenToScreen (source,dest,40,VIEWHEIGHT);
		screenpage = 0;
		VW_SetScreen (dest,0);
		displayofs = dest;
	}

//
// cache wall pictures
//
	for (i=1;i<NUMFLOORS;i++)
		if (tileneeded[i])
		{
			SetupScaleWall (walllight1[i]);
			SetupScaleWall (walllight2[i]);
			SetupScaleWall (walldark1[i]);
			SetupScaleWall (walldark2[i]);
		}

//
// cache the actor pictures
//
	for (i=0;i<NUMLUMPS;i++)
		if (lumpneeded[i])
			for (j=lumpstart[i];j<=lumpend[i];j++)
				SetupScalePic(j);

	source = screenloc[0];
	for (i=1;i<=2;i++)
	{
		dest = screenloc[i];
		VW_ScreenToScreen (source,dest,40,VIEWHEIGHT);
	}

	screenpage = 1;
}

//==========================================================================

/*
==================
=
= SetupGameLevel
=
==================
*/

void SetupGameLevel ()
{
	id0_int_t     x,y/*,i,loop*/;
	id0_unsigned_t        id0_far *map,tile,id0_far *spotptr,spot;

	memset (tileneeded,0,sizeof(tileneeded));
//
// randomize if not a demo
//
#if 0
	if (DemoMode)
	{
		US_InitRndT(false);
		gamestate.difficulty = gd_Normal;
	}
	else
#endif
		US_InitRndT(true);

//
// load the level
//
	CA_CacheMap (gamestate.mapon);

	mapwidth = mapheaderseg[mapon]->width;
	mapheight = mapheaderseg[mapon]->height;

//
// make a lookup table for the maps left edge
//
	spot = 0;
	for (y=0;y<mapheight;y++)
	{
	  farmapylookup[y] = spot;
	  spot += mapwidth;
	}

//
// copy the wall data to a data segment array
//
	memset (tilemap,0,sizeof(tilemap));
	memset (actorat,0,sizeof(actorat));
	map = mapsegs[0];
	spotptr = mapsegs[2];
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *map++;

			if (((*spotptr)>>8) == EXP_WALL_CODE)
			{
				extern id0_unsigned_t gnd_colors[];

				if (gnd_colors[gamestate.mapon] == 0x0101)
					tileneeded[WATEREXP] = tileneeded[WATEREXP+1] = tileneeded[WATEREXP+2] = true;
				else
					tileneeded[WALLEXP] = tileneeded[WALLEXP+1] = tileneeded[WALLEXP+2] = true;
			}

			if (tile<NUMFLOORS)
			{
				if (tile == WALL_SKELETON_CODE)
					tileneeded[tile+1] = tileneeded[tile+2] = true;

				tileneeded[tile] = true;
				tilemap[x][y] = tile;
				if (tile>0)
					actorat[x][y] = tile;
					//(id0_unsigned_t)actorat[x][y] = tile;
			}
			spotptr++;
		}


	//
	// Mark any gfx chunks needed
	//

//	CA_MarkGrChunk(NORTHICONSPR);
//	CA_CacheMarks(NULL);

//
// decide which graphics are needed and spawn actors
//
	zombie_base_delay = 0;	// (1*60) + BE_Cross_Brandom(1*60);
	ScanInfoPlane ();

//
// have the caching manager load and purge stuff to make sure all marks
// are in memory
//
	CA_LoadAllSounds ();
}


//==========================================================================

/*
=====================
=
= LatchDrawPic
=
=====================
*/

void LatchDrawPic (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t picnum)
{
	id0_unsigned_t wide, height, source, dest;

	wide = pictable[picnum-STARTPICS].width;
	height = pictable[picnum-STARTPICS].height;
	dest = bufferofs + ylookup[y]+x;
	source = latchpics[picnum-FIRSTLATCHPIC];

	for (id0_unsigned_t lineCount = height; lineCount; --lineCount)
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest, source, wide);
		source += wide;
		dest += linewidth;
	}
#if 0
	EGAWRITEMODE(1);
	EGAMAPMASK(15);

asm     mov     bx,[linewidth]
asm     sub     bx,[wide]

asm     mov     ax,[screenseg]
asm     mov     es,ax
asm     mov     ds,ax

asm     mov     si,[source]
asm     mov     di,[dest]
asm     mov     dx,[height]                             // scan lines to draw
asm     mov     ax,[wide]

lineloop:
asm     mov     cx,ax
asm     rep     movsb
asm     add     di,bx

asm     dec     dx
asm     jnz     lineloop

asm     mov     ax,ss
asm     mov     ds,ax                                   // restore turbo's data segment

	EGAWRITEMODE(0);
#endif
}


//==========================================================================

/*
=====================
=
= Victory
=
=====================
*/

void Victory (id0_boolean_t playsounds)
{
	//struct Shape shape;

	if (playsounds)
	{
		SD_PlaySound (GETBOLTSND);
		SD_WaitSoundDone ();
		SD_PlaySound (GETNUKESND);
		SD_WaitSoundDone ();
		SD_PlaySound (GETPOTIONSND);
		SD_WaitSoundDone ();
		SD_PlaySound (GETKEYSND);
		SD_WaitSoundDone ();
		SD_PlaySound (GETSCROLLSND);
		SD_WaitSoundDone ();
		SD_PlaySound (GETPOINTSSND);
	}


	FreeUpMemory();

	if (!screenfaded)
		VW_FadeOut();

	screenpage = 1;
	VW_SetScreen (screenloc[screenpage],0);
	bufferofs = displayofs = screenloc[screenpage];
	VW_Bar (0,0,320,120,0);

	CA_CacheGrChunk (FINALEPIC);
	UNMARKGRCHUNK(FINALEPIC);
	VW_DrawPic(0, 0, FINALEPIC);

	VW_FadeIn();

#if 0
	FreeUpMemory();

	if (!screenfaded)
		VW_FadeOut();
	screenpage = 1;
	VW_SetScreen (screenloc[screenpage],0);
	if (!FindFile("FINALE."EXT,NULL,1))
		Quit("Error: Can't find victory screen.");
	if (LoadShape("FINALE."EXT,&shape))
		TrashProg("Can't load FINALE."EXT);
	bufferofs = displayofs = screenloc[screenpage];
	VW_Bar (0,0,320,120,0);
	UnpackEGAShapeToScreen(&shape,(320-shape.bmHdr.w)/2,0);
	FreeShape(&shape);
	VW_FadeIn();
#endif
}

//==========================================================================

#if 0
/*
===================
=
= Died
=
===================
*/

void Died (void)
{
	id0_unsigned_t page1,page2;
//
// fizzle fade screen to grey
//
	FreeUpMemory ();
	SD_PlaySound (GAMEOVERSND);
	bufferofs = screenloc[(screenpage+1)%3];
	DisplayMsg("Though fallen, your Spirit ...",NULL);
//	LatchDrawPic(0,0,DEADPIC);
//	FizzleFade(bufferofs,displayofs,VIEWWIDTH,VIEWHEIGHT,false);
	IN_ClearKeysDown();
	while (!Keyboard[sc_Enter]);
//	IN_Ack();
	VW_SetScreen (bufferofs,0);
	VW_ColorBorder(0);
}
#endif

//==========================================================================

/*
===================
=
= NormalScreen
=
===================
*/

void NormalScreen (void)
{
	 VW_SetSplitScreen (200);
	 bufferofs = displayofs = SCREEN1START;
	 VW_Bar(0,0,320,200,0);
	 bufferofs = SCREEN2START;
	 VW_Bar(0,0,320,200,0);
	 VW_SetScreen (displayofs,0);
	 splitscreen = false;
}

//==========================================================================

/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (void)
{
	id0_int_t     i/*,j,p,m*/;

	screenpage = 0;

	bufferofs = 0;
	VW_Bar (0,0,320,STATUSLINES,0);
	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		VW_Bar (0,0,320,VIEWHEIGHT,0);
	}

	splitscreen = true;
	VW_SetSplitScreen(120);
	VW_SetScreen(screenloc[0],0);

	CA_CacheGrChunk (STATUSPIC);

	bufferofs = 0;
	VW_DrawPic (0,0,STATUSPIC);

	grneeded[STATUSPIC] &= ~ca_levelbit;
	MM_SetPurge(&grsegs[STATUSPIC],3);

	// (REFKEEN) VANILLA BUG REPRODUCTION: In the original code the last
	// call to VW_DrawPic leaves the map mask value at 8 (intensity plane),
	// so numbers aren't drawn in the following call to RedrawStatusWindow.
	// We add a workaround here since we don't store EGA write/read mode
	// related values internally (we almost don't need these).
	id0_workaround_catadventures_nodraw_digits_on_startup = true;
	RedrawStatusWindow ();
	id0_workaround_catadventures_nodraw_digits_on_startup = false;
	bufferofs = displayofs = screenloc[0];
}


//==========================================================================

/*
===================
=
= LoadLatchMem
=
===================
*/

void LoadLatchMem (void)
{
	id0_int_t     i,j,p,m;
	id0_byte_t    id0_far *src/*, id0_far *dest*/;
	id0_unsigned_t        destoff;

	EGAWRITEMODE(0);

//
// draw some pics into latch memory
//

//
// tile 8s
//
	latchpics[0] = freelatch;
	src = (id0_byte_t id0_seg *)grsegs[STARTTILE8];
	// (REFKEEN) Use destoff instead of dest
	destoff = freelatch;
	//dest = MK_FP(0xa000,freelatch);

	for (i=0;i<NUMTILE8;i++)
	{
		for (p=0;p<4;p++)
		{
			m = 1<<p;
			BE_ST_EGAUpdateGFXBuffer(destoff, src, 8, m);
			src+=8;
#if 0
			asm     mov     dx,SC_INDEX
			asm     mov     al,SC_MAPMASK
			asm     mov     ah,[BYTE PTR m]
			asm     out     dx,ax
			for (j=0;j<8;j++)
				*(dest+j)=*src++;
#endif
		}
		destoff+=8;
		//dest+=8;
	}

//
// tile 16s
//
	src = (id0_byte_t id0_seg *)grsegs[STARTTILE16];

	for (i=0;i<NUMTILE16;i++)
	{
		CA_CacheGrChunk (STARTTILE16+i);
		src = (id0_byte_t id0_seg *)grsegs[STARTTILE16+i];
		if (src)
		{
			tileoffsets[i] = destoff;
			//tileoffsets[i] = FP_OFF(dest);
			for (p=0;p<4;p++)
			{
				m = 1<<p;
				BE_ST_EGAUpdateGFXBuffer(destoff, src, 32, m);
				src+=32;
#if 0
				asm     mov     dx,SC_INDEX
				asm     mov     al,SC_MAPMASK
				asm     mov     ah,[BYTE PTR m]
				asm     out     dx,ax
				for (j=0;j<32;j++)
					*(dest+j)=*src++;
#endif
			}
			destoff+=32;
			//dest+=32;
			MM_FreePtr (&grsegs[STARTTILE16+i]);
			UNMARKGRCHUNK(STARTTILE16+i);
		}
		else
			tileoffsets[i] = 0;
	}


//
// pics
//
	// (REFKEEN) We already have destoff now
	//destoff = FP_OFF(dest);
	for (i=FIRSTLATCHPIC+1;i<FIRSTSCALEPIC;i++)
	{
		latchpics[i-FIRSTLATCHPIC] = destoff;
		CA_CacheGrChunk (i);
		j = pictable[i-STARTPICS].width * pictable[i-STARTPICS].height;
		VW_MemToScreen (grsegs[i],destoff,j,1);
		destoff+=j;
		MM_FreePtr (&grsegs[i]);
		UNMARKGRCHUNK(i);
	}

	EGAMAPMASK(15);
}

//==========================================================================

/*
===================
=
= FizzleOut
=
===================
*/

void FizzleOut (id0_int_t showlevel)
{
	//id0_unsigned_t page1,page2;
//
// fizzle fade screen to grey
//
	bufferofs = screenloc[(screenpage+1)%3];
	if (showlevel)
		DrawEnterScreen ();
	FizzleFade(bufferofs,displayofs,VIEWWIDTH,VIEWHEIGHT,false);
}

//==========================================================================

/*
====================
=
= FreeUpMemory
=
====================
*/

void FreeUpMemory (void)
{
	id0_int_t     i;

	for (i=0;i<NUMSCALEPICS;i++)
		if (shapedirectory[i])
			MM_SetPurge ((memptr *)&shapedirectory[i],3);

	for (i=0;i<NUMSCALEWALLS;i++)
		if (walldirectory[i])
			MM_SetPurge ((memptr *)&walldirectory[i],3);
}

//==========================================================================

#if 0

/*
==================
=
= DrawHighScores
=
==================
*/

void    DrawHighScores(void)
{
	id0_char_t            buffer[16],*str;
	id0_word_t            i,j,
				w,h,
				x,y;
	HighScore       *s;


	CA_CacheGrChunk (HIGHSCORESPIC);
	VWB_DrawPic (0,0,HIGHSCORESPIC);
	MM_SetPurge (&grsegs[HIGHSCORESPIC],3);
	UNMARKGRCHUNK(HIGHSCORESPIC);

	for (i = 0,s = Scores;i < MaxScores;i++,s++)
	{
		PrintY = 68 + (16 * i);

		//
		// name
		//
		PrintX = 60;
		US_Print(s->name);

		//
		// level
		//
		BE_Cross_ultoa_dec(s->completed, buffer);
		//ultoa(s->completed,buffer,10);
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');      // Used fixed-width numbers (129...)
		USL_MeasureString(buffer,NULL,&w,&h);
		PrintX = (25 * 8) - 8 - w;
		US_Print(buffer);

		//
		// score
		//
		BE_Cross_ultoa_dec(s->score, buffer);
		//ultoa(s->score,buffer,10);
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');      // Used fixed-width numbers (129...)
		USL_MeasureString(buffer,NULL,&w,&h);
		PrintX = (34 * 8) - 8 - w;
		US_Print(buffer);
	}

	fontcolor = F_BLACK;
}



/*
=======================
=
= CheckHighScore
=
=======================
*/

void    CheckHighScore (id0_long_t score,id0_word_t other)
{
	id0_word_t            i,j;
	id0_int_t                     n;
	HighScore       myscore;

	strcpy(myscore.name,"");
	myscore.score = score;
	myscore.completed = other;

	for (i = 0,n = -1;i < MaxScores;i++)
	{
		if
		(
			(myscore.score > Scores[i].score)
		||      (
				(myscore.score == Scores[i].score)
			&&      (myscore.completed > Scores[i].completed)
			)
		)
		{
			for (j = MaxScores;--j > i;)
				Scores[j] = Scores[j - 1];
			Scores[i] = myscore;
			n = i;
			HighScoresDirty = true;
			break;
		}
	}

	if (n != -1)
	{
	//
	// got a high score
	//
		DrawHighScores ();
		PrintY = 68 + (16 * n);
		PrintX = 60;
		US_LineInput(PrintX,PrintY,Scores[n].name,nil,true,MaxHighName,100);
	}
}

#endif


//==========================================================================

/*
===================
=
= GameLoop
=
===================
*/

void GameLoop (void)
{
	id0_boolean_t wait = false;
	//id0_int_t i,xl,yl,xh,yh;
	//id0_char_t num[20];
#ifdef PROFILE
	clock_t start,end;
#endif

	DrawPlayScreen ();
	IN_ClearKeysDown();

restart:
	if (!loadedgame)
	{
		gamestate.difficulty = restartgame;
		restartgame = gd_Continue;
		DrawEnterScreen ();
		if (gamestate.mapon != 8)
			fizzlein = true;
		wait = true;
	}

	do
	{
		// REFKEEN - Looks like a hack (cast added for C++)
		playstate = (exittype)gd_Continue;
		if (!loadedgame)
			SetupGameLevel ();
		else
			loadedgame = false;

		FreeUpMemory();
		LoadLatchMem();
		CacheScaleds ();

		// (REFKEEN) Simulate a short wait while loading (includes vanilla bug: numbers not shown in the HUD on startup immediately)
		VW_WaitVBL(25);
		//

		if (EASYMODEON)
			DisplaySMsg("*** NOVICE ***", NULL);
		else
			DisplaySMsg("*** WARRIOR ***", NULL);

		status_delay = 250;

		RedrawStatusWindow();
		if (wait)
		{
			VW_WaitVBL(120);
			wait = false;
		}


#ifdef PROFILE
start = clock();
while (start == clock());
start++;
#endif

		PlayLoop ();

#ifdef PROFILE
end = clock();
itoa(end-start,str,10);
		Quit (str);
#endif


		switch (playstate)
		{
		case ex_abort:
			FreeUpMemory ();
			return;
		case ex_resetgame:
			NewGame();
		case ex_loadedgame:
		case ex_warped:
			goto restart;
		break;
		}

	} while (1);

}


#if 0
//
// make wall pictures purgable
//
	for (i=0;i<NUMSCALEWALLS;i++)
		if (walldirectory[i])
			MM_SetPurge ((memptr *)walldirectory[i],3);


//
// cache wall pictures back in
//
	for (i=1;i<NUMFLOORS;i++)
		if (tileneeded[i])
		{
			SetupScaleWall (walllight1[i]);
			SetupScaleWall (walllight2[i]);
			SetupScaleWall (walldark1[i]);
			SetupScaleWall (walldark2[i]);
		}
#endif
