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

// ID_CA.H

#ifndef __TYPES__
#include "id_types.h"
#endif

#ifndef __ID_MM__
#include "id_mm.h"
#endif

#ifndef __ID_GLOB__
#include "id_glob.h"
#endif

#define __ID_CA__

//===========================================================================

#define MAPHEADERLINKED
#define GRHEADERLINKED
#define AUDIOHEADERLINKED

#define NUMMAPS	30

//
// tile info defines, as bytes after tinf the table starts
//


#define	SPEED	502
#define ANIM	(SPEED+NUMTILE16)

#define NORTHWALL	(ANIM+NUMTILE16)
#define EASTWALL	(NORTHWALL+NUMTILE16M)
#define SOUTHWALL   (EASTWALL+NUMTILE16M)
#define WESTWALL    (SOUTHWALL+NUMTILE16M)
#define MANIM       (WESTWALL+NUMTILE16M)
#define INTILE      (MANIM+NUMTILE16M)
#define MSPEED      (INTILE+NUMTILE16M)

//===========================================================================

typedef	struct
{
	id0_long_t		planestart[3];
	id0_unsigned_t	planelength[3];
	id0_unsigned_t	width,height;
	id0_char_t		name[16];
} __attribute__((__packed__)) maptype;

//===========================================================================

extern	id0_byte_t 		id0_seg	*tinf;
extern	id0_int_t			mapon;

extern	id0_unsigned_t	id0_seg	*mapsegs[3];
extern	maptype		id0_seg	*mapheaderseg[NUMMAPS];
extern	id0_byte_t		id0_seg	*audiosegs[NUMSNDCHUNKS];
extern	void		id0_seg	*grsegs[NUMCHUNKS];

extern	id0_byte_t		grneeded[NUMCHUNKS];
extern	id0_byte_t		ca_levelbit,ca_levelnum;

extern	const id0_char_t		*titleptr[8];

extern	BE_FILE_T			profilehandle;

//===========================================================================

// just for the score box reshifting

//void CAL_ShiftSprite (id0_unsigned_t segment,id0_unsigned_t source,id0_unsigned_t dest,
//	id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift);
void CAL_ShiftSprite (id0_byte_t *source, id0_byte_t *dest,
	id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift);

//===========================================================================

id0_boolean_t CA_FarRead (BE_FILE_T handle, id0_byte_t id0_far *dest, id0_long_t length);
id0_boolean_t CA_FarWrite (BE_FILE_T handle, id0_byte_t id0_far *source, id0_long_t length);
id0_boolean_t CA_LoadFile (const id0_char_t *filename, memptr *ptr);

id0_long_t CA_RLEWCompress (id0_unsigned_t id0_huge *source, id0_long_t length, id0_unsigned_t id0_huge *dest,
  id0_unsigned_t rlewtag);

void CA_RLEWexpand (id0_unsigned_t id0_huge *source, id0_unsigned_t id0_huge *dest,id0_long_t length,
  id0_unsigned_t rlewtag);

void CA_Startup (void);
void CA_Shutdown (void);

void CA_CacheAudioChunk (id0_int_t chunk);
void CA_LoadAllSounds (void);

void CA_UpLevel (void);
void CA_DownLevel (void);

void CA_ClearMarks (void);
void CA_ClearAllMarks (void);

#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (id0_int_t chunk);
void CA_CacheMap (id0_int_t mapnum);

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
void CA_CacheMarks (const id0_char_t *title);
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
void CA_CacheMarks (const id0_char_t *title, id0_boolean_t cachedownlevel);
#endif

