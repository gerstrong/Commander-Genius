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
#include "jam_io.h"

//----------------------------------------------------------------------------
//
//	 						PTR/PTR COMPRESSION ROUTINES
//
//
//----------------------------------------------------------------------------



//---------------------------------------------------------------------------
// WritePtr()  -- Outputs data to a particular ptr type
//
//	PtrType MUST be of type DEST_TYPE.
//
// NOTE : For PtrTypes DEST_MEM a ZERO (0) is always returned.
//
//---------------------------------------------------------------------------
id0_char_t WritePtr(void **outfile, id0_unsigned_char_t data, id0_unsigned_t PtrType)
{
	id0_int_t returnval = 0;

	switch (PtrType & DEST_TYPES)
	{
		case DEST_FILE:
			// REFKEEN: Actually DEST_FFILE but done for compatibility
			returnval = BE_Cross_putc(data, *(BE_FILE_T *)outfile);
			//write(*(int *)outfile,(id0_char_t *)&data,1);
		break;

		case DEST_FFILE:
			returnval = putc(data, *(FILE **)outfile);
		break;

		case DEST_IMEM:
			BE_ST_printf("WritePtr - unsupported ptr type\n");
			BE_ST_HandleExit(0);
		break;

		case DEST_MEM:
			//*((id0_char_t id0_far *)*(id0_char_t id0_far **)outfile)++ = data;
		{
			id0_char_t **ptrptr = (id0_char_t **)outfile;
			*((*ptrptr)++) = data;
		}
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
id0_int_t ReadPtr(void **infile, id0_unsigned_t PtrType)
{
	id0_int_t returnval = 0;

	switch (PtrType & SRC_TYPES)
	{
		case SRC_FILE:
			// REFKEEN: Actually SRC_FFILE but done for compatibility
			returnval = BE_Cross_getc(*(BE_FILE_T *)infile);
			//read(*(int *)infile,(id0_char_t *)&returnval,1);
		break;

		case SRC_FFILE:
			returnval = getc(*(FILE id0_far **)infile);
		break;

		case SRC_BFILE:
			returnval = bio_readch((BufferedIO *)*(void id0_far **)infile);
		break;

//		case SRC_IMEM:
//			BE_ST_printf("WritePtr - unsupported ptr type\n");
//			BE_ST_HandleExit(0);
//		break;

		case SRC_MEM:
			//returnval = (id0_unsigned_char_t)*((id0_char_t id0_far *)*(id0_char_t id0_far **)infile)++;
		{
			id0_char_t **ptrptr = (id0_char_t **)infile;
			returnval = (id0_unsigned_char_t)*((*ptrptr)++);
		}		
		break;
	}

	return(returnval);
}



