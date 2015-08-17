/* Catacomb Armageddon Source Code
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

/////////////////////////////////////////////////
//
// MUSE Header for .ARM
// Created Mon Jun 08 16:14:17 1992
//
/////////////////////////////////////////////////

#define NUMSOUNDS		35
#define NUMSNDCHUNKS		106

//
// Sound names & indexes
//
typedef enum {
		HITWALLSND,              // 0
		WARPUPSND,               // 1
		WARPDOWNSND,             // 2
		GETBOLTSND,              // 3
		GETNUKESND,              // 4
		GETPOTIONSND,            // 5
		GETKEYSND,               // 6
		GETSCROLLSND,            // 7
		GETPOINTSSND,            // 8
		USEBOLTSND,              // 9
		USENUKESND,              // 10
		USEPOTIONSND,            // 11
		USEKEYSND,               // 12
		NOITEMSND,               // 13
		WALK1SND,                // 14
		WALK2SND,                // 15
		TAKEDAMAGESND,           // 16
		MONSTERMISSSND,          // 17
		GAMEOVERSND,             // 18
		SHOOTSND,                // 19
		BIGSHOOTSND,             // 20
		SHOOTWALLSND,            // 21
		SHOOTMONSTERSND,         // 22
		TAKEDMGHURTSND,          // 23
		BALLBOUNCESND,           // 24
		NOWAYSND,                // 25
		WARPSND,                 // 26
		HIT_GATESND,             // 27
		GETGEMSND,               // 28
		BOOMSND,                 // 29
		GRELM_DEADSND,           // 30
		FREEZETIMESND,           // 31
		TIMERETURNSND,           // 32
		TICKSND,                 // 33
		BODY_EXPLODESND,         // 34
		LASTSOUND
	     } soundnames;

//
// Base offsets
//
#define STARTPCSOUNDS		0
#define STARTADLIBSOUNDS	35
#define STARTDIGISOUNDS		70
#define STARTMUSIC		105

//
// Music names & indexes
//
typedef enum {
		TOOHOT_MUS,              // 0
		LASTMUSIC
	     } musicnames;

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
