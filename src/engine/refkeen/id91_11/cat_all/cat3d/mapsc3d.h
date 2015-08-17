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

///////////////////////////////////////
//
// TED5 Map Header for C3D
//
///////////////////////////////////////

//
// Map Names
//
typedef enum {
		APPROACH_MAP,            // 0
		ENTRANCE_MAP,            // 1
		GROUND_FLOOR_MAP,        // 2
		SECOND_FLOOR_MAP,        // 3
		THIRD_FLOOR_MAP,         // 4
		TOWER_1_MAP,             // 5
		TOWER_2_MAP,             // 6
		SECRET_HALLS_MAP,        // 7
		ACCESS_FLOOR_MAP,        // 8
		DUNGEON_MAP,             // 9
		LOWER_DUNGEON_MAP,       // 10
		CATACOMB_MAP,            // 11
		LOWER_REACHES_MAP,       // 12
		WARRENS_MAP,             // 13
		HIDDEN_CAVERNS_MAP,      // 14
		FENSOFINSANITY_MAP,      // 15
		CHAOSCORRIDORS_MAP,      // 16
		LABYRINTH_MAP,           // 17
		HALLS_OF_BLOOD_MAP,      // 18
		NEMESISSLAIR_MAP,        // 19
		LASTMAP
	     } mapnames;

//
// TILEINFO offsets
//
#define ANIM		402
