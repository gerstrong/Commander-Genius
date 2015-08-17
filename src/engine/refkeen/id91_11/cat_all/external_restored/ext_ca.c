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

/* Reconstructed from the LOADSCN.EXE file bundled with The Catacomb Abyss
 * Shareware v1.13, using available source files
 */


#include "ext_heads.h"

/*
==========================
=
= CA_FarRead
=
= Read from a file to a far pointer
=
==========================
*/

id0_boolean_t FarRead (BE_FILE_T handle, id0_byte_t id0_far *dest, id0_long_t length)
{
	if (length>0xffffl)
		TrashProg ("CA_FarRead doesn't support 64K reads yet!");

	// Ported from ASM
	int bytesread = BE_Cross_readInt8LEBuffer(handle, dest, length);
	if (bytesread == 0)
	{
		// Keep errno as set by read
		return false;
	}

	if (bytesread != length)
	{
		//errno = EINVFMT; // user manager knows this is bad read
		return false;
	}
	return true;
}


#if 0
/*
==========================
=
= CA_ReadFile
=
= Reads a file into an allready allocated buffer
=
==========================
*/

id0_boolean_t ReadFile (id0_char_t *filename, memptr ptr)
{
	BE_FILE_T handle;
	id0_long_t size;

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(filename)))
	//if ((handle = open(filename,O_RDONLY | O_BINARY, S_IREAD)) == -1)
		return false;

	size = filelength (handle);
	if (!FarRead (handle,ptr,size))
	{
		close (handle);
		return false;
	}
	close (handle);
	return true;
}
#endif


/*
==========================
=
= CA_LoadFile
=
= Allocate space for and load a file
=
==========================
*/

id0_boolean_t LoadFile (const id0_char_t *filename, memptr *ptr)
{
	BE_FILE_T handle;
	id0_long_t size;

	if (!BE_Cross_IsFileValid(handle = BE_Cross_open_for_reading(filename)))
	//if ((handle = open(filename,O_RDONLY | O_BINARY, S_IREAD)) == -1)
		return false;

	size = BE_Cross_FileLengthFromHandle (handle);
	if (!(*ptr))
	{
		if (!(*ptr = malloc/*farmalloc*/(size)))
			TrashProg("Can't get memory!");
	}
	if (!FarRead (handle,(id0_byte_t *)*ptr,size))
	{
		BE_Cross_close (handle);
		return false;
	}
	BE_Cross_close (handle);
	return true;
}

