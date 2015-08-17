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

// C3_SCALE.C

#ifdef REFKEEN_VER_CATADVENTURES
#include "def.h"
#else
#include "c3_def.h"
#endif

#include <stddef.h> // For offsetof
//#pragma hdrstop

//const	id0_unsigned_t	viewheight = 144;
const	id0_unsigned_t	screenbwide = 40;
const	id0_byte_t		BACKGROUNDPIX	=   5;

#ifdef REFKEEN_VER_CATADVENTURES
id0_unsigned_t		shapesize[NUMSCALEPICS];
t_compscale id0_seg *scaledirectory[NUMSCALEPICS];
#else
id0_unsigned_t		shapesize[MAXSCALE+1];
t_compscale id0_seg *scaledirectory[MAXSCALE+1];
#endif
t_compshape id0_seg *shapedirectory[NUMSCALEPICS];
memptr			walldirectory[NUMSCALEWALLS];

/*
===========================
=
= DeplanePic
=
= Takes a raw bit map of width bytes by height and creates a scaleable shape
=
= Returns the length of the shape in bytes
=
= Fills in spotvis (a convenient 64*64 array) with the color values
=
===========================
*/

void DeplanePic (id0_int_t picnum)
{
	id0_byte_t		id0_far *plane0,id0_far *plane1,id0_far *plane2,id0_far *plane3;
	id0_byte_t		by0,by1,by2,by3;
	id0_unsigned_t	x,y,b,color,shift,width,height;
	id0_byte_t		*dest;

//
// convert ega pixels to byte color values in a temp buffer
//
	width = pictable[picnum-STARTPICS].width;
	height = pictable[picnum-STARTPICS].height;

#ifdef REFKEEN_VER_CATADVENTURES
	if (width>8 || height!=64)
#else
	if (width>64 || height!=64)
#endif
		Quit ("DePlanePic: Bad size shape");

	memset (spotvis,BACKGROUNDPIX,sizeof(spotvis));

	plane0 = (id0_byte_t id0_seg *)grsegs[picnum];
	plane1 = plane0 + width*height;
	plane2 = plane1 + width*height;
	plane3 = plane2 + width*height;

	for (y=0;y<height;y++)
	{
		dest = &spotvis[y][0];
		for (x=0;x<width;x++)
		{
			by0 = *plane0++;
			by1 = *plane1++;
			by2 = *plane2++;
			by3 = *plane3++;

			for (b=0;b<8;b++)
			{
				shift=8-b;

				//color = 0; // (REFKEEN) COMMENTED OUT (cause color is (re)set to another value now)

				// (REFKEEN) Originally only the low byte of
				// color was modified, but even here it should
				// be modified in the exact same way and then
				// stored in *dest (a byte) as-is
				color = (by3>>(shift-1))&1;
				//color = (color << 1) /*| ((color&0x80)>>7)*/ | ((by3>>(shift-1))&1);
				color = (color << 1) /*| ((color&0x80)>>7)*/ | ((by2>>(shift-1))&1);
				color = (color << 1) /*| ((color&0x80)>>7)*/ | ((by1>>(shift-1))&1);
				color = (color << 1) /*| ((color&0x80)>>7)*/ | ((by0>>(shift-1))&1);
#if 0
				asm	mov	cl,[BYTE PTR shift]
				asm	mov	al,[BYTE PTR by3]
				asm	rcr	al,cl;
				asm	rcl	[BYTE PTR color],1;

				asm	mov	cl,[BYTE PTR shift]
				asm	mov	al,[BYTE PTR by2]
				asm	rcr	al,cl;
				asm	rcl	[BYTE PTR color],1;

				asm	mov	cl,[BYTE PTR shift]
				asm	mov	al,[BYTE PTR by1]
				asm	rcr	al,cl;
				asm	rcl	[BYTE PTR color],1;

				asm	mov	cl,[BYTE PTR shift]
				asm	mov	al,[BYTE PTR by0]
				asm	rcr	al,cl;
				asm	rcl	[BYTE PTR color],1;
#endif

				*dest++ = color;
			}	// B
		}		// X
	}			// Y
}



// NOTE (REFKEEN) This is useless on modern and with (somewhat) portable code,
// but we leave this here for compatibility in terms of memory management.

/*
========================
=
= BuildCompScale
=
= Builds a compiled scaler object that will scale a 64 tall object to
= the given height (centered vertically on the screen)
=
= height should be even
=
= Call with
= ---------
= DS:SI		Source for scale
= ES:DI		Dest for scale
=
= Calling the compiled scaler only destroys AL
=
========================
*/

id0_unsigned_t BuildCompScale (id0_int_t height, memptr *finalspot)
{
	t_compscale 	id0_seg *work;
	id0_byte_t		id0_far *code;

	//id0_int_t			i;
	id0_long_t		fix,step;
	id0_unsigned_t	src/*,totalscaled*/,totalsize;
	id0_int_t			startpix,endpix,toppix;


	MM_GetPtr ((memptr *)&work,20000);

	step = ((id0_long_t)height<<16) / 64;
	code = &work->code[0];
	toppix = (viewheight-height)/2;
	fix = 0;

	for (src=0;src<=64;src++)
	{
		startpix = (id0_word_t)((id0_longword_t)fix>>16);
		fix += step;
		endpix = (id0_word_t)((id0_longword_t)fix>>16);

		work->start[src] = startpix;
		if (endpix>startpix)
			work->width[src] = endpix-startpix;
		else
			work->width[src] = 0;

//
// mark the start of the code
//
		// (REFKEEN) Kind of backwards compatibility
		work->codeofs[src] = (code-(id0_byte_t *)work);
		//work->codeofs[src] = FP_OFF(code);

//
// compile some code if the source pixel generates any screen pixels
//
		startpix+=toppix;
		endpix+=toppix;

		if (startpix == endpix || endpix < 0 || startpix >= VIEWHEIGHT || src == 64)
			continue;

	//
	// mov al,[si+src]
	//
		*code++ = 0x8a;
		*code++ = 0x44;
		*code++ = src;

		for (;startpix<endpix;startpix++)
		{
			if (startpix >= VIEWHEIGHT)
				break;						// off the bottom of the view area
			if (startpix < 0)
				continue;					// not into the view area

		//
		// and [es:di+heightofs],al
		//
			*code++ = 0x26;
			*code++ = 0x20;
			*code++ = 0x85;
			*((id0_unsigned_t id0_far *)code) = startpix*screenbwide;
			code += 2;
			//*((id0_unsigned_t id0_far *)code)++ = startpix*screenbwide;
		}

	}

//
// retf
//
	*code++ = 0xcb;

	// (REFKEEN) Kind of backwards compatibility
	totalsize = (code-(id0_byte_t *)work);
	//totalsize = FP_OFF(code);
	MM_GetPtr (finalspot,totalsize);
	memcpy ((id0_byte_t id0_seg *)(*finalspot),(id0_byte_t id0_seg *)work,totalsize);
	//_fmemcpy ((id0_byte_t id0_seg *)(*finalspot),(id0_byte_t id0_seg *)work,totalsize);
	MM_FreePtr ((memptr *)&work);

	return totalsize;
}

// (REFKEEN) Replacement for functions generated by BuildCompScale
void ExecuteCompScale(const id0_byte_t *codePtr, id0_unsigned_t egaDestOff, const id0_byte_t *srcPtr, id0_byte_t mask)
{
	while (*codePtr != 0xcb)
	{
		//
		// mov al, [si+src] ;We fetch src from code
		//
		id0_byte_t srcVal = srcPtr[*(codePtr += 2)];
		++codePtr;
		while (*codePtr == 0x26)
		{
			//
			// and [es:di+heightofs],al
			//
			id0_unsigned_t heightofs;
			memcpy(&heightofs, codePtr += 3, sizeof(heightofs));
			codePtr += sizeof(heightofs);
			// Originally drawn with write mode 2 (pixel based)
			// and read mode 1, but the "don't care" register bits
			// were all set to 0, so each read of a byte from EGA
			// memory ended with 0xFF.
			//
			// Then the original code applied an "and" operation to
			// that input (of 0xff) with srcVal, using the given
			// masks to select correct bits in each plane.
			BE_ST_EGAUpdateGFXPixel4bpp(egaDestOff+heightofs, srcVal, mask);
		}
	}
}


// NOTE (REFKEEN): Like BuildCompScale, we keep this code for compatibility
// in terms of memory management. What makes things somewhat more difficult is
// the usage of EGA memory (with its planes) as temporary storage.

/*
========================
=
= BuildCompShape
=
= typedef struct
= {
=	id0_unsigned_t	width;
=	id0_unsigned_t	codeofs[64];
= }	t_compshape;
=
= Width is the number of compiled line draws in the shape.  The shape
= drawing code will assume that the midpoint of the shape is in the
= middle of the width.
=
= The non background pixel data will start at codeofs[width], so codeofs
= greater than width will be invalid.
=
= Each code offset will draw one vertical line of the shape, consisting
= of 0 or more segments of scaled pixels.
=
= The scaled shapes use ss:0-4 as a scratch variable for the far call to
= the compiled scaler, so zero it back out after the shape is scaled, or
= a "null pointer assignment" will result upon termination.
=
= Setup for a call to a compiled shape
= -----------------------------------
= ax	toast
= bx	toast
= cx	toast
= dx	segment of compiled shape
= si	toast
= di	byte at top of view area to draw the line in
= bp	0
= ss:2 and ds  the segment of the compiled scaler to use
= es	screenseg
=
= Upon return, ds IS NOT SET to the data segment.  Do:
=	mov	ax,ss
=	mov	ds,ax
=
=
= GC_BITMASK	set to the pixels to be drawn in the row of bytes under DI
= GC_MODE		read mode 1, write mode 2
= GC_COLORDONTCARE  set to 0, so all reads from video memory return 0xff
=
=
= Code generated for each segment
= -------------------------------
=	mov	bx,[(segend+1)*2]
=	mov	cx,[bx]
=	mov	[BYTE PTR bx],0xc8		// far return
=	mov	ax,[segstart*2]
=	mov	[ss:0],ax				// entry point into the compiled scaler
=	mov	ds,dx                   // (mov ds,cs) the data is after the compiled code
=	mov	si,ofs data
=	call	[bp]				// scale some pixels
=	mov	ds,[bp+2]
=	mov	[bx],cx					// un patch return
=
= Code generated after all segments on a line
= -------------------------------------------
=	retf
=
========================
*/

id0_unsigned_t BuildCompShape (t_compshape id0_seg **finalspot)
{
	// (REFKEEN) Use offsets into (planar) EGA memory instead
	id0_unsigned_t workEgaOff, codeEgaOff;
	//t_compshape 	id0_seg *work;
	//id0_byte_t		id0_far *code;
	id0_int_t			firstline,lastline,x,y;
	id0_unsigned_t	firstpix,lastpix,width;
	id0_unsigned_t	totalsize,pixelofs;
	id0_unsigned_t	buff;


//	MM_GetPtr ((memptr *)&work,20000);
	EGAWRITEMODE(0);
	EGAREADMAP(0);		// use ega screen memory for temp buffer
	EGAMAPMASK(1);
	buff = screenloc[1];
	workEgaOff = ((buff+15)/16)*16;
	//work = (t_compshape id0_seg *)(0xa000+(buff+15)/16);

//
// find the width of the shape
//
	firstline = -1;
	x=0;
	do
	{
		for (y=0;y<64;y++)
			if (spotvis[y][x] != BACKGROUNDPIX)
			{
				firstline = x;
				break;
			}
		if (++x == 64)
			Quit ("BuildCompShape: No shape data!");
	} while (firstline == -1);

	lastline = -1;
	x=63;
	do
	{
		for (y=0;y<64;y++)
			if (spotvis[y][x] != BACKGROUNDPIX)
			{
				lastline = x;
				break;
			}
		x--;
	} while (lastline == -1);

	width = lastline-firstline+1;

	BE_ST_EGAUpdateGFXBuffer(workEgaOff+offsetof(t_compshape, width), (id0_byte_t *)&width, sizeof(width), 1);
	//work->width = width;
	codeEgaOff = workEgaOff+offsetof(t_compshape, codeofs)+width*sizeof(id0_unsigned_t);
	//codeEgaOff = workEgaOff+offsetof(t_compshape, codeofs[width]);
	//code = (id0_byte_t id0_far *)&work->codeofs[width];

//
// copy all non background pixels to the work space
//
	pixelofs = codeEgaOff-workEgaOff; // In the original code, work should be a segment after A000
	//pixelofs = FP_OFF(code);

	for (x=firstline;x<=lastline;x++)
		for (y=0;y<64;y++)
			if (spotvis[y][x] != BACKGROUNDPIX)
				BE_ST_EGAUpdateGFXByte(codeEgaOff++, spotvis[y][x], 1);
				//*code++ = spotvis[y][x];

//
// start compiling the vertical lines
//
	for (x=firstline;x<=lastline;x++)
	{
		id0_unsigned_t tempUnsignedWord = codeEgaOff-workEgaOff;
		BE_ST_EGAUpdateGFXBuffer(workEgaOff+offsetof(t_compshape, codeofs)+(x-firstline)*sizeof(id0_unsigned_t), (id0_byte_t *)&tempUnsignedWord, sizeof(tempUnsignedWord), 1);
		//BE_ST_EGAUpdateGFXBuffer(workEgaOff+offsetof(t_compshape, codeofs[x-firstline]), (id0_byte_t *)&tempUnsignedWord, sizeof(tempUnsignedWord), 1);
		//work->codeofs[x-firstline] = FP_OFF(code);

		y=0;
		do
		{
		//
		// scan past black background pixels
		//
			while (spotvis[y][x] == BACKGROUNDPIX && y<64)
				y++;

			if (y>63)		// no more segments
				break;

			firstpix = y+1;		// +1 because width is before codeofs

		//
		// scan past scalable pixels
		//
			while (spotvis[y][x] != BACKGROUNDPIX && y<64)
				y++;

			if (y>63)
				lastpix = 65;
			else
				lastpix = y+1;	// actually one pixel past the last displayed

		//
		// compile the scale call
		//
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x8b, 1);		// mov bx,[lastpix*2]
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x1e, 1);
			tempUnsignedWord = lastpix*2;
			BE_ST_EGAUpdateGFXBuffer(codeEgaOff, (id0_byte_t *)&tempUnsignedWord, sizeof(tempUnsignedWord), 1);
			codeEgaOff += 2;

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x8b, 1);		// mov cx,[bx]
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x0f, 1);

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xc6, 1);		// move [BYTE bx],0xcb
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x07, 1);
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xcb, 1);

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xa1, 1);		// mov ax,[firstpix*2]	/*************
			tempUnsignedWord = firstpix*2;
			BE_ST_EGAUpdateGFXBuffer(codeEgaOff, (id0_byte_t *)&tempUnsignedWord, sizeof(tempUnsignedWord), 1);
			codeEgaOff += 2;

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x36, 1);		// mov [ss:0],ax
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xa3, 1);
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x00, 1);
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x00, 1);

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x8e, 1);		// mov ds,dx	(mov ds,cs)
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xda, 1);

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xbe, 1);		// mov si,OFFSET pixelofs-firstpixel
			tempUnsignedWord = pixelofs-firstpix;
			BE_ST_EGAUpdateGFXBuffer(codeEgaOff, (id0_byte_t *)&tempUnsignedWord, sizeof(tempUnsignedWord), 1);
			codeEgaOff += 2;

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xff, 1);		// call [DWORD bp]
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x5e, 1);
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x00, 1);


			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x8e, 1);		// mov ds,[bp+2]
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x5e, 1);
			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x02, 1);

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x89, 1);		// mov [bx],cx

			BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0x0f, 1);
#if 0
			*code++ = 0x8b;		// mov bx,[lastpix*2]
			*code++ = 0x1e;
			*((id0_unsigned_t id0_far *)code)++ = lastpix*2;

			*code++ = 0x8b;		// mov cx,[bx]
			*code++ = 0x0f;

			*code++ = 0xc6;		// move [BYTE bx],0xcb
			*code++ = 0x07;
			*code++ = 0xcb;

			*code++ = 0xa1;		// mov ax,[firstpix*2]	/*************
			*((id0_unsigned_t id0_far *)code)++ = firstpix*2;

			*code++ = 0x36;		// mov [ss:0],ax
			*code++ = 0xa3;
			*code++ = 0x00;
			*code++ = 0x00;

			*code++ = 0x8e;		// mov ds,dx	(mov ds,cs)
			*code++ = 0xda;

			*code++ = 0xbe;		// mov si,OFFSET pixelofs-firstpixel
			*((id0_unsigned_t id0_far *)code)++ = pixelofs-firstpix;

			*code++ = 0xff;		// call [DWORD bp]
			*code++ = 0x5e;
			*code++ = 0x00;

			*code++ = 0x8e;		// mov ds,[bp+2]
			*code++ = 0x5e;
			*code++ = 0x02;

			*code++ = 0x89;		// mov [bx],cx
			*code++ = 0x0f;
#endif

			pixelofs += (lastpix-firstpix);
		} while (y<63);

	//
	// retf
	//
		BE_ST_EGAUpdateGFXByte(codeEgaOff++, 0xcb, 1);
		//*code++ = 0xcb;
	}


//
// copy the final shape to a properly sized buffer
//
	totalsize = codeEgaOff-workEgaOff;
	//totalsize = FP_OFF(code);
#ifdef REFKEEN_VER_CATADVENTURES

	if (totalsize >= (PAGELEN*2))
		Quit("BuildCompShape(): Shape is too complex!");

#endif
	MM_GetPtr ((memptr *)finalspot,totalsize);
	BE_ST_EGAFetchGFXBuffer((id0_byte_t *)*finalspot, workEgaOff, totalsize, 0);
	//_fmemcpy ((id0_byte_t id0_seg *)(*finalspot),(id0_byte_t id0_seg *)work,totalsize);
//	MM_FreePtr ((memptr *)&work);

	return totalsize;
}



// (REFKEEN) Replacement for functions generated by BuildCompShape
static void ExecuteCompShape(const id0_byte_t *codePtr, id0_byte_t *comptablebuff, id0_unsigned_t egaDestOff, const id0_byte_t *srcGfxPtr, id0_byte_t mask)
{
	// WARNING: All offsets into segments here are treated as SIGNED
	// - at least one case fixes fireball drawing glitch
	// (applies e.g., to ds:[-7] but does NOT seem to apply to ds:[si])
	const id0_byte_t *currCodePtr = codePtr;
	while (*currCodePtr != 0xcb)
	{
		// patch far return into scaler:
		//
		// mov bx,[lastpix*2]
		id0_byte_t *scalerFarRetPtr = comptablebuff + *(id0_int_t *)(comptablebuff+(*(id0_int_t *)(currCodePtr+2)));
		// mov cx,[bx] ; Actually we store in scalerPreFarRet just a byte
		id0_byte_t scalerPreFarRet = *scalerFarRetPtr;
		// move [BYTE bx],0xcb
		*scalerFarRetPtr = 0xcb; // far return
		// mov ax,[firstpix*2]
		// mov [ss:0],ax
		// mov ds,dx	(mov ds,cs)
		// mov si,OFFSET pixelofs-firstpixel
		// call [DWORD bp]
		ExecuteCompScale(comptablebuff + *(id0_int_t *)(comptablebuff+(*(id0_int_t *)(currCodePtr+10))), egaDestOff, srcGfxPtr+(*(id0_int_t *)(currCodePtr+19)), mask);
		// mov ds,[bp+2]
		//
		// unpatch far return:
		// mov [bx],cx
		*scalerFarRetPtr = scalerPreFarRet;
		currCodePtr += 29;
	}
}

/*
=======================
=
= ScaleShape
=
= Draws a compiled shape at [scale] pixels high
=
= Setup for call
= --------------
= GC_MODE			read mode 1, write mode 2
= GC_COLORDONTCARE  set to 0, so all reads from video memory return 0xff
= GC_INDEX			pointing at GC_BITMASK
=
=======================
*/

//static	id0_long_t		longtemp;

void ScaleShape (id0_int_t xcenter, t_compshape id0_seg *compshape, id0_unsigned_t scale)
{
	// REFKEEN - In vanilla Catacomb Abyss MAX_OBJ_SCALE is defined
	// to be (MAXSCALE) here, and is used as a bound for scale below
	// instead of MAXSCALE. Since MAXSCALE is defined as (VIEWWIDTH/2)
	// in Catacomb 3-D and all episodes of the Adventures series
	// (so the parentheses are guarding from unexpected behaviors),
	// there's no real need to use MAX_OBJ_SCALE here.
	t_compscale id0_seg *comptable;
	id0_unsigned_t	width,scalewidth;
	id0_int_t			/*x,*/pixel,lastpixel,pixwidth,min;
	id0_unsigned_t	id0_far *codehandle, id0_far *widthptr;
	id0_unsigned_t	badcodeptr;
	id0_int_t			rightclip;

	if (!compshape)
		Quit ("ScaleShape: NULL compshape ptr!");

	scale = (scale+1)/2;
	if (!scale)
		return;								// too far away

	if (scale>MAXSCALE)
		scale = MAXSCALE;

	comptable = scaledirectory[scale];

	width = compshape->width;
	scalewidth = comptable->start[width];

	pixel = xcenter - scalewidth/2;
	lastpixel = pixel+scalewidth-1;

	if (pixel >= VIEWWIDTH || lastpixel < 0)
		return;								// totally off screen

//
// scan backwards from the right edge until pixels are visable
// rightclip is the first NON VISABLE pixel
//
	if (lastpixel>=VIEWWIDTH-1)
		rightclip = VIEWWIDTH-1;
	else
		rightclip = lastpixel;

	if (zbuffer[rightclip]>scale)
	{
		if (pixel>0)
			min = pixel;
		else
			min = 0;
		do
		{
			if (--rightclip < min)
				return;							// totally covered or before 0
			if (zbuffer[rightclip]<=scale)
				break;
		} while (1);
	}
	rightclip++;

//
// scan from the left until it is on screen, leaving
// [pixel],[pixwidth],[codehandle],and [widthptr] set correctly
//
	//*(((id0_unsigned_t *)&longtemp)+1) = (id0_unsigned_t)compshape;	// seg of shape
	codehandle = &compshape->codeofs[0];
	badcodeptr = compshape->codeofs[width];
	widthptr = &comptable->width[0];
#if 0
	asm	mov	ax,[comptable]
	asm	mov	WORD PTR [2],ax				// ds:0-4 is used as a far call pointer
										// by the compiled shapes
#endif
	pixwidth = *widthptr;				// scaled width of this pixel
	while (!pixwidth)
	{
		pixwidth = *++widthptr;			// find the first visable pixel
		codehandle++;
	}

	if (pixel<0)
	{
		do
		{
			if (pixel+pixwidth>0)
			{
				pixwidth += pixel;
				pixel = 0;
				break;
			}
			do
			{
				pixwidth = *++widthptr;
				codehandle++;
			} while (!pixwidth);
			pixel+=pixwidth;
		} while (1);
	}

//
// scan until it is visable, leaving
// [pixel],[pixwidth],[codehandle],and [widthptr] set correctly
//
	do
	{
		if (zbuffer[pixel] <= scale)
			break;							// start drawing here
		pixel++;
		if (!--pixwidth)
		{
			do
			{
				pixwidth = *++widthptr;
				codehandle++;
			} while (!pixwidth);
		}
	} while (1);

	if (pixel+pixwidth>rightclip)
		pixwidth = rightclip-pixel;
//
// draw lines
//
	do		// while (1)
	{
	//
	// scale a vertical segment [pixwidth] pixels wide at [pixel]
	//
		if (*codehandle == badcodeptr)
		{
			Quit ("ScaleShape: codehandle past end!");
		}
#if 0
		(id0_unsigned_t)longtemp = *codehandle;	// offset of compiled code
		if ((id0_unsigned_t)longtemp == badcodeptr)
			Quit ("ScaleShape: codehandle past end!");
#endif
		id0_unsigned_t egaDestOff = ((id0_unsigned_t)pixel>>3) + bufferofs;
		// scale the line of pixels
		ExecuteCompShape((id0_byte_t *)compshape + (*codehandle), (id0_byte_t *)comptable, egaDestOff, (id0_byte_t *)compshape, bitmasks1[pixel&7][pixwidth-1]);
		if (bitmasks2[pixel&7][pixwidth-1])
		{
			//
			// draw a second byte for vertical strips that cross two bytes
			//
			++egaDestOff;
			ExecuteCompShape((id0_byte_t *)compshape + (*codehandle), (id0_byte_t *)comptable, egaDestOff, (id0_byte_t *)compshape, bitmasks2[pixel&7][pixwidth-1]);
		}
#if 0
		asm	mov	bx,[pixel]
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
		asm	push	bx
		asm	mov	al,BYTE PTR [bitmasks1+bx]
		asm	mov	dx,GC_INDEX+1
		asm	out	dx,al						// set bit mask register

		asm	mov	es,[screenseg]
		asm	push	si
		asm	push	di
		asm	push	bp
		asm	xor	bp,bp
		asm	mov	dx,[WORD PTR longtemp+2]
		asm	mov	ds,[2]
		asm	call ss:[DWORD PTR longtemp]		// scale the line of pixels
		asm	mov	ax,ss
		asm	mov	ds,ax
		asm	pop		bp
		asm	pop		di
		asm	pop		si

		asm	pop	bx
		asm	mov	al,BYTE PTR [bitmasks2+bx]
		asm	or	al,al
		asm	jz	nosecond

	//
	// draw a second byte for vertical strips that cross two bytes
	//
		asm	inc	di
		asm	mov	dx,GC_INDEX+1
		asm	out	dx,al						// set bit mask register
		asm	push	si
		asm	push	di
		asm	push	bp
		asm	xor	bp,bp
		asm	mov	dx,[WORD PTR longtemp+2]
		asm	mov	ds,[2]
		asm	call ss:[DWORD PTR longtemp]		// scale the line of pixels
		asm	mov	ax,ss
		asm	mov	ds,ax
		asm	pop		bp
		asm	pop		di
		asm	pop		si


	//
	// advance to the next drawn line
	//
nosecond:;
#endif
		if ( (pixel+=pixwidth) == rightclip )
		{
#if 0
			asm	mov	WORD PTR [0],0
			asm	mov	WORD PTR [2],0
#endif
			return;							// all done!
		}

		do
		{
			pixwidth = *++widthptr;
			codehandle++;
		} while (!pixwidth);

		if (pixel+pixwidth > rightclip)
			pixwidth = rightclip-pixel;

	} while (1);
}

//
// bit mask tables for drawing scaled strips up to eight pixels wide
//

id0_byte_t	bitmasks1[8][8] = {
{0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff},
{0x40,0x60,0x70,0x78,0x7c,0x7e,0x7f,0x7f},
{0x20,0x30,0x38,0x3c,0x3e,0x3f,0x3f,0x3f},
{0x10,0x18,0x1c,0x1e,0x1f,0x1f,0x1f,0x1f},
{0x8,0xc,0xe,0xf,0xf,0xf,0xf,0xf},
{0x4,0x6,0x7,0x7,0x7,0x7,0x7,0x7},
{0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3},
{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1} };

id0_byte_t	bitmasks2[8][8] = {
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0x80},
{0,0,0,0,0,0,0x80,0xc0},
{0,0,0,0,0,0x80,0xc0,0xe0},
{0,0,0,0,0x80,0xc0,0xe0,0xf0},
{0,0,0,0x80,0xc0,0xe0,0xf0,0xf8},
{0,0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc},
{0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe} };






