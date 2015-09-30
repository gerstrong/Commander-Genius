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

#include "kd_def.h"

#define BIO_BUFFER_LEN	(512)



////////////////////////////////////////////////////////////////////////////
//
// FreeShape()
//
void FreeShape(struct Shape *shape)
{
	if (shape->Data)
		MM_FreePtr(&shape->Data);
}

////////////////////////////////////////////////////////////////////////////
//
// UnpackEGAShapeToScreen()
//
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty)
{
	id0_int_t currenty;
	id0_signed_char_t n, Rep, id0_far *Src/*, id0_far *Dst[8]*/, loop, Plane;
	id0_unsigned_int_t DstOff[8];
	id0_unsigned_int_t BPR, Height;
	id0_int_t NotWordAligned;

	NotWordAligned = SHP->BPR & 1;
	startx>>=3;
	Src = (id0_signed_char_t *)(SHP->Data);
	currenty = starty;
	Plane = 0;
	Height = SHP->bmHdr.h;
	while (Height--)
	{
#if 0
		Dst[0] = (MK_FP(0xA000,displayofs));
		Dst[0] += ylookup[currenty];
		Dst[0] += startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			Dst[loop] = Dst[0];
#endif
		DstOff[0] = displayofs + ylookup[currenty] + startx;
		for (loop=1; loop<SHP->bmHdr.d; loop++)
			DstOff[loop] = DstOff[0];


		for (Plane=0; Plane<SHP->bmHdr.d; Plane++)
		{
			//outport(0x3c4,((1<<Plane)<<8)|2);

			BPR = ((SHP->BPR+1) >> 1) << 1;               // IGNORE WORD ALIGN
			while (BPR)
			{
				if (SHP->bmHdr.comp)
					n = *Src++;
				else
					n = BPR-1;

				if (n < 0)
				{
					if (n != -128)
					{
						n = (-n)+1;
						BPR -= n;
						Rep = *Src++;
						if ((!BPR) && (NotWordAligned))   // IGNORE WORD ALIGN
							n--;

						while (n--)
							BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, Rep, 1<<Plane);
							//*Dst[Plane]++ = Rep;
					}
					else
						BPR--;
				}
				else
				{
					n++;
					BPR -= n;
					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						n--;

					while (n--)
						BE_ST_EGAUpdateGFXByte(DstOff[Plane]++, *Src++, 1<<Plane);
						//*Dst[Plane]++ = *Src++;

					if ((!BPR) && (NotWordAligned))     // IGNORE WORD ALIGN
						Src++;
				}
			}
		}
		currenty++;
	}

	return(0);
}

////////////////////////////////////////////////////////////////////////////
//
// Verify()
//
id0_long_t Verify(const id0_char_t *filename)
{
	BE_FILE_T handle;
	id0_long_t size;

	if (!BE_Cross_IsFileValid(handle=BE_Cross_open_for_reading(filename)))
	//if ((handle=open(filename,O_BINARY))==-1)
		return (0);
	size=BE_Cross_FileLengthFromHandle(handle);
	BE_Cross_close(handle);
	return(size);
}

//--------------------------------------------------------------------------
// InitBufferedIO()
//--------------------------------------------------------------------------
memptr InitBufferedIO(BE_FILE_T handle, BufferedIO *bio)
{
	bio->handle = handle;
	bio->offset = BIO_BUFFER_LEN;
	bio->status = 0;
	MM_GetPtr(&bio->buffer,BIO_BUFFER_LEN);

	return(bio->buffer);
}

//--------------------------------------------------------------------------
// FreeBufferedIO()
//--------------------------------------------------------------------------
void FreeBufferedIO(BufferedIO *bio)
{
	if (bio->buffer)
		MM_FreePtr(&bio->buffer);
}

//--------------------------------------------------------------------------
// bio_readch()
//--------------------------------------------------------------------------
id0_byte_t bio_readch(BufferedIO *bio)
{
	id0_byte_t id0_far *buffer;

	if (bio->offset == BIO_BUFFER_LEN)
	{
		bio->offset = 0;
		bio_fillbuffer(bio);
	}

	buffer = (id0_byte_t *)(bio->buffer) + (bio->offset++);
	//buffer = MK_FP(bio->buffer,bio->offset++);

	return(*buffer);
}

//--------------------------------------------------------------------------
// bio_fillbuffer()
//
// BUGS (Not really bugs... More like RULES!)
//
//    1) This code assumes BIO_BUFFER_LEN is no smaller than
//       NEAR_BUFFER_LEN!!
//
//    2) BufferedIO.status should be altered by this code to report
//       read errors, end of file, etc... If you know how big the file
//       is you're reading, determining EOF should be no problem.
//
//--------------------------------------------------------------------------
void bio_fillbuffer(BufferedIO *bio)
{
	#define NEAR_BUFFER_LEN	(64)
	id0_byte_t near_buffer[NEAR_BUFFER_LEN];
	id0_short_t bio_length,bytes_read,bytes_requested;

	bytes_read = 0;
	bio_length = BIO_BUFFER_LEN;
	while (bio_length)
	{
		if (bio_length > NEAR_BUFFER_LEN-1)
			bytes_requested = NEAR_BUFFER_LEN;
		else
			bytes_requested = bio_length;

		BE_Cross_readInt8LEBuffer(bio->handle,near_buffer,bytes_requested);
		//read(bio->handle,near_buffer,bytes_requested);
		memcpy((id0_byte_t *)(bio->buffer) + bytes_read,near_buffer,bytes_requested);
		//_fmemcpy(MK_FP(bio->buffer,bytes_read),near_buffer,bytes_requested);

		bio_length -= bytes_requested;
		bytes_read += bytes_requested;
	}
}

// REFKEEN - UNUSED (cross-platform replacements may be used, depending on arch)
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

////////////////////////////////////////////////////////////////////////////
//
// MoveGfxDst()
//
void MoveGfxDst(id0_short_t x, id0_short_t y)
{
	id0_unsigned_t address;

	address = (y*linewidth)+(x/8);
	bufferofs = displayofs = address;
}
