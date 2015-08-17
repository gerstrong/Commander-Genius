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

 * ID_RF_A.C
 */

// Ported from ASM

#include "id_heads.h"

//============================================================================

#define TILESWIDE 21
#define TILESHIGH 14

//#define UPDATESIZE ((TILESWIDE+1)*TILESHIGH+1)

extern id0_unsigned_t screenstart[3]; // starts of three screens (0/1/master) in EGA mem
extern id0_unsigned_t originmap;
extern id0_byte_t id0_seg *tinf;

extern id0_byte_t planemask;
extern id0_byte_t planenum;

id0_unsigned_t screenstartcs; // in code segment for accesability


#if GRMODE == CGAGR
//============================================================================
//
// CGA refresh routines
//
//============================================================================

#define TILEWIDTH 4

//=================
//
// RFL_NewTile
//
// Draws a composit two plane tile to the master screen and sets the update
// spot to 1 in both update pages, forcing the tile to be copied to the
// view pages the next two refreshes
//
// Called to draw newly scrolled on strips and animating tiles
//
//=================

void RFL_NewTile (id0_unsigned_t updateoffset)
{
	/*
	 * mark both update lists at this spot
	 */
	updateptr[updateoffset] = 1;
	// offset in map from origin
	id0_unsigned_t currmapoffset = updatemapofs[updateoffset]+originmap;
	// screen location for tile
	id0_unsigned_t currtileoffset = blockstarts[updateoffset]+masterofs;
	// destination pointer on screen
	id0_byte_t *destPtr = &screenseg[currtileoffset];
	// If either of the tile numbers is 0xFFFF, the tile does not need to
	// be masked together; as one of the planes totally eclipses the other
	id0_unsigned_t foretilenum = mapsegs[1][currmapoffset/2];
	id0_unsigned_t backtilenum = mapsegs[0][currmapoffset/2];
	if (!foretilenum)
	{
		//=============
		//
		// Draw single background tile from main memory
		//
		//=============
		const id0_byte_t *backSrcPtr = (const id0_byte_t *)grsegs[STARTTILE16+backtilenum];
		backSrcPtr = backSrcPtr ? backSrcPtr : g_be_cross_dosZeroSeg; // VANILLA KEEN BUG WORKAROUND ("Empty" tile found in map)
		for (int loopVar = 15; loopVar; --loopVar, backSrcPtr += TILEWIDTH, BE_Cross_Wrapped_Add(screenseg, &destPtr, SCREENWIDTH))
		{
			BE_Cross_LinearToWrapped_MemCopy(screenseg, destPtr, backSrcPtr, TILEWIDTH);
		}
		BE_Cross_LinearToWrapped_MemCopy(screenseg, destPtr, backSrcPtr, TILEWIDTH);

		//BE_ST_MarkGfxForPendingUpdate();
		return;
	}
	//=========
	//
	// Draw a masked tile combo
	//
	//=========
	const id0_byte_t *foreSrcPtr = (const id0_byte_t *)grsegs[STARTTILE16M+foretilenum];
	const id0_byte_t *backSrcPtr = (const id0_byte_t *)grsegs[STARTTILE16+backtilenum];
	backSrcPtr = backSrcPtr ? backSrcPtr : g_be_cross_dosZeroSeg; // VANILLA KEEN BUG WORKAROUND ("Empty" tile found in map)

	for (int loopVar = 16; loopVar; --loopVar, backSrcPtr += TILEWIDTH-3, foreSrcPtr += TILEWIDTH-3, BE_Cross_Wrapped_Add(screenseg, &destPtr, SCREENWIDTH-3))
	{
		// backSrcPtr - background tile
		// foreSrcPtr - mask
		// &foreSrcPtr[64] - masked data
		*destPtr = ((*backSrcPtr++) & (*foreSrcPtr))
		                       | (foreSrcPtr)[64];
		BE_Cross_Wrapped_Inc(screenseg, &destPtr);
		++foreSrcPtr;
		*destPtr = ((*backSrcPtr++) & (*foreSrcPtr))
		                       | (foreSrcPtr)[64];
		BE_Cross_Wrapped_Inc(screenseg, &destPtr);
		++foreSrcPtr;
		*destPtr = ((*backSrcPtr++) & (*foreSrcPtr))
		                       | (foreSrcPtr)[64];
		BE_Cross_Wrapped_Inc(screenseg, &destPtr);
		++foreSrcPtr;
		*destPtr = ((*backSrcPtr  ) & (*foreSrcPtr))
		                       | (foreSrcPtr)[64];
#if 0
		*(id0_longword_t *)destPtr = ((*(id0_longword_t *)backSrcPtr) & (*(id0_longword_t *)foreSrcPtr))
		                       | ((id0_longword_t *)foreSrcPtr)[16];
#endif
	}

	//BE_ST_MarkGfxForPendingUpdate();
}
#endif



#if GRMODE == EGAGR
//===========================================================================
//
// EGA refresh routines
//
//===========================================================================

#define CACHETILES 1 // enable master screen tile caching
extern id0_unsigned_t tilecache[NUMTILE16];

#define TILEWIDTH 2

//=================
//
// RFL_NewTile
//
// Draws a composit two plane tile to the master screen and sets the update
// spot to 1 in both update pages, forcing the tile to be copied to the
// view pages the next two refreshes
//
// Called to draw newlly scrolled on strips and animating tiles
//
// Assumes write mode 0
//
//=================

extern id0_byte_t *updatestart[2];

void RFL_NewTile (id0_unsigned_t updateoffset)
{
	/*
	 * mark both update lists at this spot
	 */
	updatestart[0][updateoffset] = 1; // page 0 pointer
	updatestart[1][updateoffset] = 1; // page 1 pointer
	// offset in map from origin
	id0_unsigned_t currmapoffset = updatemapofs[updateoffset]+originmap;
	// screen location for tile
	id0_unsigned_t currtileoffset = blockstarts[updateoffset]+masterofs;
	// set screenstartcs to the location in screenseg to draw the tile
	screenstartcs = currtileoffset;
	// If either of the tile numbers is 0xFFFF, the tile does not need to
	// be masked together; as one of the planes totally eclipses the other
	id0_unsigned_t foretilenum = mapsegs[1][currmapoffset/2];
	id0_unsigned_t backtilenum = mapsegs[0][currmapoffset/2];
	if (!foretilenum)
	{
		//=========
		//
		// No foreground tile, so draw a single background tile.
		// Use the master screen cache if possible
		//
		//=========
#if CACHETILES
		if (tilecache[backtilenum])
		{
			//=============
			//
			// Draw single tile from cache
			//
			//=============
			id0_unsigned_t egaSrcOff = tilecache[backtilenum];
			id0_unsigned_t egaDestOff = screenstartcs;
			for (int loopVar = 15; loopVar; --loopVar)
			{
				BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
				BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
				egaSrcOff += SCREENWIDTH-2;
				egaDestOff += SCREENWIDTH-2;
			}
			BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff++, egaSrcOff++);
			BE_ST_EGAUpdateGFXByteScrToScr(egaDestOff, egaSrcOff);

			return;
		}
#endif
		//=============
		//
		// Draw single tile from main memory
		//
		//=============
		tilecache[backtilenum] = screenstartcs; // next time it can be drawn from here with latch
		const id0_byte_t *backSrcPtr = (id0_byte_t *)grsegs[STARTTILE16+backtilenum];
		backSrcPtr = backSrcPtr ? backSrcPtr : g_be_cross_dosZeroSeg; // VANILLA KEEN BUG WORKAROUND ("Empty" tile found in map)
		for (int planeCounter = 4, mapMask = 1; planeCounter; --planeCounter, mapMask <<= 1) // draw four planes
		{
			id0_unsigned_t egaDestOff = screenstartcs; // start at same place in all planes
			for (int loopVar = 15; loopVar; --loopVar)
			{
				BE_ST_EGAUpdateGFXBuffer(egaDestOff, backSrcPtr, 2, mapMask);
				backSrcPtr += 2;
				egaDestOff += SCREENWIDTH;
			}
			BE_ST_EGAUpdateGFXBuffer(egaDestOff, backSrcPtr, 2, mapMask);
			backSrcPtr += 2;
		}

		return;
	}

	//=========
	//
	// Draw a masked tile combo
	// Interupts are disabled and the stack segment is reassigned
	//
	//=========
	const id0_byte_t *backSrcPtr = (id0_byte_t *)grsegs[STARTTILE16+backtilenum];
	backSrcPtr = backSrcPtr ? backSrcPtr : g_be_cross_dosZeroSeg; // VANILLA KEEN BUG WORKAROUND ("Empty" tile found in map)
	id0_unsigned_t egaDestOff = screenstartcs;
	for (int planeCounter = 4, mapMask = 1, dataLoc = 32; planeCounter; --planeCounter, mapMask <<= 1, dataLoc += 32)
	{
		const id0_byte_t *foreSrcPtr = (id0_byte_t *)grsegs[STARTTILE16M+foretilenum];
		foreSrcPtr = foreSrcPtr ? foreSrcPtr : g_be_cross_dosZeroSeg; // VANILLA KEEN BUG WORKAROUND ("Empty" tile found in map)
		for (id0_unsigned_t loopVar = 0, lineoffset = 0; loopVar < 16; ++loopVar, lineoffset += SCREENWIDTH)
		{
			// backSrcPtr - background tile
			// foreSrcPtr - mask
			// &foreSrcPtr[dataLoc] - masked data
			BE_ST_EGAUpdateGFXByte(egaDestOff+lineoffset, ((*backSrcPtr) & (*foreSrcPtr)) | foreSrcPtr[dataLoc], mapMask);
			++backSrcPtr;
			++foreSrcPtr;
			BE_ST_EGAUpdateGFXByte(egaDestOff+lineoffset+1, ((*backSrcPtr) & (*foreSrcPtr)) | foreSrcPtr[dataLoc], mapMask);
			++backSrcPtr;
			++foreSrcPtr;
		}
	}
}

#endif

#if GRMODE == VGAGR
;============================================================================
;
; VGA refresh routines
;
;============================================================================


#endif


//============================================================================
//
// reasonably common refresh routines
//
//============================================================================


//=================
//
// RFL_UpdateTiles
//
// Scans through the update matrix pointed to by updateptr, looking for 1s.
// A 1 represents a tile that needs to be copied from the master screen to the
// current screen (a new row or an animated tiled).  If more than one adjacent
// tile in a horizontal row needs to be copied, they will be copied as a group.
//
// Assumes write mode 1
//
//=================

void RFL_UpdateTiles (void)
{
	id0_byte_t *scanPtr = updateptr;
	id0_byte_t *scanEndPtr = updateptr + (TILESWIDE+1)*TILESHIGH+1;
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

		if (scanPtr == scanEndPtr)
		{
			//BE_ST_MarkGfxForPendingUpdate();
			return; // Nothing left
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
#if (GRMODE == CGAGR)
			id0_byte_t *destPtr = &screenseg[(id0_unsigned_t)(tileLoc+bufferofs)]; // dest in current screen
			id0_byte_t *srcPtr = &screenseg[(id0_unsigned_t)(tileLoc+masterofs)]; // source in master screen
			for (int loopVar = 15; loopVar; --loopVar)
			{
				BE_Cross_WrappedToWrapped_MemCopy(screenseg, destPtr, srcPtr, TILEWIDTH);
				BE_Cross_Wrapped_Add(screenseg, &srcPtr, SCREENWIDTH);
				BE_Cross_Wrapped_Add(screenseg, &destPtr, SCREENWIDTH);
			}
			BE_Cross_WrappedToWrapped_MemCopy(screenseg, destPtr, srcPtr, TILEWIDTH);
#endif
#if (GRMODE == EGAGR)
			id0_word_t egaDestOff = tileLoc+bufferofs; // dest in current screen
			id0_word_t egaSrcOff = tileLoc+masterofs; // source in master screen
			for (int loopVar = 15; loopVar; --loopVar)
			{
				BE_ST_EGAUpdateGFXBufferScrToScr(egaDestOff, egaSrcOff, TILEWIDTH);
				egaSrcOff += SCREENWIDTH;
				egaDestOff += SCREENWIDTH;
			}
			BE_ST_EGAUpdateGFXBufferScrToScr(egaDestOff, egaSrcOff, TILEWIDTH);
#endif
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
#if (GRMODE == CGAGR)
		id0_byte_t *destPtr = &screenseg[(id0_unsigned_t)(tileLoc+bufferofs)]; // dest in current screen
		id0_byte_t *srcPtr = &screenseg[(id0_unsigned_t)(tileLoc+masterofs)]; // source in master screen
		id0_word_t bytesToSkip = SCREENWIDTH-2*bytesPerRow; // words wide in CGA tiles
#else
#if (GRMODE == EGAGR)
		id0_word_t egaDestOff = tileLoc+bufferofs; // dest in current screen
		id0_word_t egaSrcOff = tileLoc+masterofs; // source in master screen
#endif
		id0_word_t bytesToSkip = SCREENWIDTH-bytesPerRow;
#endif
		for (int loopVar = 15; loopVar; --loopVar)
		{
			iterationsToDo = bytesPerRow;
#if (GRMODE == CGAGR)
			for (; iterationsToDo; --iterationsToDo)
			{
				// Was originally a single instrument, so not calling BE_Cross_LinearToWrapped_MemCopy
				memcpy(destPtr, srcPtr, TILEWIDTH/2);
				BE_Cross_Wrapped_Add(screenseg, &srcPtr, TILEWIDTH/2);
				BE_Cross_Wrapped_Add(screenseg, &destPtr, TILEWIDTH/2);
			}
			BE_Cross_Wrapped_Add(screenseg, &srcPtr, bytesToSkip);
			BE_Cross_Wrapped_Add(screenseg, &destPtr, bytesToSkip);
#endif
#if (GRMODE == EGAGR)
			BE_ST_EGAUpdateGFXBufferScrToScr(egaDestOff, egaSrcOff, iterationsToDo);
			iterationsToDo = 0;
			egaSrcOff += bytesToSkip+bytesPerRow;
			egaDestOff += bytesToSkip+bytesPerRow;
#endif
		}
		iterationsToDo = bytesPerRow;
#if (GRMODE == CGAGR)
		for (; iterationsToDo; --iterationsToDo)
		{
			// Was originally a single instrument, so not calling BE_Cross_LinearToWrapped_MemCopy
			memcpy(destPtr, srcPtr, TILEWIDTH/2);
			BE_Cross_Wrapped_Add(screenseg, &srcPtr, TILEWIDTH/2);
			BE_Cross_Wrapped_Add(screenseg, &destPtr, TILEWIDTH/2);
		}
#endif
#if (GRMODE == EGAGR)
		BE_ST_EGAUpdateGFXBufferScrToScr(egaDestOff, egaSrcOff, iterationsToDo);
		iterationsToDo = 0;
#endif
		// was 0, now 0xFFFF for above loop
		// WARNING: This should be UNSIGNED, or else we get undefined behaviors
		--iterationsToDo;
	} while (true);
}

//============================================================================


//=================
//
// RFL_MaskForegroundTiles
//
// Scan through update looking for 3's.  If the foreground tile there is a
// masked foreground tile, draw it to the screen
//
//=================

void RFL_MaskForegroundTiles (void)
{
	id0_byte_t *scanPtr = updateptr;
	id0_byte_t *scanEndPtr = updateptr + (TILESWIDE+1)*TILESHIGH+2;
	id0_word_t iterationsToDo = 0xFFFF; // definitely scan the entire thing
	do
	{
		/*
		 * scan for a 3 in the updates list
		 */
		while (iterationsToDo)
		{
			--iterationsToDo;
			if (*(scanPtr++) == 3)
			{
				break;
			}
		};
		if (scanPtr == scanEndPtr)
		{
			//BE_ST_MarkGfxForPendingUpdate();
			return; // Nothing left
		}

		//============
		//
		// found a tile, see if it needs to be masked on
		//
		//============

		id0_word_t offsettedoriginmap = updatemapofs[scanPtr-updateptr-1] + originmap;
		id0_word_t foretilenum = mapsegs[1][offsettedoriginmap/2];
		if (!foretilenum) // 0 = no foreground tile
		{
			continue;
		}
		if (!(tinf[foretilenum+INTILE] & 0x80)) // high bit = masked tile
		{
			continue;
		}

#if (GRMODE == CGAGR)
		//=================
		//
		// mask the tile CGA
		//
		//=================

		id0_word_t tileLoc = blockstarts[scanPtr-updateptr-1];
		id0_byte_t *destPtr = &screenseg[(id0_unsigned_t)(tileLoc + bufferofs)];
		const id0_byte_t *srcPtr = (const id0_byte_t *)grsegs[STARTTILE16M+foretilenum];

		for (int loopVar = 16; loopVar; --loopVar)
		{
			// destPtr - background tile
			// srcPtr - mask
			// &srcPtr[64] - masked data
			*destPtr = ((*destPtr) & (*srcPtr))
				               | srcPtr[64];
			BE_Cross_Wrapped_Inc(screenseg, &destPtr);
			++srcPtr;
			*destPtr = ((*destPtr) & (*srcPtr))
				               | srcPtr[64];
			BE_Cross_Wrapped_Inc(screenseg, &destPtr);
			++srcPtr;
			*destPtr = ((*destPtr) & (*srcPtr))
				               | srcPtr[64];
			BE_Cross_Wrapped_Inc(screenseg, &destPtr);
			++srcPtr;
			*destPtr   = ((*destPtr) & (*srcPtr))
				               | srcPtr[64];

			++srcPtr;
			BE_Cross_Wrapped_Add(screenseg, &destPtr, SCREENWIDTH-3);
#if 0
			*((id0_longword_t *)destPtr) =
				((*((id0_longword_t *)destPtr)) // background
				 & (*(id0_longword_t *)srcPtr) // mask
				) | (*(id0_longword_t *)(srcPtr+64)); // masked data
#endif
		}
#endif

#if (GRMODE == EGAGR)
		//=================
		//
		// mask the tile
		//
		//=================

		planemask = 1;
		planenum = 0;

		id0_word_t tileLoc = blockstarts[scanPtr-updateptr-1];
		screenstartcs = (id0_unsigned_t)(tileLoc + bufferofs);

		id0_unsigned_t dataLoc = 32; // data starts 32 bytes after mask

		do // plane loop
		{
			const id0_byte_t *srcPtr = (id0_byte_t *)grsegs[STARTTILE16M+foretilenum];
			id0_unsigned_t egaDestOff = screenstartcs;
			for (int loopVar = 0; loopVar < 16; ++loopVar, egaDestOff += SCREENWIDTH-1)
			{
				BE_ST_EGAUpdateGFXByte(egaDestOff, (BE_ST_EGAFetchGFXByte(egaDestOff, planenum) & (*srcPtr)) | srcPtr[dataLoc], planemask);
				++srcPtr;
				++egaDestOff;
				BE_ST_EGAUpdateGFXByte(egaDestOff, (BE_ST_EGAFetchGFXByte(egaDestOff, planenum) & (*srcPtr)) | srcPtr[dataLoc], planemask);
				++srcPtr;
				//++egaDestOff;
			}
			dataLoc += 32; // the mask is now further away

			++planenum;
			planemask <<= 1; // shift plane mask over for next plane
		} while (planemask != 0x10);
#endif

		iterationsToDo = 0xFFFF; // definitely scan the entire thing
	} while (true);
}
