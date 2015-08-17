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

// C3_GAME.C

#include "c3_def.h"
//#pragma hdrstop

#ifdef PROFILE
#include "time.h"
#endif


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define NUMLUMPS        25

#define CONTROLSLUMP    0
#define ORCLUMP         1
#define TROLLLUMP        2
#define WARPLUMP        3
#define BOLTLUMP        4
#define NUKELUMP        5
#define POTIONLUMP      6
#define RKEYLUMP        7
#define YKEYLUMP        8
#define GKEYLUMP        9
#define BKEYLUMP        10
#define SCROLLLUMP      11
#define CHESTLUMP       12
#define PLAYERLUMP      13
#define WALL1LUMP       14
#define WALL2LUMP       15
#define BDOORLUMP       16
#define DEMONLUMP               17
#define MAGELUMP                18
#define BATLUMP                 19
#define GRELLUMP                20
#define GOALLUMP                21


id0_int_t     lumpstart[NUMLUMPS] = {
CONTROLS_LUMP_START,
ORC_LUMP_START,
TROLL_LUMP_START,
WARP_LUMP_START,
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
WALL1_LUMP_START,
WALL2_LUMP_START,
BDOOR_LUMP_START,
DEMON_LUMP_START,
MAGE_LUMP_START,
BAT_LUMP_START,
GREL_LUMP_START,
NEMESISPIC
};


id0_int_t     lumpend[NUMLUMPS] = {
CONTROLS_LUMP_END,
ORC_LUMP_END,
TROLL_LUMP_END,
WARP_LUMP_END,
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
WALL1_LUMP_END,
WALL2_LUMP_END,
BDOOR_LUMP_END,
DEMON_LUMP_END,
MAGE_LUMP_END,
BAT_LUMP_END,
GREL_LUMP_END,
NEMESISPIC
};



/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_unsigned_t        latchpics[NUMLATCHPICS];
id0_unsigned_t        tileoffsets[NUMTILE16];
id0_unsigned_t        textstarts[27];

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_boolean_t lumpneeded[NUMLUMPS];


//===========================================================================


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

			case 20:        // goal
				lumpneeded[GOALLUMP] = true;
				SpawnBonus(x,y,B_GOAL);
				break;

			case 21:        // chest
				lumpneeded[CHESTLUMP] = true;
				SpawnBonus(x,y,B_CHEST);
				break;

			case 24:
				lumpneeded[WARPLUMP] = true;
				SpawnWarp (x,y,0);
				break;
//------
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
				lumpneeded[GRELLUMP] = true;
				SpawnNemesis (x,y);
				break;

			case 29:
				SpawnBounce (x,y,0);
				break;

			case 30:
				SpawnBounce (x,y,1);
				break;

			case 31:
			case 32:
			case 33:
			case 34:
				lumpneeded[WARPLUMP] = true;
				SpawnWarp (x,y,tile-30);
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
					"The Approach",
					"Nemesis's Keep",
					"Ground Floor",
					"Second Floor",
					"Third Floor",
					"Tower One",
					"Tower Two",
					"Secret Halls",
					"Access Floor",
					"The Dungeon",
					"Lower Dungeon",
					"Catacomb",
					"Lower Reaches",
					"The Warrens",
					"Hidden Caverns",
					"The Fens of Insanity",
					"Chaos Corridors",
					"The Labyrinth",
					"Halls of Blood",
					"Nemesis's Lair"
	// REFKEEN - Vanilla Cat3D behavior reproduction:
	// If F10+E cheat is used in the very last map, this is called with
	// the following map, for which no title is shown,
	// So, rather than undefined behaviors (possibly a crash),
	// print empty string.
					, ""
				};
void DrawEnterScreen (void)
{
	id0_int_t     x,y;

	VW_Bar(0,0,VIEWWIDTH,VIEWHEIGHT,9);     // Medium blue

	x = (VIEWWIDTH - (18 * 8)) / 2 -3;
	y = (VIEWHEIGHT - (5 * 8)) / 2;
	VW_DrawPic(x / 8,y,ENTERPLAQUEPIC);

	WindowX = x;
	WindowW = 18 * 8;
	PrintY = (VIEWHEIGHT/2) + 3;
	US_CPrint (levelnames[gamestate.mapon]);
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

void SetupGameLevel (void)
{
	id0_int_t     x,y/*,i*/;
	id0_unsigned_t        id0_far *map,tile,spot;

	memset (tileneeded,0,sizeof(tileneeded));
//
// randomize if not a demo
//
	if (DemoMode)
	{
		US_InitRndT(false);
		gamestate.difficulty = gd_Normal;
	}
	else
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
	for (y=0;y<mapheight;y++)
		for (x=0;x<mapwidth;x++)
		{
			tile = *map++;
			if (tile<NUMFLOORS)
			{
				tileneeded[tile] = true;
				tilemap[x][y] = tile;
				if (tile>=EXPWALLSTART && tile<EXPWALLSTART+NUMEXPWALLS)
				{
					tileneeded[WALLEXP] = tileneeded[WALLEXP+1]
					= tileneeded[WALLEXP+2] = true;
				}

				if (tile>0)
					actorat[x][y] = tile;
					//(id0_unsigned_t)actorat[x][y] = tile;
			}
		}


//
// decide which graphics are needed and spawn actors
//
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

void Victory (void)
{
	FreeUpMemory ();
	NormalScreen ();
	CA_CacheGrChunk (FINALEPIC);
	VWB_DrawPic (0,0,FINALEPIC);
	UNMARKGRCHUNK(FINALEPIC);
	VW_UpdateScreen ();
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
	SD_WaitSoundDone ();
	IN_ClearKeysDown ();
	IN_Ack();
}

//==========================================================================

/*
===================
=
= Died
=
===================
*/

void Died (void)
{
	//id0_unsigned_t page1,page2;
//
// fizzle fade screen to grey
//
	FreeUpMemory ();
	SD_PlaySound (GAMEOVERSND);
	bufferofs = screenloc[(screenpage+1)%3];
	LatchDrawPic(0,0,DEADPIC);
	FizzleFade(bufferofs,displayofs,VIEWWIDTH,VIEWHEIGHT,false);
	IN_ClearKeysDown();
	IN_Ack();
	VW_SetScreen (bufferofs,0);
}

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
	VW_Bar (0,0,320,STATUSLINES,7);
	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		VW_Bar (0,0,320,VIEWHEIGHT,7);
	}


	VW_SetSplitScreen(144);
	VW_SetScreen(screenloc[0],0);
	bufferofs = 0;

	CA_CacheGrChunk (STATUSPIC);
	CA_CacheGrChunk (SIDEBARSPIC);

	VW_DrawPic (0,0,STATUSPIC);

	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		VW_DrawPic (33,0,SIDEBARSPIC);
	}

	grneeded[STATUSPIC]&= ~ca_levelbit;
	grneeded[SIDEBARSPIC]&= ~ca_levelbit;
	MM_SetPurge(&grsegs[STATUSPIC],3);
	MM_SetPurge(&grsegs[SIDEBARSPIC],3);

	RedrawStatusWindow ();
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
= FizzleFade
=
===================
*/

#define PIXPERFRAME     1600

void FizzleFade (id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t width,id0_unsigned_t height, id0_boolean_t abortable)
{
	id0_unsigned_t        drawofs,pagedelta;
	id0_unsigned_char_t maskb[8] = {1,2,4,8,16,32,64,128};
	id0_unsigned_t        x,y,p,frame;
	id0_long_t            rndval;

	pagedelta = dest-source;
	VW_SetScreen (dest,0);
	rndval = 1;
	y = 0;

#if 0
asm     mov     es,[screenseg]
asm     mov     dx,SC_INDEX
asm     mov     al,SC_MAPMASK
asm     out     dx,al
#endif

	frame=0;
	SD_SetTimeCount(0);
	//TimeCount=frame=0;
	do      // while (1)
	{
		if (abortable)
		{
			IN_ReadControl(0,&c);
			if (c.button0 || c.button1 || Keyboard[sc_Space]
			|| Keyboard[sc_Enter])
			{
				VW_ScreenToScreen (source,dest,width/8,height);
				return;
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
			{
				EGABITMASK(255);
				EGAMAPMASK(15);
				return;
			};
		}
		frame++;
		BE_ST_TimeCountWaitForDest(frame);
#if 0
		while (TimeCount<frame)         // don't go too fast
		;
#endif
	} while (1);


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
	id0_word_t            i/*,j*/,
				w,h/*,
				x,y*/;
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
		US_LineInput(PrintX,PrintY,Scores[n].name,id0_nil_t,true,MaxHighName,100);
	}
}


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
	//id0_int_t i,xl,yl,xh,yh;
	//id0_char_t num[20];
#ifdef PROFILE
	clock_t start,end;
#endif

	DrawPlayScreen ();

restart:
	if (!loadedgame)
	{
		gamestate.difficulty = restartgame;
		restartgame = gd_Continue;
		DrawEnterScreen ();
	}

	do
	{
		// REFKEEN - Looks like a hack (cast added for C++)
		playstate = (exittype)gd_Continue;
		if (!loadedgame)
			SetupGameLevel ();
		else
			loadedgame = false;

		CacheScaleds ();

		// (REFKEEN) Simulate a short wait while loading
		VW_WaitVBL(25);
		//

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
		case ex_died:
			Died ();
			NormalScreen ();
			FreeUpMemory ();
			CheckHighScore (gamestate.score,gamestate.mapon+1);
			return;
		case ex_warped:
			FizzleOut (true);
			if (gamestate.mapon >= NUMLEVELS)
			{
				Victory ();
				FreeUpMemory ();
				CheckHighScore(gamestate.score,gamestate.mapon+1);
				return;
			}
			break;
		case ex_abort:
			FreeUpMemory ();
			return;
		case ex_resetgame:
		case ex_loadedgame:
			goto restart;
		case ex_victorious:
			Victory ();
			FreeUpMemory();
			CheckHighScore(gamestate.score,gamestate.mapon+1);
			return;
		}

	} while (1);

}

// (REFKEEN) Used for patching version-specific stuff
void RefKeen_Patch_c3_game(void)
{
	switch (refkeen_current_gamever)
	{
	case BE_GAMEVER_CAT3D100:
		levelnames[1] = "Grelminar's Keep";
		levelnames[19] = "Grelminar's Lair";
		break;
	case BE_GAMEVER_CAT3D122:
		levelnames[1] = "Nemesis's Keep";
		levelnames[19] = "Nemesis's Lair";
		break;
	}
}
