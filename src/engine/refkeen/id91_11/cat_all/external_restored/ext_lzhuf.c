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

//===========================================================================
//
//								 LZHUFF COMPRESSION ROUTINES
//										  VERSION 1.0
//
//  				Compression algrythim by Haruhiko OKUMURA
//  						Implementation by Jim T. Row
//
//
//   Copyright (c) 1992 - Softdisk Publishing inc. - All rights reserved
//
//===========================================================================
//
// Compiler #ifdef switches
//
// 	LZHUFF_COMPRESSION & LZHUFF_DECOMPRESSION		- not yet functional!
//
// Usage Explanition :
//
//    if LZHUFF_COMPRESSION is defined then the compression code & data is
//    compiled and so-forth for the decompression code.
//
//---------------------------------------------------------------------------



#include <fcntl.h>
//#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <alloc.h>
//#include <dos.h>

#include "ext_heads.h" // For types like id0_int_t, and refkeen.h

#include "ext_lzhuff.h"
#include "ext_jam_io.h"



//===========================================================================
//
//											SWITCHES
//
// NOTE : Make sure the appropriate switches are set in SOFT.c for Softlib
//			 archive support.
//
//===========================================================================


#define INCLUDE_LZH_COMP			0
#define INCLUDE_LZH_DECOMP			1





//===========================================================================
//
//											DEFINES
//
//===========================================================================


#define EXIT_OK 			0
#define EXIT_FAILED 		-1

/* LZSS Parameters */

#define N				4096								/* Size of string buffer */
#define F				30									/* Size of look-ahead buffer */
#define THRESHOLD		2
#define NIL				N									/* End of tree's node  */

/* Huffman coding parameters */

#define N_CHAR  		(256 - THRESHOLD + F)		/* character code (= 0..N_CHAR-1) */
#define T 				(N_CHAR * 2 - 1)				/* Size of table */
#define R 				(T - 1)							/* root position */
#define MAX_FREQ		0x8000                     /* update when cumulative frequency */
																/* reaches to this value */


//==========================================================================
//
//								LOCAL PROTOTYPES
//
//==========================================================================


static void StartHuff();
static void reconst();
static void update(id0_int_t c);

// REFKEEN - Unnecessary to declare here, especially when unused
#if 0
static void DeleteNode(id0_int_t p);  /* Deleting node from the tree */
static void InsertNode(id0_int_t r);  /* Inserting node to the tree */
static void InitTree(void);  /* Initializing tree */
static void Putcode(void **outfile_ptr, id0_int_t l, id0_unsigned_t c,id0_unsigned_t PtrTypes);		/* output c bits */
static void EncodeChar(void **outfile_ptr, id0_unsigned_t c, id0_unsigned_t PtrTypes);
static void EncodePosition(void **outfile_ptr, id0_unsigned_t c, id0_unsigned_t PtrTypes);
static void EncodeEnd(void **outfile_ptr,id0_unsigned_t PtrTypes);
#endif

static id0_int_t GetByte(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes);
static id0_int_t GetBit(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes);	/* get one bit */
static id0_int_t DecodeChar(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes);
static id0_int_t DecodePosition(void **infile_ptr,id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes);




//==========================================================================
//
//								USER AVAILABLE VECTORS
//
//==========================================================================




//---------------------------------------------------------------------------
//
// 								LZHUFF DISPLAY VECTORS
//
// These vectors allow you to hook up any form of display you desire for
// displaying the compression/decompression status.
//
// These routines are called inside of the compression/decompression routines
// and pass the orginal size of data and current position within that
// data.  This allows for any kind of "% Done" messages.
//
// Your functions MUST have the following parameters in this order...
//
//   void VectorRoutine(id0_unsigned_long_t OrginalSize,id0_unsigned_long_t CurPosition)
//
//

// REFKEEN - Wrong amount of arguments (errors as C++), but unused anyway
#if 0
#if INCLUDE_LZH_COMP
void (*LZH_CompressDisplayVector)() = NULL;
#endif

#if INCLUDE_LZH_DECOMP
void (*LZH_DecompressDisplayVector)() = NULL;
#endif
#endif




//===========================================================================
//
//											GLOBAL VARIABLES
//
//===========================================================================
	/* pointing children nodes (son[], son[] + 1)*/

static id0_int_t id0_far son[T];
//static id0_unsigned_t code, len;

	//
	// pointing parent nodes.
	// area [T..(T + N_CHAR - 1)] are pointers for leaves
	//

static id0_int_t id0_far prnt[T + N_CHAR];

static id0_unsigned_t id0_far freq[T + 1];	/* cumulative freq table */

static id0_unsigned_long_t textsize = 0/*, codesize = 0, printcount = 0*/,datasize;
static id0_unsigned_char_t id0_far text_buf[N + F - 1];



	//
	// COMPRESSION VARIABLES
	//

#if INCLUDE_LZH_COMP

static id0_int_t match_position,match_length, lson[N + 1], rson[N + 257], dad[N + 1];
static id0_unsigned_t putbuf = 0;
static id0_unsigned_char_t putlen = 0;

	//
	// Tables for encoding/decoding upper 6 bits of
	// sliding dictionary pointer
	//

	//
	// encoder table
	//

static id0_unsigned_char_t id0_far p_len[64] = {
	0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

static id0_unsigned_char_t id0_far p_code[64] = {
	0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
	0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
	0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
	0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
	0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
	0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};
#endif


	//
	// DECOMPRESSION VARIABLES
	//


	//
	// decoder table
	//

#if INCLUDE_LZH_DECOMP

static id0_unsigned_char_t id0_far d_code[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
	0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
	0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
	0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
	0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
	0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
	0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
	0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
	0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static id0_unsigned_char_t id0_far d_len[256] = {
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static id0_unsigned_t getbuf = 0;
static id0_unsigned_char_t getlen = 0;

#endif



//===========================================================================
//
//							COMPRESSION & DECOMPRESSION ROUTINES
//
//===========================================================================







//---------------------------------------------------------------------------
//  StartHuff    /* initialize freq tree */
//---------------------------------------------------------------------------
static void StartHuff()
{
	id0_int_t i, j;

	for (i = 0; i < N_CHAR; i++) {
		freq[i] = 1;
		son[i] = i + T;
		prnt[i + T] = i;
	}
	i = 0; j = N_CHAR;
	while (j <= R) {
		freq[j] = freq[i] + freq[i + 1];
		son[j] = i;
		prnt[i] = prnt[i + 1] = j;
		i += 2; j++;
	}
	freq[T] = 0xffff;
	prnt[R] = 0;
}






//---------------------------------------------------------------------------
//   reconst        /* reconstruct freq tree */
//---------------------------------------------------------------------------
static void reconst()
{
	id0_int_t i, j, k;
	id0_unsigned_t f, l;

	/* halven cumulative freq for leaf nodes */

	j = 0;

	for (i = 0; i < T; i++)
	{
		if (son[i] >= T)
		{
			freq[j] = (freq[i] + 1) / 2;
			son[j] = son[i];
			j++;
		}
	}

	/* make a tree : first, connect children nodes */

	for (i = 0, j = N_CHAR; j < T; i += 2, j++)
	{
		k = i + 1;
		f = freq[j] = freq[i] + freq[k];

		for (k = j - 1;f < freq[k]; k--);

		k++;
		l = (j - k) * 2;

		(void)memmove(&freq[k + 1], &freq[k], l);
		freq[k] = f;

		(void)memmove(&son[k + 1], &son[k], l);
		son[k] = i;
	}

	/* connect parent nodes */

	for (i = 0; i < T; i++)
	{
		if ((k = son[i]) >= T)
		{
			prnt[k] = i;
		}
		else
		{
			prnt[k] = prnt[k + 1] = i;
		}
	}
}






//---------------------------------------------------------------------------
//  update()	 update freq tree
//---------------------------------------------------------------------------
static void update(id0_int_t c)
{
	id0_int_t i, j, k, l;

	if (freq[R] == MAX_FREQ)
	{
		reconst();
	}

	c = prnt[c + T];

	do {
		k = ++freq[c];

		//
		// swap nodes to keep the tree freq-ordered
		//

		if (k > freq[l = c + 1])
		{
			while (k > freq[++l]);

			l--;
			freq[c] = freq[l];
			freq[l] = k;

			i = son[c];
			prnt[i] = l;
			if (i < T)
				prnt[i + 1] = l;

			j = son[l];
			son[l] = i;

			prnt[j] = c;
			if (j < T)
				prnt[j + 1] = c;

			son[c] = j;

			c = l;
		}
	} while ((c = prnt[c]) != 0);	/* do it until reaching the root */
}




//===========================================================================
//
//									 COMPRESSION ROUTINES
//
//===========================================================================






#if INCLUDE_LZH_COMP


//---------------------------------------------------------------------------
// DeleteNode
//---------------------------------------------------------------------------
static void DeleteNode(id0_int_t p)  /* Deleting node from the tree */
{
	id0_int_t  q;

	if (dad[p] == NIL)
		return;			/* unregistered */

	if (rson[p] == NIL)
		q = lson[p];
	else
	if (lson[p] == NIL)
		q = rson[p];
	else
	{
		q = lson[p];
		if (rson[q] != NIL)
		{
			do {
				q = rson[q];
			} while (rson[q] != NIL);

			rson[dad[q]] = lson[q];
			dad[lson[q]] = dad[q];
			lson[q] = lson[p];
			dad[lson[p]] = q;
		}

		rson[q] = rson[p];
		dad[rson[p]] = q;
	}

	dad[q] = dad[p];

	if (rson[dad[p]] == p)
		rson[dad[p]] = q;
	else
		lson[dad[p]] = q;

	dad[p] = NIL;
}






//---------------------------------------------------------------------------
//  InsertNode
//---------------------------------------------------------------------------
static void InsertNode(id0_int_t r)  /* Inserting node to the tree */
{
	id0_int_t  i, p, cmp;
	id0_unsigned_char_t  *key;
	id0_unsigned_t c;

	cmp = 1;
	key = &text_buf[r];
	p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;
	match_length = 0;
	for ( ; ; )
	{
		if (cmp >= 0)
		{
			if (rson[p] != NIL)
				p = rson[p];
			else
			{
				rson[p] = r;
				dad[r] = p;
				return;
			}
		}
		else
		{
			if (lson[p] != NIL)
				p = lson[p];
			else
			{
				lson[p] = r;
				dad[r] = p;
				return;
			}
		}


		for (i = 1; i < F; i++)
			if ((cmp = key[i] - text_buf[p + i]) != 0)
				break;

		if (i > THRESHOLD)
		{
			if (i > match_length)
			{
				match_position = ((r - p) & (N - 1)) - 1;
				if ((match_length = i) >= F)
					break;
			}

			if (i == match_length)
			{
				if ((c = ((r - p) & (N - 1)) - 1) < match_position)
				{
					match_position = c;
				}
			}
		}
	}

	dad[r] = dad[p];
	lson[r] = lson[p];
	rson[r] = rson[p];
	dad[lson[p]] = r;
	dad[rson[p]] = r;

	if (rson[dad[p]] == p)
		rson[dad[p]] = r;
	else
		lson[dad[p]] = r;

	dad[p] = NIL;  /* remove p */
}





//---------------------------------------------------------------------------
// InitTree
//---------------------------------------------------------------------------
static void InitTree(void)  /* Initializing tree */
{
	id0_int_t  i;

	for (i = N + 1; i <= N + 256; i++)
		rson[i] = NIL;			/* root */

	for (i = 0; i < N; i++)
		dad[i] = NIL;			/* node */
}






//---------------------------------------------------------------------------
//  Putcode
//---------------------------------------------------------------------------
static void Putcode(void **outfile_ptr, id0_int_t l, id0_unsigned_t c,id0_unsigned_t PtrTypes)		/* output c bits */
{
	putbuf |= c >> putlen;

	if ((putlen += l) >= 8)
	{
		WritePtr(outfile_ptr, putbuf >> 8, PtrTypes);
		codesize++;

		if ((putlen -= 8) >= 8)
		{
			WritePtr(outfile_ptr, putbuf, PtrTypes);
			codesize++;

			putlen -= 8;
			putbuf = c << (l - putlen);
		}
		else
		{
			putbuf <<= 8;
		}
	}
}






//---------------------------------------------------------------------------
//  EncodeChar
//---------------------------------------------------------------------------
static void EncodeChar(void **outfile_ptr, id0_unsigned_t c, id0_unsigned_t PtrTypes)
{
	id0_unsigned_t i;
	id0_int_t j, k;

	i = 0;
	j = 0;
	k = prnt[c + T];

	/* search connections from leaf node to the root */

	do {
		i >>= 1;

		//
		// if node's address is odd, output 1 else output 0
		//

		if (k & 1)
			i += 0x8000;

		j++;
	} while ((k = prnt[k]) != R);

	Putcode(outfile_ptr, j, i, PtrTypes);

	code = i;
	len = j;
	update(c);
}




//---------------------------------------------------------------------------
// EncodePosition
//---------------------------------------------------------------------------
static void EncodePosition(void **outfile_ptr, id0_unsigned_t c, id0_unsigned_t PtrTypes)
{
	id0_unsigned_t i;

	//
	// output upper 6 bits with encoding
	//

	i = c >> 6;
	Putcode(outfile_ptr, p_len[i], (unsigned)p_code[i] << 8,PtrTypes);

	//
	// output lower 6 bits directly
	//

	Putcode(outfile_ptr, 6, (c & 0x3f) << 10,PtrTypes);
}




//---------------------------------------------------------------------------
// EncodeEnd
//---------------------------------------------------------------------------
static void EncodeEnd(void **outfile_ptr,id0_unsigned_t PtrTypes)
{
	if (putlen)
	{
		WritePtr(outfile_ptr,(putbuf >> 8),PtrTypes);
		codesize++;
	}
}

#endif





//===========================================================================
//
//									DECOMPRESSION ROUTINES
//
//===========================================================================



#if INCLUDE_LZH_DECOMP

//---------------------------------------------------------------------------
// GetByte
//---------------------------------------------------------------------------
static id0_int_t GetByte(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes)
{
	id0_unsigned_t i;

	while (getlen <= 8)
	{
		if (*CompressLength)
		{
			i = ReadPtr(infile_ptr,PtrTypes);
			(*CompressLength)--;
		}
		else
			i = 0;

		getbuf |= i << (8 - getlen);
		getlen += 8;
	}

	i = getbuf;
	getbuf <<= 8;
	getlen -= 8;
	return i>>8;
}






//---------------------------------------------------------------------------
// GetBit
//---------------------------------------------------------------------------
static id0_int_t GetBit(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes)	/* get one bit */
{
	id0_int_t i;

	while (getlen <= 8)
	{
		if (*CompressLength)
		{
			i = ReadPtr(infile_ptr,PtrTypes);
			(*CompressLength)--;
		}
		else
			i = 0;

		getbuf |= i << (8 - getlen);
		getlen += 8;
	}

	i = getbuf;
	getbuf <<= 1;
	getlen--;
	return (i < 0);
}





//---------------------------------------------------------------------------
// DecodeChar
//---------------------------------------------------------------------------
static id0_int_t DecodeChar(void **infile_ptr, id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes)
{
	id0_unsigned_t c;

	c = son[R];

	/*
	 * start searching tree from the root to leaves.
	 * choose node #(son[]) if input bit == 0
	 * else choose #(son[]+1) (input bit == 1)
	 */

	while (c < T)
	{
		c += GetBit(infile_ptr,CompressLength,PtrTypes);
		c = son[c];
	}

	c -= T;
	update(c);
	return c;
}





//---------------------------------------------------------------------------
// DecodePosition
//---------------------------------------------------------------------------
static id0_int_t DecodePosition(void **infile_ptr,id0_unsigned_long_t *CompressLength, id0_unsigned_t PtrTypes)
{
	id0_unsigned_t i, j, c;

	//
	// decode upper 6 bits from given table
	//

	i = GetByte(infile_ptr, CompressLength, PtrTypes);
	c = (unsigned)d_code[i] << 6;
	j = d_len[i];

	//
	// input lower 6 bits directly
	//

	j -= 2;
	while (j--)
	{
		i = (i << 1) + GetBit(infile_ptr, CompressLength, PtrTypes);
	}

	return c | i & 0x3f;
}

#endif





//===========================================================================
//
//									EXTERNAL REFERENCED
//							  COMPRESSION & DECOMPRESSION
//									     ROUTINES
//
//===========================================================================




#if INCLUDE_LZH_DECOMP

//---------------------------------------------------------------------------
// lzhDecompress()
//---------------------------------------------------------------------------
id0_long_t lzhDecompress(void id0_far *infile, void id0_far *outfile, id0_unsigned_long_t OrginalLength, id0_unsigned_long_t CompressLength, id0_unsigned_t PtrTypes)
{
	id0_int_t  i, j, k, r, c;
	id0_long_t count;

	datasize = textsize = OrginalLength;
	getbuf = 0;
	getlen = 0;

	if (textsize == 0)
		return 0;

	StartHuff();
	for (i = 0; i < N - F; i++)
		text_buf[i] = ' ';

	r = N - F;

	// REFKEEN - Looks like this is an unsigned comparison in original EXE
	for (count = 0; (id0_unsigned_long_t)count < textsize; )
	{
		c = DecodeChar(/*(id0_long_t)*/&infile,&CompressLength,PtrTypes);

		if (c < 256)
		{
			WritePtr(/*(id0_long_t)*/&outfile,c,PtrTypes);
			datasize--;								// Dec # of bytes to write

			text_buf[r++] = c;
			r &= (N - 1);
			count++;									// inc count of bytes written
		}
		else
		{
			i = (r - DecodePosition(/*(id0_long_t)*/&infile,&CompressLength,PtrTypes) - 1) & (N - 1);
			j = c - 255 + THRESHOLD;

			for (k = 0; k < j; k++)
			{
				c = text_buf[(i + k) & (N - 1)];

				WritePtr(/*(id0_long_t)*/&outfile,c,PtrTypes);
				datasize--;							// dec count of bytes to write

				text_buf[r++] = c;
				r &= (N - 1);
				count++;								// inc count of bytes written
			}
		}
		// REFKEEN - Unused
#if 0
		if (LZH_DecompressDisplayVector && (count > printcount))
		{
			LZH_DecompressDisplayVector(OrginalLength,OrginalLength-datasize);
			printcount += 1024;
		}
#endif
	}

//	printf("%12ld\n", count);

	return(count);
}

#endif





#if INCLUDE_LZH_COMP

//---------------------------------------------------------------------------
// lzhCompress()
//---------------------------------------------------------------------------
id0_long_t lzhCompress(void id0_far *infile, void id0_far *outfile,id0_unsigned_long_t DataLength,id0_unsigned_t PtrTypes)
{
	id0_int_t  i, c, len, r, s, last_match_length;

	textsize = DataLength;

	if (textsize == 0)
		return;

	getbuf = 0;
	getlen = 0;
	textsize = 0;			/* rewind and rescan */
	codesize = 0;
	datasize = 0;			// Init our counter of ReadData...
	StartHuff();
	InitTree();

	s = 0;
	r = N - F;

	for (i = s; i < r; i++)
		text_buf[i] = ' ';

	for (len = 0; len < F && (DataLength > datasize); len++)
	{
		c = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);
		datasize++;							// Dec num of bytes to compress
		text_buf[r + len] = c;
	}

	textsize = len;

	for (i = 1; i <= F; i++)
		InsertNode(r - i);

	InsertNode(r);

	do {
		if (match_length > len)
			match_length = len;

		if (match_length <= THRESHOLD)
		{
			match_length = 1;
			EncodeChar(/*(id0_long_t)*/&outfile,text_buf[r],PtrTypes);
		}
		else
		{
			EncodeChar(/*(id0_long_t)*/&outfile, 255 - THRESHOLD + match_length,PtrTypes);
			EncodePosition(/*(id0_long_t)*/&outfile, match_position,PtrTypes);
		}

		last_match_length = match_length;

		for (i = 0; i < last_match_length && (DataLength > datasize); i++)
		{
			c = ReadPtr(/*(id0_long_t)*/&infile,PtrTypes);
			datasize++;

			DeleteNode(s);
			text_buf[s] = c;

			if (s < F - 1)
				text_buf[s + N] = c;

			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			InsertNode(r);
		}
		// REFKEEN - Unused
#if 0
		if (LZH_CompressDisplayVector && ((textsize += i) > printcount))
		{
			LZH_CompressDisplayVector(DataLength,datasize);
			printcount += 1024;
		}
#endif

		while (i++ < last_match_length)
		{
			DeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			if (--len)
				InsertNode(r);
		}

	} while (len > 0);

	EncodeEnd(/*(id0_long_t)*/&outfile,PtrTypes);

	return(codesize);

}


#endif
