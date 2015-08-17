/* Catacomb Abyss Source Code
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

///////////////////////////////////////////////////////////////////////////
//
// Defines
//

#define MAX_TEXT_PAGES  40
#define SAVEVER_DATA "0.93"
#define FILENAME_LEN 15

// REFKEEN - Replace with variables that can be dynamically exchanged
extern const char *refkeen_compat_gelib_gamename;
extern const char *refkeen_compat_gelib_version;
extern const char *refkeen_compat_gelib_revision;
// This includes the null terminator
extern size_t refkeen_compat_gelib_gamename_strbufflen;
// A few strings that include gamename
extern char refkeen_compat_gelib_c4_debug_str_with_gamename[];
extern char refkeen_compat_gelib_str_with_gamename[];

// REFKEEN - Compatibility/Reference stuff (where possible)
#define GAMENAME refkeen_compat_gelib_gamename
#define VERSION refkeen_compat_gelib_version
#define REVISION refkeen_compat_gelib_revision

//#define BOBLIST 1                          //SP - Undefine if not using BOBList

#define AUDIO_DISK              (2)
#define VIDEO_DISK              (1)
#define LEVEL_DISK              (2)

#define BIO_BUFFER_LEN  (512)

#define TrashProg Quit

///////////////////////////////////////////////////////////////////////////
//
// Externs
//

extern id0_char_t Filename[], ID[], VER[];
extern id0_boolean_t ge_textmode;
extern id0_short_t PPT_LeftEdge,PPT_RightEdge;
//extern id0_boolean_t ConserveMemory;

// (REFKEEN) Seems unused
#if 0
typedef struct Sample {
		id0_char_t *filename;
		memptr *data;
} __attribute__((__packed__)) Sample;
#endif

typedef enum {ged_none, ged_SoundSource,ged_SoundBlaster} AudioDeviceType;

typedef struct {
	memptr textptr;
	id0_char_t id0_far *pages[MAX_TEXT_PAGES];
	id0_short_t totalpages;
} textinfo;

typedef struct {
	BE_FILE_T handle;                     // handle of file
	memptr buffer;          // pointer to buffer
	id0_word_t offset;            // offset into buffer
	id0_word_t status;            // read/write status
} BufferedIO;

typedef enum ANIMINFO {at_NONE,at_INIT,at_WAIT,at_ONCE,at_CYCLE,
							  at_REBOUND,at_EXTRA,
							  at_FWD,at_REV
} ANIMINFO;

struct BitMapHeader {
	id0_unsigned_int_t    w,h,x,y;
	id0_unsigned_char_t   d,trans,comp,pad;
} __attribute__((__packed__));

// (REFKEEN) Seems unused
#if 0
struct BitMap {
	id0_unsigned_int_t Width;
	id0_unsigned_int_t Height;
	id0_unsigned_int_t Depth;
	id0_unsigned_int_t BytesPerRow;
	id0_char_t id0_far *Planes[8];
};
#endif

struct Shape {
	memptr Data;
	id0_long_t size;
	id0_unsigned_int_t BPR;
	struct BitMapHeader bmHdr;
};

///////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
void WaitKeyVBL(id0_short_t key, id0_short_t vbls);
void CalibrateJoystick(id0_short_t joynum);
void MoveScreen(id0_short_t x, id0_short_t y);
void MoveGfxDst(id0_short_t x, id0_short_t y);
void DoPiracy(void);
void PrintPropText(const id0_char_t id0_far *text);
void DisplayText(textinfo *textinfo);
id0_long_t LoadTextFile(const id0_char_t *filename,textinfo *textinfo);
void FreeTextFile(textinfo *textinfo);
void InitTextFile(textinfo *textinfo);
id0_long_t Verify(const id0_char_t *filename);
void GE_SaveGame(void);
id0_boolean_t GE_LoadGame(void);
id0_int_t GE_HardError(id0_word_t errval,id0_int_t ax,id0_int_t bp,id0_int_t si);

#ifdef BOBLIST


id0_boolean_t UpdateBOBList(objtype *obj,struct Simple_Shape *Shape,shapeclass Class, id0_short_t priority, spriteflags sprflags);
id0_boolean_t RemoveBOBShape(objtype *obj, shapeclass Class);
void RemoveBOBList(objtype *obj);
void InitBOBList(objtype *obj, struct BOB_Shape *BOB_Shape, id0_short_t NumElements);
void RefreshBOBList(objtype *obj);
#endif


id0_unsigned_long_t BLoad(const id0_char_t *SourceFile, memptr *DstPtr);
void lzwDecompressFromRAM(id0_byte_t id0_far *SrcPtr, id0_byte_t id0_far *DstPtr, id0_longword_t SrcLen);
void lzwDecompressFromFile(BufferedIO *SrcPtr, id0_byte_t id0_far *DstPtr, id0_longword_t SrcLen);
id0_byte_t readch(BE_FILE_T handle);

memptr InitBufferedIO(BE_FILE_T handle, BufferedIO *bio);
void FreeBufferedIO(BufferedIO *bio);
id0_byte_t bio_readch(BufferedIO *bio);
void bio_fillbuffer(BufferedIO *bio);


void SwapLong(id0_long_t id0_far *Var);
void SwapWord(id0_unsigned_int_t id0_far *Var);
id0_int_t LoadShape(const id0_char_t *Filename,struct Shape *SHP);
void FreeShape(struct Shape *shape);
id0_int_t UnpackEGAShapeToScreen(struct Shape *SHP,id0_int_t startx,id0_int_t starty);
id0_char_t GetKeyChoice(const id0_char_t *choices,id0_boolean_t clear);
id0_boolean_t AnimateObj(objtype *obj);
void AdvanceAnimFWD(objtype *obj);
void AdvanceAnimREV(objtype *obj);

// (REFKEEN) Seem unused
#if 0
void LoadASArray(struct Sample *ASArray);
void FreeASArray(struct Sample *ASArray);
//void SelectDigiAudio(AudioDeviceType Device);
void PlaySample(id0_unsigned_t SampleNum);
void GE_FreeAllDigiSounds(void);
void GE_LoadAllDigiSounds(void);
#endif
void DisplayGameList(id0_short_t winx, id0_short_t winy, id0_short_t list_width, id0_short_t list_height);
void ReadGameList(void);
void CheckStack(void);
void CenterObj(objtype *obj, id0_unsigned_t x, id0_unsigned_t y);
void cachein(id0_short_t s,id0_short_t e);
void cacheout(id0_short_t s,id0_short_t e);
void FizzleFade (id0_unsigned_t source, id0_unsigned_t dest,id0_unsigned_t width,id0_unsigned_t height, id0_boolean_t abortable);
void mprintf(id0_char_t *msg, ...);
id0_boolean_t FindFile(const id0_char_t *filename,const id0_char_t *disktext,id0_char_t disknum);
void CacheAV(id0_char_t *title);
void BlackPalette(void);
void ColoredPalette(void);
