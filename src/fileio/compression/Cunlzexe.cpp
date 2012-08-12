/*
 * Cunlzexe.cpp
 *
 *  Created on: 29.12.2009
 *      Author: gerstrong
 *
 *  This modified version is mainly C++ Code and attemps
 *  to store the uncompressed data in the memory first.*
 */

#include "Cunlzexe.h"
/* unlzexe ver 0.5 (PC-VAN UTJ44266 Kou )
*   UNLZEXE converts the compressed file by lzexe(ver.0.90,0.91) to the
*   UNcompressed executable one.
*
*   usage:  UNLZEXE packedfile[.EXE] [unpackedfile.EXE]

v0.6  David Kirschbaum, Toad Hall, kirsch@usasoc.soc.mil, Jul 91
	Problem reported by T.Salmi (ts@uwasa.fi) with UNLZEXE when run
	with TLB-V119 on 386's.
	Stripping out the iskanji and isjapan() stuff (which uses a somewhat
	unusual DOS interrupt) to see if that's what's biting us.

--  Found it, thanks to Dan Lewis (DLEWIS@SCUACC.SCU.EDU).
	Silly us:  didn't notice the "r.h.al=0x3800;" in isjapan().
	Oh, you don't see it either?  INT functions are called with AH
	having the service.  Changing to "r.x.ax=0x3800;".

v0.7  Alan Modra, amodra@sirius.ucs.adelaide.edu.au, Nov 91
    Fixed problem with large files by casting ihead components to long
    in various expressions.
    Fixed MinBSS & MaxBSS calculation (ohead[5], ohead[6]).  Now UNLZEXE
    followed by LZEXE should give the original file.

v0.8  Vesselin Bontchev, bontchev@fbihh.informatik.uni-hamburg.de, Aug 92
    Fixed recognition of EXE files - both 'MZ' and 'ZM' in the header
    are recognized.
    Recognition of compressed files made more robust - now just
    patching the 'LZ90' and 'LZ91' strings will not fool the program.

v0.81 Embedded Version for CG by gerstrong. This version only does
	operations in the memory as the file is already read, and can be used
	straight forward. Stripped down a lot of obsolete old DOS Code
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#define FAILURE 1
#define SUCCESS 0

Cunlzexe::Cunlzexe() :
m_headersize(0)
{}

WORD_16BIT Cunlzexe::get16bitWord(BYTE *p_data)
{
    WORD_16BIT value = (WORD_16BIT) *(p_data);
    value+= ((WORD_16BIT) *(p_data+1))<<8;
    return value;
}

void Cunlzexe::put16bitWord(WORD_16BIT value, std::vector<BYTE> &outdata)
{
	outdata.push_back( value&0xFF );
	outdata.push_back( value>>8 );
}

bool Cunlzexe::decompress(BYTE *data, std::vector<BYTE> &outdata)
{

    int ver=0;


    if(rdhead(data,&ver)!=SUCCESS)
    {
    	return false;
    }

    if(mkreltbl(data, outdata, ver)!=SUCCESS)
    {
        return false;
    }

    m_headersize = outdata.size();

    if(unpack(data, outdata)!=SUCCESS)
    {
        return false;
    }
    wrhead(outdata);

    return true;
}

/*-------------------------------------------*/
static WORD_16BIT ihead[0x10],ohead[0x10],inf[8];
static long loadsize=0;
static BYTE sig90 [] = {			/* v0.8 */
    0x06, 0x0E, 0x1F, 0x8B, 0x0E, 0x0C, 0x00, 0x8B,
    0xF1, 0x4E, 0x89, 0xF7, 0x8C, 0xDB, 0x03, 0x1E,
    0x0A, 0x00, 0x8E, 0xC3, 0xB4, 0x00, 0x31, 0xED,
    0xFD, 0xAC, 0x01, 0xC5, 0xAA, 0xE2, 0xFA, 0x8B,
    0x16, 0x0E, 0x00, 0x8A, 0xC2, 0x29, 0xC5, 0x8A,
    0xC6, 0x29, 0xC5, 0x39, 0xD5, 0x74, 0x0C, 0xBA,
    0x91, 0x01, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0xFF,
    0x4C, 0xCD, 0x21, 0x53, 0xB8, 0x53, 0x00, 0x50,
    0xCB, 0x2E, 0x8B, 0x2E, 0x08, 0x00, 0x8C, 0xDA,
    0x89, 0xE8, 0x3D, 0x00, 0x10, 0x76, 0x03, 0xB8,
    0x00, 0x10, 0x29, 0xC5, 0x29, 0xC2, 0x29, 0xC3,
    0x8E, 0xDA, 0x8E, 0xC3, 0xB1, 0x03, 0xD3, 0xE0,
    0x89, 0xC1, 0xD1, 0xE0, 0x48, 0x48, 0x8B, 0xF0,
    0x8B, 0xF8, 0xF3, 0xA5, 0x09, 0xED, 0x75, 0xD8,
    0xFC, 0x8E, 0xC2, 0x8E, 0xDB, 0x31, 0xF6, 0x31,
    0xFF, 0xBA, 0x10, 0x00, 0xAD, 0x89, 0xC5, 0xD1,
    0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5, 0xB2,
    0x10, 0x73, 0x03, 0xA4, 0xEB, 0xF1, 0x31, 0xC9,
    0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
    0xB2, 0x10, 0x72, 0x22, 0xD1, 0xED, 0x4A, 0x75,
    0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0xD1, 0xD1,
    0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
    0xB2, 0x10, 0xD1, 0xD1, 0x41, 0x41, 0xAC, 0xB7,
    0xFF, 0x8A, 0xD8, 0xE9, 0x13, 0x00, 0xAD, 0x8B,
    0xD8, 0xB1, 0x03, 0xD2, 0xEF, 0x80, 0xCF, 0xE0,
    0x80, 0xE4, 0x07, 0x74, 0x0C, 0x88, 0xE1, 0x41,
    0x41, 0x26, 0x8A, 0x01, 0xAA, 0xE2, 0xFA, 0xEB,
    0xA6, 0xAC, 0x08, 0xC0, 0x74, 0x40, 0x3C, 0x01,
    0x74, 0x05, 0x88, 0xC1, 0x41, 0xEB, 0xEA, 0x89
}, sig91 [] = {
    0x06, 0x0E, 0x1F, 0x8B, 0x0E, 0x0C, 0x00, 0x8B,
    0xF1, 0x4E, 0x89, 0xF7, 0x8C, 0xDB, 0x03, 0x1E,
    0x0A, 0x00, 0x8E, 0xC3, 0xFD, 0xF3, 0xA4, 0x53,
    0xB8, 0x2B, 0x00, 0x50, 0xCB, 0x2E, 0x8B, 0x2E,
    0x08, 0x00, 0x8C, 0xDA, 0x89, 0xE8, 0x3D, 0x00,
    0x10, 0x76, 0x03, 0xB8, 0x00, 0x10, 0x29, 0xC5,
    0x29, 0xC2, 0x29, 0xC3, 0x8E, 0xDA, 0x8E, 0xC3,
    0xB1, 0x03, 0xD3, 0xE0, 0x89, 0xC1, 0xD1, 0xE0,
    0x48, 0x48, 0x8B, 0xF0, 0x8B, 0xF8, 0xF3, 0xA5,
    0x09, 0xED, 0x75, 0xD8, 0xFC, 0x8E, 0xC2, 0x8E,
    0xDB, 0x31, 0xF6, 0x31, 0xFF, 0xBA, 0x10, 0x00,
    0xAD, 0x89, 0xC5, 0xD1, 0xED, 0x4A, 0x75, 0x05,
    0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x73, 0x03, 0xA4,
    0xEB, 0xF1, 0x31, 0xC9, 0xD1, 0xED, 0x4A, 0x75,
    0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x72, 0x22,
    0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
    0xB2, 0x10, 0xD1, 0xD1, 0xD1, 0xED, 0x4A, 0x75,
    0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0xD1, 0xD1,
    0x41, 0x41, 0xAC, 0xB7, 0xFF, 0x8A, 0xD8, 0xE9,
    0x13, 0x00, 0xAD, 0x8B, 0xD8, 0xB1, 0x03, 0xD2,
    0xEF, 0x80, 0xCF, 0xE0, 0x80, 0xE4, 0x07, 0x74,
    0x0C, 0x88, 0xE1, 0x41, 0x41, 0x26, 0x8A, 0x01,
    0xAA, 0xE2, 0xFA, 0xEB, 0xA6, 0xAC, 0x08, 0xC0,
    0x74, 0x34, 0x3C, 0x01, 0x74, 0x05, 0x88, 0xC1,
    0x41, 0xEB, 0xEA, 0x89, 0xFB, 0x83, 0xE7, 0x0F,
    0x81, 0xC7, 0x00, 0x20, 0xB1, 0x04, 0xD3, 0xEB,
    0x8C, 0xC0, 0x01, 0xD8, 0x2D, 0x00, 0x02, 0x8E,
    0xC0, 0x89, 0xF3, 0x83, 0xE6, 0x0F, 0xD3, 0xEB,
    0x8C, 0xD8, 0x01, 0xD8, 0x8E, 0xD8, 0xE9, 0x72
}, sigbuf [sizeof sig90];

/* EXE header test (is it LZEXE file?) */
int Cunlzexe::rdhead(BYTE *data_ptr ,int *ver)
{
    long entry;

    memcpy(ihead, data_ptr, sizeof ihead);
    memcpy (ohead, ihead, sizeof ohead);
    if((ihead [0] != 0x5a4d && ihead [0] != 0x4d5a) ||
    		ihead [0x0d] != 0 || ihead [0x0c] != 0x1c)
    	return FAILURE;
    entry = ((long) (ihead [4] + ihead[0x0b]) << 4) + ihead[0x0a];

    memcpy(sigbuf, &data_ptr[entry], sizeof sigbuf);
    if (memcmp (sigbuf, sig90, sizeof sigbuf) == 0) {
    	*ver = 90;
    	return SUCCESS;
    }
    if (memcmp (sigbuf, sig91, sizeof sigbuf) == 0) {
    	*ver = 91;
    	return SUCCESS;
    }
    return FAILURE;
}

/* make relocation table */
int Cunlzexe::mkreltbl(BYTE *p_data, std::vector<BYTE> &outdata,int ver)
{
    long fpos;
    int i;

    fpos=(long)(ihead[0x0b]+ihead[4])<<4;		/* goto CS:0000 */
    memcpy(inf, &p_data[fpos],  (sizeof inf[0]) *  0x08);
    ohead[0x0a]=inf[0];		/* IP */
    ohead[0x0b]=inf[1];		/* CS */
    ohead[0x08]=inf[2];		/* SP */
    ohead[0x07]=inf[3];		/* SS */
    /* inf[4]:size of compressed load module (PARAGRAPH)*/
    /* inf[5]:increase of load module size (PARAGRAPH)*/
    /* inf[6]:size of decompressor with  compressed relocation table (BYTE) */
    /* inf[7]:check sum of decompresser with compressd relocation table(Ver.0.90) */
    ohead[0x0c]=0x1c;		/* start position of relocation table */
    outdata.assign(0x1cL,0);

    switch(ver)
    {
    case 90: i=reloc90(p_data, outdata, fpos);
             break;
    case 91: i=reloc91(p_data, outdata, fpos);
             break;
    default: i=FAILURE; break;
    }

    if(i!=SUCCESS)
    {
        printf("error at relocation table.\n");
        return (FAILURE);
    }

    fpos = outdata.size();
    i = (0x200 - (int) fpos) & 0x1ff;
    ohead[4] = (int) ((fpos+i)>>4);

    for( ; i>0; i--)
    	outdata.push_back(0);

    return(SUCCESS);
}

/* for LZEXE ver 0.90 */
int Cunlzexe::reloc90(BYTE *p_data, std::vector<BYTE> &outdata, long fpos)
{
    unsigned int c;
    WORD_16BIT rel_count=0;
    WORD_16BIT rel_seg,rel_off;

     p_data += fpos+0x19d;

    rel_seg=0;
    do
    {
        c=get16bitWord(p_data);
        p_data += 2;
        for(;c>0;c--) {
            rel_off=get16bitWord(p_data);
            p_data += 2;

            put16bitWord(rel_off, outdata);
            put16bitWord(rel_seg, outdata);
            rel_count++;
        }
        rel_seg += 0x1000;
    } while(rel_seg != static_cast<WORD_16BIT>(0xf000+0x1000));
    ohead[3]=rel_count;
    return(SUCCESS);
}
/* for LZEXE ver 0.91*/
int Cunlzexe::reloc91(BYTE *p_data, std::vector<BYTE> &outdata, long fpos)
{
    WORD_16BIT span;
    WORD_16BIT rel_count=0;
    WORD_16BIT rel_seg, rel_off;

    p_data += fpos+0x158;
    				/* 0x158=compressed relocation table address */
    WORD_16BIT temp;
    rel_off=0; rel_seg=0;
    for(;;) {
    	temp = *p_data;
    	p_data++;
        if((span=temp)==0)
        {
            span = (BYTE) *p_data;
            p_data++;
            span+= ((BYTE) *p_data)<<8;
            p_data++;
            if(span==0)
            {
                rel_seg += 0x0fff;
                continue;
            }
            else if(span==1)
            {
                break;
            }
        }
        rel_off += span;
        rel_seg += (rel_off & ~0x0f)>>4;
        rel_off &= 0x0f;
        put16bitWord(rel_off, outdata);
        put16bitWord(rel_seg, outdata);
        rel_count++;
    }
    ohead[3]=rel_count;
    return(SUCCESS);
}

/*-----------------------*/
/* decompression routine */
int Cunlzexe::unpack(BYTE *p_input, std::vector<BYTE> &outdata)
{
    int len;
    short span;
    bitstream bits;
    //static BYTE data[0x4500], *p=data;
    BYTE data[0x4500], *p=data;
    unsigned long inpos;
    unsigned long outpos;
    unsigned long initial_outpos;

    inpos = ((long)ihead[0x0b]-(long)inf[4]+(long)ihead[4])<<4;
    initial_outpos = outpos = (long)ohead[4]<<4;
    initbits(&bits, p_input, inpos);
    printf(" unpacking.");
    for(;;)
    {
        if(p-data>0x4000)
        {
        	if(outdata.size() < 0x2000+outpos)
        		outdata.resize(0x2000+outpos);
        	memcpy(&outdata[0]+outpos, data, (sizeof data[0])*0x2000);
        	outpos += 0x2000;
            p-=0x2000;
            //memcpy(data,data+0x2000,p-data);
            memmove(data,data+0x2000,p-data);
            putchar('.');
        }
        if(getbit(&bits, inpos))
        {
        	*p++=p_input[inpos];
        	inpos++;
            continue;
        }
        if(!getbit(&bits, inpos))
        {
            len=getbit(&bits, inpos)<<1;
            len |= getbit(&bits, inpos);
            len += 2;
            span = p_input[inpos] | 0xff00;
            inpos++;
        } else
        {
        	span = p_input[inpos];
        	inpos++;
        	len = p_input[inpos];
        	inpos++;
            span |= ((len & ~0x07)<<5) | 0xe000;
            len = (len & 0x07)+2;
            if (len==2)
            {
                len = p_input[inpos];
                inpos++;

                if(len==0)
                    break;    /* end mark of compressed load module */

                if(len==1)
                    continue; /* segment change */
                else
                    len++;
            }
        }
        for( ;len>0;len--,p++)
        {
            *p=*(p+span);
        }
    }
    if(p!=data)
    {
    	long size = (sizeof data[0])*(p-data);
    	unsigned long vec_size = outdata.size();
    	if(vec_size < size+outpos)
    		outdata.resize(size+outpos);
        memcpy(&outdata[0]+outpos, data, size);
        outpos += size;
    }
    loadsize=outpos-initial_outpos;
    printf("end\n");
    return(SUCCESS);
}

/* write EXE header*/
void Cunlzexe::wrhead(std::vector<BYTE> &outdata) {
    if(ihead[6]!=0) {
	ohead[5]-= inf[5] + ((inf[6]+16-1)>>4) + 9;	/* v0.7 */
        if(ihead[6]!=0xffff)
            ohead[6]-=(ihead[5]-ohead[5]);
    }
    ohead[1]=((WORD_16BIT)loadsize+(ohead[4]<<4)) & 0x1ff;	/* v0.7 */
    ohead[2]=(WORD_16BIT)((loadsize+((long)ohead[4]<<4)+0x1ff) >> 9); /* v0.7 */

    memcpy(&outdata[0], ohead, (sizeof ohead[0])*0x0e );
}

/*-------------------------------------------*/

/* get compress information bit by bit */
void Cunlzexe::initbits(bitstream *p, BYTE *p_data, unsigned long &inpos){
	p->pdata = p_data;
    p->count=0x10;
    p->buf=get16bitWord(&p_data[inpos]);
    inpos += 2;
}

int Cunlzexe::getbit(bitstream *p, unsigned long &inpos) {
    int b;
    b = p->buf & 1;
    if(--p->count == 0){
    	(p->buf) = get16bitWord(&(p->pdata[inpos]));
    	inpos += 2;
        p->count= 0x10;
    }else
        p->buf >>= 1;

    return b;
}

unsigned long Cunlzexe::HeaderSize()
{
	return m_headersize;
}

Cunlzexe::~Cunlzexe()
{}
