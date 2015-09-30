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

						 GLOBAL VARIABLES

=============================================================================
*/

// REFKEEN - Use classtype instead of short for obclass
id0_boolean_t ShootPlayer (objtype *ob, classtype/*id0_short_t*/ obclass, id0_short_t speed, statetype *state);
void T_ShootPlayer(objtype *ob);

id0_short_t head_base_delay;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


void T_ShooterObj(objtype *ob);

void SpawnRamBone(id0_int_t tilex, id0_int_t tiley);
void T_SkeletonShoot(objtype *ob);
void SpawnFutureMage (id0_int_t tilex, id0_int_t tiley);
void T_FMageShoot(objtype *ob);
void SpawnRoboTank(id0_int_t tilex, id0_int_t tiley);
void T_RoboTankShoot(objtype *ob);
void SpawnStompy(id0_int_t tilex, id0_int_t tiley);
void T_StompyShoot(objtype *ob);
void SpawnBug(id0_int_t tilex, id0_int_t tiley);
void T_BugShoot(objtype *ob);
void SpawnShooterEye(id0_int_t tilex, id0_int_t tiley);
void T_EyeShootPlayer(objtype *ob);
void SpawnRunningEye(id0_int_t tilex, id0_int_t tiley);
void T_RunningEye(objtype *ob);


/*
=============================================================================

										LARGE SOUND

=============================================================================
*/
void LargeSound (objtype *ob)
{
	if (ob->temp1 != SOUNDPLAYED)
	{
		SD_PlaySound(LARGEMONSTERSND);
		ob->temp1 = SOUNDPLAYED;

	}
}


/*
=============================================================================

										SMALL SOUND

=============================================================================
*/
void SmallSound (objtype *ob)
{
	if (ob->temp1 != SOUNDPLAYED)
	{
		SD_PlaySound(SMALLMONSTERSND);
		ob->temp1 = SOUNDPLAYED;

	}
}



/*
=============================================================================

										RAMBONE

=============================================================================
*/


statetype s_skel_1 = {RAMBONEWALK1PIC, 10, &T_ShooterObj, &s_skel_2};
statetype s_skel_2 = {RAMBONEWALK2PIC, 10, &T_ShooterObj, &s_skel_3};
statetype s_skel_3 = {RAMBONEWALK3PIC, 10, &T_ShooterObj, &s_skel_4};
statetype s_skel_4 = {RAMBONEWALK4PIC, 10, &T_ShooterObj, &s_skel_1};

statetype s_skel_attack1 = {RAMBONEATTACK1PIC, 12, NULL, &s_skel_attack2};
statetype s_skel_attack2 = {RAMBONEATTACK2PIC, 20, NULL, &s_skel_attack3};
statetype s_skel_attack3 = {RAMBONEATTACK2PIC, -1, T_SkeletonShoot, &s_skel_attack4};
statetype s_skel_attack4 = {RAMBONEATTACK3PIC, 20, NULL, &s_skel_ouch};

statetype s_skel_ouch = {RAMBONEATTACK1PIC, 10, NULL, &s_skel_1};

statetype s_skel_die1 = {RAMBONEDEATH1PIC, 40, NULL, &s_skel_die2};
statetype s_skel_die2 = {RAMBONEDEATH2PIC, 30, NULL, &s_skel_die3};
statetype s_skel_die3 = {RAMBONEDEATH3PIC, 20, &LargeSound, NULL};

statetype s_skel_shot1 = {RAMBONESHOT1PIC, 10, &T_ShootPlayer, &s_skel_shot2};
statetype s_skel_shot2 = {RAMBONESHOT2PIC, 10, &T_ShootPlayer, &s_skel_shot1};

#define shooter_mode		ob->temp1
#define shooter_delay	ob->temp2


/*
===============
=
= SpawnSkeleton
=
===============
*/
void SpawnRamBone(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex, tiley, &s_skel_1,PIXRADIUS*20);
	newobj->obclass	= ramboneobj;
	newobj->speed		= 2036;
	newobj->flags		|= of_shootable;
	newobj->hitpoints	= EasyHitPoints(12);
}


/*
=================
=
= T_SkeletonShoot
=
=================
*/
void T_SkeletonShoot(objtype *ob)
{
	ShootPlayer(ob, rbshotobj, MSHOTSPEED, &s_skel_shot1);
}



/*
=============================================================================

										FUTURE MAGE

=============================================================================
*/

statetype s_fmage1 = {FMAGEWALK1PIC, 20, &T_ShooterObj, &s_fmage2};
statetype s_fmage2 = {FMAGEWALK2PIC, 20, &T_ShooterObj, &s_fmage3};
statetype s_fmage3 = {FMAGEWALK3PIC, 20, &T_ShooterObj, &s_fmage1};

statetype s_fmageattack1 = {FMAGEATTACK1PIC, 20, NULL, &s_fmageattack2};
statetype s_fmageattack2 = {FMAGEATTACK1PIC, -1, &T_FMageShoot, &s_fmageattack3};
statetype s_fmageattack3 = {FMAGEATTACK2PIC, 30, NULL, &s_fmage1};

statetype s_fmageouch = {FMAGEATTACK1PIC, 10, NULL, &s_fmage1};

statetype s_fmagedie1 = {FMAGEDEATH1PIC, 40, NULL, &s_fmagedie2};
statetype s_fmagedie2 = {FMAGEDEATH2PIC, 30, &SmallSound, &s_fmagedie3};
statetype s_fmagedie3 = {FMAGEDEATH3PIC, 0, NULL, &s_fmagedie3};

statetype s_fmshot1 = {FMAGESHOT1PIC, 8, &T_ShootPlayer, &s_fmshot2};
statetype s_fmshot2 = {FMAGESHOT2PIC, 8, &T_ShootPlayer, &s_fmshot1};

/*
=================
=
= SpawnFutureMage
=
=================
*/

void SpawnFutureMage (id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex, tiley, &s_fmage1, PIXRADIUS*15);
	newobj->obclass	= fmageobj;
	newobj->speed		= 3072;
	newobj->flags		|= of_shootable;
	newobj->hitpoints	= EasyHitPoints(12);
}


/*
=================
=
= T_FMageShoot
=
=================
*/
void T_FMageShoot(objtype *ob)
{
	ShootPlayer(ob, fmshotobj, MSHOTSPEED, &s_fmshot1);
}



/*
=============================================================================

										ROBO TANK

=============================================================================
*/

statetype s_robotank_walk1 = {ROBOTANKWALK1PIC, 15, &T_ShooterObj, &s_robotank_walk2};
statetype s_robotank_walk2 = {ROBOTANKWALK2PIC, 15, &T_ShooterObj, &s_robotank_walk3};
statetype s_robotank_walk3 = {ROBOTANKWALK3PIC, 15, &T_ShooterObj, &s_robotank_walk4};
statetype s_robotank_walk4 = {ROBOTANKWALK4PIC, 15, &T_ShooterObj, &s_robotank_walk1};

statetype s_robotank_attack1 = {ROBOTANKWALK1PIC, 15, NULL, &s_robotank_attack2};
statetype s_robotank_attack2 = {ROBOTANKATTACK1PIC, 15, NULL, &s_robotank_attack3};
statetype s_robotank_attack3 = {ROBOTANKATTACK1PIC, -1, &T_RoboTankShoot, &s_robotank_attack4};
statetype s_robotank_attack4 = {ROBOTANKWALK1PIC, 15, NULL, &s_robotank_walk1};

statetype s_robotank_death1 = {ROBOTANKDEATH1PIC, 8, NULL, &s_robotank_death2};
statetype s_robotank_death2 = {ROBOTANKDEATH2PIC, 8, NULL, &s_robotank_death3};
statetype s_robotank_death3 = {ROBOTANKDEATH2PIC, -1, &T_AlternateStates, &s_robotank_death1};
statetype s_robotank_death4 = {ROBOTANKDEATH3PIC, 25, &ExplosionSnd, &s_robotank_death5};
statetype s_robotank_death5 = {ROBOTANKDEATH4PIC, 20, NULL, &s_robotank_death5};

statetype s_robotank_shot1 = {PSHOT1PIC, 10, &T_ShootPlayer, &s_robotank_shot2};
statetype s_robotank_shot2 = {PSHOT2PIC, 10, &T_ShootPlayer, &s_robotank_shot1};


/*
=================
=
= SpawnRoboTank
=
=================
*/
void SpawnRoboTank(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex, tiley, &s_robotank_walk1, PIXRADIUS*35);
	newobj->obclass	= robotankobj;
	newobj->speed		= 1700;
	newobj->flags		|= of_shootable;
	newobj->hitpoints	= EasyHitPoints(25);
}


/*
=================
=
= T_RoboTankShoot
=
=================
*/
void T_RoboTankShoot(objtype *ob)
{
	ShootPlayer(ob, rtshotobj, 7000, &s_robotank_shot1);
}



/*
====================
=
= T_AlternateStates
=
====================
*/
void T_AlternateStates(objtype *ob)
{
	if (ob->temp1--)
	{
		ob->state = ob->state->next;
	}
	else
	{
		if (ob->state == &s_robotank_death3)
			ob->state = &s_robotank_death4;
		else
			ob->state = &s_aqua_die4;
	}
	ob->ticcount	= ob->state->tictime;
}






/*
=============================================================================

											STOMPY

=============================================================================
*/

statetype s_stompy_walk1 = {STOMPYWALK1PIC, 15, &T_ShooterObj, &s_stompy_walk2};
statetype s_stompy_walk2 = {STOMPYWALK2PIC, 15, &T_ShooterObj, &s_stompy_walk3};
statetype s_stompy_walk3 = {STOMPYWALK3PIC, 15, &T_ShooterObj, &s_stompy_walk4};
statetype s_stompy_walk4 = {STOMPYWALK4PIC, 15, &T_ShooterObj, &s_stompy_walk1};

statetype s_stompy_attack1 = {STOMPYATTACK1PIC, 10, NULL, &s_stompy_attack2};
statetype s_stompy_attack2 = {STOMPYATTACK2PIC, 15, NULL, &s_stompy_attack3};
statetype s_stompy_attack3 = {STOMPYATTACK2PIC, -1, T_StompyShoot, &s_stompy_attack4};
statetype s_stompy_attack4 = {STOMPYATTACK1PIC, 10, NULL, &s_stompy_walk1};

statetype s_stompy_ouch = {STOMPYATTACK1PIC, 10, NULL, &s_stompy_walk2};

statetype s_stompy_death1 = {STOMPYDEATH1PIC, 45, &ExplosionSnd, &s_stompy_death2};
statetype s_stompy_death2 = {STOMPYDEATH2PIC, 30, NULL, &s_stompy_death3};
statetype s_stompy_death3 = {STOMPYDEATH3PIC, 25, NULL, &s_stompy_death4};
statetype s_stompy_death4 = {STOMPYDEATH4PIC, 20, NULL, NULL};

statetype s_stompy_shot1 = {STOMPYSHOT1PIC, 6, &T_ShootPlayer, &s_stompy_shot2};
statetype s_stompy_shot2 = {STOMPYSHOT2PIC, 6, &T_ShootPlayer, &s_stompy_shot3};
statetype s_stompy_shot3 = {STOMPYSHOT1PIC, 6, &T_ShootPlayer, &s_stompy_shot4};
statetype s_stompy_shot4 = {STOMPYSHOT3PIC, 6, &T_ShootPlayer, &s_stompy_shot5};
statetype s_stompy_shot5 = {STOMPYSHOT4PIC, 6, &T_ShootPlayer, &s_stompy_shot4};


/*
=================
=
= SpawnStompy
=
=================
*/
void SpawnStompy(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex, tiley, &s_stompy_walk1, PIXRADIUS*25);
	newobj->obclass	= stompyobj;
	newobj->speed		= 1800;
	newobj->flags		|= of_shootable;
	newobj->hitpoints	= EasyHitPoints(20);
}


/*
=================
=
= T_StompyShoot
=
=================
*/
void T_StompyShoot(objtype *ob)
{
	ShootPlayer(ob, syshotobj, 8500, &s_stompy_shot1);
}



/*
=============================================================================

												BUG

=============================================================================
*/

statetype s_bug_walk1 = {BUG_WALK1PIC, 15, &T_ShooterObj, &s_bug_walk2};
statetype s_bug_walk2 = {BUG_WALK2PIC, 15, &T_ShooterObj, &s_bug_walk3};
statetype s_bug_walk3 = {BUG_WALK3PIC, 15, &T_ShooterObj, &s_bug_walk1};

statetype s_bug_attack1 = {BUG_ATTACK1PIC, 20, NULL, &s_bug_attack2};
statetype s_bug_attack2 = {BUG_ATTACK2PIC, 20, NULL, &s_bug_attack3};
statetype s_bug_attack3 = {BUG_ATTACK2PIC, -1, &T_BugShoot, &s_bug_attack4};
statetype s_bug_attack4 = {BUG_ATTACK1PIC, 15, NULL, &s_bug_walk1};

statetype s_bug_ouch = {BUG_WALK1PIC, 10, NULL, &s_bug_walk2};

statetype s_bug_death1 = {BUG_DEATH1PIC, 35, &SmallSound, &s_bug_death2};
statetype s_bug_death2 = {BUG_DEATH2PIC, 10, NULL, &s_bug_death2};

statetype s_bug_shot1 = {BUG_SHOT1PIC, 10, &T_ShootPlayer, &s_bug_shot2};
statetype s_bug_shot2 = {BUG_SHOT2PIC, 10, &T_ShootPlayer, &s_bug_shot1};


/*
=================
=
= SpawnBug
=
=================
*/
void SpawnBug(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex, tiley, &s_bug_walk1, PIXRADIUS*20);
	newobj->obclass	= bugobj;
	newobj->speed		= 1500;
	newobj->flags		|= of_shootable;
	newobj->hitpoints	= EasyHitPoints(10);
}


/*
=================
=
= T_BugShoot
=
=================
*/
void T_BugShoot(objtype *ob)
{
	ShootPlayer(ob, bgshotobj, 8000, &s_bug_shot1);
}





/*
=============================================================================

									MEC EYE

=============================================================================
*/

void T_EyeShootPlayer (objtype *ob);

statetype s_eye_pause = {EYE_WALK1PIC,40,NULL,&s_eye_2};

statetype s_eye_1 = {EYE_WALK1PIC,20,T_ShooterObj,&s_eye_2};
statetype s_eye_2 = {EYE_WALK2PIC,20,T_ShooterObj,&s_eye_3};
statetype s_eye_3 = {EYE_WALK3PIC,20,T_ShooterObj,&s_eye_4};
statetype s_eye_4 = {EYE_WALK2PIC,20,T_ShooterObj,&s_eye_1};
statetype s_eye_shootplayer_1 = {EYE_WALK1PIC,1,T_EyeShootPlayer,&s_eye_shootplayer_2};
statetype s_eye_shootplayer_2 = {EYE_WALK1PIC,20,NULL,&s_eye_1};

statetype s_eye_ouch = {EYE_OUCH1PIC,8,NULL,&s_eye_ouch2};
statetype s_eye_ouch2 = {EYE_OUCH2PIC,8,NULL,&s_eye_1};

statetype s_eye_die1 = {EYE_DEATH1PIC,22,NULL,&s_eye_die2};
statetype s_eye_die2 = {EYE_DEATH2PIC,22,&SmallSound,&s_eye_die3};
statetype s_eye_die3 = {EYE_DEATH3PIC,22,NULL,&s_eye_die4};
statetype s_eye_die4 = {EYE_DEATH4PIC,22,NULL,&s_eye_die4};

statetype s_eshot1 = {EYE_SHOT1PIC,8,&T_ShootPlayer,&s_eshot2};
statetype s_eshot2 = {EYE_SHOT2PIC,8,&T_ShootPlayer,&s_eshot1};


//-------------------------------------------------------------------------
// SpawnEye()
//-------------------------------------------------------------------------
void SpawnShooterEye(id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;

	SpawnNewObj(tilex,tiley,&s_eye_1,PIXRADIUS*10);
	ob = newobj;
	newobj->obclass = eyeobj;
	newobj->speed = 3000;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(15);
	shooter_mode = sm_other1;
}


//---------------------------------------------------------------------------
// T_EyeShootPlayer
//---------------------------------------------------------------------------
void T_EyeShootPlayer (objtype *ob)
{
	ShootPlayer(ob, eshotobj, ESHOTSPEED, &s_eshot1);
}


/*
===============
=
= T_ShooterObj
=
= **********
= ***NOTE*** This routine controls the thinks for the RamBone, RoboTank,
= **********	Stompy, Future Mage, Bug, and Old Mage
=
===============
*/

void T_ShooterObj(objtype *ob)
{
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;
	id0_int_t angle;

	shooter_delay -= realtics;
	if (shooter_delay < 0)
	{
		shooter_mode = BE_Cross_Brandom(sm_dummy);
		// REFKEEN - Force a specific order of calls to random function
		// (same as original EXEs for DOS), don't introduce undefined behaviors
		shooter_delay = BE_Cross_Brandom(10*60);
		shooter_delay += BE_Cross_Brandom(50);
		//shooter_delay = random(10*60)+random(50);
	}

	tempx = player->x;
	tempy = player->y;
	temp_tilex = player->tilex;
	temp_tiley = player->tiley;


	switch (shooter_mode)
	{
		case sm_other1:
		case sm_other2:
		case sm_other3:
		case sm_other4:
			player->x = ((id0_long_t)other_x[shooter_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)other_y[shooter_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = other_x[shooter_mode];
			player->tiley = other_y[shooter_mode];
		break;
	}

	if (Chase(ob,true))
		shooter_delay = 0;

	player->x = tempx;
	player->y = tempy;
	player->tilex = temp_tilex;
	player->tiley = temp_tiley;

	angle = AngleNearPlayer(ob);


  // Handle shooting for the different characters controlled by this think.
	switch (ob->obclass)
	{
		case ramboneobj:
			if (!BE_Cross_Brandom(2) && (angle != -1))
			{
				ob->state = &s_skel_attack1;
				ob->ticcount = ob->state->tictime;
			}
		break;

		case fmageobj:
			if (!BE_Cross_Brandom(8) && (angle != -1))
			{
				ob->state = &s_fmageattack1;
				ob->ticcount = ob->state->tictime;
			}
		break;

		case robotankobj:
			if (!BE_Cross_Brandom(15) && (angle != -1))
			{
				ob->state = &s_robotank_attack1;
				ob->ticcount = ob->state->tictime;
			}
		break;

		case stompyobj:
			if (angle != -1)
			{
				ob->state = &s_stompy_attack1;
				ob->ticcount = ob->state->tictime;
			}
		break;

		case bugobj:
			if (!BE_Cross_Brandom(5) && (angle != -1))
			{
				ob->state = &s_bug_attack1;
				ob->ticcount = ob->state->tictime;
			}
		break;

		case eyeobj:
			if (!BE_Cross_Brandom(2) && (angle != -1))
			{
				ob->state = &s_eye_shootplayer_1;
				ob->ticcount = ob->state->tictime;
			}
		break;
	}
}






/*
=============================================================================

										RUNNING EYE

=============================================================================
*/

statetype s_reye_1 = {EYE_WALK1PIC, 20, &T_RunningEye, &s_reye_2};
statetype s_reye_2 = {EYE_WALK2PIC, 20, &T_RunningEye, &s_reye_3};
statetype s_reye_3 = {EYE_WALK3PIC, 20, &T_RunningEye, &s_reye_4};
statetype s_reye_4 = {EYE_WALK2PIC, 20, &T_RunningEye, &s_reye_1};

statetype s_reye_ouch = {EYE_OUCH1PIC, 8, NULL, &s_reye_ouch2};
statetype s_reye_ouch2 = {EYE_OUCH2PIC, 8, NULL, &s_reye_1};

statetype s_reye_die1 = {EYE_DEATH1PIC, 22, NULL, &s_reye_die2};
statetype s_reye_die2 = {EYE_DEATH2PIC, 22, &SmallSound, &s_reye_die3};
statetype s_reye_die3 = {EYE_DEATH3PIC, 22, NULL, &s_reye_die4};
statetype s_reye_die4 = {EYE_DEATH4PIC, 22, NULL, &s_reye_die4};

/*
====================
=
= SpawnRunningEye()
=
====================
*/
void SpawnRunningEye(id0_int_t tilex, id0_int_t tiley)
{
	SpawnNewObj(tilex,tiley,&s_reye_1,PIXRADIUS*25);
	newobj->obclass = reyeobj;
	newobj->speed = 3500;
	newobj->flags |= of_shootable;
	newobj->hitpoints = EasyHitPoints(15);
	newobj->temp2 = (*(mapsegs[2]+farmapylookup[tiley+1]+tilex))>>8;
	*(mapsegs[2]+farmapylookup[tiley+1]+tilex) = 0;

	newobj->temp1 = 2;

	if (!newobj->temp2)
		Quit("Initialize the running eye!\n");
}


/*
====================
=
= T_RunningEye
=
====================
*/
void T_RunningEye(objtype *ob)
{
	id0_int_t /*x, y, */dir_num, switch_num;
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;

	dir_num = *(mapsegs[2]+farmapylookup[ob->tiley]+ob->tilex);
	dir_num = dir_num>>8;

	if (!dir_num)
		dir_num = ob->temp2;

	if (dir_num == 5)
	{
		if (ob->temp1)
		{
			ob->temp1--;
		}
		else
		{
			ob->temp1 = 2;
			actorat[ob->tilex][ob->tiley] = 0;
			switch (ob->temp2)
			{
				case 1:
					ob->tiley = ob->tiley-1;
					ob->y = ((id0_long_t)(ob->tiley)<<TILESHIFT)+TILEGLOBAL/2;
				break;

				case 2:
					ob->tilex = ob->tilex+1;
					ob->x = ((id0_long_t)(ob->tilex)<<TILESHIFT)+TILEGLOBAL/2;
				break;

				case 3:
					ob->tiley = ob->tiley+1;
					ob->y = ((id0_long_t)(ob->tiley)<<TILESHIFT)+TILEGLOBAL/2;
				break;

				case 0:
				case 4:
					ob->tilex = ob->tilex-1;
					ob->x = ((id0_long_t)(ob->tilex)<<TILESHIFT)+TILEGLOBAL/2;
				break;
			}
			CalcBounds (ob);
			ChaseThink(ob,false);
			actorat[ob->tilex][ob->tiley] = COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(ob);
			//actorat[ob->tilex][ob->tiley] = ob;
			return;
		}
	}

	tempx = player->x;
	tempy = player->y;
	temp_tilex = player->tilex;
	temp_tiley = player->tiley;

	if (dir_num == 5)
		switch_num = ob->temp2;
	else
		switch_num = dir_num;

	switch (switch_num)
	{
		case 1:
			player->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)(ob->tiley-2)<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = ob->tilex;
			player->tiley = ob->tiley-2;
		break;

		case 2:
			player->x = ((id0_long_t)(ob->tilex+2)<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = ob->tilex+2;
			player->tiley = ob->tiley;
		break;

		case 3:
			player->x = ((id0_long_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)(ob->tiley+2)<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = ob->tilex;
			player->tiley = ob->tiley+2;
		break;

		case 0:
		case 4:
			player->x = ((id0_long_t)(ob->tilex-2)<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = ob->tilex-2;
			player->tiley = ob->tiley;
		break;
	}

	Chase(ob, false);

	player->x = tempx;
	player->y = tempy;
	player->tilex = temp_tilex;
	player->tiley = temp_tiley;

	if (dir_num != 5)
		ob->temp2 = dir_num;
}


/*
=============================================================================

										EGYPTIAN HEAD

=============================================================================
*/

void T_Head(objtype *ob);

statetype s_head = {HEADPIC, 20, &T_Head, &s_head};

statetype s_head_shot1 = {PSHOT1PIC, 10, &T_ShootPlayer, &s_head_shot2};
statetype s_head_shot2 = {PSHOT2PIC, 10, &T_ShootPlayer, &s_head_shot1};


/*
===================
=
= SpawnEgyptianHead
=
===================
*/

void SpawnEgyptianHead (id0_int_t tilex, id0_int_t tiley)
{
	objtype *ob;
	id0_short_t current_head_delay;
	id0_unsigned_t tile;

	SpawnNewObj(tilex, tiley, &s_head, PIXRADIUS*35);
	ob = newobj;
	head_mode = h_wait_to_rise;

	tile = *(mapsegs[2]+farmapylookup[tiley+1]+tilex);
	if (tile)
		head_delay = (tile>>8)*30;
	else
	{
		current_head_delay = (3*60)+BE_Cross_Brandom(3*60);
		head_delay = head_base_delay+current_head_delay;
		head_base_delay += current_head_delay;
		if (head_base_delay > 8*60)
			head_base_delay = 0;
	}

	newobj->obclass	= realsolidobj;
	newobj->speed		= 3000;
	newobj->flags	  |= of_shootable;
}



//--------------------------------------------------------------------------
// T_Head()
//--------------------------------------------------------------------------
void T_Head(objtype *ob)
{
	fixed tempx,tempy;
	id0_unsigned_t temp_tilex,temp_tiley;
	// REFKEEN - The angle is never initialized in the original codebase.
	// This would lead to undefined behaviors. For now a value of 0 is
	// chosen for it. Since angle is never modified at all, we can omit it.
	//
	// TODO: Anything that can be done for reproduction of original
	// behaviors, if differing from "angle is always zero"?
	//id0_int_t angle;

	switch (head_mode)
	{
		case h_wait_to_rise:
			if (head_delay < 0)
			{
				if ((ob->tilex == player->tilex) && (ob->tiley == player->tiley))
					break;
				if (CheckHandAttack(ob))
					break;

				ob->obclass	= headobj;
				ob->active	= always;
				head_mode	= h_active;
				// REFKEEN - Force a specific order of calls to random function
				// (same as original EXEs for DOS), don't introduce undefined behaviors
				head_delay = BE_Cross_Brandom(100);
				head_delay += BE_Cross_Brandom(60);
				//head_delay 	= random(100)+random(60);
				ob->hitpoints = EasyHitPoints(16);
			}
			else
				head_delay -= tics;

		break;

		case h_player1:
		case h_player2:
		case h_player3:
		case h_player4:
		case h_active:
			Chase (ob,true);

			// REFKEEN - We assume angle==0 now
			if (!BE_Cross_Brandom(2)/* && (angle != -1)*/)
				ShootPlayer(ob, hshotobj, 10000, &s_head_shot1);

			head_delay -= tics;
			if (head_delay < 0)
			{
				head_mode = BE_Cross_Brandom(h_other4)+1;
				// REFKEEN - Force a specific order of calls to random function
				// (same as original EXEs for DOS), don't introduce undefined behaviors
				head_delay = BE_Cross_Brandom(10*60);
				head_delay += BE_Cross_Brandom(50);
				//head_delay = random(10*60)+random(50);
			}
		break;

		case h_other1:
		case h_other2:
		case h_other3:
		case h_other4:

			tempx = player->x;
			tempy = player->y;
			temp_tilex = player->tilex;
			temp_tiley = player->tiley;

			player->x = ((id0_long_t)other_x[head_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->y = ((id0_long_t)other_y[head_mode]<<TILESHIFT)+TILEGLOBAL/2;
			player->tilex = other_x[head_mode];
			player->tiley = other_y[head_mode];

			if (Chase(ob,true))
				head_delay = 0;

			player->x = tempx;
			player->y = tempy;
			player->tilex = temp_tilex;
			player->tiley = temp_tiley;

			head_delay -= tics;
			if (head_delay <= 0)
			{
				head_mode = h_active;
				// REFKEEN - Force a specific order of calls to random function
				// (same as original EXEs for DOS), don't introduce undefined behaviors
				head_delay = BE_Cross_Brandom(10*60);
				head_delay += BE_Cross_Brandom(50);
				//head_delay = random(10*60)+random(50);
			}
		break;
	}
}
