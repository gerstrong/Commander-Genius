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

/* Reconstructed from the LOADSCN.EXE file bundled with The Catacomb Abyss
 * Shareware v1.13, using available source files
 */

#include <stdarg.h>

#include "ext_heads.h"
#include "ext_gelib.h"

static void SetScreenMode (id0_int_t mode);
static void SetLineWidth (id0_int_t width);
static id0_boolean_t IsKeyPressed (void);
static void WaitForKeyRelease (void);

static memptr bufferptr;
static id0_int_t screenmode;
static cardtype videocard;
static struct Shape armashape;

void id0_loadscn_exe_main (void)
{
	id0_int_t step;
	id0_boolean_t pressedkey = false;
	if (!BE_Cross_strcasecmp(id0_argv[1], "/?"))
	{
		BE_ST_clrscr();
		BE_ST_textcolor(15);
		BE_ST_textbackground(1);
		BE_ST_cprintf("\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n");
		BE_ST_cprintf("\xB3 LOADSCN                              \xB3\r\n");
		BE_ST_cprintf("\xB3 by Nolan Martin                      \xB3\r\n");
		BE_ST_cprintf("\xB3 Copyright 1992 - Softdisk Publishing \xB3\r\n");
		BE_ST_cprintf("\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n");
		BE_ST_printf("\n");
		BE_ST_printf("/VER  - version number\n");
		BE_ST_printf("/?    - this help\n");
		BE_ST_HandleExit(0);
	}
	if (!BE_Cross_strcasecmp(id0_argv[1], "/VER"))
	{
		BE_ST_clrscr();
		BE_ST_textcolor(15);
		BE_ST_textbackground(1);
		BE_ST_cprintf("\xD5\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8\r\n");
		BE_ST_cprintf("\xB3 LOADSCN                              \xB3\r\n");
		BE_ST_cprintf("\xB3 by Nolan Martin                      \xB3\r\n");
		BE_ST_cprintf("\xB3 Copyright 1992 - Softdisk Publishing \xB3\r\n");
		BE_ST_cprintf("\xD4\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBE\r\n");
		BE_ST_printf("\n");
		BE_ST_printf("%s %s\n", "LOADSCN  ", " VERSION 1.10s   QA[0]");
		BE_ST_printf("\n");
		BE_ST_printf("This program requires an EGA monitor or better,\n");
		BE_ST_printf("                640K, and MS-DOS 3.0 or better.\n");
		BE_ST_HandleExit(0);
	}
	if (BE_Cross_strcasecmp(id0_argv[1], "LAST.SHL"))
		TrashProg("You must type START [ENTER] to run this program.\n");
	videocard = VideoID();
	if ((videocard != 3) && (videocard != 5))
		TrashProg("You need an EGA or VGA monitor to run this program.");
	screenmode = 3;
	SetScreenMode(screenmode);
	SetScreen(0,0);
	if (ext_LoadShape("ARMAPC.ABS", &armashape))
		TrashProg("ERROR : Can't load image.");
	ext_MoveGfxDst(0, 200);
	UnpackEGAShapeToScreen(&armashape, 0, 0);
	// (REFKEEN) Add an artificial (screen not shown immediately on older machines)
	BE_ST_Delay(250);
	ScreenToScreen(8000, 0, 40, 200);
	for (step = 0; step < 10; ++step)
	{
		BE_ST_Delay(500);
		if (IsKeyPressed())
		{
			pressedkey = true;
			WaitForKeyRelease();
		}
	}
	if (!pressedkey)
		BE_ST_BiosScanCode(0);
	if (!ext_BLoad("LAST.ABS", &bufferptr))
		TrashProg("Can't load Compressed Text - Possibly corrupt file!");
	screenmode = 1;
	SetScreenMode(screenmode);

	memcpy(BE_ST_GetTextModeMemoryPtr(), (id0_byte_t *)bufferptr+7, 4000);
	BE_ST_MarkGfxForUpdate();
	//_fmemcpy(MK_FP(0xB800,0), (byte far *)bufferptr+7, 4000);
	BE_ST_MoveTextCursorTo(0, 23); // gotoxy(1, 24)
	BE_ST_HandleExit(0);
}

void loadscn_TrashProg (const id0_char_t *OutMsg, ...)
{
	va_list ap;

	if (OutMsg)
	{
		if (screenmode != 1)
			SetScreenMode(1);

		va_start(ap, OutMsg);

		if (OutMsg && *OutMsg)
			BE_ST_vprintf(OutMsg,ap);

		va_end(ap);
	}

	BE_ST_HandleExit(0);
}

static void SetScreenMode (id0_int_t mode)
{
	switch (mode)
	{
	case 1:
		BE_ST_SetScreenMode(3);
		SetLineWidth(80);
		break;
	case 3:
		BE_ST_SetScreenMode(0xd);
		SetLineWidth(40);
		break;
	}
}

//===========================================================================

/*
====================
=
= VW_SetLineWidth
=
= Must be an even number of bytes
=
====================
*/

static void SetLineWidth (id0_int_t width)
{
  id0_int_t i,offset;

#if GRMODE == EGAGR
//
// set wide virtual screen
//
	BE_ST_EGASetLineWidth(width); // Ported from ASM
#endif

//
// set up lookup tables
//
  //linewidth = width;

  offset = 0;

  for (i=0;i<VIRTUALHEIGHT;i++)
  {
	ylookup[i]=offset;
	offset += width;
  }
}


//===========================================================================

static id0_boolean_t IsKeyPressed (void)
{
	return BE_ST_KbHit();
#if 0
asm	mov ah, 1
asm	int 0x16
asm	jnz is_pressed

	return false;
	
is_pressed:
	return true;
#endif
}

static void WaitForKeyRelease (void)
{
	if (IsKeyPressed())
		while (IsKeyPressed())
		{
			BE_ST_BiosScanCode(0);
			//getch();
		}
}
