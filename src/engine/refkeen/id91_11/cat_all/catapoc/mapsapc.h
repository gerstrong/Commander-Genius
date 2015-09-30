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

///////////////////////////////////////
//
// TED5 Map Header for APC
//
///////////////////////////////////////

//
// Map Names
//
typedef enum {
		EGYPT_CEMETERY_MAP,      // 0
		CEMETERY_EGYPT_MAP,      // 1
		WIZARDS_ROOM_MAP,        // 2
		AXIS_TIME_LORDS_MAP,     // 3
		FUTURE_NODE_MAP,         // 4
		FIRE_NODE_MAP,           // 5
		ANCIENT_NODE_MAP,        // 6
		WATER_NODE_MAP,          // 7
		BIG_WIZARD_MAZE_MAP,     // 8
		RAMB_MANTA_MAP,          // 9
		FLOODED_CITY_MAP,        // 10
		COMPUTER_CORE_MAP,       // 11
		BIG_SPACE_MAP_MAP,       // 12
		ENCOUNTER_MAP,           // 13
		NEMESIS_FINAL_MAP,       // 14
		PASSAGE_HOME_MAP,        // 15
		EGYPT_1_MAP,             // 16
		WIZARD_GARDEN_MAP,       // 17
		LASTMAP
	     } mapnames;

//
// TILEINFO offsets
//
#define ANIM		402
#define FLAGS		(ANIM+NUMTILE16)
