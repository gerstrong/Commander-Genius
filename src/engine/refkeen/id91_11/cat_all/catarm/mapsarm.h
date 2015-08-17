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

///////////////////////////////////////
//
// TED5 Map Header for ARM
//
///////////////////////////////////////

//
// Map Names
//
typedef enum {
		TOWN_MORBIDITY_MAP,      // 0
		DARK_FOREST_MAP,         // 1
		GARDEN_OF_SOULS_MAP,     // 2
		LOST_CITY_DAMND_MAP,     // 3
		TEMPLE_OF_VIPER_MAP,     // 4
		TORTURE_CHAMBER_MAP,     // 5
		DEMONS_HOLD_MAP,         // 6
		COLONY_FIRE_ANT_MAP,     // 7
		HALL_WRETCH_POX_MAP,     // 8
		LAIR_OF_SUCUBUS_MAP,     // 9
		BLOOD_CHAMB_EYE_MAP,     // 10
		FLAMING_INFERNO_MAP,     // 11
		SUBTERR_RIVER_MAP,       // 12
		CRYSTAL_MAZE_MG_MAP,     // 13
		RAMPARTS_OF_NEM_MAP,     // 14
		FORTRESS_OF_NEM_MAP,     // 15
		PASSAGE_TO_SURF_MAP,     // 16
		LASTMAP
	     } mapnames;

//
// TILEINFO offsets
//
#define ANIM		402
#define FLAGS		(ANIM+NUMTILE16)
