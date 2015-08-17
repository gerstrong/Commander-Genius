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

#define NUMLUMPS        61

#define SUCCUBUSLUMP	0
#define FATDEMONLUMP	1
#define BOLTLUMP	2
#define NUKELUMP	3
#define POTIONLUMP	4
#define RKEYLUMP        5
#define YKEYLUMP        6
#define GKEYLUMP        7
#define BKEYLUMP        8
//#define SCROLLLUMP	9
#define CHESTLUMP       10
#define PLAYERLUMP      11
#define WALL1LUMP       12
#define WALL2LUMP       13
#define BDOORLUMP       14
#define GODESSLUMP	15
#define MAGELUMP	16
#define BATLUMP		17
#define GRELLUMP	18
#define TOMBSTONESLUMP	19
#define ZOMBIELUMP	20
#define ANTLUMP		21
#define SKELETONLUMP	22
#define RGEMLUMP	23
#define GGEMLUMP	24
#define BGEMLUMP	25
#define YGEMLUMP	26
#define PGEMLUMP	27
//#define RKEY2LUMP	28
#define DRAGONLUMP	29
#define OBJ_WARPLUMP	30
#define EYELUMP		31
#define REDDEMONLUMP	32
//#define PITLUMP	33
#define FTIMELUMP	34
#define WATERCHESTLUMP	35
#define TREELUMP        36
#define ARCH1LUMP       37
#define BUNNYLUMP       38
#define ANTHILLLUMP     39
#define COLUMNLUMP      40
#define SULPHURGASLUMP  41
#define FIREPOTLUMP     42
//#define WHIRLPOOLLUMP	43
#define FOUNTAINLUMP    44
#define FORCEFIELDLUMP  45
#define ARCH2LUMP       46
#define ARCH3LUMP       47
#define ARCH4LUMP       48
#define ARCH5LUMP       49
#define ARCH6LUMP       50
#define SKELHANGLUMP    51
//#define SKELPILELUMP	52
#define ARCH7LUMP       53
#define ARCH8LUMP       54
#define ARCH9LUMP       55
#define ARCH10LUMP      56
#define ARCH11LUMP      57
#define ARCH12LUMP	58
#define ARCH13LUMP	59

id0_int_t     lumpstart[NUMLUMPS] = {
SUCCUBUS_LUMP_START,
FATDEMON_LUMP_START,
BOLT_LUMP_START,
NUKE_LUMP_START,
POTION_LUMP_START,
RKEY_LUMP_START,
YKEY_LUMP_START,
GKEY_LUMP_START,
BKEY_LUMP_START,
0,
//SCROLL_LUMP_START,
CHEST_LUMP_START,
PLAYER_LUMP_START,
//WALL1_LUMP_START,
//WALL2_LUMP_START,
//BDOOR_LUMP_START,
0,0,0,
GODESS_LUMP_START,
MAGE_LUMP_START,
BAT_LUMP_START,
GREL_LUMP_START,
TOMBSTONES_LUMP_START,
ZOMBIE_LUMP_START,
ANT_LUMP_START,
SKELDUDE_LUMP_START,
RGEM_LUMP_START,
GGEM_LUMP_START,
BGEM_LUMP_START,
YGEM_LUMP_START,
PGEM_LUMP_START,
0,					//RKEY2_LUMP_START,
DRAGON_LUMP_START,
OBJ_WARP_LUMP_START,
EYE_LUMP_START,
REDDEMON_LUMP_START,
0,					//PIT_LUMP_START,
TIME_LUMP_START,
O_WATER_CHEST_LUMP_START,
TREE_LUMP_START,
ARCH1_LUMP_START,
BUNNY_LUMP_START,
ANTHILL_LUMP_START,
COLUMN_LUMP_START,
SULPHURGAS_LUMP_START,
FIREPOT_LUMP_START,
0,					//WHIRLPOOL_LUMP_START,
FOUNTAIN_LUMP_START,
FORCEFIELD_LUMP_START,
ARCH2_LUMP_START,
ARCH3_LUMP_START,
ARCH4_LUMP_START,
ARCH5_LUMP_START,
ARCH6_LUMP_START,
SKELHANG_LUMP_START,
0,					//SKELPILE_LUMP_START,
ARCH7_LUMP_START,
ARCH8_LUMP_START,
ARCH9_LUMP_START,
ARCH10_LUMP_START,
ARCH11_LUMP_START,
ARCH12_LUMP_START,
ARCH13_LUMP_START,
};


id0_int_t     lumpend[NUMLUMPS] = {
SUCCUBUS_LUMP_END,
FATDEMON_LUMP_END,
BOLT_LUMP_END,
NUKE_LUMP_END,
POTION_LUMP_END,
RKEY_LUMP_END,
YKEY_LUMP_END,
GKEY_LUMP_END,
BKEY_LUMP_END,
0,
//SCROLL_LUMP_END,
CHEST_LUMP_END,
PLAYER_LUMP_END,
0,0,0,
GODESS_LUMP_END,
MAGE_LUMP_END,
BAT_LUMP_END,
GREL_LUMP_END,
TOMBSTONES_LUMP_END,
ZOMBIE_LUMP_END,
ANT_LUMP_END,
SKELDUDE_LUMP_END,
RGEM_LUMP_END,
GGEM_LUMP_END,
BGEM_LUMP_END,
YGEM_LUMP_END,
PGEM_LUMP_END,
0,					//RKEY2_LUMP_END,
DRAGON_LUMP_END,
OBJ_WARP_LUMP_END,
EYE_LUMP_END,
REDDEMON_LUMP_END,
0,					//PIT_LUMP_END,
TIME_LUMP_END,
O_WATER_CHEST_LUMP_END,
TREE_LUMP_END,
ARCH1_LUMP_END,
BUNNY_LUMP_END,
ANTHILL_LUMP_END,
COLUMN_LUMP_END,
SULPHURGAS_LUMP_END,
FIREPOT_LUMP_END,
0,					//WHIRLPOOL_LUMP_END,
FOUNTAIN_LUMP_END,
FORCEFIELD_LUMP_END,
ARCH2_LUMP_END,
ARCH3_LUMP_END,
ARCH4_LUMP_END,
ARCH5_LUMP_END,
ARCH6_LUMP_END,
SKELHANG_LUMP_END,
0,					//SKELPILE_LUMP_END,
ARCH7_LUMP_END,
ARCH8_LUMP_END,
ARCH9_LUMP_END,
ARCH10_LUMP_END,
ARCH11_LUMP_END,
ARCH12_LUMP_END,
ARCH13_LUMP_END,
};


//extern id0_unsigned_t scolor,gcolor;


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
//                                                      LOCAL PROTOTYPES
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
	id0_unsigned_char_t hibyte;
	id0_unsigned_t        x,y/*,i,j*/;
	id0_unsigned_int_t tile;
	id0_unsigned_t        id0_far     *start;

	InitObjList();                  // start spawning things with a clean slate

	scolor = gcolor = 0;
	skycolor = &scolor;
	groundcolor = &gcolor;


	memset (lumpneeded,0,sizeof(lumpneeded));

	start = mapsegs[2];
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *start++;
			hibyte = tile >> 8;
			tile &= 0xff;

			switch (hibyte)
			{
				id0_char_t hi;

				case 0xFB:
					wall_anim_time = tile;
					tile = 0;
					break;

				case 0xfa:								// sky/ground color
				case 0xf9:								// sky/ground 'strip'
					x++;
					tile = *start++;
					hi = tile >> 8;
					tile &= 0xff;
					switch (hibyte)
					{
						case 0xfa:			// sky / ground color
							scolor = ((hi)|(hi<<8));
							gcolor = ((tile)|(tile<<8));
							skycolor = &scolor;
							groundcolor = &gcolor;
						break;

						case 0xf9:			// sky / ground 'strip'
						break;
					}
				break;
			}

			if ((!tile) || (hibyte))
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

#if 0
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
#endif

			case 20:
				lumpneeded[REDDEMONLUMP] = true;
				SpawnRedDemon (x,y);
				break;

#if 0
			case 20:        // goal
				lumpneeded[GOALLUMP] = true;
				SpawnBonus(x,y,B_GOAL);
				break;
#endif

			case 21:
				lumpneeded[GODESSLUMP] = true;
				SpawnGodess (x,y);
				break;

			case 22:
				lumpneeded[FATDEMONLUMP] = true;
				SpawnFatDemon (x,y);
				break;

			case 23:
				lumpneeded[SUCCUBUSLUMP] = true;
				SpawnSuccubus (x,y);
				break;

			case 24:
				lumpneeded[DRAGONLUMP] = true;
				SpawnDragon(x,y);
				break;

			case 25:
				lumpneeded[BATLUMP] = true;
				SpawnBat (x,y);
				break;

			case 26:
				lumpneeded[EYELUMP] = true;
				SpawnEye(x,y);
				break;

			case 27:
				lumpneeded[MAGELUMP] = true;
				SpawnMage (x,y);
				break;

			case 28:
				lumpneeded[RKEYLUMP] = lumpneeded[GRELLUMP] = true;
				SpawnGrelminar (x,y);
				break;

			case 30:
				lumpneeded[ANTLUMP] = true;
				SpawnAnt(x,y);
				break;

			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
				lumpneeded[OBJ_WARPLUMP] = true;
				SpawnWarp (x,y,tile-30);
				break;

			case 36:
				lumpneeded[ZOMBIELUMP] = true;
				SpawnZombie(x,y);
				break;

			case 37:
				lumpneeded[SKELETONLUMP] = true;
				SpawnSkeleton(x,y);
				break;

			case 38:
				lumpneeded[SKELETONLUMP] = true;
				SpawnWallSkeleton(x,y);
				break;

			case 39:
				lumpneeded[FTIMELUMP] = true;
				SpawnFTime(x,y);
				break;

			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
				lumpneeded[tile-40+RGEMLUMP] = true;
				SpawnBonus(x,y,tile-40+B_RGEM);
			break;

			case 45:
			case 46:
			case 47:
				lumpneeded[TOMBSTONESLUMP] = true;
				SpawnTombstone(x,y,tile-45);
				break;

#if 0
			case 48:
				lumpneeded[PITLUMP]     = true;
				SpawnWarp(x,y,0);
				break;
#endif
			case 49:        // chest
				if (gcolor == 0x0101)
					lumpneeded[WATERCHESTLUMP] = true;
				else
					lumpneeded[CHESTLUMP] = true;
				SpawnBonus(x,y,B_CHEST);
			break;

			case 50:
				lumpneeded[TREELUMP] = true;
				SpawnTree(x,y);
				break;

			case 51:
				lumpneeded[BUNNYLUMP] = true;
				SpawnBunny(x,y);
				break;

			case 52:
				lumpneeded[ARCH1LUMP] = true;
				SpawnArch(x,y,1);
				break;

			case 53:
				lumpneeded[ANTHILLLUMP] = true;
				SpawnWarp(x,y,0);
				break;

			case 54:
				lumpneeded[COLUMNLUMP] = true;
				SpawnMiscObjects(x,y,1);		//1=column,2=sulphur hole,3=fire pot,4=fountain
				break;

			case 55:
				lumpneeded[SULPHURGASLUMP] = true;
				SpawnMiscObjects(x,y,2);
				break;

			case 56:
				lumpneeded[FIREPOTLUMP] = true;
				SpawnMiscObjects(x,y,3);
				break;

			case 57:
				lumpneeded[ARCH13LUMP] = true;
				SpawnArch(x,y,13);
				break;

			case 58:
				lumpneeded[FOUNTAINLUMP] = true;
				SpawnMiscObjects(x,y,4);
				break;

			case 59:
				lumpneeded[FORCEFIELDLUMP] = true;
				SpawnForceField(x,y);
				break;

			case 60:
				lumpneeded[ARCH2LUMP] = true;
				SpawnArch(x,y,2);
				break;

			case 61:
				lumpneeded[ARCH3LUMP] = true;
				SpawnArch(x,y,3);
				break;

			case 62:
				lumpneeded[ARCH4LUMP] = true;
				SpawnArch(x,y,4);
				break;

			case 63:
				lumpneeded[ARCH5LUMP] = true;
				SpawnArch(x,y,5);
				break;

			case 64:
				lumpneeded[ARCH6LUMP] = true;
				SpawnArch(x,y,6);
				break;

			case 65:
				lumpneeded[SKELHANGLUMP] = true;
				lumpneeded[SKELETONLUMP] = true;
				SpawnSkeletonHanging(x,y);
				break;

			case 66:
				lumpneeded[ARCH12LUMP] = true;
				SpawnArch(x,y,12);
				break;

			case 67:
				lumpneeded[ARCH7LUMP] = true;
				SpawnArch(x,y,7);
				break;

			case 68:
				lumpneeded[ARCH8LUMP] = true;
				SpawnArch(x,y,8);
				break;

			case 69:
				lumpneeded[ARCH9LUMP] = true;
				SpawnArch(x,y,9);
				break;

			case 70:
				lumpneeded[ARCH10LUMP] = true;
				SpawnArch(x,y,10);
				break;

			case 71:
				lumpneeded[ARCH11LUMP] = true;
				SpawnArch(x,y,11);
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
#if 0
static  id0_char_t    *levelnames[] =
				{
					"Programmers Test Map",
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
					"Nolan",
					"19",
					"20",
					"21",
					"22",
					"23",
					"24",
					"25",
					"26",
					"27",
					"28",
					"29",
					"30",
					"31",
					"32",
					"33",
					"34",
					"35",
					"36",
					"37",
					"38",
					"39",
				};
#endif

void DrawEnterScreen (void)
{
	id0_int_t width;

	bufferofs = displayofs = screenloc[screenpage];
	VW_Bar(0,0,VIEWWIDTH,VIEWHEIGHT,0);
//	width = strlen(levelnames[gamestate.mapon]);
	width = strlen("You enter a new area ...");
	if (width < 20)
		width = 20;
	CenterWindow(width,3);
	US_CPrint("\nYou enter a new area ...\n");
//	US_CPrint(levelnames[gamestate.mapon]);
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
			SetupScaleWall (walldark1[i]);
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
	id0_unsigned_t                search_tile;
	id0_boolean_t		exploding_walls_present = false;

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
				exploding_walls_present = true;
			}

			if (tile<NUMFLOORS)
			{
#if 0
				if (tile == WALL_SKELETON_CODE)
				{
					tileneeded[tile+1] = tileneeded[tile+2] = true;
					tilemap[x][y] = tile;
				}
#endif
				if ((tile == 66) || (tile == 67) || (tile == 68) || (tile == 69))
				{
					if ((tile == 66) || (tile == 67))
						tileneeded[tile+2] = true;
					tileneeded[21] = tileneeded[tile] = true;
					tilemap[x][y] = tile;
				}
				else
				if (tile != INVISIBLEWALL)
				{
					tileneeded[tile] = true;
					tilemap[x][y] = tile;
					if (ANIM_FLAGS(tile))
					{
						search_tile = tile+(id0_char_signed_t)ANIM_FLAGS(tile);

						if (!tileneeded[search_tile])
							while (search_tile != tile)
							{
								tileneeded[search_tile] = true;
								if (ANIM_FLAGS(search_tile))
									search_tile += (id0_char_signed_t)ANIM_FLAGS(search_tile);
								else
									TrashProg("Unending Tile Animation!");
							}
					}

				}
				if (tile>0)
					actorat[x][y] = tile;
					//(id0_unsigned_t)actorat[x][y] = tile;
			}
			spotptr++;
		}


	//
	// Mark any gfx chunks needed
	//

//      CA_MarkGrChunk(NORTHICONSPR);
//      CA_CacheMarks(NULL);


//
// decide which graphics are needed and spawn actors
//
	zombie_base_delay = 0;  // (1*60) + BE_Cross_Brandom(1*60);
	ScanInfoPlane ();
	memset(wall_anim_pos,0,sizeof(wall_anim_pos));
	//_fmemset(wall_anim_pos,0,sizeof(wall_anim_pos));


//
// mark which exploding walls are needed ---- the check for floor color
// is preformed in ScanInfoPlane.
//

	if (exploding_walls_present)
	{
				//extern id0_unsigned_t gnd_colors[];

				if (gcolor == 0x0101)
					tileneeded[WATEREXP] = tileneeded[WATEREXP+1] = tileneeded[WATEREXP+2] = true;
				else
					tileneeded[WALLEXP] = tileneeded[WALLEXP+1] = tileneeded[WALLEXP+2] = true;

	}


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
	id0_unsigned_t height, source, dest;
	id0_unsigned_t wide;

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

#if USE_STRIPS

//--------------------------------------------------------------------------
// LatchDrawPicStrip() - srcoff is distance into source file (in PIXELS!)
//--------------------------------------------------------------------------
void LatchDrawPicStrip (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t picnum, id0_unsigned_t srcoff)
{
	id0_unsigned_t wide, height, source, dest, shift, srcmod;

	shift = (srcoff & 7) >> 1;
	srcoff >>= 3;
	wide = pictable[picnum-STARTPICS].width;
	srcmod = wide - linewidth + (shift != 3);
	if (wide > linewidth)
		wide = linewidth;
	height = pictable[picnum-STARTPICS].height;
	dest = bufferofs + ylookup[y]+x;

	picnum = ((picnum - (FIRSTSTRIPPIC+1)) >> 2) + (shift);
	source = latchpics[(FIRSTSTRIPPIC-FIRSTLATCHPIC+1)+picnum];

	EGAWRITEMODE(1);
	EGAMAPMASK(15);

asm     mov     bx,[linewidth]
asm     sub     bx,[wide]

asm     mov     ax,[screenseg]
asm     mov     es,ax
asm     mov     ds,ax

asm     mov     si,[source]
asm	  add		 si,[srcoff]
asm     mov     di,[dest]
asm     mov     dx,[height]                             // scan lines to draw
asm     mov     ax,[wide]

lineloop:
asm     mov     cx,ax
asm     rep     movsb
asm     add     di,bx
asm	  add     si,[srcmod]

asm     dec     dx
asm     jnz     lineloop

asm     mov     ax,ss
asm     mov     ds,ax                                   // restore turbo's data segment

	EGAWRITEMODE(0);
}

#endif


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

	NormalScreen ();

	screenpage = bufferofs = 0;

	CA_CacheGrChunk (FINALEPIC);
	UNMARKGRCHUNK(FINALEPIC);
	VW_DrawPic(0, 0, FINALEPIC);

	VW_FadeIn();
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
//      LatchDrawPic(0,0,DEADPIC);
//      FizzleFade(bufferofs,displayofs,VIEWWIDTH,VIEWHEIGHT,false);
	IN_ClearKeysDown();
	while (!Keyboard[sc_Enter]);
//      IN_Ack();
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

id0_unsigned_t latchmemavail;

void LoadLatchMem (void)
{
	static id0_unsigned_t base_destoff=0;
	static id0_int_t base_numpics=0;
	id0_int_t     i,j,p,m,numpics;
	id0_byte_t    id0_far *src/*, id0_far *dest*/;
	id0_unsigned_t        destoff;

	EGAWRITEMODE(0);

//
// draw some pics into latch memory
//

  if (!base_numpics)
  {

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
	numpics=1;
	// (REFKEEN) We already have destoff now
	//destoff = FP_OFF(dest);
	for (i=FIRSTLATCHPIC+1;i<FIRSTGROUNDPIC;i++)
	{
		latchpics[numpics++] = destoff;
		CA_CacheGrChunk (i);
		j = pictable[i-STARTPICS].width * pictable[i-STARTPICS].height;
		VW_MemToScreen (grsegs[i],destoff,j,1);
		destoff+=j;
		MM_FreePtr (&grsegs[i]);
		UNMARKGRCHUNK(i);
	}

	base_numpics = numpics;
	base_destoff = destoff;

  }

	numpics = base_numpics;
	destoff = base_destoff;

#if USE_STRIPS
//
// ground pics
//
	numpics++;
	for (i=FIRSTGROUNDPIC+1;i<FIRSTSTRIPPIC;i++)
	{
		id0_int_t shape = (*groundcolor & 0xf0) - 16;

	// Is current shape needed?
	//
		if (shape != (i-(FIRSTGROUNDPIC+1)))
		{
			numpics++;
			continue;
		}

		latchpics[numpics++] = destoff;
		CA_CacheGrChunk (i);
		j = pictable[i-STARTPICS].width * pictable[i-STARTPICS].height;
		VW_MemToScreen (grsegs[i],destoff,j,1);
		destoff+=j;
		MM_FreePtr (&grsegs[i]);
		UNMARKGRCHUNK(i);
	}


//
// 'parallax' strips - used in place of a sky color
//
// Under current setup, each strip takes about 7k in latch memory.
// To create 2 pixel scrolling, 4 strips are needed, that's 28k of
// latch memory needed to produce this effect.
//
	numpics++;
	for (i=FIRSTSTRIPPIC+1;i<FIRSTSCALEPIC;i++)
	{
		memptr work;
		id0_unsigned_t workdest,stripsize,planesize;
		id0_short_t loop,pic=i-STARTPICS;
		id0_int_t shape = (*skycolor & 0xf0) - 16;

	// Is current shape needed?
	//
		if (shape != (i-(FIRSTSTRIPPIC+1)))
		{
			numpics++;
			continue;
		}

	// CAL_ShiftSprite() works with the SRC and DST in the same
	// segment. So we must allocate memory for two strips, and
	// move the base strip into that segment. Then we can use the
	// 2nd half of that memory for each shifted strip.
	//
		CA_CacheGrChunk (i);
		planesize = (pictable[pic].width+1) * pictable[pic].height;
		stripsize = planesize * 4;
//		MM_GetPtr(&work,(stripsize*2)+0000);
		MM_GetPtr(&work,65536);
		movedata((id0_unsigned_t)grsegs[i],0,(id0_unsigned_t)work,0,stripsize);
		workdest = 32768; //(stripsize+15) & 0xFFF0;

	// Free base strip
	//
		MM_FreePtr (&grsegs[i]);
		UNMARKGRCHUNK(i);

	// Create three shifted strips and move 'em to latch!
	//
		for (loop=3; loop; loop--)
		{
		// Produce current shift for this strip
		//
			latchpics[numpics++] = destoff;
			CAL_ShiftSprite ((id0_unsigned_t)work,0,workdest,pictable[pic].width,
								  pictable[pic].height,loop*2,false);

		// Copy this shift to latch memory
		//
			VW_MemToScreen ((memptr)((id0_unsigned_t)work+(workdest>>4)),destoff,planesize,1);
			destoff+=planesize;
		}

	// Copy unshifted strip to latch
	//
		latchpics[numpics++] = destoff;
		planesize = pictable[pic].width * pictable[pic].height;
		VW_MemToScreen (work,destoff,planesize,1);
		destoff+=planesize;

	// Free work buffer
	//
		MM_FreePtr(&work);
	}
#endif

// Keep track of how much latch memory we have...
//
	latchmemavail = 65535-destoff;

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
		ultoa(s->completed,buffer,10);
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');      // Used fixed-width numbers (129...)
		USL_MeasureString(buffer,&w,&h);
		PrintX = (25 * 8) - 8 - w;
		US_Print(buffer);

		//
		// score
		//
		ultoa(s->score,buffer,10);
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');      // Used fixed-width numbers (129...)
		USL_MeasureString(buffer,&w,&h);
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
			FreeUpMemory();
			if (playstate != ex_resetgame)
				DisplayMsg("                                      ", NULL);
			DisplaySMsg("                  ", NULL);
			goto restart;
		case ex_victorious:
			screenpage = 0;
			bufferofs = 0;
			status_flag = S_NONE/*0*/;
			return;
		}

	} while (1);

}


#if 0
//
// make wall pictures purgable
//
	for (i=0;i<NUMSCALEWALLS;i++)
		if (walldirectory[i])
			MM_SetPurge ((memptr *)&walldirectory[i],3);


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
