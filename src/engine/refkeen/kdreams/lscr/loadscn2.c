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
//#include <dos.h>
//#include <mem.h>
//#include <alloc.h>
#include <fcntl.h>
//#include <conio.h>
#include <string.h>
//#include <io.h>
#include <sys/stat.h>
#include <stdlib.h>
//#include <process.h>

#include "id_heads.h" // For types like id0_int_t, and refkeen.h

#include "jampak.h"
#include "sl_file.h"


#define VERSION_NUM	"1.10"
#define MESSAGE		"THIS VERSION IS MADE FOR USE IN G.E. TEXT SHELL"

/*****************************************************************
UPDATE (REFKEEN): Some functions are marked "static" here
since they conflict with functions from jam_io.c and possibly more
*****************************************************************/



//===========================================================================
//
//									 LOCAL PROTOTYPES
//
//===========================================================================


//UPDATE (REFKEEN) We define these types externally now
//typedef enum {false=0,true} boolean;

static id0_int_t WritePtr(void **outfile, id0_unsigned_char_t data, id0_unsigned_t PtrType);
static id0_int_t ReadPtr(void **infile, id0_unsigned_t PtrType);
static void id0_far lzwDecompress(void id0_far *infile, void id0_far *outfile, id0_unsigned_long_t DataLength, id0_unsigned_t PtrTypes);
static id0_boolean_t LoadLIBFile(const id0_char_t *LibName, const id0_char_t *FileName, id0_char_t id0_far **MemPtr);

//static id0_long_t FileSize(const id0_char_t *filename);

static id0_boolean_t FarRead (BE_FILE_T handle, id0_char_t id0_far *dest, id0_long_t length);

int loadscn2_main(int argc, const char **argv); // Renamed from main
static void TrashProg(const id0_char_t *OutMsg);



//=========================================================================
//
//									LOCAL VARIABLES
//
//=========================================================================


id0_unsigned_char_t id0_far LZW_ring_buffer[LZW_N + LZW_F - 1];

	// ring buffer of size LZW_N, with extra LZW_F-1 bytes to facilitate
	//	string comparison




//==========================================================================
//
//
//							DECOMPRESSION/LIB ROUTINES
//
//
//==========================================================================


//---------------------------------------------------------------------------
// WritePtr()  -- Outputs data to a particular ptr type
//
//	PtrType MUST be of type DEST_TYPE.
//
// NOTE : For PtrTypes DEST_MEM a ZERO (0) is always returned.
//
//---------------------------------------------------------------------------
static id0_int_t WritePtr(void **outfile, id0_unsigned_char_t data, id0_unsigned_t PtrType)
{
	id0_int_t returnval = 0;

	switch (PtrType & DEST_TYPES)
	{
		case DEST_FILE:
			// REFKEEN: Actually DEST_FFILE but done for compatibility
			returnval = BE_Cross_putc(data, *(BE_FILE_T *)outfile);
			//write(*(int id0_far *)outfile,(id0_char_t *)&data,1);
		break;

		case DEST_FFILE:
			returnval = putc(data, *(FILE **)outfile);
		break;

		case DEST_MEM:
#if 0
//			*(*(id0_char_t id0_far **)outfile++) = data;						// Do NOT delete
			*((id0_char_t id0_far *)*(id0_char_t id0_far **)outfile)++ = data;
#endif
		{
			id0_char_t **ptrptr = (id0_char_t **)outfile;
			*((*ptrptr)++) = data;
		}
		break;

		default:
			TrashProg("WritePtr() : Unknown DEST_PTR type");
		break;
	}

	return(returnval);

}


//---------------------------------------------------------------------------
// ReadPtr()  -- Reads data from a particular ptr type
//
//	PtrType MUST be of type SRC_TYPE.
//
// RETURNS :
//		The char read in or EOF for SRC_FFILE type of reads.
//
//
//---------------------------------------------------------------------------
static id0_int_t ReadPtr(void **infile, id0_unsigned_t PtrType)
{
	id0_int_t returnval = 0;

	switch (PtrType & SRC_TYPES)
	{
		case SRC_FILE:
			// REFKEEN: Actually SRC_FFILE but done for compatibility
			returnval = BE_Cross_getc(*(BE_FILE_T *)infile);
			//read(*(int id0_far *)infile,(id0_char_t *)&returnval,1);
		break;

		case SRC_FFILE:
			returnval = fgetc((FILE id0_far *)*(FILE id0_far **)infile);
		break;


		case SRC_MEM:
#if 0
			returnval = (id0_char_t)*(*(id0_char_t id0_far **)infile++);
//			returnval = *((id0_char_t id0_far *)*(id0_char_t id0_far **)infile)++;	// DO NOT DELETE!
#endif
		{
		{
			id0_char_t **ptrptr = (id0_char_t **)infile;
			returnval = (id0_char_t)*((*ptrptr)++);
		}		
		}
		break;

		default:
			TrashProg("ReadPtr() : Unknown SRC_PTR type");
		break;
	}

	return(returnval);
}





//--------------------------------------------------------------------------
//
// lzwDecompress() - Compresses data from an input ptr to a dest ptr
//
// PARAMS:
//		 infile     - Pointer at the BEGINNING of the compressed data (no header!)
//		 outfile    - Pointer to the destination.
// 	 DataLength - Length of compressed data.
//     PtrTypes   - Type of pointers being used (SRC_FILE,DEST_FILE,SRC_MEM etc).
//
// RETURNS:
//	    Length of compressed data.
//
//	COMPTYPE : ct_LZW
//
// NOTES    : Does not write ANY header information!
//
static void id0_far lzwDecompress(void id0_far *infile, void id0_far *outfile, id0_unsigned_long_t DataLength, id0_unsigned_t PtrTypes)
{
	id0_int_t  i, j, k, r, c;
	id0_unsigned_int_t flags;

	for (i = 0; i < LZW_N - LZW_F; i++)
		LZW_ring_buffer[i] = ' ';

	 r = LZW_N - LZW_F;
	 flags = 0;

	 for ( ; ; )
	 {
			if (((flags >>= 1) & 256) == 0)
			{
				c = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);
				if (!DataLength--)
					return;

				flags = c | 0xff00;      // uses higher byte cleverly to count 8
			}

			if (flags & 1)
			{
				c = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);		// Could test for EOF iff FFILE type
				if (!DataLength--)
					return;

				WritePtr(/*(id0_long_t)*/&outfile,c,PtrTypes);

				LZW_ring_buffer[r++] = c;
				r &= (LZW_N - 1);
			}
			else
			{
				i = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);
				if (!DataLength--)
					return;

				j = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);
				if (!DataLength--)
					return;

				i |= ((j & 0xf0) << 4);
				j = (j & 0x0f) + LZW_THRESHOLD;

				for (k = 0; k <= j; k++)
				{
					 c = LZW_ring_buffer[(i + k) & (LZW_N - 1)];

					 WritePtr(/*(id0_long_t)*/&outfile,c,PtrTypes);

					 LZW_ring_buffer[r++] = c;
					 r &= (LZW_N - 1);
				}
			}
	 }
}



//----------------------------------------------------------------------------
// LoadLIBFile() -- Copies a file from an existing archive to dos.
//
// PARAMETERS :
//
//			LibName 		 - Name of lib file created with SoftLib V1.0
//
//			FileName 	 - Name of file to load from lib file.
//
//			MemPtr  		 - Address of variable to store the file addr.
//							 - if VariablePtr is NULL then will allocmem before
//                      loading.
//       UseID_Memory - TRUE = use MM_GetPtr else farmalloc.
//
//
// RETURN :
//
//   		true   - Successfull load
//			false  - Error!
//
//----------------------------------------------------------------------------
static id0_boolean_t LoadLIBFile(const id0_char_t *LibName, const id0_char_t *FileName, id0_char_t id0_far **MemPtr)
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
	id0_boolean_t Success = true;


	//
	// OPEN SOFTLIB FILE
	//

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(LibName)))
	//if ((handle = open(LibName,O_RDONLY | O_BINARY, S_IRUSR)) == -1)
		TrashProg("LOADSCN ERROR : Error openning file.");


	//
	//	VERIFY it is a SOFTLIB (SLIB) file
	//

	if (BE_Cross_readInt8LEBuffer(handle,&header,4) < 4)
	//if (read(handle,&header,4) == -1)
	{
		BE_Cross_close(handle);
		TrashProg("LOADSCN ERROR : Error reading first header.");
	}

	if (header != id_slib)
	{
		BE_Cross_close(handle);
		TrashProg("LOADSCN ERROR : Header on id_slib.");
	}


	//
	// CHECK LIBRARY HEADER VERSION NUMBER
	//

	if (BE_Cross_readInt8LEBuffer(handle, &LibraryHeader,sizeof(struct SoftLibHdr)) < sizeof(struct SoftLibHdr))
	//if (read(handle, &LibraryHeader,sizeof(struct SoftLibHdr)) == -1)
		TrashProg("read error in LoadSLIBFile()");
	// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
	LibraryHeader.Version = BE_Cross_Swap16LE(LibraryHeader.Version);
	LibraryHeader.FileCount = BE_Cross_Swap16LE(LibraryHeader.FileCount);
#endif

	if (LibraryHeader.Version > SOFTLIB_VER)
		TrashProg("Unsupported file version ");


	//
	// MANAGE FILE ENTRY HEADERS...
	//

	for (x = 1;x<=LibraryHeader.FileCount;x++)
	{
		if (BE_Cross_readInt8LEBuffer(handle, &FileEntryHeader,sizeof(struct FileEntryHdr)) < sizeof(struct FileEntryHdr))
		//if (read(handle, &FileEntryHeader,sizeof(struct FileEntryHdr)) == -1)
		{
			BE_Cross_close(handle);
			TrashProg("LOADSCN ERROR : Error reading second header.");
		}
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		FileEntryHeader.Offset = BE_Cross_Swap32LE(FileEntryHeader.Offset);
		FileEntryHeader.ChunkLen = BE_Cross_Swap32LE(FileEntryHeader.ChunkLen);
		FileEntryHeader.OrginalLength = BE_Cross_Swap32LE(FileEntryHeader.OrginalLength);
		FileEntryHeader.Compression = BE_Cross_Swap16LE(FileEntryHeader.Compression);
#endif

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
			TrashProg("LOADSCN ERROR : Error seeking through file.");
		}

		//
		// READ CHUNK HEADER - Verify we are at the beginning of a chunk..
		//

		if (BE_Cross_readInt8LEBuffer(handle,(id0_char_t *)&Header,sizeof(struct ChunkHeader)) < sizeof(struct ChunkHeader))
		//if (read(handle,(id0_char_t *)&Header,sizeof(struct ChunkHeader)) == -1)
			TrashProg("LIB File - Unable to read Header!");
		// REFKEEN - Big Endian support
#ifdef REFKEEN_ARCH_BIG_ENDIAN
		// No need to swap Header.HeaderID, id_chunk (ID_CHUNK) should be adjusted
		Header.OrginalLength = BE_Cross_Swap32LE(Header.OrginalLength);
		Header.Compression = BE_Cross_Swap16LE(Header.Compression);
#endif

		if (Header.HeaderID != id_chunk)
			TrashProg("LIB File - BAD HeaderID!");

		//
		// Allocate memory if Necessary...
		//

		if (*MemPtr == NULL)
		{
			BE_ST_Delay(2000);
			if ((*MemPtr = (id0_char_t *)/*far*/malloc(FileEntry.OrginalLength)) == NULL)
				TrashProg("Can't get memory");
		}

		//
		//	Calculate the length of the data (without the chunk header).
		//

		ChunkLen = FileEntry.ChunkLen - sizeof(struct ChunkHeader);

		//
		// Extract Data from file
		//

		switch (Header.Compression)
		{
			case ct_LZW:
				// REFKEEN - This is a BE_FILE_T (FILE*) instead of int now, but still specifying SRC_FILE for compatibility
				lzwDecompress(handle,*MemPtr,ChunkLen,(SRC_FILE|DEST_MEM));
				break;

			case ct_NONE:
				Success = FarRead(handle,*MemPtr,ChunkLen);
				break;

			default:
				BE_Cross_close(handle);
				TrashProg("Unknown Chunk.Compression Type!");
				break;
		}
	}
	else
		Success = false;

	BE_Cross_close(handle);

	return(Success);
}




//===========================================================================
//
//                         GENERAL FUNCTIONS
//
//===========================================================================

#if 0
//---------------------------------------------------------------------------
//  FileSize() - Returns the size of a file on disk. (-1 = error)
//---------------------------------------------------------------------------
static id0_long_t FileSize(const id0_char_t *filename)
{
	  id0_long_t filesize;
	  int handle;

	  if ((handle = BE_Cross_open_for_reading(filename)) != -1)
	  //if ((handle = open(filename,O_RDONLY)) != -1)
	  {
			filesize = BE_Cross_FileLengthFromHandle(handle) ;
			close(handle);
	  }
	  else
			filesize = 0;

	return(filesize);
}
#endif


//--------------------------------------------------------------------------
// FarRead()
//-------------------------------------------------------------------------
static id0_boolean_t FarRead (BE_FILE_T handle, id0_char_t id0_far *dest, id0_long_t length)
{
	if (length>0xffffl)
		TrashProg("CA_FarRead doesn't support 64K reads yet!");
	// Ported from ASM (based on CA_FarRead)
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




//--------------------------------------------------------------------------
//  MAIN (renamed from main)
//--------------------------------------------------------------------------
int loadscn2_main(int argc, const char **argv)
{
	id0_unsigned_char_t id0_huge *bufferptr = NULL;

	if (BE_Cross_strcasecmp(argv[1], "/VER") == 0)
	{
		BE_ST_clrscr();
		printf("\nG.E. Load Text Screen.\n");
		printf("Copyright 1992 Softdisk Publishing\n");
		printf("Version Number %s\n", VERSION_NUM);
#ifdef MESSAGE
		printf("Note : %s\n", MESSAGE);
#endif
		return(0);
	}

	if ((argc < 2) || (argc > 3))
	{
		printf("\nG.E. Load Text Screen.\n");
		printf("Copyright 1992 Softdisk Publishing\n");
		printf("by Nolan Martin and Jim Row\n");
		printf("usage : LOADSCN libname Filename\n");
		return(1);
		}

	BE_ST_ToggleTextCursor(false);

	if (!LoadLIBFile(argv[1],argv[2], (id0_char_t **)&bufferptr))
		TrashProg("Error loading TEXT_SCEENS");

	memcpy(BE_ST_GetTextModeMemoryPtr(), bufferptr+7, 4000);
	BE_ST_MarkGfxForUpdate();
	//_fmemcpy(MK_FP(0xB800,0), bufferptr+7, 4000);

	BE_ST_ToggleTextCursor(true);

	BE_ST_MoveTextCursorTo(0, 23); // gotoxy(1, 24)

	BE_ST_HandleExit(0);

	return 0; // REFKEEN: Could also declare this function as void instead..

}  // main end


//---------------------------------------------------------------------------
//  TrashProg() --
//---------------------------------------------------------------------------
static void TrashProg(const id0_char_t *OutMsg)
{
	id0_int_t error = 0;

	BE_ST_ToggleTextCursor(true);

	if (OutMsg)
	{
		BE_ST_puts(OutMsg);
		//printf("%s\n",OutMsg);
		error = 1;
	}

	BE_ST_HandleExit(error);
}

