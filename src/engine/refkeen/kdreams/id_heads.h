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

// ID_GLOB.H


//#include <ALLOC.H>
#include <ctype.h>
//#include <DOS.H>
#include <errno.h>
#include <fcntl.h>
//#include <IO.H>
//#include <MEM.H>
//#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// for lseek and more
#include <sys/types.h>
#include <unistd.h>

#include "refkeen.h"

#define __ID_GLOB__

#define	EXTENSION	"KDR"

#include "graphkdr.h"
#include "audiokdr.h"

#define	TEXTGR	0
#define	CGAGR	1
#define	EGAGR	2
#define	VGAGR	3

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
#define GRMODE	CGAGR
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
#define GRMODE	EGAGR	
#else
#error "Compatible version of Keen to support isn't defined, this shouldn't happen!"
#endif

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
//typedef uint8_t id0_char_t;

typedef int8_t id0_signed_char_t;
typedef uint8_t id0_unsigned_char_t;
typedef int16_t id0_short_t; // Possibly used in kd_main.c and external decompression routines for Keen Dreams
typedef int16_t id0_int_t;
typedef uint16_t id0_unsigned_t;
typedef uint16_t id0_unsigned_int_t; // Found in shareware release v1.13
typedef int32_t id0_long_t;
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

// Initialized before calling vanilla Keen Dreams' (now renamed) main function
extern int id0_argc;
extern const char **id0_argv;

#include "id_mm.h"
#include "id_ca.h"
#include "id_vw.h"
#include "id_rf.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_us.h"
