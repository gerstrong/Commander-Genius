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

#include "id_heads.h"
#include <math.h>
//#include <VALUES.H>
#include <limits.h>

//#define PROFILE

/*
=============================================================================

						 GLOBAL CONSTANTS

=============================================================================
*/

//
// MAX_BASE - represents 100 percent in 1st base
// MAX_PERC - represents 100 percent in 2nd base
// PERC - fractional portion of 2nd base
// SCALE - arbitrary scaling value (bigger number means more accurate)
//
// ex: PERCENTAGE(320,16,8,8)    returns  160
//
//

// (REFKEEN) Replace with distinct variations for compatibility (Used for drawing health and freeze time)
#define PERCENTAGE_SIGNED16BITS(MAX_BASE,MAX_PERC,PERC,SCALE) ((id0_unsigned_t)(MAX_BASE*(((id0_int_t)(PERC<<SCALE))/MAX_PERC))>>SCALE)
#define PERCENTAGE_SIGNED32BITS(MAX_BASE,MAX_PERC,PERC,SCALE) ((id0_unsigned_t)(MAX_BASE*(((id0_long_t)(PERC<<SCALE))/MAX_PERC))>>SCALE)
//#define PERCENTAGE(MAX_BASE,MAX_PERC,PERC,SCALE) ((id0_unsigned_t)(MAX_BASE*((PERC<<SCALE)/MAX_PERC))>>SCALE)

#define PI	3.141592657


#define SIZE_TEST			0 //50000			// DO NOT USE!! ARGH!

#define FL_QUICK 			0x01
#define FL_NOMEMCHECK	0x02

#define FL_CLEAR		(FL_QUICK|FL_NOMEMCHECK)

#define GEM_SHIFT		2
#define FL_RGEM		0x04
#define FL_GGEM		0x08
#define FL_BGEM		0x10
#define FL_YGEM		0x20
#define FL_PGEM		0x40
#define FL_DEAD		0x80



#define MAXBOLTS		10
#define MAXNUKES		10
#define MAXPOTIONS	10

#define NUKE_COST		(1000)
#define BOLT_COST    (1200)
#define POTION_COST	(1300)

#define NUKE_COST_TXT		("1000")		// Allows for Q&D positioning..
#define BOLT_COST_TXT    	("1200")
#define POTION_COST_TXT		("1300")

#define RADARX	31															// bytes
#define RADARY	11                                           // pixels
#define RADAR_WIDTH     51                                  //   "
#define RADAR_HEIGHT		51                                  //   "
#define RADAR_XCENTER	((RADARX*8)+(RADAR_WIDTH/2)+3)      //   "
#define RADAR_YCENTER	((RADARY-8)+(RADAR_HEIGHT/2)+5)       //   "
#define MAX_RADAR_BLIPS	60


#define RADAR_RADIUS			17
#define RADAR_RADIUS_NSEW	15
#define RADAR_X_IRADIUS		(113/5)
#define RADAR_Y_IRADIUS		(113/7)
#define RADAR_ICON_CENTER	4						// Center offset into icon.

#define NAMESTART	180
#define REMOVED_DOOR_TILE	NAMESTART

#define NEXT_LEVEL_CODE		0xff
#define REMOVE_DOOR_CODE	0xfe
#define CANT_OPEN_CODE		0xfd
#define EXP_WALL_CODE		0xfc


#define UNMARKGRCHUNK(chunk)	(grneeded[chunk]&=~ca_levelbit)

#define MOUSEINT	0x33

#define EXPWALLSTART	8
#define NUMEXPWALLS		7
#define WALLEXP			15
#define WATEREXP			29
#define NUMFLOORS		71

#define NUMLATCHPICS	 (FIRSTWALLPIC-FIRSTLATCHPIC) //+5
#define NUMSCALEPICS	 (FIRSTWALLPIC-FIRSTSCALEPIC) //+5
#define NUMSCALEWALLS (LASTWALLPIC-FIRSTWALLPIC) //+5


#define FLASHCOLOR	12
#define FLASHTICS	4


#define NUMLEVELS	21

#define VIEWX		0		// corner of view window
#define VIEWY		0
#define VIEWWIDTH	(40*8)		// size of view window				// 33
#define VIEWHEIGHT	(15*8)												// 18
#define VIEWXH		(VIEWX+VIEWWIDTH-1)
#define VIEWYH		(VIEWY+VIEWHEIGHT-1)

#define CENTERX		(VIEWX+VIEWWIDTH/2-1)	// middle of view window
#define CENTERY		(VIEWY+VIEWHEIGHT/2-1)

#define GLOBAL1		(1l<<16)
#define TILEGLOBAL  GLOBAL1
#define TILESHIFT	16l

#define MINDIST		(2*GLOBAL1/5)
#define FOCALLENGTH	(TILEGLOBAL)	// in global coordinates


#define ANGLES		360		// must be divisable by 4

#define MAPSIZE		64		// maps are 64*64 max
#define MAXACTORS	100		// max number of tanks, etc / map

#define NORTH	0
#define EAST	1
#define SOUTH	2
#define WEST	3

#define SIGN(x) ((x)>0?1:-1)
#define ABS(x) ((id0_int_t)(x)>0?(x):-(x))
#define LABS(x) ((id0_long_t)(x)>0?(x):-(x))

#define	MAXSCALE	(VIEWWIDTH/2)


#define MAXBODY		100
#define MAXSHOTPOWER	56

#define SCREEN1START	0
#define SCREEN2START	8320

#define STATUSLEN			0xc80
#define PAGELEN			0x1700

#define PAGE1START	STATUSLEN
#define PAGE2START	(PAGE1START+PAGELEN)
#define PAGE3START	(PAGE2START+PAGELEN)
#define FREESTART		(PAGE3START+PAGELEN)

#define PIXRADIUS		512

#define STATUSLINES		(200-VIEWHEIGHT)

enum bonusnumbers {B_BOLT,B_NUKE,B_POTION,B_RKEY,B_YKEY,B_GKEY,B_BKEY,B_SCROLL1,
 B_SCROLL2,B_SCROLL3,B_SCROLL4,B_SCROLL5,B_SCROLL6,B_SCROLL7,B_SCROLL8,
 B_GOAL,B_CHEST,B_RGEM,B_GGEM,B_BGEM,B_YGEM,B_PGEM,B_RKEY2};


#define MAX_DOOR_STORAGE 5

#define GEM_DELAY_TIME	(120*60)

#define ROTATE_SPEED		(6)

#define WALL_SKELETON_CODE 6

#define MAXREALTICS (2*60)

#define MAXFREEZETIME			(100*30)				// 50 secs (100 half)
extern	id0_boolean_t EASYMODEON;


/*
=============================================================================

						   GLOBAL TYPES

=============================================================================
*/

enum {BLANKCHAR=9,BOLTCHAR,NUKECHAR,POTIONCHAR,KEYCHARS,SCROLLCHARS=17,
	NUMBERCHARS=25};

typedef id0_long_t fixed;

typedef struct {id0_int_t x,y;} tilept;
typedef struct {fixed x,y;} globpt;

typedef struct
{
  id0_int_t	x1,x2,leftclip,rightclip;// first pixel of wall (may not be visable)
  id0_unsigned_t	height1,height2,color,walllength,side;
	id0_long_t	planecoord;
} walltype;

typedef enum
  {nothing,playerobj,bonusobj,orcobj,batobj,skeletonobj,trollobj,demonobj,
  mageobj,pshotobj,bigpshotobj,mshotobj,inertobj,bounceobj,grelmobj,
  gateobj,zombieobj,spookobj,wetobj,expobj,eyeobj,wallskelobj,eshotobj,
  gshotobj,reddemonobj,freezeobj,solidobj} classtype;

typedef enum {north,east,south,west,northeast,southeast,southwest,
		  northwest,nodir} dirtype;		// a catacombs 2 carryover

// REFKEEN - Used for C++ patches for function pointers in statetype
struct objstruct;

typedef struct	statestruct
{
	id0_int_t		shapenum;
	id0_int_t		tictime;
	// REFKEEN - C++ patches: Write the correct arguments list, and
	// rename function pointer: think ==> thinkptr comes from conflict
	// with the 'think' enum value for progress in Keen Dreams
	void (*thinkptr) (struct objstruct *);
	//void	(*think) ();
	struct	statestruct	*next;
	// (REFKEEN) Backwards compatibility:
	// MUST follow all the rest of the members above. Given a statetype
	// instance, stores what would be the 16-bit offset pointer in the dseg
	// while using the original 16-bit DOS executable (corresponding version).
	// This member must be the last so it doesn't have to be filled during
	// compile-time (requires changes to struct initializations in a few places)
	id0_word_t compatdospointer;
} statetype;

#define of_shootable		0x01
#define of_damagedone	0x02

// REFKEEN - enum type for active field should be outside struct if we want
// to be able to build the same code as C++. It's also good for other reasons.
typedef enum {no,noalways,yes,always} activetype;

typedef struct objstruct
{
  activetype active;
  //enum {no,noalways,yes,always}	active;
  id0_int_t		ticcount;
  classtype	obclass;
  statetype	*state;

  id0_unsigned_char_t flags;
//  id0_boolean_t	tileobject;		// true if entirely inside one tile

  id0_long_t		distance;
  dirtype	dir;
  fixed 	x,y;
  id0_unsigned_t	tilex,tiley;
  id0_int_t	 	viewx;
  id0_unsigned_t	viewheight;

  id0_int_t 		angle;
  id0_int_t		hitpoints;
  id0_long_t		speed;

  id0_unsigned_t	size;			// global radius for hit rect calculation
  fixed		xl,xh,yl,yh;	// hit rectangle

  id0_int_t		temp1,temp2;
  struct	objstruct	*next,*prev;
} objtype;


typedef	enum	{ex_stillplaying,ex_died,ex_warped,ex_resetgame
	,ex_loadedgame,ex_victorious,ex_turning,ex_abort} exittype;


typedef enum {S_NONE,     S_HELP,    S_SND,     S_SAVING,   S_RESTORING,
			S_JOYSTICK, S_RETREAT, S_ADVANCE, S_SIDESTEP, S_QTURN,
			S_MISSLE,   S_ZAPPER,  S_XTER,    S_CURING,   S_READ,
			S_VIEWING,  S_ITEMDES,  S_DAMAGE,  S_TURN,    S_TIMESTOP} status_flags;

typedef struct {
	id0_char_t x,y;
	id0_unsigned_t ondoor,underdoor;
} doorinfo;

typedef struct {
	id0_char_t x,y;
	id0_short_t angle;
	doorinfo doors[MAX_DOOR_STORAGE];
} levelinfo;

typedef	struct
{
	id0_int_t		difficulty;
	id0_int_t		mapon;
	id0_int_t		bolts,nukes,potions,keys[4],scrolls[8];

	id0_int_t		gems[5];				// "int allgems[5]" is used for 1:1 comparison
										// in play loop for radar... CHANGE IT, TOO!

	id0_long_t	score;
	id0_int_t		body,shotpower;
//	levelinfo levels[NUMLEVELS];
} gametype;


// (REFKEEN) BACKWARDS COMPATIBILITY: At times, one of the temp members of
// objstruct may store a 16-bit pointer with another object; Or at least this
// is the case in Keen Dreams. Furthermore, in the Catacombs, actorat may be
// declared as a bidimensional array of objtype pointers, but it is also used
// to store plain 16-bit integers.

#define COMPAT_OBJ_CONVERT_OBJ_PTR_TO_DOS_PTR(objptr) ((objptr)?((id0_word_t)((id0_word_t)((objptr)-objlist)*sizeof(objtype)+refkeen_compat_c4_play_objoffset)):(id0_word_t)0)
#define COMPAT_OBJ_CONVERT_DOS_PTR_TO_OBJ_PTR(dosptr) ((dosptr)?(objlist+(id0_word_t)((id0_word_t)(dosptr)-refkeen_compat_c4_play_objoffset)/sizeof(objtype)):NULL)

extern id0_word_t refkeen_compat_c4_play_objoffset;
extern objtype objlist[MAXACTORS]; // FOR CONVERSIONS AS ABOVE (COMPATIBILITY) ONLY


/*
=============================================================================

						 C3_MAIN DEFINITIONS

=============================================================================
*/

extern id0_char_t inlevel[][2];
extern	id0_char_t		str[80],str2[20];
extern	id0_unsigned_t	tedlevelnum;
extern	id0_boolean_t		tedlevel;
extern	gametype	gamestate;
extern	exittype	playstate;
extern 	id0_char_t SlowMode;
extern   id0_unsigned_t Flags;


void NewGame (void);
id0_boolean_t	SaveTheGame(BE_FILE_T file);
id0_boolean_t	LoadTheGame(BE_FILE_T file);
void ResetGame(void);
void ShutdownId (void);
void InitGame (void);
void Quit (const id0_char_t *error, ...);
void TEDDeath(void);
void DemoLoop (void);
void SetupScalePic (id0_unsigned_t picnum);
void SetupScaleWall (id0_unsigned_t picnum);
void SetupScaling (void);
//void main (void);

/*
=============================================================================

						 C3_GAME DEFINITIONS

=============================================================================
*/

extern	id0_unsigned_t	latchpics[NUMLATCHPICS];
extern	id0_unsigned_t	tileoffsets[NUMTILE16];
extern	id0_unsigned_t	textstarts[27];


#define	L_CHARS		0
#define L_NOSHOT	1
#define L_SHOTBAR	2
#define L_NOBODY	3
#define L_BODYBAR	4


void ScanInfoPlane (void);
void ScanText (void);
void SetupGameLevel (void);
void Victory (id0_boolean_t playsounds);
void Died (void);
void NormalScreen (void);
void DrawPlayScreen (void);
void LoadLatchMem (void);
void FizzleFade (id0_unsigned_t source, id0_unsigned_t dest,
	id0_unsigned_t width,id0_unsigned_t height, id0_boolean_t abortable);
void FizzleOut (id0_int_t showlevel);
void FreeUpMemory (void);
void GameLoop (void);
void CacheScaleds (void);
void LatchDrawPic (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t picnum);


/*
=============================================================================

						 C3_PLAY DEFINITIONS

=============================================================================
*/

#define BGF_NIGHT					0x01			// it is officially night
#define BGF_NOT_LIGHTNING		0x02			// lightning flash has ended

extern id0_byte_t BGFLAGS,bcolor;

extern id0_unsigned_t *skycolor,*groundcolor;

extern	ControlInfo	control;
extern	id0_boolean_t		running,slowturn;

extern	id0_int_t			bordertime;

extern	id0_byte_t		tilemap[MAPSIZE][MAPSIZE];
// (REFKEEN) BACKWARDS COMPATIBILITY: Originally used to store objtype
// pointers, as well as 16-bit unsigned integers. We now store just integers
// and convert with a macro when required.
extern id0_unsigned_t actorat[MAPSIZE][MAPSIZE];
//extern	objtype		*actorat[MAPSIZE][MAPSIZE];
extern	id0_byte_t		spotvis[MAPSIZE][MAPSIZE];

// (REFKEEN) new has been renamed newobj since new is a reserved C++ keyword
extern	objtype 	objlist[MAXACTORS],*newobj,*obj,*player;

extern	id0_unsigned_t	farmapylookup[MAPSIZE];
extern	id0_byte_t		*nearmapylookup[MAPSIZE];
extern	id0_byte_t		update[];

extern	id0_boolean_t		godmode,singlestep;
extern	id0_int_t			extravbls;

extern id0_int_t objectcount;
extern	id0_int_t			mousexmove,mouseymove;
extern	id0_int_t			pointcount,pointsleft;
extern	status_flags    status_flag;
extern  id0_int_t             status_delay;

extern 	objtype 		dummyobj;
extern 	id0_short_t 		BeepTime;				//


void CenterWindow(id0_word_t w,id0_word_t h);
void DebugMemory (void);
void PicturePause (void);
id0_int_t  DebugKeys (void);
void CheckKeys (void);
void InitObjList (void);
void GetNewObj (id0_boolean_t usedummy);
void RemoveObj (objtype *gone);
void PollControlls (void);
void PlayLoop (void);
void InitBgChange(id0_short_t stimer, id0_unsigned_t *scolors, id0_short_t gtimer, id0_unsigned_t *gcolors, id0_byte_t flag);

void DisplayStatus (status_flags *stat_flag);

/*
=============================================================================

						 C3_STATE DEFINITIONS

=============================================================================
*/

//void SpawnNewObj (id0_unsigned_t x, id0_unsigned_t y, statetype *state, id0_unsigned_t size);
//void SpawnNewObjFrac (id0_long_t x, id0_long_t y, statetype *state, id0_unsigned_t size);

void Internal_SpawnNewObj (id0_unsigned_t x, id0_unsigned_t y, statetype *state, id0_unsigned_t size,id0_boolean_t UseDummy);
void Internal_SpawnNewObjFrac (id0_long_t x, id0_long_t y, statetype *state, id0_unsigned_t size,id0_boolean_t UseDummy);

#define DSpawnNewObj(x, y, state, size)				Internal_SpawnNewObj(x,y,state,size,true)
#define SpawnNewObj(x, y, state, size)					Internal_SpawnNewObj(x,y,state,size,false)
#define SpawnNewObjFrac(x, y, state, size,Dummy)	Internal_SpawnNewObjFrac(x, y, state, size,false)
#define DSpawnNewObjFrac(x, y, state, size)			Internal_SpawnNewObjFrac(x, y, state, size,true)

id0_boolean_t CheckHandAttack (objtype *ob);
void T_DoDamage (objtype *ob);
id0_boolean_t Walk (objtype *ob);
void ChaseThink (objtype *obj, id0_boolean_t diagonal);
void MoveObj (objtype *ob, id0_long_t move);
id0_boolean_t Chase (objtype *ob, id0_boolean_t diagonal);

extern	dirtype opposite[9];

void ShootActor (objtype *ob, id0_unsigned_t damage);

/*
=============================================================================

						 C3_TRACE DEFINITIONS

=============================================================================
*/

id0_int_t FollowTrace (fixed tracex, fixed tracey, id0_long_t deltax, id0_long_t deltay, id0_int_t max);
id0_int_t BackTrace (id0_int_t finish);
void ForwardTrace (void);
id0_int_t FinishWall (void);
void InsideCorner (void);
void OutsideCorner (void);
void FollowWalls (void);

extern	id0_boolean_t	aborttrace;

/*
=============================================================================

						 C3_DRAW DEFINITIONS

=============================================================================
*/

#define MAXWALLS	50
#define DANGERHIGH	45

#define	MIDWALL		(MAXWALLS/2)

//==========================================================================

extern	tilept	tile,lasttile,focal,left,mid,right;

extern	globpt	edge,view;

extern	id0_unsigned_t screenloc[3];
extern	id0_unsigned_t freelatch;

extern	id0_int_t screenpage;

extern	id0_boolean_t		fizzlein;

extern	id0_long_t lasttimecount;

extern	id0_int_t firstangle,lastangle;

extern	fixed prestep;

extern	id0_int_t traceclip,tracetop;

extern	fixed sintable[ANGLES+ANGLES/4],*costable;

extern	fixed	viewx,viewy,viewsin,viewcos;			// the focal point
extern	id0_int_t	viewangle;

extern	fixed scale,scaleglobal;
extern	id0_unsigned_t slideofs;

extern	id0_int_t zbuffer[VIEWXH+1];

extern	walltype	walls[MAXWALLS],*leftwall,*rightwall;


extern	fixed	tileglobal;
extern	fixed	focallength;
extern	fixed	mindist;
extern	id0_int_t		viewheight;
extern	fixed scale;

extern	id0_int_t	walllight1[NUMFLOORS];
extern	id0_int_t	walldark1[NUMFLOORS];
extern	id0_int_t	walllight2[NUMFLOORS];
extern	id0_int_t	walldark2[NUMFLOORS];

extern id0_unsigned_t topcolor,bottomcolor;

//==========================================================================

void	DrawLine (id0_int_t xl, id0_int_t xh, id0_int_t y,id0_int_t color);
void	DrawWall (walltype *wallptr);
void	TraceRay (id0_unsigned_t angle);
fixed	FixedByFrac (fixed a, fixed b);
void	TransformPoint (fixed gx, fixed gy, id0_int_t *screenx, id0_unsigned_t *screenheight);
fixed	TransformX (fixed gx, fixed gy);
id0_int_t	FollowTrace (fixed tracex, fixed tracey, id0_long_t deltax, id0_long_t deltay, id0_int_t max);
void	ForwardTrace (void);
id0_int_t	FinishWall (void);
id0_int_t	TurnClockwise (void);
id0_int_t	TurnCounterClockwise (void);
void	FollowWall (void);

void	NewScene (void);
void	BuildTables (void);
void	ThreeDRefresh (void);


/*
=============================================================================

						 C3_SCALE DEFINITIONS

=============================================================================
*/


#define COMPSCALECODESTART	(65*6)		// offset to start of code in comp scaler

typedef struct
{
	id0_unsigned_t	codeofs[65];
	id0_unsigned_t	start[65];
	id0_unsigned_t	width[65];
	id0_byte_t		code[];
}	__attribute__((__packed__)) t_compscale;

typedef struct
{
	id0_unsigned_t	width;
	id0_unsigned_t	codeofs[64];
}	__attribute__((__packed__)) t_compshape;


extern id0_unsigned_t	scaleblockwidth,
		scaleblockheight,
		scaleblockdest;

extern	id0_byte_t	plotpix[8];
extern	id0_byte_t	bitmasks1[8][8];
extern	id0_byte_t	bitmasks2[8][8];


extern	t_compscale id0_seg *scaledirectory[NUMSCALEPICS];
extern	t_compshape id0_seg *shapedirectory[NUMSCALEPICS];
extern	memptr			walldirectory[NUMSCALEWALLS];
extern	id0_unsigned_t	shapesize[NUMSCALEPICS];

void 		DeplanePic (id0_int_t picnum);
void ScaleShape (id0_int_t xcenter, t_compshape id0_seg *compshape, id0_unsigned_t scale);
id0_unsigned_t	BuildCompShape (t_compshape id0_seg **finalspot);
id0_unsigned_t BuildCompScale (id0_int_t height, memptr *finalspot);

// (REFKEEN) Replacement for functions generated by BuildCompScale
void ExecuteCompScale(const id0_byte_t *codePtr, id0_unsigned_t egaDestOff, const id0_byte_t *srcPtr, id0_byte_t mask);

/*
=============================================================================

						 C3_ASM DEFINITIONS

=============================================================================
*/

extern	id0_unsigned_t	wallheight	[VIEWWIDTH];
extern	id0_unsigned_t	wallwidth	[VIEWWIDTH];
//extern	id0_unsigned_t	wallseg		[VIEWWIDTH];
extern	id0_byte_t	*wallseg		[VIEWWIDTH];
extern	id0_unsigned_t	wallofs		[VIEWWIDTH];
// REFKEEN - These arrays may actually be larger than VIEWWIDTH cells long,
// and they're used just in C3_ASM.C anyway
//extern	id0_unsigned_t	screenbyte	[VIEWWIDTH];
//extern	id0_unsigned_t	screenbit	[VIEWWIDTH];
extern	id0_unsigned_t	bitmasks	[64];

extern	id0_long_t		wallscalecall;

void	ScaleWalls (void);
void RadarBlip(id0_word_t x, id0_word_t y, id0_word_t color);

/*
=============================================================================

						 C3_WIZ DEFINITIONS

=============================================================================
*/

#define MAXHANDHEIGHT	72

extern statetype s_pshot_exp1;
extern statetype s_pshot_exp2;
extern statetype s_pshot_exp3;

extern	id0_long_t	lastnuke;
extern   id0_int_t lasttext;
extern	id0_int_t		handheight;
extern	id0_int_t		boltsleft,bolttimer;
extern id0_short_t RadarXY[][3];

extern id0_short_t RotateAngle;
extern id0_short_t FreezeTime;

//void FaceDir(id0_short_t x, id0_short_t y, id0_boolean_t StopTime);
//short CalcAngle(id0_short_t dx, id0_short_t dy);

void FaceAngle(id0_short_t DestAngle);
void RotateView();
void InitRotate(id0_short_t DestAngle);
id0_short_t FaceDoor(id0_short_t x, id0_short_t y);

id0_char_t DisplayMsg(const id0_char_t *text,const id0_char_t *choices);
id0_char_t DisplaySMsg(const id0_char_t *text,const id0_char_t *choices);

extern statetype s_explode;

void SpawnExplosion(fixed x, fixed y,id0_short_t Delay);
void T_ExpThink(objtype *obj);
void SpawnBigExplosion(fixed x, fixed y, id0_short_t Delay, fixed Range);

void Thrust (id0_int_t angle, id0_unsigned_t speed);
void CalcBounds (objtype *ob);
void TakeDamage (id0_int_t points);
void GiveBolt (void);
void GiveNuke (void);
void GivePotion (void);
void GiveKey (id0_int_t keytype);
void GiveScroll (id0_int_t scrolltype,id0_boolean_t show);
id0_boolean_t ShotClipMove (objtype *ob, id0_long_t xmove, id0_long_t ymove);
void SpawnPlayer (id0_int_t tilex, id0_int_t tiley, id0_int_t dir);
void RedrawStatusWindow (void);
void StopTime();
void DrawText (id0_boolean_t draw_text_whether_it_needs_it_or_not);
void DrawHealth(void);


/*
=============================================================================

						 C3_ACT1 DEFINITIONS

=============================================================================
*/

id0_int_t EasyHitPoints(id0_int_t NrmHitPts);
id0_int_t EasyDoDamage(id0_int_t Damage);

extern id0_short_t zombie_base_delay;

extern	statetype s_trollouch;
extern	statetype s_trolldie1;


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


extern	statetype s_demonouch;
extern	statetype s_demondie1;

extern	statetype s_mageouch;
extern	statetype s_magedie1;

extern	statetype s_grelouch;
extern	statetype s_greldie1;

extern	statetype s_batdie1;

extern	statetype s_zombie_death1;
extern	statetype s_zombie_ouch;

extern statetype s_zombie_rise1;
extern statetype s_zombie_rise2;
extern statetype s_zombie_rise3;
extern statetype s_zombie_rise4;

extern statetype s_spook0_1;
extern statetype s_spook0_2;
extern statetype s_spook0_3;
extern statetype s_spook0_4;
extern statetype s_spook0;
extern statetype s_spook1;
extern statetype s_spook2;
extern statetype s_spook3;
extern statetype s_spook4;
extern statetype s_spook5;
extern statetype s_spook6;
extern statetype s_spook_pause;
extern statetype s_spook_attack1;
extern statetype s_spook_attack2;
extern statetype s_spook_attack3;
extern statetype s_spookouch;
extern statetype s_spookdie;
extern statetype s_spookdie1;
extern statetype s_spookdie2;
extern statetype s_spookdie3;
extern statetype s_spookdie4;
extern statetype s_spookdie5;
extern statetype s_spookdie6;
extern statetype s_spookdie7;
extern statetype s_spook_wait;


extern statetype s_skel_pause;
extern statetype s_skel_1;
extern statetype s_skel_2;
extern statetype s_skel_3;
extern statetype s_skel_4;
extern statetype s_skel_attack1;
extern statetype s_skel_attack2;
extern statetype s_skel_attack3;
extern statetype s_skel_ouch;
extern statetype s_skel_die1;
extern statetype s_skel_die2;
extern statetype s_skel_die3;

extern statetype s_wet_pause;

extern statetype s_wet_bubbles1;
extern statetype s_wet_bubbles2;
extern statetype s_wet_bubbles3;
extern statetype s_wet_bubbles4;

extern statetype s_wet_peek;

extern statetype s_wet_rise1;
extern statetype s_wet_rise2;
extern statetype s_wet_rise3;
extern statetype s_wet_rise4;
extern statetype s_wet_rise5;

extern statetype s_wet_sink1;
extern statetype s_wet_sink2;
extern statetype s_wet_sink3;

extern statetype s_wet_walk1;
extern statetype s_wet_walk2;
extern statetype s_wet_walk3;
extern statetype s_wet_walk4;

extern statetype s_wet_attack1;
extern statetype s_wet_attack2;
extern statetype s_wet_attack3;
extern statetype s_wet_attack4;

extern statetype s_wet_ouch;

extern statetype s_wet_die1;
extern statetype s_wet_die2;
extern statetype s_wet_die3;
extern statetype s_wet_die4;
extern statetype s_wet_die5;

extern	statetype s_obj_gate1;
extern	statetype s_obj_gate2;
extern	statetype s_obj_gate3;
extern	statetype s_obj_gate4;

extern statetype s_eye_pause;

extern statetype s_eye_1;
extern statetype s_eye_2;
extern statetype s_eye_3;
extern statetype s_eye_4;

//statetype s_eye_attack1 = {SKELETON_ATTACK_1PIC,20,NULL,&s_eye_attack2};
//statetype s_eye_attack2 = {SKELETON_ATTACK_2PIC,10,T_DoDamage,&s_eye_attack3};
//statetype s_eye_attack3 = {SKELETON_ATTACK_3PIC,40,NULL,&s_eye_pause};

extern statetype s_eye_ouch;
extern statetype s_eye_ouch2;

extern statetype s_eye_die1;
extern statetype s_eye_die2;
extern statetype s_eye_die3;

extern	statetype s_mshot1;
extern	statetype s_mshot2;

extern statetype s_bonus_die;

extern	statetype s_red_demonouch;

extern	statetype s_red_demondie1;

void SpawnBonus (id0_int_t tilex, id0_int_t tiley, id0_int_t number);
void SpawnWarp (id0_int_t tilex, id0_int_t tiley, id0_int_t type);
void SpawnTroll (id0_int_t tilex, id0_int_t tiley);
void SpawnOrc (id0_int_t tilex, id0_int_t tiley);
void SpawnBat (id0_int_t tilex, id0_int_t tiley);
void SpawnDemon (id0_int_t tilex, id0_int_t tiley);
void SpawnMage (id0_int_t tilex, id0_int_t tiley);
void SpawnGrelminar (id0_int_t tilex, id0_int_t tiley);
void SpawnTombstone (id0_int_t tilex, id0_int_t tiley, id0_int_t shape);
void SpawnZombie (id0_int_t tilex, id0_int_t tiley);
void SpawnSpook(id0_int_t tilex, id0_int_t tiley);
void SpawnFTime(id0_int_t tilex, id0_int_t tiley);
void SpawnSkeleton(id0_int_t tilex, id0_int_t tiley);
void SpawnWetMan(id0_int_t tilex, id0_int_t tiley);
void SpawnEye(id0_int_t tilex, id0_int_t tiley);
void SpawnWallSkeleton(id0_int_t tilex, id0_int_t tiley);
void SpawnRedDemon (id0_int_t tilex, id0_int_t tiley);
void ExplodeWall (id0_int_t tilex, id0_int_t tiley);

// (REFKEEN) Backwards compatibility: Used for statetype offset conversions.
extern statetype* (*RefKeen_GetObjStatePtrFromDOSPointer)(uint_fast32_t dosptr);
