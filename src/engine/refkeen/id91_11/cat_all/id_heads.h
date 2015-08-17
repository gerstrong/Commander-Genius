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

// ID_GLOB.H


//#include <ALLOC.H>
#include <ctype.h>
//#include <DOS.H>
#include <errno.h>
#include <fcntl.h>
//#include <IO.H>
//#include <MEM.H>
//#include <PROCESS.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// for lseek and more
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "refkeen.h"

#define __ID_GLOB__

//--------------------------------------------------------------------------

// REFKEEN Use EXTENSION instead of EXT also in the Catacomb Adventures Series,
// for simpler maintenance (and header is unified for all four Catacomb games)
#ifdef REFKEEN_VER_CAT3D
#define EXTENSION "C3D"
#elif defined REFKEEN_VER_CATABYSS
#define EXTENSION "ABS"
#elif defined REFKEEN_VER_CATARM
#define EXTENSION "ARM"
#elif defined REFKEEN_VER_CATAPOC
#define EXTENSION "APC"
#endif
//#define	EXT	"ABS"

// REFKEEN moved to bottom (where id0_char_t and more are defined)
// - also modified type for different platforms
//extern	id0_char_t id0_far introscn;

#ifdef REFKEEN_VER_CAT3D
#include "gfxe_c3d.h"
#include "audioc3d.h"
#elif defined REFKEEN_VER_CATABYSS
#include "gfxe_abs.h"
#include "audioabs.h"
#include "mapsabs.h"
#elif defined REFKEEN_VER_CATARM
#include "gfxe_arm.h"
#include "audioarm.h"
#include "mapsarm.h"
#elif defined REFKEEN_VER_CATAPOC
#include "gfxe_apc.h"
#include "audioapc.h"
#include "mapsapc.h"
#endif

//--------------------------------------------------------------------------

#ifdef REFKEEN_VER_CATADVENTURES

//
// DEFINES THE TILE ATTRIBUTE CHECKING CONVENTION (macros).
//
// DEFINE CUSTOM BIT-FLAG NAMES...
//


#define tf_SOLID							0x01
#define tf_SPECIAL						0x02
#define tf_EMBEDDED_KEY_COLOR			0x04
//
#ifndef REFKEEN_VER_CATABYSS
#define tf_INVISIBLE_WALL			0x09
#define tf_MARKED                	0x80

#define ANIM_FLAGS(tile)	(tinf[ANIM+(tile)])
#endif
//
#define TILE_FLAGS(tile)   (tinf[FLAGS+(tile)])

#define GATE_KEY_COLOR(tile)		((id0_unsigned_char_t)(TILE_FLAGS(tile)>>4))

#endif // REFKEEN_VER_CATADVENTURES

#define CAT3D // REFKEEN: Yes, it's also defined for The Adventures (but unused)

#define	TEXTGR	0
#define	CGAGR	1
#define	EGAGR	2
#define	VGAGR	3

#ifdef REFKEEN_VER_CATADVENTURES
#define  EGA320GR	10					// MDM (GAMERS EDGE)
#define  EGA640GR	11					// MDM (GAMERS EDGE)
#endif

#define GRMODE	EGAGR

#if GRMODE == EGAGR
#define GREXT	"EGA"
#endif
#if GRMODE == CGAGR
#define GREXT	"CGA"
#endif

//#define PROFILE

//
//	ID Engine
//	Types.h - Generic types, #defines, etc.
//	v1.0d1
//

#ifndef	__TYPES__
#define	__TYPES__

#include <stdint.h>
#include <stdbool.h>

typedef	bool id0_boolean_t;

// WARNING:
//
// Should be UNSIGNED for VWL_MeasureString (high scores table) in Keen Dreams
// but SIGNED in Catacomb Abyss for which casting is done in VWL_MeasureString
// (and Keen Dreams' loadscn2)
//
// BUT, to reduce the amount of compilation warnings in C and errors in C++,
// we still use plain char here, and fix Keen Dreams' VWL_MeasureString instead
typedef char id0_char_t;
//typedef int8_t id0_char_t;

typedef int8_t id0_signed_char_t;
typedef uint8_t id0_unsigned_char_t;
// Surprise! c5_game.c has this (could simply used signed char but eh...)
typedef id0_signed_char_t id0_char_signed_t;
typedef int16_t id0_short_t; // Possibly used in kd_main.c and external decompression routines for Keen Dreams
typedef int16_t id0_int_t;
typedef uint16_t id0_unsigned_t;
typedef uint16_t id0_unsigned_int_t; // Found in shareware release v1.13
typedef int32_t id0_long_t;
typedef int32_t id0_signed_long_t;
typedef uint32_t id0_unsigned_long_t;

typedef uint8_t id0_byte_t;
typedef uint16_t id0_word_t;
typedef uint32_t id0_longword_t;
// REFKEEN - Used just in Keen Dreams' SD_StartMusic, but there's no music
// in the original game and it was commented that Ptr shouldn't be used...
//typedef uint8_t * id0_ptr_t;

typedef	struct
		{
			id0_int_t	x,y;
		} Point;
typedef	struct
		{
			Point	ul,lr;
		} Rect;

#ifdef __cplusplus
#define	id0_nil_t	NULL
#else
#define	id0_nil_t	((void *)0)
#endif

// TODO (REFKEEN): These should really be removed, but just for now and to document...
#define id0_far
#define id0_huge
#define id0_seg

#endif

// (REFKEEN) VANILLA BUG REPRODUCTION: In the original code, a call to
// VW_DrawPic on startup leaves the map mask value at 8 (intensity plane),
// so numbers aren't drawn in the following call to RedrawStatusWindow.
// We add a workaround here since we don't store EGA write/read mode
// related values internally (we almost don't need these).
extern bool id0_workaround_catadventures_nodraw_digits_on_startup;

// Initialized before calling vanilla app's (now renamed) main function
extern int id0_argc;
extern const char **id0_argv;

extern id0_char_t *introscn;

#include "id_mm.h"
#include "id_ca.h"
#include "id_vw.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_us.h"


#ifdef REFKEEN_VER_CATADVENTURES
void	Quit (const id0_char_t *error, ...);		// defined in user program
#else
void	Quit (const id0_char_t *error);
#endif

//
// replacing refresh manager with custom routines
//

#define	PORTTILESWIDE		21      // all drawing takes place inside a
#define	PORTTILESHIGH		14		// non displayed port of this size

#define UPDATEWIDE			(PORTTILESWIDE+1)
#define UPDATEHIGH			PORTTILESHIGH

#define	MAXTICS				6
#define DEMOTICS			3

#define	UPDATETERMINATE	0x0301

// REFKEEN - realtics isn't declared in vanilla Catacomb 3-D,
// but again it's just a declaration so we may keep it here
extern	id0_unsigned_t	mapwidth,mapheight,tics,realtics;
extern	id0_boolean_t		compatability;

extern	id0_byte_t		*updateptr;
extern	id0_unsigned_t	uwidthtable[UPDATEHIGH];
extern	id0_unsigned_t	blockstarts[UPDATEWIDE*UPDATEHIGH];
