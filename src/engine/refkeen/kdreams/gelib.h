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

#include "SL_FILE.h"
#include "id_mm.h"

///////////////////////////////////////////////////////////////////////////
//
// Defines
//

struct BitMapHeader {
	id0_unsigned_int_t	w,h,x,y;
	id0_unsigned_char_t	d,trans,comp,pad;
} __attribute__((__packed__));

// (REFKEEN) Seems unused
#if 0
struct BitMap {
	id0_unsigned_t id0_int_t Width;
	id0_unsigned_t id0_int_t Height;
	id0_unsigned_t id0_int_t Depth;
	id0_unsigned_t id0_int_t BytesPerRow;
	id0_char_t far *Planes[8];
};
#endif

struct Shape {
	memptr Data;
	id0_long_t size;
	id0_unsigned_t id0_int_t BPR;
	struct BitMapHeader bmHdr;
};

void FreeShape(struct Shape *shape);
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty);
