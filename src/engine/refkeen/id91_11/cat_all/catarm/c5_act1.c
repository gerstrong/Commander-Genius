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

// C3_PLAY.C

#include "def.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#if 0
#define MSHOTDAMAGE	2
#define MSHOTSPEED	10000

#define ESHOTDAMAGE	1
#define ESHOTSPEED	5000

#define SSHOTDAMAGE	3
#define SSHOTSPEED	6500

#define RANDOM_ATTACK 20
#endif

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
//statetype s_rkey2bonus = {RKEY2PIC,0,NULL,&s_rkey2bonus};
statetype s_rkeybonus = {RKEYOBJPIC,0,NULL,&s_rkeybonus};
statetype s_ykeybonus = {YKEYOBJPIC,0,NULL,&s_ykeybonus};
statetype s_gkeybonus = {GKEYOBJPIC,0,NULL,&s_gkeybonus};
statetype s_bkeybonus = {BKEYOBJPIC,0,NULL,&s_bkeybonus};
//////////////////////////statetype s_scrollbonus = {SCROLLOBJPIC,0,NULL,&s_scrollbonus};
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
	extern id0_unsigned_t gcolor;

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
			if (gcolor == 0x0101)
				state = &s_waterchestbonus1;
			else
				state = &s_chestbonus;
		break;

#if 0
		case B_SCROLL1:
		case B_SCROLL2:
		case B_SCROLL3:
		case B_SCROLL4:
		case B_SCROLL5:
		case B_SCROLL6:
		case B_SCROLL7:
		case B_SCROLL8:			state = &s_scrollbonus;		break;
#endif

//		case B_RKEY2:			state = &s_rkey2bonus;		break;

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
	newobj->obclass = realsolidobj;
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

void ExplodeWall (id0_int_t tilex, id0_int_t tiley)
{
	extern id0_unsigned_t gcolor;
	id0_unsigned_t tilenum;

	DSpawnNewObj (tilex,tiley,&s_walldie1,0);
	if (newobj == &dummyobj)
		return;
	newobj->obclass = inertobj;
	newobj->active = always;
	if (gcolor == 0x0101)
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
	extern id0_unsigned_t gcolor;
	id0_unsigned_t tile/*,other*/,spot,x,y;

	if (++ob->temp1 == 3)
		tile = 0;
	else
		if (gcolor == 0x0101)
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

extern statetype s_anthill;
statetype s_anthill = {ANT_HILLPIC, 20, T_Gate, &s_anthill};

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
		SpawnNewObj (tilex,tiley,&s_anthill,TILEGLOBAL/3);
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

					FAT DEMON

=============================================================================
*/

#define FATCLOUDDAMAGE 2

void T_FatDemon (objtype *ob);
void T_CheckCnt(objtype *ob);
void ExplodeSound(objtype *ob);

extern statetype s_fatdemon_pause;
extern statetype s_fatdemon_walk1;
extern statetype s_fatdemon_walk2;
extern statetype s_fatdemon_walk3;
extern statetype s_fatdemon_walk4;
extern statetype s_fatdemon_attack1;
extern statetype s_fatdemon_attack2;
extern statetype s_fatdemon_blowup2;
extern statetype s_fatdemon_blowup3;
extern statetype s_fatdemon_blowup4;
extern statetype s_fatdemon_blowup5;
extern statetype s_fatdemon_blowup6;
extern statetype s_fatdemon_blowup7;
extern statetype s_fatdemon_explode;
extern statetype s_fatdemon_feet;

statetype s_fatdemon_pause = {FATDEMON_WALK1PIC,40,NULL,&s_fatdemon_walk2};

statetype s_fatdemon_walk1 = {FATDEMON_WALK1PIC,13,T_FatDemon,&s_fatdemon_walk2};
statetype s_fatdemon_walk2 = {FATDEMON_WALK2PIC,13,T_FatDemon,&s_fatdemon_walk3};
statetype s_fatdemon_walk3 = {FATDEMON_WALK3PIC,13,T_FatDemon,&s_fatdemon_walk4};
statetype s_fatdemon_walk4 = {FATDEMON_WALK4PIC,13,T_FatDemon,&s_fatdemon_walk1};

statetype s_fatdemon_attack1 = {FATDEMON_ATTACK1PIC,20,NULL,&s_fatdemon_attack2};
statetype s_fatdemon_attack2 = {FATDEMON_ATTACK2PIC,20,T_DoDamage,&s_fatdemon_pause};

statetype s_fatdemon_ouch = {FATDEMON_OUCHPIC,14,NULL,&s_fatdemon_walk1};

statetype s_fatdemon_blowup1 = {FATDEMON_BLOWUP1PIC,25,NULL,&s_fatdemon_blowup2};
statetype s_fatdemon_blowup2 = {FATDEMON_BLOWUP2PIC,25,NULL,&s_fatdemon_blowup3};
statetype s_fatdemon_blowup3 = {FATDEMON_BLOWUP1PIC,15,NULL,&s_fatdemon_blowup4};
statetype s_fatdemon_blowup4 = {FATDEMON_BLOWUP2PIC,15,NULL,&s_fatdemon_blowup5};
statetype s_fatdemon_blowup5 = {FATDEMON_BLOWUP1PIC,6,NULL,&s_fatdemon_blowup6};
statetype s_fatdemon_blowup6 = {FATDEMON_BLOWUP2PIC,6,T_CheckCnt,&s_fatdemon_blowup5};
statetype s_fatdemon_blowup7 = {FATDEMON_BLOWUP3PIC,30,NULL,&s_fatdemon_explode};


statetype s_fatdemon_explode = {FATDEMON_EXPLODEPIC,40,ExplodeSound,&s_fatdemon_feet};
statetype s_fatdemon_feet = {FATDEMON_FEETPIC,30,NULL,&s_fatdemon_feet};

#define cnt		ob->temp1
#define cloud_delay	ob->temp2

/*
===============
=
= SpawnFatDemon
=
===============
*/

void SpawnFatDemon (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_fatdemon_walk1,35*PIXRADIUS);
	newobj->speed = 2500;
	newobj->obclass = fatdemonobj;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(10);
	newobj->temp1 = 25;	//used to "shake" the fat dude??????
}


/*
===============
=
= T_FatDemon
=
===============
*/

void T_FatDemon (objtype *ob)
{
	if (Chase(ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_fatdemon_attack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}


/*
===============
=
= T_DecCnt
=
===============
*/

void T_CheckCnt (objtype *ob)
{
	ob->temp1--;
	if (!ob->temp1)
	{
		ob->state = &s_fatdemon_blowup7;
		ob->ticcount = ob->state->tictime;
	}
}

/*
===============
=
= ExplodeSound
=
===============
*/
void ExplodeSound(objtype *ob)
{
	if (ob->temp1 != 666)						// Has this think been called already?
	{
		SD_PlaySound(BODY_EXPLODESND);
		ob->temp1 = 666;              		// Has now!
	}
}


/*
=============================================================================

				WATER DRAGON

=============================================================================
*/

extern statetype s_dragon_shot1;
extern statetype s_dragon_shot2;


void T_Dragon(objtype *ob);
void T_DragonShoot(objtype *ob);


statetype s_wet_bubbles1 = {DRAGON_BUBBLES1PIC,13,T_Dragon,&s_wet_bubbles2};
statetype s_wet_bubbles2 = {DRAGON_BUBBLES2PIC,15,T_Dragon,&s_wet_bubbles1};
statetype s_wet_bubbles3 = {0,35,T_Dragon,&s_wet_bubbles1};

statetype s_wet_peek = {DRAGON_EYESPIC,45,NULL,&s_wet_bubbles1};

statetype s_wet_rise1 = {DRAGON_BUBBLES2PIC,15,NULL,&s_wet_rise3};
statetype s_wet_rise3 = {DRAGON_EYESPIC,20,NULL,&s_wet_rise4};
statetype s_wet_rise4 = {DRAGON_RISE1PIC,20,NULL,&s_wet_rise5};
statetype s_wet_rise5 = {DRAGON_RISE2PIC,20,NULL,&s_wet_walk1};

statetype s_wet_sink1 = {DRAGON_RISE2PIC,20,NULL,&s_wet_sink2};
statetype s_wet_sink2 = {DRAGON_RISE1PIC,20,NULL,&s_wet_sink3};
statetype s_wet_sink3 = {DRAGON_EYESPIC,20,NULL,&s_wet_bubbles1};

statetype s_wet_walk1 = {DRAGON_WALK1PIC,12,T_Dragon,&s_wet_walk2};
statetype s_wet_walk2 = {DRAGON_WALK2PIC,12,T_Dragon,&s_wet_walk3};
statetype s_wet_walk3 = {DRAGON_WALK3PIC,12,T_Dragon,&s_wet_walk4};
statetype s_wet_walk4 = {DRAGON_WALK4PIC,12,T_Dragon,&s_wet_walk1};

statetype s_wet_attack1 = {DRAGON_ATTACK1PIC,10,NULL,&s_wet_attack2};
statetype s_wet_attack2 = {DRAGON_ATTACK2PIC,10,NULL,&s_wet_attack3};
statetype s_wet_attack3 = {DRAGON_ATTACK2PIC,10,NULL,&s_wet_attack4};
statetype s_wet_attack4 = {DRAGON_ATTACK3PIC,10,T_DragonShoot,&s_wet_walk1};

statetype s_wet_ouch = {DRAGON_OUCHPIC,10,T_Dragon,&s_wet_walk1};

statetype s_wet_die1 = {DRAGON_DEATH1PIC,27,NULL,&s_wet_die2};
statetype s_wet_die2 = {DRAGON_DEATH2PIC,29,NULL,&s_wet_die3};
statetype s_wet_die3 = {DRAGON_DEATH3PIC,44,NULL,&s_wet_die4};
statetype s_wet_die4 = {DRAGON_BUBBLES2PIC,26,NULL,&s_wet_die5};
statetype s_wet_die5 = {DRAGON_BUBBLES1PIC,23,NULL,NULL};

statetype s_dragon_shot1 = {PSHOT1PIC,8,&T_ShootPlayer,&s_dragon_shot2};
statetype s_dragon_shot2 = {PSHOT2PIC,8,&T_ShootPlayer,&s_dragon_shot1};


typedef enum {wt_BUBBLES,wt_WALK,wt_CORNER1,wt_CORNER2,wt_CORNER3,wt_CORNER4} DragonTypes;


#define WD_TIMEREMAIN 	(ob->temp1)
#define WD_STAGE	(ob->temp2)
#define WATER_DRAGON_LEAVE	0x04

/*
===============
=
= SpawnDragon
=
===============
*/
void SpawnDragon(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	SpawnNewObj(tilex,tiley,&s_wet_bubbles1,PIXRADIUS*35);
	ob=newobj;

	WD_STAGE = wt_BUBBLES;
	WD_TIMEREMAIN = 80;

	newobj->obclass = wetobj;
	newobj->speed = 1000;
	newobj->flags &= ~of_shootable;
	newobj->hitpoints = EasyHitPoints(20);
}


/*
===============
=
= T_Dragon
=
===============
*/

void T_Dragon(objtype *ob)
{
	switch (WD_STAGE)
	{
		case wt_BUBBLES:
			ob->flags &= ~of_shootable;
			if (Chase(ob,true))
			{
				// RISE & GOTO WALK STAGE
				//

				WD_STAGE = wt_WALK;
				WD_TIMEREMAIN = 60*8+BE_Cross_Brandom(60*5);
				ob->state = &s_wet_rise1;
				ob->speed = 2200;
				ob->ticcount = ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for WALK
				//
				if ((WD_TIMEREMAIN-=realtics) < 0)
				{
					// RISE & GOTO WALK STAGE
					//

					WD_STAGE = wt_WALK;
					WD_TIMEREMAIN = 60*8+BE_Cross_Brandom(60*5);
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

			if (Chase(ob,true) || (CheckHandAttack(ob)))

			{
					ob->flags |= WATER_DRAGON_LEAVE;
					WD_STAGE = BE_Cross_Brandom(wt_CORNER3) + 2;
					WD_TIMEREMAIN = 60*2+(BE_Cross_Brandom(6)*60);
					ob->state = &s_wet_bubbles1;
					ob->ticcount = ob->state->tictime;
			}
			else
				if (AngleNearPlayer(ob) != -1)
				{
					ob->state = &s_wet_attack1;
					ob->ticcount = ob->state->tictime;
				}

			else
			{
				// DEC COUNTER - And check for SINK
				//
				if ((WD_TIMEREMAIN-=realtics) < 0)
				{
					// SINK & GOTO BUBBLE STAGE
					//

					WD_STAGE = wt_BUBBLES;
					WD_TIMEREMAIN = 60*2+BE_Cross_Brandom(60*2);
					ob->state = &s_wet_sink1;
					ob->speed = 1200;
					ob->ticcount = ob->state->tictime;
					ob->flags &= ~of_shootable;
				}

			}
			break;
		case wt_CORNER1:
		case wt_CORNER2:
		case wt_CORNER3:
		case wt_CORNER4:
			ob->flags &= ~of_shootable;
			if ((WD_TIMEREMAIN -= realtics) < 0)
			{
				WD_STAGE = wt_BUBBLES;
				ob->flags &= ~WATER_DRAGON_LEAVE;
			}
			else
			{
				fixed tempx,tempy;
				id0_unsigned_t temp_tilex,temp_tiley;

				tempx = player->x;
				tempy = player->y;
				temp_tilex = player->tilex;
				temp_tiley = player->tiley;

				player->x = ((id0_long_t)other_x[WD_STAGE-2]<<TILESHIFT)+TILEGLOBAL/2;
				player->y = ((id0_long_t)other_y[WD_STAGE-2]<<TILESHIFT)+TILEGLOBAL/2;
				player->tilex = other_x[WD_STAGE-2];
				player->tiley = other_y[WD_STAGE-2];


				Chase(ob,true);

				player->x = tempx;
				player->y = tempy;
				player->tilex = temp_tilex;
				player->tiley = temp_tiley;
			}
			break;
	}
}

/*
===============
=
= T_DragonShoot
=
===============
*/
void T_DragonShoot (objtype *ob)
{
	ShootPlayer(ob,dshotobj,10000,&s_dragon_shot1);
}
