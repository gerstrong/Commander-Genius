/* Catacomb Apocalypse Source Code
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

/* Modified for building LOADSCN.EXE as bundled with The Catacomb Abyss
 * Shareware v1.13
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <alloc.h>
#include <fcntl.h>
//#include <dos.h>
//#include <io.h>

#include "ext_heads.h"
#include "ext_gelib.h"
#include "ext_soft.h"
//#include "ext_lzw.h"
#include "ext_lzhuff.h"
#include "ext_jam_io.h"







//===========================================================================
//
//										SWITCHES
//
//===========================================================================

#define LZH_SUPPORT		1
#define LZW_SUPPORT		0




//=========================================================================
//
//
//								GENERAL LOAD ROUTINES
//
//
//=========================================================================



//--------------------------------------------------------------------------
// BLoad()			-- THIS HAS NOT BEEN FULLY TESTED!
//
// NOTICE : This version of BLOAD is compatable with JAMPak V3.0 and the
//				new fileformat...
//--------------------------------------------------------------------------
id0_unsigned_long_t ext_BLoad(const id0_char_t *SourceFile, memptr *DstPtr)
{
	BE_FILE_T handle;

	memptr SrcPtr = NULL;
	//id0_unsigned_long_t i, j, k, r, c;
	//id0_word_t flags;
	id0_byte_t Buffer[8];
	id0_unsigned_long_t SrcLen,DstLen;
	struct CMP1Header CompHeader;
	id0_boolean_t Compressed = false;
	id0_unsigned_t offset;


	memset((void *)&CompHeader,0,sizeof(struct CMP1Header));

	//
	// Open file to load....
	//

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(SourceFile)))
	//if ((handle = open(SourceFile, O_RDONLY|O_BINARY)) == -1)
		return(0);

	//
	// Look for JAMPAK headers
	//

	BE_Cross_readInt8LEBuffer(handle,Buffer,4);
	//read(handle,Buffer,4);

	if (!strncmp((char *)Buffer,COMP,4))
	{
		//
		// Compressed under OLD file format
		//

		Compressed = true;
		SrcLen = Verify(SourceFile);

		BE_Cross_readInt32LE(handle, &CompHeader.OrginalLen);
		//read(handle,(void *)&CompHeader.OrginalLen,4);
		CompHeader.CompType = ct_LZW;
		*DstPtr = malloc/*farmalloc*/(CompHeader.OrginalLen);
		if (!*DstPtr)
			return(0);
		offset = 8;
	}
	else
	if (!strncmp((char *)Buffer,CMP1,4))
	{
		//
		// Compressed under new file format...
		//

		Compressed = true;
		SrcLen = Verify(SourceFile);

		BE_Cross_readInt8LEBuffer(handle,(void *)&CompHeader,sizeof(struct CMP1Header));
		//read(handle,(void *)&CompHeader,sizeof(struct CMP1Header));
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		CompHeader.CompType = BE_Cross_Swap16LE(CompHeader.CompType);
		CompHeader.OrginalLen = BE_Cross_Swap32LE(CompHeader.OrginalLen);
		CompHeader.CompressLen = BE_Cross_Swap32LE(CompHeader.CompressLen);
#endif
		offset = 14;
		*DstPtr = malloc/*farmalloc*/(CompHeader.OrginalLen);
		if (!*DstPtr)
			return(0);
	}
	else
		DstLen = Verify(SourceFile);


	//
	// Load the file in memory...
	//

	if (Compressed)
	{
		DstLen = CompHeader.OrginalLen;

		// REFKEEN - Looks like this is an unsigned comparison in original EXE
		if (((id0_unsigned_long_t)BE_Mem_FarCoreLeft() < 2*SrcLen) && (CompHeader.CompType))
		{
			//if (!InitBufferedIO(handle,&lzwBIO))
			//	TrashProg("No memory for buffered I/O.");

			switch (CompHeader.CompType)
			{
				#if LZW_SUPPORT
				case ct_LZW:
					// REFKEEN - This is a BE_FILE_T (FILE*) instead of int now, but this call was buggy and unused anyway (should've been SRC_FILE)
					lzwDecompress(handle,*DstPtr,CompHeader.OrginalLen,(SRC_BFILE|DEST_MEM));
				break;
				#endif

				#if LZH_SUPPORT
				case ct_LZH:
					// REFKEEN - This is a BE_FILE_T (FILE*) instead of int now, but this call was buggy and unused anyway (should've been SRC_FILE)
					lzhDecompress(handle,*DstPtr,CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_BFILE|DEST_MEM));
				break;
				#endif

				default:
					TrashProg("BLoad() - Unrecognized/Supported compression");
				break;
			}

			//FreeBufferedIO(&lzwBIO);
		}
		else
		{
			// REFKEEN - Better close the current file handle before re-opening here
			BE_Cross_close(handle);
			//
			LoadFile(SourceFile,&SrcPtr);
			switch (CompHeader.CompType)
			{
				#if LZW_SUPPORT
				case ct_LZW:
					lzwDecompress((uint8_t *)SrcPtr + offset,*DstPtr,CompHeader.OrginalLen,(SRC_MEM|DEST_MEM));
				break;
				#endif

				#if LZH_SUPPORT
				case ct_LZH:
					lzhDecompress((uint8_t *)SrcPtr + offset,*DstPtr,CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_MEM|DEST_MEM));
				break;
				#endif

				default:
					TrashProg("BLoad() - Unrecognized/Supported compression");
				break;
			}
			free/*farfree*/(SrcPtr);
			// REFKEEN - File handle already closed
			return(DstLen);
		}
	}
	else
	{
		// REFKEEN - Again we close the current file handle first, then load and finally return DstLen without re-closing file handle
		BE_Cross_close(handle);
		LoadFile(SourceFile,DstPtr);
		return(DstLen);
	}

	BE_Cross_close(handle);
	return(DstLen);
}




////////////////////////////////////////////////////////////////////////////
//
// LoadLIBShape()
//
id0_int_t ext_LoadShape(const id0_char_t *Filename, struct Shape *SHP)
{
	#define CHUNK(Name) ( \
		(*ptr == *Name) &&			\
		(*(ptr+1) == *(Name+1)) &&	\
		(*(ptr+2) == *(Name+2)) &&	\
		(*(ptr+3) == *(Name+3)) \
	)

	id0_int_t RT_CODE;
//	struct ffblk ffblk;
	//FILE *fp;
	//id0_char_t CHUNK[5];
	id0_char_t id0_far *ptr;
	memptr IFFfile = NULL;
	id0_unsigned_long_t FileLen, size, ChunkLen;
	//id0_int_t loop;


	RT_CODE = 1;

	// Decompress to ram and return ptr to data and return len of data in
	//	passed variable...

	if (!(FileLen = ext_BLoad(Filename,&IFFfile)))
		TrashProg("Can't load Compressed Shape - Possibly corrupt file!");

	// Evaluate the file
	//
	ptr = (id0_char_t *)IFFfile;
	if (!CHUNK("FORM"))
		goto EXIT_FUNC;
	ptr += 4;

	FileLen = BE_Cross_Swap32BE(*(id0_long_t id0_far *)ptr);
	//FileLen = *(id0_long_t id0_far *)ptr;
	//SwapLong((id0_long_t id0_far *)&FileLen);
	ptr += 4;

	if (!CHUNK("ILBM"))
		goto EXIT_FUNC;
	ptr += 4;

	FileLen += 4;
	while (FileLen)
	{
		ChunkLen = BE_Cross_Swap32BE(*(id0_long_t id0_far *)(ptr+4));
		//ChunkLen = *(id0_long_t id0_far *)(ptr+4);
		//SwapLong((id0_long_t id0_far *)&ChunkLen);
		ChunkLen = (ChunkLen+1) & 0xFFFFFFFE;

		if (CHUNK("BMHD"))
		{
			ptr += 8;
			SHP->bmHdr.w = BE_Cross_Swap16BE(((struct BitMapHeader id0_far *)ptr)->w);
			SHP->bmHdr.h = BE_Cross_Swap16BE(((struct BitMapHeader id0_far *)ptr)->h);
			SHP->bmHdr.x = BE_Cross_Swap16BE(((struct BitMapHeader id0_far *)ptr)->x);
			SHP->bmHdr.y = BE_Cross_Swap16BE(((struct BitMapHeader id0_far *)ptr)->y);
#if 0
			SHP->bmHdr.w = ((struct BitMapHeader id0_far *)ptr)->w;
			SHP->bmHdr.h = ((struct BitMapHeader id0_far *)ptr)->h;
			SHP->bmHdr.x = ((struct BitMapHeader id0_far *)ptr)->x;
			SHP->bmHdr.y = ((struct BitMapHeader id0_far *)ptr)->y;
#endif
			SHP->bmHdr.d = ((struct BitMapHeader id0_far *)ptr)->d;
			SHP->bmHdr.trans = ((struct BitMapHeader id0_far *)ptr)->trans;
			SHP->bmHdr.comp = ((struct BitMapHeader id0_far *)ptr)->comp;
			SHP->bmHdr.pad = ((struct BitMapHeader id0_far *)ptr)->pad;
#if 0
			SwapWord(&SHP->bmHdr.w);
			SwapWord(&SHP->bmHdr.h);
			SwapWord(&SHP->bmHdr.x);
			SwapWord(&SHP->bmHdr.y);
#endif
			ptr += ChunkLen;
		}
		else
		if (CHUNK("BODY"))
		{
			ptr += 4;
			size = BE_Cross_Swap32BE(*((id0_long_t id0_far *)ptr));
			//size = *((id0_long_t id0_far *)ptr);
			ptr += 4;
			//SwapLong((id0_long_t id0_far *)&size);
			SHP->BPR = (SHP->bmHdr.w+7) >> 3;
			SHP->Data = malloc/*farmalloc*/(size);
			if (!SHP->Data)
				goto EXIT_FUNC;
			//movedata(FP_SEG(ptr),FP_OFF(ptr),FP_SEG(SHP->Data),FP_OFF(SHP->Data),size);
			memcpy(SHP->Data,ptr,size);
			ptr += ChunkLen;

			break;
		}
		else
			ptr += ChunkLen+8;

		FileLen -= ChunkLen+8;
	}

	RT_CODE = 0;

EXIT_FUNC:;
	if (IFFfile)
	{
//		segptr = (memptr)FP_SEG(IFFfile);
		free/*farfree*/(IFFfile);
	}

	return (RT_CODE);
}



////////////////////////////////////////////////////////////////////////////
//
// FreeShape()
//
void ext_FreeShape(struct Shape *shape)
{
	if (shape->Data)
		free/*farfree*/(shape->Data);
}

// (REFKEEN) Functionality equivalant to SwapLong, SwapWord from CATABYSS.EXE's gelib
#if 0
///////////////////////////////////////////////////////////////////////////
//
// SwapLong()
//
void SwapLong(id0_long_t id0_far *Var)
{
	*Var = ((id0_longword_t)(*Var) >> 24) |
	       (((id0_longword_t)(*Var) >> 8) & 0xFF00) |
	       (((id0_longword_t)(*Var) << 8) & 0xFF0000) |
	       ((id0_longword_t)(*Var) << 24);
}

///////////////////////////////////////////////////////////////////////////
//
// SwapWord()
//
void SwapWord(id0_unsigned_int_t id0_far *Var)
{
	*Var = ((*Var) >> 8) | ((*Var) << 8);
}
#endif
