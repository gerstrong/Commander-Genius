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

// KD_ACT1.C
#include "kd_def.h"
//#pragma hdrstop


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


#define PLACESPRITE RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum, \
	spritedraw,0);

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

id0_int_t	flowertime[4] = {700,700,350,175};

/*
=============================================================================

						MISC ACTOR STUFF

=============================================================================
*/

/*
==================
=
= DoGravity
=
= Changes speed and location
=
==================
*/


void DoGravity (objtype *ob)
{
	id0_long_t	i;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -ACCGRAVITY)
			{
			// stop at apex of jump
				ob->ymove += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed+=ACCGRAVITY;
			if (ob->yspeed>SPDMAXY)
			  ob->yspeed=SPDMAXY;
		}
		ob->ymove+=ob->yspeed;
	}
}


/*
===============
=
= AccelerateX
=
===============
*/

void AccelerateX (objtype *ob,id0_int_t dir,id0_int_t max)
{
	id0_long_t	i;
	id0_unsigned_t	olddir;

	olddir = ob->xspeed & 0x8000;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			ob->xspeed+=dir;
			if ( (ob->xspeed & 0x8000) != olddir)
			{
				olddir = ob->xspeed & 0x8000;
				ob->xdir = olddir ? -1 : 1;
			}
			if (ob->xspeed>max)
			  ob->xspeed=max;
			else if (ob->xspeed<-max)
			  ob->xspeed=-max;
		}
		ob->xmove+=ob->xspeed;
	}
}


/*
===============
=
= FrictionX
=
===============
*/

void FrictionX (objtype *ob)
{
	id0_long_t	i;
	id0_int_t		dir;
	id0_unsigned_t	olddir;

	olddir = ob->xspeed & 0x8000;

	if (ob->xspeed > 0)
		dir = -1;
	else if (ob->xspeed < 0)
		dir = 1;
	else
		dir = 0;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			ob->xspeed+=dir;
			if ( (ob->xspeed & 0x8000) != olddir)
				ob->xspeed = 0;
		}
		ob->xmove+=ob->xspeed;
	}
}


/*
===============
=
= ProjectileThink
=
===============
*/

void ProjectileThink (objtype *ob)
{
	DoGravity (ob);
	ob->xmove = tics*ob->xspeed;
}

/*
===============
=
= VelocityThink
=
===============
*/

void VelocityThink (objtype *ob)
{
	ob->xmove = tics*ob->xspeed;
	ob->ymove = tics*ob->yspeed;
}

/*
===============
=
= DrawReact
=
===============
*/

void DrawReact (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,1);
}

void DrawReact2 (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,2);
}

void DrawReact3 (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,3);
}

/*
==================
=
= ChangeState
=
==================
*/

void ChangeState (objtype *ob, statetype *state)
{
	ob->state = state;
	ob->ticcount = 0;
	if (state->rightshapenum)
	{
		if (ob->xdir>0)
			ob->shapenum = state->rightshapenum;
		else
			ob->shapenum = state->leftshapenum;
	}

	ob->xmove = 0;
	ob->ymove = 0;

	ob->needtoreact = true;			// it will need to be redrawn this frame
	ClipToWalls (ob);
}


/*
====================
=
= WalkReact
=
====================
*/

void WalkReact (objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= 2*ob->xmove;
		ob->y -= ob->ymove;

		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}

	PLACESPRITE;
}


/*
=============================================================================

							DOOR

=============================================================================
*/

void	DoorContact (objtype *ob, objtype *hit);

extern	statetype s_door;
extern	statetype s_doorraise;

statetype s_door  	 = {DOORSPR,DOORSPR,think,false,
	false,0, 0,0, NULL, DoorContact, DrawReact, &s_door};
statetype s_doorraise = {DOORSPR,DOORSPR,slide,false,
	false,24, 0,32, NULL, DoorContact, DrawReact, NULL};

/*
======================
=
= SpawnDoor
=
======================
*/

void	SpawnDoor (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = doorobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	newobj->xdir = 1;
	newobj->ydir = -1;
	newobj->needtoclip = false;
	NewState (newobj,&s_door);
}

/*
======================
=
= DoorContact
=
======================
*/

void	DoorContact (objtype *ob, objtype *hit)
{
	ClipToSpriteSide (hit,ob);
}

/*
=============================================================================

							FLOWER

temp1 = original class
temp2 = original state
temp3 = flower count

=============================================================================
*/

void ChangeToFlower (objtype *ob);
void FlowerThink (objtype *ob);
void ChangeFromFlower (objtype *ob);

extern	statetype s_flower1;
extern	statetype s_flower2;
extern	statetype s_flower3;
extern	statetype s_flower4;
extern	statetype s_flower5;
extern	statetype s_flower6;

extern	statetype s_poofto1;
extern	statetype s_poofto2;
extern	statetype s_poofto3;
extern	statetype s_poofto4;

extern	statetype s_pooffrom1;
extern	statetype s_pooffrom2;
extern	statetype s_pooffrom3;
extern	statetype s_pooffrom4;
extern	statetype s_pooffrom5;
extern	statetype s_pooffrom6;
extern	statetype s_pooffrom7;

extern	statetype s_bonus1;

//#pragma warn -sus

statetype s_flower1  	 = {FLOWER1SPR,FLOWER1SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower2};
statetype s_flower2  	 = {FLOWER2SPR,FLOWER2SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower3};
statetype s_flower3  	 = {FLOWER3SPR,FLOWER3SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower4};
statetype s_flower4  	 = {FLOWER4SPR,FLOWER4SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower5};
statetype s_flower5  	 = {FLOWER3SPR,FLOWER3SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower6};
statetype s_flower6  	 = {FLOWER2SPR,FLOWER2SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower1};

statetype s_poofto1	  	 = {POOF1SPR,POOF1SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_poofto2};
statetype s_poofto2	  	 = {POOF2SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_poofto3};
statetype s_poofto3	  	 = {POOF3SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_poofto4};
statetype s_poofto4	  	 = {POOF4SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, NULL};

statetype s_pooffrom1  	 = {POOF4SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_pooffrom2};
statetype s_pooffrom2  	 = {POOF3SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_pooffrom3};
statetype s_pooffrom3  	 = {POOF2SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_pooffrom4};
statetype s_pooffrom4  	 = {POOF1SPR,POOF1SPR,step,false,
	false,20, 0,0, ChangeFromFlower, NULL, DrawReact2, &s_pooffrom5};
statetype s_pooffrom5  	 = {POOF2SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_pooffrom6};
statetype s_pooffrom6  	 = {POOF3SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, &s_pooffrom7};
statetype s_pooffrom7  	 = {POOF4SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact2, NULL};


//#pragma warn +sus

/*
======================
=
= ChangeToFlower
=
======================
*/

void ChangeToFlower (objtype *ob)
{
	SD_PlaySound (FLOWERPOWERSND);
	ob->y = ob->bottom-TILEGLOBAL*2;
	ob->temp1 = (id0_int_t)ob->obclass;
	// (REFKEEN) BACKWARDS COMPATIBILITY
	ob->temp2 = ob->state->compatdospointer;
	ob->temp2stateptr = ob->state;
	//ob->temp2 = ob->state;
	ob->temp3 = 0;
	ob->needtoclip = true;
	ob->obclass = inertobj;
	ob->xspeed = 0;
	ChangeState (ob,&s_flower1);
	ob->active = allways;			// flower never deactivated
	GetNewObj (true);
	newobj->x = ob->x;
	newobj->y = ob->y;
	NewState (newobj,&s_poofto1);
	newobj->active = removable;
}


/*
======================
=
= FlowerThink
=
======================
*/

void FlowerThink (objtype *ob)
{
	ProjectileThink (ob);
	if ( (ob->temp3+=tics) >= flowertime[gamestate.difficulty])
	{
		GetNewObj (true);
		newobj->active = allways;
		newobj->temp1 = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);
		//newobj->temp1 = ob;
		newobj->x = ob->x;
		newobj->y = ob->y;
		NewState (newobj,&s_pooffrom1);
		ob->temp3 = 0;
	}
}


/*
======================
=
= ChangeFromFlower
=
======================
*/

void ChangeFromFlower (objtype *ob)
{
	objtype *flower;
	//statetype *state;
	id0_unsigned_t	oldbottom;

	SD_PlaySound (UNFLOWERPOWERSND);
	flower = COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(ob->temp1);
	//flower = (objtype *)ob->temp1;

	oldbottom = flower->bottom;
	ChangeState (flower,flower->temp2stateptr);
	//ChangeState (flower,(statetype *)flower->temp2);
	flower->y += oldbottom - flower->bottom;

	flower->obclass = (classtype)flower->temp1;
	flower->active = yes;			// allow it to unspawn now if off screen
}


/*
=============================================================================

							BONUS

temp1 = bonus type
temp2 = base shape number
temp3 = last animated shape number +1

=============================================================================
*/

void BonusThink (objtype *ob);

extern	statetype s_bonus1;

//#pragma warn -sus

statetype s_bonus  	 = {0/*NULL*/,0/*NULL*/,step,false,
	false,20, 0,0, BonusThink, NULL, DrawReact2, &s_bonus};

statetype s_bonusrise  	 = {0/*NULL*/,0/*NULL*/,slide,false,
	false,40, 0,8, NULL, NULL, DrawReact3, NULL};

//#pragma warn +sus

/*
====================
=
= SpawnBonus
=
====================
*/

id0_int_t bonusshape[12] = {PEPPERMINT1SPR,COOKIE1SPR,CANDYCANE1SPR,CANDYBAR1SPR,
	LOLLIPOP1SPR,COTTONCANDY1SPR,EXTRAKEEN1SPR,SUPERBONUS1SPR,FLOWERPOWER1SPR,
	FLOWERPOWERUP1SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

void SpawnBonus (id0_int_t tilex, id0_int_t tiley, id0_int_t type)
{
	GetNewObj (false);

	newobj->needtoclip = false;
	newobj->obclass = bonusobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = tiley<<G_T_SHIFT;

	if (type == 9)
		newobj->y -= 8*PIXGLOBAL;	// flower power up one block

	newobj->ydir = -1;			// bonus stuff flies up when touched

	newobj->temp1 = type;
	newobj->temp2 = newobj->shapenum = bonusshape[type];
	if (type != 7)
		newobj->temp3 = newobj->temp2 + 2;
	else
		newobj->temp3 = newobj->temp2 + 4;	// super bonus is 4 stage animation

	NewState (newobj,&s_bonus);
}

/*
====================
=
= BonusThink
=
====================
*/

void BonusThink (objtype *ob)
{
	if (++ob->shapenum == ob->temp3)
		ob->shapenum = ob->temp2;
}



/*
=============================================================================

						BROCCOLASH

=============================================================================
*/

void BroccoThink (objtype *ob);
void BroccoGetUp (objtype *ob);

extern	statetype s_broccowalk1;
extern	statetype s_broccowalk2;
extern	statetype s_broccowalk3;
extern	statetype s_broccowalk4;

extern	statetype s_broccosmash1;
extern	statetype s_broccosmash2;
extern	statetype s_broccosmash3;
extern	statetype s_broccosmash4;
extern	statetype s_broccosmash5;
extern	statetype s_broccosmash6;
extern	statetype s_broccosmash7;
extern	statetype s_broccosmash8;
extern	statetype s_broccosmash9;

//#pragma warn -sus

statetype s_broccowalk1	 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,false,
	true,7, 128,0, BroccoThink, NULL, WalkReact, &s_broccowalk2};
statetype s_broccowalk2	 = {BROCCOLASHRUNL2SPR,BROCCOLASHRUNR2SPR,step,false,
	true,7, 128,0, BroccoThink, NULL, WalkReact, &s_broccowalk3};
statetype s_broccowalk3	 = {BROCCOLASHRUNL3SPR,BROCCOLASHRUNR3SPR,step,false,
	true,7, 128,0, BroccoThink, NULL, WalkReact, &s_broccowalk4};
statetype s_broccowalk4	 = {BROCCOLASHRUNL4SPR,BROCCOLASHRUNR4SPR,step,false,
	true,7, 128,0, BroccoThink, NULL, WalkReact, &s_broccowalk1};

statetype s_broccosmash1 = {BROCCOLASHSMASHL1SPR,BROCCOLASHSMASHR1SPR,step,true,
	false,3, 0,0, NULL, NULL, DrawReact, &s_broccosmash2};
statetype s_broccosmash2 = {BROCCOLASHSMASHL2SPR,BROCCOLASHSMASHR2SPR,step,true,
	false,3, 0,0, NULL, NULL, DrawReact, &s_broccosmash3};
statetype s_broccosmash3 = {BROCCOLASHSMASHL3SPR,BROCCOLASHSMASHR3SPR,step,true,
	false,3, 0,0, NULL, NULL, DrawReact, &s_broccosmash4};
statetype s_broccosmash4 = {BROCCOLASHSMASHL4SPR,BROCCOLASHSMASHR4SPR,step,false,
	false,7, 0,0, NULL, NULL, DrawReact, &s_broccosmash5};
statetype s_broccosmash5 = {BROCCOLASHSMASHL3SPR,BROCCOLASHSMASHR3SPR,step,true,
	false,6, 0,0, NULL, NULL, DrawReact, &s_broccosmash6};
statetype s_broccosmash6 = {BROCCOLASHSMASHL2SPR,BROCCOLASHSMASHR2SPR,step,true,
	false,6, 0,0, NULL, NULL, DrawReact, &s_broccosmash7};
statetype s_broccosmash7 = {BROCCOLASHSMASHL1SPR,BROCCOLASHSMASHR1SPR,step,true,
	false,6, 0,0, NULL, NULL, DrawReact, &s_broccosmash8};
statetype s_broccosmash8 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,true,
	false,6, 0,0, BroccoGetUp, NULL, DrawReact, &s_broccosmash9};
statetype s_broccosmash9 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,true,
	false,6, 128,0, NULL, NULL, WalkReact, &s_broccowalk1};

//#pragma warn +sus

/*
====================
=
= SpawnBrocco
=
====================
*/

void SpawnBrocco (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = broccoobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	newobj->xdir = 1;
	NewState (newobj,&s_broccowalk1);
}

/*
====================
=
= BroccoGetUp
=
====================
*/

void BroccoGetUp (objtype *ob)
{
	ob->needtoclip = true;
}


/*
====================
=
= BroccoThink
=
====================
*/

void BroccoThink (objtype *ob)
{
	id0_int_t delta;

	if (ob->top > player->bottom || ob->bottom < player->top)
		return;

	delta = player->x - ob->x;

	if ( ob->xdir == -1 )
	{
		if (delta < -3*TILEGLOBAL)
			return;
		if (delta > TILEGLOBAL/2)
		{
			ob->xdir = 1;
			return;
		}
		ob->state = &s_broccosmash1;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
	else
	{
		delta = player->left - ob->right;
		if (delta > 3*TILEGLOBAL)
			return;
		if (delta < -TILEGLOBAL/2)
		{
			ob->xdir = -1;
			return;
		}
		ob->state = &s_broccosmash1;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
}


/*
=============================================================================

						 TOMATOOTH

ob->temp1 = jumptime

=============================================================================
*/

#define	SPDTOMATBOUNCE	30
#define TICTOMATJUMP	10
#define SPDTOMAT		16

void TomatBounceThink (objtype *ob);
void TomatReact (objtype *ob);

extern	statetype s_tomatbounce;
extern	statetype s_tomatbounce2;

//#pragma warn -sus

statetype s_tomatbounce	 = {TOMATOOTHL1SPR,TOMATOOTHR1SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, NULL, TomatReact, &s_tomatbounce2};
statetype s_tomatbounce2 = {TOMATOOTHL2SPR,TOMATOOTHR2SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, NULL, TomatReact, &s_tomatbounce};

//#pragma warn +sus

/*
====================
=
= SpawnTomat
=
====================
*/

void SpawnTomat (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = tomatobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = (tiley<<G_T_SHIFT)-1*BLOCKSIZE;
	newobj->xdir = 1;
	NewState (newobj,&s_tomatbounce);
}

/*
====================
=
= TomatBounceThink
=
====================
*/

void TomatBounceThink (objtype *ob)
{
	AccelerateX (ob,ob->x > player->x ? -1 : 1,SPDTOMAT);
	if (ob->xspeed > 0)
		ob->xdir = 1;
	else
		ob->xdir = -1;

	if (ob->temp1)
	{
		if (ob->temp1<tics)
		{
			ob->ymove = ob->yspeed*ob->temp1;
			ob->temp1 = 0;
		}
		else
		{
			ob->ymove = ob->yspeed*tics;
			ob->temp1-=tics;
		}
	}
	else
		DoGravity(ob);

}


/*
====================
=
= TomatReactThink
=
====================
*/

void TomatReact (objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
	{
		ob->xdir = -ob->xdir;
		ob->xspeed = -ob->xspeed;
	}

	if (ob->hitsouth)
	{
		if (ob->tileright >= originxtile
		&& ob->tileleft <= originxtilemax
		&& ob->tiletop >= originytile
		&& ob->tilebottom <= originytilemax)
			SD_PlaySound (BOUNCESND);
		ob->yspeed = -ob->yspeed;
	}

	if (ob->hitnorth)
	{
		if (ob->tileright >= originxtile
		&& ob->tileleft <= originxtilemax
		&& ob->tiletop >= originytile
		&& ob->tilebottom <= originytilemax)
			SD_PlaySound (BOUNCESND);
		ob->yspeed = -SPDTOMATBOUNCE-(US_RndT()>>4);
		ob->temp1 = TICTOMATJUMP;
	}

	PLACESPRITE;
}


/*
=============================================================================

						 CARROT COURIER

=============================================================================
*/

#define	SPDCARROTLEAPX	32
#define SPDCARROTLEAPY  40

void CarrotThink (objtype *ob);
void CarrotReact (objtype *ob);
void CarrotAirReact (objtype *ob);

extern	statetype s_carrotwalk1;
extern	statetype s_carrotwalk2;
extern	statetype s_carrotwalk3;
extern	statetype s_carrotwalk4;

extern	statetype s_carrotleap;

//#pragma warn -sus

statetype s_carrotwalk1	 = {CARROTRUNL1SPR,CARROTRUNR1SPR,step,false,
	true,5, 128,0, NULL, NULL, CarrotReact, &s_carrotwalk2};
statetype s_carrotwalk2	 = {CARROTRUNL2SPR,CARROTRUNR2SPR,step,false,
	true,5, 128,0, NULL, NULL, CarrotReact, &s_carrotwalk3};
statetype s_carrotwalk3	 = {CARROTRUNL3SPR,CARROTRUNR3SPR,step,false,
	true,5, 128,0, NULL, NULL, CarrotReact, &s_carrotwalk4};
statetype s_carrotwalk4	 = {CARROTRUNL4SPR,CARROTRUNR4SPR,step,false,
	true,5, 128,0, NULL, NULL, CarrotReact, &s_carrotwalk1};

statetype s_carrotleap	 = {CARROTLEAPL1SPR,CARROTLEAPR1SPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, CarrotAirReact, NULL};

//#pragma warn +sus

/*
====================
=
= SpawnCarrot
=
====================
*/

void SpawnCarrot (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = carrotobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	newobj->xdir = 1;
	newobj->ydir = 1;
	NewState (newobj,&s_carrotwalk1);
	newobj->hitnorth = 1;
}


/*
====================
=
= CarrotReact
=
====================
*/

void CarrotReact (objtype *ob)
{
	//id0_unsigned_t x, width, bot, id0_far *map;

	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->xdir = -1;
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->xdir = 1;
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->y -= ob->ymove;

		ob->yspeed = -SPDCARROTLEAPY;
		ob->xspeed = SPDCARROTLEAPX*ob->xdir;
		ChangeState (ob,&s_carrotleap);
	}

	PLACESPRITE;
}


/*
====================
=
= CarrotAirReact
=
====================
*/

void CarrotAirReact (objtype *ob)
{
	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
		ChangeState (ob,&s_carrotwalk1);

	PLACESPRITE;
}



/*
=============================================================================

						   ASPARAGUSTO

=============================================================================
*/

void AsparThink (objtype *ob);

extern	statetype s_asparwalk1;
extern	statetype s_asparwalk2;
extern	statetype s_asparwalk3;
extern	statetype s_asparwalk4;

//#pragma warn -sus

statetype s_asparwalk1	 = {ASPARAGUSRUNL1SPR,ASPARAGUSRUNR1SPR,step,true,
	true,3, 100,0, NULL, NULL, WalkReact, &s_asparwalk2};
statetype s_asparwalk2	 = {ASPARAGUSRUNL2SPR,ASPARAGUSRUNR2SPR,step,false,
	true,3, 100,0, NULL, NULL, WalkReact, &s_asparwalk3};
statetype s_asparwalk3	 = {ASPARAGUSRUNL3SPR,ASPARAGUSRUNR3SPR,step,true,
	true,3, 100,0, NULL, NULL, WalkReact, &s_asparwalk4};
statetype s_asparwalk4	 = {ASPARAGUSRUNL4SPR,ASPARAGUSRUNR4SPR,step,false,
	true,3, 100,0, NULL, NULL, WalkReact, &s_asparwalk1};

//#pragma warn +sus

/*
====================
=
= SpawnAspar
=
====================
*/

void SpawnAspar (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = asparobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = tiley<<G_T_SHIFT;
	newobj->xdir = 1;
	NewState (newobj,&s_asparwalk1);
}


/*
=============================================================================

						   SOUR GRAPE

=============================================================================
*/

void GrapeThink (objtype *ob);
void GrapeRiseReact (objtype *ob);
void GrapeFallReact (objtype *ob);

extern	statetype s_grapewait;
extern	statetype s_grapefall;
extern	statetype s_grapesit;
extern	statetype s_graperise;

//#pragma warn -sus

statetype s_grapewait	= {GRAPEONVINESPR,GRAPEONVINESPR,think,false,
	false,0, 0,0, GrapeThink, NULL, DrawReact, NULL};

statetype s_grapefall	= {GRAPEFALLINGSPR,GRAPEFALLINGSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, GrapeFallReact, NULL};

statetype s_grapesit	= {GRAPEONVINESPR,GRAPEONVINESPR,step,false,
	false,30, 0,0, NULL, NULL, DrawReact, &s_graperise};
statetype s_graperise	= {GRAPEONVINESPR,GRAPEONVINESPR,slide,false,
	false,0, 0,-16, NULL, NULL, GrapeRiseReact, NULL};

//#pragma warn +sus

/*
====================
=
= SpawnGrape
=
====================
*/

void SpawnGrape (id0_int_t tilex, id0_int_t tiley)
{
	GetNewObj (false);

	newobj->obclass = grapeobj;
	newobj->x = tilex<<G_T_SHIFT;
	newobj->y = tiley<<G_T_SHIFT;
	newobj->xdir = 1;
	newobj->ydir = 1;
	NewState (newobj,&s_grapewait);
}


/*
====================
=
= GrapeThink
=
====================
*/

void GrapeThink (objtype *ob)
{
	id0_unsigned_t y,starty,endy, id0_far *map;

	if (player->left > ob->right
		|| player->right < ob->left
		|| player->y < ob->y )
		return;

//
// see if there are any walls between grape and player
//
	starty = ob->tilebottom;
	endy = player->tiletop;

	map = mapsegs[1] + mapbwidthtable[starty]/2 + ob->tilemidx;
	for (y = starty ; y<endy ; y++,map+=mapwidth)
		if (tinf[NORTHWALL+*map])
			return;

	ob->state = &s_grapefall;
	SD_PlaySound (GRAPESCREAMSND);

}


/*
====================
=
= GrapeRiseReact
=
====================
*/

void GrapeRiseReact (objtype *ob)
{
	if (ob->hitsouth)
		ChangeState(ob,&s_grapewait);
	PLACESPRITE;
}


/*
====================
=
= GrapeFallReact
=
====================
*/

void GrapeFallReact (objtype *ob)
{
	if (ob->hitnorth)
	{
		SD_PlaySound (BOUNCESND);
		ob->yspeed = -(ob->yspeed<<1)/3;
		if (ob->yspeed > -32)
			ChangeState(ob,&s_grapesit);
	}
	PLACESPRITE;
}

