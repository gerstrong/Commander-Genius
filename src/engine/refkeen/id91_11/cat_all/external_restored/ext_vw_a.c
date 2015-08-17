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

/* Modified for building LOADSCN.EXE as bundled with The Catacomb Abyss
 * Shareware v1.13
 */

// ID_VW_A.C
// PORTED FROM ASM

#include "ext_heads.h"

// NOTE: Moved to ext_heads.h
// #define WAITFORVBL 1

//=========================================================
//
// Name:	VW_VideoID
//
// Function:	Detects the presence of various video subsystems
//
// id0_int_t VideoID;
//
// Subsystem ID values:
// 	 0  = (none)
// 	 1  = MDA
// 	 2  = CGA
// 	 3  = EGA
// 	 4  = MCGA
// 	 5  = VGA
// 	80h = HGC
// 	81h = HGC+
// 	82h = Hercules InColor
//
//=========================================================

cardtype VideoID (void)
{
	return VGAcard;
}

// Taken off ID_VW_AE.ASM

//===========================================================================
//
//                    MISC EGA ROUTINES
//
//===========================================================================

//==============
//
// VW_SetScreen
//
//==============

void SetScreen (id0_unsigned_t CRTC, id0_unsigned_t pelpan)
{
#if WAITFORVBL
#if 0
	mov	dx,STATUS_REGISTER_1

;
; wait util the CRTC just starts scaning a diplayed line to set the CRTC start
;
	cli

@@waitnodisplay:
	in	al,dx
	test	al,1
	jz	@@waitnodisplay

; the display is now disabled (in a HBL / VBL)

@@waitdisplay:
	in	al,dx
	test	al,1	;1 = display is disabled (HBL / VBL)
	jnz	@@waitdisplay

; the display was just enabled, so a full scan line is available for CRTC set

#endif
#endif
	BE_ST_SetScreenStartAddress(CRTC);
#if WAITFORVBL
#if 0
;
; wait for a vertical retrace to set pel panning
;
	mov	dx,STATUS_REGISTER_1
@@waitvbl:
	sti     		;service interrupts
	jmp	$+2
	cli
	in	al,dx
	test	al,00001000b	;look for vertical retrace
	jz	@@waitvbl

#endif
#endif
	BE_ST_EGASetPelPanning(pelpan);
#if WAITFORVBL
	// (REFKEEN) Doing this before BE_ST_EGASetPelPanning leads to
	// very scrolling glitches, so call here instead
	VW_WaitVBL(1);
#endif
}




//============================================================================
//
// VW_ScreenToScreen
//
// Basic block copy routine.  Copies one block of screen memory to another,
// using write mode 1 (sets it and returns with write mode 0).  bufferofs is
// NOT accounted for.
//
//============================================================================

void ScreenToScreen(id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t wide, id0_unsigned_t height)
{
	for (id0_unsigned_t lineCounter = height; lineCounter; --lineCounter, source += 40, dest += 40)
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest, source, wide);
	}

}
