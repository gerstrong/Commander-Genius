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

///////////////////////////////////////
//
// TED5 Map Header for ABS
//
///////////////////////////////////////

//
// Map Names
//
typedef enum {
		TOWNE_CEMETARY_MAP,      // 0
		GARDEN_OF_TEARS_MAP,     // 1
		DEN_OF_ZOMBIES_MAP,      // 2
		MAUSOLEUM_GROUN_MAP,     // 3
		MAUSOLEUM_MAIN_MAP,      // 4
		MIKES_LEVEL_MAP,         // 5
		CRYPT_OF_UNDEAD_MAP,     // 6
		SUBVAULT_1_MAP,          // 7
		AQUADUCT_MAP,            // 8
		ORC_MINES_MAP,           // 9
		TROLLS_LAIR_MAP,         // 10
		DEMON_INFERNO_MAP,       // 11
		BATTLEGROUND_MAP,        // 12
		COVEN_OF_MAGES_MAP,      // 13
		INNER_SANCTUM_MAP,       // 14
		NEMESIS_HAUNT_MAP,       // 15
		PASSAGE_TO_SURF_MAP,     // 16
		BIG_JIMS_DOMAIN_MAP,     // 17
		NOLANS_NASTY_MAP,        // 18
		TILE_TESTER_3_MAP,       // 19
		TILE_TESTER_1_MAP,       // 20
		TILE_TESTER_2_MAP,       // 21
		GOOD_MAP_TO_USE_MAP,     // 22
		LASTMAP
	     } mapnames;

//
// TILEINFO offsets
//
#define ANIM		402
#define FLAGS		(ANIM+NUMTILE16)
