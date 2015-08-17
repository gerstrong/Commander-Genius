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

#ifndef __HEADS_H__
#define __HEADS_H__

// Taken off ID_HEADS.H

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

#ifdef REFKEEN_VER_CATABYSS
#define EXTENSION "ABS"
#elif defined REFKEEN_VER_CATARM
#define EXTENSION "ARM"
#elif defined REFKEEN_VER_CATAPOC
#define EXTENSION "APC"
#endif

#define GRMODE EGAGR

//
//	ID Engine
//	Types.h - Generic types, #defines, etc.
//	v1.0d1
//

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

// TODO (REFKEEN): These should really be removed, but just for now and to document...
#define id0_far
#define id0_huge
#define id0_seg

// Initialized before calling vanilla app's (now renamed) main function
extern int id0_argc;
extern const char **id0_argv;

// WARNING: THIS IS *DIFFERENT* FROM THE ID_MM.H DEFINITION!!!
// ... in 16-bit code (far vs segment pointer).

typedef void id0_far * memptr;

// Taken off ID_VW.H

#define VIRTUALHEIGHT 300

#define CRTC_INDEX 0x3D4
#define CRTC_OFFSET 19

typedef enum {NOcard,MDAcard,CGAcard,EGAcard,MCGAcard,VGAcard,
              HGCcard=0x80,HGCPcard,HICcard} cardtype;

extern id0_unsigned_t displayofs; // origin of port on visable screen
extern id0_unsigned_t ylookup[VIRTUALHEIGHT];

void TrashProg (const id0_char_t *OutMsg, ...);
// CA
id0_boolean_t LoadFile (const id0_char_t *filename, memptr *ptr);
// VW
cardtype VideoID (void);
void SetScreen (id0_unsigned_t CRTC, id0_unsigned_t pelpan);
void ScreenToScreen(id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t wide, id0_unsigned_t height);

#endif
