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

// C3_DEBUG.C

#include "def.h"
#include "gelib.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define DEBUG_OVERHEAD 0


#define VIEWTILEX	20
#define VIEWTILEY	(VIEWHEIGHT/16)

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

id0_short_t colordelay=0;
#ifdef REFKEEN_VER_CATARM
id0_boolean_t autofire=false;
#endif
id0_int_t	maporgx;
id0_int_t	maporgy;
// REFKEEN - Probably unused
#if DEBUG_OVERHEAD
// REFKEEN - Give this enum some type so we can build code as C++
typedef enum {mapview,tilemapview,actoratview,visview,mapseg2,lastview} viewtypetype
viewtypeenum viewtype;
//enum {mapview,tilemapview,actoratview,visview,mapseg2,lastview}	viewtype;
#endif

void ViewMap (void);

//===========================================================================

#if 0
/*
================
=
= PicturePause
=
================
*/

void PicturePause (void)
{
	id0_int_t	y;
	id0_unsigned_t	source;

	source = displayofs+panadjust;

//	VW_ColorBorder (15);
	VW_SetLineWidth (40);
	VW_SetScreen (0,0);

	if (source<0x10000l-200*64)
	{
	//
	// copy top line first
	//
		for (y=0;y<200;y++)
			VW_ScreenToScreen (source+y*64,y*40,40,1);
	}
	else
	{
	//
	// copy bottom line first
	//
		for (y=199;y>=0;y--)
			VW_ScreenToScreen (source+y*64,y*40,40,1);
	}

	IN_Shutdown ();

	VW_WaitVBL(70);
	BE_ST_BiosScanCode(0);
	VW_WaitVBL(70);
	Quit (NULL);
}
#endif


//===========================================================================

//===========================================================================

#define	sc_1			0x02
#define	sc_2			0x03
#define	sc_3			0x04
#define	sc_4			0x05
#define	sc_5			0x06
#define	sc_6			0x07
#define	sc_7			0x08
#define	sc_8			0x09
#define	sc_9			0x0a
#define	sc_0			0x0b



/*
================
=
= DebugKeys
=
================
*/

id0_int_t DebugKeys (void)
{
	id0_boolean_t esc;
	id0_int_t level,i;

#if DEBUG_KEYS_AVAILABLE
	if (Keyboard[sc_R])
	{
		CenterWindow (12,2);
		if (autofire)
		  US_PrintCentered ("Rapid-Fire OFF");
		else
		  US_PrintCentered ("Rapid-Fire ON");
		VW_UpdateScreen();
		IN_Ack();
		autofire ^= 1;
		return 1;
	}
#endif

#if DEBUG_KEYS_AVAILABLE
	if (Keyboard[sc_A])
	{
		id0_char_t levelstr[50];
		id0_unsigned_t org_tile,org_mapon,msgnum;
		id0_boolean_t newmsg=true,newlevel=false;

		VW_FixRefreshBuffer ();
		CenterWindow (16,3);
		US_Print("\n");
		US_CPrint("Message Test");
		VW_UpdateScreen();

		org_mapon = mapon;
		msgnum = (org_tile = *(mapsegs[0]+farmapylookup[player->tiley]+player->tilex))-NAMESTART;
		while (1)
		{
	// Get outta' here
	//
			if (Keyboard[sc_Escape])
			{
				while (Keyboard[sc_Escape])
				{
					BE_ST_ShortSleep();
				}
				break;
			}

	// Move to previous message
	//
			if (Keyboard[sc_UpArrow])
			{
				if (msgnum)
				{
					msgnum--;
					newmsg = true;
				}
			}

	// Move to next message
	//
			if (Keyboard[sc_DownArrow])
			{
				if (msgnum < 24)
				{
					msgnum++;
					newmsg = true;
				}
			}

	// Move to previous level
	//
			if (Keyboard[sc_LeftArrow])
			{
				if (mapon)
				{
					MM_SetPurge(&grsegs[LEVEL1TEXT+mapon],3);
					mapon--;
					newlevel = true;
				}
			}

	// Move to next level
	//
			if (Keyboard[sc_RightArrow])
			{
				if (mapon < LASTMAP-1)
				{
					MM_SetPurge(&grsegs[LEVEL1TEXT+mapon],3);
					mapon++;
					newlevel = true;
				}
			}

	// Load new level text
	//
			if (newlevel)
			{
				CA_CacheGrChunk(LEVEL1TEXT+mapon);
				ScanText();
				newmsg = true;
				newlevel=false;
			}

	// Display new message text
	//
			if (newmsg)
			{
				*(mapsegs[0]+farmapylookup[player->tiley]+player->tilex) = msgnum+NAMESTART;
				DrawText(true);
				strcpy(levelstr,"Level: ");
				itoa(mapon,levelstr+strlen(levelstr),10);
				strcat(levelstr,"  Msg: ");
				itoa(msgnum,levelstr+strlen(levelstr),10);
				DisplaySMsg(levelstr,NULL);
				newmsg = false;

				if (Keyboard[sc_UpArrow] || Keyboard[sc_DownArrow] || Keyboard[sc_LeftArrow] || Keyboard[sc_RightArrow])
					VW_WaitVBL(6);
			}

			BE_ST_ShortSleep();
		}
// Restore game
//
		MM_SetPurge(&grsegs[LEVEL1TEXT+mapon],3);
		mapon = org_mapon;
		CA_CacheGrChunk(LEVEL1TEXT+mapon);
		ScanText();
		*(mapsegs[0]+farmapylookup[player->tiley]+player->tilex) = org_tile;
		DrawText(true);
		status_flag = 0;
	}


	if (Keyboard[sc_V])
	{
		displayofs = bufferofs = screenloc[screenpage];
		CenterWindow (16,4);
		US_CPrint("\n"GAMENAME);
		US_CPrint(VERSION);
		US_CPrint(REVISION);
		VW_UpdateScreen();
		IN_Ack ();
	}

#endif

#if (DEBUG_KEYS_AVAILABLE) || (defined REFKEEN_VER_CATAPOC)
	if (Keyboard[sc_Q])			// Q = Insta-Quit!
		Quit("Insta-Quit!");
#endif

#if (DEBUG_KEYS_AVAILABLE) && !(defined REFKEEN_VER_CATAPOC)
	if (Keyboard[sc_Z])		// Z = freeze Time
	{
		if (FreezeTime)
		  FreezeTime = 1;		// Allow refresh to dec to zero..
		else
			StopTime();

		IN_Ack();
		return 1;
	}
#endif

//	if (Keyboard[sc_E])
//		FaceDoor((player->x>>16l)+1,(player->y>>16l));
//		FaceAngle(90);

#if 0
	if (Keyboard[sc_B])		// B = border color
	{
		CenterWindow(24,3);
		PrintY+=6;
		US_Print(" Border color (0-15):");
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=15)
				VW_ColorBorder (level);
		}
		return 1;
	}
#endif


#if 1//DEBUG_KEYS_AVAILABLE
	if (Keyboard[sc_O])
	{
		extern id0_unsigned_t objectcount,latchmemavail;
		id0_unsigned_t unused,total;

		CenterWindow (30,13);
		US_Print ("Objects: ");
		US_PrintUnsigned (objectcount);

		US_Print("\n\nTics: ");
		US_PrintUnsigned (tics);
		US_Print("      Real Tics: ");
		US_PrintUnsigned(realtics);

		US_Print ("\n\n    Total Available: ");
		US_PrintUnsigned (mminfo.mainmem/1024);
		US_Print ("k\n        Mem In Use: ");
		unused=MM_UnusedMemory()/1024;
		US_PrintUnsigned (unused);
		US_Print ("k\n Mem After Purge: ");
		total=MM_TotalFree()/1024;
		US_PrintUnsigned (total);
		US_Print ("k (");
		US_PrintUnsigned (total-unused);

		US_Print (")\n\nLatch Mem Free: ");
		US_PrintUnsigned (latchmemavail);
		US_Print ("\n");
		VW_UpdateScreen();
		IN_Ack();
	}

	if (colordelay<1)
	{
		if (Keyboard[26])
		{
			extern id0_unsigned_t *groundcolor,debug_gnd;

			groundcolor = &debug_gnd;
			debug_gnd += 0x0101;
			if (debug_gnd == 0x1010)
				debug_gnd = 0;
			colordelay = 10;
		}

		if (Keyboard[27])
		{
			extern id0_unsigned_t *skycolor,debug_sky;

			skycolor = &debug_sky;
			debug_sky += 0x0101;
			if (debug_sky == 0x1010)
				debug_sky = 0;
			colordelay = 10;
		}
	}
	else
		colordelay -= realtics;
#endif


#if 0
	if (Keyboard[sc_C])		// C = count objects
	{
		CountObjects();
		return 1;
	}


	if (Keyboard[sc_D])		// D = start / end demo record
	{
		if (DemoMode == demo_Off)
			StartDemoRecord ();
		else if (DemoMode == demo_Record)
		{
			EndDemoRecord ();
			playstate = ex_completed;
		}
		return 1;
	}
#endif

#if 0
	if (Keyboard[sc_E])		// E = quit level
	{
		if (tedlevel)
			TEDDeath();
		playstate = ex_warped;
		gamestate.mapon++;
	}
#endif

#if 0
	if (Keyboard[sc_F])		// F = facing spot
	{
		CenterWindow (12,4);
		US_Print ("X:");
		US_PrintUnsigned (player->x);
		US_Print ("Y:");
		US_PrintUnsigned (player->y);
		US_Print ("A:");
		US_PrintUnsigned (player->angle);
		VW_UpdateScreen();
		IN_Ack();
		return 1;
	}
#endif

	if (Keyboard[sc_G])		// G = god mode
	{
		CenterWindow (12,2);
		if (godmode)
		  US_PrintCentered ("God mode OFF");
		else
		  US_PrintCentered ("God mode ON");
		VW_UpdateScreen();
		IN_Ack();
		godmode ^= 1;
		return 1;
	}

#if 0
	if (Keyboard[sc_H])		// H = hurt self
	{
		TakeDamage (5);
	}
#endif

	if (Keyboard[sc_I])			// I = item cheat
	{
		extern id0_boolean_t redraw_gems;

		CenterWindow (12,3);
		US_PrintCentered ("Free items!");
		VW_UpdateScreen();
		for (i=0;i<4;i++)
		{
			GiveBolt ();
			GiveNuke ();
			GivePotion ();
//			if (!gamestate.keys[i])
				GiveKey (i);
			gamestate.gems[i] = GEM_DELAY_TIME;
		}
		gamestate.gems[4] = GEM_DELAY_TIME;
		redraw_gems = true;
/////////		for (i=0;i<8;i++)
/////////			GiveScroll (i,false);

		IN_Ack ();
		return 1;
	}

#if DEBUG_OVERHEAD
	if (Keyboard[sc_Z])			// O is used elsewhere...
	{
		ViewMap();
		return 1;
	}
#endif

#if 0
	if (Keyboard[sc_P])			// P = pause with no screen disruptioon
	{
		PicturePause ();
		return 1;
	}
#endif

#if 0
	if (Keyboard[sc_S])	// S = slow motion
	{
		singlestep^=1;
		CenterWindow (18,3);
		if (singlestep)
			US_PrintCentered ("Slow motion ON");
		else
			US_PrintCentered ("Slow motion OFF");
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
#endif

#if 0
	if (Keyboard[sc_V])			// V = extra VBLs
	{
		CenterWindow(30,3);
		PrintY+=6;
		US_Print("  Add how many extra VBLs(0-8):");
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=8)
				extravbls = level;
		}
		return 1;
	}
#endif

	if (Keyboard[sc_W])	// W = warp to level
	{
		CenterWindow(26,3);
		PrintY+=6;
#ifdef REFKEEN_VER_CATARM
		US_Print("  Warp to which level(0-16):");
#elif defined REFKEEN_VER_CATAPOC
		US_Print("  Warp to which level(0-17):");
#endif
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=LASTMAP-1)
			{
				gamestate.mapon = level;
				playstate = ex_warped;
				lasttext = -1;
			}
		}
		return 1;
	}

#if 0
	if (Keyboard[sc_X])			// X = item cheat
	{
		CenterWindow (12,3);
		US_PrintCentered ("Extra stuff!");
		VW_UpdateScreen();
		for (i=0;i<4;i++)
		{
			GiveBolt ();
			GiveNuke ();
			GivePotion ();
		}
		IN_Ack ();
		return 1;
	}
#endif

////////	if (LastScan >= sc_1 && LastScan <= sc_8)	// free scrolls
////////	{
////////		GiveScroll (LastScan-sc_1,false);
////////		IN_ClearKeysDown ();
////////	}

	return 0;
}


#if DEBUG_OVERHEAD

/*
=====================
=
= LatchDrawChar
=
=====================
*/

void LatchDrawChar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t picnum)
{
	id0_unsigned_t	source, dest;

	dest = bufferofs + ylookup[y]+x;
	source = latchpics[0]+picnum*8;

	BE_ST_EGAUpdateGFXByteScrToScr(dest, source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
	BE_ST_EGAUpdateGFXByteScrToScr(dest += linewidth, ++source);
#if 0
	EGAWRITEMODE(1);
	EGAMAPMASK(15);

asm	mov	bx,[linewidth]
asm	dec	bx

asm	mov	ax,[screenseg]
asm	mov	es,ax
asm	mov	ds,ax

asm	mov	si,[source]
asm	mov	di,[dest]

asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb

asm	mov	ax,ss
asm	mov	ds,ax					// restore turbo's data segment

	EGAWRITEMODE(0);
#endif
}

#endif


#if DEBUG_OVERHEAD
/*
=====================
=
= LatchDrawTile
=
=====================
*/

void LatchDrawTile (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t picnum)
{
	id0_unsigned_t	source, dest;

	dest = bufferofs + ylookup[y]+x;
	source = tileoffsets[picnum];

	for (int lineCount = 16; lineCount; --lineCount)
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest, source, 2);
		source += 2;
		dest += linewidth;
	}
#if 0
	EGAWRITEMODE(1);
	EGAMAPMASK(15);

asm	mov	bx,[linewidth]
asm	sub	bx,2

asm	mov	ax,[screenseg]
asm	mov	es,ax
asm	mov	ds,ax

asm	mov	si,[source]
asm	mov	di,[dest]
asm	mov	dx,16

lineloop:
asm	movsb
asm	movsb
asm	add	di,bx

asm	dec	dx
asm	jnz	lineloop

asm	mov	ax,ss
asm	mov	ds,ax					// restore turbo's data segment

	EGAWRITEMODE(0);
#endif
}
#endif


#if DEBUG_OVERHEAD
/*
===================
=
= OverheadRefresh
=
===================
*/

void OverheadRefresh (void)
{
	id0_unsigned_t	x,y,endx,endy,sx,sy;
	id0_unsigned_t	tile;


	if (++screenpage == 3)
		screenpage = 0;

	bufferofs = screenloc[screenpage];

	endx = maporgx+VIEWTILEX;
	endy = maporgy+VIEWTILEY;

	for (y=maporgy;y<endy;y++)
		for (x=maporgx;x<endx;x++)
		{
			sx = (x-maporgx)*2;
			sy = (y-maporgy)*16;

			switch (viewtype)
			{
			case mapview:
				tile = *(mapsegs[0]+farmapylookup[y]+x);
				break;

			case tilemapview:
				tile = tilemap[x][y];
				break;

			case actoratview:
				tile = (id0_unsigned_t)actorat[x][y];
				break;

			case visview:
				tile = spotvis[x][y];
				break;

			case mapseg2:
				tile = *(mapsegs[2]+farmapylookup[y]+x);
				if (tile < 256)
					tile = *(mapsegs[0]+farmapylookup[y]+x);
			break;

			}

			if (tile<NUMTILE16)
				LatchDrawTile(sx,sy,tile);
			else
			{
				LatchDrawChar(sx,sy,NUMBERCHARS+((tile&0xf000)>>12));
				LatchDrawChar(sx+1,sy,NUMBERCHARS+((tile&0x0f00)>>8));
				LatchDrawChar(sx,sy+8,NUMBERCHARS+((tile&0x00f0)>>4));
				LatchDrawChar(sx+1,sy+8,NUMBERCHARS+(tile&0x000f));
			}
		}

	VW_SetScreen (bufferofs,0);
	displayofs = bufferofs;
}


/*
===================
=
= ViewMap
=
===================
*/

void ViewMap (void)
{
	id0_boolean_t		button0held;

	viewtype = actoratview;
	button0held = false;


	maporgx = player->tilex - VIEWTILEX/2;
	if (maporgx<0)
		maporgx = 0;
	maporgy = player->tiley - VIEWTILEY/2;
	if (maporgy<0)
		maporgy = 0;

	do
	{
//
// let user pan around
//
		IN_ReadControl(0,&control);
		if (control.xaxis == -1 && maporgx>0)
			maporgx--;
		if (control.xaxis == 1 && maporgx<mapwidth-VIEWTILEX)
			maporgx++;
		if (control.yaxis == -1 && maporgy>0)
			maporgy--;
		if (control.yaxis == 1 && maporgy<mapheight-VIEWTILEY)
			maporgy++;

		if (control.button0 && !button0held)
		{
			button0held = true;
			// REFKEEN - Make this C++ friendly
			if (viewtype == lastview-1)
				viewtype = mapview;
			else
				viewtype = (viewtypeenum)(viewtype + 1);
#if 0
			viewtype++;
			if (viewtype==lastview)
				viewtype = mapview;
#endif
		}
		if (!control.button0)
			button0held = false;


		OverheadRefresh ();

		BE_ST_ShortSleep();

	} while (!Keyboard[sc_Escape]);

	IN_ClearKeysDown ();
	DrawPlayScreen ();
}
#endif

