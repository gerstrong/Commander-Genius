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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <alloc.h>
#include <fcntl.h>
//#include <dos.h>
//#include <io.h>

#include "kd_def.h"
//#include "gelib.h"
#include "soft.h"
#include "lzw.h"
#include "lzhuff.h"
#include "jam_io.h"


BufferedIO lzwBIO;




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
id0_unsigned_long_t BLoad(const id0_char_t *SourceFile, memptr *DstPtr)
{
	BE_FILE_T handle;

	memptr SrcPtr;
	//id0_unsigned_long_t i, j, k, r, c;
	//id0_word_t flags;
	id0_byte_t Buffer[8];
	id0_unsigned_long_t SrcLen,DstLen;
	struct CMP1Header CompHeader;
	id0_boolean_t Compressed = false;


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
		MM_GetPtr(DstPtr,CompHeader.OrginalLen);
		if (!*DstPtr)
			return(0);
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
		MM_GetPtr(DstPtr,CompHeader.OrginalLen);
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
		if (((id0_unsigned_long_t)MM_TotalFree() < SrcLen) && (CompHeader.CompType))
		{
			if (!InitBufferedIO(handle,&lzwBIO))
				Quit("No memory for buffered I/O.");

			switch (CompHeader.CompType)
			{
				#if LZW_SUPPORT
				case ct_LZW:
					//lzwDecompress(&lzwBIO,MK_FP(*DstPtr,0),CompHeader.OrginalLen,(SRC_BFILE|DEST_MEM));
					lzwDecompress(&lzwBIO,*DstPtr,CompHeader.OrginalLen,(SRC_BFILE|DEST_MEM));
				break;
				#endif

				#if LZH_SUPPORT
				case ct_LZH:
					//lzhDecompress(&lzwBIO,MK_FP(*DstPtr,0),CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_BFILE|DEST_MEM));
					lzhDecompress(&lzwBIO,*DstPtr,CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_BFILE|DEST_MEM));
				break;
				#endif

				default:
					Quit("BLoad() - Unrecognized/Supported compression");
				break;
			}

			FreeBufferedIO(&lzwBIO);
		}
		else
		{
			// REFKEEN - Better close the current file handle before re-opening here
			BE_Cross_close(handle);
			//
			CA_LoadFile(SourceFile,&SrcPtr);
			switch (CompHeader.CompType)
			{
				#if LZW_SUPPORT
				case ct_LZW:
					//lzwDecompress(MK_FP(SrcPtr,8),MK_FP(*DstPtr,0),CompHeader.OrginalLen,(SRC_MEM|DEST_MEM));
					lzwDecompress((id0_byte_t *)SrcPtr + 8,*DstPtr,CompHeader.OrginalLen,(SRC_MEM|DEST_MEM));
				break;
				#endif

				#if LZH_SUPPORT
				case ct_LZH:
					//lzhDecompress(MK_FP(SrcPtr,8),MK_FP(*DstPtr,0),CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_MEM|DEST_MEM));
					lzhDecompress((id0_byte_t *)SrcPtr + 8,*DstPtr,CompHeader.OrginalLen,CompHeader.CompressLen,(SRC_MEM|DEST_MEM));
				break;
				#endif

				default:
					Quit("BLoad() - Unrecognized/Supported compression");
				break;
			}
			MM_FreePtr(&SrcPtr);
			// REFKEEN - File handle already closed
			return(DstLen);
		}
	}
	else
	{
		// REFKEEN - Again we close the current file handle first, then load and finally return DstLen without re-closing file handle
		BE_Cross_close(handle);
		CA_LoadFile(SourceFile,DstPtr);
		return(DstLen);
	}

	BE_Cross_close(handle);
	return(DstLen);
}




////////////////////////////////////////////////////////////////////////////
//
// LoadLIBShape()
//
id0_int_t LoadLIBShape(const id0_char_t *SLIB_Filename, const id0_char_t *Filename,struct Shape *SHP)
{
	#define CHUNK(Name)	(*ptr == *Name) &&			\
								(*(ptr+1) == *(Name+1)) &&	\
								(*(ptr+2) == *(Name+2)) &&	\
								(*(ptr+3) == *(Name+3))


	id0_int_t RT_CODE;
	//FILE *fp;
	//id0_char_t CHUNK[5];
	id0_char_t id0_far *ptr;
	memptr IFFfile = NULL;
	id0_unsigned_long_t FileLen, size, ChunkLen;
	//id0_int_t loop;


	RT_CODE = 1;

	// Decompress to ram and return ptr to data and return len of data in
	//	passed variable...

	if (!LoadLIBFile(SLIB_Filename,Filename,&IFFfile))
		Quit("Error Loading Compressed lib shape!");

	// Evaluate the file
	//
	ptr = (id0_char_t *)IFFfile;
	//ptr = MK_FP(IFFfile,0);
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
			MM_GetPtr(&SHP->Data,size);
			if (!SHP->Data)
				goto EXIT_FUNC;
			//movedata(FP_SEG(ptr),FP_OFF(ptr),FP_SEG(SHP->Data),0,size);
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
		MM_FreePtr(&IFFfile);
	}

	return (RT_CODE);
}





//----------------------------------------------------------------------------
// LoadLIBFile() -- Copies a file from an existing archive to dos.
//
// PARAMETERS :
//
//			LibName 	- Name of lib file created with SoftLib V1.0
//
//			FileName - Name of file to load from lib file.
//
//			MemPtr   - (IF !NULL) - Pointer to memory to load into ..
//						  (IF NULL)  - Routine allocates necessary memory and
//											returns a MEM(SEG) pointer to memory allocated.
//
// RETURN :
//
//   		(IF !NULL) - A pointer to the loaded data.
//			(IF NULL)  - Error!
//
//----------------------------------------------------------------------------
memptr LoadLIBFile(const id0_char_t *LibName,const id0_char_t *FileName,memptr *MemPtr)
{
	BE_FILE_T handle;
	id0_unsigned_long_t header;
	struct ChunkHeader Header;
	id0_unsigned_long_t ChunkLen;
	id0_short_t x;
	struct FileEntryHdr FileEntry;     			// Storage for file once found
	struct FileEntryHdr FileEntryHeader;		// Header used durring searching
	struct SoftLibHdr LibraryHeader;				// Library header - Version Checking
	id0_boolean_t FileFound = false;
	id0_unsigned_long_t id_slib = ID_SLIB;
	id0_unsigned_long_t id_chunk = ID_CHUNK;


	//
	// OPEN SOFTLIB FILE
	//

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(LibName)))
	//if ((handle = open(LibName,O_RDONLY|O_BINARY, S_IREAD)) == -1)
		return(NULL);


	//
	//	VERIFY it is a SOFTLIB (SLIB) file
	//

	if (BE_Cross_readInt8LEBuffer(handle,&header,4) < 4)
	//if (read(handle,&header,4) == -1)
	{
		BE_Cross_close(handle);
		return(NULL);
	}

	if (header != id_slib)
	{
		BE_Cross_close(handle);
		return(NULL);
	}


	//
	// CHECK LIBRARY HEADER VERSION NUMBER
	//

	if (BE_Cross_readInt8LEBuffer(handle, &LibraryHeader,sizeof(struct SoftLibHdr)) < sizeof(struct SoftLibHdr))
	//if (read(handle, &LibraryHeader,sizeof(struct SoftLibHdr)) == -1)
		Quit("read error in LoadSLIBFile()\n");
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	LibraryHeader.Version = BE_Cross_Swap16LE(LibraryHeader.Version);
	LibraryHeader.FileCount = BE_Cross_Swap16LE(LibraryHeader.FileCount);
#endif

	if (LibraryHeader.Version > SOFTLIB_VER)
		Quit("Unsupported file ver ");


	//
	// MANAGE FILE ENTRY HEADERS...
	//

	for (x = 1;x<=LibraryHeader.FileCount;x++)
	{
		if (BE_Cross_readInt8LEBuffer(handle, &FileEntryHeader,sizeof(struct FileEntryHdr)) < sizeof(struct FileEntryHdr))
		//if (read(handle, &FileEntryHeader,sizeof(struct FileEntryHdr)) == -1)
		{
			BE_Cross_close(handle);
			return(NULL);
		}
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		FileEntryHeader.Offset = BE_Cross_Swap32LE(FileEntryHeader.Offset);
		FileEntryHeader.ChunkLen = BE_Cross_Swap32LE(FileEntryHeader.ChunkLen);
		FileEntryHeader.OrginalLength = BE_Cross_Swap32LE(FileEntryHeader.OrginalLength);
		FileEntryHeader.Compression = BE_Cross_Swap16LE(FileEntryHeader.Compression);
#endif

		//if (!stricmp(FileEntryHeader.FileName,FileName))
		if (!BE_Cross_strcasecmp(FileEntryHeader.FileName,FileName))
		{
			FileEntry = FileEntryHeader;
			FileFound = true;
		}
	}

	//
	// IF FILE HAS BEEN FOUND THEN SEEK TO POSITION AND EXTRACT
	//	ELSE RETURN WITH ERROR CODE...
	//

	if (FileFound)
	{
		if (BE_Cross_seek(handle,FileEntry.Offset,SEEK_CUR) == -1)
		{
			BE_Cross_close(handle);
			return(NULL);
		}

		//
		// READ CHUNK HEADER - Verify we are at the beginning of a chunk..
		//

		if (BE_Cross_readInt8LEBuffer(handle,(id0_char_t *)&Header,sizeof(struct ChunkHeader)) < sizeof(struct ChunkHeader))
		//if (read(handle,(id0_char_t *)&Header,sizeof(struct ChunkHeader)) == -1)
			Quit("LIB File - Unable to read Header!");
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		// No need to swap Header.HeaderID, id_chunk (ID_CHUNK) should be adjusted
		Header.OrginalLength = BE_Cross_Swap32LE(Header.OrginalLength);
		Header.Compression = BE_Cross_Swap16LE(Header.Compression);
#endif

		if (Header.HeaderID != id_chunk)
			Quit("LIB File - BAD HeaderID!");

		//
		// Allocate memory if Necessary...
		//


		if (!*MemPtr)
			MM_GetPtr(MemPtr,FileEntry.OrginalLength);

		//
		//	Calculate the length of the data (without the chunk header).
		//

		ChunkLen = FileEntry.ChunkLen - sizeof(struct ChunkHeader);


		//
		// Extract Data from file
		//

		switch (Header.Compression)
		{

			#if LZW_SUPPORT
			case ct_LZW:
				if (!InitBufferedIO(handle,&lzwBIO))
					Quit("No memory for buffered I/O.");
				//lzwDecompress(&lzwBIO,MK_FP(*MemPtr,0),FileEntry.OrginalLength,(SRC_BFILE|DEST_MEM));
				lzwDecompress(&lzwBIO,*MemPtr,FileEntry.OrginalLength,(SRC_BFILE|DEST_MEM));
				FreeBufferedIO(&lzwBIO);
				break;
			#endif

			#if LZH_SUPPORT
			case ct_LZH:
				if (!InitBufferedIO(handle,&lzwBIO))
					Quit("No memory for buffered I/O.");
				//lzhDecompress(&lzwBIO, MK_FP(*MemPtr,0), FileEntry.OrginalLength, ChunkLen, (SRC_BFILE|DEST_MEM));
				lzhDecompress(&lzwBIO, *MemPtr, FileEntry.OrginalLength, ChunkLen, (SRC_BFILE|DEST_MEM));
				FreeBufferedIO(&lzwBIO);
				break;
			#endif

			case ct_NONE:
				//if (!CA_FarRead(handle,MK_FP(*MemPtr,0),ChunkLen))
				if (!CA_FarRead(handle,(id0_byte_t *)(*MemPtr),ChunkLen))
				{
//					BE_Cross_close(handle);
					*MemPtr = NULL;
				}
				break;

			default:
				BE_Cross_close(handle);
				Quit("Unknown Chunk.Compression Type!");
				break;
		}
	}
	else
		*MemPtr = NULL;

	BE_Cross_close(handle);
	return(*MemPtr);
}




