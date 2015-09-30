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
MM_SizePtr to change the size of a given pointer

Multiple purge levels utilized

EMS / XMS unmanaged routines

=============================================================================
*/

#include "id_heads.h"
//#pragma hdrstop

//#pragma warn -pro
//#pragma warn -use

// REFKEEN - Use macro for all titles, with the original message

#ifdef REFKEEN_VER_CATABYSS

#define OUT_OF_MEM_MSG "\n"                                                                          \
							  "You need more free memory to run CATACOMB ABYSS.  Type START [ENTER]\n"  \
							  "and read the \"INFORMATION YOU SHOULD KNOW BEFORE PLAYING\" section\n"       \
							  "for more information about this.\n"

#elif defined REFKEEN_VER_CATARM

#define OUT_OF_MEM_MSG	"\n"                                                                          \
								"You need more memory to run CATACOMB ARMAGEDDON.  Read the INSTRUCTION\n"   \
								"section of the START program for tips on getting more memory.\n"

#elif defined REFKEEN_VER_CATAPOC

#define OUT_OF_MEM_MSG	"\n"                                                                          \
								"You need more memory to run CATACOMB APOCALYPSE.  Read the INSTRUCTION\n"   \
								"section of the START program for tips on getting more memory.\n"

#else // Default for e.g., Catacomb 3-D

#define OUT_OF_MEM_MSG	"MM_GetPtr: Out of memory!"

#endif


/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable

#define MAXUMBS		10

typedef struct mmblockstruct
{
	id0_unsigned_t	start,length;
	id0_unsigned_t	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct id0_far *next;
} mmblocktype;

/*
#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!")\
	;mmfree=mmfree->next;}
*/
#define GETNEWBLOCK {if(!mmfree)MML_ClearBlock();mmnew=mmfree;mmfree=mmfree->next;}

// (REFKEEN) Taking care of useptr==NULL (the assignment of NULL to a
// small var at dseg:0000 is "harmless" and shouldn't change the value)
#define FREEBLOCK(x) {if (x->useptr) *x->useptr=NULL;x->next=mmfree;mmfree=x;}
//#define FREEBLOCK(x) {*x->useptr=NULL;x->next=mmfree;mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

mminfotype	mminfo;
memptr		bufferseg;
id0_boolean_t		mmerror;

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

id0_boolean_t		bombonerror;

//id0_unsigned_t	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;

//void		(* XMSaddr) (void);		// far pointer to XMS driver

//id0_unsigned_t	numUMBs,UMBbase[MAXUMBS];

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
void		MML_UseSpace (id0_unsigned_t segstart, id0_unsigned_t seglength);
void 		MML_ClearBlock (void);

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

id0_char_t	emmname[9] = "EMMXXXX0";

id0_boolean_t MML_CheckForEMS (void)
{
asm	mov	dx,OFFSET emmname[0]
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
= MML_SetupEMS
=
=======================
*/

void MML_SetupEMS (void)
{
	id0_char_t	str[80],str2[10];
	id0_unsigned_t	error;

	totalEMSpages = freeEMSpages = EMSpageframe = EMSpagesmapped = 0;

asm {
	mov	ah,EMS_STATUS
	int	EMS_INT						// make sure EMS hardware is present
	or	ah,ah
	jnz	error

	mov	ah,EMS_VERSION
	int	EMS_INT
	or	ah,ah
	jnz	error
	cmp	al,0x32						// only work on ems 3.2 or greater
	jb	error

	mov	ah,EMS_GETFRAME
	int	EMS_INT						// find the page frame address
	or	ah,ah
	jnz	error
	mov	[EMSpageframe],bx

	mov	ah,EMS_GETPAGES
	int	EMS_INT						// find out how much EMS is there
	or	ah,ah
	jnz	error
	mov	[totalEMSpages],dx
	mov	[freeEMSpages],bx
	or	bx,bx
	jz	noEMS						// no EMS at all to allocate

	cmp	bx,4
	jle	getpages					// there is only 1,2,3,or 4 pages
	mov	bx,4						// we can't use more than 4 pages
	}

getpages:
asm {
	mov	[EMSpagesmapped],bx
	mov	ah,EMS_ALLOCPAGES			// allocate up to 64k of EMS
	int	EMS_INT
	or	ah,ah
	jnz	error
	mov	[EMShandle],dx
	}
	return;

error:
	error = _AH;
	strcpy (str,"MML_SetupEMS: EMS error 0x");
	itoa(error,str2,16);
	strcpy (str,str2);
	Quit (str);

noEMS:
;
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
	if (!EMShandle)
		return;

asm	{
	mov	ah,EMS_FREEPAGES
	mov	dx,[EMShandle]
	int	EMS_INT
	or	ah,ah
	jz	ok
	}

	Quit ("MML_ShutdownEMS: Error freeing EMS!");

ok:
;
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
	id0_char_t	str[80],str2[10];
	id0_unsigned_t	error;
	int	i;

	for (i=0;i<EMSpagesmapped;i++)
	{
	asm	{
		mov	ah,EMS_MAPPAGE
		mov	bx,[i]			// logical page
		mov	al,bl			// physical page
		mov	dx,[EMShandle]	// handle
		int	EMS_INT
		or	ah,ah
		jnz	error
		}
	}

	return;

error:
	error = _AH;
	strcpy (str,"MM_MapEMS: EMS error 0x");
	itoa(error,str2,16);
	strcpy (str,str2);
	Quit (str);
}

//==========================================================================

/*
======================
=
= MML_CheckForXMS
=
= Check for XMM driver
=
=======================
*/

id0_boolean_t MML_CheckForXMS (void)
{
	numUMBs = 0;

asm {
	mov	ax,0x4300
	int	0x2f				// query status of installed diver
	cmp	al,0x80
	je	good
	}
	return false;
good:
	return true;
}


/*
======================
=
= MML_SetupXMS
=
= Try to allocate all upper memory block
=
=======================
*/

void MML_SetupXMS (void)
{
	id0_unsigned_t	base,size;

asm	{
	mov	ax,0x4310
	int	0x2f
	mov	[WORD PTR XMSaddr],bx
	mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver
	}

getmemory:
asm	{
	mov	ah,XMS_ALLOCUMB
	mov	dx,0xffff					// try for largest block possible
	call	[DWORD PTR XMSaddr]
	or	ax,ax
	jnz	gotone

	cmp	bl,0xb0						// error: smaller UMB is available
	jne	done;

	mov	ah,XMS_ALLOCUMB
	call	[DWORD PTR XMSaddr]		// DX holds largest available UMB
	or	ax,ax
	jz	done						// another error...
	}

gotone:
asm	{
	mov	[base],bx
	mov	[size],dx
	}
	MML_UseSpace (base,size);
	mminfo.XMSmem += size*16;
	UMBbase[numUMBs] = base;
	numUMBs++;
	if (numUMBs < MAXUMBS)
		goto getmemory;

done:;
}


/*
======================
=
= MML_ShutdownXMS
=
======================
*/

void MML_ShutdownXMS (void)
{
	int	i;
	id0_unsigned_t	base;

	for (i=0;i<numUMBs;i++)
	{
		base = UMBbase[i];

asm	mov	ah,XMS_FREEUMB
asm	mov	dx,[base]
asm	call	[DWORD PTR XMSaddr]
	}
}
#endif

//==========================================================================

/*
======================
=
= MML_UseSpace
=
= Marks a range of paragraphs as usable by the memory manager
= This is used to mark space for the near heap, far heap, ems page frame,
= and upper memory blocks
=
======================
*/

void MML_UseSpace (id0_unsigned_t segstart, id0_unsigned_t seglength)
{
	mmblocktype id0_far *scan,id0_far *last;
	id0_unsigned_t	oldend;
	id0_long_t		extra;

	scan = last = mmhead;
	mmrover = mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while (scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
	extra = oldend - (segstart+seglength);
	if (extra < 0)
		Quit ("MML_UseSpace: Segment spans two blocks!");

	if (segstart == scan->start)
	{
		last->next = scan->next;			// unlink block
		FREEBLOCK(scan);
		scan = last;
	}
	else
		scan->length = segstart-scan->start;	// shorten block

	if (extra > 0)
	{
		GETNEWBLOCK;
		mmnew->next = scan->next;
		scan->next = mmnew;
		mmnew->start = segstart+seglength;
		mmnew->length = extra;
		mmnew->attributes = LOCKBIT;
	}

}

//==========================================================================

/*
====================
=
= MML_ClearBlock
=
= We are out of blocks, so free a purgable block
=
====================
*/

void MML_ClearBlock (void)
{
	mmblocktype id0_far *scan/*,id0_far *last*/;

	scan = mmhead->next;

	while (scan)
	{
		if (!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS) )
		{
			MM_FreePtr(scan->useptr);
			return;
		}
		scan = scan->next;
	}

	Quit ("MM_ClearBlock: No purgable blocks!");
}


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

static const id0_char_t *ParmStrings[] = {"noems","noxms",""};

void MM_Startup (void)
{
	id0_int_t i;
	id0_unsigned_long_t length;
	//void id0_far 	*start;
	id0_unsigned_t 	segstart,seglength/*,endfree*/;

	if (mmstarted)
		MM_Shutdown ();


	mmstarted = true;
	bombonerror = true;
//
// set up the linked list (everything in the free list;
//
	mmhead = NULL;
	mmfree = &mmblocks[0];
	for (i=0;i<MAXBLOCKS-1;i++)
		mmblocks[i].next = &mmblocks[i+1];
	mmblocks[i].next = NULL;

//
// locked block of all memory until we punch out free space
//
	GETNEWBLOCK;
	mmhead = mmnew;				// this will always be the first node
	mmnew->start = 0;
	mmnew->length = 0xffff;
	mmnew->attributes = LOCKBIT;
	mmnew->next = NULL;
	mmrover = mmhead;


//
// get all available near conventional memory segments
//
	length = EMULATED_NEAR_PARAGRAPHS*16 - SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = EMULATED_NEAR_SEG;
	MML_UseSpace (segstart,seglength);
	mminfo.nearheap = length;
#if 0
	length=coreleft();
	start = (void id0_far *)(nearheap = malloc(length));

	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.nearheap = length;
#endif

//
// get all available far conventional memory segments
//
	length = EMULATED_FAR_PARAGRAPHS*16 - SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = EMULATED_FAR_SEG;
	MML_UseSpace (segstart,seglength);
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;
#if 0
	length=farcoreleft();
	start = farheap = farmalloc(length);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;
#endif


//
// detect EMS and allocate up to 64K at page frame
//
	mminfo.EMSmem = 0;
	for (i = 1;i < id0_argc;i++)
	{
		if ( US_CheckParm(id0_argv[i],ParmStrings) == 0)
			goto emsskip;				// param NOEMS
	}

	seglength = EMULATED_EMS_PARAGRAPHS;
	segstart = EMULATED_EMS_SEG;
	length = seglength*16;
	MML_UseSpace (segstart,seglength);
	mminfo.EMSmem = length;
#if 0
	if (MML_CheckForEMS())
	{
		MML_SetupEMS();					// allocate space
		MML_UseSpace (EMSpageframe,EMSpagesmapped*0x400);
		MM_MapEMS();					// map in used pages
		mminfo.EMSmem = EMSpagesmapped*0x4000l;
	}
#endif

//
// detect XMS and get upper memory blocks
//
emsskip:
	mminfo.XMSmem = 0;
	for (i = 1;i < id0_argc;i++)
	{
		if ( US_CheckParm(id0_argv[i],ParmStrings) == 0)
			goto xmsskip;				// param NOXMS
	}

	seglength = EMULATED_XMS_PARAGRAPHS;
	segstart = EMULATED_XMS_SEG;
	length = seglength*16;
	MML_UseSpace (segstart,seglength);
	mminfo.XMSmem = length;
#if 0
	if (MML_CheckForXMS())
		MML_SetupXMS();					// allocate as many UMBs as possible
#endif

//
// allocate the misc buffer
//
xmsskip:
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

	if (bombonerror)
		// REFKEEN - Always use macro, but with a game-specific message
#if (defined REFKEEN_VER_CATADVENTURES) && !(defined REFKEEN_VER_CATABYSS)
		Quit (OUT_OF_MEM_MSG,(size-mminfo.nearheap));
#else
		Quit (OUT_OF_MEM_MSG);
#endif
	else
		mmerror = true;
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
	id0_unsigned_t	start,length,source,dest/*,oldborder*/;
#ifndef REFKEEN_VER_CATADVENTURES
	id0_unsigned_t oldborder;
#endif
	id0_int_t			playing;

	//
	// lock down a currently playing sound
	//
	playing = SD_SoundPlaying ();
	if (playing)
	{
		switch (SoundMode)
		{
		case sdm_PC:
			playing += STARTPCSOUNDS;
			break;
		case sdm_AdLib:
			playing += STARTADLIBSOUNDS;
			break;
		}
		MM_SetLock((memptr *)&audiosegs[playing],true);
	}


	SD_StopSound();
#ifndef REFKEEN_VER_CATADVENTURES
	oldborder = bordercolor;
	VW_ColorBorder (15);
#endif
	if (beforesort)
		beforesort();

	scan = mmhead;

	last = NULL;		// shut up compiler warning

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

#ifndef REFKEEN_VER_CATADVENTURES
	VW_ColorBorder (oldborder);
#endif
	if (playing)
		MM_SetLock((memptr *)&audiosegs[playing],false);
}


//==========================================================================

#ifndef REFKEEN_VER_CATADVENTURES
/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory (void)
{
	mmblocktype id0_far *scan;
	id0_unsigned_t color,temp;
	id0_long_t	end/*,owner*/;
	//id0_char_t    scratch[80],str[10];

	VW_SetDefaultColors();
	VW_SetLineWidth(40);
	temp = bufferofs;
	bufferofs = 0;
	VW_SetScreen (0,0);

	scan = mmhead;

	end = -1;

//CA_OpenDebug ();

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

#if 0
strcpy (scratch,"Size:");
ltoa ((id0_long_t)scan->length*16,str,10);
strcat (scratch,str);
strcat (scratch,"\tOwner:0x");
owner = (id0_unsigned_t)scan->useptr;
ultoa (owner,str,16);
strcat (scratch,str);
strcat (scratch,"\n");
write (debughandle,scratch,strlen(scratch));
#endif

		scan = scan->next;
	}

//CA_CloseDebug ();

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

//==========================================================================

/*
=====================
=
= MM_BombOnError
=
=====================
*/

void MM_BombOnError (id0_boolean_t bomb)
{
	bombonerror = bomb;
}


