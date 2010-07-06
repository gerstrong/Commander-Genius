/*
 * spritedefines.h
 *
 *  Created on: 22.10.2009
 *      Author: gerstrong
 *
 *  Spritetable: Indexes where CG must look into to get the proper sprite
 */

#ifndef SPRITEDEFINES_H_
#define SPRITEDEFINES_H_

#define MAX_SPRITES  1000

#define SECOND_PLAYER_BASEFRAME 500
#define THIRD_PLAYER_BASEFRAME 548
#define FOURTH_PLAYER_BASEFRAME 596

// Normals sprites
#define ENEMYRAY           109
#define ENEMYRAYEP2        123
#define ENEMYRAYEP3        103
#define RAY_VERT_EP3       104

#define RAY_FRAME_ZAP_EP1     110
#define RAY_FRAME_ZOT_EP1     111

#define RAY_FRAME_ZAP_EP2     124
#define RAY_FRAME_ZOT_EP2     125

#define RAY_FRAME_ZAP_EP3     105
#define RAY_FRAME_ZOT_EP3     106

// horizontally-moving platform (ep2 & 3)
#define OBJ_PLATFORM_DEFSPRITE_EP2   126
#define OBJ_PLATFORM_DEFSPRITE_EP3   107

#define OBJ_YORP_DEFSPRITE       50
#define OBJ_GARG_DEFSPRITE       60
#define OBJ_VORT_DEFSPRITE_EP1   78
#define OBJ_VORT_DEFSPRITE_EP2   82
#define OBJ_VORT_DEFSPRITE_EP3   71
#define OBJ_BUTLER_DEFSPRITE     88
#define OBJ_ICECHUNK_DEFSPRITE   112
#define OBJ_ICEBIT_DEFSPRITE     113
#define OBJ_ROPE_DEFSPRITE       114
#define OBJ_TELEPORTER_DEFSPRITE 180

#define OBJ_RAY_DEFSPRITE_EP1    108
#define OBJ_RAY_DEFSPRITE_EP2    122
#define OBJ_RAY_DEFSPRITE_EP3    102

#define OBJ_TANKEP2_DEFSPRITE    112
#define OBJ_TANK_DEFSPRITE       98

#define OBJ_SNDWAVE_DEFSPRITE      128

#define OBJ_SCRUB_DEFSPRITE     102

#define OBJ_VORTELITE_DEFSPRITE  88

#define OBJ_FOOB_DEFSPRITE			95
#define OBJ_NINJA_DEFSPRITE			77
#define OBJ_MOTHER_DEFSPRITE		87
#define OBJ_BALL_DEFSPRITE			109
#define OBJ_JACK_DEFSPRITE			110
#define OBJ_MEEP_DEFSPRITE			118
#define OBJ_NESSIE_DEFSPRITE		132

#define OBJ_BABY_DEFSPRITE_EP2   52
#define OBJ_BABY_DEFSPRITE_EP3   51

#define OBJ_SPARK_DEFSPRITE_EP2		128

// special sprites (they either weren't in the game originally,
// or are used for internal engine stuff).
#define BLANKSPRITE			(MAX_SPRITES-1)
#define DOOR_YELLOW_SPRITE	(MAX_SPRITES-2)			// opening door (yellow)
#define DOOR_BLUE_SPRITE	(MAX_SPRITES-3)
#define DOOR_GREEN_SPRITE	(MAX_SPRITES-4)
#define DOOR_RED_SPRITE		(MAX_SPRITES-5)
#define PT5000_SPRITE		(MAX_SPRITES-6)			// rising bonus pts (+5000)
#define PT1000_SPRITE		(MAX_SPRITES-7)
#define PT500_SPRITE		(MAX_SPRITES-8)
#define PT200_SPRITE		(MAX_SPRITES-9)
#define PT100_SPRITE		(MAX_SPRITES-10)
#define PT1UP_SPRITE		(MAX_SPRITES-11)		// rising 1up bonus
#define PTCARDY_SPRITE		(MAX_SPRITES-12)		// rising access card
#define PTCARDR_SPRITE		(MAX_SPRITES-13)
#define PTCARDG_SPRITE		(MAX_SPRITES-14)
#define PTCARDB_SPRITE		(MAX_SPRITES-15)
#define SHOTUP_SPRITE		(MAX_SPRITES-16)		// rising single shot (ep3)
#define ANKHUP_SPRITE		(MAX_SPRITES-17)		// rising single shot (ep3)
#define GUNUP_SPRITE		(MAX_SPRITES-18)		// rising ray gun
#define YORPSHIELD_SPRITE	(MAX_SPRITES-19)		// invincibility force field, used in editor and in "yorps have forcefields" mode
#define VERSION_SPRITE		(MAX_SPRITES-20)		// version text shown in lower-right corner at startup
#define DEMOBOX_SPRITE		(MAX_SPRITES-21)		// says "Demo"
#define OSD_LIVES_SPRITE	(MAX_SPRITES-22)
#define OSD_AMMO_SPRITE		(MAX_SPRITES-23)
#define OSD_YORPS_SPRITE	(MAX_SPRITES-24)
#define TITLE_LOGO1_SPRITE	(MAX_SPRITES-25)		// left half of the logo
#define TITLE_LOGO2_SPRITE	(MAX_SPRITES-26)		// right half of the logo
#define TITLE_FLOOR_SPRITE	(MAX_SPRITES-27)		// more stuff for the logo
#define TITLE_VORT_SPRITE   (MAX_SPRITES-28)
#define ARROWLR_SPRITE		(MAX_SPRITES-29)		// these are directional arrows used in the editor
#define ARROWUD_SPRITE		(MAX_SPRITES-30)
#define ARROWUR_SPRITE		(MAX_SPRITES-31)
#define ARROWUL_SPRITE		(MAX_SPRITES-32)
#define ARROWU_SPRITE		(MAX_SPRITES-33)
#define ARROWD_SPRITE		(MAX_SPRITES-34)
#define LAST_SPECIAL_SPRITE (MAX_SPRITES-35)
#define CG_LOGO				(MAX_SPRITES-36)

#endif /* DEFINES_H_ */
