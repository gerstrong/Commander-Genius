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

#include "id_heads.h"

//=================================
//
// EGA view manager routines
//
//=================================

//============================================================================
//
// All EGA drawing routines that write out words need to have alternate forms
// for starting on even and odd addresses, because writing a word at segment
// offset 0xffff causes an exception!  To work around this, write a single
// byte out to make the address even, so it wraps cleanly at the end.
//
// All of these routines assume read/write mode 0, and will allways return
// in that state.
// The direction flag should be clear
// readmap/writemask is left in an undefined state
//
//============================================================================


//============================================================================
//
// VW_Plot (int x,y,color)
//
//============================================================================

id0_byte_t plotpixels[] = {128, 64, 32, 16, 8, 4, 2, 1};
extern id0_unsigned_t linedelta;

void VW_Plot(id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t color)
{
	BE_ST_EGAUpdateGFXPixel4bpp(bufferofs+ylookup[y]+(x>>3), color, plotpixels[x&7]);
}

#if 0
DATASEG

plotpixels	db	128,64,32,16,8,4,2,1

CODESEG

PROC	VW_Plot x:WORD, y:WORD, color:WORD
PUBLIC	VW_Plot
USES	SI,DI

	mov	es,[screenseg]

	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK+15*256
	WORDOUT

	mov	dx,GC_INDEX
	mov	ax,GC_MODE+2*256	;write mode 2
	WORDOUT

	mov	di,[bufferofs]
	mov	bx,[y]
	shl	bx,1
	add	di,[ylookup+bx]
	mov	bx,[x]
	mov	ax,bx
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	di,ax				; di = byte on screen

	and	bx,7
	mov	ah,[plotpixels+bx]
	mov	al,GC_BITMASK		;mask off other pixels
	WORDOUT

	mov		bl,[BYTE color]
	xchg	bl,[es:di]		; load latches and write pixel

	mov	dx,GC_INDEX
	mov	ah,0ffh				;no bit mask
	WORDOUT
	mov	ax,GC_MODE+0*256	;write mode 0
	WORDOUT

	ret

ENDP
#endif


//============================================================================
//
// VW_Vlin (int yl,yh,x,color)
//
//============================================================================

void VW_Vlin(id0_unsigned_t yl, id0_unsigned_t yh, id0_unsigned_t x, id0_unsigned_t color)
{
	id0_unsigned_t egaDestOff = bufferofs+ylookup[yl]+(x>>3);
	id0_byte_t mask = plotpixels[x&7];
	for (id0_unsigned_t loopVar = yh-yl+1; loopVar; --loopVar, egaDestOff += linewidth)
	{
		BE_ST_EGAUpdateGFXPixel4bpp(egaDestOff, color, mask);
	}
}

#if 0
PROC	VW_Vlin yl:WORD, yh:WORD, x:WORD, color:WORD
PUBLIC	VW_Vlin
USES	SI,DI

	mov	es,[screenseg]

	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK+15*256
	WORDOUT

	mov	dx,GC_INDEX
	mov	ax,GC_MODE+2*256	;write mode 2
	WORDOUT

	mov	di,[bufferofs]
	mov	bx,[yl]
	shl	bx,1
	add	di,[ylookup+bx]
	mov	bx,[x]
	mov	ax,bx
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	di,ax				; di = byte on screen

	and	bx,7
	mov	ah,[plotpixels+bx]
	mov	al,GC_BITMASK		;mask off other pixels
	WORDOUT

	mov	cx,[yh]
	sub	cx,[yl]
	inc	cx					;number of pixels to plot

	mov	bh,[BYTE color]
	mov	dx,[linewidth]

@@plot:
	mov		bl,bh
	xchg	bl,[es:di]		; load latches and write pixel
	add		di,dx

	loop	@@plot

	mov	dx,GC_INDEX
	mov	ah,0ffh				;no bit mask
	WORDOUT
	mov	ax,GC_MODE+0*256	;write mode 0
	WORDOUT

	ret

ENDP
#endif


//============================================================================


//===================
//
// VW_DrawTile8
//
// xcoord in bytes (8 pixels), ycoord in pixels
// All Tile8s are in one grseg, so an offset is calculated inside it
//
//===================

id0_unsigned_t screendest;

void VW_DrawTile8(id0_unsigned_t xcoord, id0_unsigned_t ycoord, id0_unsigned_t tile)
{
	id0_unsigned_t egaDestOff;
	screendest = bufferofs+xcoord+ylookup[ycoord];
	// This one is really weird, but looks like STARTTILE8 is really 314,
	// rather than 315, in the case of the original Catacomb Abyss
	// Shareware v1.13 EXE (only in ID_VW_AE.ASM, not e.g., C code).
	// There's a similar difference in the v1.24 EXE.
	// In practice, though, it looks like STARTTILE8 is never used anyway...
#ifdef REFKEEN_VER_CATABYSS
	id0_byte_t *tilePtr = (id0_byte_t *)grsegs[STARTTILE8-1]+(tile<<5);
#else
	id0_byte_t *tilePtr = (id0_byte_t *)grsegs[STARTTILE8]+(tile<<5);
#endif
	for (int planeCounter = 4, mapMask = 1; planeCounter; --planeCounter, mapMask <<= 1)
	{
		egaDestOff = screendest;
		for (int loopVar = 0; loopVar < 7; ++loopVar, egaDestOff += linewidth, ++tilePtr)
		{
			BE_ST_EGAUpdateGFXByte(egaDestOff, *tilePtr, mapMask);
		}
		BE_ST_EGAUpdateGFXByte(egaDestOff, *tilePtr, mapMask);
		++tilePtr;
	}
}

#if 0
PROC	VW_DrawTile8	xcoord:WORD, ycoord:WORD, tile:WORD
PUBLIC	VW_DrawTile8
USES	SI,DI

	mov	es,[screenseg]

	mov	di,[bufferofs]
	add	di,[xcoord]
	mov	bx,[ycoord]
	shl	bx,1
	add	di,[ylookup+bx]
	mov	[ss:screendest],di		;screen destination

	mov	bx,[linewidth]
	dec	bx

	mov	si,[tile]
	shl	si,1
	shl	si,1
	shl	si,1
	shl	si,1
	shl	si,1

	mov	ds,[grsegs+STARTTILE8*2] ; segment for all tile8s

	mov	cx,4					;planes to draw
	mov	ah,0001b				;map mask

	mov	dx,SC_INDEX
	mov	al,SC_MAPMASK

;
; start drawing
;

@@planeloop:
	WORDOUT
	shl	ah,1					;shift plane mask over for next plane

	mov	di,[ss:screendest]		;start at same place in all planes

REPT	7
	movsb
	add	di,bx
ENDM
	movsb

	loop	@@planeloop

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

ENDP
#endif


//============================================================================
//
// VW_MaskBlock
//
// Draws a masked block shape to the screen.  bufferofs is NOT accounted for.
// The mask comes first, then four planes of data.
//
//============================================================================

//#define UNWOUNDMASKS 18

id0_byte_t planemask;
id0_byte_t planenum;

void VW_MaskBlock(memptr segm,id0_unsigned_t ofs,id0_unsigned_t dest,
	id0_unsigned_t wide,id0_unsigned_t height,id0_unsigned_t planesize)
{
	planemask = 1;
	planenum = 0;
	linedelta = linewidth-wide; // amount to add after drawing each line
	id0_unsigned_t dataLoc = planesize;
	do
	{
		id0_byte_t *srcPtr = (id0_byte_t *)segm + ofs; // start back at the top of the mask
		id0_unsigned_t egaDestOff = dest; // start at same place in all planes
		id0_unsigned_t linesLeft = height; // scan lines to draw
		// draw one plane
		do
		{
			id0_unsigned_t colsLeft = wide;
			do
			{
				BE_ST_EGAUpdateGFXByte(egaDestOff, (BE_ST_EGAFetchGFXByte(egaDestOff, planenum) & (*srcPtr)) | srcPtr[dataLoc], planemask);
				++srcPtr;
				++egaDestOff;
				--colsLeft;
			} while (colsLeft);
			egaDestOff += linedelta;
			--linesLeft;
		} while (linesLeft);
		// Go to next plane
		dataLoc += planesize; // start of mask = start of next plane
		++planenum;
		planemask <<= 1; // shift plane mask over for next plane
	} while (planemask != 0x10); // done all four planes?
}

#if 0
DATASEG

UNWOUNDMASKS	=	10


maskroutines	dw	mask0,mask0,mask1E,mask1E,mask2E,mask2O,mask3E,mask3O
				dw	mask4E,mask4O,mask5E,mask5O,mask6E,mask6O
				dw	mask7E,mask7O,mask8E,mask8O,mask9E,mask9O
				dw	mask10E,mask10O


routinetouse	dw	?

CODESEG

PROC	VW_MaskBlock	segm:WORD, ofs:WORD, dest:WORD, wide:WORD, height:WORD, planesize:WORD
PUBLIC	VW_MaskBlock
USES	SI,DI

	mov	es,[screenseg]

	mov	[BYTE planemask],1
	mov	[BYTE planenum],0

	mov	di,[wide]
	mov	dx,[linewidth]
	sub	dx,[wide]
	mov	[linedelta],dx			;amount to add after drawing each line

	mov	bx,[planesize]			; si+bx = data location

	cmp	di,UNWOUNDMASKS
	jbe	@@unwoundroutine
	mov	[routinetouse],OFFSET generalmask
	jmp	NEAR @@startloop

;=================
;
; use the unwound routines
;
;=================

@@unwoundroutine:
	mov	cx,[dest]
	shr	cx,1
	rcl	di,1					;shift a 1 in if destination is odd
	shl	di,1					;to index into a word width table
	mov	ax,[maskroutines+di]	;call the right routine
	mov	[routinetouse],ax

@@startloop:
	mov	ds,[segm]

@@drawplane:
	mov	dx,SC_INDEX
	mov	al,SC_MAPMASK
	mov	ah,[ss:planemask]
	WORDOUT
	mov	dx,GC_INDEX
	mov	al,GC_READMAP
	mov	ah,[ss:planenum]
	WORDOUT

	mov	si,[ofs]				;start back at the top of the mask
	mov	di,[dest]				;start at same place in all planes
	mov	cx,[height]				;scan lines to draw
	mov dx,[ss:linedelta]

	jmp [ss:routinetouse]		;draw one plane
planereturn:					;routine jmps back here

	add	bx,[ss:planesize]		;start of mask = start of next plane

	inc	[ss:planenum]
	shl	[ss:planemask],1		;shift plane mask over for next plane
	cmp	[ss:planemask],10000b	;done all four planes?
	jne	@@drawplane

mask0:
	mov	ax,ss
	mov	ds,ax
	ret							;width of 0 = no drawing

;==============
;
; General purpose masked block drawing.  This could be optimised into
; four routines to use words, but few play loop sprites should be this big!
;
;==============

generalmask:
	mov	dx,cx

@@lineloopgen:
	mov	cx,[wide]
@@byteloop:
	mov	al,[es:di]
	and	al,[si]
	or	al,[bx+si]
	inc	si
	stosb
	loop	@@byteloop

	add	di,[ss:linedelta]
	dec	dx
	jnz	@@lineloopgen
	jmp	planereturn

;=================
;
; Horizontally unwound routines to draw certain masked blocks faster
;
;=================

MACRO	MASKBYTE
	mov	al,[es:di]
	and	al,[si]
	or	al,[bx+si]
	inc	si
	stosb
ENDM

MACRO	MASKWORD
	mov	ax,[es:di]
	and	ax,[si]
	or	ax,[bx+si]
	inc	si
	inc	si
	stosw
ENDM

MACRO	SPRITELOOP	addr
	add	di,dx
	loop	addr
	jmp	planereturn
ENDM


EVEN
mask1E:
	MASKBYTE
	SPRITELOOP	mask1E

EVEN
mask2E:
	MASKWORD
	SPRITELOOP	mask2E

EVEN
mask2O:
	MASKBYTE
	MASKBYTE
	SPRITELOOP	mask2O

EVEN
mask3E:
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask3E

EVEN
mask3O:
	MASKBYTE
	MASKWORD
	SPRITELOOP	mask3O

EVEN
mask4E:
	MASKWORD
	MASKWORD
	SPRITELOOP	mask4E

EVEN
mask4O:
	MASKBYTE
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask4O

EVEN
mask5E:
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask5E

EVEN
mask5O:
	MASKBYTE
	MASKWORD
	MASKWORD
	SPRITELOOP	mask5O

EVEN
mask6E:
	MASKWORD
	MASKWORD
	MASKWORD
	SPRITELOOP	mask6E

EVEN
mask6O:
	MASKBYTE
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask6O

EVEN
mask7E:
	MASKWORD
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask7E

EVEN
mask7O:
	MASKBYTE
	MASKWORD
	MASKWORD
	MASKWORD
	SPRITELOOP	mask7O

EVEN
mask8E:
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	SPRITELOOP	mask8E

EVEN
mask8O:
	MASKBYTE
	MASKWORD
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask8O

EVEN
mask9E:
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask9E

EVEN
mask9O:
	MASKBYTE
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	SPRITELOOP	mask9O

EVEN
mask10E:
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	SPRITELOOP	mask10E

EVEN
mask10O:
	MASKBYTE
	MASKWORD
	MASKWORD
	MASKWORD
	MASKWORD
	MASKBYTE
	SPRITELOOP	mask10O


ENDP
#endif


//============================================================================
//
// VW_ScreenToScreen
//
// Basic block copy routine.  Copies one block of screen memory to another,
// using write mode 1 (sets it and returns with write mode 0).  bufferofs is
// NOT accounted for.
//
//============================================================================

void VW_ScreenToScreen(id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t wide, id0_unsigned_t height)
{
	for (id0_unsigned_t lineCounter = height; lineCounter; --lineCounter, source += linewidth, dest += linewidth)
	{
		BE_ST_EGAUpdateGFXBufferScrToScr(dest, source, wide);
	}

}
#if 0
PROC	VW_ScreenToScreen	source:WORD, dest:WORD, wide:WORD, height:WORD
PUBLIC	VW_ScreenToScreen
USES	SI,DI

	pushf
	cli

	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK+15*256
	WORDOUT
	mov	dx,GC_INDEX
	mov	ax,GC_MODE+1*256
	WORDOUT

	popf

	mov	bx,[linewidth]
	sub	bx,[wide]

	mov	ax,[screenseg]
	mov	es,ax
	mov	ds,ax

	mov	si,[source]
	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw
	mov	ax,[wide]

@@lineloop:
	mov	cx,ax
	rep	movsb
	add	si,bx
	add	di,bx

	dec	dx
	jnz	@@lineloop

	mov	dx,GC_INDEX
	mov	ax,GC_MODE+0*256
	WORDOUT

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

ENDP
#endif


//============================================================================
//
// VW_MemToScreen
//
// Basic block drawing routine. Takes a block shape at segment pointer source
// with four planes of width by height data, and draws it to dest in the
// virtual screen, based on linewidth.  bufferofs is NOT accounted for.
// There are four drawing routines to provide the best optimized code while
// accounting for odd segment wrappings due to the floating screens.
//
//============================================================================


void VW_MemToScreen(memptr source, id0_unsigned_t dest,
	id0_unsigned_t wide,id0_unsigned_t height)
{
	id0_byte_t *srcPtr = (id0_byte_t *)source; // block is segment aligned
	id0_unsigned_t mapMask = 1; // map mask for plane 0
	do
	{
		id0_unsigned_t egaDestOff = dest; // start at same place in all planes
		id0_unsigned_t lineCounter = height; // scan lines to draw

		// NOTE: Using just one loop instead of four drawing routines now
		do
		{
			BE_ST_EGAUpdateGFXBuffer(egaDestOff, srcPtr, wide, mapMask);
			srcPtr += wide;
			egaDestOff += linewidth;
			--lineCounter;
		} while (lineCounter);
		mapMask <<= 1;
	} while (mapMask != 0x10);

}

#if 0
DATASEG

memtoscreentable	dw	eventoeven,eventoodd,oddtoeven,oddtoodd

CODESEG


PROC	VW_MemToScreen	source:WORD, dest:WORD, wide:WORD, height:WORD
PUBLIC	VW_MemToScreen
USES	SI,DI

	mov	es,[screenseg]

	mov	bx,[linewidth]
	sub	bx,[wide]

	mov	ds,[source]


	xor	si,si					;block is segment aligned

	xor	di,di
	shr	[wide],1				;change wide to words, and see if carry is set
	rcl	di,1					;1 if wide is odd
	mov	ax,[dest]
	shr	ax,1
	rcl	di,1					;shift a 1 in if destination is odd
	shl	di,1					;to index into a word width table
	mov	ax,SC_MAPMASK+0001b*256	;map mask for plane 0
	jmp	[ss:memtoscreentable+di]	;call the right routine

;==============
;
; Copy an even width block to an even video address
;
;==============

eventoeven:
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@lineloopEE:
	mov	cx,[wide]
	rep	movsw

	add	di,bx

	dec	dx
	jnz	@@lineloopEE

	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b				;done all four planes?
	jne	eventoeven

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

;==============
;
; Copy an odd width block to an even video address
;
;==============

oddtoeven:
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@lineloopOE:
	mov	cx,[wide]
	rep	movsw
	movsb						;copy the last byte

	add	di,bx

	dec	dx
	jnz	@@lineloopOE

	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b				;done all four planes?
	jne	oddtoeven

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

;==============
;
; Copy an even width block to an odd video address
;
;==============

eventoodd:
	dec	[wide]					;one word has to be handled seperately
EOplaneloop:
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@lineloopEO:
	movsb
	mov	cx,[wide]
	rep	movsw
	movsb

	add	di,bx

	dec	dx
	jnz	@@lineloopEO

	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b				;done all four planes?
	jne	EOplaneloop

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

;==============
;
; Copy an odd width block to an odd video address
;
;==============

oddtoodd:
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@lineloopOO:
	movsb
	mov	cx,[wide]
	rep	movsw

	add	di,bx

	dec	dx
	jnz	@@lineloopOO

	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b				;done all four planes?
	jne	oddtoodd

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret


ENDP
#endif

// REFKEEN - Unused function (for VW_DrawPic2x) and
// an unused macro from Catacomb Armageddon/Apocalypse
#if 0
; MDM (GAMERS EDGE) begin


MACRO	XPAND_BYTE
	test	al,128									; handle bit 7
	jne   @@over7
	or		[BYTE PTR es:di],11000000b
@@over7:

	test	al,64										; handle bit 6
	jne   @@over6
	or		[BYTE PTR es:di],00110000b
@@over6:

	test	al,32										; handle bit 5
	jne   @@over5
	or		[BYTE PTR es:di],00001100b
@@over5:

	test	al,16										; handle bit 4
	jne   @@over4
	or		[BYTE PTR es:di],00000011b
@@over4:

	inc	di					  						; inc destination

	test	al,8										; handle bit 3
	jne   @@over3
	or		[BYTE PTR es:di],11000000b
@@over3:

	test	al,4										; handle bit 2
	jne   @@over2
	or		[BYTE PTR es:di],00110000b
@@over2:

	test	al,2										; handle bit 1
	jne   @@over1
	or		[BYTE PTR es:di],00001100b
@@over1:

	test	al,1										; handle bit 0
	jne   @@over0
	or		[BYTE PTR es:di],00000011b
@@over0:

	inc	si											; inc source
	inc	di											; inc destination
ENDM


;============================================================================
;
; VW_MemToScreen2x
;
; Basic block drawing routine. Takes a block shape at segment pointer source
; with four planes of width by height data, and draws it to dest in the
; virtual screen, based on linewidth.  bufferofs is NOT accounted for.
; There are four drawing routines to provide the best optimized code while
; accounting for odd segment wrappings due to the floating screens.
;
;============================================================================

DATASEG

xpandhorz	db 00000000b,00000011b,00001100b,00001111b
				db 00110000b,00110011b,00111100b,00111111b
				db 11000000b,11000011b,11001100b,11001111b
				db 11110000b,11110011b,11111100b,11111111b

CODESEG


PROC	VW_MemToScreen2x	source:WORD, dest:WORD, wide:WORD, height:WORD
PUBLIC	VW_MemToScreen2x
USES	SI,DI

	mov	es,[screenseg]

	mov	bx,[linewidth]
	sub	bx,[wide]
	sub	bx,[wide]

	mov	ds,[source]


	xor	si,si					;block is segment aligned

	mov	ah,0001b				;map mask for plane 0

@@depthloop:
	mov	al,SC_MAPMASK			;restore map mask in al
	mov	dx,SC_INDEX
	WORDOUT

	mov	di,[dest]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@heightloop:
	mov	cx,[wide]
@@widthloop:

; handle first nybble
;
	push	di
	mov	di,[si]
	shr	di,1
	shr	di,1
	shr	di,1
	shr	di,1
	and	di,15
	mov	al,[ss:xpandhorz+di]
	pop	di
	mov	[es:di],al
	inc	di

; handle second nybble
;
	push	di
	mov	di,[si]
	and	di,15
	mov	al,[ss:xpandhorz+di]
	pop	di
	mov	[es:di],al
	inc	si
	inc	di


	dec	cx
	jne	@@widthloop

	add	di,bx

	dec	dx
	jnz	@@heightloop

	shl	ah,1					;shift plane mask over for next plane
	cmp	ah,10000b				;done all four planes?
	jne	@@depthloop

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

ENDP
; MDM (GAMERS EDGE) end
#endif

//===========================================================================
//
// VW_ScreenToMem
//
// Copies a block of video memory to main memory, in order from planes 0-3.
// This could be optimized along the lines of VW_MemToScreen to take advantage
// of word copies, but this is an infrequently called routine.
//
//===========================================================================

void VW_ScreenToMem(id0_unsigned_t source, memptr dest,
	id0_unsigned_t wide, id0_unsigned_t height)
{
	id0_byte_t *destPtr = (id0_byte_t *)dest;

	int planeCounter = 0;
	do
	{
		id0_unsigned_t egaSrcOff = source; //start at same place in all planes
		id0_unsigned_t lineCounter = height; // scan lines to draw
		do
		{
			BE_ST_EGAFetchGFXBuffer(destPtr, egaSrcOff, wide, planeCounter);
			egaSrcOff += linewidth;
			destPtr += wide;
			--lineCounter;
		} while (lineCounter);
		++planeCounter;
	} while (planeCounter != 4);
}


#if 0
PROC	VW_ScreenToMem	source:WORD, dest:WORD, wide:WORD, height:WORD
PUBLIC	VW_ScreenToMem
USES	SI,DI

	mov	es,[dest]

	mov	bx,[linewidth]
	sub	bx,[wide]

	mov	ds,[screenseg]

	mov	ax,GC_READMAP			;read map for plane 0

	xor	di,di

@@planeloop:
	mov	dx,GC_INDEX
	WORDOUT

	mov	si,[source]				;start at same place in all planes
	mov	dx,[height]				;scan lines to draw

@@lineloop:
	mov	cx,[wide]
	rep	movsb

	add	si,bx

	dec	dx
	jnz	@@lineloop

	inc	ah
	cmp	ah,4					;done all four planes?
	jne	@@planeloop

	mov	ax,ss
	mov	ds,ax					//restore turbo's data segment (UPDATE: Edited to C99/C++ style comment so compiler doesn't emit a warning)

	ret

ENDP
#endif


//============================================================================
//
// VWL_UpdateScreenBlocks
//
// Scans through the update matrix and copies any areas that have changed
// to the visable screen, then zeros the update array
//
//============================================================================

void VWL_UpdateScreenBlocks (void)
{
	id0_byte_t *scanPtr = updateptr;
	id0_byte_t *scanEndPtr = updateptr + UPDATEWIDE*UPDATEHIGH+1;
	id0_word_t iterationsToDo = 0xFFFF; // definitely scan the entire thing

	do
	{
		/*	
		 * scan for a 1 in the update list, meaning a tile needs
		 * to be copied from the master screen to the current screen
		 */
		while (iterationsToDo)
		{
			--iterationsToDo;
			if (*(scanPtr++) == 1)
			{
				break;
			}
		}

		if (scanPtr >= scanEndPtr) // all tiles have been scanned
		{
			memset(updateptr, 0, 2*(UPDATEWIDE*UPDATEHIGH/2)); // clear out the update matrix
			return;
		}
		if (*scanPtr != 1)
		{
			//============
			//
			// copy a single tile
			//
			//============

			++scanPtr; // we know the next tile is nothing
			id0_word_t tileLoc = blockstarts[scanPtr-updateptr-2]; // start of tile location on screen
			id0_word_t egaSrcOff = tileLoc+bufferofs;
			id0_word_t egaDestOff = tileLoc+displayofs;
			for (int loopVar = 15; loopVar; --loopVar)
			{
				BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
				BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
				egaSrcOff += linewidth-2;
				egaDestOff += linewidth-2;
			}
			BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
			BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff, egaSrcOff);
			continue;
		}
		//============
		//
		// more than one tile in a row needs to be updated, so do it as a group
		//
		//============
		id0_byte_t *rowScanStartPtr = scanPtr; // hold starting position + 1
		++scanPtr; // we know the next tile also gets updated
		// see how many more in a row
		while (iterationsToDo)
		{
			--iterationsToDo;
			if (*(scanPtr++) != 1)
			{
				break;
			}
		}
		id0_word_t bytesPerRow = 2*(scanPtr - rowScanStartPtr);
		id0_word_t tileLoc = blockstarts[rowScanStartPtr-updateptr-1]; // start of tile location
		id0_word_t egaSrcOff = tileLoc+bufferofs;
		id0_word_t egaDestOff = tileLoc+displayofs;
		id0_word_t bytesToSkip = linewidth-bytesPerRow;
		for (int loopVar = 15; loopVar; --loopVar)
		{
			iterationsToDo = bytesPerRow;
			for (; iterationsToDo; --iterationsToDo)
			{
				BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
			}
			egaSrcOff += bytesToSkip;
			egaDestOff += bytesToSkip;
		}
		iterationsToDo = bytesPerRow;
		for (; iterationsToDo; --iterationsToDo)
		{
			BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
		}
		iterationsToDo = 0;
		// was 0, now 0xFFFF for above loop
		// WARNING: This should be UNSIGNED, or else we get undefined behaviors
		--iterationsToDo;
	} while (true);
}

#if 0
; AX	0/1 for scasb, temp for segment register transfers
; BX    width for block copies
; CX	REP counter
; DX	line width deltas
; SI	source for copies
; DI	scas dest / movsb dest
; BP	pointer to end of bufferblocks

PROC	VWL_UpdateScreenBlocks
PUBLIC	VWL_UpdateScreenBlocks
USES	SI,DI,BP

	jmp	SHORT @@realstart
@@done:
;
; all tiles have been scanned
;
	mov	dx,GC_INDEX				; restore write mode 0
	mov	ax,GC_MODE+0*256
	WORDOUT

	xor	ax,ax					; clear out the update matrix
	mov	cx,UPDATEWIDE*UPDATEHIGH/2

	mov	di,[updateptr]
	rep	stosw

	ret

@@realstart:
	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK+15*256
	WORDOUT
	mov	dx,GC_INDEX
	mov	ax,GC_MODE+1*256
	WORDOUT

	mov	di,[updateptr]			; start of floating update screen
	mov	bp,di
	add	bp,UPDATEWIDE*UPDATEHIGH+1 ; when di = bp, all tiles have been scanned

	push	di
	mov	cx,-1					; definately scan the entire thing

;
; scan for a 1 in the update list, meaning a tile needs to be copied
; from the master screen to the current screen
;
@@findtile:
	pop	di						; place to continue scaning from
	mov	ax,ss
	mov	es,ax					; search in the data segment
	mov	ds,ax
	mov al,1
	repne	scasb
	cmp	di,bp
	jae	@@done

	cmp	[BYTE di],al
	jne	@@singletile
	jmp	@@tileblock

;============
;
; copy a single tile
;
;============
@@singletile:
	inc	di						; we know the next tile is nothing
	push	di					; save off the spot being scanned
	sub	di,[updateptr]
	shl	di,1
	mov	di,[blockstarts-4+di]	; start of tile location on screen
	mov	si,di
	add	si,[bufferofs]
	add	di,[displayofs]

	mov	dx,[linewidth]
	sub	dx,2
	mov	ax,[screenseg]
	mov	ds,ax
	mov	es,ax

REPT	15
	movsb
	movsb
	add	si,dx
	add	di,dx
ENDM
	movsb
	movsb

	jmp	@@findtile

;============
;
; more than one tile in a row needs to be updated, so do it as a group
;
;============
EVEN
@@tileblock:
	mov	dx,di					; hold starting position + 1 in dx
	inc	di						; we know the next tile also gets updated
	repe	scasb				; see how many more in a row
	push	di					; save off the spot being scanned

	mov	bx,di
	sub	bx,dx					; number of tiles in a row
	shl	bx,1					; number of bytes / row

	mov	di,dx					; lookup position of start tile
	sub	di,[updateptr]
	shl	di,1
	mov	di,[blockstarts-2+di]	; start of tile location
	mov	si,di
	add	si,[bufferofs]
	add	di,[displayofs]

	mov	dx,[linewidth]
	sub	dx,bx					; offset to next line on screen

	mov	ax,[screenseg]
	mov	ds,ax
	mov	es,ax

REPT	15
	mov	cx,bx
	rep	movsb
	add	si,dx
	add	di,dx
ENDM
	mov	cx,bx
	rep	movsb

	dec	cx						; was 0 from last rep movsb, now $ffff for scasb
	jmp	@@findtile


ENDP
#endif


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

void 	VW_SetScreen (id0_unsigned_t CRTC, id0_unsigned_t pelpan)
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

#if NUMFONT+NUMFONTM

//===========================================================================
//
// GENERAL FONT DRAWING ROUTINES
//
//===========================================================================

/* SIGNED */ id0_int_t px, py; // proportional character drawing coordinates
//pdrawmode
id0_byte_t fontcolor = 15; // 0-15 mapmask value

/*
 * offsets in font structure
 */
//#define pcharheight		0	// lines high
//#define charloc			2	// pointers to every character
//#define charwidth		514	// every character's width in pixels


//id0_word_t propchar; // the character number to shift // UNUSED VARIABLE
id0_char_t *stringptr;


#ifdef REFKEEN_VER_CATADVENTURES

#ifdef REFKEEN_VER_CATABYSS
#define BUFFWIDTH 80 // MDM (GAMERS EDGE) - increased from 50
#else
#define BUFFWIDTH 82 // MDM (GAMERS EDGE) - increased from 50
#endif
#define BUFFHEIGHT 20 // must be twice as high as font for masked fonts

#else

#define BUFFWIDTH 50
#define BUFFHEIGHT 32 // must be twice as high as font for masked fonts

#endif

id0_byte_t databuffer[BUFFWIDTH*BUFFHEIGHT];

id0_word_t bufferbyte;
id0_word_t bufferbit;

id0_word_t bufferextra; // add at end of a line copy
id0_word_t screenextra;

id0_word_t bufferwidth;
id0_word_t bufferheight;
id0_word_t screenspot;

//======================
//
// Macros to table shift a byte of font
//
//======================

#define SHIFTNOXOR \
	currshift = currshifttable[*srcPtr]; /* table shift into two bytes */ \
	*(destPtr++) |= (currshift & 0xFF); /* or with first byte */ \
	*destPtr = (currshift >> 8); /* replace next byte */ \
	srcPtr++; /* next source byte */


/*** LOOKS LIKE THIS ONE IS UNUSED ***/
#if 0 
#define SHIFTWITHXOR \
	currshift = ~currshifttable[*srcPtr]; /* table neg of shift into two bytes */ \
	*(destPtr++) &= (currshift & 0xFF); /* and with first byte */ \
	*destPtr = (currshift >> 8); /* replace next byte */ \
	srcPtr++; /* next source byte */
#endif


//=======================
//
// VWL_XORBuffer (BufferToScreen)
//
// Pass buffer start in SI (somewhere in databuffer)
// Draws the buffer to the EGA screen in the current write mode
//
//========================

// NOTE (REFKEEN): Renamed BufferToScreen -> VWL_XORBuffer because:
// - It's used only with the xor operation, while original implementation
// could have worked with other operations by modifying the EGA "Data Rotate"
// register.
// - The only other function with calls to BufferToScreen
// is VW_DrawMPropString, but it appears to be unused.
// - Also a bit more consistent with the CGA counterpart.

//void BufferToScreen(id0_byte_t *buffer)
void VWL_XORBuffer(id0_byte_t *buffer)
{
	if (!bufferwidth)
	{
		return; // nothing to draw
	}
	id0_byte_t *srcPtr = buffer;
	id0_unsigned_t egaDestOff = screenspot;
	// calculate offsets for end of each line
	screenextra = linewidth-bufferwidth;
	bufferextra = BUFFWIDTH-bufferwidth;

	id0_unsigned_t linesLeft = bufferheight; // lines to copy
	do
	{
		id0_unsigned_t bytesLeft = bufferwidth; // bytes to copy
		do
		{
			// Originally the mask is set from VW_DrawPropString
			BE_ST_EGAXorGFXByte(egaDestOff++, *(srcPtr++), fontcolor);
			--bytesLeft;
		} while (bytesLeft);
		srcPtr += bufferextra;
		egaDestOff += screenextra;
		--linesLeft;
	} while (linesLeft);
}


//============================================================================
//
// NON MASKED FONT DRAWING ROUTINES
//
//============================================================================

#if NUMFONT

//==================
//
// ShiftPropChar
//
// Call with BX = character number (0-255)
// Draws one character to the buffer at bufferbyte/bufferbit, and adjusts
// them to the new position
//
//==================

void ShiftPropChar(id0_word_t charnum)
{
	// WARNING: Later [es:bx] is the source while [di] (ds:di) is the dest

/*
 * find character location, width, and height
 */

	// Font to use
	fontstruct *currfont = (fontstruct *)(grsegs[STARTFONT+fontnumber]);
	// Width in pixels
	id0_word_t adjustedwidth = (currfont->width[charnum]) & 0xFF;
	// Location of data in the font
	id0_int_t fontloc = currfont->location[charnum];
	// Going to point at character data
	id0_byte_t *srcPtr = (id0_byte_t *)currfont+fontloc;

/*
 * look up which shift table to use, based on bufferbit
 */
	id0_unsigned_t *currshifttable = shifttabletable[bufferbit];
	id0_unsigned_t currshift;
	id0_byte_t *destPtr = &databuffer[bufferbyte];
/*
 * advance position by character width
 */
	id0_word_t posToSplit = bufferbit+adjustedwidth;
	bufferbit = (posToSplit & 7); // new bit position
	bufferbyte += (posToSplit >> 3); // new byte position

	switch ((adjustedwidth+7)>>3)
	{
	case 1: // one byte character
		for (id0_word_t loopvar = currfont->height; loopvar; --loopvar, destPtr += BUFFWIDTH-1)
		{
			SHIFTNOXOR
		}
		break;
	case 2: // two byte character
		for (id0_word_t loopvar = currfont->height; loopvar; --loopvar, destPtr += BUFFWIDTH-2)
		{
			SHIFTNOXOR
			SHIFTNOXOR
		}
		break;
	case 3: // three byte character
		for (id0_word_t loopvar = currfont->height; loopvar; --loopvar, destPtr += BUFFWIDTH-3)
		{
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
		}
		break;
	case 4: // four byte character
		for (id0_word_t loopvar = currfont->height; loopvar; --loopvar, destPtr += BUFFWIDTH-4)
		{
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
		}
		break;
	case 5: // five byte character
		for (id0_word_t loopvar = currfont->height; loopvar; --loopvar, destPtr += BUFFWIDTH-5)
		{
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
			SHIFTNOXOR
		}
		break;
	}
}

//============================================================================

//==================
//
// VW_DrawPropString
//
// Draws a C string of characters at px/py and advances px
//
//==================

void VW_DrawPropString (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend)
{
	// (REFKEEN) Modifications from vanilla Keen:
	// - All input strings are now const.
	// - An additional "optsend" argument marking one char past end
	// of string. Set to NULL for original behaviors.
	// - Related to modifcation to US_Print and US_CPrint, properly taking
	// care of C string literals as inputs.

	/*
	 * proportional spacing, which clears the buffer ahead of it, so only
	 * clear the first column
	 */
	for (id0_word_t line = 0; line < BUFFHEIGHT; ++line) {
		databuffer[BUFFWIDTH*line] = 0;
	}
	/*
	 * shift the characters into the buffer
	 */
	bufferbit = ((id0_unsigned_t)px&7);
	bufferbyte = 0;
	if (optsend)
	{
		while (string != optsend) // Check if e.g., *string=='\n'
		{
			id0_byte_t currChAsUnsigned = *(string++);
			if (!currChAsUnsigned)
			{
				break;
			}
			ShiftPropChar(currChAsUnsigned);
		}
	}
	else
	{
		do // We're going to check *string is non-NULL anyway
		{
			id0_byte_t currChAsUnsigned = *(string++);
			if (!currChAsUnsigned)
			{
				break;
			}
			ShiftPropChar(currChAsUnsigned);
		} while (true);
	}

	/*
	 * calculate position to draw buffer on screen
	 */

	// px>>2: x location in bytes
	screenspot = (ylookup[py]+bufferofs+panadjust) + (px>>3);

	// advance px
	px += ((bufferbyte << 3) | bufferbit);

	// draw it
	bufferwidth = (bufferbit & 7) ? (bufferbyte+1) : bufferbyte;
	fontstruct *currfont = (fontstruct *)(grsegs[STARTFONT+fontnumber]);
	bufferheight = currfont->height;
	VWL_XORBuffer(databuffer);
}

#endif	// numfont

/*
 * Seemingly unused functions
 */

#if 0

;============================================================================
;
; MASKED FONT DRAWING ROUTINES
;
;============================================================================

#if NUMFONTM

DATASEG

mshiftdrawtable	dw      0,mshift1wide,mshift2wide,mshift3wide


CODESEG

;==================
;
; ShiftMPropChar
;
; Call with BX = character number (0-255)
; Draws one character to the buffer at bufferbyte/bufferbit, and adjusts
; them to the new position
;
;==================

PROC	ShiftMPropChar	NEAR

	mov	si,[fontnumber]
	shl	si,1
	mov	es,[grsegs+STARTFONTM*2+si]	;segment of font to use

;
; find character location, width, and height
;
	mov	si,[es:charwidth+bx]
	and	si,0ffh					;SI hold width in pixels
	shl	bx,1
	mov	bx,[es:charloc+bx]		;BX holds pointer to character data

;
; look up which shift table to use, based on bufferbit
;
	mov	di,[bufferbit]
	shl	di,1
	mov	bp,[shifttabletable+di]	;BP holds pointer to shift table

	mov	di,OFFSET databuffer
	add	di,[bufferbyte]			;DI holds pointer to buffer

	mov	cx,[bufferbit]
	add	cx,si					;new bit position
	mov	ax,cx
	and	ax,7
	mov	[bufferbit],ax			;new bit position
	mov	ax,cx
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	[bufferbyte],ax			;new byte position

	add	si,7
	shr	si,1
	shr	si,1
	shr	si,1					;bytes the character is wide
	shl	si,1                    ;*2 to look up in shiftdrawtable

	mov	cx,[es:pcharheight]
	mov	dx,BUFFWIDTH
	jmp	[ss:mshiftdrawtable+si]	;procedure to draw this width

;
; one byte character
;
mshift1wide:
	dec	dx

EVEN
@@loop1m:
	SHIFTWITHXOR
	add	di,dx			; next line in buffer

	loop	@@loop1m

	mov	cx,[es:pcharheight]

EVEN
@@loop1:
	SHIFTNOXOR
	add	di,dx			; next line in buffer
	loop	@@loop1

	ret

;
; two byte character
;
mshift2wide:
	dec	dx
	dec	dx
EVEN
@@loop2m:
	SHIFTWITHXOR
	SHIFTWITHXOR
	add	di,dx			; next line in buffer

	loop	@@loop2m

	mov	cx,[es:pcharheight]

EVEN
@@loop2:
	SHIFTNOXOR
	SHIFTNOXOR
	add	di,dx			; next line in buffer
	loop	@@loop2

	ret

;
; three byte character
;
mshift3wide:
	sub	dx,3
EVEN
@@loop3m:
	SHIFTWITHXOR
	SHIFTWITHXOR
	SHIFTWITHXOR
	add	di,dx			; next line in buffer

	loop	@@loop3m

	mov	cx,[es:pcharheight]

EVEN
@@loop3:
	SHIFTNOXOR
	SHIFTNOXOR
	SHIFTNOXOR
	add	di,dx			; next line in buffer
	loop	@@loop3

	ret


ENDP

;============================================================================

;==================
;
; VW_DrawMPropString
;
; Draws a C string of characters at px/py and advances px
;
; Assumes write mode 0
;
;==================



PROC	VW_DrawMPropString	string:DWORD
PUBLIC	VW_DrawMPropString
USES	SI,DI

;
; clear out the first byte of the buffer, the rest will automatically be
; cleared as characters are drawn into it
;
	mov	si,[fontnumber]
	shl	si,1
	mov	es,[grsegs+STARTFONTM*2+si]
	mov	dx,[es:pcharheight]
	mov	di,OFFSET databuffer
	mov	ax,ds
	mov	es,ax
	mov	bx,BUFFWIDTH-1

	mov	cx,dx
	mov	al,0ffh
@@maskfill:
	stosb				; fill the mask part with $ff
	add	di,bx
	loop	@@maskfill

	mov	cx,dx
	xor	al,al
@@datafill:
	stosb				; fill the data part with $0
	add	di,bx
	loop	@@datafill

;
; shift the characters into the buffer
;
	mov	ax,[px]
	and	ax,7
	mov	[bufferbit],ax
	mov	[bufferbyte],0

	mov	ax,[WORD string]
	mov	[stringptr],ax
	mov	ax,[WORD string+2]
	mov	[stringptr+2],ax

@@shiftone:
	mov	es,[stringptr+2]
	mov	bx,[stringptr]
	inc	[stringptr]
	mov	bx,[es:bx]
	xor	bh,bh
	or	bl,bl
	jz	@@allshifted
	call	ShiftMPropChar
	jmp	@@shiftone

@@allshifted:
;
; calculate position to draw buffer on screen
;
	mov	bx,[py]
	shl	bx,1
	mov	di,[ylookup+bx]
	add	di,[bufferofs]
	add	di,[panadjust]

	mov	ax,[px]
	shr	ax,1
	shr	ax,1
	shr	ax,1		;x location in bytes
	add	di,ax
	mov	[screenspot],di

;
; advance px
;
	mov	ax,[bufferbyte]
	shl	ax,1
	shl	ax,1
	shl	ax,1
	or	ax,[bufferbit]
	add	[px],ax

;
; draw it
;
	mov	ax,[bufferbyte]
	test	[bufferbit],7
	jz	@@go
	inc	ax				;so the partial byte also gets drawn
@@go:
	mov	[bufferwidth],ax
	mov	es,[grsegs+STARTFONTM*2]
	mov	ax,[es:pcharheight]
	mov	[bufferheight],ax

; set AND mode to punch out the mask
	mov	dx,GC_INDEX
	mov	ax,GC_DATAROTATE + 8*256
	WORDOUT

; set mapmask to all
	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK + 15*256
	WORDOUT

	mov	si,OFFSET databuffer
	call	BufferToScreen

; set OR mode to fill in the color
	mov	dx,GC_INDEX
	mov	ax,GC_DATAROTATE + 16*256
	WORDOUT

; set mapmask to color
	mov	dx,SC_INDEX
	mov	al,SC_MAPMASK
	mov	ah,[fontcolor]
	WORDOUT

	call	BufferToScreen		; SI is still in the right position in buffer

; set copy mode
	mov	dx,GC_INDEX
	mov	ax,GC_DATAROTATE
	WORDOUT

; set mapmask to all
	mov	dx,SC_INDEX
	mov	ax,SC_MAPMASK + 15*256
	WORDOUT


	ret

ENDP

#endif // if NUMFONTM

#endif // VW_DrawMPropString

#endif // if fonts
