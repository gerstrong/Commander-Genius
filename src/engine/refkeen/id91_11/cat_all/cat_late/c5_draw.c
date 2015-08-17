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

// C3_DRAW.C

#include "def.h"
#include "gelib.h" // REFKEEN: For AnimateWallList...
//#pragma hdrstop

//#define DRAWEACH				// draw walls one at a time for debugging

id0_unsigned_t	highest;
id0_unsigned_t	mostwalls,numwalls;

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define PI	3.141592657
#define ANGLEQUAD	(ANGLES/4)

id0_unsigned_t	oldend;

#define FINEANGLES	3600

#define MINRATIO	16


const	id0_unsigned_t	MAXSCALEHEIGHT	= (VIEWWIDTH/2);
const	id0_unsigned_t	MAXVISHEIGHT	= (VIEWHEIGHT/2);
const	id0_unsigned_t	BASESCALE		= 32;

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

//
// calculate location of screens in video memory so they have the
// maximum possible distance seperating them (for scaling overflow)
//

id0_unsigned_t screenloc[3]= {PAGE1START,PAGE2START,PAGE3START};
id0_unsigned_t freelatch = FREESTART;

id0_boolean_t		fizzlein;

//id0_long_t	scaleshapecalll;
//id0_long_t	scaletablecall;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_long_t 	bytecount,endcount;		// for profiling
id0_int_t		animframe;
id0_int_t		pixelangle[VIEWWIDTH];
id0_int_t		id0_far finetangent[FINEANGLES+1];
id0_int_t		fineviewangle;
id0_unsigned_t	viewxpix,viewypix;

/*
============================================================================

			   3 - D  DEFINITIONS

============================================================================
*/

fixed	tileglobal	= TILEGLOBAL;
fixed	focallength	= FOCALLENGTH;
fixed	mindist		= MINDIST;
id0_int_t		viewheight	= VIEWHEIGHT;
fixed scale;


tilept	tile,lasttile,		// tile of wall being followed
	focal,			// focal point in tiles
	left,mid,right;		// rightmost tile in view

globpt edge,view;

id0_int_t	segstart[VIEWHEIGHT],	// addline tracks line segment and draws
	segend[VIEWHEIGHT],
	segcolor[VIEWHEIGHT];	// only when the color changes


walltype	walls[MAXWALLS],*leftwall,*rightwall;


//==========================================================================

//
// refresh stuff
//

id0_int_t screenpage;

id0_long_t lasttimecount;

//
// rendering stuff
//

id0_int_t firstangle,lastangle;

fixed prestep;

fixed sintable[ANGLES+ANGLES/4],*costable = sintable+(ANGLES/4);

fixed	viewx,viewy;			// the focal point
id0_int_t	viewangle;
fixed	viewsin,viewcos;

id0_int_t	zbuffer[VIEWXH+1];	// holds the height of the wall at that point

//==========================================================================

void	DrawLine (id0_int_t xl, id0_int_t xh, id0_int_t y,id0_int_t color);
void	DrawWall (walltype *wallptr);
void	TraceRay (id0_unsigned_t angle);
fixed	FixedByFrac (fixed a, fixed b);
fixed	FixedAdd (void);
fixed	TransformX (fixed gx, fixed gy);
id0_int_t		FollowTrace (fixed tracex, fixed tracey, id0_long_t deltax, id0_long_t deltay, id0_int_t max);
id0_int_t		BackTrace (id0_int_t finish);
void	ForwardTrace (void);
id0_int_t		TurnClockwise (void);
id0_int_t		TurnCounterClockwise (void);
void	FollowWall (void);

void	NewScene (void);
void	BuildTables (void);

//==========================================================================


#if 0
/*
==================
=
= DrawLine
=
= Must be in write mode 2 with all planes enabled
= The bit mask is left set to the end value, so clear it after all lines are
= drawn
=
= draws a black dot at the left edge of the line
=
==================
*/

id0_unsigned_t static	id0_char_t dotmask[8] = {0x80,0x40,0x20,0x10,8,4,2,1};
id0_unsigned_t static	id0_char_t leftmask[8] = {0xff,0x7f,0x3f,0x1f,0xf,7,3,1};
id0_unsigned_t static	id0_char_t rightmask[8] = {0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};

void DrawLine (id0_int_t xl, id0_int_t xh, id0_int_t y,id0_int_t color)
{
  id0_unsigned_t dest,xlp,xlb,xhb,maskleft,maskright,maskdot,mid;

  xlb=xl/8;
  xhb=xh/8;

  if (xh<xl)
	Quit("DrawLine: xh<xl");
  if (y<VIEWY)
	Quit("DrawLine: y<VIEWY");
  if (y>VIEWYH)
	Quit("DrawLine: y>VIEWYH");

	xlp = xl&7;
	maskleft = leftmask[xlp];
	maskright = rightmask[xh&7];

  mid = xhb-xlb-1;
  dest = bufferofs+ylookup[y]+xlb;

	//
	// set the GC index register to point to the bit mask register
	//
	asm	mov	al,GC_BITMASK
	asm	mov	dx,GC_INDEX
	asm	out	dx,al

  if (xlb==xhb)
  {
  //
  // entire line is in one byte
  //

	maskleft&=maskright;

	asm	mov	es,[screenseg]
	asm	mov	di,[dest]
	asm	mov	dx,GC_INDEX+1

	asm	mov	al,[BYTE PTR maskleft]
	asm	out	dx,al		// mask off pixels

	asm	mov	al,[BYTE PTR color]
	asm	xchg	al,[es:di]	// load latches and write pixels

	return;
  }

asm	mov	es,[screenseg]
asm	mov	di,[dest]
asm	mov	dx,GC_INDEX+1
asm	mov	bh,[BYTE PTR color]

//
// draw left side
//
asm	mov	al,[BYTE PTR maskleft]
asm	out	dx,al		// mask off pixels

asm	mov	al,bh
asm	xchg	al,[es:di]	// load latches and write pixels
asm	inc	di

//
// draw middle
//
asm	mov	al,255
asm	out	dx,al		// no masking

asm	mov	al,bh
asm	mov	cx,[mid]
asm	rep	stosb

//
// draw right side
//
asm	mov	al,[BYTE PTR maskright]
asm	out	dx,al		// mask off pixels
asm	xchg	bh,[es:di]	// load latches and write pixels

}

//==========================================================================

void DrawLineDot (id0_int_t xl, id0_int_t xh, id0_int_t y,id0_int_t color)
{
  id0_unsigned_t dest,xlp,xlb,xhb,maskleft,maskright,maskdot,mid;

  xlb=xl/8;
  xhb=xh/8;

  if (xh<xl)
	Quit("DrawLine: xh<xl");
  if (y<VIEWY)
	Quit("DrawLine: y<VIEWY");
  if (y>VIEWYH)
	Quit("DrawLine: y>VIEWYH");

	xlp = xl&7;
	maskdot = dotmask[xlp];
	maskleft = leftmask[xlp];
	maskright = rightmask[xh&7];

  mid = xhb-xlb-1;
  dest = bufferofs+ylookup[y]+xlb;

	//
	// set the GC index register to point to the bit mask register
	//
	asm	mov	al,GC_BITMASK
	asm	mov	dx,GC_INDEX
	asm	out	dx,al

  if (xlb==xhb)
  {
  //
  // entire line is in one byte
  //

	maskleft&=maskright;

	asm	mov	es,[screenseg]
	asm	mov	di,[dest]
	asm	mov	dx,GC_INDEX+1

	asm	mov	al,[BYTE PTR maskleft]
	asm	out	dx,al		// mask off pixels

	asm	mov	al,[BYTE PTR color]
	asm	xchg	al,[es:di]	// load latches and write pixels


	//
	// write the black dot at the start
	//
	asm	mov	al,[BYTE PTR maskdot]
	asm	out	dx,al		// mask off pixels

	asm	xor	al,al
	asm	xchg	al,[es:di]	// load latches and write pixels


	return;
  }

asm	mov	es,[screenseg]
asm	mov	di,[dest]
asm	mov	dx,GC_INDEX+1
asm	mov	bh,[BYTE PTR color]

//
// draw left side
//
asm	mov	al,[BYTE PTR maskleft]
asm	out	dx,al		// mask off pixels

asm	mov	al,bh
asm	xchg	al,[es:di]	// load latches and write pixels

//
// write the black dot at the start
//
asm	mov	al,[BYTE PTR maskdot]
asm	out	dx,al		// mask off pixels
asm	xor	al,al
asm	xchg	al,[es:di]	// load latches and write pixels
asm	inc	di

//
// draw middle
//
asm	mov	al,255
asm	out	dx,al		// no masking

asm	mov	al,bh
asm	mov	cx,[mid]
asm	rep	stosb

//
// draw right side
//
asm	mov	al,[BYTE PTR maskright]
asm	out	dx,al		// mask off pixels
asm	xchg	bh,[es:di]	// load latches and write pixels

}

#endif

//==========================================================================


//id0_long_t		wallscalesource;

#ifdef DRAWEACH
static uint8_t *wallscalesourceptr;
/*
====================
=
= ScaleOneWall
=
====================
*/

void near ScaleOneWall (id0_int_t xl, id0_int_t xh)
{
	id0_int_t	x,pixwidth,height;

	//*(((id0_unsigned_t *)&wallscalesource)+1) = wallseg[xl];

	for (x=xl;x<=xh;x+=pixwidth)
	{
		height = wallheight[x];
		pixwidth = wallwidth[x];
		//(id0_unsigned_t)wallscalesource = wallofs[x];
		wallscalesourceptr = wallseg[xl]+wallofs[x];

		//*(((id0_unsigned_t *)&scaletablecall)+1) = (id0_unsigned_t)scaledirectory[height];
		(//id0_unsigned_t)scaletablecall = scaledirectory[height]->codeofs[0];

		id0_unsigned_t egaDestOff = (((id0_unsigned_t)x)>>3) + bufferofs;
		// (REFKEEN) Replacement for functions generated by BuildCompScale
		ExecuteCompScale(&scaledirectory[height]->code[0], egaDestOff, wallscalesourceptr, bitmasks1[x&7][pixwidth-1]);
		if (bitmasks2[x&7][pixwidth-1])
		{
			//
			// draw a second byte for vertical strips that cross two bytes
			//
			++egaDestOff;
			// (REFKEEN) Replacement for functions generated by BuildCompScale
			ExecuteCompScale(&scaledirectory[height]->code[0], egaDestOff, wallscalesourceptr, bitmasks2[x&7][pixwidth-1]);
		}
#if 0
		//
		// scale a byte wide strip of wall
		//
		asm	mov	bx,[x]
		asm	mov	di,bx
		asm	shr	di,1
		asm	shr	di,1
		asm	shr	di,1						// X in bytes
		asm	add	di,[bufferofs]
		asm	and	bx,7
		asm	shl	bx,1
		asm	shl	bx,1
		asm	shl	bx,1
		asm	add	bx,[pixwidth]				// bx = pixel*8+pixwidth-1
		asm	dec	bx
		asm	mov	al,BYTE PTR [bitmasks1+bx]
		asm	mov	dx,GC_INDEX+1
		asm	out	dx,al						// set bit mask register
		asm	mov	es,[screenseg]
		asm	lds	si,[wallscalesource]
		asm	call [DWORD PTR ss:scaletablecall]		// scale the line of pixels

		asm	mov	al,BYTE PTR [ss:bitmasks2+bx]
		asm	or	al,al
		asm	jz	nosecond

		//
		// draw a second byte for vertical strips that cross two bytes
		//
		asm	inc	di
		asm	out	dx,al						// set bit mask register
		asm	call [DWORD PTR ss:scaletablecall]	// scale the line of pixels
	nosecond:
		asm	mov	ax,ss
		asm	mov	ds,ax
#endif
	}
}

#endif

id0_char_t wall_anim_pos[NUMFLOORS];

#ifdef REFKEEN_VER_CATARM

// EAST / WEST WALLS
//
id0_int_t     id0_far walllight1[NUMFLOORS] = {0,

	CRYSTAL_LIGHT_1PIC,
	CRYSTAL_LIGHT_2PIC,
	CRYSTAL_LIGHT_3PIC,
	CRYSTAL_LIGHT_4PIC,			//4

	FIRE_WALL_1PIC,
	FIRE_WALL_2PIC,
	FIRE_WALL_3PIC,
	FIRE_WALL_4PIC,				//8

	BRN_STONE_GATEPIC,
	BRN_STONE_WALL_1PIC,
	KUDZU_WEAK_LIGHTPIC,
	KUDZU_LIGHT_WALLPIC,
	HEDGE_WALLPIC,
	HEDGE_EYESPIC,    			//14

	W_GEN_DOOR1PIC,					//15
	BRN_WINDOW_LIGHTPIC,

	ALTAR_LEFTPIC,
	ALTAR_RIGHTPIC,
	GRAY_LIGHT_WALLPIC,
	GRAY_LIGHT_SIGNPIC,			//20

	MANICLE_LIGHT_WALLPIC,
	MANICLE_LIGHT_BLOODYPIC,

	LIGHT_CURTAIN_WINDOWPIC,
	LIGHT_CURTAIN_WALLPIC,
	BRN_LIGHT_SIGNPIC,			//25

	LIGHT_STONE_WALLPIC,

	W_GEN_DOOR2PIC,					//27

	TROLL_LIGHT_STONEPIC,

	BRN_FLAGSTONE_LIGHT_2PIC,

	W_CRYSTAL_DOORPIC,

	DMG_BRN_FSTN_LTPIC,

	RUST_METAL_LIGHTPIC,
	GRAY_METAL_LIGHTPIC,			//33

	WEAK_STONE_LIGHTPIC,

	DMG_FIN_FSTN_LTPIC,

	WEAK_GRAY_RFGSTN_LIGHTPIC,
	0,

	WEAK_CRYSTAL_LIGHTPIC,

	RED_MUD_LIGHTPIC,

	STEEL_DOOR1PIC,				//40

	RED_MUD_WEAK_LIGHTPIC,

	STEEL_DOOR2PIC,                         //42

	HORN_DOORPIC,
	TROLL_BLOODY_LT_STONEPIC,
	CLOSED_DOOR_1PIC,

	GRY_DOOR_LTPIC,				//46

	BRN_DOOR_LTPIC,                         //47

	GRY_FGSTN_LTPIC,                           //48
	DOOR_2PIC,

	WATER_LIGHT_WEAK_1PIC,
	WATER_LIGHT_WEAK_2PIC,
	WATER_LIGHT_WEAK_3PIC,			//52

	WATER_LIGHT_1PIC,
	WATER_LIGHT_2PIC,
	WATER_LIGHT_3PIC,

	LIGHT_BREATH_1PIC,
	LIGHT_BREATH_2PIC,
	LIGHT_BREATH_3PIC,			//58

	EXP_WALL_1PIC,
	EXP_WALL_2PIC,
	EXP_WALL_3PIC,

	WATER_EXP_WALL_1PIC,
	WATER_EXP_WALL_2PIC,
	WATER_EXP_WALL_3PIC,			//64

	FINALWALLPIC,

	LT_SKEL1PIC,
	DK_SKEL1PIC,
	LT_SKEL2PIC,
	DK_SKEL2PIC,

	0,

	TAP_1PIC,
	TAP_2PIC,
	TAP_3PIC,
	TAP_4PIC,
	TAP_5PIC,

	WATER_DOOR1_PIC,
	WATER_DOOR2_PIC,
	};

// NORTH / SOUTH WALLS
//
id0_int_t     id0_far walldark1[NUMFLOORS] = {0,

	CRYSTAL_DARK_1PIC,
	CRYSTAL_DARK_2PIC,
	CRYSTAL_DARK_3PIC,
	CRYSTAL_DARK_4PIC,			//4

	FIRE_WALL_1PIC,
	FIRE_WALL_2PIC,
	FIRE_WALL_3PIC,
	FIRE_WALL_4PIC,				//8

	BRN_STONE_GATEPIC,
	BRN_STONE_WALL_2PIC,
	KUDZU_WEAK_DARKPIC,
	KUDZU_DARK_WALLPIC,
	HEDGE_WALLPIC,
	HEDGE_EYESPIC,				//14

	W_GEN_DOOR1PIC,				//15
	BRN_WINDOW_DARKPIC,

	ALTAR_LEFTPIC,
	ALTAR_RIGHTPIC,
	GRAY_DARK_WALLPIC,
	GRAY_DARK_SIGNPIC,			//20

	MANICLE_DARK_WALLPIC,
	MANICLE_DARK_BLOODYPIC,

	DARK_CURTAIN_WINDOWPIC,
	DARK_CURTAIN_WALLPIC,
	BRN_DARK_SIGNPIC,

	DARK_STONE_WALLPIC,

	W_GEN_DOOR2PIC,				//27

	TROLL_DARK_STONEPIC,

	BRN_FLAGSTONE_DARK_2PIC,

	W_CRYSTAL_DOORPIC,				//30

	DMG_BRN_FSTN_DKPIC,

	RUST_METAL_DARKPIC,
	GRAY_METAL_DARKPIC,

	WEAK_STONE_DARKPIC,

	DMG_FIN_FSTN_DKPIC,			//35

	WEAK_GRAY_RFGSTN_DARKPIC,
	0,

	WEAK_CRYSTAL_DARKPIC,

	BRN_MUD_DARKPIC,

	STEEL_DOOR1PIC,				//40

	BRN_MUD_WEAK_DARKPIC,

	STEEL_DOOR2PIC,

	HORN_DOORPIC,
	TROLL_BLOODY_DK_STONEPIC,

	CLOSED_DOOR_1PIC,

	GRY_DOOR_DKPIC,				//46
	BRN_DOOR_DKPIC,				//47
	GRY_FGSTN_DKPIC,				//48
	DOOR_2PIC,

	WATER_DARK_WEAK_1PIC,
	WATER_DARK_WEAK_2PIC,
	WATER_DARK_WEAK_3PIC,

	WATER_DARK_1PIC,
	WATER_DARK_2PIC,
	WATER_DARK_3PIC,

	DARK_BREATH_1PIC,
	DARK_BREATH_2PIC,
	DARK_BREATH_3PIC,

	EXP_WALL_1PIC,
	EXP_WALL_2PIC,
	EXP_WALL_3PIC,

	WATER_EXP_WALL_1PIC,
	WATER_EXP_WALL_2PIC,
	WATER_EXP_WALL_3PIC,

	FINALWALLPIC,

	LT_SKEL1PIC,
	DK_SKEL1PIC,
	LT_SKEL2PIC,
	DK_SKEL2PIC,

	0,

	TAP_1PIC,
	TAP_2PIC,
	TAP_3PIC,
	TAP_4PIC,
	TAP_5PIC,

	WATER_DOOR1_PIC,
	WATER_DOOR2_PIC,
	};

#elif defined REFKEEN_VER_CATAPOC

// EAST / WEST WALLS
//
id0_int_t     id0_far walllight1[NUMFLOORS] = {0,

	0,//CRYSTAL1LIGHTPIC,
	0,//EGYPT1LIGHTPIC,
	EGYPT2LIGHTPIC,
	EGYPT3LIGHTPIC,

	FIREWALL1PIC,
	FIREWALL2PIC,
	FIREWALL3PIC,
	FIREWALL4PIC,


	NEMESISPIC,

	ALTARLEFTPIC,
	ALTARRIGHTPIC,

	TEMPLEWALLLIGHTPIC,

	TORCHWALL1PIC,
	TORCHWALL2PIC,

	BRNBRKLIGHTPIC,
	BRNBRKEMLIGHTPIC,

	IRONGATEPIC,

	BRNFLGLIGHTPIC,
	BRNFLGWINDOWLIGHTPIC,
	BRNFLGVINELIGHTPIC,
	BRNFLGDMGLIGHTPIC,

	SPACEDMG1LIGHTPIC,
	SPACEDMG2LIGHTPIC,

	SPACE1LIGHTPIC,
	SPACE2LIGHTPIC,
	SPACE3LIGHTPIC,
	SPACE4LIGHTPIC,

	SPACE5LIGHTPIC,
	SPACE6LIGHTPIC,
	SPACE7LIGHTPIC,
	SPACE8LIGHTPIC,

	0,//SPACE9LIGHTPIC,
	0,//SPACEDMG9LIGHTPIC,
	SPACE10LIGHTPIC,
	RUSTDOORLIGHTPIC,

	SPACE11LIGHTPIC,
	SPACE12LIGHTPIC,
	SPACE13LIGHTPIC,
	SPACE14LIGHTPIC,

	SPACEDMG5LIGHTPIC,
	SPACEDMG6LIGHTPIC,

	TAP1PIC,
	TAP2PIC,
	ENDPIC,
	0,//SIRONLIGHTPIC,

	SPCDOOR1LIGHTPIC,
	SPCDOOR2LIGHTPIC,
	SPCDOOR3LIGHTPIC,
	SPCDOOR4LIGHTPIC,

	COLUMNSLIGHTPIC,

	DEMONSTATUELIGHTPIC,

	0,//CRYSTALBWALL1LIGHTPIC,

	0,//SRUSTLIGHTPIC,

	TROLLSTATUELIGHTPIC,

	BRNDMGVINELIGHTPIC,
	TAP3PIC,
	HORNDOORPIC,
	RUNEDOORPIC,

	EXP_WALL_1PIC,
	EXP_WALL_2PIC,
	EXP_WALL_3PIC,
	WATER_EXP_WALL_1PIC,
	WATER_EXP_WALL_2PIC,
	WATER_EXP_WALL_3PIC,

	IRONDMGLIGHTPIC,
	IRONLIGHTPIC,
	0,
	TROLLBLOODYLIGHTPIC,
	TROLLLIGHTPIC,

	0,												// INVISIBLE WALL

	STONEDOORLIGHTPIC,
	0,

	IRONWTR1LIGHTPIC,
	IRONWTR2LIGHTPIC,
	IRONWTR3LIGHTPIC,

	RUSTWTR1LIGHTPIC,
	RUSTWTR2LIGHTPIC,
	RUSTWTR3LIGHTPIC,

	CEMETARYLIGHTPIC,
	0,	//	STAIRDWNLIGHTPIC,

	WGRATE1LIGHTPIC,
	WGRATE2LIGHTPIC,
	WGRATE3LIGHTPIC,

	MAS_WIN_LIGHTPIC,
	MAS_DOOR_LIGHTPIC,
	MAS_VINE1_LIGHTPIC,
	MAS_VINE2_LIGHTPIC,

  // Start of non-solid walls
	0,
	0,
	0,
	0,
	0,
	0,

  // solid walls
	SGRATEPIC,
};

// NORTH / SOUTH WALLS
//
id0_int_t     id0_far walldark1[NUMFLOORS] = {0,

	0,//CRYSTAL1DARKPIC,
	0,//EGYPT1DARKPIC,
	EGYPT2DARKPIC,
	EGYPT3DARKPIC,

	FIREWALL1PIC,
	FIREWALL2PIC,
	FIREWALL3PIC,
	FIREWALL4PIC,

	NEMESISPIC,

	ALTARLEFTPIC,
	ALTARRIGHTPIC,

	TEMPLEWALLDARKPIC,

	TORCHWALL1PIC,
	TORCHWALL2PIC,

	BRNBRKDARKPIC,
	BRNBRKEMDARKPIC,

	IRONGATEPIC,

	BRNFLGDARKPIC,
	BRNFLGWINDOWDARKPIC,
	BRNFLGVINEDARKPIC,
	BRNFLGDMGDARKPIC,

	SPACEDMG1DARKPIC,
	SPACEDMG2DARKPIC,

	SPACE1DARKPIC,
	SPACE2DARKPIC,
	SPACE3DARKPIC,
	SPACE4DARKPIC,

	SPACE5DARKPIC,
	SPACE6DARKPIC,
	SPACE7DARKPIC,
	SPACE8DARKPIC,

	0,//SPACE9DARKPIC,
	0,//SPACEDMG9DARKPIC,
	SPACE10DARKPIC,
	RUSTDOORDARKPIC,

	SPACE11DARKPIC,
	SPACE12DARKPIC,
	SPACE13DARKPIC,
	SPACE14DARKPIC,

	SPACEDMG5DARKPIC,
	SPACEDMG6DARKPIC,

	TAP1PIC,
	TAP2PIC,
	ENDPIC,
	0,//SIRONDARKPIC,

	SPCDOOR1DARKPIC,
	SPCDOOR2DARKPIC,
	SPCDOOR3DARKPIC,
	SPCDOOR4DARKPIC,

	COLUMNSDARKPIC,

	DEMONSTATUEDARKPIC,

	0,//CRYSTALBWALL1DARKPIC,

	0,//SRUSTDARKPIC,

	TROLLSTATUEDARKPIC,

	BRNDMGVINEDARKPIC,
	TAP3PIC,
	HORNDOORPIC,
	RUNEDOORPIC,

	EXP_WALL_1PIC,
	EXP_WALL_2PIC,
	EXP_WALL_3PIC,

	WATER_EXP_WALL_1PIC,
	WATER_EXP_WALL_2PIC,
	WATER_EXP_WALL_3PIC,

	IRONDMGDARKPIC,
	IRONDARKPIC,
	0,
	TROLLBLOODYDARKPIC,

	TROLLDARKPIC,

	0,											// INVISIBLE WALL

	STONEDOORDARKPIC,
	0,

	IRONWTR1DARKPIC,
	IRONWTR2DARKPIC,
	IRONWTR3DARKPIC,

	RUSTWTR1DARKPIC,
	RUSTWTR2DARKPIC,
	RUSTWTR3DARKPIC,

	CEMETARYDARKPIC,
	0,

	WGRATE1DARKPIC,
	WGRATE2DARKPIC,
	WGRATE3DARKPIC,

	MAS_WIN_DARKPIC,
	MAS_DOOR_DARKPIC,
	MAS_VINE1_DARKPIC,
	MAS_VINE2_DARKPIC,

  // Start of non-solid walls
	0,
	0,
	0,
	0,
	0,
	0,

  // solid walls
	SGRATEPIC,
};

#endif // REFKEEN_VER_CATARM/APOC

/*
=====================
=
= DrawVWall
=
= Draws a wall by vertical segments, for texture mapping!
=
= wallptr->side is true for east/west walls (constant x)
=
= fracheight and fracstep are 16.16 bit fractions
=
=====================
*/

void DrawVWall (walltype *wallptr)
{
	id0_int_t			x/*,i*/;
	id0_unsigned_t	source;
	id0_unsigned_t	width/*,sourceint*/;
	id0_unsigned_t	wallpic/*,wallpicseg*/;
	memptr wallpicseg; // REFKEEN - Safer
	id0_unsigned_t	skip;
	id0_long_t		fracheight,fracstep,longheightchange;
	id0_unsigned_t	height;
	id0_int_t			heightchange;
	id0_unsigned_t	slope,distance;
	id0_int_t			traceangle,angle;
	id0_int_t			mapadd;
	id0_unsigned_t	lastpix,lastsource,lastwidth;

	if (wallptr->rightclip < wallptr->leftclip)
		Quit ("DrawVWall: Right < Left");

//
// setup for height calculation
//
	wallptr->height1 >>= 1;
	wallptr->height2 >>= 1;
	wallptr->planecoord>>=10;			// remove non significant bits

	width = wallptr->x2 - wallptr->x1;
	if (width)
	{
		heightchange = wallptr->height2 - wallptr->height1;
		longheightchange = (id0_longword_t)(((id0_longword_t)((id0_word_t)heightchange)) << 16);
#if 0
		asm	mov	ax,[heightchange]
		asm	mov	WORD PTR [longheightchange+2],ax
		asm	mov	WORD PTR [longheightchange],0	// avoid long shift by 16
#endif
		fracstep = longheightchange/width;
	}

	fracheight = ((id0_long_t)wallptr->height1<<16)+0x8000;
	skip = wallptr->leftclip - wallptr->x1;
	if (skip)
		fracheight += fracstep*skip;

//
// setup for texture mapping
//
// mapadd is 64*64 (to keep source positive) + the origin wall intercept
// distance has 6 unit bits, and 6 frac bits
// traceangle is the center view angle in FINEANGLES, moved to be in
// the +-90 degree range (to thew right of origin)
//
	traceangle = fineviewangle;
	//
	// find wall picture to map from
	//
	if (wallptr->side)
	{	// east or west wall

		wallpic = walllight1[wallptr->color+wall_anim_pos[wallptr->color]];
		if (wallptr->planecoord < viewxpix)
		{
			distance = viewxpix-wallptr->planecoord;
			traceangle -= FINEANGLES/2;
			mapadd = (64-viewypix&63);		// the pixel spot of the origin
		}
		else
		{
			distance = wallptr->planecoord-viewxpix;
			// traceangle is correct
			mapadd = viewypix&63;		// the pixel spot of the origin
		}
	}
	else
	{	// north or south wall

		wallpic = walldark1[wallptr->color+wall_anim_pos[wallptr->color]];
		if (wallptr->planecoord < viewypix)
		{
			distance = viewypix-wallptr->planecoord;
			traceangle -= FINEANGLES/4;
			mapadd = viewxpix&63;		// the pixel spot of the origin
		}
		else
		{
			distance = wallptr->planecoord-viewypix;
			traceangle -= FINEANGLES*3/4;
			mapadd = (64-viewxpix&63);		// the pixel spot of the origin
		}
	}

	mapadd = 64*64-mapadd;				// make sure it stays positive

	wallpicseg = walldirectory[wallpic-FIRSTWALLPIC];
	//wallpicseg = (id0_unsigned_t)walldirectory[wallpic-FIRSTWALLPIC];
	if (traceangle > FINEANGLES/2)
		traceangle -= FINEANGLES;

//
// calculate everything
//
// IMPORTANT!  This loop is executed around 5000 times / second!
//
	lastpix = lastsource = (id0_unsigned_t)-1;

	for (x = wallptr->leftclip ; x <= wallptr->rightclip ; x++)
	{
		//
		// height
		//
#if 0
		asm	mov	ax,WORD PTR [fracheight]
		asm	mov	dx,WORD PTR [fracheight+2]
		asm	mov	cx,dx
		asm	add	ax,WORD PTR [fracstep]
		asm	adc	dx,WORD PTR [fracstep+2]
		asm	mov	WORD PTR [fracheight],ax
		asm	mov	WORD PTR [fracheight+2],dx
		asm	mov	bx,[x]
		asm	shl	bx,1
		asm	cmp	cx,MAXSCALEHEIGHT
		asmf	jbe	storeheight
		asm	mov	cx,MAXSCALEHEIGHT
storeheight:
		asm	mov WORD PTR [wallheight+bx],cx
		asm	mov WORD PTR [zbuffer+bx],cx
#endif
		height = ((id0_longword_t)fracheight)>>16;
		fracheight += fracstep;
		if (height > MAXSCALEHEIGHT)
			height = MAXSCALEHEIGHT;
		wallheight[x] = zbuffer[x] = height;

		//
		// texture map
		//
		angle = pixelangle[x]+traceangle;
		if (angle<0)
			angle+=FINEANGLES;

		slope = finetangent[angle];

//
// distance is an unsigned 6.6 bit number (12 pixel bits)
// slope is a signed 5.10 bit number
// result is a signed 11.16 bit number
//

		source = distance*slope;
		source = (source&0xFF00)|((source&0xFF00)>>8);
		source >>=2;

		source += mapadd;
		source &= 63;				// mask off the unused units
		source = 63-source;
		source <<= 6;				// multiply by 64 for offset into pic
#if 0
#if 0
		source = distance*slope;
		source >>=20;

		source += mapadd;
		source &= 63;				// mask off the unused units
		source = 63-source;
		source <<= 6;				// multiply by 64 for offset into pic
#endif
		asm	mov	ax,[distance]
		asm	imul	[slope]			// ax is the source pixel
		asm	mov	al,ah
		asm	shr	al,1
		asm	shr	al,1				// low 6 bits is now pixel number
		asm	add	ax,[mapadd]
		asm	and ax,63
		asm	mov	dx,63
		asm	sub	dx,ax				// otherwise it is backwards
		asm	shl	dx,1
		asm	shl	dx,1
		asm	shl	dx,1
		asm	shl	dx,1
		asm	shl	dx,1
		asm	shl	dx,1				// *64 to index into shape
		asm	mov	[source],dx
#endif

		if (source != lastsource)
		{
			if (lastpix != (id0_unsigned_t)-1)
			{
				wallofs[lastpix] = lastsource;
				wallseg[lastpix] = (id0_byte_t *)wallpicseg;
				wallwidth[lastpix] = lastwidth;
			}
			lastpix = x;
			lastsource = source;
			lastwidth = 1;
		}
		else
			lastwidth++;			// optimized draw, same map as last one
	}
	wallofs[lastpix] = lastsource;
	wallseg[lastpix] = (id0_byte_t *)wallpicseg;
	wallwidth[lastpix] = lastwidth;
}


//==========================================================================


/*
=================
=
= TraceRay
=
= Used to find the left and rightmost tile in the view area to be traced from
= Follows a ray of the given angle from viewx,viewy in the global map until
= it hits a solid tile
= sets:
=   tile.x,tile.y	: tile coordinates of contacted tile
=   tilecolor	: solid tile's color
=
==================
*/

id0_int_t tilecolor;

void TraceRay (id0_unsigned_t angle)
{
  id0_long_t tracex,tracey,tracexstep,traceystep,searchx,searchy;
  fixed fixtemp;
  id0_int_t otx,oty,searchsteps;

  tracexstep = costable[angle];
  traceystep = sintable[angle];

//
// advance point so it is even with the view plane before we start checking
//
  fixtemp = FixedByFrac(prestep,tracexstep);
  tracex = viewx+fixtemp;
  fixtemp = FixedByFrac(prestep,traceystep);
  tracey = viewy-fixtemp;

  tile.x = tracex>>TILESHIFT;	// starting point in tiles
  tile.y = tracey>>TILESHIFT;


  if (tracexstep<0)			// use 2's complement, not signed magnitude
	tracexstep = -(tracexstep&0x7fffffff);

  if (traceystep<0)			// use 2's complement, not signed magnitude
	traceystep = -(traceystep&0x7fffffff);

//
// we assume viewx,viewy is not inside a solid tile, so go ahead one step
//

  do	// until a solid tile is hit
  {
    otx = tile.x;
	oty = tile.y;
	spotvis[otx][oty] = true;
	tracex += tracexstep;
    tracey -= traceystep;
    tile.x = tracex>>TILESHIFT;
	tile.y = tracey>>TILESHIFT;

	if (tile.x!=otx && tile.y!=oty && (tilemap[otx][tile.y] || tilemap[tile.x][oty]) )
    {
      //
	  // trace crossed two solid tiles, so do a binary search along the line
	  // to find a spot where only one tile edge is crossed
      //
      searchsteps = 0;
      searchx = tracexstep;
      searchy = traceystep;
      do
      {
	searchx/=2;
	searchy/=2;
	if (tile.x!=otx && tile.y!=oty)
	{
	 // still too far
	  tracex -= searchx;
	  tracey += searchy;
	}
	else
	{
	 // not far enough, no tiles crossed
	  tracex += searchx;
	  tracey -= searchy;
	}

	//
	// if it is REAL close, go for the most clockwise intersection
	//
	if (++searchsteps == 16)
	{
	  tracex = (id0_long_t)otx<<TILESHIFT;
	  tracey = (id0_long_t)oty<<TILESHIFT;
	  if (tracexstep>0)
	  {
		if (traceystep<0)
		{
		  tracex += TILEGLOBAL-1;
		  tracey += TILEGLOBAL;
		}
		else
		{
		  tracex += TILEGLOBAL;
		}
	  }
	  else
	  {
		if (traceystep<0)
		{
		  tracex --;
		  tracey += TILEGLOBAL-1;
		}
		else
		{
		  tracey --;
		}
	  }
	}

	tile.x = tracex>>TILESHIFT;
	tile.y = tracey>>TILESHIFT;

	  } while (( tile.x!=otx && tile.y!=oty) || (tile.x==otx && tile.y==oty) );
	}
  } while (!(tilecolor = tilemap[tile.x][tile.y]) );

}

//==========================================================================


/*
========================
=
= FixedByFrac
=
= multiply a 16/16 bit, 2's complement fixed point number by a 16 bit
= fraction, passed as a signed magnitude 32 bit number
=
========================
*/

//#pragma warn -rvl			// I stick the return value in with ASMs

fixed FixedByFrac (fixed a, fixed b)
{
	int result_sign = (b < 0) ? -1 : 1; // sign of result == sign of fraction
	id0_longword_t a_as_unsigned = a;
	if (a < 0) // negative?
	{
		//2's complement...
		a_as_unsigned = -a;
		//a_as_unsigned ^= -1;
		//++a_as_unsigned;
		result_sign *= -1; // toggle sign of result
	}
	//
	// Multiply a_as_unsigned by the low 8 bits of b
	//
	id0_word_t b_lo = b&0xFFFF;
	id0_longword_t result = b_lo*(a_as_unsigned>>16) + ((b_lo*(a_as_unsigned&0xFFFF))>>16);
	//id0_longword_t result = b_lo*(a_as_unsigned>>16) + b_lo*(a_as_unsigned&0xFFFF);
	//
	// put result in 2's complement
	//
	if (result_sign < 0) // Is the result negative?
	{
		//2's complement...
		result = -result;
		//result ^= -1;
		//++result;
	}
	return result;
#if 0
  fixed value;

//
// setup
//
asm	mov	si,[WORD PTR b+2]	// sign of result = sign of fraction

asm	mov	ax,[WORD PTR a]
asm	mov	cx,[WORD PTR a+2]

asm	or	cx,cx
asm	jns	aok:				// negative?
asm	not	ax
asm	not	cx
asm	add	ax,1
asm	adc	cx,0
asm	xor	si,0x8000			// toggle sign of result
aok:

//
// multiply  cx:ax by bx
//
asm	mov	bx,[WORD PTR b]
asm	mul	bx					// fraction*fraction
asm	mov	di,dx				// di is low word of result
asm	mov	ax,cx				//
asm	mul	bx					// units*fraction
asm add	ax,di
asm	adc	dx,0

//
// put result dx:ax in 2's complement
//
asm	test	si,0x8000		// is the result negative?
asm	jz	ansok:
asm	not	ax
asm	not	dx
asm	add	ax,1
asm	adc	dx,0

ansok:;
#endif
}

//#pragma warn +rvl

#if 0
/*
=========================
=
= FixedAdd
=
= add two 16 bit fixed point numbers
= to subtract, invert the sign of B before invoking
=
=========================
*/

fixed FixedAdd (fixed a, fixed b)
{
  fixed value;

asm	mov	ax,[WORD PTR a]
asm	mov	dx,[WORD PTR a+2]

asm	mov	bx,[WORD PTR b]
asm	mov	cx,[WORD PTR b+2]

asm	or	dx,dx
asm	jns	aok:		// negative?
asm	and	dx,0x7fff
asm	not	ax		// convert a from signed magnitude to 2's compl
asm	not	dx
asm	add	ax,1
asm	adc	dx,0
aok:

asm	or	cx,cx
asm	jns	bok:		// negative?
asm	and	cx,0x7fff
asm	not	bx		// convert b from signed magnitude to 2's compl
asm	not	cx
asm	add	bx,1
asm	adc	cx,0
bok:

asm	add	ax,bx		// perform the addition
asm	adc	dx,cx
asm	jns	done

asm	and	dx,0x7fff	// value was negative
asm	not	ax		// back to signed magnitude
asm	not	dx
asm	add	ax,1
asm	adc	dx,0

done:

asm	mov	[WORD PTR value],ax
asm	mov	[WORD PTR value+2],dx

  return value;
}
#endif

//==========================================================================


/*
========================
=
= TransformPoint
=
= Takes paramaters:
=   gx,gy		: globalx/globaly of point
=
= globals:
=   viewx,viewy		: point of view
=   viewcos,viewsin	: sin/cos of viewangle
=
=
= defines:
=   CENTERX		: pixel location of center of view window
=   TILEGLOBAL		: size of one
=   FOCALLENGTH		: distance behind viewx/y for center of projection
=   scale		: conversion from global value to screen value
=
= returns:
=   screenx,screenheight: projected edge location and size
=
========================
*/

void TransformPoint (fixed gx, fixed gy, id0_int_t *screenx, id0_unsigned_t *screenheight)
{
  id0_int_t ratio;
  fixed gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
  gx = gx-viewx;
  gy = gy-viewy;

//
// calculate newx
//
  gxt = FixedByFrac(gx,viewcos);
  gyt = FixedByFrac(gy,viewsin);
  nx = gxt-gyt;

//
// calculate newy
//
  gxt = FixedByFrac(gx,viewsin);
  gyt = FixedByFrac(gy,viewcos);
  ny = gyt+gxt;

//
// calculate perspective ratio
//
  if (nx<0)
	nx = 0;

  ratio = nx*scale/FOCALLENGTH;

  if (ratio<=MINRATIO)
	ratio = MINRATIO;

  *screenx = CENTERX + ny/ratio;

  *screenheight = TILEGLOBAL/ratio;

}


//
// transform actor
//
void TransformActor (objtype *ob)
{
  id0_int_t ratio;
  fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
  gx = ob->x-viewx;
  gy = ob->y-viewy;

//
// calculate newx
//
  gxt = FixedByFrac(gx,viewcos);
  gyt = FixedByFrac(gy,viewsin);
  nx = gxt-gyt-ob->size;

//
// calculate newy
//
  gxt = FixedByFrac(gx,viewsin);
  gyt = FixedByFrac(gy,viewcos);
  ny = gyt+gxt;

//
// calculate perspective ratio
//
  if (nx<0)
	nx = 0;

  ratio = nx*scale/FOCALLENGTH;

  if (ratio<=MINRATIO)
	ratio = MINRATIO;

  ob->viewx = CENTERX + ny/ratio;

  ob->viewheight = TILEGLOBAL/ratio;
}

//==========================================================================

fixed TransformX (fixed gx, fixed gy)
{
  //id0_int_t ratio;
  fixed gxt,gyt/*,nx,ny*/;

//
// translate point to view centered coordinates
//
  gx = gx-viewx;
  gy = gy-viewy;

//
// calculate newx
//
  gxt = FixedByFrac(gx,viewcos);
  gyt = FixedByFrac(gy,viewsin);

  return gxt-gyt;
}

//==========================================================================

/*
==================
=
= BuildTables
=
= Calculates:
=
= scale			projection constant
= sintable/costable	overlapping fractional tables
= firstangle/lastangle	angles from focalpoint to left/right view edges
= prestep		distance from focal point before checking for tiles
=
==================
*/

void BuildTables (void)
{
  id0_int_t 		i;
  id0_long_t		intang;
  id0_long_t		x;
  float 	angle,anglestep,radtoint;
  double	tang;
  fixed 	value;

//
// calculate the angle offset from view angle of each pixel's ray
//
	radtoint = (float)FINEANGLES/2/PI;
	for (i=0;i<VIEWWIDTH/2;i++)
	{
	// start 1/2 pixel over, so viewangle bisects two middle pixels
		x = (TILEGLOBAL*i+TILEGLOBAL/2)/VIEWWIDTH;
		tang = (float)x/(FOCALLENGTH+MINDIST);
		angle = atan(tang);
		intang = angle*radtoint;
		pixelangle[VIEWWIDTH/2-1-i] = intang;
		pixelangle[VIEWWIDTH/2+i] = -intang;
	}


	// FIXME (REFKEEN) Should replace this with pre-constructed table
#ifdef MININT
#undef MININT
#endif
#ifdef MAXINT
#undef MAXINT
#endif

#define MAXINT 32767
//
// calculate fine tangents
// 1 sign bit, 5 units (clipped to), 10 fracs
//
#define MININT	(-MAXINT)

	for (i=0;i<FINEANGLES/4;i++)
	{
		intang = tan(i/radtoint)*(1l<<10);

		//
		// if the tangent is not reprentable in this many bits, bound the
		// units part ONLY
		//
		if (intang>MAXINT)
			intang = 0x8f00 | (intang & 0xff);
		else if (intang<MININT)
			intang = 0xff00 | (intang & 0xff);

		finetangent[i] = intang;
//		finetangent[FINEANGLES/2+i] = intang;
//		finetangent[FINEANGLES/2-i-1] = -intang;
		finetangent[FINEANGLES-i-1] = -intang;
	}

//
// calculate scale value so one tile at mindist allmost fills the view horizontally
//
  scale = GLOBAL1/VIEWWIDTH;
  scale *= focallength;
  scale /= (focallength+mindist);

//
// costable overlays sintable with a quarter phase shift
// ANGLES is assumed to be divisable by four
//
// The low word of the value is the fraction, the high bit is the sign bit,
// bits 16-30 should be 0
//

  angle = 0;
  anglestep = PI/2/ANGLEQUAD;
  for (i=0;i<=ANGLEQUAD;i++)
  {
	value=GLOBAL1*sin(angle);
	sintable[i]=
	  sintable[i+ANGLES]=
	  sintable[ANGLES/2-i] = value;
	sintable[ANGLES-i]=
	  sintable[ANGLES/2+i] = value | 0x80000000l;
	angle += anglestep;
  }

//
// figure trace angles for first and last pixel on screen
//
  angle = atan((float)VIEWWIDTH/2*scale/FOCALLENGTH);
  angle *= ANGLES/(PI*2);

  intang = (id0_int_t)angle+1;
  firstangle = intang;
  lastangle = -intang;

  prestep = GLOBAL1*((float)FOCALLENGTH/costable[firstangle]);

//
// misc stuff
//
  walls[0].x2 = VIEWX-1;
  walls[0].height2 = 32000;
}


//==========================================================================

/*
=====================
=
= ClearScreen
=
=====================
*/

void ClearScreen (void)
{
	id0_unsigned_t topcolor=*skycolor, bottomcolor=*groundcolor;
#if USE_STRIPS
	id0_unsigned_t topimage=topcolor&0xf0,bottomimage=bottomcolor&0xf0;
	id0_unsigned_t pfoffset=0;
#endif


#if USE_STRIPS
	if (topimage == 0x20)		// special code for lightning
		topimage = topcolor = 0;

// Manually wipe screen with solid color.
// If BOTH sky and ground are 'images' don't manually clear it!
//
	if ((!topimage) || (!bottomimage))
	{
#endif

	//
	// clear the screen
	//

	id0_unsigned_t egaDestOff = bufferofs + ((SCREENWIDTH*VIEWY)+(VIEWX/8));
	// top loop
	for (int loopVar = CENTERY+1; loopVar; --loopVar)
	{
		// (REFKEEN) DIFFERENCE FROM VANILLA:
		// topcolor aka *sycolor is a 16-bit word which is actually a pair of colors (e.g., 0x0404),
		// but we ignore one of the two for the sake of simplicity and a bit better performance, since
		// skycolor always points to a member of sky_colors, where each pair has the exact same color repeated twice.
		//
		BE_ST_EGAUpdateGFXPixel4bppRepeatedly(egaDestOff, (id0_byte_t)topcolor, (VIEWWIDTH/16)*2+1, 0xFF);
		// no need to add (40-VIEWWIDTH/8) i.e., 0 modulo
		egaDestOff += ((VIEWWIDTH/16)*2+1) /*+ (40-VIEWWIDTH/8)*/;
	}
	// bottom loop
	for (int loopVar = CENTERY+1; loopVar; --loopVar)
	{
		// (REFKEEN) DIFFERENCE FROM VANILLA: Same as above but with bottomcolor
		BE_ST_EGAUpdateGFXPixel4bppRepeatedly(egaDestOff, (id0_byte_t)bottomcolor, (VIEWWIDTH/16)*2+1, 0xFF);
		// no need to add (40-VIEWWIDTH/8) i.e., 0 modulo
		egaDestOff += ((VIEWWIDTH/16)*2+1) /*+ (40-VIEWWIDTH/8)*/;
	}

#if USE_STRIPS
	}


//
// code to test parallax turning
//

	if (topimage)
	{
		topimage -= 16;
		pfoffset = LONG_PERCENTAGE(3200,359,(359-player->angle),12);
		while (pfoffset >= 640)
			pfoffset -= 640;
		LatchDrawPicStrip(0,0,SKY1PIC+topimage,pfoffset+8);
	}

	if (bottomimage)
	{
////		pfoffset = LONG_PERCENTAGE(3200,359,(359-player->angle),12)+320;
//		pfoffset += 320;
//		while (pfoffset >= 640)
//			pfoffset -= 640;
//		LatchDrawPicStrip(0,64,SKY1PIC+topimage,pfoffset+8);
		bottomimage -= 16;
		LatchDrawPic(0,64,GND1PIC+bottomimage);
	}
#endif

#if 0
asm	mov	dx,GC_INDEX
asm	mov	ax,GC_MODE + 256*2		// read mode 0, write mode 2
asm	out	dx,ax
asm	mov	ax,GC_BITMASK + 255*256
asm	out	dx,ax

asm	mov	dx,40-VIEWWIDTH/8
asm	mov	bl,VIEWWIDTH/16
asm	mov	bh,CENTERY+1

asm	mov	ax,topcolor
asm	mov	es,[screenseg]
asm	mov	di,[bufferofs]
asm	add	di,((SCREENWIDTH*VIEWY)+(VIEWX/8))

toploop:
asm	mov	cl,bl
asm	rep	stosw
asm	stosb
asm	add	di,dx
asm	dec	bh
asm	jnz	toploop

asm	mov	bh,CENTERY+1
asm	mov	ax,bottomcolor

bottomloop:
asm	mov	cl,bl
asm	rep	stosw
asm	stosb
asm	add	di,dx
asm	dec	bh
asm	jnz	bottomloop


asm	mov	dx,GC_INDEX
asm	mov	ax,GC_MODE + 256*10		// read mode 1, write mode 2
asm	out	dx,ax
asm	mov	al,GC_BITMASK
asm	out	dx,al
#endif

}

//==========================================================================

/*
=====================
=
= DrawWallList
=
= Clips and draws all the walls traced this refresh
=
=====================
*/

void DrawWallList (void)
{
	id0_int_t /*i,*/leftx,newleft,rightclip;
	walltype *wall, *check;

	memset(wallwidth, 0, 2*(VIEWWIDTH/2));
#if 0
asm	mov	ax,ds
asm	mov	es,ax
asm	mov	di,OFFSET wallwidth
asm	xor	ax,ax
asm	mov	cx,VIEWWIDTH/2
asm	rep	stosw
#endif

	ClearScreen ();

	rightwall->x1 = VIEWXH+1;
	rightwall->height1 = 32000;
	(rightwall+1)->x1 = 32000;

	leftx = -1;

	for (wall=&walls[1];wall<rightwall && leftx<=VIEWXH ;wall++)
	{
	  if (leftx >= wall->x2)
		continue;

	  rightclip = wall->x2;

	  check = wall+1;
	  while (check->x1 <= rightclip && check->height1 >= wall->height2)
	  {
		rightclip = check->x1-1;
		check++;
	  }

	  if (rightclip>VIEWXH)
		rightclip=VIEWXH;

	  if (leftx < wall->x1 - 1)
		newleft = wall->x1-1;		// there was black space between walls
	  else
		newleft = leftx;

	  if (rightclip > newleft)
	  {
		wall->leftclip = newleft+1;
		wall->rightclip = rightclip;
		DrawVWall (wall);
		leftx = rightclip;
	  }
	}

#ifndef DRAWEACH
	ScaleWalls ();					// draw all the walls
#endif
}

//==========================================================================

/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visable
=
=====================
*/

objtype *depthsort[MAXACTORS];

void DrawScaleds (void)
{
#if USE_INERT_LIST
		extern inertobjtype inertobjlist[], *inert;

		id0_boolean_t inertlist=false;
#endif
	id0_int_t 		i,j,least,numvisable,height;
	objtype 	*obj,**vislist,*farthest;
	memptr		shape;
	id0_byte_t		*tilespot,*visspot;

	numvisable = 0;

//
// calculate base positions of all objects
//
	vislist = &depthsort[0];

	obj = player->next;
	while (obj)
	{
		// (REFKEEN) WORKAROUND FOR VANILLA BUG (TODO EMULATE BEHAVIORS?):
		// obj->tiley may be very large (say 65534), seems to be reproduced
		// as a right-shift of obj->y with signed value of -95792 by 16 bits.
		//
		// Workaround itself: Add a few conditions.
		tilespot = &tilemap[0][0]+(obj->tilex<<6)+obj->tiley;
		visspot = &spotvis[0][0]+(obj->tilex<<6)+obj->tiley;
		//
		// could be in any of the nine surrounding tiles
		//
		if ((obj->tilex < MAPSIZE) && (obj->tiley < MAPSIZE) && /* WORKAROUND/FIX */
		   (*visspot
		|| ( *(visspot-1) && !*(tilespot-1) )
		|| ( *(visspot+1) && !*(tilespot+1) )
		|| ( *(visspot-65) && !*(tilespot-65) )
		|| ( *(visspot-64) && !*(tilespot-64) )
		|| ( *(visspot-63) && !*(tilespot-63) )
		|| ( *(visspot+65) && !*(tilespot+65) )
		|| ( *(visspot+64) && !*(tilespot+64) )
		|| ( *(visspot+63) && !*(tilespot+63) ) ))
		{
#if USE_INERT_LIST
			if (!inertlist)
#endif
				if ((obj->active == noalways) || (obj->active == always))
					obj->active = always;
				else
					obj->active = yes;
			TransformActor (obj);
			if (!obj->viewheight || obj->viewheight > VIEWWIDTH)
				goto cont;			// too close or far away

			if (!obj->state->shapenum)
				goto cont;

			*vislist++ = obj;
			numvisable++;
		}
		else
#if USE_INERT_LIST
			if (!inertlist)
#endif
				if ((obj->active != always) && (obj->active != noalways))
					obj->active = no;

cont:;
		obj = obj->next;
#if USE_INERT_LIST
		if ((!obj) && (!inertlist))
		{
			if (inert != inertobjlist)
				obj = (objtype *)inertobjlist;
			inertlist = true;
		}
#endif
	}

	if (vislist == &depthsort[0])
		return;						// no visable objects

//
// draw from back to front
//
	for (i = 0; i<numvisable; i++)
	{
		least = 32000;
		for (j=0;j<numvisable;j++)
		{
			height = depthsort[j]->viewheight;
			if (height < least)
			{
				least = height;
				farthest = depthsort[j];
			}
		}
		//
		// draw farthest
		//
		shape = shapedirectory[farthest->state->shapenum-FIRSTSCALEPIC];
		ScaleShape(farthest->viewx,(t_compshape *)shape,farthest->viewheight);
		farthest->viewheight = 32000;
	}
}

//==========================================================================


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
	id0_long_t	newtime/*,oldtimecount*/;


#ifdef PROFILE
	tics = 1;
	return;
#endif

//
// calculate tics since last refresh for adaptive timing
//
	// REFKEEN - Looks like this is an unsigned comparison in original EXE
	if ((id0_longword_t)lasttimecount > SD_GetTimeCount())
		SD_SetTimeCount(lasttimecount);		// if the game was paused a LONG time

#if 0
	if (DemoMode)					// demo recording and playback needs
	{								// to be constant
//
// take DEMOTICS or more tics, and modify Timecount to reflect time taken
//
		oldtimecount = lasttimecount;
		BE_ST_TimeCountWaitForDest(oldtimecount+DEMOTICS*2);
#if 0
		while (TimeCount<oldtimecount+DEMOTICS*2)
		;
#endif
		lasttimecount = oldtimecount + DEMOTICS;
		SD_SetTimeCount(lasttimecount + DEMOTICS);
		realtics = tics = DEMOTICS;
	}
	else
#endif
	{
//
// non demo, so report actual time
//
		newtime = SD_GetTimeCount();
		realtics = tics = newtime-lasttimecount;
		lasttimecount = newtime;

#ifdef FILEPROFILE
			strcpy (scratch,"\tTics:");
			itoa (tics,str,10);
			strcat (scratch,str);
			strcat (scratch,"\n");
			write (profilehandle,scratch,strlen(scratch));
#endif

		if (tics>MAXTICS)
		{
			SD_SetTimeCount(SD_GetTimeCount() - (tics-MAXTICS));
			tics = MAXTICS;
		}

		if (realtics>MAXREALTICS)
			realtics = MAXREALTICS;
	}
}


//==========================================================================


/*
========================
=
= DrawHand
=
========================
*/

void	DrawHand (void)
{
	#define HAND_X_POS	((VIEWWIDTH/16)-(10/2))		// "10" = hand width in bytes

	#define picnum HAND1PICM

	memptr source;
	id0_unsigned_t dest,width,height;

//	if (gamestate.shotpower || boltsleft)
//		picnum += (((id0_unsigned_t)SD_GetTimeCount()>>3)&1);

	source = grsegs[picnum];
	dest = ylookup[VIEWHEIGHT-handheight]+HAND_X_POS+bufferofs;			// 12
	width = picmtable[picnum-STARTPICM].width;
	height = picmtable[picnum-STARTPICM].height;

	VW_MaskBlock(source,0,dest,width,handheight,width*height);
	EGAMAPMASK(15);
}

//==========================================================================


/*
========================
=
= ThreeDRefresh
=
========================
*/

void	ThreeDRefresh (void)
{
	id0_int_t tracedir;

restart:
	aborttrace = false;

//
// clear out the traced array
//
	memset(spotvis, 0, 64*mapwidth);
#if 0
asm	mov	ax,ds
asm	mov	es,ax
asm	mov	di,OFFSET spotvis
asm	xor	ax,ax
asm	mov	cx,[mapwidth]		// mapheight*32 words
asm	shl	cx,1
asm	shl	cx,1
asm	shl	cx,1
asm	shl	cx,1
asm	shl	cx,1
asm	rep stosw
#endif


//
// set up variables for this view
//

	viewangle = player->angle;
	fineviewangle = viewangle*(FINEANGLES/ANGLES);
	viewsin = sintable[viewangle];
	viewcos = costable[viewangle];
	viewx = player->x - FixedByFrac(FOCALLENGTH,viewcos);
	viewy = player->y + FixedByFrac(FOCALLENGTH,viewsin);
	viewx &= 0xfffffc00;		// stop on a pixel boundary
	viewy &= 0xfffffc00;
	viewx += 0x180;
	viewy += 0x180;
	viewxpix = viewx>>10;
	viewypix = viewy>>10;

	focal.x = viewx>>TILESHIFT;
	focal.y = viewy>>TILESHIFT;

//
// find the rightmost visable tile in view
//
	tracedir = viewangle + lastangle;
	if (tracedir<0)
	  tracedir+=ANGLES;
	else if (tracedir>=ANGLES)
	  tracedir-=ANGLES;
	TraceRay( tracedir );
	right.x = tile.x;
	right.y = tile.y;

//
// find the leftmost visable tile in view
//
	tracedir = viewangle + firstangle;
	if (tracedir<0)
	  tracedir+=ANGLES;
	else if (tracedir>=ANGLES)
	  tracedir-=ANGLES;
	TraceRay( tracedir );

//
// follow the walls from there to the right
//
	rightwall = &walls[1];
	FollowWalls ();

	if (aborttrace)
		goto restart;

//
// actually draw stuff
//
	if (++screenpage == 3)
		screenpage = 0;

	bufferofs = screenloc[screenpage];

	EGAWRITEMODE(2);
	EGAMAPMASK(15);

//
// draw the wall list saved be FollowWalls ()
//
//	animframe = (SD_GetTimeCount()&8)>>3;

//
// draw all the scaled images
//
#if 0
	asm	mov	dx,GC_INDEX

	asm	mov	ax,GC_COLORDONTCARE
	asm	out	dx,ax						// don't look at any of the planes

	asm	mov	ax,GC_MODE + 256*(10)		// read mode 1, write mode 2
	asm	out	dx,ax

	asm	mov	al,GC_BITMASK
	asm	out	dx,al
#endif

	AnimateWallList();
	DrawWallList();
	DrawScaleds();

	EGAWRITEMODE(0);
	EGABITMASK(0xff);

//
// draw hand
//
	if (handheight)
		DrawHand ();

//
// show screen and time last cycle
//
	if (fizzlein)
	{
		fizzlein = false;
		FizzleFade(bufferofs,displayofs,VIEWWIDTH,VIEWHEIGHT,true);
		lasttimecount = SD_GetTimeCount();
		// (REFKEEN) Minor difference from vanilla Catacomb
		if (MousePresent)
		{
			BE_ST_GetMouseDelta(NULL, NULL); // Clear accumulated mouse movement
		}
		//if (MousePresent) Mouse(MDelta);	// Clear accumulated mouse movement
	}

	BE_ST_SetScreenStartAddress(bufferofs);
#if 0
asm	cli
asm	mov	cx,[bufferofs]
asm	mov	dx,3d4h		// CRTC address register
asm	mov	al,0ch		// start address high register
asm	out	dx,al
asm	inc	dx
asm	mov	al,ch
asm	out	dx,al   	// set the high byte
asm	dec	dx
asm	mov	al,0dh		// start address low register
asm	out	dx,al
asm	inc	dx
asm	mov	al,cl
asm	out	dx,al		// set the low byte
asm	sti
#endif

	displayofs = bufferofs;

	CalcTics ();

}

