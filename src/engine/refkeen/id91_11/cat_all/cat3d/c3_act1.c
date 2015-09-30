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

// C3_PLAY.C

#include "c3_def.h"
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


dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
	southwest,south,southeast};



/*
=============================================================================

						  BONUS ITEMS

=============================================================================
*/

extern statetype	s_boltbonus2;
extern statetype	s_nukebonus2;

statetype s_boltbonus = {BOLTOBJPIC,8,NULL,&s_boltbonus2};
statetype s_boltbonus2 = {BOLTOBJ2PIC,8,NULL,&s_boltbonus};

statetype s_nukebonus = {NUKEOBJPIC,8,NULL,&s_nukebonus2};
statetype s_nukebonus2 = {NUKEOBJ2PIC,8,NULL,&s_nukebonus};

statetype s_potionbonus = {POTIONOBJPIC,0,NULL,&s_potionbonus};
statetype s_rkeybonus = {RKEYOBJPIC,0,NULL,&s_rkeybonus};
statetype s_ykeybonus = {YKEYOBJPIC,0,NULL,&s_ykeybonus};
statetype s_gkeybonus = {GKEYOBJPIC,0,NULL,&s_gkeybonus};
statetype s_bkeybonus = {BKEYOBJPIC,0,NULL,&s_bkeybonus};
statetype s_scrollbonus = {SCROLLOBJPIC,0,NULL,&s_scrollbonus};
statetype s_chestbonus = {CHESTOBJPIC,0,NULL,&s_chestbonus};
statetype s_goalbonus = {NEMESISPIC,0,NULL,&s_goalbonus};

/*
===============
=
= SpawnBonus
=
===============
*/

void SpawnBonus (id0_int_t tilex, id0_int_t tiley, id0_int_t number)
{
	statetype *state;

	if (number == B_BOLT)
		state = &s_boltbonus;
	else if (number == B_NUKE)
		state = &s_nukebonus;
	else if (number == B_POTION)
		state = &s_potionbonus;
	else if (number == B_RKEY)
		state = &s_rkeybonus;
	else if (number == B_YKEY)
		state = &s_ykeybonus;
	else if (number == B_GKEY)
		state = &s_gkeybonus;
	else if (number == B_BKEY)
		state = &s_bkeybonus;
	else if (number >= B_SCROLL1 && number <= B_SCROLL8)
		state = &s_scrollbonus;
	else if (number == B_CHEST)
		state = &s_chestbonus;
	else if (number == B_GOAL)
		state = &s_goalbonus;

	SpawnNewObj (tilex,tiley,state,TILEGLOBAL/2);
	newobj->tileobject = true;
	newobj->temp1 = number;
	newobj->obclass = bonusobj;
	newobj->shootable = false;
}


/*
=============================================================================

					  EXPLODING WALL

=============================================================================
*/


void T_WallDie (objtype *ob);

extern	statetype s_walldie1;
extern	statetype s_walldie2;
extern	statetype s_walldie3;
extern	statetype s_walldie4;
extern	statetype s_walldie5;
extern	statetype s_walldie6;

statetype s_walldie1 = {0,20,NULL,&s_walldie2};
statetype s_walldie2 = {0,-1,T_WallDie,&s_walldie3};
statetype s_walldie3 = {0,20,NULL,&s_walldie4};
statetype s_walldie4 = {0,-1,T_WallDie,&s_walldie5};
statetype s_walldie5 = {0,20,NULL,&s_walldie6};
statetype s_walldie6 = {0,-1,T_WallDie,NULL};

/*
================
=
= ExplodeWall
=
================
*/

void ExplodeWall (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj (tilex,tiley,&s_walldie1,0);
	newobj->obclass = inertobj;
	newobj->active = yes/*true*/;
	/*(id0_unsigned_t)actorat[newobj->tilex][newobj->tiley] = */tilemap[newobj->tilex][newobj->tiley] =
	*(mapsegs[0]+farmapylookup[newobj->tiley]+newobj->tilex) = WALLEXP;
	actorat[newobj->tilex][newobj->tiley] = tilemap[newobj->tilex][newobj->tiley];
}


/*
================
=
= T_WallDie
=
================
*/

void T_WallDie (objtype *ob)
{
	id0_unsigned_t tile,other;

	if (++ob->temp1 == 3)
		tile = 0;
	else
		tile = WALLEXP-1 + ob->temp1;

	/*(id0_unsigned_t)actorat[ob->tilex][ob->tiley] = */tilemap[ob->tilex][ob->tiley] =
	*(mapsegs[0]+farmapylookup[ob->tiley]+ob->tilex) = tile;
	actorat[ob->tilex][ob->tiley] = tilemap[ob->tilex][ob->tiley];

	if (ob->temp1 == 1)
	{
	//
	// blow up nearby walls
	//
		other = tilemap[ob->tilex-1][ob->tiley];
		if ((id0_unsigned_t)(other-EXPWALLSTART)<NUMEXPWALLS)
			ExplodeWall (ob->tilex-1,ob->tiley);
		other = tilemap[ob->tilex+1][ob->tiley];
		if ((id0_unsigned_t)(other-EXPWALLSTART)<NUMEXPWALLS)
			ExplodeWall (ob->tilex+1,ob->tiley);
		other = tilemap[ob->tilex][ob->tiley-1];
		if ((id0_unsigned_t)(other-EXPWALLSTART)<NUMEXPWALLS)
			ExplodeWall (ob->tilex,ob->tiley-1);
		other = tilemap[ob->tilex][ob->tiley+1];
		if ((id0_unsigned_t)(other-EXPWALLSTART)<NUMEXPWALLS)
			ExplodeWall (ob->tilex,ob->tiley+1);
	}
}


/*
=============================================================================

						 WARP GATE

=============================================================================
*/

void T_Gate (objtype *ob);

extern	statetype s_gate1;
extern	statetype s_gate2;
extern	statetype s_gate3;
extern	statetype s_gate4;

extern	statetype s_fgate1;
extern	statetype s_fgate2;
extern	statetype s_fgate3;
extern	statetype s_fgate4;

statetype s_gate1 = {WARP1PIC,12,T_Gate,&s_gate2};
statetype s_gate2 = {WARP2PIC,12,T_Gate,&s_gate3};
statetype s_gate3 = {WARP3PIC,12,T_Gate,&s_gate4};
statetype s_gate4 = {WARP4PIC,12,T_Gate,&s_gate1};

statetype s_fgate1 = {WARP1PIC,6,T_Gate,&s_fgate2};
statetype s_fgate2 = {WARP2PIC,6,T_Gate,&s_fgate3};
statetype s_fgate3 = {WARP3PIC,6,T_Gate,&s_fgate4};
statetype s_fgate4 = {WARP4PIC,6,T_Gate,&s_fgate1};

/*
===============
=
= SpawnWarp
=
===============
*/

void SpawnWarp (id0_int_t tilex, id0_int_t tiley, id0_int_t type)
{
	if (type)
		SpawnNewObj (tilex,tiley,&s_fgate1,TILEGLOBAL/3);
	else
		SpawnNewObj (tilex,tiley,&s_gate1,TILEGLOBAL/3);
	newobj->obclass = gateobj;
	newobj->temp1 = type;
}


/*
===============
=
= T_Gate
=
===============
*/

#define STATUSCOLOR	4

void T_Gate (objtype *ob)
{
	id0_int_t	spot;
	objtype *check;
	id0_unsigned_t	temp;

	if (CheckHandAttack (ob) && !playstate)
	{
	//
	// warp
	//
		temp = bufferofs;
		bufferofs = 0;
		VW_Bar (26,4,232,9,STATUSCOLOR);		// clear text description
		bufferofs = temp;
		IN_ClearKeysDown ();
		if (ob->temp1)
		{
		//
		// teleport inside level
		//
			for (check=player->next;check;check=check->next)
				if (check->obclass==gateobj && check->temp1==ob->temp1 &&
					check != ob)
				{
					player->x = check->x;
					player->y = check->y;
					Thrust (player->angle,TILEGLOBAL/2);		// move forwards
					Thrust (player->angle,TILEGLOBAL/2);		// move forwards
					Thrust (player->angle,TILEGLOBAL/2);		// move forwards
					fizzlein=true;
				}
		}
		else
		{
		//
		// teleport out of level
		//
			playstate = ex_warped;
			spot = *(mapsegs[0]+farmapylookup[ob->tiley]+ob->tilex)-NAMESTART;
			if (spot<1)
				gamestate.mapon++;
			else
				gamestate.mapon=spot-1;
			SD_PlaySound(WARPUPSND);
		}
	}
}


/*
=============================================================================

						   TROLLS

=============================================================================
*/

void T_Troll (objtype *ob);

extern	statetype s_trollpause;

extern	statetype s_troll1;
extern	statetype s_troll2;
extern	statetype s_troll3;
extern	statetype s_troll4;

extern	statetype s_trollattack1;
extern	statetype s_trollattack2;
extern	statetype s_trollattack3;

extern	statetype s_trollouch;

extern	statetype s_trolldie1;
extern	statetype s_trolldie2;
extern	statetype s_trolldie3;


statetype s_trollpause = {TROLL1PIC,40,NULL,&s_troll2};

statetype s_troll1 = {TROLL1PIC,13,T_Troll,&s_troll2};
statetype s_troll2 = {TROLL2PIC,13,T_Troll,&s_troll3};
statetype s_troll3 = {TROLL3PIC,13,T_Troll,&s_troll4};
statetype s_troll4 = {TROLL4PIC,13,T_Troll,&s_troll1};

statetype s_trollattack1 = {TROLLATTACK1PIC,20,NULL,&s_trollattack2};
statetype s_trollattack2 = {TROLLATTACK2PIC,10,T_DoDamage,&s_trollattack3};
statetype s_trollattack3 = {TROLLATTACK2PIC,40,NULL,&s_trollpause};

statetype s_trollouch = {TROLLOUCHPIC,8,NULL,&s_troll1};

statetype s_trolldie1 = {TROLLDIE1PIC,8,NULL,&s_trolldie2};
statetype s_trolldie2 = {TROLLDIE2PIC,8,NULL,&s_trolldie3};
statetype s_trolldie3 = {TROLLDIE3PIC,0,NULL,&s_trolldie3};


/*
===============
=
= SpawnTroll
=
===============
*/

void SpawnTroll (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_troll1,40*PIXRADIUS);
	newobj->speed = 2500;
	newobj->obclass = trollobj;
	newobj->shootable = true;
	newobj->hitpoints = 10;
}


/*
===============
=
= T_Troll
=
===============
*/

void T_Troll (objtype *ob)
{
	if (Chase (ob,true))
	{
		ob->state = &s_trollattack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}



/*
=============================================================================

						   ORCS

=============================================================================
*/

void T_Orc (objtype *ob);

extern	statetype s_orcpause;

extern	statetype s_orc1;
extern	statetype s_orc2;
extern	statetype s_orc3;
extern	statetype s_orc4;

extern	statetype s_orcattack1;
extern	statetype s_orcattack2;
extern	statetype s_orcattack3;

extern	statetype s_orcouch;

extern	statetype s_orcdie1;
extern	statetype s_orcdie2;
extern	statetype s_orcdie3;



statetype s_orcpause = {ORC1PIC,40,NULL,&s_orc2};

statetype s_orc1 = {ORC1PIC,20,T_Orc,&s_orc2};
statetype s_orc2 = {ORC2PIC,20,T_Orc,&s_orc3};
statetype s_orc3 = {ORC3PIC,20,T_Orc,&s_orc4};
statetype s_orc4 = {ORC4PIC,20,T_Orc,&s_orc1};

statetype s_orcattack1 = {ORCATTACK1PIC,20,NULL,&s_orcattack2};
statetype s_orcattack2 = {ORCATTACK2PIC,10,T_DoDamage,&s_orcattack3};
statetype s_orcattack3 = {ORCATTACK2PIC,40,NULL,&s_orcpause};

statetype s_orcouch = {ORCOUCHPIC,10,NULL,&s_orc1};

statetype s_orcdie1 = {ORCDIE1PIC,8,NULL,&s_orcdie2};
statetype s_orcdie2 = {ORCDIE2PIC,8,NULL,&s_orcdie3};
statetype s_orcdie3 = {ORCDIE3PIC,0,NULL,&s_orcdie3};


/*
===============
=
= SpawnOrc
=
===============
*/

void SpawnOrc (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_orc1,PIXRADIUS*32);
	newobj->obclass = orcobj;
	newobj->speed = 1536;
	newobj->shootable = true;
	newobj->hitpoints = 3;
}


/*
===============
=
= T_Orc
=
===============
*/

void T_Orc (objtype *ob)
{
	if (Chase (ob,true))
	{
		ob->state = &s_orcattack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}


/*
=============================================================================

						   DEMON

=============================================================================
*/

void T_Demon (objtype *ob);


extern	statetype s_demonpause;

extern	statetype s_demon1;
extern	statetype s_demon2;
extern	statetype s_demon3;
extern	statetype s_demon4;

extern	statetype s_demonattack1;
extern	statetype s_demonattack2;
extern	statetype s_demonattack3;

extern	statetype s_demonouch;

extern	statetype s_demondie1;
extern	statetype s_demondie2;
extern	statetype s_demondie3;


statetype s_demonpause = {DEMON1PIC,40,NULL,&s_demon2};

statetype s_demon1 = {DEMON1PIC,20,T_Demon,&s_demon2};
statetype s_demon2 = {DEMON2PIC,20,T_Demon,&s_demon3};
statetype s_demon3 = {DEMON3PIC,20,T_Demon,&s_demon4};
statetype s_demon4 = {DEMON4PIC,20,T_Demon,&s_demon1};

statetype s_demonattack1 = {DEMONATTACK1PIC,20,NULL,&s_demonattack2};
statetype s_demonattack2 = {DEMONATTACK2PIC,20,T_DoDamage,&s_demonattack3};
statetype s_demonattack3 = {DEMONATTACK3PIC,30,NULL,&s_demonpause};

statetype s_demonouch = {DEMONOUCHPIC,10,NULL,&s_demon1};

statetype s_demondie1 = {DEMONDIE1PIC,20,NULL,&s_demondie2};
statetype s_demondie2 = {DEMONDIE2PIC,20,NULL,&s_demondie3};
statetype s_demondie3 = {DEMONDIE3PIC,0,NULL,&s_demondie3};



/*
===============
=
= SpawnDemon
=
===============
*/

void SpawnDemon (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_demon1,TILEGLOBAL/2);
	newobj->obclass = demonobj;
	newobj->speed = 2048;
	newobj->shootable = true;
	newobj->hitpoints = 50;
}


/*
===============
=
= T_Demon
=
===============
*/

void T_Demon (objtype *ob)
{
	if (Chase (ob,true))
	{
		ob->state = &s_demonattack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}

/*
=============================================================================

							MSHOTS

temp1 = dir

=============================================================================
*/

#define MSHOTDAMAGE	2
#define MSHOTSPEED	10000

void T_Mshot (objtype *ob);


extern	statetype s_mshot1;
extern	statetype s_mshot2;

statetype s_mshot1 = {PSHOT1PIC,8,&T_Mshot,&s_mshot2};
statetype s_mshot2 = {PSHOT2PIC,8,&T_Mshot,&s_mshot1};


/*
===============
=
= T_Mshot
=
===============
*/

void T_Mshot (objtype *ob)
{
	objtype	*check;
	id0_long_t	xmove,ymove/*,speed*/;

	xmove = ymove = 0;

	switch (ob->dir)
	{
	case north:
		ymove = -ob->speed*tics;
		break;
	case east:
		xmove = ob->speed*tics;
		break;
	case south:
		ymove = ob->speed*tics;
		break;
	case west:
		xmove = -ob->speed*tics;
		break;
	}

	ob->x+=xmove;
	ob->y+=ymove;

	CalcBounds (ob);

	ob->tilex = ob->x>>TILESHIFT;
	ob->tiley = ob->y>>TILESHIFT;

	if (tilemap[ob->tilex][ob->tiley])
	{
		SD_PlaySound (SHOOTWALLSND);
		ob->state = NULL;
		return;
	}

//
// check final position for monsters hit
//
	if ( ob->xl <= player->xh
	&& ob->xh >= player->xl
	&& ob->yl <= player->yh
	&& ob->yh >= player->yl)
	{
		TakeDamage (MSHOTDAMAGE*2);
		ob->state = NULL;
		return;
	}

	for (check = player->next; check; check=check->next)
		if (ob->shootable && ob->obclass != mageobj
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			ShootActor (check,MSHOTDAMAGE);
			ob->state = NULL;
			return;
		}
}




/*
=============================================================================

							MAGE

=============================================================================
*/


void T_Mage (objtype *ob);
void T_MageShoot (objtype *ob);

extern	statetype s_magepause;

extern	statetype s_mage1;
extern	statetype s_mage2;

extern	statetype s_mageattack1;
extern	statetype s_mageattack2;
extern	statetype s_mageattack3;

extern	statetype s_mageouch;

extern	statetype s_magedie1;
extern	statetype s_magedie2;


statetype s_magepause = {MAGE1PIC,100,NULL,&s_mage2};

statetype s_mage1 = {MAGE1PIC,20,T_Mage,&s_mage2};
statetype s_mage2 = {MAGE2PIC,20,T_Mage,&s_mage1};

statetype s_mageattack1 = {MAGEATTACKPIC,20,NULL,&s_mageattack2};
statetype s_mageattack2 = {MAGEATTACKPIC,-1,T_MageShoot,&s_mageattack3};
statetype s_mageattack3 = {MAGEATTACKPIC,30,NULL,&s_magepause};

statetype s_mageouch = {MAGEOUCHPIC,10,NULL,&s_mage1};

statetype s_magedie1 = {MAGEDIE1PIC,20,NULL,&s_magedie2};
statetype s_magedie2 = {MAGEDIE2PIC,0,NULL,&s_magedie2};


/*
===============
=
= SpawnMage
=
===============
*/

void SpawnMage (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_mage1,TILEGLOBAL/2);
	newobj->obclass = mageobj;
	newobj->speed = 2048;
	newobj->shootable = true;
	newobj->hitpoints = 5;
}


/*
===============
=
= T_Mage
=
===============
*/

void T_Mage (objtype *ob)
{
	Chase (ob,false);
//
// check for line up with player
//

	if (ob->x-PIXRADIUS*14 < player->xh
	&& ob->x+PIXRADIUS > player->xl)
	{
		ob->temp1 = 1;
		ob->state = &s_mageattack1;
	}
	else if (ob->y-PIXRADIUS*14 < player->yh
	&& ob->y+PIXRADIUS > player->yl)
	{
		ob->temp1 = 0;
		ob->state = &s_mageattack1;
	}
}


/*
===============
=
= T_MageShoot
=
===============
*/

void T_MageShoot (objtype *ob)
{
	SpawnNewObjFrac (ob->x,ob->y,&s_mshot1,PIXRADIUS*14);
	newobj->obclass = mshotobj;
	newobj->speed = MSHOTSPEED;
	if (ob->temp1)
	{
		if (ob->tiley < player->tiley)
			newobj->dir = south;
		else
			newobj->dir = north;
	}
	else
	{
		if (ob->tilex < player->tilex)
			newobj->dir = east;
		else
			newobj->dir = west;
	}
}


/*
=============================================================================

							nemesis

=============================================================================
*/


void T_Nemesis (objtype *ob);
void T_NemesisShoot (objtype *ob);

extern	statetype s_grelpause;

extern	statetype s_grel1;
extern	statetype s_grel2;

extern	statetype s_grelattack1;
extern	statetype s_grelattack2;
extern	statetype s_grelattack3;

extern	statetype s_grelouch;

extern	statetype s_greldie1;
extern	statetype s_greldie2;
extern	statetype s_greldie3;
extern	statetype s_greldie4;
extern	statetype s_greldie5;
extern	statetype s_greldie6;


statetype s_grelpause = {GREL1PIC,50,NULL,&s_grel2};

statetype s_grel1 = {GREL1PIC,20,T_Nemesis,&s_grel2};
statetype s_grel2 = {GREL2PIC,20,T_Nemesis,&s_grel1};

statetype s_grelattack1 = {GRELATTACKPIC,20,NULL,&s_grelattack2};
statetype s_grelattack2 = {GRELATTACKPIC,-1,T_NemesisShoot,&s_grelattack3};
statetype s_grelattack3 = {GRELATTACKPIC,30,NULL,&s_grelpause};

statetype s_grelouch = {GRELHITPIC,6,NULL,&s_grel1};

statetype s_greldie1 = {GRELDIE1PIC,20,NULL,&s_greldie2};
statetype s_greldie2 = {GRELDIE2PIC,20,NULL,&s_greldie3};
statetype s_greldie3 = {GRELDIE3PIC,20,NULL,&s_greldie4};
statetype s_greldie4 = {GRELDIE4PIC,20,NULL,&s_greldie5};
statetype s_greldie5 = {GRELDIE5PIC,20,NULL,&s_greldie6};
statetype s_greldie6 = {GRELDIE6PIC,0,NULL,&s_greldie6};


/*
===============
=
= SpawnNemesis
=
===============
*/

void SpawnNemesis (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_grel1,PIXRADIUS*56);
	newobj->obclass = grelmobj;
	newobj->speed = 2048;
	newobj->shootable = true;
	newobj->hitpoints = 100;
}


/*
===============
=
= T_Nemesis
=
===============
*/

void T_Nemesis (objtype *ob)
{
	Chase (ob,false);
//
// check for line up with player
//
	if (ob->tilex == player->tilex)
	{
		ob->temp1 = 1;
		ob->state = &s_grelattack1;
	}
	else if (ob->tiley == player->tiley)
	{
		ob->temp1 = 0;
		ob->state = &s_grelattack1;
	}
}


/*
===============
=
= T_NemesisShoot
=
===============
*/

void T_NemesisShoot (objtype *ob)
{
	SpawnNewObjFrac (ob->x,ob->y,&s_mshot1,PIXRADIUS*14);
	newobj->obclass = mshotobj;
	newobj->speed = MSHOTSPEED;
	if (ob->temp1)
	{
		if (ob->tiley < player->tiley)
			newobj->dir = south;
		else
			newobj->dir = north;
	}
	else
	{
		if (ob->tilex < player->tilex)
			newobj->dir = east;
		else
			newobj->dir = west;
	}
}


/*
=============================================================================

						   BAT

=============================================================================
*/

void T_Bat (objtype *ob);
void T_BatPast (objtype *ob);

extern	statetype s_bat1;
extern	statetype s_bat2;
extern	statetype s_bat3;
extern	statetype s_bat4;

extern	statetype s_batdie1;
extern	statetype s_batdie2;


statetype s_bat1 = {BAT1PIC,6,T_Bat,&s_bat2};
statetype s_bat2 = {BAT2PIC,6,T_Bat,&s_bat3};
statetype s_bat3 = {BAT3PIC,6,T_Bat,&s_bat4};
statetype s_bat4 = {BAT4PIC,6,T_Bat,&s_bat1};

statetype s_batpast = {BAT4PIC,80,T_BatPast,&s_bat1};

statetype s_batdie1 = {BATDIE1PIC,8,NULL,&s_batdie2};
statetype s_batdie2 = {BATDIE2PIC,8,NULL,NULL};


/*
===============
=
= SpawnBat
=
===============
*/

void SpawnBat (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_bat1,PIXRADIUS*24);
	newobj->obclass =batobj;
	newobj->shootable = true;

	newobj->hitpoints = 1;
	newobj->speed = 2000;
}


/*
==================================
=
= BatChaseThink
=
==================================
*/

void BatChaseThink (objtype *obj)
{
	id0_int_t deltax,deltay;

	deltax=player->tilex - obj->tilex;
	deltay=player->tiley - obj->tiley;

	if (deltax>0)
		deltax = 2;
	else if (deltax<0)
		deltax = 0;
	else deltax = 1;

	if (deltay>0)
		deltay = 2;
	else if (deltay<0)
		deltay = 0;
	else deltay = 1;

	obj->dir = dirtable[deltay*3+deltax];
	if (Walk(obj))
		return;

	obj->dir = dirtable[3+deltax];
	if (Walk(obj))
		return;

	obj->dir = dirtable[deltay*3+1];
	if (Walk(obj))
		return;

	obj->dir = nodir;
}


void BatRunThink (objtype *obj)
{
	id0_int_t deltax,deltay;

	deltax=player->tilex - obj->tilex;
	deltay=player->tiley - obj->tiley;

	if (deltax>=0)
		deltax = 0;
	else
		deltax = 2;

	if (deltay>=0)
		deltay = 0;
	else
		deltay = 2;

	obj->dir = dirtable[deltay*3+deltax];
	if (Walk(obj))
		return;

	obj->dir = dirtable[3+deltax];
	if (Walk(obj))
		return;

	obj->dir = dirtable[deltay*3+1];
	Walk(obj);
}



/*
===============
=
= T_Bat
=
===============
*/

void T_Bat (objtype *ob)
{
	id0_long_t move;
	id0_long_t deltax,deltay,size;

	move = ob->speed*tics;
	size = (id0_long_t)ob->size + player->size + move;


	do
	{
		deltax = ob->x - player->x;
		deltay = ob->y - player->y;

		if (deltax <= size && deltax >= -size
		&& deltay <= size && deltay >= -size && !ob->temp1)
		{
			TakeDamage (4);
			ob->temp1 = 2;
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}

		actorat[ob->tilex][ob->tiley] = 0;	// pick up marker from goal
		if (ob->dir == nodir)
			ob->dir = north;

		ob->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		if (ob->temp1)
		{
			Walk (ob);				// go straight
			if (!--ob->temp1)
			{
				ob->state = &s_batpast;
				ob->ticcount = ob->state->tictime;
			}
		}
		else
			BatChaseThink (ob);		// head towards player

		actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob); // set down a new goal marker
		//actorat[ob->tilex][ob->tiley] = ob;	// set down a new goal marker
	} while (0);	// just once
	CalcBounds (ob);
}


/*
===============
=
= T_BatPast
=
===============
*/

void T_BatPast (objtype *ob)
{
	id0_long_t move;
	//id0_long_t deltax,deltay,size;

	move = ob->speed*tics;

	do
	{
		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}
		actorat[ob->tilex][ob->tiley] = 0;	// pick up marker from goal

		ob->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		BatRunThink (ob);

		actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);	// set down a new goal marker
		//actorat[ob->tilex][ob->tiley] = ob;	// set down a new goal marker
	} while (0);	//(move)
	CalcBounds (ob);
}



/*
=============================================================================

						   BOUNCE

temp2 = set when hit player, reset when hit wall

=============================================================================
*/

#define SPDBOUNCE	4096
#define DMGBOUNCE	10

void T_Bounce (objtype *ob);

extern	statetype s_bounce1;
extern	statetype s_bounce2;


statetype s_bounce1 = {BIGPSHOT1PIC,8,T_Bounce,&s_bounce2};
statetype s_bounce2 = {BIGPSHOT2PIC,8,T_Bounce,&s_bounce1};

/*
===============
=
= SpawnBounce
=
===============
*/

void SpawnBounce (id0_int_t tilex, id0_int_t tiley, id0_boolean_t towest)
{
	SpawnNewObj(tilex,tiley,&s_bounce1,24*PIXRADIUS);
	newobj->obclass = bounceobj;
	if (towest)
		newobj->dir = west;
	else
		newobj->dir = north;
}


/*
===============
=
= T_Bounce
=
===============
*/

void T_Bounce (objtype *ob)
{
	id0_long_t move;
	id0_long_t deltax,deltay,size;

	move = SPDBOUNCE*tics;
	size = (id0_long_t)ob->size + player->size + move;

	while (move)
	{
		deltax = ob->x - player->x;
		deltay = ob->y - player->y;

		if (deltax <= size && deltax >= -size
		&& deltay <= size && deltay >= -size && !ob->temp2)
		{
			ob->temp2 = 1;
			TakeDamage (DMGBOUNCE);
		}

		if (move < ob->distance)
		{
			MoveObj (ob,move);
			break;
		}
		actorat[ob->tilex][ob->tiley] = 0;	// pick up marker from goal

		ob->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		move -= ob->distance;

		//
		// bounce if hit wall
		//
		switch (ob->dir)
		{
		case north:
			if (tilemap[ob->tilex][--ob->tiley])
			{
				ob->dir = south;
				ob->tiley+=2;
				ob->temp2 = 0;
			}
			break;
		case east:
			if (tilemap[++ob->tilex][ob->tiley])
			{
				ob->dir = west;
				ob->tilex-=2;
				ob->temp2 = 0;
			}
			break;
		case south:
			if (tilemap[ob->tilex][++ob->tiley])
			{
				ob->dir = north;
				ob->tiley-=2;
				ob->temp2 = 0;
			}
			break;
		case west:
			if (tilemap[--ob->tilex][ob->tiley])
			{
				ob->dir = east;
				ob->tilex+=2;
				ob->temp2 = 0;
			}
			break;
		}

		ob->distance = TILEGLOBAL;

		actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);	// set down a new goal marker
		//actorat[ob->tilex][ob->tiley] = ob;	// set down a new goal marker
	}
	CalcBounds (ob);
}

