/* unlzexe.c */
//#define VERSION "0.8"

/* unlzexe ver 0.5 (PC-VAN UTJ44266 Kou )
*   UNLZEXE converts the compressed file by lzexe(ver.0.90,0.91) to the
*   UNcompressed executable one.
*
*   usage:  UNLZEXE packedfile[.EXE] [unpackedfile.EXE]

- Modified version based on v0.8.

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
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if 0
#include <dos.h>
#ifdef __TURBOC__
#include <dir.h>
/*
#define MAXPATH   80
#define MAXDRIVE  3
#define MAXDIR	  66
#define MAXFILE   9
#define MAXEXT	  5
#define FILENAME_MAX MAXPATH
*/
#endif
#endif

#include "be_cross.h"

#define FAILURE 1
#define SUCCESS 0

typedef uint16_t WORD;
typedef uint8_t BYTE;

static int rdhead(FILE *,int *);
static int mkreltbl(FILE *,unsigned char *,int);
static int unpack(FILE *,unsigned char *);
static void wrhead(unsigned char *);

bool Unlzexe_unpack(FILE *ifile, unsigned char *obuff, int buffsize)
{
	int ver;
	if (rdhead(ifile,&ver) != SUCCESS)
		return false;
	if (mkreltbl(ifile,obuff,ver) != SUCCESS)
		return false;
	if (unpack(ifile,obuff) != SUCCESS)
		return false;
	wrhead(obuff);
	return true;
}

#if 0
int isjapan(void);
int japan_f;
#define iskanji(c)	('\x81'<=(c)&&(c)<='\x9f' || '\xe0'<=(c)&&(c)<='\xfc')

char *tmpfname = "$tmpfil$.exe";
char *backup_ext = ".olz";
char ipath[FILENAME_MAX],
     opath[FILENAME_MAX],
     ofname[13];

#ifdef __TURBOC__
/* I like prototypes */
int reloc90(FILE *ifile,FILE *ofile,long fpos);
int reloc91(FILE *ifile,FILE *ofile,long fpos);
#endif

void main(int argc,char **argv){
    int fnamechk(char*,char*,char*,int,char**);
    int fnamechg(char*,char*,char*,int);
    int rdhead(FILE *,int *);
    int mkreltbl(FILE *,FILE *,int);
    int unpack(FILE *,FILE *);
    void wrhead(FILE *);

    FILE *ifile,*ofile;
    int  ver,rename_sw=0;

	printf("UNLZEXE Ver. "VERSION"\n");             /* v0.6 */
	japan_f=isjapan();
	if(argc!=3 && argc!=2){
	printf("usage: UNLZEXE packedfile [unpackedfile]\n");
	exit(EXIT_FAILURE);
    }
    if(argc==2)
	rename_sw=1;
    if(fnamechk(ipath,opath,ofname,argc,argv)!=SUCCESS) {
	exit(EXIT_FAILURE);
    }
    if((ifile=fopen(ipath,"rb"))==NULL){
	printf("'%s' :not found\n",ipath);
	    exit(EXIT_FAILURE);
    }

    if(rdhead(ifile,&ver)!=SUCCESS){
	printf("'%s' is not LZEXE file.\n",ipath);
	fclose(ifile); exit(EXIT_FAILURE);
    }
    if((ofile=fopen(opath,"w+b"))==NULL){
	printf("can't open '%s'.\n",opath);
	fclose(ifile); exit(EXIT_FAILURE);
    }
    printf("file '%s' is compressed by LZEXE Ver. 0.%d\n",ipath,ver); /* v0.8 */
/*  switch(ver){
 *  case 90: printf("0.90\n"); break;
 *  case 91: printf("0.91\n"); break;
 *  }
 */
    if(mkreltbl(ifile,ofile,ver)!=SUCCESS) {
	fclose(ifile);
	fclose(ofile);
	remove(opath);
	exit(EXIT_FAILURE);
    }
    if(unpack(ifile,ofile)!=SUCCESS) {
	fclose(ifile);
	fclose(ofile);
	remove(opath);
	exit(EXIT_FAILURE);
    }
    fclose(ifile);
    wrhead(ofile);
    fclose(ofile);

    if(fnamechg(ipath,opath,ofname,rename_sw)!=SUCCESS){
	exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}




void parsepath(char *pathname, int *fname, int *ext);

/* file name check */
int fnamechk(char *ipath,char *opath, char *ofname,
	      int argc,char **argv) {
    int idx_name,idx_ext;

    strcpy(ipath,argv[1]);
    parsepath(ipath,&idx_name,&idx_ext);
    if (! ipath[idx_ext]) strcpy(ipath+idx_ext,".exe");
    if(! stricmp(ipath+idx_name,tmpfname)){
	printf("'%s':bad filename.\n",ipath);
	return(FAILURE);
    }
    if(argc==2)
	strcpy(opath,ipath);
    else
	strcpy(opath,argv[2]);
    parsepath(opath,&idx_name,&idx_ext);
    if (! opath[idx_ext]) strcpy(opath+idx_ext,".exe");
    if (!stricmp(opath+idx_ext,backup_ext)){
	printf("'%s':bad filename.\n",opath);
	return(FAILURE);
    }
    strncpy(ofname,opath+idx_name,12);
    strcpy(opath+idx_name,tmpfname);
    return(SUCCESS);
}


int fnamechg(char *ipath,char *opath,char *ofname,int rename_sw) {
    int idx_name,idx_ext;
    char tpath[FILENAME_MAX];

    if(rename_sw) {
	strcpy(tpath,ipath);
	parsepath(tpath,&idx_name,&idx_ext);
	strcpy(tpath+idx_ext,backup_ext);
	remove(tpath);
	if(rename(ipath,tpath)){
	    printf("can't make '%s'.\n", tpath);
	    remove(opath);
	    return(FAILURE);
	}
	printf("'%s' is renamed to '%s'.\n",ipath,tpath);
    }
    strcpy(tpath,opath);
    parsepath(tpath,&idx_name,&idx_ext);
    strcpy(tpath+idx_name,ofname);
    remove(tpath);
    if(rename(opath,tpath)){
	if(rename_sw) {
	    strcpy(tpath,ipath);
	    parsepath(tpath,&idx_name,&idx_ext);
	    strcpy(tpath+idx_ext,backup_ext);
	    rename(tpath,ipath);
	}
	printf("can't make '%s'.  unpacked file '%s' is remained.\n",
		 tpath, tmpfname);

	return(FAILURE);
    }
    printf("unpacked file '%s' is generated.\n",tpath);
    return(SUCCESS);
}

int isjapan() {
    union REGS r;
    struct SREGS rs;
    BYTE buf[34];

    segread(&rs);
    rs.ds=rs.ss;  r.x.dx=(WORD)buf;
/*	r.h.al=0x3800; v0.6 */
	r.x.ax=0x3800;		/* svc 38H, check for country v0.6 */
    intdosx(&r,&r,&rs);
    return (! strcmp ((char *) (buf + 2), "\\"));       /* v0.8 */
}

void parsepath(char *pathname, int *fname, int *ext) {
    /* use  int japan_f */
    char c;
    int i;

    *fname=0; *ext=0;
    for(i=0;c=pathname[i];i++) {
		if(japan_f && iskanji(c))
	    i++;
	else
			switch(c) {
	    case ':' :
	    case '\\':  *fname=i+1; break;
	    case '.' :  *ext=i; break;
	    default  :	;
	    }
    }
    if(*ext<=*fname) *ext=i;
}
#endif

/*-------------------------------------------*/
static WORD ihead[0x10],ohead[0x10],inf[8];
static long loadsize;
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
static int rdhead(FILE *ifile ,int *ver){
    long entry; 	/* v0.8 */
/* v0.7 old code */
/*  if(BE_Cross_readInt16LEBuffer(ifile,ihead,0x10*sizeof(ihead[0]))!=0x10)
 *  //if(fread(ihead,sizeof ihead[0],0x10,ifile)!=0x10)
 *	return FAILURE;
 *  memcpy(ohead,ihead,sizeof ihead[0] * 0x10);
 *  if(ihead[0]!=0x5a4d || ihead[4]!=2 || ihead[0x0d]!=0)
 *	return FAILURE;
 *  if(ihead[0x0c]==0x1c && memcmp(&ihead[0x0e],"LZ09",4)==0){
 *	*ver=90; return SUCCESS ;
 *  }
 *
 *  if(ihead[0x0c]==0x1c && memcmp(&ihead[0x0e],"LZ91",4)==0){
 *	*ver=91; return SUCCESS ;
 *  }
 */
    if(BE_Cross_readInt16LEBuffer(ifile,ihead,sizeof(ihead))!=sizeof(ihead))
    //if (fread (ihead, 1, sizeof ihead, ifile) != sizeof ihead)	     /* v0.8 */
	return FAILURE; 					     /* v0.8 */
    memcpy (ohead, ihead, sizeof ohead);			     /* v0.8 */
    if((ihead [0] != 0x5a4d && ihead [0] != 0x4d5a) ||		     /* v0.8 */
       ihead [0x0d] != 0 || ihead [0x0c] != 0x1c)		     /* v0.8 */
	return FAILURE; 					     /* v0.8 */
    entry = ((long) (ihead [4] + ihead[0x0b]) << 4) + ihead[0x0a];   /* v0.8 */
    if (fseek (ifile, entry, SEEK_SET) != 0)			     /* v0.8 */
	return FAILURE; 					     /* v0.8 */
    if (fread (sigbuf, 1, sizeof sigbuf, ifile) != sizeof sigbuf)    /* v0.8 */
	return FAILURE; 					     /* v0.8 */
    if (memcmp (sigbuf, sig90, sizeof sigbuf) == 0) {		     /* v0.8 */
	*ver = 90;						     /* v0.8 */
	return SUCCESS; 					     /* v0.8 */
    }								     /* v0.8 */
    if (memcmp (sigbuf, sig91, sizeof sigbuf) == 0) {		     /* v0.8 */
	*ver = 91;						     /* v0.8 */
	return SUCCESS; 					     /* v0.8 */
    }								     /* v0.8 */
    return FAILURE;
}

   static int reloc90(FILE *ifile,unsigned char **obuffptrptr,long fpos);
   static int reloc91(FILE *ifile,unsigned char **obuffptrptr,long fpos);

/* make relocation table */
static int mkreltbl(FILE *ifile,unsigned char *obuff,int ver) {
    long fpos;
    int i;
    unsigned char *obuffptr;

/* v0.7 old code
 *  allocsize=((ihead[1]+16-1)>>4) + ((ihead[2]-1)<<5) - ihead[4] + ihead[5];
 */
    fpos=(long)(ihead[0x0b]+ihead[4])<<4;		/* goto CS:0000 */
    fseek(ifile,fpos,SEEK_SET);
    BE_Cross_readInt16LEBuffer(ifile, inf, sizeof(inf));
    //fread(inf, sizeof inf[0], 0x08, ifile);
    ohead[0x0a]=inf[0]; 	/* IP */
    ohead[0x0b]=inf[1]; 	/* CS */
    ohead[0x08]=inf[2]; 	/* SP */
    ohead[0x07]=inf[3]; 	/* SS */
    /* inf[4]:size of compressed load module (PARAGRAPH)*/
    /* inf[5]:increase of load module size (PARAGRAPH)*/
    /* inf[6]:size of decompressor with  compressed relocation table (BYTE) */
    /* inf[7]:check sum of decompresser with compressd relocation table(Ver.0.90) */
    ohead[0x0c]=0x1c;		/* start position of relocation table */
    obuffptr=obuff+0x1c;
    //fseek(ofile,0x1cL,SEEK_SET);
    switch(ver){
    case 90: i=reloc90(ifile,&obuffptr,fpos);
	     break;
    case 91: i=reloc91(ifile,&obuffptr,fpos);
	     break;
    default: i=FAILURE; break;
    }
    if(i!=SUCCESS){
	BE_Cross_LogMessage(BE_LOG_MSG_ERROR, "mkreltbl (unlzexe) - error at relocation table.\n");
	return (FAILURE);
    }

    fpos=obuffptr-obuff;
    //fpos=ftell(ofile);
/* v0.7 old code
 *  i= (int) fpos & 0x1ff;
 *  if(i) i=0x200-i;
 *  ohead[4]= (int) (fpos+i)>>4;
 */
    i= (0x200 - (int) fpos) & 0x1ff;	/* v0.7 */
    ohead[4]= (int) ((fpos+i)>>4);	/* v0.7 */

    memset(obuffptr, 0, i);
//    for( ; i>0; i--)
//	putc(0, ofile);
    return(SUCCESS);
}
/* for LZEXE ver 0.90 */
static int reloc90(FILE *ifile,unsigned char **obuffptrptr,long fpos) {
    uint16_t c;
    WORD rel_count=0;
    WORD rel_seg/*,rel_off*/;

    fseek(ifile,fpos+0x19d,SEEK_SET);
				/* 0x19d=compressed relocation table address */
    rel_seg=0;
    do{
	if(feof(ifile) || ferror(ifile)/* || ferror(ofile)*/) return(FAILURE);
        BE_Cross_readInt16LE(ifile, &c);
	//c=getw(ifile);
	for(;c>0;c--) {
            // Preserve original endianness (Little-Endian)
            *(*obuffptrptr)++ = getc(ifile);
            *(*obuffptrptr)++ = getc(ifile);
            *(*obuffptrptr)++ = rel_seg&0xFF;
            *(*obuffptrptr)++ = rel_seg>>8;
	    //rel_off=getw(ifile);
	    //putw(rel_off,ofile);
	    //putw(rel_seg,ofile);
	    rel_count++;
	}
	rel_seg += 0x1000;
    } while(rel_seg!=(0xf000+0x1000));
    ohead[3]=rel_count;
    return(SUCCESS);
}
/* for LZEXE ver 0.91*/
static int reloc91(FILE *ifile,unsigned char **obuffptrptr,long fpos) {
    WORD span;
    WORD rel_count=0;
    WORD rel_seg,rel_off;

    fseek(ifile,fpos+0x158,SEEK_SET);
				/* 0x158=compressed relocation table address */
    rel_off=0; rel_seg=0;
    for(;;) {
	if(feof(ifile) || ferror(ifile)/* || ferror(ofile)*/) return(FAILURE);
	if((span=getc(ifile))==0) {
            BE_Cross_readInt16LE(ifile, &span);
	    //span=getw(ifile);
	    if(span==0){
		rel_seg += 0x0fff;
		continue;
	    } else if(span==1){
		break;
	    }
	}
	rel_off += span;
	rel_seg += (rel_off & ~0x0f)>>4;
	rel_off &= 0x0f;
        // Preserve original endianness (Little-Endian)
        *(*obuffptrptr)++ = rel_off&0xFF;
        *(*obuffptrptr)++ = rel_off>>8;
        *(*obuffptrptr)++ = rel_seg&0xFF;
        *(*obuffptrptr)++ = rel_seg>>8;
	//putw(rel_off,ofile);
	//putw(rel_seg,ofile);
	rel_count++;
    }
    ohead[3]=rel_count;
    return(SUCCESS);
}

/*---------------------*/
typedef struct {
	FILE  *fp;
	WORD  buf;
	BYTE  count;
    } bitstream;

static void initbits(bitstream *,FILE *);
static int getbit(bitstream *);

/*---------------------*/
/* decompressor routine */
static int unpack(FILE *ifile,unsigned char *obuff){
    int len;
    int16_t span;
    long fpos;
    bitstream bits;
    static BYTE data[0x4500], *p=data;

    fpos=((long)ihead[0x0b]-(long)inf[4]+(long)ihead[4])<<4;
    fseek(ifile,fpos,SEEK_SET);
    fpos=(long)ohead[4]<<4;
    unsigned char *obuffptr = obuff+fpos;
    //fseek(ofile,fpos,SEEK_SET);
    initbits(&bits,ifile);
    BE_Cross_LogMessage(BE_LOG_MSG_NORMAL, "unpack (unlzexe) - unpacking...\n");
    //printf(" unpacking. ");
    for(;;){
        if(ferror(ifile)) { BE_Cross_LogMessage(BE_LOG_MSG_ERROR, "unpack (unlzexe) - read error\n"); return(FAILURE); }
	//if(ferror(ifile)) {printf("\nread error\n"); return(FAILURE); }
	//if(ferror(ofile)) {printf("\nwrite error\n"); return(FAILURE); }
	if(p-data>0x4000){
	    memcpy(obuffptr,data,0x2000);
	    obuffptr += 0x2000;
	    //fwrite(data,sizeof data[0],0x2000,ofile);
	    p-=0x2000;
	    memmove(data,data+0x2000,p-data);
	    //memcpy(data,data+0x2000,p-data);
	    //putchar('.');
	}
	if(getbit(&bits)) {
	    *p++=getc(ifile);
	    continue;
	}
	if(!getbit(&bits)) {
	    len=getbit(&bits)<<1;
	    len |= getbit(&bits);
	    len += 2;
	    span=getc(ifile) | 0xff00;
	} else {
	    span=(BYTE)getc(ifile);
	    len=getc(ifile);
	    span |= ((len & ~0x07)<<5) | 0xe000;
	    len = (len & 0x07)+2;
	    if (len==2) {
		len=getc(ifile);

		if(len==0)
		    break;    /* end mark of compreesed load module */

		if(len==1)
		    continue; /* segment change */
		else
		    len++;
	    }
	}
	for( ;len>0;len--,p++){
	    *p=*(p+span);
	}
    }
    if(p!=data)
	//fwrite(data,sizeof data[0],p-data,ofile);
    {
        memcpy(obuffptr,data,p-data);
        obuffptr += (p-data);
    }
    loadsize = (obuffptr-obuff)-fpos;
    //loadsize=ftell(ofile)-fpos;
    BE_Cross_LogMessage(BE_LOG_MSG_NORMAL, "unpack (unlzexe) - end\n");
    //printf("end\n");
    return(SUCCESS);
}

/* write EXE header*/
static void wrhead(unsigned char *obuff) {
    if(ihead[6]!=0) {
	ohead[5]-= inf[5] + ((inf[6]+16-1)>>4) + 9;	/* v0.7 */
	if(ihead[6]!=0xffff)
	    ohead[6]-=(ihead[5]-ohead[5]);
    }
    ohead[1]=((WORD)loadsize+(ohead[4]<<4)) & 0x1ff;	/* v0.7 */
    ohead[2]=(WORD)((loadsize+((long)ohead[4]<<4)+0x1ff) >> 9); /* v0.7 */
#ifdef REFKEEN_ARCH_BIG_ENDIAN
    for (int i = 0; i < 0x0e; ++i)
    {
        *obuff++ = ohead[i]&0xFF;
        *obuff++ = (ohead[i]>>8);
    }
#else
    memcpy(obuff,ohead,sizeof ohead[0] * 0x0e);
#endif
    //fseek(ofile,0L,SEEK_SET);
    //fwrite(ohead,sizeof ohead[0],0x0e,ofile);
}


/*-------------------------------------------*/

/* get compress information bit by bit */
static void initbits(bitstream *p,FILE *filep){
    p->fp=filep;
    p->count=0x10;
    p->buf=getc(filep);
    p->buf|=(getc(filep)<<8);
    //p->buf=getw(filep);
    /* printf("%04x ",p->buf); */
}

static int getbit(bitstream *p) {
    int b;
    b = p->buf & 1;
    if(--p->count == 0){
        (p->buf)=getc(p->fp);
        (p->buf)|=(getc(p->fp)<<8);
	//(p->buf)=getw(p->fp);
	/* printf("%04x ",p->buf); */
	p->count= 0x10;
    }else
	p->buf >>= 1;

    return b;
}
