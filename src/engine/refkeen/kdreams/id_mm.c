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

// NEWMM.C

/*
=============================================================================

		   ID software memory manager
		   --------------------------

Primary coder: John Carmack

RELIES ON
---------
Quit (char *error) function


WORK TO DO
----------
Soft error on out of memory

EMS 64k frame, upper memory block mapping

MM_SizePtr to change the size of a given pointer

Multiple purge levels utilized

EMS / XMS unmanaged routines

=============================================================================
*/

#include "id_heads.h"
//#pragma hdrstop

/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define EMSINT		0x67

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable

typedef struct mmblockstruct
{
	id0_unsigned_t	start,length;
	id0_unsigned_t	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct id0_far *next;
} mmblocktype;


#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!")\
	;mmfree=mmfree->next;}

#define FREEBLOCK(x) {*x->useptr=NULL;x->next=mmfree;mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

mminfotype	mminfo;
memptr		bufferseg;
id0_boolean_t		bombonerror;

void		(* beforesort) (void);
void		(* aftersort) (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

id0_boolean_t		mmstarted;

//void id0_far	*farheap;
//void		*nearheap;

mmblocktype	id0_far mmblocks[MAXBLOCKS]
			,id0_far *mmhead,id0_far *mmfree,id0_far *mmrover,id0_far *mmnew;

// A static memory buffer used for our allocations, made out of 16-bytes
// long paragraphs (each of them beginning with some emulated "segment")
//
// NOTE: Main mem (near+far) should consist of 335*1024 bytes
// (for Keen Dreams with EGA graphics)
//
// Based on tests with Catacomb Abyss (even if not precise), targetting...
// - 3408 bytes returned by coreleft (before removing SAVENEARHEAP and up to 15 more bytes).
// - 448592 returned by farcoreleft.
// - 0 EMS bytes.
// - 65520 XMS bytes.

// The very first "segment" in the emulated space
#define EMULATED_FIRST_SEG 0
// Different portions of the space being emulated - start points
#define EMULATED_NEAR_SEG (EMULATED_FIRST_SEG+EMULATED_FIRST_PARAGRAPHS)
#define EMULATED_FAR_SEG (EMULATED_NEAR_SEG+EMULATED_NEAR_PARAGRAPHS)
#define EMULATED_EMS_SEG (EMULATED_FAR_SEG+EMULATED_FAR_PARAGRAPHS)
#define EMULATED_XMS_SEG (EMULATED_EMS_SEG+EMULATED_EMS_PARAGRAPHS)
// Lengths in paragraphs of the different sections
#define EMULATED_FIRST_PARAGRAPHS 4096
#define EMULATED_NEAR_PARAGRAPHS 213
#define EMULATED_FAR_PARAGRAPHS 28037
#define EMULATED_EMS_PARAGRAPHS 0 // Yes!
#define EMULATED_XMS_PARAGRAPHS 4095
// Used to obtain a pointer to some location in mmEmulatedMemSpace
#define EMULATED_SEG_TO_PTR(seg) (mmEmulatedMemSpace+(seg)*16)

static id0_byte_t mmEmulatedMemSpace[16*(EMULATED_FIRST_PARAGRAPHS+EMULATED_NEAR_PARAGRAPHS+EMULATED_FAR_PARAGRAPHS+EMULATED_EMS_PARAGRAPHS+EMULATED_XMS_PARAGRAPHS)];

//==========================================================================

//
// local prototypes
//

//id0_boolean_t		MML_CheckForEMS (void);
//void 		MML_ShutdownEMS (void);
//void 		MM_MapEMS (void);
//id0_boolean_t 	MML_CheckForXMS (void);
//void 		MML_ShutdownXMS (void);

//==========================================================================

#if 0
/*
======================
=
= MML_CheckForEMS
=
= Routine from p36 of Extending DOS
=
=======================
*/

id0_boolean_t MML_CheckForEMS (void)
{
  id0_char_t	emmname[9] = "EMMXXXX0";

asm	mov	dx,OFFSET emmname
asm	mov	ax,0x3d00
asm	int	0x21		// try to open EMMXXXX0 device
asm	jc	error

asm	mov	bx,ax
asm	mov	ax,0x4400

asm	int	0x21		// get device info
asm	jc	error

asm	and	dx,0x80
asm	jz	error

asm	mov	ax,0x4407

asm	int	0x21		// get status
asm	jc	error
asm	or	al,al
asm	jz	error

asm	mov	ah,0x3e
asm	int	0x21		// close handle
asm	jc	error

//
// EMS is good
//
  return true;

error:
//
// EMS is bad
//
  return false;
}

/*
======================
=
= MML_ShutdownEMS
=
=======================
*/

void MML_ShutdownEMS (void)
{

}

/*
====================
=
= MM_MapEMS
=
= Maps the 64k of EMS used by memory manager into the page frame
= for general use.  This only needs to be called if you are keeping
= other things in EMS.
=
====================
*/

void MM_MapEMS (void)
{

}

//==========================================================================

/*
======================
=
= MML_CheckForXMS
=
= Try to allocate an upper memory block
=
=======================
*/

id0_boolean_t MML_CheckForXMS (void)
{

	return false;
}


/*
======================
=
= MML_ShutdownXMS
=
=======================
*/

void MML_ShutdownXMS (void)
{

}
#endif

//==========================================================================

/*
===================
=
= MM_Startup
=
= Grabs all space from turbo with malloc/farmalloc
= Allocates bufferseg misc buffer
=
===================
*/

void MM_Startup (void)
{
	id0_int_t i;
	id0_unsigned_long_t length;
	//void id0_far 	*start;
	id0_unsigned_t 	segstart,seglength,endfree;

	if (mmstarted)
		MM_Shutdown ();

	mmstarted = true;
	bombonerror = true;

//
// set up the linked list (everything in the free list)
//
	mmhead = NULL;
	mmfree = &mmblocks[0];
	for (i=0;i<MAXBLOCKS-1;i++)
		mmblocks[i].next = &mmblocks[i+1];
	mmblocks[i].next = NULL;

//
// get all available near conventional memory segments
//
	length = EMULATED_NEAR_PARAGRAPHS*16 - SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = EMULATED_NEAR_SEG;
	mminfo.nearheap = length;
#if 0
	length=coreleft();
	start = (void id0_far *)(nearheap = malloc(length));

	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	mminfo.nearheap = length;
#endif

	// locked block of unusable low memory
	// from 0 to start of near heap
	GETNEWBLOCK;
	mmhead = mmnew;				// this will always be the first node
	mmnew->start = 0;
	mmnew->length = segstart;
	mmnew->attributes = LOCKBIT;
	endfree = segstart+seglength;
	mmrover = mmhead;

//
// get all available far conventional memory segments
//
	length = EMULATED_FAR_PARAGRAPHS*16 - SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = EMULATED_FAR_SEG;
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;
#if 0
	length=farcoreleft();
	start = farheap = farmalloc(length);

	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;
#endif

	// locked block of unusable near heap memory (usually just the stack)
	// from end of near heap to start of far heap
	GETNEWBLOCK;
	mmnew->start = endfree;
	mmnew->length = segstart-endfree;
	mmnew->attributes = LOCKBIT;
	mmrover->next = mmnew;
	endfree = segstart+seglength;
	mmrover = mmnew;


//
// detect EMS and allocate 64K at page frame
//
	mminfo.EMSmem = 0x10000; // Looks unused
#if 0
	if (MML_CheckForEMS())
	{
		MM_MapEMS();					// map in used pages
		mminfo.EMSmem = 0x10000l;
	}
	else
	{
		mminfo.EMSmem = 0;
	}
#endif

//
// detect XMS and get upper memory blocks
//
	mminfo.XMSmem = 0; // Unused
#if 0
	if (MML_CheckForXMS())
	{

	}
	else
	{
		mminfo.XMSmem = 0;
	}
#endif


//
// cap off the list
//
	// locked block of high memory (video, rom, etc)
	// from end of far heap or EMS/XMS to 0xffff
	GETNEWBLOCK;
	mmnew->start = endfree;
	mmnew->length = 0xffff-endfree;
	mmnew->attributes = LOCKBIT;
	mmnew->next = NULL;
	mmrover->next = mmnew;

//
// allocate the misc buffer
//
	mmrover = mmhead;		// start looking for space after low block

	MM_GetPtr (&bufferseg,BUFFERSIZE);
}

//==========================================================================

/*
====================
=
= MM_Shutdown
=
= Frees all conventional, EMS, and XMS allocated
=
====================
*/

void MM_Shutdown (void)
{
#if 0
  if (!mmstarted)
	return;

  farfree (farheap);
  free (nearheap);
  MML_ShutdownEMS ();
  MML_ShutdownXMS ();
#endif
}

//==========================================================================

/*
====================
=
= MM_GetPtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_GetPtr (memptr *baseptr,id0_unsigned_long_t size)
{
	mmblocktype id0_far *scan,id0_far *lastscan,id0_far *endscan
				,id0_far *purge,id0_far *next;
	id0_int_t			search;
	id0_unsigned_t	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	GETNEWBLOCK;				// fill in start and next after a spot is found
	mmnew->length = needed;
	mmnew->useptr = baseptr;
	mmnew->attributes = BASEATTRIBUTES;

	for (search = 0; search<3; search++)
	{
	//
	// first search:	try to allocate right after the rover, then on up
	// second search: 	search from the head pointer up to the rover
	// third search:	compress memory, then scan from start
		if (search == 1 && mmrover == mmhead)
			search++;

		switch (search)
		{
		case 0:
			lastscan = mmrover;
			scan = mmrover->next;
			endscan = NULL;
			break;
		case 1:
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = mmrover;
			break;
		case 2:
			MM_SortMem ();
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = NULL;
			break;
		}

		startseg = lastscan->start + lastscan->length;

		while (scan != endscan)
		{
			if (scan->start - startseg >= needed)
			{
			//
			// got enough space between the end of lastscan and
			// the start of scan, so throw out anything in the middle
			// and allocate the new block
			//
				purge = lastscan->next;
				lastscan->next = mmnew;
				mmnew->start /*= *(id0_unsigned_t *)baseptr*/ = startseg;
				*baseptr = EMULATED_SEG_TO_PTR(startseg);
				mmnew->next = scan;
				while ( purge != scan)
				{	// free the purgable block
					next = purge->next;
					FREEBLOCK(purge);
					purge = next;		// purge another if not at scan
				}
				mmrover = mmnew;
				return;	// good allocation!
			}

			//
			// if this block is purge level zero or locked, skip past it
			//
			if ( (scan->attributes & LOCKBIT)
				|| !(scan->attributes & PURGEBITS) )
			{
				lastscan = scan;
				startseg = lastscan->start + lastscan->length;
			}


			scan=scan->next;		// look at next line
		}
	}

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	Quit ("MM_GetPtr: Out of memory!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	Quit ("Out of memory!  Please make sure you have enough free memory.");
#endif
}

//==========================================================================

/*
====================
=
= MM_FreePtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_FreePtr (memptr *baseptr)
{
	mmblocktype id0_far *scan,id0_far *last;

	last = mmhead;
	scan = last->next;

	if (baseptr == mmrover->useptr)	// removed the last allocated block
		mmrover = mmhead;

	while (scan->useptr != baseptr && scan)
	{
		last = scan;
		scan = scan->next;
	}

	if (!scan)
		Quit ("MM_FreePtr: Block not found!");

	last->next = scan->next;

	FREEBLOCK(scan);
}
//==========================================================================

/*
=====================
=
= MM_SetPurge
=
= Sets the purge level for a block (locked blocks cannot be made purgable)
=
=====================
*/

void MM_SetPurge (memptr *baseptr, id0_int_t purge)
{
	mmblocktype id0_far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
			Quit ("MM_SetPurge: Block not found!");

	} while (1);

	mmrover->attributes &= ~PURGEBITS;
	mmrover->attributes |= purge;
}

//==========================================================================

/*
=====================
=
= MM_SetLock
=
= Locks / unlocks the block
=
=====================
*/

void MM_SetLock (memptr *baseptr, id0_boolean_t locked)
{
	mmblocktype id0_far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
			Quit ("MM_SetLock: Block not found!");

	} while (1);

	mmrover->attributes &= ~LOCKBIT;
	mmrover->attributes |= locked*LOCKBIT;
}

//==========================================================================

/*
=====================
=
= MM_SortMem
=
= Throws out all purgable stuff and compresses movable blocks
=
=====================
*/

void MM_SortMem (void)
{
	mmblocktype id0_far *scan,id0_far *last,id0_far *next;
	id0_unsigned_t	start,length,source,dest;

	VW_ColorBorder (15);
	// (REFKEEN) HACK: Actually show border color
	BE_ST_ShortSleep();

	if (beforesort)
		beforesort();

	scan = mmhead;

	while (scan)
	{
		if (scan->attributes & LOCKBIT)
		{
		//
		// block is locked, so try to pile later blocks right after it
		//
			start = scan->start + scan->length;
		}
		else
		{
			if (scan->attributes & PURGEBITS)
			{
			//
			// throw out the purgable block
			//
				next = scan->next;
				FREEBLOCK(scan);
				last->next = next;
				scan = next;
				continue;
			}
			else
			{
			//
			// push the non purgable block on top of the last moved block
			//
				if (scan->start != start)
				{
					length = scan->length;
					source = scan->start;
					dest = start;
					while (length > 0xf00)
					{
						memmove(EMULATED_SEG_TO_PTR(dest), EMULATED_SEG_TO_PTR(source), 0xf00*16);
						//movedata(source,0,dest,0,0xf00*16);
						length -= 0xf00;
						source += 0xf00;
						dest += 0xf00;
					}
					memmove(EMULATED_SEG_TO_PTR(dest), EMULATED_SEG_TO_PTR(source), length*16);
					//movedata(source,0,dest,0,length*16);

					scan->start = start;
					//*(id0_unsigned_t *)scan->useptr = start;
					*(scan->useptr) = EMULATED_SEG_TO_PTR(start);
				}
				start = scan->start + scan->length;
			}
		}

		last = scan;
		scan = scan->next;		// go to next block
	}

	mmrover = mmhead;

	if (aftersort)
		aftersort();

	VW_ColorBorder (0);
}


//==========================================================================

/*
=====================
=
= MM_ShowMemory
=
=====================
*/

// (REFKEEN) Called only if GRMODE==EGAGR - assuming FRILLS is nonzero
// (VW_SetScreen has been disabled for CGA)

#if GRMODE == EGAGR
void MM_ShowMemory (void)
{
	mmblocktype id0_far *scan;
	id0_unsigned_t color,temp;
	id0_long_t	end;

	VW_SetLineWidth(40);
	temp = bufferofs;
	bufferofs = 0;
	VW_SetScreen (0,0);

	scan = mmhead;

	end = -1;

	while (scan)
	{
		if (scan->attributes & PURGEBITS)
			color = 5;		// dark purple = purgable
		else
			color = 9;		// medium blue = non purgable
		if (scan->attributes & LOCKBIT)
			color = 12;		// red = locked
		if (scan->start<=end)
			Quit ("MM_ShowMemory: Memory block order currupted!");
		end = scan->start+scan->length-1;
		VW_Hlin(scan->start,(id0_unsigned_t)end,0,color);
		VW_Plot(scan->start,0,15);
		if (scan->next->start > end+1)
			VW_Hlin(end+1,scan->next->start,0,0);	// black = free
		scan = scan->next;
	}

	IN_Ack();
	VW_SetLineWidth(64);
	bufferofs = temp;
}
#endif

//==========================================================================


/*
======================
=
= MM_UnusedMemory
=
= Returns the total free space without purging
=
======================
*/

id0_long_t MM_UnusedMemory (void)
{
	id0_unsigned_t free;
	mmblocktype id0_far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
}

//==========================================================================


/*
======================
=
= MM_TotalFree
=
= Returns the total free space with purging
=
======================
*/

id0_long_t MM_TotalFree (void)
{
	id0_unsigned_t free;
	mmblocktype id0_far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		if ((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
			free += scan->length;
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
}

