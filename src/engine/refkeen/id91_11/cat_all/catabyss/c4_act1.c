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

// C3_PLAY.C

#include "def.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/
void SpawnSkeleton(id0_int_t tilex, id0_int_t tiley);


#define MSHOTDAMAGE	2
#define MSHOTSPEED	10000

#define ESHOTDAMAGE	1
#define ESHOTSPEED	5000

#define RANDOM_ATTACK 20

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

// REFKEEN - Use classtype instead of short for obclass
id0_boolean_t ShootPlayer (objtype *ob, classtype/*id0_short_t*/ obclass, id0_short_t speed, statetype *state);
void T_ShootPlayer(objtype *ob);

id0_short_t zombie_base_delay;

id0_short_t other_x[] = {0,39,39,0},
		other_y[] = {0,0,27,27};


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
extern statetype	s_boltbonus3;
extern statetype	s_nukebonus3;

statetype s_boltbonus = {BOLTOBJPIC,8,NULL,&s_boltbonus2};
statetype s_boltbonus2 = {BOLT2OBJPIC,8,NULL,&s_boltbonus3};
statetype s_boltbonus3 = {BOLT3OBJPIC,8,NULL,&s_boltbonus};

statetype s_nukebonus = {NUKEOBJPIC,8,NULL,&s_nukebonus2};
statetype s_nukebonus2 = {NUKE2OBJPIC,8,NULL,&s_nukebonus3};
statetype s_nukebonus3 = {NUKE3OBJPIC,8,NULL,&s_nukebonus};

statetype s_potionbonus = {POTIONOBJPIC,0,NULL,&s_potionbonus};
statetype s_rkey2bonus = {RKEY2PIC,0,NULL,&s_rkey2bonus};
statetype s_rkeybonus = {RKEYOBJPIC,0,NULL,&s_rkeybonus};
statetype s_ykeybonus = {YKEYOBJPIC,0,NULL,&s_ykeybonus};
statetype s_gkeybonus = {GKEYOBJPIC,0,NULL,&s_gkeybonus};
statetype s_bkeybonus = {BKEYOBJPIC,0,NULL,&s_bkeybonus};
statetype s_scrollbonus = {SCROLLOBJPIC,0,NULL,&s_scrollbonus};
statetype s_chestbonus = {CHESTOBJPIC,0,NULL,&s_chestbonus};
//statetype s_goalbonus = {NEMESISPIC,0,NULL,&s_goalbonus};

extern statetype s_waterchestbonus2;
statetype s_waterchestbonus1 = {O_WATER_CHEST1PIC,8,NULL,&s_waterchestbonus2};
statetype s_waterchestbonus2 = {O_WATER_CHEST2PIC,8,NULL,&s_waterchestbonus1};

extern statetype s_rgem2bonus;
extern statetype s_ygem2bonus;
extern statetype s_ggem2bonus;
extern statetype s_bgem2bonus;
extern statetype s_pgem2bonus;

statetype s_rgem1bonus = {RGEM1PIC,30,NULL,&s_rgem2bonus};
statetype s_ygem1bonus = {YGEM1PIC,30,NULL,&s_ygem2bonus};
statetype s_ggem1bonus = {GGEM1PIC,30,NULL,&s_ggem2bonus};
statetype s_bgem1bonus = {BGEM1PIC,30,NULL,&s_bgem2bonus};
statetype s_pgem1bonus = {PGEM1PIC,30,NULL,&s_pgem2bonus};

statetype s_rgem2bonus = {RGEM2PIC,30,NULL,&s_rgem1bonus};
statetype s_ygem2bonus = {YGEM2PIC,30,NULL,&s_ygem1bonus};
statetype s_ggem2bonus = {GGEM2PIC,30,NULL,&s_ggem1bonus};
statetype s_bgem2bonus = {BGEM2PIC,30,NULL,&s_bgem1bonus};
statetype s_pgem2bonus = {PGEM2PIC,30,NULL,&s_pgem1bonus};

statetype s_bonus_die = {0,8,NULL,NULL};

/*
===============
=
= SpawnBonus
=
===============
*/

void SpawnBonus (id0_int_t tilex, id0_int_t tiley, id0_int_t number)
{
	extern id0_unsigned_t gnd_colors[];

	statetype *state;

	switch (number)
	{
		case B_BOLT:			state = &s_boltbonus;		break;
		case B_NUKE:			state = &s_nukebonus;		break;
		case B_POTION:			state = &s_potionbonus;		break;

		case B_RKEY:			state = &s_rkeybonus;		break;
		case B_YKEY:			state = &s_ykeybonus;		break;
		case B_GKEY:			state = &s_gkeybonus;		break;
		case B_BKEY:			state = &s_bkeybonus;		break;

		case B_RGEM:			state = &s_rgem1bonus;		break;
		case B_YGEM:			state = &s_ygem1bonus;		break;
		case B_GGEM:			state = &s_ggem1bonus;		break;
		case B_BGEM:			state = &s_bgem1bonus;		break;
		case B_PGEM:			state = &s_pgem1bonus;		break;

		case B_CHEST:
			if (gnd_colors[gamestate.mapon] == 0x0101)
				state = &s_waterchestbonus1;
			else
				state = &s_chestbonus;
		break;

		case B_SCROLL1:
		case B_SCROLL2:
		case B_SCROLL3:
		case B_SCROLL4:
		case B_SCROLL5:
		case B_SCROLL6:
		case B_SCROLL7:
		case B_SCROLL8:			state = &s_scrollbonus;		break;

		case B_RKEY2:			state = &s_rkey2bonus;		break;

		default:
			Quit("SpawnBonus(): INVALID BONUS");
		break;
	}

	SpawnNewObj (tilex,tiley,state,TILEGLOBAL/2);
//	newobj->tileobject = true;
	newobj->temp1 = number;
	newobj->obclass = bonusobj;

	switch (number)
	{
		case B_POTION:
		case B_CHEST:
		case B_BOLT:
		case B_NUKE:
			newobj->flags |= of_shootable;
		break;

		default:
			newobj->flags &= ~of_shootable;
		break;
	}
}


/*
===============
=
= SpawnTombstone
=
===============
*/

statetype s_tombs[3] = {{TOMB1PIC,8,NULL,&s_tombs[0]},
								{TOMB2PIC,8,NULL,&s_tombs[1]},
								{TOMB3PIC,8,NULL,&s_tombs[2]}};

void SpawnTombstone (id0_int_t tilex, id0_int_t tiley, id0_int_t shape)
{
	statetype *state=&s_tombs[shape];

	SpawnNewObj (tilex,tiley,state,TILEGLOBAL/2);
//	newobj->tileobject = true;
	newobj->obclass = solidobj;
	newobj->flags |= of_shootable;
}


/*
============================================================================

									FREEZE TIME OBJECT

============================================================================
*/

extern statetype s_ftimebonus;
extern statetype s_ftimebonus2;

statetype s_ftimebonus = {TIMEOBJ1PIC,6,NULL,&s_ftimebonus2};
statetype s_ftimebonus2 = {TIMEOBJ2PIC,6,NULL,&s_ftimebonus};

/*
===============
=
= SpawnFTime
=
===============
*/
void SpawnFTime(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_ftimebonus,TILEGLOBAL/2);
//	newobj->tileobject = true;
	newobj->obclass = freezeobj;
	newobj->flags |= of_shootable;
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

	extern id0_unsigned_t gnd_colors[];

void ExplodeWall (id0_int_t tilex, id0_int_t tiley)
{
	id0_unsigned_t tilenum;

	DSpawnNewObj (tilex,tiley,&s_walldie1,0);
	if (newobj == &dummyobj)
		return;
	newobj->obclass = inertobj;
	newobj->active = always;
	if (gnd_colors[gamestate.mapon] == 0x0101)
		tilenum = WATEREXP;
	else
		tilenum = WALLEXP;
	/*(id0_unsigned_t)actorat[newobj->tilex][newobj->tiley] = */tilemap[newobj->tilex][newobj->tiley] =
		*(mapsegs[0]+farmapylookup[newobj->tiley]+newobj->tilex) = tilenum;
	actorat[newobj->tilex][newobj->tiley] = tilemap[newobj->tilex][newobj->tiley];
	*(mapsegs[2]+farmapylookup[newobj->tiley]+newobj->tilex) &= 0xFF;
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
	id0_unsigned_t tile/*,other*/,spot,x,y;

	if (++ob->temp1 == 3)
		tile = 0;
	else
		if (gnd_colors[gamestate.mapon] == 0x0101)
			tile = WATEREXP-1 + ob->temp1;
		else
			tile = WALLEXP-1 + ob->temp1;
	x = ob->tilex;
	y = ob->tiley;

	/*(id0_unsigned_t)actorat[x][y] = */tilemap[x][y] = *(mapsegs[0]+farmapylookup[y]+x) = tile;
	actorat[x][y] = tilemap[x][y];

	if (ob->temp1 == 1)
	{
	//
	// blow up nearby walls
	//
		spot = (*(mapsegs[2]+farmapylookup[y]+(x-1))) >> 8;
		if (spot == EXP_WALL_CODE)
			ExplodeWall (x-1,y);
		spot = (*(mapsegs[2]+farmapylookup[y]+(x+1))) >> 8;
		if (spot == EXP_WALL_CODE)
			ExplodeWall (x+1,y);
		spot = (*(mapsegs[2]+farmapylookup[y-1]+x)) >> 8;
		if (spot == EXP_WALL_CODE)
			ExplodeWall (x,y-1);
		spot = (*(mapsegs[2]+farmapylookup[y+1]+x)) >> 8;
		if (spot == EXP_WALL_CODE)
			ExplodeWall (x,y+1);
	}
}
/*
=============================================================================

								OBJ_WARP GATE

=============================================================================
*/

void T_Gate (objtype *ob);

extern	statetype s_obj_gate1;
extern	statetype s_obj_gate2;
extern	statetype s_obj_gate3;
extern	statetype s_obj_gate4;

statetype s_obj_gate1 = {OBJ_WARP1PIC,10,T_Gate,&s_obj_gate2};
statetype s_obj_gate2 = {OBJ_WARP2PIC,10,T_Gate,&s_obj_gate3};
statetype s_obj_gate3 = {OBJ_WARP3PIC,10,T_Gate,&s_obj_gate4};
statetype s_obj_gate4 = {OBJ_WARP4PIC,10,T_Gate,&s_obj_gate1};

statetype s_pit = {PITOBJPIC,6,T_Gate,&s_pit};


//---------------------------------------------------------------------------
//	SpawnWarp()
//
// TYPE : Type param is the gate number (1-what ever) will link you to
//			 gate of that type.
//---------------------------------------------------------------------------
void SpawnWarp (id0_int_t tilex, id0_int_t tiley, id0_int_t type)
{

	if (type)
		SpawnNewObj (tilex,tiley,&s_obj_gate1,TILEGLOBAL/3);
	else
		SpawnNewObj (tilex,tiley,&s_pit,TILEGLOBAL/3);
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
	objtype *check;
	id0_unsigned_t	/*temp,*/spot;

	if (CheckHandAttack (ob) && !playstate)
	{
		// make
		//
//		spot = (*(mapsegs[2]+farmapylookup[ob->tiley+1]+ob->tilex)) >> 8;
//		if (spot--)
//			if (gamestate.keys[spot])
//				TakeKey(spot);
//			else
//				return;

		//
		// warp
		//

//		temp = bufferofs;
//		bufferofs = 0;
//		VW_Bar (26,4,232,9,STATUSCOLOR);		// clear text description
//		bufferofs = temp;

//		IN_ClearKeysDown ();
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
					SD_PlaySound(WARPSND);
				}
		}
		else
		{
			//
			// teleport out of level
			//

			playstate = ex_warped;
			spot = (*(mapsegs[2]+farmapylookup[ob->tiley+1]+ob->tilex)) >> 8;
			gamestate.mapon=spot;
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

statetype s_trollattack1 = {TROLLATTACK1PIC,15,NULL,&s_trollattack2};
statetype s_trollattack2 = {TROLLATTACK2PIC,15,NULL,&s_trollattack3};
statetype s_trollattack3 = {TROLLATTACK3PIC,30,T_DoDamage,&s_trollpause};

statetype s_trollouch = {TROLLOUCHPIC,14,T_Troll,&s_troll1};

statetype s_trolldie1 = {TROLLDIE1PIC,18,NULL,&s_trolldie2};
statetype s_trolldie2 = {TROLLDIE2PIC,18,NULL,&s_trolldie3};
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
	SpawnNewObj(tilex,tiley,&s_troll1,35*PIXRADIUS);
	newobj->speed = 2500;
	newobj->obclass = trollobj;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(14);
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
	if (Chase(ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_trollattack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}


void T_WetMan(objtype *ob);


//statetype s_wet_pause = {WET_WALK1PIC,25,NULL,&s_wet_walk1};

statetype s_wet_bubbles1 = {WET_BUBBLE1PIC,13,T_WetMan,&s_wet_bubbles2};
statetype s_wet_bubbles2 = {WET_BUBBLE2PIC,15,T_WetMan,&s_wet_bubbles1};
statetype s_wet_bubbles3 = {0,18,T_WetMan,&s_wet_bubbles1};

statetype s_wet_peek = {WET_EYESPIC,45,NULL,&s_wet_bubbles1};

//statetype s_wet_rise1 = {WET_BUBBLE1PIC,14,NULL,&s_wet_rise2};
statetype s_wet_rise1 = {WET_BUBBLE2PIC,15,NULL,&s_wet_rise3};
statetype s_wet_rise3 = {WET_EYESPIC,20,NULL,&s_wet_rise4};
statetype s_wet_rise4 = {WET_RISE1PIC,20,NULL,&s_wet_rise5};
statetype s_wet_rise5 = {WET_RISE2PIC,20,NULL,&s_wet_walk1};

statetype s_wet_sink1 = {WET_RISE2PIC,20,NULL,&s_wet_sink2};
statetype s_wet_sink2 = {WET_RISE1PIC,20,NULL,&s_wet_sink3};
statetype s_wet_sink3 = {WET_EYESPIC,20,NULL,&s_wet_bubbles1};

statetype s_wet_walk1 = {WET_WALK1PIC,12,T_WetMan,&s_wet_walk2};
statetype s_wet_walk2 = {WET_WALK2PIC,12,T_WetMan,&s_wet_walk3};
statetype s_wet_walk3 = {WET_WALK3PIC,12,T_WetMan,&s_wet_walk4};
statetype s_wet_walk4 = {WET_WALK4PIC,12,T_WetMan,&s_wet_walk1};


statetype s_wet_attack1 = {WET_ATTACK1PIC,10,NULL,&s_wet_attack2};
statetype s_wet_attack2 = {WET_ATTACK2PIC,10,NULL,&s_wet_attack3};
statetype s_wet_attack3 = {WET_ATTACK2PIC,10,NULL,&s_wet_attack4};
statetype s_wet_attack4 = {WET_ATTACK3PIC,10,T_DoDamage,&s_wet_walk1};

statetype s_wet_ouch = {WET_OUCHPIC,10,NULL,&s_wet_walk1};

statetype s_wet_die1 = {WET_DIE1PIC,27,NULL,&s_wet_die2};
statetype s_wet_die2 = {WET_DIE2PIC,29,NULL,&s_wet_die3};
statetype s_wet_die3 = {WET_DIE3PIC,44,NULL,&s_wet_die4};
statetype s_wet_die4 = {WET_BUBBLE2PIC,26,NULL,&s_wet_die5};
statetype s_wet_die5 = {WET_BUBBLE1PIC,23,NULL,NULL};


typedef enum {wt_BUBBLES,wt_WALK} WetManTypes;

#define WT_TIMEREMAIN 	(ob->temp1)
#define WT_STAGE			(ob->temp2)


/*
===============
=
= SpawnWetMan
=
===============
*/
void SpawnWetMan(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	SpawnNewObj(tilex,tiley,&s_wet_bubbles1,PIXRADIUS*35);
	ob=newobj;

	WT_STAGE = wt_BUBBLES;
	WT_TIMEREMAIN = 60*4+BE_Cross_Brandom(60*3);

	newobj->obclass = wetobj;
	newobj->speed = 1000;
	newobj->flags &= ~of_shootable;
	newobj->hitpoints = EasyHitPoints(18);
}


/*
===============
=
= T_WetMan
=
===============
*/

void T_WetMan(objtype *ob)
{
	switch (WT_STAGE)
	{
		case wt_BUBBLES:
			ob->flags &= ~of_shootable;
			if (Chase(ob,true))
			{
				// RISE & GOTO WALK STAGE
				//

				WT_STAGE = wt_WALK;
				WT_TIMEREMAIN = 60*5+BE_Cross_Brandom(60*5);
				ob->state = &s_wet_rise1;
				ob->speed = 2200;
				ob->ticcount = ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for WALK
				//
				if ((WT_TIMEREMAIN-=realtics) < 0)
				{
					// RISE & GOTO WALK STAGE
					//

					WT_STAGE = wt_WALK;
					WT_TIMEREMAIN = 60+BE_Cross_Brandom(60*2);
					ob->state = &s_wet_rise1;
					ob->speed = 2200;
					ob->ticcount = ob->state->tictime;
				}
				else
				if (BE_Cross_Brandom(1000)<5)
				{
					// RANDOM PEEK UP OUT OF WATER
					//

					ob->state=&s_wet_peek;
					ob->ticcount = ob->state->tictime;
				}
			}
			break;


		case wt_WALK:
			ob->flags |= of_shootable;
			if (Chase(ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
			{
				ob->state = &s_wet_attack1;
				ob->ticcount = ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for SINK
				//
				if ((WT_TIMEREMAIN-=realtics) < 0)
				{
					// SINK & GOTO BUBBLE STAGE
					//

					WT_STAGE = wt_BUBBLES;
					WT_TIMEREMAIN = 60*4+BE_Cross_Brandom(60*3);
					ob->state = &s_wet_sink1;
					ob->speed = 1200;
					ob->ticcount = ob->state->tictime;
					ob->flags &= ~of_shootable;
				}

			}
			break;
	}
}

/*
=============================================================================

							ZOMBIE

=============================================================================
*/

extern statetype s_zombie_rise1;
extern statetype s_zombie_rise2;
extern statetype s_zombie_rise3;
extern statetype s_zombie_rise4;

extern statetype s_zombie_alive1;
extern statetype s_zombie_alive2;
extern statetype s_zombie_alive3;

//extern statetype s_zombie_attack1;

extern statetype s_zombie_death1;
extern statetype s_zombie_death2;
extern statetype s_zombie_death3;

void T_Zombie (objtype *ob);
void T_ZombieRisen(objtype *obj);

statetype s_zombie_risen = {ZOMB_WALK3PIC,1,T_ZombieRisen,&s_zombie_alive1};

statetype s_zombie_pause = {ZOMB_WALK1PIC,20,NULL,&s_zombie_alive1};

statetype s_zombie_inground = {0,13,T_Zombie,&s_zombie_inground};

statetype s_zombie_rise1 = {ZOMB_APPEAR1PIC,24,NULL,&s_zombie_rise2};
statetype s_zombie_rise2 = {ZOMB_APPEAR2PIC,24,NULL,&s_zombie_rise3};
statetype s_zombie_rise3 = {ZOMB_APPEAR3PIC,24,NULL,&s_zombie_rise4};
statetype s_zombie_rise4 = {ZOMB_APPEAR4PIC,24,NULL,&s_zombie_risen};

statetype s_zombie_alive1 = {ZOMB_WALK1PIC,13,T_Zombie,&s_zombie_alive2};
statetype s_zombie_alive2 = {ZOMB_WALK2PIC,13,T_Zombie,&s_zombie_alive3};
statetype s_zombie_alive3 = {ZOMB_WALK3PIC,13,T_Zombie,&s_zombie_alive1};

statetype s_zombie_death1 = {ZOMB_DIE1PIC,16,NULL,&s_zombie_death2};
statetype s_zombie_death2 = {ZOMB_DIE2PIC,16,NULL,&s_zombie_death3};
statetype s_zombie_death3 = {ZOMB_DIE3PIC,16,NULL,&s_zombie_death3};

statetype s_zombie_attack  = {ZOMB_ATTACKPIC,15,T_DoDamage,&s_zombie_pause};
//statetype s_zombie_attack1 = {ZOMB_ATTACKPIC,15,NULL,&s_zombie_pause};

statetype s_zombie_ouch = {ZOMB_OUCHPIC,15,NULL,&s_zombie_alive1};




enum zombie_modes {zm_wait_for_dark,zm_wait_to_rise,zm_active};

#define zombie_mode	ob->temp1
#define zombie_delay	ob->temp2


//--------------------------------------------------------------------------
// SpawnZombie()
//--------------------------------------------------------------------------
void SpawnZombie (id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	id0_short_t current_zombie_delay;
	id0_unsigned_t tile;

	SpawnNewObj(tilex,tiley,&s_zombie_inground,35*PIXRADIUS);
	ob = newobj;
	zombie_mode = zm_wait_for_dark;

	tile = *(mapsegs[2]+farmapylookup[tiley+1]+tilex);
	if (tile)
		zombie_delay = (tile>>8)*30;
	else
	{
		current_zombie_delay = (2*60)+BE_Cross_Brandom(4*60);
		zombie_delay = zombie_base_delay+current_zombie_delay;
		zombie_base_delay += current_zombie_delay;
		if (zombie_base_delay > 8*60)
			zombie_base_delay = 0;
	}

	newobj->speed = 2500;
	newobj->obclass = zombieobj;
	newobj->hitpoints = EasyHitPoints(5);
	newobj->active = yes;
	newobj->flags &= ~of_shootable;
}

//--------------------------------------------------------------------------
// T_Zombie()
//--------------------------------------------------------------------------
void T_Zombie (objtype *ob)
{
	switch (zombie_mode)
	{
		case zm_wait_for_dark:
			if (gamestate.mapon == 0)
			{
				if (BGFLAGS & BGF_NIGHT)
					zombie_mode = zm_wait_to_rise;
			}
			else
				zombie_mode = zm_wait_to_rise;
		break;

		case zm_wait_to_rise:
			if (zombie_delay < 0)
			{
				if ((ob->tilex == player->tilex) && (ob->tiley == player->tiley))
					break;

				ob->active = always;
				ob->state = &s_zombie_rise1;
				ob->ticcount = ob->state->tictime;		// JIM Added
				zombie_mode = zm_active;
			}
			else
				zombie_delay -= tics;

		break;

		case zm_active:
			if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
			{
				ob->state = &s_zombie_attack;
				ob->ticcount = ob->state->tictime;
				return;
			}
		break;
	}
}

//--------------------------------------------------------------------------
// T_ZombieRisen()
//--------------------------------------------------------------------------
void T_ZombieRisen(objtype *obj)
{
	obj->flags |= of_shootable;
}



/*
=============================================================================

											SPOOKS

=============================================================================
*/

void T_Spook(objtype *ob);

statetype s_spook_wait = {0,10,T_Spook,&s_spook_wait};

statetype s_spook0 = {SPOOK_INOUTPIC,7,NULL,&s_spook0_1};
statetype s_spook0_1 = {0,7,NULL,&s_spook0_2};
statetype s_spook0_2 = {SPOOK_INOUTPIC,7,NULL,&s_spook1};

statetype s_spook1 = {SPOOK1PIC,10,NULL,&s_spook2};
statetype s_spook2 = {SPOOK2PIC,10,T_Spook,&s_spook3};
statetype s_spook3 = {SPOOK3PIC,10,T_Spook,&s_spook4};
statetype s_spook4 = {SPOOK4PIC,10,T_Spook,&s_spook5};
statetype s_spook5 = {SPOOK3PIC,10,T_Spook,&s_spook6};
statetype s_spook6 = {SPOOK2PIC,10,T_Spook,&s_spook1};

statetype s_spook_attack1 = {SPOOK_ATTACKPIC,35,NULL,&s_spook_attack3};
statetype s_spook_attack3 = {SPOOK3PIC,20,T_DoDamage,&s_spook1};

statetype s_spook_pause  = {SPOOK1PIC,20,NULL,&s_spook1};

statetype s_spookouch = {SPOOKHITPIC,5,NULL,&s_spook1};

statetype s_spookdie = {SPOOK_INOUTPIC,9,NULL,&s_spookdie1};
statetype s_spookdie1 = {SPOOK4PIC,9,NULL,&s_spookdie2};
statetype s_spookdie2 = {SPOOK_INOUTPIC,9,NULL,&s_spookdie3};
statetype s_spookdie3 = {SPOOK4PIC,9,NULL,&s_spookdie4};
statetype s_spookdie4 = {SPOOK_INOUTPIC,9,NULL,&s_spookdie5};
statetype s_spookdie5 = {0,11,NULL,NULL};

#define spook_mode	ob->temp1
#define spook_delay	ob->temp2

/*
===============
=
= SpawnSpook
=
===============
*/
void SpawnSpook(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	id0_unsigned_t tile;
	SpawnNewObj(tilex,tiley,&s_spook_wait,PIXRADIUS*35);
	ob = newobj;

	tile = *(mapsegs[2]+farmapylookup[tiley+1]+tilex);
	if (tile)
		spook_delay = (tile>>8)*30;
	else
		spook_delay = 2*60+BE_Cross_Brandom(5*60);

	spook_mode = zm_wait_for_dark;

	newobj->active = yes;
	newobj->obclass = spookobj;
	newobj->speed = 1900;
	newobj->flags &= ~of_shootable;
	newobj->hitpoints = EasyHitPoints(5);
}


/*
===============
=
= T_Spook
=
===============
*/
void T_Spook(objtype *ob)
{
	switch (zombie_mode)
	{
		case zm_wait_for_dark:
			if (!gamestate.mapon)
			{
				if (BGFLAGS & BGF_NIGHT)
					spook_mode = zm_wait_to_rise;
			}
			else
				spook_mode = zm_wait_to_rise;
		break;

		case zm_wait_to_rise:
			spook_delay -= tics;
			if (spook_delay < 0)
			{
				if ((ob->tilex == player->tilex) && (ob->tiley == player->tiley))
					break;

				ob->active = always;
				ob->flags |= of_shootable;
				ob->state = &s_spook0;
				ob->ticcount = ob->state->tictime;
				spook_mode = zm_active;
			}
			else
				spook_delay -= tics;
		break;

		case zm_active:
			if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
			{
				ob->state = &s_spook_attack1;
				ob->ticcount = ob->state->tictime;
				return;
			}
		break;
	}
}


/*
=============================================================================

								SKELETON IN WALL

=============================================================================
*/

void T_WallSkeleton(objtype *ob);

extern statetype s_skel_1;

statetype s_wallskel = {0,40,T_WallSkeleton,&s_wallskel};
statetype s_wallskel2 = {0,1,NULL,NULL};


enum wskel_modes {ws_wall1,ws_wall2,ws_wall3,ws_exit};

#define wskel_mode	ob->temp1
#define wskel_delay	ob->temp2
#define wskel_base	ob->angle
#define wskel_wallx	ob->hitpoints
#define wskel_wally	ob->speed

/*
===============
=
= SpawnWallSkeleton
=
===============
*/
void SpawnWallSkeleton(id0_int_t tilex, id0_int_t tiley)
{
	id0_char_t xofs[] = {0,0,-1,+1};
	id0_char_t yofs[] = {-1,+1,0,0};

	objtype *ob;
	id0_int_t wallx=tilex,wally=tiley,wallbase,wallmode,loop;
	id0_unsigned_t tile,current_delay;

	for (loop=0; loop<4; loop++)
	{
		tile = *(mapsegs[0]+farmapylookup[tiley+yofs[loop]]+tilex+xofs[loop]);
		switch (tile)
		{
			case WALL_SKELETON_CODE:
			case WALL_SKELETON_CODE+1:
			case WALL_SKELETON_CODE+2:
				wallmode = ws_wall1+(tile-WALL_SKELETON_CODE);
				wallbase = WALL_SKELETON_CODE;
				goto foundtile;
			break;

			case 41:
			case 43:
				wallmode = ws_wall1+(tile-41);
				wallbase = 41;
				goto foundtile;
			break;

			case 42:
			case 44:
				wallmode = ws_wall1+(tile-42);
				wallbase = 42;
				goto foundtile;
			break;
		}
	}

	return;
foundtile:;

	wallx += xofs[loop];
	wally += yofs[loop];

	SpawnNewObj(tilex,tiley,&s_wallskel,PIXRADIUS*35);
	ob = newobj;
	newobj->obclass = wallskelobj;
	newobj->speed = 1900;
	newobj->flags &= ~of_shootable;
	newobj->hitpoints = EasyHitPoints(5);

//	newobj->tilex = wallx;
//	newobj->tiley = wally;
	wskel_wallx = wallx;
	wskel_wally = wally;
	wskel_base = wallbase;
	newobj->active = no;

	wskel_mode = wallmode;

	tile = *(mapsegs[2]+farmapylookup[wally]+wallx);
	if (tile)
		wskel_delay = (tile>>8)*30;
	else
	{
		current_delay = (2*60)+BE_Cross_Brandom(4*60);
		wskel_delay = zombie_base_delay+current_delay;
		zombie_base_delay += current_delay;
		if (zombie_base_delay > 8*60)
			zombie_base_delay = 0;
	}
}

/*
===============
=
= T_WallSkeleton
=
===============
*/
void T_WallSkeleton(objtype *ob)
{
	id0_int_t x=wskel_wallx,y=wskel_wally;

	wskel_delay -= realtics;
	if (wskel_delay > 0)
		return;

	switch (wskel_mode)
	{
		case ws_wall2:
			if ((wskel_base == 41) || (wskel_base == 42))
				wskel_mode++;
		case ws_wall1:
		case ws_wall3:
			/*(id0_unsigned_t)actorat[x][y]
				=*/ tilemap[x][y]
				= *(mapsegs[0]+farmapylookup[y]+x)
				= wskel_base+(wskel_mode-ws_wall1);
			actorat[x][y] = tilemap[x][y];

			wskel_mode++;
			wskel_delay = (120);
			ob->active = always;
		break;

		case ws_exit:
//			wskel_delay = 0;
//			if (objectcount >= MAXACTORS-3)
//				break;

			/*(id0_unsigned_t)actorat[x][y]
				=*/ tilemap[x][y]
				= *(mapsegs[0]+farmapylookup[y]+x)
				= wskel_base;
			actorat[x][y] = tilemap[x][y];

			ob->tilex = ob->x >> TILESHIFT;
			ob->tiley = ob->y >> TILESHIFT;

			ob->obclass = skeletonobj;
			ob->speed = 2036;
			ob->flags |= of_shootable;
			ob->hitpoints = EasyHitPoints(12);
			ob->state = &s_skel_1;
			ob->ticcount = ob->state->tictime;

//			SpawnSkeleton(ob->tilex,ob->tiley);
//			ob->state = NULL;
		break;
	}
}


/*
=============================================================================

								SKELETONS

=============================================================================
*/

void T_Skeleton(objtype *ob);




statetype s_skel_pause = {SKELETON_1PIC,40,NULL,&s_skel_2};

statetype s_skel_1 = {SKELETON_1PIC,10,T_Skeleton,&s_skel_2};
statetype s_skel_2 = {SKELETON_2PIC,10,T_Skeleton,&s_skel_3};
statetype s_skel_3 = {SKELETON_3PIC,10,T_Skeleton,&s_skel_4};
statetype s_skel_4 = {SKELETON_4PIC,10,T_Skeleton,&s_skel_1};

statetype s_skel_attack1 = {SKELETON_ATTACK_1PIC,12,NULL,&s_skel_attack2};
statetype s_skel_attack2 = {SKELETON_ATTACK_2PIC,12,NULL,&s_skel_attack3};
statetype s_skel_attack3 = {SKELETON_ATTACK_3PIC,12,T_DoDamage,&s_skel_pause};

statetype s_skel_ouch = {SKELETON_OUCHPIC,8,NULL,&s_skel_1};

statetype s_skel_die1 = {SKELETON_OUCHPIC,18,NULL,&s_skel_die2};
statetype s_skel_die2 = {SKELETON_DEATH_1PIC,18,NULL,&s_skel_die3};
statetype s_skel_die3 = {SKELETON_DEATH_2PIC,18,NULL,&s_skel_die3};

/*
===============
=
= SpawnSkeleton
=
===============
*/
void SpawnSkeleton(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_skel_1,PIXRADIUS*35);
	newobj->obclass = skeletonobj;
	newobj->speed = 2036;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(12);
}


/*
===============
=
= T_Skeleton
=
===============
*/

void T_Skeleton(objtype *ob)
{
	if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_skel_attack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}

/*
=============================================================================

								EYE

=============================================================================
*/

void T_Eye(objtype *ob);
id0_boolean_t T_EyeShoot (objtype *ob, id0_boolean_t eyeshot);




statetype s_eye_pause = {EYE_WALK1PIC,40,NULL,&s_eye_2};

statetype s_eye_1 = {EYE_WALK1PIC,20,T_Eye,&s_eye_2};
statetype s_eye_2 = {EYE_WALK2PIC,20,T_Eye,&s_eye_3};
statetype s_eye_3 = {EYE_WALK3PIC,20,T_Eye,&s_eye_4};
statetype s_eye_4 = {EYE_WALK2PIC,20,T_Eye,&s_eye_1};

statetype s_eye_ouch = {EYE_OUCH1PIC,8,NULL,&s_eye_ouch2};
statetype s_eye_ouch2 = {EYE_OUCH2PIC,8,NULL,&s_eye_1};

statetype s_eye_die1 = {EYE_DEATH1PIC,22,NULL,&s_eye_die2};
statetype s_eye_die2 = {EYE_DEATH2PIC,22,NULL,&s_eye_die3};
statetype s_eye_die3 = {EYE_DEATH2PIC,22,NULL,NULL};

extern statetype s_eshot2;

statetype s_eshot1 = {EYE_SHOT1PIC,8,&T_ShootPlayer,&s_eshot2};
statetype s_eshot2 = {EYE_SHOT2PIC,8,&T_ShootPlayer,&s_eshot1};

enum eye_modes {em_other1,em_player1,em_other2,em_player2,em_other3,em_player3,em_other4,em_player4};

#define eye_mode	ob->temp1
#define eye_delay	ob->temp2

//-------------------------------------------------------------------------
// SpawnEye()
//-------------------------------------------------------------------------
void SpawnEye(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;

	SpawnNewObj(tilex,tiley,&s_eye_1,PIXRADIUS*35);
	ob = newobj;
	newobj->obclass = eyeobj;
	newobj->speed = 1200;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(14);
	eye_mode = em_other1;
}


//-------------------------------------------------------------------------
// T_Eye()
//-------------------------------------------------------------------------
void T_Eye(objtype *ob)
{
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;

	eye_delay -= realtics;
	if (eye_delay < 0)
	{
		eye_mode = BE_Cross_Brandom(em_player4);
		eye_delay = (10*60);
	}

	tempx = player->x;
	tempy = player->y;
	temp_tilex = player->tilex;
	temp_tiley = player->tiley;


	switch (eye_mode)
	{
		case em_other1:
		case em_other2:
		case em_other3:
		case em_other4:
			player->x = ((id0_long_t)other_x[eye_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)other_y[eye_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = other_x[eye_mode];
			player->tiley = other_y[eye_mode];
		break;
	}

	if (Chase(ob,true))
		eye_delay = 0;

	player->x = tempx;
	player->y = tempy;
	player->tilex = temp_tilex;
	player->tiley = temp_tiley;

	if (!BE_Cross_Brandom(2))
		ShootPlayer(ob,eshotobj,ESHOTSPEED,&s_eshot1);
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
statetype s_orcattack2 = {ORCATTACK2PIC,20,NULL,&s_orcattack3};
statetype s_orcattack3 = {ORCATTACK2PIC,30,T_DoDamage,&s_orcpause};

statetype s_orcouch = {ORCOUCHPIC,15,NULL,&s_orc1};

statetype s_orcdie1 = {ORCDIE1PIC,18,NULL,&s_orcdie2};
statetype s_orcdie2 = {ORCDIE2PIC,18,NULL,&s_orcdie3};
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
	SpawnNewObj(tilex,tiley,&s_orc1,PIXRADIUS*35);
	newobj->obclass = orcobj;
	newobj->speed = 1536;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(6);
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
	if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
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
statetype s_demonattack2 = {DEMONATTACK2PIC,20,NULL,&s_demonattack3};
statetype s_demonattack3 = {DEMONATTACK3PIC,30,T_DoDamage,&s_demonpause};

statetype s_demonouch = {DEMONOUCHPIC,15,T_Demon,&s_demon1};

statetype s_demondie1 = {DEMONDIE1PIC,40,NULL,&s_demondie2};
statetype s_demondie2 = {DEMONDIE2PIC,30,NULL,&s_demondie3};
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
	SpawnNewObj(tilex,tiley,&s_demon1,PIXRADIUS*35);
	newobj->obclass = demonobj;
	newobj->speed = 2048;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(50);
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
	if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_demonattack1;
		ob->ticcount = ob->state->tictime;
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


statetype s_magepause = {MAGE1PIC,10,NULL,&s_mage2};

statetype s_mage1 = {MAGE1PIC,20,T_Mage,&s_mage2};
statetype s_mage2 = {MAGE2PIC,20,T_Mage,&s_mage1};

//statetype s_mageattack1 = {MAGEATTACKPIC,20,NULL,&s_mageattack2};
//statetype s_mageattack2 = {MAGEATTACKPIC,-1,T_MageShoot,&s_mageattack3};
statetype s_mageattack3 = {MAGEATTACKPIC,30,NULL,&s_magepause};

statetype s_mageouch = {MAGEOUCHPIC,10,NULL,&s_mage1};

statetype s_magedie1 = {MAGEDIE1PIC,20,NULL,&s_magedie2};
statetype s_magedie2 = {MAGEDIE2PIC,0,NULL,&s_magedie2};


statetype s_mshot1 = {PSHOT1PIC,8,&T_ShootPlayer,&s_mshot2};
statetype s_mshot2 = {PSHOT2PIC,8,&T_ShootPlayer,&s_mshot1};

/*
===============
=
= SpawnMage
=
===============
*/

void SpawnMage (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_mage1,PIXRADIUS*35);
	newobj->obclass = mageobj;
	newobj->speed = 3072;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(5);
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
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;

	eye_delay -= realtics;
	if (eye_delay < 0)
	{
		eye_mode = BE_Cross_Brandom(em_player4);
		eye_delay = (10*60);
	}

	tempx = player->x;
	tempy = player->y;
	temp_tilex = player->tilex;
	temp_tiley = player->tiley;


	switch (eye_mode)
	{
		case em_other1:
		case em_other2:
		case em_other3:
		case em_other4:
			player->x = ((id0_long_t)other_x[eye_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)other_y[eye_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = other_x[eye_mode];
			player->tiley = other_y[eye_mode];
		break;
	}

	if (Chase(ob,true))
		eye_delay = 0;

	player->x = tempx;
	player->y = tempy;
	player->tilex = temp_tilex;
	player->tiley = temp_tiley;

	if (!BE_Cross_Brandom(10))
		if (ShootPlayer(ob,mshotobj,MSHOTSPEED,&s_mshot1))
		{
			ob->state = &s_mageattack3;
			ob->ticcount = ob->state->tictime;
		}
}

/*
=============================================================================

						RED DEMON

=============================================================================
*/

void T_RedDemon (objtype *ob);

extern	statetype s_red_demonpause;

extern	statetype s_red_demon1;
extern	statetype s_red_demon2;
extern	statetype s_red_demon3;
extern	statetype s_red_demon4;

extern	statetype s_red_demonattack1;
extern	statetype s_red_demonattack2;
extern	statetype s_red_demonattack3;

extern	statetype s_red_demonouch;

extern	statetype s_red_demondie1;
extern	statetype s_red_demondie2;
extern	statetype s_red_demondie3;

statetype s_red_demonpause = {RED_DEMON1PIC,40,NULL,&s_red_demon2};

statetype s_red_demon1 = {RED_DEMON1PIC,20,T_RedDemon,&s_red_demon2};
statetype s_red_demon2 = {RED_DEMON2PIC,20,T_RedDemon,&s_red_demon3};
statetype s_red_demon3 = {RED_DEMON3PIC,20,T_RedDemon,&s_red_demon4};
statetype s_red_demon4 = {RED_DEMON4PIC,20,T_RedDemon,&s_red_demon1};

statetype s_red_demonattack1 = {RED_DEMONATTACK1PIC,20,NULL,&s_red_demonattack2};
statetype s_red_demonattack2 = {RED_DEMONATTACK2PIC,20,NULL,&s_red_demonattack3};
statetype s_red_demonattack3 = {RED_DEMONATTACK3PIC,30,T_DoDamage,&s_red_demon2};

statetype s_red_demonouch = {RED_DEMONOUCHPIC,15,T_RedDemon,&s_red_demon1};

statetype s_red_demondie1 = {RED_DEMONDIE1PIC,40,NULL,&s_red_demondie2};
statetype s_red_demondie2 = {RED_DEMONDIE2PIC,30,NULL,&s_red_demondie3};
statetype s_red_demondie3 = {RED_DEMONDIE3PIC,0,NULL,&s_red_demondie3};



/*
===============
=
= SpawnRedDemon
=
===============
*/

void SpawnRedDemon (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_red_demon1,PIXRADIUS*35);
	newobj->obclass = reddemonobj;
	newobj->speed = 2048;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(50);
}


/*
===============
=
= T_RedDemon
=
===============
*/

void T_RedDemon (objtype *ob)
{
	if (Chase (ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_red_demonattack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}


/*
=============================================================================

							GRELMINAR

=============================================================================
*/


void T_Grelminar (objtype *ob);
void T_GrelminarShoot (objtype *ob);
void T_Grelm_DropKey(objtype *ob);

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
extern 	statetype s_greldie5a;
extern	statetype s_greldie6;


statetype s_grelpause = {GREL1PIC,50,NULL,&s_grel2};

statetype s_grel1 = {GREL1PIC,20,T_Grelminar,&s_grel2};
statetype s_grel2 = {GREL2PIC,20,T_Grelminar,&s_grel1};

//statetype s_grelattack1 = {GRELATTACKPIC,20,NULL,&s_grelattack2};
//statetype s_grelattack2 = {GRELATTACKPIC,-1,T_GrelminarShoot,&s_grelattack3};
statetype s_grelattack3 = {GRELATTACKPIC,30,NULL,&s_grelpause};

statetype s_grelouch = {GRELHITPIC,6,NULL,&s_grel1};

statetype s_greldie1 = {GRELDIE1PIC,22,NULL,&s_greldie2};
statetype s_greldie2 = {GRELDIE2PIC,22,NULL,&s_greldie3};
statetype s_greldie3 = {GRELDIE3PIC,22,NULL,&s_greldie4};
statetype s_greldie4 = {GRELDIE4PIC,22,NULL,&s_greldie5};
statetype s_greldie5 = {GRELDIE5PIC,22,NULL,&s_greldie5a};
statetype s_greldie5a = {GRELDIE5PIC,1,T_Grelm_DropKey,&s_greldie6};
statetype s_greldie6 = {GRELDIE6PIC,0,NULL,&s_greldie6};


extern statetype s_gshot1;

statetype s_gshot1 = {SKULL_SHOTPIC,8,T_ShootPlayer,&s_gshot1};

/*
===============
=
= SpawnGrelminar
=
===============
*/

void SpawnGrelminar (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_grel1,PIXRADIUS*35);
	newobj->obclass = grelmobj;
	newobj->speed = 2048;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(100);
}


/*
===============
=
= T_Grelminar
=
===============
*/

void T_Grelminar (objtype *ob)
{
	Chase (ob,false);

	if (!BE_Cross_Brandom(10))
		if (ShootPlayer(ob,gshotobj,10000,&s_gshot1))
		{
			ob->state = &s_grelattack3;
		}
}


//=================================
//
// T_Grelm_DropKey
//
//=================================
void T_Grelm_DropKey(objtype *ob)
{
	if (ob->temp1)
		return;

	SpawnBonus(ob->tilex,ob->tiley,B_RKEY);
	SD_PlaySound(GRELM_DEADSND);
	ob->temp1 = true;
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

statetype s_batdie1 = {BATDIE1PIC,18,NULL,&s_batdie2};
statetype s_batdie2 = {BATDIE2PIC,18,NULL,NULL};


/*
===============
=
= SpawnBat
=
===============
*/

void SpawnBat (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_bat1,PIXRADIUS*35);
	newobj->obclass =batobj;
	newobj->flags |= of_shootable;

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


//--------------------------------------------------------------------------
// ShootPlayer()
//--------------------------------------------------------------------------

// REFKEEN - Use classtype instead of short for obclass
id0_boolean_t ShootPlayer(objtype *ob, classtype/*id0_short_t*/ obclass, id0_short_t speed, statetype *state)
{
	id0_int_t AngleNearPlayer(objtype *ob);

	id0_int_t angle = AngleNearPlayer(ob);

	if (angle == -1)
		return(false);

	DSpawnNewObjFrac (ob->x,ob->y,state,PIXRADIUS*35);
	newobj->speed = speed;
	newobj->obclass = obclass;
	newobj->active = always;
	newobj->angle = angle;

	return(true);
}

//--------------------------------------------------------------------------
// T_ShootPlayer()
//--------------------------------------------------------------------------
void T_ShootPlayer(objtype *ob)
{
	objtype *check;
	id0_long_t xmove,ymove,speed;

	speed = ob->speed*tics;

	xmove = FixedByFrac(speed,costable[ob->angle]);
	ymove = -FixedByFrac(speed,sintable[ob->angle]);

	if (ShotClipMove(ob,xmove,ymove))
	{
		ob->state = &s_pshot_exp1;
		ob->ticcount = ob->state->tictime;
		return;
	}

	ob->tilex = ob->x >> TILESHIFT;
	ob->tiley = ob->y >> TILESHIFT;

// check for collision with wall
//
	if (tilemap[ob->tilex][ob->tiley])
	{
		SD_PlaySound (SHOOTWALLSND);
		ob->state = &s_pshot_exp1;
		ob->ticcount = s_pshot_exp1.tictime;
		return;
	}

//
//
	if ( ob->xl <= player->xh
	&& ob->xh >= player->xl
	&& ob->yl <= player->yh
	&& ob->yh >= player->yl)
	{
		switch (ob->obclass)
		{
			case eshotobj:
				TakeDamage (ESHOTDAMAGE);
			break;

			case mshotobj:
				TakeDamage (MSHOTDAMAGE);
			break;

			case gshotobj:
				TakeDamage (25);
			break;
		}
		ob->state = NULL;
		return;
	}

// check for collision
//
	for (check = player->next; check; check=check->next)
		if ((ob->flags & of_shootable) && ob->obclass != mageobj
		&& ob->xl <= check->xh
		&& ob->xh >= check->xl
		&& ob->yl <= check->yh
		&& ob->yh >= check->yl)
		{
			switch (ob->obclass)
			{
				case eshotobj:
					ShootActor (check,ESHOTDAMAGE);
				break;

				case mshotobj:
					ShootActor (check,MSHOTDAMAGE);
				break;

				case gshotobj:
					ShootActor (check,25);
				break;

				case pshotobj:
					ShootActor (check,25);
				break;
			}
			ob->state = &s_pshot_exp1;
			ob->ticcount = s_pshot_exp1.tictime;
			return;
		}
}

//-------------------------------------------------------------------------
// AngleNearPlayer()
//-------------------------------------------------------------------------
id0_int_t AngleNearPlayer(objtype *ob)
{
	id0_int_t angle=-1;
	id0_int_t xdiff = ob->tilex-player->tilex;
	id0_int_t ydiff = ob->tiley-player->tiley;

	if (ob->tiley == player->tiley)
	{
		if (ob->tilex < player->tilex)
			angle = 0;
		else
			angle = 180;
	}
	else
	if (ob->tilex == player->tilex)
	{
		if (ob->tiley < player->tiley)
			angle = 270;
		else
			angle = 90;
	}
	else
	if (xdiff == ydiff)
		if (ob->tilex < player->tilex)
		{
			if (ob->tiley < player->tiley)
				angle = 315;
			else
				angle = 45;
		}
		else
		{
			if (ob->tiley < player->tiley)
				angle = 225;
			else
				angle = 135;
		}

	return(angle);
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

