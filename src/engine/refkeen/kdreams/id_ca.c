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

// ID_CA.C

/*
=============================================================================

Id Software Caching Manager
---------------------------

Must be started BEFORE the memory manager, because it needs to get the headers
loaded into the data segment

=============================================================================
*/

#include "id_heads.h"
//#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

typedef struct
{
  id0_unsigned_t bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} __attribute__((__packed__)) huffnode;


typedef struct
{
	id0_unsigned_t	RLEWtag;
	id0_long_t		headeroffsets[100];
	id0_byte_t		headersize[100];		// headers are very small
	id0_byte_t		tileinfo[];
} __attribute__((__packed__)) mapfiletype;


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

id0_byte_t 		id0_seg	*tinf;
id0_int_t			mapon;

id0_unsigned_t	id0_seg	*mapsegs[3];
maptype		id0_seg	*mapheaderseg[NUMMAPS];
id0_byte_t		id0_seg	*audiosegs[NUMSNDCHUNKS];
void		id0_seg	*grsegs[NUMCHUNKS];

id0_byte_t		grneeded[NUMCHUNKS];
id0_byte_t		ca_levelbit,ca_levelnum;

const id0_char_t		*titleptr[8];

BE_FILE_T			profilehandle;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

extern	id0_long_t	*CGAhead;
extern	id0_long_t	*EGAhead;
extern	id0_byte_t	*CGAdict;
extern	id0_byte_t	*EGAdict;
extern	id0_byte_t	*maphead;
extern	id0_byte_t	*mapdict;
extern	id0_byte_t	*audiohead;
extern	id0_byte_t	*audiodict;


id0_long_t		id0_seg *grstarts;	// array of offsets in egagraph, -1 for sparse
id0_long_t		id0_seg *audiostarts;	// array of offsets in audio / audiot

#ifdef GRHEADERLINKED
huffnode	*grhuffman;
#else
huffnode	grhuffman[255];
#endif

#ifdef MAPHEADERLINKED
huffnode	*maphuffman;
#else
huffnode	maphuffman[255];
#endif

#ifdef AUDIOHEADERLINKED
huffnode	*audiohuffman;
#else
huffnode	audiohuffman[255];
#endif


BE_FILE_T			grhandle;		// handle to EGAGRAPH
BE_FILE_T			maphandle;		// handle to MAPTEMP / GAMEMAPS
BE_FILE_T			audiohandle;	// handle to AUDIOT / AUDIO

id0_long_t		chunkcomplen,chunkexplen;

SDMode		oldsoundmode;

/*
=============================================================================

					   LOW LEVEL ROUTINES

=============================================================================
*/

/*
============================
=
= CAL_GetGrChunkLength
=
= Gets the length of an explicit length chunk (not tiles)
= The file pointer is positioned so the compressed data can be read in next.
=
============================
*/

void CAL_GetGrChunkLength (id0_int_t chunk)
{
	BE_Cross_seek(grhandle,grstarts[chunk],SEEK_SET);
	BE_Cross_readInt32LE(grhandle, &chunkexplen);
	//read(grhandle,&chunkexplen,sizeof(chunkexplen));
	chunkcomplen = grstarts[chunk+1]-grstarts[chunk]-4;
}


/*
==========================
=
= CA_FarRead
=
= Read from a file to a far pointer
=
==========================
*/

id0_boolean_t CA_FarRead (BE_FILE_T handle, id0_byte_t id0_far *dest, id0_long_t length)
{
	if (length>0xffffl)
		Quit ("CA_FarRead doesn't support 64K reads yet!");
	// Ported from ASM
	int bytesread = BE_Cross_readInt8LEBuffer(handle, dest, length);
	if (bytesread == 0)
	{
		// Keep errno as set by read
		return false;
	}

	if (bytesread != length)
	{
		/* FIXME (REFKEEN) - Is that the right way? */
		errno = 11;
		//errno = EINVFMT; // user manager knows this is bad read
		return false;
	}
	return true;
}


/*
==========================
=
= CA_SegWrite
=
= Write from a file to a far pointer
=
==========================
*/

id0_boolean_t CA_FarWrite (BE_FILE_T handle, id0_byte_t id0_far *source, id0_long_t length)
{
	if (length>0xffffl)
		Quit ("CA_FarWrite doesn't support 64K reads yet!");
	// Ported from ASM
	int byteswritten = BE_Cross_writeInt8LEBuffer(handle, source, length);
	if (byteswritten == 0)
	{
		// Keep errno as set by write
		return false;
	}

	if (byteswritten != length)
	{
		errno = ENOMEM; // user manager knows this is bad write
		return false;
	}
	return true;
}


/*
==========================
=
= CA_LoadFile
=
= Allocate space for and load a file
=
==========================
*/

id0_boolean_t CA_LoadFile (const id0_char_t *filename, memptr *ptr)
{
	BE_FILE_T handle;
	id0_long_t size;

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(filename)))
	//if ((handle = open(filename,O_RDONLY | O_BINARY, S_IREAD)) == -1)
		return false;

	size = BE_Cross_FileLengthFromHandle (handle);
	MM_GetPtr (ptr,size);
	if (!CA_FarRead (handle,(id0_byte_t *)(*ptr),size))
	{
		BE_Cross_close (handle);
		return false;
	}
	BE_Cross_close (handle);
	return true;
}

/*
============================================================================

		COMPRESSION routines, see JHUFF.C for more

============================================================================
*/



/*
===============
=
= CAL_OptimizeNodes
=
= Goes through a huffman table and changes the 256-511 node numbers to the
= actular address of the node.  Must be called before CAL_HuffExpand
=
===============
*/

void CAL_OptimizeNodes (huffnode *table)
{
  /*** COMMENTED OUT - Not really useful in modern platforms as-is ***/
#if 0
  huffnode *node;
  id0_int_t i;

  node = table;

  for (i=0;i<255;i++)
  {
	if (node->bit0 >= 256)
	  node->bit0 = (id0_unsigned_t)(table+(node->bit0-256));
	if (node->bit1 >= 256)
	  node->bit1 = (id0_unsigned_t)(table+(node->bit1-256));
	node++;
  }
#endif
}



/*
======================
=
= CAL_HuffExpand
=
= Length is the length of the EXPANDED data
=
======================
*/

void CAL_HuffExpand (id0_byte_t id0_huge *source, id0_byte_t id0_huge *dest,
  id0_long_t length,huffnode *hufftable)
{
	id0_unsigned_t code;
	huffnode *headptr = hufftable+254; // head node is always node 254
	huffnode *nodeon = headptr;

	//------------
	// expand data
	//------------

	// Ported from ASM
	id0_byte_t id0_huge *srcptr = source, *dstptr = dest, *dstendptr = dest+length;
	id0_byte_t byteval = *(srcptr++); // load first byte
	id0_byte_t bitmask = 1;
	do
	{
		// take bit0 or bit1 path from node
		code = (byteval & bitmask) ? nodeon->bit1 : nodeon->bit0;
		if (bitmask & 0x80)
		{
			byteval = *(srcptr++); // load next byte
			bitmask = 1; // back to first bit
		}
		else
		{
			bitmask <<= 1; // advance to next bit position
		}
		// if < 256 it's a byte, else move node
		if (code >= 256)
		{
			// NOTE: CAL_OptimizeNodes does nothing now
			// and we do refer to hufftable itself here
			nodeon = hufftable + (code-256);
			continue;
		}
		*(dstptr++) = code; // write a decompressed byte out
		nodeon = headptr; // back to the head node for next bit

		if (dstptr == dstendptr) // done?
		{
			break;
		}
	} while (true);
}



/*
======================
=
= CA_RLEWcompress
=
======================
*/

id0_long_t CA_RLEWCompress (id0_unsigned_t id0_huge *source, id0_long_t length, id0_unsigned_t id0_huge *dest,
  id0_unsigned_t rlewtag)
{
  id0_long_t complength;
  id0_unsigned_t value,count,i;
  id0_unsigned_t id0_huge *start,id0_huge *end;

  start = dest;

  end = source + (length+1)/2;

//
// compress it
//
  do
  {
    count = 1;
    value = *source++;
    while (*source == value && source<end)
    {
      count++;
      source++;
    }
    if (count>3 || value == rlewtag)
    {
    //
    // send a tag / count / value string
    //
      *dest++ = rlewtag;
      *dest++ = count;
      *dest++ = value;
    }
    else
    {
    //
    // send word without compressing
    //
      for (i=1;i<=count;i++)
	*dest++ = value;
	}

  } while (source<end);

  complength = 2*(dest-start);
  return complength;
}


/*
======================
=
= CA_RLEWexpand
= length is COMPRESSED length
=
======================
*/

void CA_RLEWexpand (id0_unsigned_t id0_huge *source, id0_unsigned_t id0_huge *dest,id0_long_t length,
  id0_unsigned_t rlewtag)
{
  id0_unsigned_t value, count, i;
  id0_unsigned_t id0_huge *end = dest + (length)/2;


//
// expand it
//
  do
  {
	value = *source++;
	if (value != rlewtag)
	//
	// uncompressed
	//
	  *dest++=value;
	else
	{
	//
	// compressed string
	//
	  count = *source++;
	  value = *source++;
	  for (i=1;i<=count;i++)
	    *dest++ = value;
	}
  } while (dest<end);
}



/*
=============================================================================

					 CACHE MANAGER ROUTINES

=============================================================================
*/


/*
======================
=
= CAL_SetupGrFile
=
======================
*/

void CAL_SetupGrFile (void)
{
	// REFKEEN - Shut up compiler warnings
#ifndef GRHEADERLINKED
	BE_FILE_T handle;
#endif
#if (NUMPICS>0) || (NUMPICM>0) || (NUMSPRITES>0)
	memptr compseg;
#endif


#ifdef GRHEADERLINKED

#if GRMODE == EGAGR
	grhuffman = (huffnode *)EGAdict;
	grstarts = EGAhead;
#endif
#if GRMODE == CGAGR
	grhuffman = (huffnode *)CGAdict;
	grstarts = CGAhead;
#endif

	CAL_OptimizeNodes (grhuffman);

#else

//
// load ???dict.ext (huffman dictionary for graphics files)
//

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(GREXT"DICT."EXTENSION)))
	//if ((handle = open(GREXT"DICT."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open "GREXT"DICT."EXTENSION"!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading("KDREAMS.EGA")))
	//if ((handle = open("KDREAMS.EGA",
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open KDREAMS.EGA!");
#endif

	read(handle, &grhuffman, sizeof(grhuffman));
	BE_Cross_close(handle);
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	for (int i = 0; i < sizeof(grhuffman)/sizeof(*grhuffman); ++i)
	{
		grhuffman[i].bit0 = BE_Cross_Swap16LE(grhuffman[i].bit0);
		grhuffman[i].bit1 = BE_Cross_Swap16LE(grhuffman[i].bit1);
	}
#endif
	CAL_OptimizeNodes (grhuffman);
//
// load the data offsets from ???head.ext
//
	MM_GetPtr ((memptr *)&grstarts,(NUMCHUNKS+1)*4);

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(GREXT"HEAD."EXTENSION)))
	//if ((handle = open(GREXT"HEAD."EXTENSION,
	//	 O_RDONLY | O_BINARY, /*S_IREAD*/S_IRUSR)) == -1)
		Quit ("Can't open "GREXT"HEAD."EXTENSION"!");

	BE_Cross_readInt32LE(handle, grstarts, (NUMCHUNKS+1)*4);
	//CA_FarRead(handle, (memptr)grstarts, (NUMCHUNKS+1)*4);

	BE_Cross_close(handle);


#endif

//
// Open the graphics file, leaving it open until the game is finished
//
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	grhandle = BE_Cross_open_for_reading(GREXT"GRAPH."EXTENSION);
	//grhandle = open(GREXT"GRAPH."EXTENSION, O_RDONLY | O_BINARY);
	if (!BE_Cross_IsFileValid(grhandle))
	//if (grhandle == -1)
		Quit ("Cannot open "GREXT"GRAPH."EXTENSION"!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	grhandle = BE_Cross_open_for_reading("KDREAMS.EGA");
	//grhandle = open("KDREAMS.EGA", O_RDONLY | O_BINARY);
	if (!BE_Cross_IsFileValid(grhandle))
 	//if (grhandle == -1)
		Quit ("Cannot open KDREAMS.EGA!");
#endif


//
// load the pic and sprite headers into the arrays in the data segment
//
#if NUMPICS>0
	MM_GetPtr((memptr *)&pictable,NUMPICS*sizeof(pictabletype));
	CAL_GetGrChunkLength(STRUCTPIC);		// position file pointer
	MM_GetPtr(&compseg,chunkcomplen);
	CA_FarRead (grhandle,(id0_byte_t *)compseg,chunkcomplen);
	CAL_HuffExpand ((id0_byte_t *)compseg, (id0_byte_t id0_huge *)pictable,NUMPICS*sizeof(pictabletype),grhuffman);
	MM_FreePtr(&compseg);
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	for (int i = 0; i < NUMPICS; ++i)
	{
		pictable[i].width = BE_Cross_Swap16LE(pictable[i].width);
		pictable[i].height = BE_Cross_Swap16LE(pictable[i].height);
	}
#endif
#endif

#if NUMPICM>0
	MM_GetPtr((memptr *)&picmtable,NUMPICM*sizeof(pictabletype));
	CAL_GetGrChunkLength(STRUCTPICM);		// position file pointer
	MM_GetPtr(&compseg,chunkcomplen);
	CA_FarRead (grhandle,(id0_byte_t *)compseg,chunkcomplen);
	CAL_HuffExpand ((id0_byte_t *)compseg, (id0_byte_t id0_huge *)picmtable,NUMPICS*sizeof(pictabletype),grhuffman);
	MM_FreePtr(&compseg);
	// REFKEEN - Big Endian support (including above vanilla bug with NUMPICS?)
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	for (int i = 0; i < NUMPICS; ++i)
	{
		picmtable[i].width = BE_Cross_Swap16LE(picmtable[i].width);
		picmtable[i].height = BE_Cross_Swap16LE(picmtable[i].height);
	}
#endif
#endif

#if NUMSPRITES>0
	MM_GetPtr((memptr *)&spritetable,NUMSPRITES*sizeof(spritetabletype));
	CAL_GetGrChunkLength(STRUCTSPRITE);	// position file pointer
	MM_GetPtr(&compseg,chunkcomplen);
	CA_FarRead (grhandle,(id0_byte_t *)compseg,chunkcomplen);
	CAL_HuffExpand ((id0_byte_t *)compseg, (id0_byte_t id0_huge *)spritetable,NUMSPRITES*sizeof(spritetabletype),grhuffman);
	MM_FreePtr(&compseg);
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	for (int i = 0; i < NUMSPRITES; ++i)
	{
		spritetable[i].width = BE_Cross_Swap16LE(spritetable[i].width);
		spritetable[i].height = BE_Cross_Swap16LE(spritetable[i].height);
		spritetable[i].orgx = BE_Cross_Swap16LE(spritetable[i].orgx);
		spritetable[i].orgy = BE_Cross_Swap16LE(spritetable[i].orgy);
		spritetable[i].xl = BE_Cross_Swap16LE(spritetable[i].xl);
		spritetable[i].yl = BE_Cross_Swap16LE(spritetable[i].yl);
		spritetable[i].xh = BE_Cross_Swap16LE(spritetable[i].xh);
		spritetable[i].yh = BE_Cross_Swap16LE(spritetable[i].yh);
		spritetable[i].shifts = BE_Cross_Swap16LE(spritetable[i].shifts);
	}
#endif
#endif

}

//==========================================================================


/*
======================
=
= CAL_SetupMapFile
=
======================
*/

void CAL_SetupMapFile (void)
{
	// REFKEEN - Shut up compiler warnings
#ifndef MAPHEADERLINKED
	BE_FILE_T handle;
	id0_long_t length;
#endif

//
// load maphead.ext (offsets and tileinfo for map file)
//
#ifndef MAPHEADERLINKED
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading("MAPHEAD."EXTENSION)))
	//if ((handle = open("MAPHEAD."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open MAPHEAD."EXTENSION"!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading("KDREAMS.MAP")))
	//if ((handle = open("KDREAMS.MAP",
 	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open KDREAMS.MAP!");
#endif
	length = BE_Cross_FileLengthFromHandle(handle);
	MM_GetPtr ((memptr *)&tinf,length);
	CA_FarRead(handle, tinf, length);
	BE_Cross_close(handle);
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	mapfiletype id0_seg *tinfasmapfile = (mapfiletype id0_seg *)tinf;
	tinfasmapfile->RLEWtag = BE_Cross_Swap16LE(tinfasmapfile->RLEWtag);
	for (int i = 0; i < sizeof(tinfasmapfile->headeroffsets)/sizeof(*(tinfasmapfile->headeroffsets)); ++i)
	{
		tinfasmapfile->headeroffsets[i] = BE_Cross_Swap32LE(tinfasmapfile->headeroffsets[i]);
	}
#endif

#else

	maphuffman = (huffnode *)mapdict;
	CAL_OptimizeNodes (maphuffman);
	tinf = maphead;

#endif

//
// open the data file
//
#ifdef MAPHEADERLINKED
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	if (!BE_Cross_IsFileValid(maphandle = BE_Cross_open_for_reading("GAMEMAPS."EXTENSION)))
	//if ((maphandle = open("GAMEMAPS."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open GAMEMAPS."EXTENSION"!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	if (!BE_Cross_IsFileValid(maphandle = BE_Cross_open_for_reading("KDREAMS.MAP")))
	//if ((maphandle = open("KDREAMS.MAP",
 	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open KDREAMS.MAP!");
#endif // VERSION
#else
	if (!BE_Cross_IsFileValid(maphandle = BE_Cross_open_for_reading("MAPTEMP."EXTENSION)))
	//if ((maphandle = open("MAPTEMP."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open MAPTEMP."EXTENSION"!");
#endif
}

//==========================================================================


/*
======================
=
= CAL_SetupAudioFile
=
======================
*/

void CAL_SetupAudioFile (void)
{
	// REFKEEN - Shut up compiler warnings
#ifndef MAPHEADERLINKED
	BE_FILE_T handle;
	id0_long_t length;
#endif

//
// load maphead.ext (offsets and tileinfo for map file)
//
#ifndef AUDIOHEADERLINKED
	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading("AUDIOHED."EXTENSION)))
	//if ((handle = open("AUDIOHED."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open AUDIOHED."EXTENSION"!");
	length = BE_Cross_FileLengthFromHandle(handle);
	MM_GetPtr ((memptr *)&audiostarts,length);
	BE_Cross_readInt32LE(handle, audiostarts, length);
	//CA_FarRead(handle, (id0_byte_t id0_far *)audiostarts, length);
	BE_Cross_close(handle);
#else
	audiohuffman = (huffnode *)audiodict;
	CAL_OptimizeNodes (audiohuffman);
	audiostarts = (id0_long_t *)audiohead;
#endif

//
// open the data file
//
#ifndef AUDIOHEADERLINKED
	if (!BE_Cross_IsFileValid(audiohandle = BE_Cross_open_for_reading("AUDIOT."EXTENSION)))
	//if ((audiohandle = open("AUDIOT."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open AUDIOT."EXTENSION"!");
#else
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	if (!BE_Cross_IsFileValid(audiohandle = BE_Cross_open_for_reading("AUDIO."EXTENSION)))
	//if ((audiohandle = open("AUDIO."EXTENSION,
	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open AUDIO."EXTENSION"!");
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	if (!BE_Cross_IsFileValid(audiohandle = BE_Cross_open_for_reading("KDREAMS.AUD")))
	//if ((audiohandle = open("KDREAMS.AUD",
 	//	 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit ("Can't open KDREAMS.AUD!");
#endif // VERSION
#endif
}

//==========================================================================


/*
======================
=
= CA_Startup
=
= Open all files and load in headers
=
======================
*/

void CA_Startup (void)
{
#ifdef PROFILE
	unlink ("PROFILE.TXT");
	profilehandle = open("PROFILE.TXT", O_CREAT | O_WRONLY | O_TEXT);
#endif

	CAL_SetupMapFile ();
	CAL_SetupGrFile ();
	CAL_SetupAudioFile ();

	mapon = -1;
	ca_levelbit = 1;
	ca_levelnum = 0;
}

//==========================================================================


/*
======================
=
= CA_Shutdown
=
= Closes all files
=
======================
*/

void CA_Shutdown (void)
{
#ifdef PROFILE
	BE_Cross_close (profilehandle);
#endif

	BE_Cross_close (maphandle);
	BE_Cross_close (grhandle);
}

//===========================================================================

/*
======================
=
= CA_CacheAudioChunk
=
======================
*/

void CA_CacheAudioChunk (id0_int_t chunk)
{
	id0_long_t	pos,compressed,expanded;
	memptr	bigbufferseg;
	id0_byte_t	id0_far *source;

	if (audiosegs[chunk])
	{
		MM_SetPurge ((memptr *)&audiosegs[chunk],0);
		return;							// allready in memory
	}

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = audiostarts[chunk];
	compressed = audiostarts[chunk+1]-pos;

	BE_Cross_seek(audiohandle,pos,SEEK_SET);

#ifndef AUDIOHEADERLINKED

	MM_GetPtr ((memptr *)&audiosegs[chunk],compressed);
	CA_FarRead(audiohandle,audiosegs[chunk],compressed);

#else

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(audiohandle,(id0_byte_t *)bufferseg,compressed);
		source = (id0_byte_t *)bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		CA_FarRead(audiohandle,(id0_byte_t *)bigbufferseg,compressed);
		source = (id0_byte_t *)bigbufferseg;
	}

	// REFKEEN - Big Endian support
	expanded = BE_Cross_Swap32LE(*(id0_long_t id0_far *)source);
	//expanded = *(id0_long_t id0_far *)source;
	source += 4;			// skip over length
	MM_GetPtr ((memptr *)&audiosegs[chunk],expanded);
	CAL_HuffExpand (source,audiosegs[chunk],expanded,audiohuffman);

	if (compressed>BUFFERSIZE)
		MM_FreePtr(&bigbufferseg);
#endif
	// REFKEEN - Big Endian support, possibly the most complicated case,
	// since we need to know the exact type of each chunk.
	//
	// FIXME (consider): With matching proper data and definitions, this
	// should be OK, but otherwise one could consider delaying the swaps
	// to runtime (although it's a stretch).

	// Sanity check before doing the swaps on BE
#if !((STARTPCSOUNDS <= STARTADLIBSOUNDS) && (STARTADLIBSOUNDS <= STARTDIGISOUNDS) && (STARTDIGISOUNDS <= STARTMUSIC))
#error "ID_CA.C sanity check for Big-Endian byte swaps (audio chunks) has failed!"
#endif

#ifdef REFKEEN_ARCH_BIG_ENDIAN
	if (chunk < STARTMUSIC) // Sound effects
	{
		SoundCommon *sndCommonPtr = (SoundCommon *)audiosegs[chunk];
		sndCommonPtr->length = BE_Cross_Swap32LE(sndCommonPtr->length);
		sndCommonPtr->priority = BE_Cross_Swap16LE(sndCommonPtr->priority);
		if ((chunk >= STARTDIGISOUNDS) /*&& (chunk < STARTMUSIC)*/) // Digitized sounds
		{
			SampledSound *sampledSndPtr = (SampledSound *)audiosegs[chunk];
			sampledSndPtr->hertz = BE_Cross_Swap16LE(sampledSndPtr->hertz);
		}
	}
	else // Music chunk
	{
		// NOTE: Doing nothing since we don't even know how was
		// this used; Original code may be a bit incomplete anyway.
	}
#endif
}

//===========================================================================

/*
======================
=
= CA_LoadAllSounds
=
= Purges all sounds, then loads all new ones (mode switch)
=
======================
*/

void CA_LoadAllSounds (void)
{
	id0_unsigned_t	start,i;

	switch (oldsoundmode)
	{
	case sdm_Off:
		goto cachein;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	case sdm_SoundBlaster:
	case sdm_SoundSource:
		start = STARTDIGISOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		if (audiosegs[start])
			MM_SetPurge((memptr *)&audiosegs[start],3);		// make purgable

cachein:

	switch (SoundMode)
	{
	case sdm_Off:
		return;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	case sdm_SoundBlaster:
	case sdm_SoundSource:
		start = STARTDIGISOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		CA_CacheAudioChunk (start);

	oldsoundmode = SoundMode;
}

//===========================================================================

#if GRMODE == EGAGR

/*
======================
=
= CAL_ShiftSprite
=
= Make a shifted (one byte wider) copy of a sprite into another area
=
======================
*/

//id0_unsigned_t	static	sheight,swidth;

//void CAL_ShiftSprite (id0_unsigned_t segment,id0_unsigned_t source,id0_unsigned_t dest,
	//id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift)
void CAL_ShiftSprite (id0_byte_t *source, id0_byte_t *dest,
	id0_unsigned_t width, id0_unsigned_t height, id0_unsigned_t pixshift)
{
	// Ported from ASM, now without segment/offset separation

	id0_byte_t *srcptr = source, *destptr = dest;
	id0_unsigned_t *currshifttable = shifttabletable[pixshift];
	id0_unsigned_t val;

//
// table shift the mask
//

	for (id0_unsigned_t i = height; i; --i)
	{
		/** mask row **/
		*destptr = 255; // 0xff first byte
		for (id0_unsigned_t j = width; j; --j)
		{
			/** mask byte **/
			val = currshifttable[(*(srcptr++)) ^ 0xFF]; // take shift into two bytes
			val ^= 0xFFFF;
			*(destptr++) &= (val & 0xFF); // and with first byte
			*destptr = (val >> 8); // replace next byte
		}
		++destptr; // the last shifted byte has 1s in it
	}

//
// table shift the data
//

	for (id0_unsigned_t i = height*4; i; --i) // four planes of data
	{
		*destptr = 0; // 0 first byte
		for (id0_unsigned_t j = width; j; --j)
		{
			val = currshifttable[*(srcptr++)]; // take shift into two bytes
			*(destptr++) |= (val & 0xFF); // or with first byte
			*destptr = (val >> 8); // replace next byte;
		}
		++destptr; // the last shifted byte has 0s in it
	}
}

#endif

//===========================================================================

/*
======================
=
= CAL_CacheSprite
=
= Generate shifts and set up sprite structure for a given sprite
=
======================
*/

void CAL_CacheSprite (id0_int_t chunk, id0_char_t id0_far *compressed)
{
	//id0_int_t i;
	//id0_unsigned_t shiftstarts[5];
	id0_unsigned_t smallplane/*,bigplane,expanded*/;
	spritetabletype id0_far *spr;
	spritetype id0_seg *dest;

#if GRMODE == CGAGR
//
// CGA has no pel panning, so shifts are never needed
//
	spr = &spritetable[chunk-STARTSPRITES];
	smallplane = spr->width*spr->height;
	MM_GetPtr (&grsegs[chunk],smallplane*2+MAXSHIFTS*6);
	dest = (spritetype id0_seg *)grsegs[chunk];
	dest->sourceoffset[0] = MAXSHIFTS*6;	// start data after 3 unsigned tables
	dest->planesize[0] = smallplane;
	dest->width[0] = spr->width;

//
// expand the unshifted shape
//
	CAL_HuffExpand ((id0_byte_t *)compressed, &dest->data[0],smallplane*2,grhuffman);

#endif


#if GRMODE == EGAGR
	id0_int_t i;
	id0_unsigned_t shiftstarts[5];
	id0_unsigned_t bigplane,expanded;

//
// calculate sizes
//
	spr = &spritetable[chunk-STARTSPRITES];
	smallplane = spr->width*spr->height;
	bigplane = (spr->width+1)*spr->height;

	shiftstarts[0] = MAXSHIFTS*6;	// start data after 3 unsigned tables
	shiftstarts[1] = shiftstarts[0] + smallplane*5;	// 5 planes in a sprite
	shiftstarts[2] = shiftstarts[1] + bigplane*5;
	shiftstarts[3] = shiftstarts[2] + bigplane*5;
	shiftstarts[4] = shiftstarts[3] + bigplane*5;	// nothing ever put here

	expanded = shiftstarts[spr->shifts];
	MM_GetPtr (&grsegs[chunk],expanded);
	dest = (spritetype id0_seg *)grsegs[chunk];

//
// expand the unshifted shape
//
	CAL_HuffExpand ((id0_byte_t *)compressed, &dest->data[0],smallplane*5,grhuffman);

//
// make the shifts!
//
	switch (spr->shifts)
	{
	case	1:
		for (i=0;i<4;i++)
		{
			dest->sourceoffset[i] = shiftstarts[0];
			dest->planesize[i] = smallplane;
			dest->width[i] = spr->width;
		}
		break;

	case	2:
		for (i=0;i<2;i++)
		{
			dest->sourceoffset[i] = shiftstarts[0];
			dest->planesize[i] = smallplane;
			dest->width[i] = spr->width;
		}
		for (i=2;i<4;i++)
		{
			dest->sourceoffset[i] = shiftstarts[1];
			dest->planesize[i] = bigplane;
			dest->width[i] = spr->width+1;
		}
		CAL_ShiftSprite ((id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[0],
			(id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[2],spr->width,spr->height,4);
		//CAL_ShiftSprite ((id0_unsigned_t)grsegs[chunk],dest->sourceoffset[0],
		//	dest->sourceoffset[2],spr->width,spr->height,4);
		break;

	case	4:
		dest->sourceoffset[0] = shiftstarts[0];
		dest->planesize[0] = smallplane;
		dest->width[0] = spr->width;

		dest->sourceoffset[1] = shiftstarts[1];
		dest->planesize[1] = bigplane;
		dest->width[1] = spr->width+1;
		CAL_ShiftSprite ((id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[0],
			(id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[1],spr->width,spr->height,2);
		//CAL_ShiftSprite ((id0_unsigned_t)grsegs[chunk],dest->sourceoffset[0],
		//	dest->sourceoffset[1],spr->width,spr->height,2);

		dest->sourceoffset[2] = shiftstarts[2];
		dest->planesize[2] = bigplane;
		dest->width[2] = spr->width+1;
		CAL_ShiftSprite ((id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[0],
			(id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[2],spr->width,spr->height,4);
		//CAL_ShiftSprite ((id0_unsigned_t)grsegs[chunk],dest->sourceoffset[0],
		//	dest->sourceoffset[2],spr->width,spr->height,4);

		dest->sourceoffset[3] = shiftstarts[3];
		dest->planesize[3] = bigplane;
		dest->width[3] = spr->width+1;
		CAL_ShiftSprite ((id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[0],
			(id0_byte_t *)(grsegs[chunk])+dest->sourceoffset[3],spr->width,spr->height,6);
		//CAL_ShiftSprite ((id0_unsigned_t)grsegs[chunk],dest->sourceoffset[0],
		//	dest->sourceoffset[3],spr->width,spr->height,6);

		break;

	default:
		Quit ("CAL_CacheSprite: Bad shifts number!");
	}

#endif
}

//===========================================================================


/*
======================
=
= CAL_ExpandGrChunk
=
= Does whatever is needed with a pointer to a compressed chunk
=
======================
*/

void CAL_ExpandGrChunk (id0_int_t chunk, id0_byte_t id0_far *source)
{
	id0_long_t	/*pos,compressed,*/expanded;
	//id0_int_t		next;
	//spritetabletype	*spr;


	if (chunk>=STARTTILE8)
	{
	//
	// expanded sizes of tile8/16/32 are implicit
	//

#if GRMODE == EGAGR
#define BLOCK		32
#define MASKBLOCK	40
#endif

#if GRMODE == CGAGR
#define BLOCK		16
#define MASKBLOCK	32
#endif

		if (chunk<STARTTILE8M)			// tile 8s are all in one chunk!
			expanded = BLOCK*NUMTILE8;
		else if (chunk<STARTTILE16)
			expanded = MASKBLOCK*NUMTILE8M;
		else if (chunk<STARTTILE16M)	// all other tiles are one/chunk
			expanded = BLOCK*4;
		else if (chunk<STARTTILE32)
			expanded = MASKBLOCK*4;
		else if (chunk<STARTTILE32M)
			expanded = BLOCK*16;
		else
			expanded = MASKBLOCK*16;
	}
	else
	{
	//
	// everything else has an explicit size longword
	//
		// REFKEEN - Big Endian support
		expanded = BE_Cross_Swap32LE(*(id0_long_t id0_far *)source);
		//expanded = *(id0_long_t id0_far *)source;
		source += 4;			// skip over length
	}

//
// allocate final space, decompress it, and free bigbuffer
// Sprites need to have shifts made and various other junk
//
	if (chunk>=STARTSPRITES && chunk< STARTTILE8)
		CAL_CacheSprite(chunk,(id0_char_t *)source);
	else
	{
		MM_GetPtr (&grsegs[chunk],expanded);
		CAL_HuffExpand (source,(id0_byte_t *)grsegs[chunk],expanded,grhuffman);
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		if (((chunk >= STARTFONT) && (chunk < STARTFONT+NUMFONT)) || ((chunk >= STARTFONTM) && (chunk < STARTFONTM+NUMFONTM)))
		{
			fontstruct *font = (fontstruct *)(grsegs[chunk]);
			font->height = BE_Cross_Swap16LE(font->height);
			for (int i = 0; i < (int)(sizeof(font->location)/sizeof(*(font->location))); ++i)
			{
				font->location[i] = BE_Cross_Swap16LE(font->location[i]);
			}
		}
#endif
	}
}


/*
======================
=
= CAL_ReadGrChunk
=
= Gets a chunk off disk, optimizing reads to general buffer
=
======================
*/

void CAL_ReadGrChunk (id0_int_t chunk)
{
	id0_long_t	pos,compressed/*,expanded*/;
	memptr	bigbufferseg;
	id0_byte_t	id0_far *source;
	id0_int_t		next;
	//spritetabletype	*spr;

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = grstarts[chunk];
	if (pos<0)							// $FFFFFFFF start is a sparse tile
	  return;

	next = chunk +1;
	while (grstarts[next] == -1)		// skip past any sparse tiles
		next++;

	compressed = grstarts[next]-pos;

	BE_Cross_seek(grhandle,pos,SEEK_SET);

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(grhandle,(id0_byte_t *)bufferseg,compressed);
		source = (id0_byte_t *)bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		CA_FarRead(grhandle,(id0_byte_t *)bigbufferseg,compressed);
		source = (id0_byte_t *)bigbufferseg;
	}

	CAL_ExpandGrChunk (chunk,source);

	if (compressed>BUFFERSIZE)
		MM_FreePtr(&bigbufferseg);
}


/*
======================
=
= CA_CacheGrChunk
=
= Makes sure a given chunk is in memory, loadiing it if needed
=
======================
*/

void CA_CacheGrChunk (id0_int_t chunk)
{
	id0_long_t	pos,compressed/*,expanded*/;
	memptr	bigbufferseg;
	id0_byte_t	id0_far *source;
	id0_int_t		next;

	grneeded[chunk] |= ca_levelbit;		// make sure it doesn't get removed
	if (grsegs[chunk])
	  return;							// allready in memory

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = grstarts[chunk];
	if (pos<0)							// $FFFFFFFF start is a sparse tile
	  return;

	next = chunk +1;
	while (grstarts[next] == -1)		// skip past any sparse tiles
		next++;

	compressed = grstarts[next]-pos;

	BE_Cross_seek(grhandle,pos,SEEK_SET);

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(grhandle,(id0_byte_t *)bufferseg,compressed);
		source = (id0_byte_t *)bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		CA_FarRead(grhandle,(id0_byte_t *)bigbufferseg,compressed);
		source = (id0_byte_t *)bigbufferseg;
	}

	CAL_ExpandGrChunk (chunk,source);

	if (compressed>BUFFERSIZE)
		MM_FreePtr(&bigbufferseg);
}



//==========================================================================

/*
======================
=
= CA_CacheMap
=
======================
*/

void CA_CacheMap (id0_int_t mapnum)
{
	id0_long_t	pos,compressed,expanded;
	id0_int_t		plane;
	memptr	*dest,bigbufferseg,buffer2seg;
	id0_unsigned_t	size;
	id0_unsigned_t	id0_far	*source;


//
// free up memory from last map
//
	if (mapon>-1 && mapheaderseg[mapon])
		MM_SetPurge ((memptr *)&mapheaderseg[mapon],3);
	for (plane=0;plane<3;plane++)
		if (mapsegs[plane])
			MM_FreePtr ((memptr *)&mapsegs[plane]);

	mapon = mapnum;


//
// load map header
// The header will be cached if it is still around
//
	if (!mapheaderseg[mapnum])
	{
		pos = ((mapfiletype	id0_seg *)tinf)->headeroffsets[mapnum];
		if (pos<0)						// $FFFFFFFF start is a sparse map
		  Quit ("CA_CacheMap: Tried to load a non existant map!");

		MM_GetPtr((memptr *)&mapheaderseg[mapnum],sizeof(maptype));
		BE_Cross_seek(maphandle,pos,SEEK_SET);

#ifdef MAPHEADERLINKED
		// Preprocessor can't check if BUFFERSIZE < sizeof(maptype), so apply this trick found online...
		((void)sizeof(char[1 - 2*!!(BUFFERSIZE < sizeof(maptype))]));
#if 0
#if BUFFERSIZE < sizeof(maptype)
The general buffer size is too small!
#endif
#endif
		//
		// load in, then unhuffman to the destination
		//
		CA_FarRead (maphandle,(id0_byte_t *)bufferseg,((mapfiletype	id0_seg *)tinf)->headersize[mapnum]);
		CAL_HuffExpand ((id0_byte_t id0_huge *)bufferseg,
			(id0_byte_t id0_huge *)mapheaderseg[mapnum],sizeof(maptype),maphuffman);
#else
		CA_FarRead (maphandle,(id0_byte_t *)((memptr)mapheaderseg[mapnum]),sizeof(maptype));
		//CA_FarRead (maphandle,(memptr)mapheaderseg[mapnum],sizeof(maptype));
#endif
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		mapheaderseg[mapnum]->planestart[0] = BE_Cross_Swap32LE(mapheaderseg[mapnum]->planestart[0]);
		mapheaderseg[mapnum]->planestart[1] = BE_Cross_Swap32LE(mapheaderseg[mapnum]->planestart[1]);
		mapheaderseg[mapnum]->planestart[2] = BE_Cross_Swap32LE(mapheaderseg[mapnum]->planestart[2]);
		mapheaderseg[mapnum]->planelength[0] = BE_Cross_Swap16LE(mapheaderseg[mapnum]->planelength[0]);
		mapheaderseg[mapnum]->planelength[1] = BE_Cross_Swap16LE(mapheaderseg[mapnum]->planelength[1]);
		mapheaderseg[mapnum]->planelength[2] = BE_Cross_Swap16LE(mapheaderseg[mapnum]->planelength[2]);
		mapheaderseg[mapnum]->width = BE_Cross_Swap16LE(mapheaderseg[mapnum]->width);
		mapheaderseg[mapnum]->height = BE_Cross_Swap16LE(mapheaderseg[mapnum]->height);
#endif
	}
	else
		MM_SetPurge ((memptr *)&mapheaderseg[mapnum],0);

//
// load the planes in
// If a plane's pointer still exists it will be overwritten (levels are
// allways reloaded, never cached)
//

	size = mapheaderseg[mapnum]->width * mapheaderseg[mapnum]->height * 2;

	for (plane = 0; plane<3; plane++)
	{
		dest = (memptr *)&mapsegs[plane];
		MM_GetPtr(dest,size);

		pos = mapheaderseg[mapnum]->planestart[plane];
		compressed = mapheaderseg[mapnum]->planelength[plane];
		BE_Cross_seek(maphandle,pos,SEEK_SET);
		if (compressed<=BUFFERSIZE)
			source = (id0_unsigned_t *)bufferseg;
		else
		{
			MM_GetPtr(&bigbufferseg,compressed);
			source = (id0_unsigned_t *)bigbufferseg;
		}

		CA_FarRead(maphandle,(id0_byte_t id0_far *)source,compressed);
#ifdef MAPHEADERLINKED
		//
		// unhuffman, then unRLEW
		// The huffman'd chunk has a two byte expanded length first
		// The resulting RLEW chunk also does, even though it's not really
		// needed
		//
		expanded = BE_Cross_Swap16LE(*source); // REFKEEN - Big Endian support
		//expanded = *source;
		source++;
		MM_GetPtr (&buffer2seg,expanded);
		CAL_HuffExpand ((id0_byte_t id0_huge *)source,(id0_byte_t *)buffer2seg,expanded,maphuffman);
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		id0_unsigned_t id0_far * rlewunsignedptr = (id0_unsigned_t id0_far *)buffer2seg;
		for (int i = 0; i < expanded/2; ++i, ++rlewunsignedptr)
		{
			*rlewunsignedptr = BE_Cross_Swap16LE(*rlewunsignedptr);
		}
#endif
		CA_RLEWexpand (((id0_unsigned_t id0_far *)buffer2seg)+1,(id0_unsigned_t *)(*dest),size,
		((mapfiletype id0_seg *)tinf)->RLEWtag);
		MM_FreePtr (&buffer2seg);

#else
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		id0_unsigned_t id0_far * rlewunsignedptr = source;
		for (int i = 0; i < compressed/2; ++i, ++rlewunsignedptr)
		{
			*rlewunsignedptr = BE_Cross_Swap16LE(*rlewunsignedptr);
		}
#endif
		//
		// unRLEW, skipping expanded length
		//
		CA_RLEWexpand (source+1, *dest,size,
		((mapfiletype id0_seg *)tinf)->RLEWtag);
#endif

		if (compressed>BUFFERSIZE)
			MM_FreePtr(&bigbufferseg);
	}
}

//===========================================================================

/*
======================
=
= CA_UpLevel
=
= Goes up a bit level in the needed lists and clears it out.
= Everything is made purgable
=
======================
*/

void CA_UpLevel (void)
{
	//id0_int_t i;

	if (ca_levelnum==7)
		Quit ("CA_UpLevel: Up past level 7!");

//	for (i=0;i<NUMCHUNKS;i++)
//			if (grsegs[i])
//				MM_SetPurge(&grsegs[i],3);

	ca_levelbit<<=1;
	ca_levelnum++;
}

//===========================================================================

/*
======================
=
= CA_DownLevel
=
= Goes down a bit level in the needed lists and recaches
= everything from the lower level
=
======================
*/

void CA_DownLevel (void)
{
	if (!ca_levelnum)
		Quit ("CA_DownLevel: Down past level 0!");
	ca_levelbit>>=1;
	ca_levelnum--;
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	CA_CacheMarks(titleptr[ca_levelnum]);
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
	CA_CacheMarks(titleptr[ca_levelnum], 1);
#endif
}

//===========================================================================

/*
======================
=
= CA_ClearMarks
=
= Clears out all the marks at the current level
=
======================
*/

void CA_ClearMarks (void)
{
	id0_int_t i;

	for (i=0;i<NUMCHUNKS;i++)
		grneeded[i]&=~ca_levelbit;
}


//===========================================================================

/*
======================
=
= CA_ClearAllMarks
=
= Clears out all the marks on all the levels
=
======================
*/

void CA_ClearAllMarks (void)
{
	memset (grneeded,0,sizeof(grneeded));
	ca_levelbit = 1;
	ca_levelnum = 0;
}


//===========================================================================


/*
======================
=
= CA_CacheMarks
=
======================
*/

#define NUMBARS	(17l*8)
#define BARSTEP	8
#define MAXEMPTYREAD	1024

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
void CA_CacheMarks (const id0_char_t *title)
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
void CA_CacheMarks (const id0_char_t *title, id0_boolean_t cachedownlevel)
#endif
{
	id0_boolean_t dialog;
	id0_int_t 	i,next,homex,homey,x/*,y*/,thx,thy,numcache,lastx/*,xl*/,xh;
	id0_long_t	barx,barstep;
	id0_long_t	pos,endpos,nextpos,nextendpos,compressed;
	id0_long_t	bufferstart,bufferend;	// file position of general buffer
	id0_byte_t	id0_far *source;
	memptr	bigbufferseg;

	//
	// save title so cache down level can redraw it
	//
	titleptr[ca_levelnum] = title;

	dialog = (title!=NULL);
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL

	if (cachedownlevel)
		dialog = false;

#endif
	if (dialog)
	{
	//
	// draw dialog window (masked tiles 12 - 20 are window borders)
	//
		US_CenterWindow (20,8);
		homex = PrintX;
		homey = PrintY;

		US_CPrint ("Loading");
		fontcolor = F_SECONDCOLOR;
		US_CPrint (title);
		fontcolor = F_BLACK;
		VW_UpdateScreen();
#ifdef PROFILE
		write(profilehandle,title,strlen(title));
		write(profilehandle,"\n",1);
#endif

	}

	numcache = 0;
//
// go through and make everything not needed purgable
//
	for (i=0;i<NUMCHUNKS;i++)
		if (grneeded[i]&ca_levelbit)
		{
			if (grsegs[i])					// its allready in memory, make
				MM_SetPurge(&grsegs[i],0);	// sure it stays there!
			else
				numcache++;
		}
		else
		{
			if (grsegs[i])					// not needed, so make it purgeable
				MM_SetPurge(&grsegs[i],3);
		}

	if (!numcache)			// nothing to cache!
		return;

	if (dialog)
	{
	//
	// draw thermometer bar
	//
		thx = homex + 8;
		thy = homey + 32;
		VWB_DrawTile8(thx,thy,11);
		VWB_DrawTile8(thx,thy+8,14);
		VWB_DrawTile8(thx,thy+16,17);
		VWB_DrawTile8(thx+17*8,thy,13);
		VWB_DrawTile8(thx+17*8,thy+8,16);
		VWB_DrawTile8(thx+17*8,thy+16,19);
		for (x=thx+8;x<thx+17*8;x+=8)
		{
			VWB_DrawTile8(x,thy,12);
			VWB_DrawTile8(x,thy+8,15);
			VWB_DrawTile8(x,thy+16,18);
		}

		thx += 4;		// first line location
		thy += 5;
		barx = (id0_long_t)thx<<16;
		lastx = thx;
		VW_UpdateScreen();
	}

//
// go through and load in anything still needed
//
	barstep = (NUMBARS<<16)/numcache;
	bufferstart = bufferend = 0;		// nothing good in buffer now

	for (i=0;i<NUMCHUNKS;i++)
		if ( (grneeded[i]&ca_levelbit) && !grsegs[i])
		{
//
// update thermometer
//
			if (dialog)
			{
				barx+=barstep;
				xh = barx>>16;
				if (xh - lastx > BARSTEP)
				{
					for (x=lastx;x<=xh;x++)
#if GRMODE == EGAGR
						VWB_Vlin (thy,thy+13,x,14);
#endif
#if GRMODE == CGAGR
						VWB_Vlin (thy,thy+13,x,SECONDCOLOR);
#endif
					lastx = xh;
					VW_UpdateScreen();
					// (REFKEEN) HACK: Ensure this is at least seen
					BE_ST_ShortSleep();
				}

			}
			pos = grstarts[i];
			if (pos<0)
				continue;

			next = i +1;
			while (grstarts[next] == -1)		// skip past any sparse tiles
				next++;

			compressed = grstarts[next]-pos;
			endpos = pos+compressed;

			if (compressed<=BUFFERSIZE)
			{
				if (bufferstart<=pos
				&& bufferend>= endpos)
				{
				// data is allready in buffer
					source = (id0_byte_t id0_seg *)bufferseg+(pos-bufferstart);
				}
				else
				{
				// load buffer with a new block from disk
				// try to get as many of the needed blocks in as possible
					while ( next < NUMCHUNKS )
					{
						while (next < NUMCHUNKS &&
						!(grneeded[next]&ca_levelbit && !grsegs[next]))
							next++;
						if (next == NUMCHUNKS)
							continue;

						nextpos = grstarts[next];
						while (grstarts[++next] == -1)	// skip past any sparse tiles
							;
						nextendpos = grstarts[next];
						if (nextpos - endpos <= MAXEMPTYREAD
						&& nextendpos-pos <= BUFFERSIZE)
							endpos = nextendpos;
						else
							next = NUMCHUNKS;			// read pos to posend
					}

					BE_Cross_seek(grhandle,pos,SEEK_SET);
					CA_FarRead(grhandle,(id0_byte_t *)bufferseg,endpos-pos);
					bufferstart = pos;
					bufferend = endpos;
					source = (id0_byte_t *)bufferseg;
				}
			}
			else
			{
			// big chunk, allocate temporary buffer
				MM_GetPtr(&bigbufferseg,compressed);
				BE_Cross_seek(grhandle,pos,SEEK_SET);
				CA_FarRead(grhandle,(id0_byte_t *)bigbufferseg,compressed);
				source = (id0_byte_t *)bigbufferseg;
			}

			CAL_ExpandGrChunk (i,source);

			if (compressed>BUFFERSIZE)
				MM_FreePtr(&bigbufferseg);

		}

//
// finish up any thermometer remnants
//
		if (dialog)
		{
			xh = thx + NUMBARS;
			for (x=lastx;x<=xh;x++)
#if GRMODE == EGAGR
				VWB_Vlin (thy,thy+13,x,14);
#endif
#if GRMODE == CGAGR
				VWB_Vlin (thy,thy+13,x,SECONDCOLOR);
#endif
			VW_UpdateScreen();
			// (REFKEEN) And again...
			BE_ST_ShortSleep();
		}
}

// (REFKEEN) Used for patching version-specific stuff
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
id0_long_t	*CGAhead;
id0_byte_t	*CGAdict;
#else
id0_long_t	*EGAhead;
id0_byte_t	*EGAdict;
#endif
id0_byte_t	*maphead;
id0_byte_t	*mapdict;
id0_byte_t	*audiohead;
id0_byte_t	*audiodict;

void RefKeen_Patch_id_ca(void)
{
	int audiodictsize, audioheadsize, GFXdictsize, GFXheadsize, mapdictsize, mapheadsize;
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	id0_byte_t **GFXdictptr = &CGAdict;
	id0_long_t **GFXheadptr = &CGAhead;
#else
	id0_byte_t **GFXdictptr = &EGAdict;
	id0_long_t **GFXheadptr = &EGAhead;
#endif
	// Just in case these may ever be reloaded
	BE_Cross_free_mem_loaded_embedded_rsrc(audiodict);
	BE_Cross_free_mem_loaded_embedded_rsrc(audiohead);
	BE_Cross_free_mem_loaded_embedded_rsrc(*GFXdictptr);
	BE_Cross_free_mem_loaded_embedded_rsrc(*GFXheadptr);
	BE_Cross_free_mem_loaded_embedded_rsrc(mapdict);
	BE_Cross_free_mem_loaded_embedded_rsrc(maphead);
	// Don't use CA_LoadFile for (sort-of) compatibility; It also doesn't work!
	if (((audiodictsize = BE_Cross_load_embedded_rsrc_to_mem("AUDIODCT."EXTENSION, (memptr *)&audiodict)) < 0) ||
	    ((audioheadsize = BE_Cross_load_embedded_rsrc_to_mem("AUDIOHHD."EXTENSION, (memptr *)&audiohead)) < 0) ||
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
	    ((GFXdictsize = BE_Cross_load_embedded_rsrc_to_mem("CGADICT."EXTENSION, (memptr *)GFXdictptr)) < 0) ||
	    ((GFXheadsize = BE_Cross_load_embedded_rsrc_to_mem("CGAHEAD."EXTENSION, (memptr *)GFXheadptr)) < 0) ||
#else
	    ((GFXdictsize = BE_Cross_load_embedded_rsrc_to_mem("EGADICT."EXTENSION, (memptr *)GFXdictptr)) < 0) ||
	    ((GFXheadsize = BE_Cross_load_embedded_rsrc_to_mem("EGAHEAD."EXTENSION, (memptr *)GFXheadptr)) < 0) ||
#endif
	    ((mapdictsize = BE_Cross_load_embedded_rsrc_to_mem("MAPDICT."EXTENSION, (memptr *)&mapdict)) < 0) ||
	    ((mapheadsize = BE_Cross_load_embedded_rsrc_to_mem("MAPHEAD."EXTENSION, (memptr *)&maphead)) < 0)
	)
		// Similarly we don't use Quit
		BE_ST_ExitWithErrorMsg("RefKeen_Patch_id_ca - Failed to load at least one file.");

#ifdef REFKEEN_ARCH_BIG_ENDIAN
	for (uint16_t *dictptr = (uint16_t *)audiodict; audiodictsize >= 2; ++dictptr, audiodictsize -= 2)
		*dictptr = BE_Cross_Swap16LE(*dictptr);
	for (uint32_t *headptr = (uint32_t *)audiohead; audioheadsize >= 4; ++headptr, audioheadsize -= 4)
		*headptr = BE_Cross_Swap32LE(*headptr);
	for (uint16_t *dictptr = (uint16_t *)(*GFXdictptr); GFXdictsize >= 2; ++dictptr, GFXdictsize -= 2)
		*dictptr = BE_Cross_Swap16LE(*dictptr);
	for (uint32_t *headptr = (uint32_t *)(*GFXheadptr); GFXheadsize >= 4; ++headptr, GFXheadsize -= 4)
		*headptr = BE_Cross_Swap32LE(*headptr);
	for (uint16_t *dictptr = (uint16_t *)mapdict; mapdictsize >= 2; ++dictptr, mapdictsize -= 2)
		*dictptr = BE_Cross_Swap16LE(*dictptr);

	mapfiletype *tinfasmapfile = (mapfiletype *)mapdict;
	tinfasmapfile->RLEWtag = BE_Cross_Swap16LE(tinfasmapfile->RLEWtag);
	for (int i = 0; i < sizeof(tinfasmapfile->headeroffsets)/sizeof(*(tinfasmapfile->headeroffsets)); ++i)
	{
		tinfasmapfile->headeroffsets[i] = BE_Cross_Swap32LE(tinfasmapfile->headeroffsets[i]);
	}
#endif
}
