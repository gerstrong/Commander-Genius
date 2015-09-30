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

#ifdef REFKEEN_VER_CATADVENTURES
#include "def.h"
#define VIEWWIDTH (40*8)
#else
#include "c3_def.h"
#define VIEWWIDTH (33*8)
#endif

//=================== Tables filled in by DrawVWall ==========================

/*
 * wallheight has the height (scale number) of that collumn of scaled wall
 * it is pre bounded to 1-MAXSCALE (the actuial height on screen is 2*height)
 */
id0_word_t wallheight[VIEWWIDTH];

/*
 * wallwidth has the pixel width (1-7) of that column
 */
id0_word_t wallwidth[VIEWWIDTH];

/*
 * wallseg has the segment of the wall picture
 */
id0_byte_t *wallseg[VIEWWIDTH];
//wallseg		dw	VIEWWIDTH dup (?)

/*
 * wallofs has the offset of the wall picture
 */
id0_word_t wallofs[VIEWWIDTH];
//wallofs		dw	VIEWWIDTH dup (?)

//============================================================================

/* (REFKEEN) ASM to C porting: Unfortunately we don't have a clear way
 * in C99 to fill an array of VIEWWIDTH integers at compile time, so that
 * the i-th element is f(i) for every i (where f is a known simple function).
 *
 * This is how the array was filled in ASM (with REPT).
 *
 * Instead, we manually fill such arrays of a sufficiently large common size,
 * as they're accessed just here anyway. They may further be declared static.
 *
 * Note: Should be possible in C++11 with a few tricks at the least.
 */

// Catacomb 3-D defines VIEWWIDTH to be (33*8); Adventure Series - (40*8).
#define REFKEEN_VIEWWIDTH_UPPERBOUND (40*8)
#if (VIEWWIDTH > REFKEEN_VIEWWIDTH_UPPERBOUND)
#error "VIEWWIDTH > REFKEEN_VIEWWIDTH_UPPERBOUND!"
#endif

/*
 * screenbyte is just position/8
 */
static id0_word_t screenbyte[REFKEEN_VIEWWIDTH_UPPERBOUND] = {
	 0,  0,  0,  0,  0,  0,  0,  0,
	 1,  1,  1,  1,  1,  1,  1,  1,
	 2,  2,  2,  2,  2,  2,  2,  2,
	 3,  3,  3,  3,  3,  3,  3,  3,
	 4,  4,  4,  4,  4,  4,  4,  4,
	 5,  5,  5,  5,  5,  5,  5,  5,
	 6,  6,  6,  6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,  7,  7,  7,
	 8,  8,  8,  8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,  9,  9,  9,
	10, 10, 10, 10, 10, 10, 10, 10,
	11, 11, 11, 11, 11, 11, 11, 11,
	12, 12, 12, 12, 12, 12, 12, 12,
	13, 13, 13, 13, 13, 13, 13, 13,
	14, 14, 14, 14, 14, 14, 14, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	16, 16, 16, 16, 16, 16, 16, 16,
	17, 17, 17, 17, 17, 17, 17, 17,
	18, 18, 18, 18, 18, 18, 18, 18,
	19, 19, 19, 19, 19, 19, 19, 19,
	20, 20, 20, 20, 20, 20, 20, 20,
	21, 21, 21, 21, 21, 21, 21, 21,
	22, 22, 22, 22, 22, 22, 22, 22,
	23, 23, 23, 23, 23, 23, 23, 23,
	24, 24, 24, 24, 24, 24, 24, 24,
	25, 25, 25, 25, 25, 25, 25, 25,
	26, 26, 26, 26, 26, 26, 26, 26,
	27, 27, 27, 27, 27, 27, 27, 27,
	28, 28, 28, 28, 28, 28, 28, 28,
	29, 29, 29, 29, 29, 29, 29, 29,
	30, 30, 30, 30, 30, 30, 30, 30,
	31, 31, 31, 31, 31, 31, 31, 31,
	32, 32, 32, 32, 32, 32, 32, 32,
	33, 33, 33, 33, 33, 33, 33, 33,
	34, 34, 34, 34, 34, 34, 34, 34,
	35, 35, 35, 35, 35, 35, 35, 35,
	36, 36, 36, 36, 36, 36, 36, 36,
	37, 37, 37, 37, 37, 37, 37, 37,
	38, 38, 38, 38, 38, 38, 38, 38,
	39, 39, 39, 39, 39, 39, 39, 39,
};

/*
 * screenbit is (position&7)*16
 */
static id0_word_t screenbit[REFKEEN_VIEWWIDTH_UPPERBOUND] = {
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
	 0, 16, 32, 48, 64, 80, 96, 112,
};

/*
 * Use offset: screenbit[]+pixwidth*2
 * access from bitmasks-2+offset for one biased pixwidth
 * the low byte of bitmasks is for the first screen byte, the high byte
 * is the bitmask for the second screen byte (if non 0)
 */

id0_word_t bitmasks[] = {
	0x0080, 0x00c0, 0x00e0, 0x00f0, 0x00f8, 0x00fc, 0x00fe, 0x00ff,
	0x0040, 0x0060, 0x0070, 0x0078, 0x007c, 0x007e, 0x007f, 0x807f,
	0x0020, 0x0030, 0x0038, 0x003c, 0x003e, 0x003f, 0x803f, 0x0c03f,
	0x0010, 0x0018, 0x001c, 0x001e, 0x001f, 0x801f, 0x0c01f, 0x0e01f,
	0x0008, 0x000c, 0x000e, 0x000f, 0x800f, 0x0c00f, 0x0e00f, 0x0f00f,
	0x0004, 0x0006, 0x0007, 0x8007, 0x0c007, 0x0e007, 0x0f007, 0x0f807,
	0x0002, 0x0003, 0x8003, 0x0c003, 0x0e003, 0x0f003, 0x0f803, 0x0fc03,
	0x0001, 0x8001, 0x0c001, 0x0e001, 0x0f001, 0x0f801, 0x0fc01, 0x0fe01,
};

/*
 * wallscalecall is a far pointer to the start of a compiled scaler
 * The low word will never change, while the high word is set to
 * compscaledirectory[scale]
 */
//wallscalecall	dd	(65*6)			; offset of t_compscale->code[0]


#if 0
EXTRN	scaledirectory:WORD			; array of MAXSCALE segment pointers to
									; compiled scalers
EXTRN	screenseg:WORD				; basically just 0xa000
EXTRN	bufferofs:WORD				; offset of the current work screen
EXTRN ylookup:WORD
EXTRN screenpage:WORD
#endif


//============================================================================
//
// ScaleWalls
//
// AX	AL is scratched in bit mask setting and scaling
// BX	table index
// CX	pixwidth*2
// DX	GC_INDEX
// SI	offset into wall data to scale from, allways 0,64,128,...4032
// DI    byte at top of screen that the collumn is contained in
// BP	x pixel * 2, index into VIEWWIDTH wide tables
// DS	segment of the wall data to texture map
// ES	screenseg
// SS	addressing DGROUP variables
//
//============================================================================

void ScaleWalls(void)
{
	id0_unsigned_t colNum = 0;
	do
	{
		if (!wallwidth[colNum])
		{
			++colNum;
			continue;
		}
		// (REFKEEN) VANILLA CAT3D BUG WORKAROUND: It may happen that an
		// attempt to draw an "empty" wall is done, so better use this.
		// Example: Due to a different bug depending on EXE layout,
		// random data could've drawn during in the first Cat3D map,
		// with the player at the position given by x==354607, y==497530
		// and with the viewing angle given by angle==196.
		// (Array copied off DOSBox 0000:0000..)
		id0_byte_t *wallSrcPtr = (wallseg[colNum] ? wallseg[colNum] : g_be_cross_dosZeroSeg) + wallofs[colNum];
		//id0_byte_t *wallSrcPtr = wallseg[colNum]+wallofs[colNum];
		id0_unsigned_t egaDestOff = screenbyte[colNum] + bufferofs;
		id0_unsigned_t masksPair = bitmasks[screenbit[colNum]/2 + wallwidth[colNum] - 1];
		// (REFKEEN) Replacement for functions generated by BuildCompScale
		ExecuteCompScale(&scaledirectory[wallheight[colNum]]->code[0], egaDestOff, wallSrcPtr, masksPair&0xFF);
		if (masksPair&0xFF00) // is there anything in the second byte?
		{
			++egaDestOff;
			ExecuteCompScale(&scaledirectory[wallheight[colNum]]->code[0], egaDestOff, wallSrcPtr, masksPair>>8);
		}
		colNum += wallwidth[colNum];
	} while (colNum < VIEWWIDTH);
}

#if 0
PROC	ScaleWalls
PUBLIC	ScaleWalls
USES	SI,DI,BP

	xor	bp,bp						; start at location 0 in the tables
	mov	dx,GC_INDEX+1
	mov	es,[screenseg]

;
; scale one collumn of data, possibly across two bytes
;
nextcollumn:

	mov	bx,[wallheight+bp]			; height of walls (1-MAXSCALE)
	shl	bx,1
	mov	ax,[ss:scaledirectory+bx]	; segment of the compiled scaler
	mov [WORD PTR ss:wallscalecall+2],ax

	mov	cx,[wallwidth+bp]
	or	cx,cx
	jnz	okwidth
	mov	cx,2
	jmp	next

okwidth:
	shl	cx,1
	mov	ds,[wallseg+bp]
	mov	si,[wallofs+bp]

	mov	di,[screenbyte+bp]			; byte at the top of the scaled collumn
	add	di,[ss:bufferofs]			; offset of current page flip
	mov	bx,[screenbit+bp]			; 0-7 << 4
	add	bx,cx
	mov	ax,[ss:bitmasks-2+bx]
	out	dx,al						; set bit mask register
	call [DWORD PTR ss:wallscalecall]		; scale the line of pixels
	or	ah,ah						; is there anything in the second byte?
	jnz	secondbyte
;
; next
;
next:
	add	bp,cx
	cmp	bp,VIEWWIDTH*2
	jb	nextcollumn
	jmp	done

;
; draw a second byte for vertical strips that cross two bytes
;
secondbyte:
	mov	al,ah
	inc	di								; next byte over
	out	dx,al							; set bit mask register
	call [DWORD PTR ss:wallscalecall]	; scale the line of pixels
;
; next
;
	add	bp,cx
	cmp	bp,VIEWWIDTH*2
	jb	nextcollumn

done:
	mov	ax,ss
	mov	ds,ax
	ret

ENDP
#endif

#ifdef REFKEEN_VER_CATADVENTURES
//---------------------------------------------------------------------------
//
// RadarBlip()
//
// Displays a 'blip' (1 pixel wide X 2 pixel high) on the radar at
// an (X,Y) relative to (RADAR_X,RADAR_Y) (defined below...)
//
//---------------------------------------------------------------------------

void RadarBlip(id0_word_t x, id0_word_t y, id0_word_t color)
{
	id0_word_t egaOffset = ylookup[y] + x/8;
	BE_ST_EGAUpdateGFXPixel4bpp(egaOffset, color&0xFF, ((unsigned)0x80)>>(x&7));
#if 0
	mov	ax,[screenseg]

	mov	es,ax
	xor	di,di

	lea	si,[ylookup]
	add	si,[y]
	add	si,[y]
	add	di,[si]

	mov	ax,[x]
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	di,ax

	mov	ax,[x]
	and	ax,7
	mov	cl,al
	mov	ah,080h
	shr	ah,cl
	cli
	mov	al,GC_BITMASK
	mov	dx,GC_INDEX
	out	dx,ax
	sti

	mov	ax,[color]
	mov	ah,[es:di]						; read into latches
	mov	[es:di],al						; write latches / color bit

	ret
#endif
}
#endif
