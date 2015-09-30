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

// ID_VW.C

#include "id_heads.h"

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define VIEWWIDTH		40

#define PIXTOBLOCK		4		// 16 pixels to an update block

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

cardtype	videocard;		// set by VW_Startup
// REFKEEN - Related code won't compile as C++ as-is with unused grtype
int grmode; // TEXTGR, CGAGR, EGAGR, VGAGR
//grtype		grmode;			// CGAgr, EGAgr, VGAgr

id0_unsigned_t	bufferofs;		// hidden area to draw to before displaying
id0_unsigned_t	displayofs;		// origin of the visable screen
id0_unsigned_t	panx,pany;		// panning adjustments inside port in pixels
id0_unsigned_t	pansx,pansy;	// panning adjustments inside port in screen
							// block limited pixel values (ie 0/8 for ega x)
id0_unsigned_t	panadjust;		// panx/pany adjusted by screen resolution

id0_byte_t *screenseg;
//id0_unsigned_t	screenseg;		// normally 0xa000 / 0xb800
id0_unsigned_t	linewidth;
id0_unsigned_t	ylookup[VIRTUALHEIGHT];

id0_unsigned_t	fontnumber;		// 0 based font number for drawing

id0_boolean_t		screenfaded;

pictabletype	id0_seg *pictable;
pictabletype	id0_seg *picmtable;
spritetabletype id0_seg *spritetable;

id0_int_t			bordercolor;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void	VWL_MeasureString (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend, id0_word_t *width, id0_word_t *height,
		fontstruct id0_seg *font);
void 	VWL_DrawCursor (void);
void 	VWL_EraseCursor (void);
void 	VWL_DBSetup (void);
void	VWL_UpdateScreenBlocks (void);


//id0_int_t			bordercolor;
id0_int_t			cursorvisible;
id0_int_t			cursornumber,cursorwidth,cursorheight,cursorx,cursory;
memptr		cursorsave;
id0_unsigned_t	cursorspot;

//===========================================================================


/*
=======================
=
= VW_Startup
=
=======================
*/

static	const id0_char_t *ParmStrings[] = {"HIDDENCARD",""};

void	VW_Startup (void)
{
	id0_int_t i;

	// Originally used for certain ASM code loops (clears direction flag)
	//asm	cld;

	videocard = NOcard/*0*/;

	for (i = 1;i < id0_argc;i++)
		if (US_CheckParm(id0_argv[i],ParmStrings) == 0)
		{
			videocard = EGAcard;
			break;
		}

	if (!videocard)
		videocard = VW_VideoID ();

#if GRMODE == EGAGR
	grmode = EGAGR;
	if (videocard != EGAcard && videocard != VGAcard)
Quit ("Improper video card!  If you really have an EGA/VGA card that I am not \n"
	  "detecting, use the -HIDDENCARD command line parameter!");
	EGAWRITEMODE(0);
#endif

#if GRMODE == CGAGR
	grmode = CGAGR;
	if (videocard < CGAcard || videocard > VGAcard)
Quit ("Improper video card!  If you really have a CGA card that I am not \n"
	  "detecting, use the -HIDDENCARD command line parameter!");
	MM_GetPtr ((memptr *)&screenseg,0x10000l);	// grab 64k for floating screen
#endif

	cursorvisible = 0;
}

//===========================================================================

/*
=======================
=
= VW_Shutdown
=
=======================
*/

void	VW_Shutdown (void)
{
	VW_SetScreenMode (TEXTGR);
#if GRMODE == EGAGR
	VW_SetLineWidth (80);
#endif
}

//===========================================================================

/*
========================
=
= VW_SetScreenMode
= Call BIOS to set TEXT / CGAgr / EGAgr / VGAgr
=
========================
*/

void VW_SetScreenMode (id0_int_t grmode)
{
	switch (grmode)
	{
	  case TEXTGR:
		BE_ST_SetScreenMode(3);
		screenseg=BE_ST_GetTextModeMemoryPtr();
		break;
	  case CGAGR:
		BE_ST_SetScreenMode(4);
		// screenseg is actually a main mem buffer
		break;
	  case EGAGR:
#ifdef REFKEEN_VER_CATADVENTURES
	  case EGA320GR:						// MDM start (GAMERS EDGE)
		MaxX=320;
		MaxY=200;
#endif
		BE_ST_SetScreenMode(0xd/*|128 for EGA320GR, MDM*/);
		// REFKEEN no need to obtain screenseg
		// - different EGA planes are accessed with new functions
		break;
#ifdef REFKEEN_VER_CATADVENTURES
	  case EGA640GR:
		MaxX=640;
		MaxY=200;
		BE_ST_SetScreenMode(0xe/*|128*/);
		// REFKEEN no need to obtain screenseg
		// - different EGA planes are accessed with new functions
		break;						 		// MDM end
#endif

#ifdef VGAGAME
	  case VGAGR:{
		  id0_char_t extern VGAPAL;	// deluxepaint vga pallet .OBJ file
		  void id0_far *vgapal = &VGAPAL;
		  SetCool256 ();		// custom 256 color mode
		  screenseg=0xa000;
		  _ES = FP_SEG(vgapal);
		  _DX = FP_OFF(vgapal);
		  _BX = 0;
		  _CX = 0x100;
		  _AX = 0x1012;
		  geninterrupt(0x10);			// set the deluxepaint pallet

		  break;
#endif
	}
	VW_SetLineWidth(SCREENWIDTH);
}

/*
=============================================================================

							SCREEN FADES

=============================================================================
*/

id0_char_t colors[7][17]=
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,0},
 {0,0,0,0,0,0,0,0,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0},
 {0,1,2,3,4,5,6,7,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0},
 {0,1,2,3,4,5,6,7,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0},
 {0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f}};


void VW_ColorBorder (id0_int_t color)
{
	BE_ST_SetBorderColor(color);
	bordercolor = color;
}

void VW_SetPalette(id0_byte_t *palette)
{
	id0_byte_t	p;
	id0_word_t	i;

	for (i = 0;i < 15;i++)
	{
		p = palette[i];
		colors[0][i] = 0;
		colors[1][i] = (p > 0x10)? (p & 0x0f) : 0;
		colors[2][i] = (p > 0x10)? p : 0;
		colors[3][i] = p;
		colors[4][i] = (p > 0x10)? 0x1f : p;
		colors[5][i] = 0x1f;
	}
}

void VW_SetDefaultColors(void)
{
#if GRMODE == EGAGR
	colors[3][16] = bordercolor;
	BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[3]);
	screenfaded = false;
#endif
}


void VW_FadeOut(void)
{
#if GRMODE == EGAGR
	id0_int_t i;

	for (i=3;i>=0;i--)
	{
	  colors[i][16] = bordercolor;
	  BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[i]);
	  VW_WaitVBL(6);
	}
	screenfaded = true;
#endif
}


void VW_FadeIn(void)
{
#if GRMODE == EGAGR
	id0_int_t i;

	for (i=0;i<4;i++)
	{
	  colors[i][16] = bordercolor;
	  BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[i]);
	  VW_WaitVBL(6);
	}
	screenfaded = false;
#endif
}

void VW_FadeUp(void)
{
#if GRMODE == EGAGR
	id0_int_t i;

	for (i=3;i<6;i++)
	{
	  colors[i][16] = bordercolor;
	  BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[i]);
	  VW_WaitVBL(6);
	}
	screenfaded = true;
#endif
}

void VW_FadeDown(void)
{
#if GRMODE == EGAGR
	id0_int_t i;

	for (i=5;i>2;i--)
	{
	  colors[i][16] = bordercolor;
	  BE_ST_EGASetPaletteAndBorder((id0_byte_t *)&colors[i]);
	  VW_WaitVBL(6);
	}
	screenfaded = false;
#endif
}

// REFKEEN - UNUSED

#if 0
/*
========================
=
= VW_SetAtrReg
=
= Sets an attribute (pallete / border) register
= Does NOT vsync!
=
========================
*/

void VW_SetAtrReg (id0_int_t reg, id0_int_t value)
{
  asm	cli
  asm	mov	dx,STATUS_REGISTER_1
  asm	in	al,dx
  asm	mov	dx,ATR_INDEX

  asm	mov	al,BYTE PTR [reg]
  asm	out	dx,al
  asm	mov	al,BYTE PTR [value]
  asm	out	dx,al
  asm	mov	dx,0x3da
  asm	in	al,dx
  asm	mov	dx,ATR_INDEX
  asm	mov	al,0x20
  asm	out	dx,al
  asm	sti
}
#endif



//===========================================================================

/*
====================
=
= VW_SetLineWidth
=
= Must be an even number of bytes
=
====================
*/

void VW_SetLineWidth (id0_int_t width)
{
  id0_int_t i,offset;

#if GRMODE == EGAGR
//
// set wide virtual screen
//
	BE_ST_EGASetLineWidth(width); // Ported from ASM
#endif

//
// set up lookup tables
//
  linewidth = width;

  offset = 0;

  for (i=0;i<VIRTUALHEIGHT;i++)
  {
	ylookup[i]=offset;
	offset += width;
  }
}


//===========================================================================

/*
====================
=
= VW_SetSplitScreen
=
====================
*/

void VW_SetSplitScreen (id0_int_t linenum)
{
	VW_WaitVBL (1);
	if (videocard==VGAcard)
		linenum=linenum*2-1;
	BE_ST_EGASetSplitScreen(linenum);
#if 0 
	outportb (CRTC_INDEX,CRTC_LINECOMPARE);
	outportb (CRTC_INDEX+1,linenum % 256);
	outportb (CRTC_INDEX,CRTC_OVERFLOW);
	outportb (CRTC_INDEX+1, 1+16*(linenum/256));
	if (videocard==VGAcard)
	{
		outportb (CRTC_INDEX,CRTC_MAXSCANLINE);
		outportb (CRTC_INDEX+1,inportb(CRTC_INDEX+1) & (255-64));
	}
#endif
}

//===========================================================================

/*
====================
=
= VW_ClearVideo
=
====================
*/

void	VW_ClearVideo (id0_int_t color)
{
#if GRMODE == EGAGR
	EGAWRITEMODE(2);
	EGAMAPMASK(15);
#endif

#if GRMODE == EGAGR
	BE_ST_EGAUpdateGFXPixel4bppRepeatedly(0, color, 0xffff, 0xff);
#endif
#if GRMODE == CGAGR
	memset(screenseg, color, 0xffff);
	BE_ST_MarkGfxForPendingUpdate();
#endif


#if GRMODE == EGAGR
	EGAWRITEMODE(0);
#endif
}

//===========================================================================

#if NUMPICS>0

/*
====================
=
= VW_DrawPic
=
= X in bytes, y in pixels, chunknum is the #defined picnum
=
====================
*/

void VW_DrawPic(id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t chunknum)
{
	id0_int_t	picnum = chunknum - STARTPICS;
	memptr source;
	id0_unsigned_t dest,width,height;

	source = grsegs[chunknum];
	dest = ylookup[y]+x+bufferofs;
	width = pictable[picnum].width;
	height = pictable[picnum].height;

	VW_MemToScreen(source,dest,width,height);
}


// REFKEEN - Unused function from Catacomb Armageddon/Apocalypse (for gelib.c:Presenter)
#if 0
// MDM (GAMERS EDGE) begin
/*
====================
=
= VW_DrawPic2x - Same as VW_DrawPic, but doubles pixels horizontally
=                (Great for drawing 320 graphics on 640 screen!)
=
= X in bytes, y in pixels, chunknum is the #defined picnum
=
====================
*/

void VW_DrawPic2x(unsigned x, unsigned y, unsigned chunknum)
{
	int	picnum = chunknum - STARTPICS;
	memptr source;
	unsigned dest,width,height;

	source = grsegs[chunknum];
	dest = ylookup[y]+x+bufferofs;
	width = pictable[picnum].width;
	height = pictable[picnum].height;

	VW_MemToScreen2x(source,dest,width,height);
}
// MDM (GAMERS EDGE) end
#endif

#endif

#if NUMPICM>0

/*
====================
=
= VW_DrawMPic
=
= X in bytes, y in pixels, chunknum is the #defined picnum
=
====================
*/

void VW_DrawMPic(id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t chunknum)
{
	id0_int_t	picnum = chunknum - STARTPICM;
	memptr source;
	id0_unsigned_t dest,width,height;

	source = grsegs[chunknum];
	dest = ylookup[y]+x+bufferofs;
	width = picmtable[picnum].width;
	height = picmtable[picnum].height;

	VW_MaskBlock(source,0,dest,width,height,width*height);
}

void VW_ClipDrawMPic(id0_unsigned_t x, id0_int_t y, id0_unsigned_t chunknum)
{
	id0_int_t	picnum = chunknum - STARTPICM;
	memptr source;
	id0_unsigned_t dest,width,ofs,plane;
	id0_int_t		height;

	source = grsegs[chunknum];
	width = picmtable[picnum].width;
	height = picmtable[picnum].height;
	plane = width*height;

	ofs = 0;
	if (y<0)
	{
		ofs= -y*width;
		height+=y;
		y=0;
	}
	else if (y+height>216)
	{
		height-=(y-216);
	}
	dest = ylookup[y]+x+bufferofs;
	if (height<1)
		return;

	VW_MaskBlock(source,ofs,dest,width,height,plane);
}


#endif

//===========================================================================

#if NUMSPRITES>0

/*
====================
=
= VW_DrawSprite
=
= X and Y in pixels, it will match the closest shift possible
=
= To do:
= Add vertical clipping!
= Make the shifts act as center points, rather than break points
=
====================
*/

void VW_DrawSprite(id0_int_t x, id0_int_t y, id0_unsigned_t chunknum)
{
	spritetabletype id0_far *spr;
	spritetype id0_seg	*block;
	id0_unsigned_t	dest,shift;

	spr = &spritetable[chunknum-STARTSPRITES];
	block = (spritetype id0_seg *)grsegs[chunknum];

	y+=spr->orgy>>G_P_SHIFT;
	x+=spr->orgx>>G_P_SHIFT;

#if GRMODE == EGAGR
	shift = (x&7)/2;
#endif
#if GRMODE == CGAGR
	shift = 0;
#endif

	dest = bufferofs + ylookup[y];
	if (x>=0)
		dest += x/SCREENXDIV;
	else
		dest += (x+1)/SCREENXDIV;

	VW_MaskBlock (block,block->sourceoffset[shift],dest,
		block->width[shift],spr->height,block->planesize[shift]);
}

#endif


/*
==================
=
= VW_Hlin
=
==================
*/


#if GRMODE == EGAGR

id0_unsigned_char_t leftmask[8] = {0xff,0x7f,0x3f,0x1f,0xf,7,3,1};
id0_unsigned_char_t rightmask[8] = {0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};

void VW_Hlin(id0_unsigned_t xl, id0_unsigned_t xh, id0_unsigned_t y, id0_unsigned_t color)
{
  id0_unsigned_t dest,xlb,xhb,maskleft,maskright,mid;

	xlb=xl/8;
	xhb=xh/8;

	EGAWRITEMODE(2);
	EGAMAPMASK(15);

	maskleft = leftmask[xl&7];
	maskright = rightmask[xh&7];

	mid = xhb-xlb-1;
	dest = bufferofs+ylookup[y]+xlb;

  if (xlb==xhb)
  {
  	//
  	// entire line is in one byte
  	//

	maskleft&=maskright;

	BE_ST_EGAUpdateGFXPixel4bpp(dest, color, maskleft);
	return;
  }

	//
	// draw left side
	//
	BE_ST_EGAUpdateGFXPixel4bpp(dest++, color, maskleft);

	//
	// draw middle
	//
	BE_ST_EGAUpdateGFXPixel4bppRepeatedly(dest, color, mid, 255); // no masking
	dest += mid;

	//
	// draw right side
	//
	BE_ST_EGAUpdateGFXPixel4bpp(dest, color, maskright);
}
#endif


#if GRMODE == CGAGR

id0_unsigned_char_t pixmask[4] = {0xc0,0x30,0x0c,0x03};
id0_unsigned_char_t leftmask[4] = {0xff,0x3f,0x0f,0x03};
id0_unsigned_char_t rightmask[4] = {0xc0,0xf0,0xfc,0xff};
extern id0_unsigned_char_t colorbyte[4];

//
// could be optimized for rep stosw
//
void VW_Hlin(id0_unsigned_t xl, id0_unsigned_t xh, id0_unsigned_t y, id0_unsigned_t color)
{
	id0_unsigned_t dest,xlb,xhb,mid;
	id0_byte_t maskleft,maskright;

	color = colorbyte[color];	// expand 2 color bits to 8

	xlb=xl/4;
	xhb=xh/4;

	maskleft = leftmask[xl&3];
	maskright = rightmask[xh&3];

	mid = xhb-xlb-1;
	dest = bufferofs+ylookup[y]+xlb;

	if (xlb==xhb)
	{
		//
		// entire line is in one byte
		//
		maskleft&=maskright;
		// mask out pixels; 'or' in color
		screenseg[dest] = (screenseg[dest] & ~maskleft) | ((id0_byte_t)color & maskleft);

		BE_ST_MarkGfxForPendingUpdate();
		return;
	}

	//
	// draw left side
	//

	// mask out pixels; 'or' in color
	screenseg[dest] = (screenseg[dest] & ~maskleft) | ((id0_byte_t)color & maskleft);
	++dest;

	//
	// draw middle
	//

	BE_Cross_Wrapped_MemSet(screenseg, &screenseg[dest], (id0_byte_t)color, mid);
	//memset(&screenseg[dest], color, mid);
	dest += mid; // dest is a 16-bit unsigned so it wraps "automagically" (and can "safely" be used as an offset)

	//
	// draw right side
	//

	// mask out pixels; 'or' in color
	screenseg[dest] = (screenseg[dest] & ~maskright) | ((id0_byte_t)color & maskright);

	BE_ST_MarkGfxForPendingUpdate();
}
#endif


/*
==================
=
= VW_Bar
=
= Pixel addressable block fill routine
=
==================
*/

#if GRMODE == CGAGR

void VW_Bar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t width, id0_unsigned_t height,
	id0_unsigned_t color)
{
	id0_unsigned_t xh = x+width-1;

	while (height--)
		VW_Hlin (x,xh,y++,color);
}

#endif


#if	GRMODE == EGAGR

void VW_Bar (id0_unsigned_t x, id0_unsigned_t y, id0_unsigned_t width, id0_unsigned_t height,
	id0_unsigned_t color)
{
	id0_unsigned_t dest,xh,xlb,xhb,maskleft,maskright,mid;

	xh = x+width-1;
	xlb=x/8;
	xhb=xh/8;

	EGAWRITEMODE(2);
	EGAMAPMASK(15);

	maskleft = leftmask[x&7];
	maskright = rightmask[xh&7];

	mid = xhb-xlb-1;
	dest = bufferofs+ylookup[y]+xlb;

	if (xlb==xhb)
	{
		//
		// entire line is in one byte
		//

		maskleft&=maskright;

		do
		{
			BE_ST_EGAUpdateGFXPixel4bpp(dest, color, maskleft);
			dest += linewidth; // down to next line
			--height;
		} while (height);

		return;
	}

	id0_unsigned_t bytesToAdd = linewidth-mid-1;
	do
	{
		//
		// draw left side
		//
		BE_ST_EGAUpdateGFXPixel4bpp(dest++, color, maskleft);

		//
		// draw middle
		//
		BE_ST_EGAUpdateGFXPixel4bppRepeatedly(dest, color, mid, 255); // no masking
		dest += mid;

		//
		// draw right side
		//
		BE_ST_EGAUpdateGFXPixel4bpp(dest, color, maskright);

		dest += bytesToAdd; // move to start of next line
		--height;
	} while (height);
}

#endif

//==========================================================================

/*
==================
=
= VW_MeasureString
=
==================
*/

#if NUMFONT+NUMFONTM>0
void
VWL_MeasureString (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend, id0_word_t *width, id0_word_t *height, fontstruct id0_seg *font)
{
	// (REFKEEN) Modifications from vanilla Keen:
	// - All input strings are now const.
	// - An additional "optsend" argument marking one char past end
	// of string. Set to NULL for original behaviors.
	// - Related to modifcation to US_Print and US_CPrint, properly taking
	// care of C string literals as inputs.
#ifdef REFKEEN_VER_CATADVENTURES
	*height = font->height-1;			// MDM (GAMERS EDGE) - squeeze font vertically...
#else
	*height = font->height;
#endif
#if 0
	for (*width = 0;*string;string++)
		*width += font->width[*((id0_byte_t far *)string)];		// proportional width
#endif
	if (optsend)
	{
		for (*width = 0;string!=optsend;string++)
			*width += font->width[*((id0_byte_t id0_far *)string)];		// proportional width
	}
	else
	{
		for (*width = 0;*string;string++)
			*width += font->width[*((id0_byte_t id0_far *)string)];		// proportional width
	}
}

void	VW_MeasurePropString (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend, id0_word_t *width, id0_word_t *height)
{
	VWL_MeasureString(string,optsend,width,height,(fontstruct id0_seg *)grsegs[STARTFONT+fontnumber]);
}

void	VW_MeasureMPropString  (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend, id0_word_t *width, id0_word_t *height)
{
	VWL_MeasureString(string,optsend,width,height,(fontstruct id0_seg *)grsegs[STARTFONTM+fontnumber]);
}


#endif


/*
=============================================================================

							CGA stuff

=============================================================================
*/

#if GRMODE == CGAGR

#define CGACRTCWIDTH	40

/*
==========================
=
= VW_CGAFullUpdate
=
==========================
*/

void VW_CGAFullUpdate (void)
{
	id0_byte_t	*update;
	id0_boolean_t	halftile;
	id0_unsigned_t	x,y,middlerows,middlecollumns;

	displayofs = bufferofs+panadjust;

	uint8_t *srcPtr = &screenseg[displayofs];
	uint8_t *destPtr = BE_ST_GetCGAMemoryPtr();

	id0_unsigned_t linePairsToCopy = 100; // pairs of scan lines to copy

	do
	{
		BE_Cross_WrappedToLinear_MemCopy(destPtr, screenseg, srcPtr, 80);
		//memcpy(destPtr, srcPtr, 80);
		BE_Cross_Wrapped_Add(screenseg, &srcPtr, linewidth);
		//srcPtr += linewidth;
		destPtr += 0x2000; // go to the interlaced bank

		BE_Cross_WrappedToLinear_MemCopy(destPtr, screenseg, srcPtr, 80);
		//memcpy(destPtr, srcPtr, 80);
		BE_Cross_Wrapped_Add(screenseg, &srcPtr, linewidth);
		//srcPtr += linewidth;
		destPtr -= (0x2000 - 80); // go to the non interlaced bank
	} while (--linePairsToCopy);

	// clear out the update matrix
	memset(baseupdateptr, 0, UPDATEWIDE*UPDATEHIGH);

	updateptr = baseupdateptr;
	*(id0_unsigned_t *)(updateptr + UPDATEWIDE*PORTTILESHIGH) = UPDATETERMINATE;

	// Rather than BE_ST_MarkGfxForPendingUpdate()...
	BE_ST_MarkGfxForUpdate();

#if 0
	id0_byte_t	*update;
	id0_boolean_t	halftile;
	id0_unsigned_t	x,y,middlerows,middlecollumns;

	displayofs = bufferofs+panadjust;

asm	mov	ax,0xb800
asm	mov	es,ax

asm	mov	si,[displayofs]
asm	xor	di,di

asm	mov	bx,100				// pairs of scan lines to copy
asm	mov	dx,[linewidth]
asm	sub	dx,80

asm	mov	ds,[screenseg]
asm	test	si,1
asm	jz	evenblock

//
// odd source
//
asm	mov	ax,39				// words accross screen
copytwolineso:
asm	movsb
asm	mov	cx,ax
asm	rep	movsw
asm	movsb
asm	add	si,dx
asm	add	di,0x2000-80		// go to the interlaced bank
asm	movsb
asm	mov	cx,ax
asm	rep	movsw
asm	movsb
asm	add	si,dx
asm	sub	di,0x2000			// go to the non interlaced bank

asm	dec	bx
asm	jnz	copytwolineso
asm	jmp	blitdone

//
// even source
//
evenblock:
asm	mov	ax,40				// words accross screen
copytwolines:
asm	mov	cx,ax
asm	rep	movsw
asm	add	si,dx
asm	add	di,0x2000-80		// go to the interlaced bank
asm	mov	cx,ax
asm	rep	movsw
asm	add	si,dx
asm	sub	di,0x2000			// go to the non interlaced bank

asm	dec	bx
asm	jnz	copytwolines

blitdone:
asm	mov	ax,ss
asm	mov	ds,ax
asm	mov	es,ax

asm	xor	ax,ax				// clear out the update matrix
asm	mov	cx,UPDATEWIDE*UPDATEHIGH/2

asm	mov	di,[baseupdateptr]
asm	rep	stosw

	updateptr = baseupdateptr;
	*(id0_unsigned_t *)(updateptr + UPDATEWIDE*PORTTILESHIGH) = UPDATETERMINATE;
#endif
}


#endif

/*
=============================================================================

					   CURSOR ROUTINES

These only work in the context of the double buffered update routines

=============================================================================
*/

/*
====================
=
= VWL_DrawCursor
=
= Background saves, then draws the cursor at cursorspot
=
====================
*/

void VWL_DrawCursor (void)
{
	cursorspot = bufferofs + ylookup[cursory+pansy]+(cursorx+pansx)/SCREENXDIV;
	VW_ScreenToMem(cursorspot,cursorsave,cursorwidth,cursorheight);
	VWB_DrawSprite(cursorx,cursory,cursornumber);
}


//==========================================================================


/*
====================
=
= VWL_EraseCursor
=
====================
*/

void VWL_EraseCursor (void)
{
	VW_MemToScreen(cursorsave,cursorspot,cursorwidth,cursorheight);
	VW_MarkUpdateBlock ((cursorx+pansx)&SCREENXMASK,cursory+pansy,
		( (cursorx+pansx)&SCREENXMASK)+cursorwidth*SCREENXDIV-1,
		cursory+pansy+cursorheight-1);
}


//==========================================================================


/*
====================
=
= VW_ShowCursor
=
====================
*/

void VW_ShowCursor (void)
{
	cursorvisible++;
}


//==========================================================================

/*
====================
=
= VW_HideCursor
=
====================
*/

void VW_HideCursor (void)
{
	cursorvisible--;
}

//==========================================================================

/*
====================
=
= VW_MoveCursor
=
====================
*/
#define MAXCURSORX	(319-24)
#define MAXCURSORY	(199-24)

void VW_MoveCursor (id0_int_t x, id0_int_t y)
{
	if (x>MAXCURSORX)
		x=MAXCURSORX;
	if (y>MAXCURSORY)
		y=MAXCURSORY;			// catacombs hack to keep cursor on screen

	cursorx = x;
	cursory = y;
}

//==========================================================================

/*
====================
=
= VW_SetCursor
=
= Load in a sprite to be used as a cursor, and allocate background save space
=
====================
*/

void VW_SetCursor (id0_int_t spritenum)
{
	VW_FreeCursor ();

	cursornumber = spritenum;

	CA_CacheGrChunk (spritenum);
	MM_SetLock (&grsegs[spritenum],true);

	cursorwidth = spritetable[spritenum-STARTSPRITES].width+1;
	cursorheight = spritetable[spritenum-STARTSPRITES].height;

	MM_GetPtr (&cursorsave,cursorwidth*cursorheight*5);
	MM_SetLock (&cursorsave,true);
}


/*
====================
=
= VW_FreeCursor
=
= Frees the memory used by the cursor and its background save
=
====================
*/

void VW_FreeCursor (void)
{
	if (cursornumber)
	{
		MM_SetLock (&grsegs[cursornumber],false);
		MM_SetPurge (&grsegs[cursornumber],3);
		MM_SetLock (&cursorsave,false);
		MM_FreePtr (&cursorsave);
		cursornumber = 0;
	}
}


/*
=============================================================================

				Double buffer management routines

=============================================================================
*/

/*
======================
=
= VW_InitDoubleBuffer
=
======================
*/

void VW_InitDoubleBuffer (void)
{
#if GRMODE == EGAGR
	VW_SetScreen (displayofs+panadjust,0);			// no pel pan
#endif
}


/*
======================
=
= VW_FixRefreshBuffer
=
= Copies the view page to the buffer page on page flipped refreshes to
= avoid a one frame shear around pop up windows
=
======================
*/

void VW_FixRefreshBuffer (void)
{
#if GRMODE == EGAGR
	VW_ScreenToScreen (displayofs,bufferofs,PORTTILESWIDE*4*CHARWIDTH,
		(PORTTILESHIGH-1)*16);
#endif
}


/*
======================
=
= VW_QuitDoubleBuffer
=
======================
*/

void VW_QuitDoubleBuffer (void)
{
}


/*
=======================
=
= VW_MarkUpdateBlock
=
= Takes a pixel bounded block and marks the tiles in bufferblocks
= Returns 0 if the entire block is off the buffer screen
=
=======================
*/

id0_int_t VW_MarkUpdateBlock (id0_int_t x1, id0_int_t y1, id0_int_t x2, id0_int_t y2)
{
// MDM (GAMERS EDGE) begin - NOT NEEDED FOR 3D ENGINE
#if !(defined REFKEEN_VER_CATADVENTURES) || (defined REFKEEN_VER_CATABYSS)
	id0_int_t	x,y,xt1,yt1,xt2,yt2,nextline;
	id0_byte_t *mark;

	xt1 = x1>>PIXTOBLOCK;
	yt1 = y1>>PIXTOBLOCK;

	xt2 = x2>>PIXTOBLOCK;
	yt2 = y2>>PIXTOBLOCK;

	if (xt1<0)
		xt1=0;
	else if (xt1>=UPDATEWIDE-1)
		return 0;

	if (yt1<0)
		yt1=0;
	else if (yt1>UPDATEHIGH)
		return 0;

	if (xt2<0)
		return 0;
	else if (xt2>=UPDATEWIDE-1)
		xt2 = UPDATEWIDE-2;

	if (yt2<0)
		return 0;
	else if (yt2>=UPDATEHIGH)
		yt2 = UPDATEHIGH-1;

	mark = updateptr + uwidthtable[yt1] + xt1;
	nextline = UPDATEWIDE - (xt2-xt1) - 1;

	for (y=yt1;y<=yt2;y++)
	{
		for (x=xt1;x<=xt2;x++)
			*mark++ = 1;			// this tile will need to be updated

		mark += nextline;
	}
#endif
// MDM (GAMERS EDGE) end

	return 1;
}


/*
===========================
=
= VW_UpdateScreen
=
= Updates any changed areas of the double buffer and displays the cursor
=
===========================
*/

void VW_UpdateScreen (void)
{
	if (cursorvisible>0)
		VWL_DrawCursor();

#if GRMODE == EGAGR
	VWL_UpdateScreenBlocks();

	// cat3d patch
	memset(updateptr, 0, 2*(UPDATEWIDE*UPDATEHIGH/2)); // clear out the update matrix
	*(id0_unsigned_t *)(updateptr + UPDATEWIDE*PORTTILESHIGH) = UPDATETERMINATE;

	BE_ST_SetScreenStartAddress(displayofs+panadjust); // Ported from ASM
#endif
#if GRMODE == CGAGR
	VW_CGAFullUpdate();
#endif

	if (cursorvisible>0)
		VWL_EraseCursor();
}



void VWB_DrawTile8 (id0_int_t x, id0_int_t y, id0_int_t tile)
{
	x+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+7,y+7))
		VW_DrawTile8 (x/SCREENXDIV,y,tile);
}

void VWB_DrawTile8M (id0_int_t x, id0_int_t y, id0_int_t tile)
{
	id0_int_t xb;

	x+=pansx;
	y+=pansy;
	xb = x/SCREENXDIV; 			// use intermediate because VW_DT8M is macro
#ifdef REFKEEN_VER_CATADVENTURES
//	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+7,y+7))	// MDM (GAMER EDGE)
		VW_DrawTile8M(xb,y,tile);	// statement prevents drawing chars past 42
#else
	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+7,y+7))
		VW_DrawTile8M(xb,y,tile);
#endif
}

void VWB_DrawTile16 (id0_int_t x, id0_int_t y, id0_int_t tile)
{
	x+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+15,y+15))
		VW_DrawTile16 (x/SCREENXDIV,y,tile);
}

void VWB_DrawTile16M (id0_int_t x, id0_int_t y, id0_int_t tile)
{
	id0_int_t xb;

	x+=pansx;
	y+=pansy;
	xb = x/SCREENXDIV;		// use intermediate because VW_DT16M is macro
	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+15,y+15))
		VW_DrawTile16M (xb,y,tile);
}

#if NUMPICS
void VWB_DrawPic (id0_int_t x, id0_int_t y, id0_int_t chunknum)
{
// mostly copied from drawpic
	id0_int_t	picnum = chunknum - STARTPICS;
	memptr source;
	id0_unsigned_t dest,width,height;

	x+=pansx;
	y+=pansy;
	x/= SCREENXDIV;

	source = grsegs[chunknum];
	dest = ylookup[y]+x+bufferofs;
	width = pictable[picnum].width;
	height = pictable[picnum].height;

	if (VW_MarkUpdateBlock (x*SCREENXDIV,y,(x+width)*SCREENXDIV-1,y+height-1))
		VW_MemToScreen(source,dest,width,height);
}
#endif

#if NUMPICM>0
void VWB_DrawMPic(id0_int_t x, id0_int_t y, id0_int_t chunknum)
{
// mostly copied from drawmpic
	id0_int_t	picnum = chunknum - STARTPICM;
	memptr source;
	id0_unsigned_t dest,width,height;

	x+=pansx;
	y+=pansy;
	x/=SCREENXDIV;

	source = grsegs[chunknum];
	dest = ylookup[y]+x+bufferofs;
	width = picmtable[picnum].width;
	height = picmtable[picnum].height;

	if (VW_MarkUpdateBlock (x*SCREENXDIV,y,(x+width)*SCREENXDIV-1,y+height-1))
		VW_MaskBlock(source,0,dest,width,height,width*height);
}
#endif


void VWB_Bar (id0_int_t x, id0_int_t y, id0_int_t width, id0_int_t height, id0_int_t color)
{
	x+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock (x,y,x+width,y+height-1) )
		VW_Bar (x,y,width,height,color);
}


#if NUMFONT
void VWB_DrawPropString	 (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend)
{
	id0_int_t x,y;
	x = px+pansx;
	y = py+pansy;
	VW_DrawPropString (string,optsend);
	VW_MarkUpdateBlock(x,y,x+bufferwidth*8-1,y+bufferheight-1);
}
#endif


#if NUMFONTM
void VWB_DrawMPropString (const id0_char_t id0_far *string, const id0_char_t id0_far *optsend)
{
	id0_int_t x,y;
	x = px+pansx;
	y = py+pansy;
	VW_DrawMPropString (string,optsend);
	VW_MarkUpdateBlock(x,y,x+bufferwidth*8-1,y+bufferheight-1);
}
#endif

#if NUMSPRITES
void VWB_DrawSprite(id0_int_t x, id0_int_t y, id0_int_t chunknum)
{
	spritetabletype id0_far *spr;
	spritetype id0_seg	*block;
	id0_unsigned_t	dest,shift,width,height;

	x+=pansx;
	y+=pansy;

	spr = &spritetable[chunknum-STARTSPRITES];
	block = (spritetype id0_seg *)grsegs[chunknum];

	y+=spr->orgy>>G_P_SHIFT;
	x+=spr->orgx>>G_P_SHIFT;


#if GRMODE == EGAGR
	shift = (x&7)/2;
#endif
#if GRMODE == CGAGR
	shift = 0;
#endif

	dest = bufferofs + ylookup[y];
	if (x>=0)
		dest += x/SCREENXDIV;
	else
		dest += (x+1)/SCREENXDIV;

	width = block->width[shift];
	height = spr->height;

	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+width*SCREENXDIV-1
		,y+height-1))
		VW_MaskBlock (block,block->sourceoffset[shift],dest,
			width,height,block->planesize[shift]);
}
#endif

void VWB_Plot (id0_int_t x, id0_int_t y, id0_int_t color)
{
	x+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock (x,y,x,y))
		VW_Plot(x,y,color);
}

void VWB_Hlin (id0_int_t x1, id0_int_t x2, id0_int_t y, id0_int_t color)
{
	x1+=pansx;
	x2+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock (x1,y,x2,y))
		VW_Hlin(x1,x2,y,color);
}

void VWB_Vlin (id0_int_t y1, id0_int_t y2, id0_int_t x, id0_int_t color)
{
	x+=pansx;
	y1+=pansy;
	y2+=pansy;
	if (VW_MarkUpdateBlock (x,y1,x,y2))
		VW_Vlin(y1,y2,x,color);
}


//===========================================================================
