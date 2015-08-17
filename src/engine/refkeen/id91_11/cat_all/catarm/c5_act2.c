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
void SpawnSkeleton(id0_int_t tilex, id0_int_t tiley);

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

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



/*
=============================================================================

								SKELETON IN WALL

=============================================================================
*/

void T_WallSkeleton(objtype *ob);

statetype s_wallskel = {0,40,T_WallSkeleton,&s_wallskel};
statetype s_wallskel2 = {0,1,NULL,NULL};


enum wskel_modes {ws_wall1,ws_wall2,ws_wall3,ws_exit};
//enum wskel_modes {ws_wall1,ws_exit};

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
//			case WALL_SKELETON_CODE:
//			case WALL_SKELETON_CODE+1:
//			case WALL_SKELETON_CODE+2:
//				wallmode = ws_wall1+(tile-WALL_SKELETON_CODE);
//				wallbase = WALL_SKELETON_CODE;
//				goto foundtile;
//			break;

			case 66:
			case 68:
//			case 21:
				wallmode = ws_wall1+(tile-66);
				wallbase = 66;
				goto foundtile;
//			break;

			case 67:
			case 69:
				wallmode = ws_wall1+(tile-67);
				wallbase = 67;
				goto foundtile;
//			break;
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
	newobj->hitpoints = 12;

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
			if ((wskel_base == 66) || (wskel_base == 67))
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
			/*(id0_unsigned_t)actorat[x][y]
				=*/ tilemap[x][y]
				= *(mapsegs[0]+farmapylookup[y]+x)
				= 21;
//				= wskel_base;
			actorat[x][y] = tilemap[x][y];
			ob->tilex = ob->x >> TILESHIFT;
			ob->tiley = ob->y >> TILESHIFT;

			ob->obclass = skeletonobj;
			ob->speed = 2036;
			ob->flags |= of_shootable;
			ob->hitpoints = 12;
			ob->state = &s_skel_1;
			ob->ticcount = ob->state->tictime;
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

void T_EyeMage (objtype *ob);
id0_boolean_t T_EyeShoot (objtype *ob, id0_boolean_t eyeshot);
void T_EyeShootPlayer (objtype *ob);

extern statetype s_eye_shootplayer_1;
extern statetype s_eye_shootplayer_2;

statetype s_eye_pause = {EYE_WALK1PIC,40,NULL,&s_eye_2};

statetype s_eye_1 = {EYE_WALK1PIC,20,T_EyeMage,&s_eye_2};
statetype s_eye_2 = {EYE_WALK2PIC,20,T_EyeMage,&s_eye_3};
statetype s_eye_3 = {EYE_WALK3PIC,20,T_EyeMage,&s_eye_4};
statetype s_eye_4 = {EYE_WALK2PIC,20,T_EyeMage,&s_eye_1};
statetype s_eye_shootplayer_1 = {EYE_SCOWLPIC,1,T_EyeShootPlayer,&s_eye_shootplayer_2};
statetype s_eye_shootplayer_2 = {EYE_SCOWLPIC,20,NULL,&s_eye_1};

statetype s_eye_ouch = {EYE_OUCH1PIC,8,NULL,&s_eye_ouch2};
statetype s_eye_ouch2 = {EYE_OUCH2PIC,8,NULL,&s_eye_1};

statetype s_eye_die1 = {EYE_DEATH1PIC,22,NULL,&s_eye_die2};
statetype s_eye_die2 = {EYE_DEATH2PIC,22,NULL,&s_eye_die3};
statetype s_eye_die3 = {EYE_DEATH2PIC,22,NULL,NULL};

extern statetype s_eshot2;

statetype s_eshot1 = {EYE_SHOT1PIC,8,&T_ShootPlayer,&s_eshot2};
statetype s_eshot2 = {EYE_SHOT2PIC,8,&T_ShootPlayer,&s_eshot1};

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
	newobj->hitpoints = EasyHitPoints(15);
	eye_mode = em_other1;
}


//---------------------------------------------------------------------------
// T_EyeShootPlayer
//---------------------------------------------------------------------------
void T_EyeShootPlayer (objtype *ob)
{
	ShootPlayer(ob,eshotobj,ESHOTSPEED,&s_eshot1);
}


/*
=============================================================================

					SUCCUBUS

=============================================================================
*/

void T_Succubus (objtype *ob);
void T_SuccubusShot (objtype *ob);

extern statetype s_succubus_pause;
extern statetype s_succubus_walk1;
extern statetype s_succubus_walk2;
extern statetype s_succubus_walk3;
extern statetype s_succubus_walk4;
extern statetype s_succubus_shot1;
extern statetype s_succubus_attack1;
extern statetype s_succubus_attack2;
extern statetype s_succubus_attack3;
extern statetype s_succubus_death1;
extern statetype s_succubus_death2;

statetype s_succubus_pause = {SUCCUBUS_WALK2PIC,10,NULL,&s_succubus_walk3};

statetype s_succubus_walk1 = {SUCCUBUS_WALK1PIC,10,T_EyeMage,&s_succubus_walk2};
statetype s_succubus_walk2 = {SUCCUBUS_WALK2PIC,10,T_EyeMage,&s_succubus_walk3};
statetype s_succubus_walk3 = {SUCCUBUS_WALK3PIC,10,T_EyeMage,&s_succubus_walk4};
statetype s_succubus_walk4 = {SUCCUBUS_WALK4PIC,10,T_EyeMage,&s_succubus_walk1};

statetype s_succubus_attack1 = {SUCCUBUS_ATTACK1PIC,15,NULL,&s_succubus_attack2};
statetype s_succubus_attack2 = {SUCCUBUS_ATTACK1PIC,-1,T_SuccubusShot,&s_succubus_attack3};
statetype s_succubus_attack3 = {SUCCUBUS_ATTACK2PIC,15,NULL,&s_succubus_pause};

statetype s_succubus_ouch = {SUCCUBUS_OUCHPIC,15,NULL,&s_succubus_walk1};

statetype s_succubus_death1 = {SUCCUBUS_DEATH1PIC,55,NULL,&s_succubus_death2};
statetype s_succubus_death2 = {SUCCUBUS_DEATH2PIC,20,NULL,&s_succubus_death2};

statetype s_succubus_shot1 = {SUCCUBUS_SHOT1PIC,12,&T_ShootPlayer,&s_succubus_shot1};

/*
===============
=
= SpawnSuccubus
=
===============
*/

void SpawnSuccubus (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_succubus_walk1,PIXRADIUS*30);
	newobj->obclass = succubusobj;
	newobj->speed = 2500;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(12);
}

/*
===============
=
= T_SuccubusShot
=
===============
*/

void T_SuccubusShot (objtype *ob)
{
	ShootPlayer(ob,sshotobj,ob->temp1 ? MSHOTSPEED : SSHOTSPEED,&s_succubus_shot1);
//	ob->state = &s_succubus_attack3;
//	ob->ticcount = ob->temp1 ? 7 : ob->state->tictime;
}


/*
=============================================================================

							MAGE

=============================================================================
*/


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

statetype s_mage1 = {MAGE1PIC,20,T_EyeMage,&s_mage2};
statetype s_mage2 = {MAGE2PIC,20,T_EyeMage,&s_mage1};

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
	newobj->hitpoints = EasyHitPoints(12);
}


/*
===============
=
= T_EyeMage
=
= **********
= ***NOTE*** This routine controls the thinks for the Eye, Mage, and Succubus.
= **********
=
===============
*/

void T_EyeMage (objtype *ob)
{
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;
	id0_int_t angle;

	eye_delay -= realtics;
	if (eye_delay < 0)
	{
		eye_mode = BE_Cross_Brandom(em_dummy);
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


	if (ob->obclass == mageobj)					// do the mage shot
	{
		if (!BE_Cross_Brandom(10))
			if (ShootPlayer(ob,mshotobj,MSHOTSPEED,&s_mshot1))
			{
				ob->state = &s_mageattack3;
				ob->ticcount = ob->state->tictime;
			}
	}
	else
		if (ob->obclass == succubusobj)				// do the succubus shot
		{
			angle = AngleNearPlayer(ob);        // make sure angle is correct
															//	    - see AngleNearPlayer
			if (!BE_Cross_Brandom(5) && (angle != -1))   // if correct angle and random # attack
			{
				ob->state = &s_succubus_attack1;    // change state to attack
				ob->ticcount = ob->state->tictime;	// init ticcount - otherwise
			}													//  object may get hung in a
		}														//  endless state

		else
		{
			angle = AngleNearPlayer(ob);			// do the eye shot

			if (!BE_Cross_Brandom(2) && (angle != -1))
			{
				ob->state = &s_eye_shootplayer_1;
				ob->ticcount = ob->state->tictime;
			}
		}

}


/*
=============================================================================

					BUNNY

=============================================================================
*/

void T_HarmlessBunnyWalk(objtype *ob);
void T_Bunny(objtype *ob);

extern statetype s_bunny_left1;
extern statetype s_bunny_left2;
extern statetype s_bunny_left3;
extern statetype s_bunny_right1;
extern statetype s_bunny_right2;
extern statetype s_bunny_right3;
extern statetype s_bunny_meta1;
extern statetype s_bunny_meta2;
extern statetype s_bunny_walk1;
extern statetype s_bunny_walk2;
extern statetype s_bunny_attack1;
extern statetype s_bunny_attack2;
extern statetype s_bunny_pause;
extern statetype s_bunny_death1;
extern statetype s_bunny_death2;
extern statetype s_bunny_death3;

statetype s_bunny_left1 = {BUNNY_LEFT1PIC, 55, NULL, &s_bunny_left2};
statetype s_bunny_left2 = {BUNNY_LEFT1PIC, 10, T_HarmlessBunnyWalk, &s_bunny_left1};
statetype s_bunny_left3 = {BUNNY_LEFT2PIC, 30, NULL, &s_bunny_left1};

statetype s_bunny_right1 = {BUNNY_RIGHT1PIC, 55, NULL, &s_bunny_right2};
statetype s_bunny_right2 = {BUNNY_RIGHT1PIC, 10, T_HarmlessBunnyWalk, &s_bunny_right1};
statetype s_bunny_right3 = {BUNNY_RIGHT2PIC, 30, NULL, &s_bunny_right1};

statetype s_bunny_meta1 = {BUNNY_META1PIC, 30, NULL, &s_bunny_meta2};
statetype s_bunny_meta2 = {BUNNY_META2PIC, 30, NULL, &s_bunny_walk1};

statetype s_bunny_walk1 = {BUNNY_WALK1PIC, 25, T_Bunny, &s_bunny_walk2};
statetype s_bunny_walk2 = {BUNNY_WALK2PIC, 25, T_Bunny, &s_bunny_walk1};

statetype s_bunny_attack1 = {BUNNY_WALK1PIC, 25, NULL, &s_bunny_attack2};
statetype s_bunny_attack2 = {BUNNY_WALK2PIC, 25, T_DoDamage, &s_bunny_walk1};

statetype s_bunny_ouch = {BUNNY_OUCHPIC, 30, NULL, &s_bunny_pause};
statetype s_bunny_pause = {BUNNY_WALK1PIC, 50, T_Bunny, &s_bunny_walk2};

statetype s_bunny_death1 = {BUNNY_OUCHPIC, 40, NULL, &s_bunny_death2};
statetype s_bunny_death2 = {BUNNY_DEATH1PIC, 50, NULL, &s_bunny_death3};
statetype s_bunny_death3 = {BUNNY_DEATH2PIC, 20, NULL, &s_bunny_death3};


#define bunny_dir_hop	ob->temp1
#define bunny_delay	ob->temp2
#define LEFTSIDE	0x8             // 1=left 0=right --side showing

/*
===============
=
= SpawnBunny
=
===============
*/

void SpawnBunny (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_bunny_left1,PIXRADIUS*35);
	newobj->obclass =	hbunnyobj;
	newobj->speed = 1947;
	newobj->temp1 = (BE_Cross_Brandom(3))+2;
	newobj->temp2 = BE_Cross_Brandom(30);
	newobj->flags &= ~of_shootable;
	newobj->flags |= LEFTSIDE;				//left side showing}
}

/*
===============
=
= T_HarmlessBunnyWalk
=
===============
*/


void T_HarmlessBunnyWalk(objtype *ob)
{
	// REFKEEN:
	// 1. Convert to dirtype for C++ (FIXME: Use dir enum names)
	// 2. There can be a buffer overflow with valid_dir[player_dir][2] when
	// when player_dir == (dirtype)7, but luckily it looks like it's just
	// the boolean variable InLoadSaveGame from gelib.c (as of v1.02),
	// always set to false.
	dirtype valid_dir[9/*8*/][2] = {{(dirtype)6,(dirtype)5}, {(dirtype)7,(dirtype)6}, {(dirtype)4,(dirtype)7}, {(dirtype)5,(dirtype)4}, {(dirtype)3,(dirtype)2}, {(dirtype)0,(dirtype)3}, {(dirtype)1,(dirtype)0}, {(dirtype)2,(dirtype)1},
		{(dirtype)0, (dirtype)0} // Buffer overflow workaround
	};
	//id0_int_t valid_dir[8][2] = {{6,5}, {7,6}, {4,7}, {5,4}, {3,2}, {0,3}, {1,0}, {2,1}};
	id0_long_t move;
	dirtype player_dir;
	fixed old_x, old_y;
	id0_unsigned_t old_tilex, old_tiley;
	id0_long_t old_distance;


	ob->temp2 -= realtics;
	if (ob->temp2 <= 0)
	{
		if (CheckHandAttack(ob))
		{
			ob->temp2 = -1;
			return;
		}

		actorat[ob->tilex][ob->tiley] = 0;
		ob->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
		ob->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
		ob->distance = TILEGLOBAL;
		ob->state = &s_bunny_meta1;
		ob->ticcount = ob->state->tictime;
		ob->obclass = bunnyobj;
		ob->flags |= of_shootable;
		ob->hitpoints = EasyHitPoints(10);
		ob->dir = nodir;
		ChaseThink(ob,true);					// JTR - testing..
		return;
	}

	// The direction of the player isn't updated so it must be
	// calculated.  This is done so the correct side (left/right)
	// of the bunny will be showed.

	if ((player->angle > 337) || (player->angle <= 22))
		player_dir = east;
	else
		if (player->angle <= 67)
			player_dir = northeast;
		else
			if (player->angle <= 112)
		 player_dir = north;
			else
		 if (player->angle <= 157)
			 player_dir = northwest;
		 else
			 if (player->angle <= 202)
				 player_dir = west;
			 else
				 if (player->angle <= 247)
			  player_dir = southwest;
				 else
			 if (player->angle <= 292)
				 player_dir = south;
			 else
				 if (player->angle <= 337)
					 player_dir = southeast;
	if (ob->temp1)
		ob->temp1--;
	else
		ob->temp1 = (BE_Cross_Brandom(3))+2;
	if (ob->flags & LEFTSIDE)
	{
		if (ob->temp1)
		{
			if (valid_dir[player_dir][0] != ob->dir)
			{
				ob->dir = valid_dir[player_dir][0];
			}
		}
		else
		{
			ob->state = &s_bunny_right1;
			ob->ticcount = ob->state->tictime;
			ob->flags &= ~LEFTSIDE;
			ob->dir = valid_dir[player_dir][1];
			return;
		}
	}
	else
	{
		if (ob->temp1)
		{
			if (valid_dir[player_dir][1] != ob->dir)
			{
				ob->dir = valid_dir[player_dir][1];
			}
		}
		else
		{
			ob->state = &s_bunny_left1;
			ob->ticcount = ob->state->tictime;
			ob->flags |= LEFTSIDE;
			// REFKEEN - Following the buffer overflow fix above
			// the original source line may be OK, but better
			// change it anyway (should have same behaviors)
			ob->dir = valid_dir[player_dir+1][0];
			//ob->dir = valid_dir[player_dir][2];
			return;
		}
	}

	move = ob->speed*tics;

	do
	{
		old_distance	= ob->distance;
		old_x		= ob->x;
		old_y		= ob->y;
		old_tilex	= ob->tilex;
		old_tiley	= ob->tiley;

		MoveObj (ob, move);

		ob->tilex = ob->x >> TILESHIFT;
		ob->tiley = ob->y >> TILESHIFT;

		if (ob->tilex == old_tilex && ob->tiley == old_tiley)
		{
			break;
		}
		else
			if (actorat[ob->tilex][ob->tiley] == 0)
			{
				actorat[old_tilex][old_tiley] = 0;
				actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);
				//actorat[ob->tilex][ob->tiley] = ob;
				ob->distance = TILEGLOBAL;
			}
			else
			{
				ob->distance	= old_distance;
				ob->x		= old_x;
				ob->y		= old_y;
				ob->tilex	= old_tilex;
				ob->tiley	= old_tiley;
				return;
			}

	 } while (0);

	CalcBounds (ob);

	if (ob->flags & LEFTSIDE)
		ob->state = &s_bunny_left3;
	else
		ob->state = &s_bunny_right3;
	ob->ticcount = ob->state->tictime;
}

/*
===============
=
= T_Bunny
=
===============
*/

void T_Bunny(objtype *ob)
{
	if (Chase (ob, true) || (BE_Cross_Brandom(1000)<RANDOM_ATTACK))
	{
		ob->state = &s_bunny_attack1;
		ob->ticcount = ob->state->tictime;
		return;
	}
}
