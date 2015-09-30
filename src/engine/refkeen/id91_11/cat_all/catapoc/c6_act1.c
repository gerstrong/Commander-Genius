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

statetype s_boltbonus = {BOLTOBJPIC,8,NULL,&s_boltbonus2};
statetype s_boltbonus2 = {BOLT2OBJPIC,8,NULL,&s_boltbonus3};
statetype s_boltbonus3 = {BOLT3OBJPIC,8,NULL,&s_boltbonus};

statetype s_nukebonus = {NUKEOBJPIC,8,NULL,&s_nukebonus2};
statetype s_nukebonus2 = {NUKE2OBJPIC,8,NULL,&s_nukebonus3};
statetype s_nukebonus3 = {NUKE3OBJPIC,8,NULL,&s_nukebonus};

statetype s_potionbonus = {POTIONOBJPIC,0,NULL,&s_potionbonus};
statetype s_rkeybonus = {RKEYOBJPIC,0,NULL,&s_rkeybonus};
statetype s_ykeybonus = {YKEYOBJPIC,0,NULL,&s_ykeybonus};
statetype s_gkeybonus = {GKEYOBJPIC,0,NULL,&s_gkeybonus};
statetype s_bkeybonus = {BKEYOBJPIC,0,NULL,&s_bkeybonus};
statetype s_chestbonus = {CHESTOBJPIC,0,NULL,&s_chestbonus};
statetype s_oldchestbonus = {OLD_CHESTPIC,0,NULL,&s_oldchestbonus};

statetype s_waterchestbonus1 = {O_WATER_CHEST1PIC, 10, NULL, &s_waterchestbonus2};
statetype s_waterchestbonus2 = {O_WATER_CHEST2PIC, 10, NULL, &s_waterchestbonus3};
statetype s_waterchestbonus3 = {O_WATER_CHEST3PIC, 10, NULL, &s_waterchestbonus1};

statetype s_rgem1bonus = {RGEM1PIC,30,NULL,&s_rgem1bonus};
statetype s_ygem1bonus = {YGEM1PIC,30,NULL,&s_ygem1bonus};
statetype s_ggem1bonus = {GGEM1PIC,30,NULL,&s_ggem1bonus};
statetype s_bgem1bonus = {BGEM1PIC,30,NULL,&s_bgem1bonus};
statetype s_pgem1bonus = {PGEM1PIC,30,NULL,&s_pgem1bonus};

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

		case B_OLDCHEST:		state = &s_oldchestbonus;	break;


		default:
			Quit("SpawnBonus(): INVALID BONUS");
		break;
	}

	SpawnNewObj (tilex,tiley,state,TILEGLOBAL/2);
	newobj->temp1 = number;
	newobj->obclass = bonusobj;

	switch (number)
	{
		case B_POTION:
		case B_OLDCHEST:
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
void T_Gate_Wait (objtype *ob);

extern statetype s_portal_wait;
statetype s_portal_wait = {0, 10, &T_Gate_Wait, &s_portal_wait};

statetype s_portal1 = {PORTAL1PIC, 6, &T_Gate, &s_portal2};
statetype s_portal2 = {PORTAL2PIC, 6, &T_Gate, &s_portal3};
statetype s_portal3 = {PORTAL3PIC, 6, &T_Gate, &s_portal4};
statetype s_portal4 = {PORTAL4PIC, 6, &T_Gate, &s_portal5};
statetype s_portal5 = {PORTAL5PIC, 6, &T_Gate, &s_portal6};
statetype s_portal6 = {PORTAL6PIC, 6, &T_Gate, &s_portal1};

//---------------------------------------------------------------------------
//	SpawnWarp()
//---------------------------------------------------------------------------
void SpawnWarp (id0_int_t tilex, id0_int_t tiley)
{
	id0_unsigned_t spot;
	//objtype *ob;

	spot = (*(mapsegs[2]+farmapylookup[tiley]+tilex+1)) >> 8;

	if (spot)
	{
		SpawnNewObj (tilex, tiley, &s_portal_wait, TILEGLOBAL/3);
		newobj->temp1 = spot*70;
	}
	else
		SpawnNewObj (tilex, tiley, &s_portal1, TILEGLOBAL/3);

	newobj->obclass = gateobj;
}

/*
===============
=
= T_Gate_Wait
=
===============
*/

void T_Gate_Wait (objtype *ob)
{
	if ((ob->temp1 -= tics) <= 0)
	{
		if ((ob->tilex == player->tilex) && (ob->tiley == player->tiley))
			return;
		if (CheckHandAttack(ob))
			return;

		SD_PlaySound(PORTALSND);
		ob->state = &s_portal1;
		ob->ticcount = ob->state->tictime;

	}

}



/*
===============
=
= T_Gate
=
===============
*/

void T_Gate (objtype *ob)
{
	//objtype *check;
	id0_unsigned_t	/*temp,*/spot;

	if (CheckHandAttack (ob) && !playstate)
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



/*
=============================================================================

											AQUAMAN

=============================================================================
*/

void T_AquaMan(objtype *ob);

statetype s_aqua_under1 = {EYESTALKUNDER1PIC, 25, &T_AquaMan, &s_aqua_under2};
statetype s_aqua_under2 = {EYESTALKUNDER2PIC, 20, &T_AquaMan, &s_aqua_under3};
statetype s_aqua_under3 = {EYESTALKUNDER3PIC, 20, &T_AquaMan, &s_aqua_under2};

statetype s_aqua_left = {EYESTALKUNDER4PIC, 40, NULL, &s_aqua_under3};
statetype s_aqua_right = {EYESTALKUNDER5PIC, 40, NULL, &s_aqua_under3};

statetype s_aqua_rise1 = {EYESTALKRISE1PIC, 20, NULL, &s_aqua_rise2};
statetype s_aqua_rise2 = {EYESTALKRISE2PIC, 15, NULL, &s_aqua_walk1};

statetype s_aqua_sink1 = {EYESTALKRISE2PIC, 15, NULL, &s_aqua_sink2};
statetype s_aqua_sink2 = {EYESTALKRISE1PIC, 20, NULL, &s_aqua_under1};

statetype s_aqua_walk1 = {EYESTALKWALK1PIC, 12, &T_AquaMan, &s_aqua_walk2};
statetype s_aqua_walk2 = {EYESTALKWALK2PIC, 12, &T_AquaMan, &s_aqua_walk1};

statetype s_aqua_attack1 = {EYESTALKATTACKPIC, 10, NULL, &s_aqua_attack2};
statetype s_aqua_attack2 = {EYESTALKWALK1PIC, 10, &T_DoDamage, &s_aqua_walk1};

statetype s_aqua_die1 = {EYESTALKDEATH1PIC, 8, NULL, &s_aqua_die2};
statetype s_aqua_die2 = {EYESTALKDEATH2PIC, 8, NULL, &s_aqua_die3};
statetype s_aqua_die3 = {EYESTALKDEATH2PIC, -1, &T_AlternateStates, &s_aqua_die1};
statetype s_aqua_die4 = {EYESTALKDEATH2PIC, 30, NULL, &s_aqua_die5};
statetype s_aqua_die5 = {EYESTALKDEATH3PIC, 40, NULL, &s_aqua_die6};
statetype s_aqua_die6 = {EYESTALKDEATH4PIC, 30, &ExplosionSnd, &s_aqua_die7};
statetype s_aqua_die7 = {EYESTALKDEATH5PIC, 20, NULL, NULL};

typedef enum {wt_UNDER, wt_WALK} AquaManTypes;

#define AQ_TIMEREMAIN 	(ob->temp1)
#define AQ_STAGE			(ob->temp2)

/*
===============
=
= SpawnAquaMan
=
===============
*/
void SpawnAquaMan(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	SpawnNewObj(tilex,tiley,&s_aqua_under1,PIXRADIUS*32);
	ob = newobj;

	AQ_STAGE = wt_UNDER;
	AQ_TIMEREMAIN = 60*4+BE_Cross_Brandom(60*3);

	newobj->obclass = aquamanobj;
	newobj->speed = 1000;
	newobj->flags &= ~of_shootable;
	newobj->hitpoints = EasyHitPoints(15);
}

void ExplosionSnd(objtype *ob)
{
	if (ob->temp1 != SOUNDPLAYED)
	{
		SD_PlaySound(BODY_EXPLODESND);
		ob->temp1 = SOUNDPLAYED;

	}
}


/*
===============
=
= T_AquaMan
=
===============
*/

void T_AquaMan(objtype *ob)
{
	switch (AQ_STAGE)
	{
		case wt_UNDER:
			ob->flags &= ~of_shootable;
			if (Chase(ob,true))
			{
				// RISE & GOTO WALK STAGE
				//

				AQ_STAGE = wt_WALK;
				AQ_TIMEREMAIN = 60*5+BE_Cross_Brandom(60*5);
					ob->state = &s_aqua_rise1;
				ob->speed = 2200;
				ob->ticcount = ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for WALK
				//
				if ((AQ_TIMEREMAIN-=realtics) < 0)
				{
					// RISE & GOTO WALK STAGE
					//

					if (CheckHandAttack(ob))
						break;

					AQ_STAGE = wt_WALK;
					AQ_TIMEREMAIN = 60+BE_Cross_Brandom(60*2);
						ob->state = &s_aqua_rise1;
					ob->speed = 2200;
					ob->ticcount = ob->state->tictime;
				}
				else
				if (BE_Cross_Brandom(1000)<5)
				{
					// RANDOM PEEK UP OUT OF WATER
					//
						if (BE_Cross_Brandom(2) == 0)
							ob->state = &s_aqua_left;
						else
							ob->state = &s_aqua_right;
					ob->ticcount = ob->state->tictime;
				}
			}
			break;


		case wt_WALK:
			ob->flags |= of_shootable;
			if (Chase(ob,true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
			{
					ob->state = &s_aqua_attack1;
				ob->ticcount = ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for SINK
				//
				if ((AQ_TIMEREMAIN-=realtics) < 0)
				{
					// SINK & GOTO BUBBLE STAGE
					//

					AQ_STAGE = wt_UNDER;
					AQ_TIMEREMAIN = 60*4+BE_Cross_Brandom(60*3);
						ob->state = &s_aqua_sink1;
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

											WIZARD

=============================================================================
*/

void T_Wizard(objtype *ob);
void T_WizardShoot(objtype *ob);

statetype s_wizard_walk1 = {WIZARDWALK1PIC, 20, &T_Wizard, &s_wizard_walk2};
statetype s_wizard_walk2 = {WIZARDWALK2PIC, 20, &T_Wizard, &s_wizard_walk3};
statetype s_wizard_walk3 = {WIZARDWALK3PIC, 20, &T_Wizard, &s_wizard_walk4};
statetype s_wizard_walk4 = {WIZARDWALK4PIC, 20, &T_Wizard, &s_wizard_walk1};

statetype s_wizard_attack1 = {WIZARDATTACK1PIC, 20, NULL, &s_wizard_attack2};
statetype s_wizard_attack2 = {WIZARDATTACK2PIC, 20, &T_DoDamage, &s_wizard_walk1};

statetype s_wizard_ouch = {WIZARDOUCHPIC, 15, NULL, &s_wizard_walk1};

statetype s_wizard_die1 = {WIZARDDEATH1PIC, 45, &SmallSound, &s_wizard_die2};
statetype s_wizard_die2 = {WIZARDDEATH2PIC, 30, NULL, &s_wizard_die3};
statetype s_wizard_die3 = {WIZARDDEATH3PIC, 15, NULL, &s_wizard_die4};
statetype s_wizard_die4 = {WIZARDDEATH4PIC, 15, NULL, &s_wizard_die4};

statetype s_wizard_shoot1 = {WIZARDATTACK1PIC, 20, NULL, &s_wizard_shoot2};
statetype s_wizard_shoot2 = {WIZARDATTACK1PIC, -1, &T_WizardShoot, &s_wizard_shoot3};
statetype s_wizard_shoot3 = {WIZARDATTACK2PIC, 20, NULL, &s_wizard_walk1};

statetype s_wizard_shot1 = {WIZARD_SHOT1PIC, 8, &T_ShootPlayer, &s_wizard_shot2};
statetype s_wizard_shot2 = {WIZARD_SHOT2PIC, 8, &T_ShootPlayer, &s_wizard_shot1};


/*
===============
=
= SpawnWizard
=
===============
*/

void SpawnWizard (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_wizard_walk1,TILEGLOBAL/2);
	newobj->obclass	= wizardobj;
	newobj->speed		= 1536;
	newobj->flags |= of_shootable;
	newobj->hitpoints	= EasyHitPoints(10);
}


/*
===============
=
= T_Wizard
=
===============
*/

void T_Wizard(objtype *ob)
{
	if (Chase (ob,true))// || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_wizard_attack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
	else
		if (AngleNearPlayer(ob) != -1)
		{
			ob->state = &s_wizard_shoot1;
			ob->ticcount = ob->state->tictime;
			return;
		}
}

/*
===============
=
= T_Wizard
=
===============
*/
void T_WizardShoot(objtype *ob)
{
	ShootPlayer(ob, wshotobj, 10000, &s_wizard_shot1);
}



/*
=============================================================================

											RAY

=============================================================================
*/

void T_BlobRay(objtype *ob);
void T_RayShoot (objtype *ob);

statetype s_ray_under = {0, 20, &T_BlobRay, &s_ray_under};

statetype s_ray_rise = {RAYRISEPIC, 30, NULL, &s_ray_fly1};

statetype s_ray_sink = {RAYRISEPIC, 30, NULL, &s_ray_under};

statetype s_ray_fly1 = {RAYFLY1PIC, 10, &T_BlobRay, &s_ray_fly2};
statetype s_ray_fly2 = {RAYFLY2PIC, 10, &T_BlobRay, &s_ray_fly3};
statetype s_ray_fly3 = {RAYFLY1PIC, 10, &T_BlobRay, &s_ray_fly4};
statetype s_ray_fly4 = {RAYFLY3PIC, 10, &T_BlobRay, &s_ray_fly1};

statetype s_ray_attack1 = {RAYSHOOT1PIC, 15, NULL, &s_ray_attack2};
statetype s_ray_attack2 = {RAYSHOOT2PIC, -1, &T_RayShoot, &s_ray_attack3};
statetype s_ray_attack3 = {RAYSHOOT2PIC, 20, NULL, &s_ray_fly1};

statetype s_ray_die1 = {RAYDEATH1PIC, 50, &SmallSound, &s_ray_die2};
statetype s_ray_die2 = {RAYDEATH2PIC, 30, NULL, NULL};

statetype s_ray_shot1 = {RAYSHOT1PIC, 8, &T_ShootPlayer, &s_ray_shot2};
statetype s_ray_shot2 = {RAYSHOT2PIC, 8, &T_ShootPlayer, &s_ray_shot1};


typedef enum {br_GND, br_WALK, br_CORNER1, br_CORNER2, br_CORNER3, br_CORNER4} BlobTypes;

#define BR_TIMEREMAIN	(ob->temp1)
#define BR_STAGE			(ob->temp2)
#define BLOB_LEAVE		0x04

/*
===============
=
= SpawnRay
=
===============
*/
void SpawnRay(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	SpawnNewObj(tilex, tiley, &s_ray_under, PIXRADIUS*25);
	ob=newobj;

	BR_STAGE = br_GND;
	// REFKEEN - Force a specific order of calls to random function
	// (same as original EXEs for DOS), don't introduce undefined behaviors
	BR_TIMEREMAIN = BE_Cross_Brandom(60);
	BR_TIMEREMAIN += BE_Cross_Brandom(100);
	//BR_TIMEREMAIN = random(60)+random(100);

	newobj->obclass	= rayobj;
	newobj->speed		= 1700;
	newobj->flags	&= ~of_shootable;
	newobj->hitpoints	= EasyHitPoints(15);
}



/*
=============================================================================

										BLOB

=============================================================================
*/


statetype s_blob_gnd1 = {BLOBGND1PIC, 13, T_BlobRay, &s_blob_gnd2};
statetype s_blob_gnd2 = {BLOBGND2PIC, 15, T_BlobRay, &s_blob_gnd1};

statetype s_blob_rise1 = {BLOBRISE1PIC, 20, NULL, &s_blob_rise2};
statetype s_blob_rise2 = {BLOBRISE2PIC, 20, NULL, &s_blob_walk1};

statetype s_blob_sink1 = {BLOBRISE2PIC, 20, NULL, &s_blob_sink2};
statetype s_blob_sink2 = {BLOBRISE1PIC, 20, NULL, &s_blob_gnd1};

statetype s_blob_walk1 = {BLOBWALK1PIC, 15, T_BlobRay, &s_blob_walk2};
statetype s_blob_walk2 = {BLOBWALK2PIC, 15, T_BlobRay, &s_blob_walk3};
statetype s_blob_walk3 = {BLOBWALK3PIC, 15, T_BlobRay, &s_blob_walk1};

statetype s_blob_ouch = {BLOBRISE2PIC, 10, T_BlobRay, &s_blob_walk1};

statetype s_blob_die1 = {BLOBDEATH1PIC, 30, &ExplosionSnd, &s_blob_die2};
statetype s_blob_die2 = {BLOBDEATH2PIC, 30, NULL, &s_blob_die3};
statetype s_blob_die3 = {BLOBDEATH3PIC, 30, NULL, NULL};

statetype s_blob_shot1 = {BLOB_SHOT1PIC, 8, &T_ShootPlayer, &s_blob_shot2};
statetype s_blob_shot2 = {BLOB_SHOT2PIC, 8, &T_ShootPlayer, &s_blob_shot1};


/*
===============
=
= SpawnBlob
=
===============
*/
void SpawnBlob(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	SpawnNewObj(tilex, tiley, &s_blob_gnd1, PIXRADIUS*14);
	ob=newobj;

	BR_STAGE = br_GND;
	// REFKEEN - Force a specific order of calls to random function
	// (same as original EXEs for DOS), don't introduce undefined behaviors
	BR_TIMEREMAIN = BE_Cross_Brandom(60);
	BR_TIMEREMAIN += BE_Cross_Brandom(100);
	//BR_TIMEREMAIN = random(60)+random(100);

	newobj->obclass	= blobobj;
	newobj->speed		= 1200;
	newobj->flags	&= ~of_shootable;
	newobj->hitpoints	= EasyHitPoints(13);
}


/*
===============
=
= T_BlobRay
=
===============
*/

void T_BlobRay(objtype *ob)
{
	switch (BR_STAGE)
	{
		case br_GND:
			ob->flags &= ~of_shootable;
			if (Chase(ob,true))
			{
				// RISE & GOTO WALK STAGE
				//

				BR_STAGE			= br_WALK;
				BR_TIMEREMAIN	= 60*8+BE_Cross_Brandom(60*5);
				if (ob->obclass == blobobj)
					ob->state	= &s_blob_rise1;
				else
					ob->state	= &s_ray_rise;
				ob->speed		= 2200;
				ob->ticcount	= ob->state->tictime;
			}
			else
			{
				// DEC COUNTER - And check for WALK
				//
				if ((BR_TIMEREMAIN -= realtics) < 0)
				{
					// RISE & GOTO WALK STAGE
					//

					BR_STAGE			= br_WALK;
					BR_TIMEREMAIN	= 60*8+BE_Cross_Brandom(60*5);
					if (ob->obclass == blobobj)
						ob->state	= &s_blob_rise1;
					else
						ob->state	= &s_ray_rise;
					ob->speed		= 2200;
					ob->ticcount	= ob->state->tictime;
				}
			}
			break;


		case br_WALK:
			ob->flags |= of_shootable;

			if (Chase(ob,true) || (CheckHandAttack(ob)))

			{
					ob->flags		|= BLOB_LEAVE;
					BR_STAGE			= BE_Cross_Brandom(br_CORNER3) + 2;
					BR_TIMEREMAIN	= 60*2+(BE_Cross_Brandom(6)*60);
					if (ob->obclass == blobobj)
						ob->state	= &s_blob_gnd1;
					else
						ob->state	= &s_ray_under;
					ob->ticcount	= ob->state->tictime;
			}
			else
				if (AngleNearPlayer(ob) != -1)
				{
					if (ob->obclass == blobobj)
					{
						if (!(BE_Cross_Brandom(15)))
								ShootPlayer(ob, bshotobj, 10000, &s_blob_shot1);
					}
					else
						if (!(BE_Cross_Brandom(7)))
						{
							ob->state = &s_ray_attack1;
							ob->ticcount = ob->state->tictime;
						}
				}

			else
			{
				// DEC COUNTER - And check for SINK
				//
				if ((BR_TIMEREMAIN -= realtics) < 0)
				{
					// SINK & GOTO GROUND STAGE
					//

					BR_STAGE			= br_GND;
					BR_TIMEREMAIN	= 60*2+BE_Cross_Brandom(60*2);
					if (ob->obclass == blobobj)
					{
						ob->state		= &s_blob_sink1;
						ob->speed		= 1200;
					}
					else
					{
						ob->state		= &s_ray_sink;
						ob->speed		= 1700;
					}
					ob->ticcount	= ob->state->tictime;
					ob->flags		&= ~of_shootable;
				}

			}
			break;
		case br_CORNER1:
		case br_CORNER2:
		case br_CORNER3:
		case br_CORNER4:
			ob->flags &= ~of_shootable;
			if ((BR_TIMEREMAIN -= realtics) < 0)
			{
				BR_STAGE = br_GND;
				ob->flags &= ~BLOB_LEAVE;
			}
			else
			{
				fixed tempx,tempy;
				id0_unsigned_t temp_tilex,temp_tiley;

				tempx			= player->x;
				tempy			= player->y;
				temp_tilex	= player->tilex;
				temp_tiley	= player->tiley;

				player->x = ((id0_long_t)other_x[BR_STAGE-2]<<TILESHIFT)+TILEGLOBAL/2;
				player->y = ((id0_long_t)other_y[BR_STAGE-2]<<TILESHIFT)+TILEGLOBAL/2;
				player->tilex = other_x[BR_STAGE-2];
				player->tiley = other_y[BR_STAGE-2];


				Chase(ob,true);

				player->x		= tempx;
				player->y		= tempy;
				player->tilex	= temp_tilex;
				player->tiley	= temp_tiley;
			}
			break;
	}
}

/*
===============
=
= T_RayShoot
=
===============
*/
void T_RayShoot (objtype *ob)
{
	ShootPlayer(ob, rshotobj, 10000, &s_ray_shot1);
}
