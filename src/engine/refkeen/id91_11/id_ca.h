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

//#define NOMAPS
//#define NOGRAPHICS
//#define NOAUDIO

#define MAPHEADERLINKED
#define GRHEADERLINKED
#define AUDIOHEADERLINKED

#ifdef REFKEEN_VER_CAT3D

#define NUMMAPS		30

#elif defined REFKEEN_VER_CATADVENTURES

#if REFKEEN_VER_CATABYSS
#define NUMMAPS		19
#else
#define NUMMAPS		39
#endif

#endif

#define MAPPLANES	3

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

extern	id0_byte_t		id0_far	grneeded[NUMCHUNKS];
extern	id0_byte_t		ca_levelbit,ca_levelnum;

extern	id0_char_t		*titleptr[8];

extern	BE_FILE_T			profilehandle,debughandle;

//
// hooks for custom cache dialogs
//
extern	void	(*drawcachebox)		(const id0_char_t *title, id0_unsigned_t numcache);
extern	void	(*updatecachebox)	(void);
extern	void	(*finishcachebox)	(void);

//===========================================================================

// just for the score box reshifting

//void CAL_ShiftSprite (id0_unsigned_t segment,id0_unsigned_t source,id0_unsigned_t dest,
//	id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift);
void CAL_ShiftSprite (id0_byte_t *source, id0_byte_t *dest,
	id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift);

//===========================================================================

// (REFKEEN) UNUSED FUNCTIONS
#if 0
void CA_OpenDebug (void);
void CA_CloseDebug (void);
#endif
id0_boolean_t CA_FarRead (BE_FILE_T handle, id0_byte_t id0_far *dest, id0_long_t length);
id0_boolean_t CA_FarWrite (BE_FILE_T handle, id0_byte_t id0_far *source, id0_long_t length);
id0_boolean_t CA_ReadFile (const id0_char_t *filename, memptr *ptr);
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

void CA_SetAllPurge (void);

void CA_ClearMarks (void);
void CA_ClearAllMarks (void);

#define CA_MarkGrChunk(chunk)	grneeded[chunk]|=ca_levelbit

void CA_CacheGrChunk (id0_int_t chunk);
void CA_CacheMap (id0_int_t mapnum);

void CA_CacheMarks (const id0_char_t *title);

