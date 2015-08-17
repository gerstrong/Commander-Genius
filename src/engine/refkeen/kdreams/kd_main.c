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

// KD_MAIN.C
/*
=============================================================================

							KEEN DREAMS

					An Id Software production

=============================================================================
*/

//#include "mem.h"
#include <string.h>

#include "kd_def.h"
//#pragma hdrstop

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
#define CATALOG
#endif

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

id0_char_t            str[80],str2[20];
id0_boolean_t         singlestep,jumpcheat,godmode,tedlevel;
id0_unsigned_t        tedlevelnum;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void    DebugMemory (void);
void    TestSprites(void);
id0_int_t             DebugKeys (void);
void    ShutdownId (void);
void    Quit (const id0_char_t *error);
void    InitGame (void);
//void    main (void);

//===========================================================================

#if FRILLS

/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory (void)
{
	VW_FixRefreshBuffer ();
	US_CenterWindow (16,7);

	US_CPrint ("Memory Usage");
	US_CPrint ("------------");
	US_Print ("Total     :");
	US_PrintUnsigned (mminfo.mainmem/1024);
	US_Print ("k\nFree      :");
	US_PrintUnsigned (MM_UnusedMemory()/1024);
	US_Print ("k\nWith purge:");
	US_PrintUnsigned (MM_TotalFree()/1024);
	US_Print ("k\n");
	VW_UpdateScreen();
	IN_Ack ();
#if GRMODE == EGAGR
	MM_ShowMemory ();
#endif
}

/*
===================
=
= TestSprites
=
===================
*/

#define DISPWIDTH       110
#define TEXTWIDTH   40
void TestSprites(void)
{
	id0_int_t hx,hy,sprite,oldsprite,bottomy,topx,shift;
	spritetabletype id0_far *spr;
	spritetype id0_seg *block;
	id0_unsigned_t        mem,scan;


	VW_FixRefreshBuffer ();
	US_CenterWindow (30,17);

	US_CPrint ("Sprite Test");
	US_CPrint ("-----------");

	hy=PrintY;
	hx=(PrintX+56)&(~7);
	topx = hx+TEXTWIDTH;

	US_Print ("Chunk:\nWidth:\nHeight:\nOrgx:\nOrgy:\nXl:\nYl:\nXh:\nYh:\n"
			  "Shifts:\nMem:\n");

	bottomy = PrintY;

	sprite = STARTSPRITES;
	shift = 0;

	do
	{
		if (sprite>=STARTTILE8)
			sprite = STARTTILE8-1;
		else if (sprite<STARTSPRITES)
			sprite = STARTSPRITES;

		spr = &spritetable[sprite-STARTSPRITES];
		block = (spritetype id0_seg *)grsegs[sprite];

		VWB_Bar (hx,hy,TEXTWIDTH,bottomy-hy,WHITE);

		PrintX=hx;
		PrintY=hy;
		US_PrintUnsigned (sprite);US_Print ("\n");PrintX=hx;
		US_PrintUnsigned (spr->width);US_Print ("\n");PrintX=hx;
		US_PrintUnsigned (spr->height);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgx);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgy);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->shifts);US_Print ("\n");PrintX=hx;
		if (!block)
		{
			US_Print ("-----");
		}
		else
		{
			mem = block->sourceoffset[3]+5*block->planesize[3];
			mem = (mem+15)&(~15);           // round to paragraphs
			US_PrintUnsigned (mem);
		}

		oldsprite = sprite;
		do
		{
		//
		// draw the current shift, then wait for key
		//
			VWB_Bar(topx,hy,DISPWIDTH,bottomy-hy,WHITE);
			if (block)
			{
				PrintX = topx;
				PrintY = hy;
				US_Print ("Shift:");
				US_PrintUnsigned (shift);
				US_Print ("\n");
				VWB_DrawSprite (topx+16+shift*2,PrintY,sprite);
			}

			VW_UpdateScreen();

			scan = IN_WaitForKey ();

			switch (scan)
			{
			case sc_UpArrow:
				sprite++;
				break;
			case sc_DownArrow:
				sprite--;
				break;
			case sc_LeftArrow:
				if (--shift == -1)
					shift = 3;
				break;
			case sc_RightArrow:
				if (++shift == 4)
					shift = 0;
				break;
			case sc_Escape:
				return;
			}

		} while (sprite == oldsprite);

  } while (1);


}

#endif


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
	id0_int_t level;

#if FRILLS
	if (Keyboard[0x12] && ingame)   // DEBUG: end + 'E' to quit level
	{
		if (tedlevel)
			TEDDeath();
		playstate = levelcomplete;
	}
#endif

	if (Keyboard[0x22] && ingame)           // G = god mode
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow (12,2);
		if (godmode)
		  US_PrintCentered ("God mode OFF");
		else
		  US_PrintCentered ("God mode ON");
		VW_UpdateScreen();
		IN_Ack();
		godmode ^= 1;
		return 1;
	}
	else if (Keyboard[0x17])                        // I = item cheat
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow (12,3);
		US_PrintCentered ("Free items!");
		gamestate.boobusbombs=99;
		gamestate.flowerpowers=99;
		gamestate.keys=99;
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
	else if (Keyboard[0x24])                        // J = jump cheat
	{
		jumpcheat^=1;
		VW_FixRefreshBuffer ();
		US_CenterWindow (18,3);
		if (jumpcheat)
			US_PrintCentered ("Jump cheat ON");
		else
			US_PrintCentered ("Jump cheat OFF");
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
#if FRILLS
	else if (Keyboard[0x32])                        // M = memory info
	{
		DebugMemory();
		return 1;
	}
#endif
	else if (Keyboard[0x19])                        // P = pause with no screen disruptioon
	{
		IN_Ack();
	}
	else if (Keyboard[0x1f] && ingame)      // S = slow motion
	{
		singlestep^=1;
		VW_FixRefreshBuffer ();
		US_CenterWindow (18,3);
		if (singlestep)
			US_PrintCentered ("Slow motion ON");
		else
			US_PrintCentered ("Slow motion OFF");
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
#if FRILLS
	else if (Keyboard[0x14])                        // T = sprite test
	{
		TestSprites();
		return 1;
	}
#endif
	else if (Keyboard[0x11] && ingame)      // W = warp to level
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow(26,3);
		PrintY+=6;
		US_Print("  Warp to which level(0-16):");
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=16)
			{
				gamestate.mapon = level;
				playstate = warptolevel;
			}
		}
		return 1;
	}
	return 0;
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
  SD_Shutdown ();
  IN_Shutdown ();
  RF_Shutdown ();
  VW_Shutdown ();
  CA_Shutdown ();
  MM_Shutdown ();
}

//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (const id0_char_t *error)
{
  ShutdownId ();
  if (error && *error)
  {
	BE_ST_clrscr();
	BE_ST_puts(error);
	BE_ST_puts("\n");
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
//      BE_ST_puts("For techinical assistance with running this software, type HELP at");
//      BE_ST_puts("    the DOS prompt or call Softdisk Publishing at 1-318-221-8311");
#else
	if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
	{
		BE_ST_puts("For techinical assistance with running this software, type HELP at");
		BE_ST_puts("    the DOS prompt or call Gamer's Edge at 1-318-221-8311");
	}
#endif
	// No additional lines for later versions (registered v1.93, shareware v1.20)
	BE_ST_HandleExit(1);
  }

// TODO (REFKEEN) Maybe we can define CATALOG based on version?
#if (!defined REFKEEN_VER_KDREAMS_CGA_ALL) || (!defined CATALOG)
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
#endif
	{
		id0_argc = 2;
		id0_argv[1] = "LAST.SHL";
		id0_argv[2] = "ENDSCN.SCN";
		id0_argv[3] = NULL;

		//if (execv("LOADSCN.EXE", _argv) == -1)
		//	Quit("Couldn't find executable LOADSCN.EXE.\n");
		void loadscn2_main(int argc, const char **argv);
		loadscn2_main(id0_argc+1, id0_argv);
	}
#endif

#if (defined REFKEEN_VER_KDREAMS_CGA_ALL) && (defined CATALOG)
	VW_SetScreenMode(TEXTGR);
	BE_ST_HandleExit(0);
#endif

// For all other versions (registered v1.93, shareware v1.20), just exit
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever != BE_GAMEVER_KDREAMSE113)
	{
		BE_ST_HandleExit(0);
	}
#endif

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

extern id0_byte_t *PIRACY;

void InitGame (void)
{
	id0_int_t i;

	MM_Startup ();

#if 0
	// Handle piracy screen...
	//
	movedata(FP_SEG(PIRACY),(id0_unsigned_t)PIRACY,0xb800,displayofs,4000);
	while (BE_ST_BiosScanCode(0) != sc_Return);
	//while ((bioskey(0)>>8) != sc_Return);
#endif

#if GRMODE == EGAGR
	if (mminfo.mainmem < 335l*1024)
	{
//#pragma warn    -pro
//#pragma warn    -nod
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
		BE_ST_textcolor(7);
#endif
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
		if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
#endif
		{
			BE_ST_textbackground(0);
		}
//#pragma warn    +nod
//#pragma warn    +pro
		BE_ST_clrscr();                       // we can't include CONIO because of a name conflict
//#pragma warn    +nod
//#pragma warn    +pro
		BE_ST_puts ("There is not enough memory available to play the game reliably.  You can");
		BE_ST_puts ("play anyway, but an out of memory condition will eventually pop up.  The");
		BE_ST_puts ("correct solution is to unload some TSRs or rename your CONFIG.SYS and");
		BE_ST_puts ("AUTOEXEC.BAT to free up more memory.\n");
		BE_ST_puts ("Do you want to (Q)uit, or (C)ontinue?");
		//i = bioskey (0);
		//if ( (i>>8) != sc_C)
		i = BE_ST_BiosScanCode (0);
		if (i != sc_C)
			Quit ("");
	}
#endif

	US_TextScreen();

	VW_Startup ();
	RF_Startup ();
	IN_Startup ();
	SD_Startup ();
	US_Startup ();

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	US_UpdateTextScreen();
#endif

	CA_Startup ();
	US_Setup ();

//
// load in and lock down some basic chunks
//

	CA_ClearMarks ();

	CA_MarkGrChunk(STARTFONT);
	CA_MarkGrChunk(STARTFONTM);
	CA_MarkGrChunk(STARTTILE8);
	CA_MarkGrChunk(STARTTILE8M);
	for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
		CA_MarkGrChunk(i);

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	CA_CacheMarks (NULL);
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	CA_CacheMarks (NULL, 0);
#endif

	MM_SetLock (&grsegs[STARTFONT],true);
	MM_SetLock (&grsegs[STARTFONTM],true);
	MM_SetLock (&grsegs[STARTTILE8],true);
	MM_SetLock (&grsegs[STARTTILE8M],true);
	for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
		MM_SetLock (&grsegs[i],true);

	CA_LoadAllSounds ();

	fontcolor = WHITE;

	US_FinishTextScreen();

	VW_SetScreenMode (GRMODE);
	VW_ClearVideo (BLACK);
}



//===========================================================================

/*
==========================
=
= main
=
==========================
*/

#if (!defined REFKEEN_VER_KDREAMS_CGA_ALL) || (!defined CATALOG)
static const id0_char_t *EntryParmStrings[] = {"detour",id0_nil_t};
#endif

// The original starting point of the game EXE
void kdreams_exe_main (void)
{
#if (!defined REFKEEN_VER_KDREAMS_CGA_ALL) || (!defined CATALOG)
	id0_boolean_t LaunchedFromShell = false;
	id0_short_t i;
#endif



#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	BE_ST_textcolor(7);
	BE_ST_textbackground(0);

	if (BE_Cross_strcasecmp(id0_argv[1], "/VER") == 0)
	{
		BE_ST_printf("KEEN DREAMS\n");
		BE_ST_printf("CGA Version\n");
		BE_ST_printf("Copyright 1991-93 Softdisk Publishing\n");
		BE_ST_printf("Version 1.05 (rev 1)\n");
		BE_ST_HandleExit(0);
	}

	if (BE_Cross_strcasecmp(id0_argv[1], "/?") == 0)
	{
		BE_ST_printf("\nKeen Dreams CGA version 1.05\n");
		BE_ST_printf("Copyright 1991-1993 Softdisk Publishing.\n\n");
		BE_ST_printf("Commander Keen is a trademark of Id Software.\n");
		BE_ST_printf("Type KDREAMS from the DOS prompt to run.\n\n");
		BE_ST_printf("KDREAMS /COMP for SVGA compatibility mode\n");
		BE_ST_printf("KDREAMS /NODR stops program hang with the drive still on\n");
		BE_ST_printf("KDREAMS /NOAL disables AdLib and Sound Blaster detection\n");
		BE_ST_printf("KDREAMS /NOSB disables Sound Blaster detection\n");
		BE_ST_printf("KDREAMS /NOJOYS ignores joystick\n");
		BE_ST_printf("KDREAMS /NOMOUSE ignores mouse\n");
		BE_ST_printf("KDREAMS /HIDDENCARD overrides video card detection\n");
		BE_ST_printf("KDREAMS /VER  for version and compatibility information\n");
		BE_ST_printf("KDREAMS /? for this help information\n");
		BE_ST_HandleExit(0);
	}
#endif // VERSION


	// REFKEEN - The code is also in CGA v1.05 but LaunchedFromShell is ignored (while CATALOG is defined)...
#if (!defined REFKEEN_VER_KDREAMS_CGA_ALL) || (!defined CATALOG)
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
#endif
	{
		for (i = 1;i < id0_argc;i++)
		{
			switch (US_CheckParm(id0_argv[i],EntryParmStrings))
			{
			case 0:
				LaunchedFromShell = true;
				break;
			}
		}
	}
#endif



#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE193)
	{
		if (BE_Cross_strcasecmp(id0_argv[1], "/VER") == 0)
		{
			BE_ST_printf("\nKeen Dreams version 1.93  (Rev 1)\n");
			BE_ST_printf("developed for use with 100%% IBM compatibles\n");
			BE_ST_printf("that have 640K memory, DOS version 3.3 or later,\n");
			BE_ST_printf("and an EGA or VGA display adapter.\n");
			BE_ST_printf("Copyright 1991-1993 Softdisk Publishing.\n");
			BE_ST_printf("Commander Keen is a trademark of Id Software.\n");
			BE_ST_HandleExit(0);
		}

		if (BE_Cross_strcasecmp(id0_argv[1], "/?") == 0)
		{
			BE_ST_printf("\nKeen Dreams version 1.93\n");
			BE_ST_printf("Copyright 1991-1993 Softdisk Publishing.\n\n");
			BE_ST_printf("Commander Keen is a trademark of Id Software.\n");
			BE_ST_printf("Type KDREAMS from the DOS prompt to run.\n\n");
			BE_ST_printf("KDREAMS /COMP for SVGA compatibility mode\n");
			BE_ST_printf("KDREAMS /NODR stops program hang with the drive still on\n");
			BE_ST_printf("KDREAMS /NOAL disables AdLib and Sound Blaster detection\n");
			BE_ST_printf("KDREAMS /NOSB disables Sound Blaster detection\n");
			BE_ST_printf("KDREAMS /NOJOYS ignores joystick\n");
			BE_ST_printf("KDREAMS /NOMOUSE ignores mouse\n");
			BE_ST_printf("KDREAMS /HIDDENCARD overrides video card detection\n");
			BE_ST_printf("KDREAMS /VER  for version and compatibility information\n");
			BE_ST_printf("KDREAMS /? for this help information\n");
			BE_ST_HandleExit(0);
		}
	}
#endif // VERSION


#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE120)
	{
		for (i = 1;i < id0_argc;i++)
		{
			switch (US_CheckParm(id0_argv[i],EntryParmStrings))
			{
			case 0:
				LaunchedFromShell = true;
				break;
			}
			if (BE_Cross_strcasecmp(id0_argv[i], "/VER") == 0)
			{
				BE_ST_printf("\nKeen Dreams Shareware Version 1.20  (Rev 1)\n");
				BE_ST_printf("developed for use with 100%% IBM compatibles\n");
				BE_ST_printf("that have 640K memory, DOS version 3.3 or later,\n");
				BE_ST_printf("and an EGA or VGA display adapter.\n");
				BE_ST_printf("Copyright 1991-1993 Softdisk Publishing.\n");
				BE_ST_printf("Commander Keen is a trademark of Id Software.\n");
				BE_ST_HandleExit(0);
			}

			if (BE_Cross_strcasecmp(id0_argv[i], "/?") == 0)
			{
				BE_ST_printf("\nKeen Dreams Shareware Version 1.20\n");
				BE_ST_printf("Copyright 1991-1993 Softdisk Publishing.\n\n");
				BE_ST_printf("Commander Keen is a trademark of Id Software.\n");
				BE_ST_printf("Type KDREAMS from the DOS prompt to run.\n\n");
				BE_ST_printf("KDREAMS /COMP for SVGA compatibility mode\n");
				BE_ST_printf("KDREAMS /NODR stops program hang with the drive still on\n");
				BE_ST_printf("KDREAMS /NOAL disables AdLib and Sound Blaster detection\n");
				BE_ST_printf("KDREAMS /NOSB disables Sound Blaster detection\n");
				BE_ST_printf("KDREAMS /NOJOYS ignores joystick\n");
				BE_ST_printf("KDREAMS /NOMOUSE ignores mouse\n");
				BE_ST_printf("KDREAMS /HIDDENCARD overrides video card detection\n");
				BE_ST_printf("KDREAMS /VER  for version and compatibility information\n");
				BE_ST_printf("KDREAMS /? for this help information\n");
				BE_ST_HandleExit(0);
			}
		}
	}
#endif


#if (!defined REFKEEN_VER_KDREAMS_CGA_ALL) || (!defined CATALOG)
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if ((refkeen_current_gamever == BE_GAMEVER_KDREAMSE113) || (refkeen_current_gamever == BE_GAMEVER_KDREAMSE120))
#endif
	{
		// REFKEEN difference from vanilla Keen Dreams (Shareware releases):
		// Role of /DETOUR has been flipped. No need to pass it (or use START),
		// but if /DETOUR is added then you get this message.
		if (LaunchedFromShell)
		//if (!LaunchedFromShell)
		{
			BE_ST_clrscr();
			BE_ST_puts("You must type START at the DOS prompt to run KEEN DREAMS.");
			BE_ST_HandleExit(0);
		}
	}
#endif


#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
	if (refkeen_current_gamever != BE_GAMEVER_KDREAMSE113)
	{
		BE_ST_textcolor(7);
		BE_ST_textbackground(0);
	}
#endif



	InitGame();

	DemoLoop();                                     // DemoLoop calls Quit when everything is done
	Quit("Demo loop exited???");
}

