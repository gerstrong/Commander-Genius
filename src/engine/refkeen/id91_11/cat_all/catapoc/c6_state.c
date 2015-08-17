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

// C3_STATE.C

#include "def.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


dirtype opposite[9] =
	{south,west,north,east,southwest,northwest,northeast,southeast,nodir};



//===========================================================================


/*
===================
=
= Internal_SpawnNewObj
=
===================
*/
void Internal_SpawnNewObj (id0_unsigned_t x, id0_unsigned_t y, statetype *state, id0_unsigned_t size, id0_boolean_t UseDummy, id0_boolean_t PutInActorat)
{
	extern objtype dummyobj;

	GetNewObj(UseDummy);
	newobj->size = size;
	newobj->state = state;
	newobj->ticcount = BE_Cross_Brandom (state->tictime)+1;

	newobj->tilex = x;
	newobj->tiley = y;
	newobj->x = ((id0_long_t)x<<TILESHIFT)+TILEGLOBAL/2;
	newobj->y = ((id0_long_t)y<<TILESHIFT)+TILEGLOBAL/2;
	CalcBounds(newobj);
	newobj->dir = nodir;
	newobj->active = noalways;

	if (newobj != &dummyobj && PutInActorat)
		actorat[newobj->tilex][newobj->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(newobj);
		//actorat[newobj->tilex][newobj->tiley] = newobj;
}

void Internal_SpawnNewObjFrac (id0_long_t x, id0_long_t y, statetype *state, id0_unsigned_t size,id0_boolean_t UseDummy)
{
	GetNewObj(UseDummy);
	newobj->size = size;
	newobj->state = state;
	newobj->ticcount = BE_Cross_Brandom (state->tictime)+1;
	newobj->active = noalways;

	newobj->x = x;
	newobj->y = y;
	newobj->tilex = x>>TILESHIFT;
	newobj->tiley = y>>TILESHIFT;
	CalcBounds(newobj);
	newobj->distance = 100;
	newobj->dir = nodir;
}




/*
===================
=
= CheckHandAttack
=
= If the object can move next to the player, it will return true
=
===================
*/

id0_boolean_t CheckHandAttack (objtype *ob)
{
	id0_long_t deltax,deltay,size;

	size = (id0_long_t)ob->size + player->size + ob->speed*tics + SIZE_TEST;
	deltax = ob->x - player->x;
	deltay = ob->y - player->y;

	if (deltax > size || deltax < -size || deltay > size || deltay < -size)
		return false;

	return true;
}


/*
===================
=
= T_DoDamage
=
= Attacks the player if still nearby, then immediately changes to next state
=
===================
*/

void T_DoDamage (objtype *ob)
{
	id0_int_t	points;


	if (CheckHandAttack(ob) && (!(ob->flags & of_damagedone)))
	{
		points = 0;

		switch (ob->obclass)
		{
		case aquamanobj:
			points = 7;
		break;

		case wizardobj:
			points = 7;
		break;

		case trollobj:
			points = 10;
		break;

		case invisdudeobj:
			points = 10;
		break;

		case demonobj:
		case cyborgdemonobj:
			points = 15;
		break;

		}
		points = EasyDoDamage(points);
		TakeDamage (points);
		ob->flags |= of_damagedone;
	}
}


//==========================================================================

/*
==================================
=
= Walk
=
==================================
*/

id0_boolean_t Walk (objtype *ob)
{
	switch (ob->dir)
	{
	case north:
		if (actorat[ob->tilex][ob->tiley-1])
			return false;
		ob->tiley--;
		ob->distance = TILEGLOBAL;
		return true;

	case northeast:
		if (actorat[ob->tilex+1][ob->tiley-1])
			return false;
		ob->tilex++;
		ob->tiley--;
		ob->distance = TILEGLOBAL;
		return true;

	case east:
		if (actorat[ob->tilex+1][ob->tiley])
			return false;
		ob->tilex++;
		ob->distance = TILEGLOBAL;
		return true;

	case southeast:
		if (actorat[ob->tilex+1][ob->tiley+1])
			return false;
		ob->tilex++;
		ob->tiley++;
		ob->distance = TILEGLOBAL;
		return true;

	case south:
		if (actorat[ob->tilex][ob->tiley+1])
			return false;
		ob->tiley++;
		ob->distance = TILEGLOBAL;
		return true;

	case southwest:
		if (actorat[ob->tilex-1][ob->tiley+1])
			return false;
		ob->tilex--;
		ob->tiley++;
		ob->distance = TILEGLOBAL;
		return true;

	case west:
		if (actorat[ob->tilex-1][ob->tiley])
			return false;
		ob->tilex--;
		ob->distance = TILEGLOBAL;
		return true;

	case northwest:
		if (actorat[ob->tilex-1][ob->tiley-1])
			return false;
		ob->tilex--;
		ob->tiley--;
		ob->distance = TILEGLOBAL;
		return true;

	case nodir:
		return false;
	}

	Quit ("Walk: Bad dir");
	return false;
}



/*
==================================
=
= ChaseThink
= have the current monster go after the player,
= either diagonally or straight on
=
==================================
*/

void ChaseThink (objtype *obj, id0_boolean_t diagonal)
{
	id0_int_t deltax,deltay/*,i*/;
	dirtype d[3];
	// (REFKEEN) Incrementing/Decrementing an enum is a bad idea (leading to undefined behaviors in C, including "Bad dir" bug reproduced),
	// and illegal in C++. Hence, tdir is redefined to be a (signed) int here. Casts are done (to be compatible with C++).
	int tdir;
	dirtype /*tdir, */olddir, turnaround;


	olddir=obj->dir;
	turnaround=opposite[olddir];

	deltax=player->tilex - obj->tilex;
	deltay=player->tiley - obj->tiley;

	d[1]=nodir;
	d[2]=nodir;

	if (deltax>0)
		d[1]= east;
	if (deltax<0)
		d[1]= west;
	if (deltay>0)
		d[2]=south;
	if (deltay<0)
		d[2]=north;

	if (abs(deltay)>abs(deltax))
	{
		tdir=(int)d[1];
		d[1]=d[2];
		d[2]=(dirtype)tdir;
	}

	if (d[1]==turnaround)
		d[1]=nodir;
	if (d[2]==turnaround)
		d[2]=nodir;


	if (diagonal)
	{                           /*ramdiagonals try the best dir first*/
		if (d[1]!=nodir)
		{
			obj->dir=d[1];
			if (Walk(obj))
				return;     /*either moved forward or attacked*/
		}

		if (d[2]!=nodir)
		{
			obj->dir=d[2];
			if (Walk(obj))
				return;
		}
	}
	else
	{                  /*ramstraights try the second best dir first*/

		if (d[2]!=nodir)
		{
			obj->dir=d[2];
			if (Walk(obj))
				return;
		}

		if (d[1]!=nodir)
		{
			obj->dir=d[1];
			if (Walk(obj))
				return;
		}
	}

	// Kluge to make the running eye stay in place if blocked, ie, not divert
	// from path
	if (obj->obclass == reyeobj)
		return;


/* there is no direct path to the player, so pick another direction */

	obj->dir=olddir;
	if (Walk(obj))
		return;

	if (US_RndT()>128) 	/*randomly determine direction of search*/
	{
		for (tdir=north;tdir<=west;tdir++)
		{
			if (tdir!=(int)turnaround)
			{
				obj->dir=(dirtype)tdir;
				if (Walk(obj))
					return;
			}
		}
	}
	else
	{
		for (tdir=west;tdir>=north;tdir--)
		{
			if (tdir!=(int)turnaround)
			{
			  obj->dir=(dirtype)tdir;
			  if (Walk(obj))
				return;
			}
		}
	}

	obj->dir=turnaround;
	Walk(obj);		/*last chance, don't worry about returned value*/
}


/*
=================
=
= MoveObj
=
=================
*/

void MoveObj (objtype *ob, id0_long_t move)
{
	ob->distance -=move;

	switch (ob->dir)
	{
	case north:
		ob->y -= move;
		return;
	case northeast:
		ob->x += move;
		ob->y -= move;
		return;
	case east:
		ob->x += move;
		return;
	case southeast:
		ob->x += move;
		ob->y += move;
		return;
	case south:
		ob->y += move;
		return;
	case southwest:
		ob->x -= move;
		ob->y += move;
		return;
	case west:
		ob->x -= move;
		return;
	case northwest:
		ob->x -= move;
		ob->y -= move;
		return;

	case nodir:
		return;
	}
}


/*
=================
=
= Chase
=
= returns true if hand attack range
=
=================
*/

id0_boolean_t Chase (objtype *ob, id0_boolean_t diagonal)
{
	id0_long_t move;
	id0_long_t deltax,deltay,size;

	ob->flags &= ~of_damagedone;

	move = ob->speed*tics;
	size = (id0_long_t)ob->size + player->size + move + SIZE_TEST;

	while (move)
	{
		deltax = ob->x - player->x;
		deltay = ob->y - player->y;

		if (deltax <= size && deltax >= -size
		&& deltay <= size && deltay >= -size)
		{
			CalcBounds (ob);
			return true;
		}

		if (move < ob->distance)		//ob->distance - distance before you move
		{                             //               over into next tile
			MoveObj (ob,move);
			break;
		}
		else
			if (ob->obclass == reyeobj)	// Kludge for the "running eye"
			{
				if (ob->temp1 < 2)
				{
					MoveObj(ob, ob->distance/2);
					ob->temp1 = 0;
				}
			}

		actorat[ob->tilex][ob->tiley] = 0;	// pick up marker from goal
		if (ob->dir == nodir)
			ob->dir = north;

		ob->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		ChaseThink (ob, diagonal);
		if (!ob->distance)
			break;			// no possible move
		actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);	// set down a new goal marker
		//actorat[ob->tilex][ob->tiley] = ob;	// set down a new goal marker
	}
	CalcBounds (ob);
	return false;
}

//===========================================================================


/*
===================
=
= ShootActor
=
===================
*/

void ShootActor (objtype *ob, id0_unsigned_t damage)
{

	ob->hitpoints -= damage;

	if (ob->hitpoints<=0)
	{
		switch (ob->obclass)
		{

		case headobj:
			ob->state = &s_pshot_exp1;
			ob->obclass = expobj;
			ob->ticcount = ob->state->tictime;
			SpawnBigExplosion(ob->x,ob->y,12,(16l<<16L));
		break;

		case aquamanobj:
			ob->state = &s_aqua_die1;
			ob->temp1 = 10;
		break;

		case wizardobj:
			ob->state = &s_wizard_die1;
		break;

		case trollobj:
			ob->state = &s_trolldie1;
		break;

		case blobobj:
			ob->state = &s_blob_die1;
		break;

		case rayobj:
			ob->state = &s_ray_die1;
		break;

		case ramboneobj:
			ob->state = &s_skel_die1;
		break;

		case fmageobj:
			ob->state = &s_fmagedie1;
		break;

		case robotankobj:
			ob->state = &s_robotank_death1;
			ob->temp1 = 10;
		break;

		case stompyobj:
			ob->state = &s_stompy_death1;
		break;

		case bugobj:
			ob->state = &s_bug_death1;
		break;

		case demonobj:
			ob->state = &s_demondie1;
		break;

		case cyborgdemonobj:
			ob->state = &s_cyborg_demondie1;
		break;

		case invisdudeobj:
			ob->state = &s_invis_death1;
		break;

		case grelmobj:
			ob->state = &s_greldie1;
		break;

		case eyeobj:
			ob->state = &s_eye_die1;
		break;

		case reyeobj:
			ob->state = &s_reye_die1;
		break;

		case bounceobj:
			ob->state = &s_pshot_exp1;
			ob->obclass = expobj;
			ob->ticcount = ob->state->tictime;
			SpawnBigExplosion(ob->x,ob->y,12,(16l<<16L));
		break;

		case rshotobj:
		case eshotobj:
		case wshotobj:
		case hshotobj:
		case bshotobj:
		case rbshotobj:
		case fmshotobj:
		case rtshotobj:
		case syshotobj:
		case bgshotobj:
			ob->state = &s_bonus_die;
#if USE_INERT_LIST
			ob->obclass = solidobj;		// don't add these objs to inert list
#endif
		break;

		case bonusobj:
		case freezeobj:
			switch (ob->temp1)
			{
				case B_POTION:
				case B_OLDCHEST:
				case B_CHEST:
				case B_NUKE:
				case B_BOLT:
					ob->state = &s_pshot_exp1;
					ob->obclass = expobj;
					ob->ticcount = ob->state->tictime;
					SpawnBigExplosion(ob->x,ob->y,12,(16l<<16L));
					bordertime = FLASHTICS<<2;
					bcolor = 14;
					VW_ColorBorder(14 | 56);
					DisplaySMsg("Item destroyed", NULL);
					status_flag  = S_NONE;
					status_delay = 80;
				break;
			}
#if USE_INERT_LIST
			ob->obclass = solidobj;		// don't add this obj to inert list
#endif
		break;
		}

		if (ob->obclass != solidobj && ob->obclass != realsolidobj)
		{
			ob->obclass = inertobj;
			ob->flags &= ~of_shootable;
			actorat[ob->tilex][ob->tiley] = 0;
			//actorat[ob->tilex][ob->tiley] = NULL;
#if USE_INERT_LIST
			MoveObjToInert(ob);
#endif
		}
		else
		{
			if (ob->flags & of_forcefield)
			{
				ob->state = &s_force_field_die;
				ob->flags &= ~of_shootable;
			}
		}
	}
	else
	{
		switch (ob->obclass)
		{
		case wizardobj:
			ob->state = &s_wizard_ouch;
		break;

		case trollobj:
			if (!BE_Cross_Brandom(5))
				ob->state = &s_trollouch;
			else
				return;
		break;

		case blobobj:
			ob->state = &s_blob_ouch;
		break;

		case ramboneobj:
			ob->state = &s_skel_ouch;
		break;

		case fmageobj:
			ob->state = &s_fmageouch;
		break;

		case stompyobj:
			ob->state = &s_stompy_ouch;
		break;

		case bugobj:
			ob->state = &s_bug_ouch;
		break;

		case cyborgdemonobj:
			if (!(BE_Cross_Brandom(8)))
				ob->state = &s_cyborg_demonouch;
			else
				return;
		break;

		case demonobj:
			if (!(BE_Cross_Brandom(8)))
				ob->state = &s_demonouch;
			else
				return;
		break;

		case invisdudeobj:
			ob->state = &s_invis_fizz1;
		break;

		case grelmobj:
			ob->state = &s_grelouch;
		break;

		case eyeobj:
			ob->state = &s_eye_ouch;
		break;

		case reyeobj:
			ob->state = &s_reye_ouch;
		break;
		}
	}

	ob->ticcount = ob->state->tictime;
}


