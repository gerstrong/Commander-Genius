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

/* Modified for building LOADSCN.EXE as bundled with The Catacomb Abyss
 * Shareware v1.13
 */

#include <fcntl.h>
//#include <dos.h>
//#include <conio.h>
#include <stdio.h>
//#include <dir.h>
//#include "mem.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>
//#include "io.h"

#include "ext_heads.h"
#include "ext_gelib.h"
#include "ext_sl_file.h"


#define MAX_GAMELIST_NAMES 20
#define FNAME_LEN				9

// REFKEEN (DIFFERENCE FROM VANILLA CAT) - Share these wtih game EXE
//id0_unsigned_t ylookup[VIRTUALHEIGHT];
//id0_unsigned_t displayofs;

// (REFKEEN) HACK (FIXME?) Conditionally pick the correct TrashProg function

void TrashProg (const id0_char_t *OutMsg, ...)
{
	va_list ap;
	va_start(ap, OutMsg);

#ifdef REFKEEN_VER_CATABYSS
	if ((refkeen_current_gamever == BE_GAMEVER_CATABYSS113) && strcmp(id0_argv[0], "INTRO.EXE"))
	{
		void loadscn_TrashProg (const id0_char_t *OutMsg, ...);
		loadscn_TrashProg(OutMsg, ap);
	}
	else
#endif
	{
		void intro_TrashProg (const id0_char_t *OutMsg, ...);
		intro_TrashProg(OutMsg, ap);
	}
	va_end(ap);
}

// (REFKEEN) Functionality equivalant to UnpackEGAShapeToScreen from CATABYSS.EXE's gelib

#if 0
////////////////////////////////////////////////////////////////////////////
//
// UnpackEGAShapeToScreen()
//
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty)
{
	id0_int_t currenty;
	id0_signed_char_t n, Rep, id0_far *Src/*, id0_far *Dst[8]*/, loop, Plane;
	id0_unsigned_int_t DstOff[8];
	id0_unsigned_int_t BPR, Height;
	id0_boolean_t NotWordAligned;

	NotWordAligned = SHP->BPR & 1;
	startx>>=3;
	Src = (id0_signed_char_t *)(SHP->Data);
	currenty = starty;
	Plane = 0;
	Height = SHP->bmHdr.h;
	while (Height--)
	{
#if 0
		Dst[0] = (MK_FP(0xA000,displayofs));
		Dst[0] += ylookup[currenty];
		Dst[0] += startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			Dst[loop] = Dst[0];
#endif
		DstOff[0] = displayofs + ylookup[currenty] + startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			DstOff[loop] = DstOff[0];

		for (Plane=0; Plane<SHP->bmHdr.d; Plane++)
		{
			//outport(0x3c4,((1<<Plane)<<8)|2);

			BPR = ((SHP->BPR+1) >> 1) << 1;               // IGNORE WORD ALIGN
			while (BPR)
			{
				if (SHP->bmHdr.comp)
					n = *Src++;
				else
					n = BPR-1;

				if (n < 0)
				{
					if (n != -128)
					{
						n = (-n)+1;
						BPR -= n;
						Rep = *Src++;
						if ((!BPR) && (NotWordAligned))   // IGNORE WORD ALIGN
							n--;

						while (n--)
							BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, Rep, 1<<Plane);
							//*Dst[Plane]++ = Rep;
					}
					else
						BPR--;
				}
				else
				{
					n++;
					BPR -= n;
					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						n--;

					while (n--)
						BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, *Src++, 1<<Plane);
						//*Dst[Plane]++ = *Src++;

					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						Src++;
				}
			}
		}
		currenty++;
	}

	return(0);
}
#endif

// (REFKEEN) Functionality equivalant to Verify from CATABYSS.EXE's gelib
#if 0
////////////////////////////////////////////////////////////////////////////
//
// Verify()
//
id0_long_t Verify(const id0_char_t *filename)
{
	int handle;
	id0_long_t size;

	if ((handle=BE_Cross_open_for_reading(filename))==-1)
	//if ((handle=open(filename,O_RDONLY))!=-1)
	{
		size=BE_Cross_FileLengthFromHandle(handle);
		close(handle);
	}
	else
	{
		size=0;
	}
	return(size);
}
#endif

// Unused in loadscn, used in intro
// NOTE: THIS VERSION OMITS THE ASCII CODE (but We don't need it)
id0_int_t TryGetScanCode/*TryGetKey*/(void)
{
	if (BE_ST_BiosScanCode(1))
		return BE_ST_BiosScanCode(0);
	return 0;
#if  0
asm	mov ah, 1
asm	int 0x16
asm	mov ax, 0
asm	jz after_loop
asm	int 0x16
asm	mov bx, ax
asm	mov ah, 2
asm	int 0x16
asm	and ax, 0xc
asm	mov cl, 3
asm	shr al, cl
asm	adc al, 0
asm	shr ax, 1
asm	ror ax, 1
asm	or ax, bx

after_loop:
	return _AX;
#endif
}

// MoveGfxDst()
//
void ext_MoveGfxDst(id0_short_t x, id0_short_t y)
{
	id0_unsigned_t address;

	address = (y*40)+(x/8);
	displayofs = address;
}
