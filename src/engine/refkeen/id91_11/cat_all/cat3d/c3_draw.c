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

// C3_DRAW.C

#include "c3_def.h"
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

id0_unsigned_t screenloc[3]= {0x900,0x2000,0x3700};
id0_unsigned_t freelatch = 0x4e00;

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

// REFKEEN - Unused functions (in fact, already commented out Catacomb Abyss)
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

id0_int_t	walllight1[NUMFLOORS] = {0,
	WALL1LPIC,WALL2LPIC,WALL3LPIC,WALL4LPIC,WALL5LPIC,WALL6LPIC,WALL7LPIC,
	WALL1LPIC,WALL2LPIC,WALL3LPIC,WALL4LPIC,WALL5LPIC,WALL6LPIC,WALL7LPIC,
	EXPWALL1PIC,EXPWALL2PIC,EXPWALL3PIC,
	RDOOR1PIC,RDOOR2PIC,RDOOR1PIC,RDOOR2PIC,
	YDOOR1PIC,YDOOR2PIC,YDOOR1PIC,YDOOR2PIC,
	GDOOR1PIC,GDOOR2PIC,GDOOR1PIC,GDOOR2PIC,
	BDOOR1PIC,BDOOR2PIC,BDOOR1PIC,BDOOR2PIC};

id0_int_t	walldark1[NUMFLOORS] = {0,
	WALL1DPIC,WALL2DPIC,WALL3DPIC,WALL4DPIC,WALL5DPIC,WALL6DPIC,WALL7DPIC,
	WALL1DPIC,WALL2DPIC,WALL3DPIC,WALL4DPIC,WALL5DPIC,WALL6DPIC,WALL7DPIC,
	EXPWALL1PIC,EXPWALL2PIC,EXPWALL3PIC,
	RDOOR1PIC,RDOOR2PIC,RDOOR1PIC,RDOOR2PIC,
	YDOOR1PIC,YDOOR2PIC,YDOOR1PIC,YDOOR2PIC,
	GDOOR1PIC,GDOOR2PIC,GDOOR1PIC,GDOOR2PIC,
	BDOOR1PIC,BDOOR2PIC,BDOOR1PIC,BDOOR2PIC};

id0_int_t	walllight2[NUMFLOORS] = {0,
	WALL1LPIC,WALL2LPIC,WALL3LPIC,WALL4LPIC,WALL5LPIC,WALL6LPIC,WALL7LPIC,
	WALL1LPIC,WALL2LPIC,WALL3LPIC,WALL4LPIC,WALL5LPIC,WALL6LPIC,WALL7LPIC,
	EXPWALL1PIC,EXPWALL2PIC,EXPWALL3PIC,
	RDOOR2PIC,RDOOR1PIC,RDOOR2PIC,RDOOR1PIC,
	YDOOR2PIC,YDOOR1PIC,YDOOR2PIC,YDOOR1PIC,
	GDOOR2PIC,GDOOR1PIC,GDOOR2PIC,GDOOR1PIC,
	BDOOR2PIC,BDOOR1PIC,BDOOR2PIC,BDOOR1PIC};

id0_int_t	walldark2[NUMFLOORS] = {0,
	WALL1DPIC,WALL2DPIC,WALL3DPIC,WALL4DPIC,WALL5DPIC,WALL6DPIC,WALL7DPIC,
	WALL1DPIC,WALL2DPIC,WALL3DPIC,WALL4DPIC,WALL5DPIC,WALL6DPIC,WALL7DPIC,
	EXPWALL1PIC,EXPWALL2PIC,EXPWALL3PIC,
	RDOOR2PIC,RDOOR1PIC,RDOOR2PIC,RDOOR1PIC,
	YDOOR2PIC,YDOOR1PIC,YDOOR2PIC,YDOOR1PIC,
	GDOOR2PIC,GDOOR1PIC,GDOOR2PIC,GDOOR1PIC,
	BDOOR2PIC,BDOOR1PIC,BDOOR2PIC,BDOOR1PIC};

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
		if (animframe)
			wallpic = walllight2[wallptr->color];
		else
			wallpic = walllight1[wallptr->color];

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
		if (animframe)
			wallpic = walldark2[wallptr->color];
		else
			wallpic = walldark1[wallptr->color];

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
	//
	// clear the screen
	//

	id0_unsigned_t egaDestOff = bufferofs;
	// top loop
	for (int loopVar = CENTERY+1; loopVar; --loopVar)
	{
		// Originally done with pairs of 0x0000 words
		BE_ST_EGAUpdateGFXPixel4bppRepeatedly(egaDestOff, 0, (VIEWWIDTH/16)*2+1, 0xFF);
		egaDestOff += ((VIEWWIDTH/16)*2+1) + (40-VIEWWIDTH/8);
	}
	// bottom loop
	for (int loopVar = CENTERY+1; loopVar; --loopVar)
	{
		// Originally done with pairs of 0x0808 words
		BE_ST_EGAUpdateGFXPixel4bppRepeatedly(egaDestOff, 8, (VIEWWIDTH/16)*2+1, 0xFF);
		egaDestOff += ((VIEWWIDTH/16)*2+1) + (40-VIEWWIDTH/8);
	}
#if 0
asm	mov	dx,GC_INDEX
asm	mov	ax,GC_MODE + 256*2		// read mode 0, write mode 2
asm	out	dx,ax
asm	mov	ax,GC_BITMASK + 255*256
asm	out	dx,ax

asm	mov	dx,40-VIEWWIDTH/8
asm	mov	bl,VIEWWIDTH/16
asm	mov	bh,CENTERY+1

asm	xor	ax,ax
asm	mov	es,[screenseg]
asm	mov	di,[bufferofs]

toploop:
asm	mov	cl,bl
asm	rep	stosw
asm	stosb
asm	add	di,dx
asm	dec	bh
asm	jnz	toploop

asm	mov	bh,CENTERY+1
asm	mov	ax,0x0808

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
	id0_int_t 		i,j,least,numvisable,height;
	objtype 	*obj,**vislist,*farthest;
	memptr		shape;
	id0_byte_t		*tilespot,*visspot;

	numvisable = 0;

//
// calculate base positions of all objects
//
	vislist = &depthsort[0];

	for (obj = player->next;obj;obj=obj->next)
	{
		if (!obj->state->shapenum)
			continue;

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
			obj->active = yes/*true*/;
			TransformActor (obj);
			if (!obj->viewheight || obj->viewheight > VIEWWIDTH)
				continue;			// too close or far away

			*vislist++ = obj;
			numvisable++;
		}
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
	id0_long_t	newtime,oldtimecount;


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
		tics = DEMOTICS;
	}
	else
	{
//
// non demo, so report actual time
//
		newtime = SD_GetTimeCount();
		tics = newtime-lasttimecount;
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
	id0_int_t	picnum;
	memptr source;
	id0_unsigned_t dest,width,height;

	picnum = HAND1PICM;
	if (gamestate.shotpower || boltsleft)
		picnum += (((id0_unsigned_t)SD_GetTimeCount()>>3)&1);

	source = grsegs[picnum];
	dest = ylookup[VIEWHEIGHT-handheight]+12+bufferofs;
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
	animframe = (SD_GetTimeCount()&8)>>3;

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

