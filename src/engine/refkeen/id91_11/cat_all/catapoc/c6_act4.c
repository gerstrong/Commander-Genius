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

// C4_PLAY.C

#include "def.h"
//#pragma hdrstop


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


//-------------------------------------------------------------------------
//
//				MISC OBJECTS
//
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
//		COLUMN, SULPHUR GAS HOLE, FIRE POT, FOUNTAIN
//-------------------------------------------------------------------------


void SpawnMiscObjects(id0_int_t tilex, id0_int_t tiley, id0_int_t num);

statetype s_column1 = {COLUMN1PIC, 20, NULL, &s_column1};
statetype s_column2 = {COLUMN2PIC, 20, NULL, &s_column2};
statetype s_column3 = {COLUMN3PIC, 20, NULL, &s_column3};
statetype s_column4 = {COLUMN4PIC, 20, NULL, &s_column4};
statetype s_column5 = {COLUMN5PIC, 20, NULL, &s_column5};
statetype s_ffire_pot = {FFIRE_POTPIC, 20, NULL, &s_ffire_pot};
statetype s_ofire_pot1 = {OFIRE_POT1PIC, 20, NULL, &s_ofire_pot2};
statetype s_ofire_pot2 = {OFIRE_POT2PIC, 20, NULL, &s_ofire_pot1};
statetype s_tomb1 = {TOMB1PIC, 20, NULL, &s_tomb1};
statetype s_tomb2 = {TOMB2PIC, 20, NULL, &s_tomb2};
void SpawnMiscObjects(id0_int_t tilex, id0_int_t tiley, id0_int_t num)
{
	statetype	*objstate;

	switch (num)
	{
		case 1:
			objstate = &s_column1;
		break;

		case 2:
			objstate = &s_column2;
		break;

		case 3:
			objstate = &s_column3;
		break;

		case 4:
			objstate = &s_ffire_pot;
		break;

		case 5:
			objstate = &s_column4;
		break;

		case 6:
			objstate = &s_ofire_pot1;
		break;

		case 7:
			objstate = &s_tomb1;
		break;

		case 8:
			objstate = &s_tomb2;
		break;

		case 9:
			objstate = &s_column5;
		break;
	}

	SpawnNewObj(tilex, tiley, objstate, PIXRADIUS*10);
	newobj->obclass = realsolidobj;
	newobj->flags |= of_shootable;
}



//------------------------------------------------------------------------
//				FORCE FIELD
//------------------------------------------------------------------------

void SpawnForceField(id0_int_t tilex, id0_int_t tiley);
void T_ForceField(objtype *ob);
void T_ForceFieldRemove(objtype *ob);

statetype s_force_field_1 = {FORCE_FIELD_1PIC, 10, T_ForceField, &s_force_field_2};
statetype s_force_field_2 = {FORCE_FIELD_2PIC, 10, T_ForceField, &s_force_field_3};
statetype s_force_field_3 = {FORCE_FIELD_3PIC, 10, T_ForceField, &s_force_field_4};
statetype s_force_field_4 = {FORCE_FIELD_4PIC, 10, T_ForceField, &s_force_field_1};

statetype s_force_field_die = {0,0,T_ForceFieldRemove,&s_force_field_die1};
statetype s_force_field_die1 = {0,0,NULL,NULL};

void SpawnForceField(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_force_field_1,PIXRADIUS*35);
	newobj->obclass = solidobj;
	newobj->hitpoints = EasyHitPoints(20);
	newobj->temp1 = 0;
	newobj->flags |= of_forcefield;		//sets bit 7 :: makes it nonsolid, but also detectable
						//		without adding another object type!
	newobj->flags |= of_shootable;
}

void T_ForceField(objtype *ob)
{
	id0_long_t /*move,*/deltax,deltay,size;

	size = (id0_long_t)ob->size + player->size;

	deltax = ob->x - player->x;
	deltay = ob->y - player->y;

	if (deltax <= size && deltax >= -size
	&& deltay <= size && deltay >= -size)
		if (!newobj->temp1)
		{
			TakeDamage (94);
			newobj->temp1 = 1;
			return;
		}
		else
			return;
	newobj->temp1 = 0;
}

void T_ForceFieldRemove(objtype *ob)
{
	actorat[ob->tilex][ob->tiley] = 0;
}




//-------------------------------------------------------------------------
//
//								INVISIBLE WALL CONTROLLER
//
//-------------------------------------------------------------------------

void SpawnInvisWallCntroller(id0_int_t x, id0_int_t y);
void T_InvisWall(objtype *ob);

extern statetype s_invis_wall_control;

statetype s_invis_wall_control = {0, 10, T_InvisWall, &s_invis_wall_control};

void SpawnInvisWallCntroller(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_invis_wall_control,PIXRADIUS*35);
	newobj->obclass = solidobj;
	newobj->flags &= ~of_shootable;
	newobj->temp1 = tilemap[tilex][tiley];		// Number for the wall tile here
														// Used for replacing the wall tile
}

void T_InvisWall(objtype *ob)
{
	id0_long_t /*move,*/deltax,deltay,size;

	size = (id0_long_t)ob->size + player->size;

	deltax = ob->x - player->x;
	deltay = ob->y - player->y;

	if ((deltax <= size && deltax >= -size
		&& deltay <= size && deltay >= -size) ||
		(ob->tilex == player->tilex) && (ob->tiley == player->tiley))
	{
	  // Get rid of the wall tile if you are on it
		tilemap[ob->tilex][ob->tiley] = 0;
	}
	else
	{
	  // Replace wall tile
		tilemap[ob->tilex][ob->tiley] = ob->temp1;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	EasyHitPoints
//
//	Checks to see if the player has selected the easy mode for playing.
//	If so then the normal hit points are cut in half.
//	This is when the object is spawned.
//
//	Parms
//		NrmHitPts - the normal hit points
//
//	Returns
//		Half of NrmHitPts
//
/////////////////////////////////////////////////////////////////////////////

id0_int_t EasyHitPoints(id0_int_t NrmHitPts)
{
	if (EASYMODEON)				// Wimpy, Wimpy, Wimpy!!!!!
	{
		return(NrmHitPts/4);
	}
	else
		return(NrmHitPts);
}

/////////////////////////////////////////////////////////////////////////////
//
//	EasyDoDamage
//
//	Checks to see if the player has selected the easy mode for playing.
//	If so then the normal amount of damage is cut in half.
//	This is called each time a monster does damage.
//
//	Parms
//		Damage - the normal damage taken
//
//	Returns
//		Half of Damage
//
/////////////////////////////////////////////////////////////////////////////

id0_int_t EasyDoDamage(id0_int_t Damage)
{
	if (EASYMODEON)				// Wimpy, Wimpy, Wimpy!!!!!
		return(Damage/2);
	else
		return(Damage);
}

