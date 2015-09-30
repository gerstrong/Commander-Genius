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

// KD_DEF.H

#include "id_heads.h"
//#include <BIOS.H>
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
#include "soft.h"
#include "sl_file.h"
#endif

#define FRILLS	0			// Cut out frills for 360K - MIKE MAYNARD

#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
#define CREDITS 0
#endif


/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

#define	MAXACTORS	MAXSPRITES

#define ACCGRAVITY	3
#define SPDMAXY		80

#define BLOCKSIZE	(8*PIXGLOBAL)		// for positioning starting actors

//
// movement scrolling edges
//
#define SCROLLEAST (TILEGLOBAL*11)
#define SCROLLWEST (TILEGLOBAL*9)
#define SCROLLSOUTH (TILEGLOBAL*8)
#define SCROLLNORTH (TILEGLOBAL*4)

#define CLIPMOVE	24					// speed to push out of a solid wall

#define GAMELEVELS	17

/*
=============================================================================

							 TYPES

=============================================================================
*/

typedef	enum	{notdone,resetgame,levelcomplete,warptolevel,died,victorious}
				exittype;

typedef	enum	{nothing,keenobj,powerobj,doorobj,
	bonusobj,broccoobj,tomatobj,carrotobj,celeryobj,asparobj,grapeobj,
	taterobj,cartobj,frenchyobj,melonobj,turnipobj,cauliobj,brusselobj,
	mushroomobj,squashobj,apelobj,peapodobj,peabrainobj,boobusobj,
	shotobj,inertobj}	classtype;

// REFKEEN - enum type for progress field should be outside struct if we want
// to be able to build the same code as C++. It's also good for other reasons.
typedef enum {step,slide,think,stepthink,slidethink} progresstype;
// REFKEEN - Used for C++ patches for function pointers in statetype
struct objstruct;

// REFKEEN - Add name statestruct for better definition of nextstate
typedef struct statestruct
{
  id0_int_t 		leftshapenum,rightshapenum;
  progresstype progress;
  //enum		{step,slide,think,stepthink,slidethink} progress;
  id0_boolean_t	skippable;

  id0_boolean_t	pushtofloor;
  id0_int_t tictime;
  id0_int_t xmove;
  id0_int_t ymove;
  // REFKEEN - C++ patches: Write the correct arguments lists, and
  // rename function pointers: think ==> thinkptr comes from conflict
  // with the 'think' enum value for progress in Keen Dreams
  void (*thinkptr) (struct objstruct *);
  void (*contactptr) (struct objstruct *, struct objstruct *);
  void (*reactptr) (struct objstruct *);
  //void (*think) ();
  //void (*contact) ();
  //void (*react) ();
  // REFKEEN - Change type of this for C++
  struct statestruct *nextstate;
  //void *nextstate;
  // (REFKEEN) Backwards compatibility:
  // MUST follow all the rest of the members above. Given a statetype
  // instance, stores what would be the 16-bit offset pointer in the dseg
  // while using the original 16-bit DOS executable (corresponding version).
  // This member must be the last so it doesn't have to be filled during
  // compile-time (requires changes to struct initializations in a few places)
  id0_word_t compatdospointer;
} statetype;


typedef	struct
{
	id0_unsigned_t	worldx,worldy;
	id0_boolean_t	leveldone[GAMELEVELS];
	id0_long_t	score,nextextra;
	id0_int_t		flowerpowers;
	id0_int_t		boobusbombs,bombsthislevel;
	id0_int_t		keys;
	id0_int_t		mapon;
	id0_int_t		lives;
	id0_int_t		difficulty;
} gametype;

// REFKEEN - enum type for active field should be outside struct if we want
// to be able to build the same code as C++. It's also good for other reasons.
typedef enum {no,yes,allways,removable} activetype;

typedef struct	objstruct
{
	classtype	obclass;
	activetype active;
	//enum		{no,yes,allways,removable} active;
	id0_boolean_t		needtoreact,needtoclip;
	id0_unsigned_t	nothink;
	id0_unsigned_t	x,y;

	id0_int_t			xdir,ydir;
	id0_int_t			xmove,ymove;
	id0_int_t			xspeed,yspeed;

	id0_int_t			ticcount,ticadjust;
	statetype	*state;

	id0_unsigned_t	shapenum;

	id0_unsigned_t	left,top,right,bottom;	// hit rectangle
	id0_unsigned_t	midx;
	id0_unsigned_t	tileleft,tiletop,tileright,tilebottom;	// hit rect in tiles
	id0_unsigned_t	tilemidx;

	id0_int_t			hitnorth,hiteast,hitsouth,hitwest;	// wall numbers contacted

	// (REFKEEN) BACKWARDS COMPATIBILITY: Usually temp2 may be just an integer,
	// but it can store a 16-bit offset pointer to a state (when creature changes into flower).
	// So the original 16-bit pointer is stored in temp2 for saved game compatibility,
	// while the real pointer is stored in temp2stateptr (can technically be a void* if there's a need).
	id0_int_t			temp1,temp2,temp3,temp4;
	statetype *temp2stateptr;

	void		*sprite;

	struct	objstruct	*next,*prev;
} objtype;


#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
struct BitMapHeader {
	id0_unsigned_int_t	w,h,x,y;
	id0_unsigned_char_t	d,trans,comp,pad;
} __attribute__((__packed__));

// (REFKEEN) Seems unused
struct BitMap {
	id0_unsigned_int_t Width;
	id0_unsigned_int_t Height;
	id0_unsigned_int_t Depth;
	id0_unsigned_int_t BytesPerRow;
	id0_char_t id0_far *Planes[8];
};

struct Shape {
	memptr Data;
	id0_long_t size;
	id0_unsigned_int_t BPR;
	struct BitMapHeader bmHdr;
};

typedef struct {
	BE_FILE_T handle;			// handle of file
	memptr buffer;		// pointer to buffer
	id0_word_t offset;		// offset into buffer
	id0_word_t status;		// read/write status
} BufferedIO;
#endif


// (REFKEEN) BACKWARDS COMPATIBILITY: At times, one of the temp members of
// objstruct may store a 16-bit pointer with another object.
#define COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(objptr) ((objptr)?((id0_word_t)((id0_word_t)((objptr)-objarray)*sizeof(objtype)+refkeen_compat_kd_play_objoffset)):(id0_word_t)0)
#define COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(dosptr) ((dosptr)?(objarray+(id0_word_t)((id0_word_t)(dosptr)-refkeen_compat_kd_play_objoffset)/sizeof(objtype)):NULL)

extern id0_word_t refkeen_compat_kd_play_objoffset;
extern objtype objarray[MAXACTORS]; // FOR CONVERSIONS AS ABOVE (COMPATIBILITY) ONLY


/*
=============================================================================

					 KD_MAIN.C DEFINITIONS

=============================================================================
*/

extern	id0_char_t	str[80],str2[20];
extern	id0_boolean_t	singlestep,jumpcheat,godmode,tedlevel;
extern	id0_unsigned_t	tedlevelnum;

void	DebugMemory (void);
void	TestSprites(void);
id0_int_t		DebugKeys (void);
void	StartupId (void);
void	ShutdownId (void);
void	Quit (const id0_char_t *error);
void	InitGame (void);
//void	main (void);


/*
=============================================================================

					  KD_DEMO.C DEFINITIONS

=============================================================================
*/

void	Finale (void);
void	GameOver (void);
void	DemoLoop (void);
void	StatusWindow (void);
void	NewGame (void);
void	TEDDeath (void);

id0_boolean_t	LoadGame (BE_FILE_T file);
id0_boolean_t	SaveGame (BE_FILE_T file);
void	ResetGame (void);

/*
=============================================================================

					  KD_PLAY.C DEFINITIONS

=============================================================================
*/

extern	gametype	gamestate;
extern	exittype	playstate;
extern	id0_boolean_t		button0held,button1held;
extern	id0_unsigned_t	originxtilemax,originytilemax;
// (REFKEEN) new has been renamed newobj since new is a reserved C++ keyword
extern	objtype		*newobj,*check,*player,*scoreobj;

extern	ControlInfo	c;

extern	objtype dummyobj;

extern	const id0_char_t		*levelnames[21];

void	CheckKeys (void);
void	CalcInactivate (void);
void 	InitObjArray (void);
void 	GetNewObj (id0_boolean_t usedummy);
void	RemoveObj (objtype *gone);
void 	ScanInfoPlane (void);
void 	PatchWorldMap (void);
void 	MarkTileGraphics (void);
void 	FadeAndUnhook (void);
void 	SetupGameLevel (id0_boolean_t loadnow);
void 	ScrollScreen (void);
void 	MoveObjVert (objtype *ob, id0_int_t ymove);
void 	MoveObjHoriz (objtype *ob, id0_int_t xmove);
void 	GivePoints (id0_unsigned_t points);
void 	ClipToEnds (objtype *ob);
void 	ClipToEastWalls (objtype *ob);
void 	ClipToWestWalls (objtype *ob);
void 	ClipToWalls (objtype *ob);
void	ClipToSprite (objtype *push, objtype *solid, id0_boolean_t squish);
void	ClipToSpriteSide (objtype *push, objtype *solid);
id0_int_t 	DoActor (objtype *ob,id0_int_t tics);
void 	StateMachine (objtype *ob);
void 	NewState (objtype *ob,statetype *state);
void 	PlayLoop (void);
void 	GameLoop (void);

/*
=============================================================================

					  KD_KEEN.C DEFINITIONS

=============================================================================
*/

void	CalcSingleGravity (void);

void	ProjectileThink		(objtype *ob);
void	VelocityThink		(objtype *ob);
void	DrawReact			(objtype *ob);

void	SpawnScore (void);
void	FixScoreBox (void);
void	SpawnWorldKeen (id0_int_t tilex, id0_int_t tiley);
void	SpawnKeen (id0_int_t tilex, id0_int_t tiley, id0_int_t dir);

void 	KillKeen (void);

extern	id0_int_t	singlegravity;
extern	id0_unsigned_t	bounceangle[8][8];

extern	statetype s_keendie1;

/*
=============================================================================

					  KD_ACT1.C DEFINITIONS

=============================================================================
*/

void WalkReact (objtype *ob);

void 	DoGravity (objtype *ob);
void	AccelerateX (objtype *ob,id0_int_t dir,id0_int_t max);
void 	FrictionX (objtype *ob);

void	ProjectileThink		(objtype *ob);
void	VelocityThink		(objtype *ob);
void	DrawReact			(objtype *ob);
void	DrawReact2			(objtype *ob);
void	DrawReact3			(objtype *ob);
void	ChangeState (objtype *ob, statetype *state);

void	ChangeToFlower (objtype *ob);

void	SpawnBonus (id0_int_t tilex, id0_int_t tiley, id0_int_t type);
void	SpawnDoor (id0_int_t tilex, id0_int_t tiley);
void 	SpawnBrocco (id0_int_t tilex, id0_int_t tiley);
void 	SpawnTomat (id0_int_t tilex, id0_int_t tiley);
void 	SpawnCarrot (id0_int_t tilex, id0_int_t tiley);
void 	SpawnAspar (id0_int_t tilex, id0_int_t tiley);
void 	SpawnGrape (id0_int_t tilex, id0_int_t tiley);

extern	statetype s_doorraise;

extern	statetype s_bonus;
extern	statetype s_bonusrise;

extern	statetype s_broccosmash3;
extern	statetype s_broccosmash4;

extern	statetype s_grapefall;

/*
=============================================================================

					  KD_ACT2.C DEFINITIONS

=============================================================================
*/

void SpawnTater (id0_int_t tilex, id0_int_t tiley);
void SpawnCart (id0_int_t tilex, id0_int_t tiley);
void SpawnFrenchy (id0_int_t tilex, id0_int_t tiley);
void SpawnMelon (id0_int_t tilex, id0_int_t tiley,id0_int_t dir);
void SpawnSquasher (id0_int_t tilex, id0_int_t tiley);
void SpawnApel (id0_int_t tilex, id0_int_t tiley);
void SpawnPeaPod (id0_int_t tilex, id0_int_t tiley);
void SpawnPeaBrain (id0_int_t tilex, id0_int_t tiley);
void SpawnBoobus (id0_int_t tilex, id0_int_t tiley);

extern	statetype s_taterattack2;
extern	statetype s_squasherjump2;
extern	statetype s_boobusdie;

extern	statetype s_deathwait1;
extern	statetype s_deathwait2;
extern	statetype s_deathwait3;
extern	statetype s_deathboom1;
extern	statetype s_deathboom2;

#ifndef REFKEEN_VER_KDREAMS_CGA_ALL

/////////////////////////////////////////////////////////////////////////////
//
//						GELIB.C DEFINITIONS
//
/////////////////////////////////////////////////////////////////////////////

struct Shape;

void FreeShape(struct Shape *shape);
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty);

id0_long_t Verify(const id0_char_t *filename);
memptr InitBufferedIO(BE_FILE_T handle, BufferedIO *bio);
void FreeBufferedIO(BufferedIO *bio);
id0_byte_t bio_readch(BufferedIO *bio);
void bio_fillbuffer(BufferedIO *bio);
void SwapLong(id0_long_t id0_far *Var);
void SwapWord(id0_unsigned_int_t id0_far *Var);
void MoveGfxDst(id0_short_t x, id0_short_t y);

#endif

// (REFKEEN) Backwards compatibility: Used for statetype offset conversions.
extern statetype* (*RefKeen_GetObjStatePtrFromDOSPointer)(uint_fast32_t dosptr);
