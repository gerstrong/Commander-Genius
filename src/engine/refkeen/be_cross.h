#ifndef __BE_CROSS_H__
#define __BE_CROSS_H__

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

// WARNING: It's actually safer to include this earlier (e.g., for endianness
// related macros), but this header may be the one included first
#include "refkeen_config.h"

typedef enum BE_Log_Message_Class_T
{
	BE_LOG_MSG_NORMAL, BE_LOG_MSG_WARNING, BE_LOG_MSG_ERROR
} BE_Log_Message_Class_T;

#define BE_Cross_Swap16(x) ((uint16_t)(((uint16_t)(x)<<8)|((uint16_t)(x)>>8)))
#define BE_Cross_Swap32(x) ((uint32_t)(((uint32_t)(x)<<24)|(((uint32_t)(x)<<8)&0x00FF0000)|(((uint32_t)(x)>>8)&0x0000FF00)|((uint32_t)(x)>>24)))

#ifdef REFKEEN_ARCH_LITTLE_ENDIAN
#define BE_Cross_Swap16LE(x) (x)
#define BE_Cross_Swap16BE(x) BE_Cross_Swap16(x)
#define BE_Cross_Swap32LE(x) (x)
#define BE_Cross_Swap32BE(x) BE_Cross_Swap32(x)
#else
#define BE_Cross_Swap16LE(x) BE_Cross_Swap16(x)
#define BE_Cross_Swap16BE(x) (x)
#define BE_Cross_Swap32LE(x) BE_Cross_Swap32(x)
#define BE_Cross_Swap32BE(x) (x)
#endif

// Used for some resource definitions internally
#ifdef REFKEEN_ARCH_LITTLE_ENDIAN
#define BE_Cross_SwapGroup16LE(a, b)       a, b,
#define BE_Cross_SwapGroup24LE(a, b, c)    a, b, c,
#define BE_Cross_SwapGroup32LE(a, b, c, d) a, b, c, d,
#else
#define BE_Cross_SwapGroup16LE(a, b)       b, a,
#define BE_Cross_SwapGroup24LE(a, b, c)    c, b, a,
#define BE_Cross_SwapGroup32LE(a, b, c, d) d, c, b, a,
#endif

inline char *BE_Cross_ultoa_dec(uint32_t n, char *buffer)
{
	sprintf(buffer, "%"PRIu32, n);
	return buffer;
}

inline char *BE_Cross_ltoa_dec(int32_t n, char *buffer)
{
	sprintf(buffer, "%"PRId32, n);
	return buffer;
}

inline char *BE_Cross_itoa_dec(int16_t n, char *buffer)
{
	sprintf(buffer, "%"PRId16, n);
	return buffer;
}

// Used for debugging
void BE_Cross_LogMessage(BE_Log_Message_Class_T msgClass, const char *format, ...);
// More (possibly semi) standard C functions emulated,
// taking English locale into account (and more, but NOT all)
inline int BE_Cross_toupper(int c)
{
	return ((c >= 'a') && (c <= 'z')) ? (c - 'a' + 'A') : c;
}
inline int BE_Cross_tolower(int c)
{
	return ((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c;
}
inline int BE_Cross_isupper(int c)
{
	return ((c >= 'A') && (c <= 'Z'));
}
int BE_Cross_strcasecmp(const char *s1, const char *s2);

/* A safe(r) string copying function that:
 * - Gets a pointer to the destination buffer's end (little performance bit).
 * - Returns the END of the written string. (This differs from stdcpy!)
 * - Is ALLOWED to write past the written string's end, if there's room
 * (e.g., any additional null terminator).
 *
 * ASSUMPTIONS: The pointers do point to valid buffers, there's enough room for
 * a null terminator, and the source and destination buffers do NOT overlap.
 */
inline char *BE_Cross_safeandfastcstringcopy(char *dest, char *destEnd, const char *src)
{
	char ch;
	// We assume that initially, destEnd - dest > 0.
	do
	{
		ch = *src++;
		*dest++ = ch; // This includes the null terminator if there's the room
	} while ((dest < destEnd) && ch);
	// These work in case dest == destEnd, and also if not
	--dest;
	*dest = '\0';
	return dest; // WARNING: This differs from strcpy!!
}

// A few convenience functions for concatenating multiple strings together
inline char *BE_Cross_safeandfastcstringcopy_2strs(char *dest, char *destEnd, const char *src0, const char *src1)
{
	return BE_Cross_safeandfastcstringcopy(BE_Cross_safeandfastcstringcopy(dest, destEnd, src0), destEnd, src1);
}

inline char *BE_Cross_safeandfastcstringcopy_3strs(char *dest, char *destEnd, const char *src0, const char *src1, const char *src2)
{
	return BE_Cross_safeandfastcstringcopy(BE_Cross_safeandfastcstringcopy(BE_Cross_safeandfastcstringcopy(dest, destEnd, src0), destEnd, src1), destEnd, src2);
}


// Often used as a replacement for file handles of type "int",
// this one is given a different name so it's easy to swap in case of a need
typedef FILE * BE_FILE_T;

inline BE_FILE_T BE_Cross_IsFileValid(BE_FILE_T fp) { return fp; }
inline int BE_Cross_seek(BE_FILE_T fp, long int offset, int origin) { return fseek(fp, offset, origin); }
inline int BE_Cross_putc(int character, BE_FILE_T fp) { return putc(character, fp); }
inline int BE_Cross_getc(BE_FILE_T fp) { return getc(fp); }

int32_t BE_Cross_FileLengthFromHandle(BE_FILE_T fp);

// Semi cross-platform file opening wrappers, hiding search paths
BE_FILE_T BE_Cross_open_for_reading(const char *filename);
BE_FILE_T BE_Cross_open_for_overwriting(const char *filename);
inline void BE_Cross_close(BE_FILE_T fp) { fclose(fp); }

// Loads a file originally embedded in the EXE (for DOS) to a newly allocated
// chunk of memory. Should be freed using BE_Cross_free_mem_loaded_file.
// Returns chunk size if successful, or a negative number in case of failure.
int BE_Cross_load_embedded_rsrc_to_mem(const char *filename, void **ptr);

// Frees file loaded using BE_Cross_load_file_to_mem. Accepts a NULL pointer.
void BE_Cross_free_mem_loaded_embedded_rsrc(void *ptr);

// Outputs a list of file names matching given name suffix from a corresponding
// "search path" (used by an implementation of gelib.c:ReadGameList), sorted
// alphabetically in a case-insensitive manner (English locale).
//
// Output is upper-case and has the suffix removed. If a filename length is
// at least strLenBound characters (excluding the suffix, only the first
// strLenBound-1 characters are stored. In addition, if there are more than
// maxNum matching files, the last maxNum entries are written.
//
// Input is assumed to point to a buffer (bidimensional array) of size
// maxNum*strLenBound;
//
// Returns: Number of filled entries.
int BE_Cross_GetSortedRewritableFilenames_AsUpperCase(char *outFilenames, int maxNum, int strLenBound, const char *suffix);

// Used preparing game installations and search paths
void BE_Cross_PrepareGameInstallations(void);
// gameVer should be BE_GAMEVER_LAST if no specific version is desired
void BE_Cross_SelectGameInstallation(int gameVerVal);

// Semi cross-platform binary (non-textual) file I/O, where it can be used directly (config file)
size_t BE_Cross_readInt8LE(BE_FILE_T fp, void *ptr);
size_t BE_Cross_readInt16LE(BE_FILE_T fp, void *ptr);
size_t BE_Cross_readInt32LE(BE_FILE_T fp, void *ptr);
size_t BE_Cross_readInt8LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte);
size_t BE_Cross_readInt16LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte);
size_t BE_Cross_readInt32LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte);
// This exists for the EGAHEADs from the Catacombs
size_t BE_Cross_readInt24LEBuffer(BE_FILE_T fp, void *ptr, size_t nbyte);
// A template for enum reading (from 16-bit little-endian int).
// A declaration and implementation must exist for each used type separately
// (implementation should be found in be_cross.c).
#if 0
size_t BE_Cross_read_EnumType_From16LE(BE_FILE_T fp, EnumType *ptr);
#endif
// boolean implementation may be separated from enums, otherwise it's the same
size_t BE_Cross_read_boolean_From16LE(BE_FILE_T fp, bool *ptr);
// booleans buffer
size_t BE_Cross_read_booleans_From16LEBuffer(BE_FILE_T fp, bool *ptr, size_t nbyte);

// Same but for writing
size_t BE_Cross_writeInt8LE(BE_FILE_T fp, const void *ptr);
size_t BE_Cross_writeInt16LE(BE_FILE_T fp, const void *ptr);
size_t BE_Cross_writeInt32LE(BE_FILE_T fp, const void *ptr);
size_t BE_Cross_writeInt8LEBuffer(BE_FILE_T fp, const void *ptr, size_t nbyte);
size_t BE_Cross_writeInt16LEBuffer(BE_FILE_T fp, const void *ptr, size_t nbyte);

#if 0
size_t BE_Cross_write_EnumType_To16LE(BE_FILE_T fp, const EnumType *ptr);
#endif

size_t BE_Cross_write_boolean_To16LE(BE_FILE_T fp, const bool *ptr);
size_t BE_Cross_write_booleans_To16LEBuffer(BE_FILE_T fp, const bool *ptr, size_t nbyte);

// Assuming segPtr is replacement for a 16-bit segment pointer, and offInSegPtr
// is a replacement for an offset in this segment (pointing to a place in the
// emulated segment), increases offInSegPtr by count with wrapping
// (if offInSegPtr-segPtr >= 0x10000 after wrapping, offInSegPtr is reduced by 0x10000)
void BE_Cross_Wrapped_Add(uint8_t *segPtr, uint8_t **offInSegPtrPtr, uint16_t count);
// Same as above but with count == 1 forced
void BE_Cross_Wrapped_Inc(uint8_t *segPtr, uint8_t **offInSegPtrPtr);
// Similar; Used for copying from linear buffer to cyclic 10000 bytes long "segment"
void BE_Cross_LinearToWrapped_MemCopy(uint8_t *segDstPtr, uint8_t *offDstPtr, const uint8_t *linearSrc, uint16_t num);
// Vice-versa
void BE_Cross_WrappedToLinear_MemCopy(uint8_t *linearDst, const uint8_t *segSrcPtr, const uint8_t *offSrcPtr, uint16_t num);
// Similar, but under a common segment.
// ASSUMPTION: Buffers do not overlap!!
void BE_Cross_WrappedToWrapped_MemCopy(uint8_t *segCommonPtr, uint8_t *offDstPtr, const uint8_t *offSrcPtr, uint16_t num);
// Wrapped memset
void BE_Cross_Wrapped_MemSet(uint8_t *segPtr, uint8_t *offInSegPtr, int value, uint16_t num);

// Used for saved game compatibility where 16-bit offset/near pointers are involved
// (temp2 field which may have a 16-bit offset pointer to an object state)
void BE_Cross_Compat_FillObjStatesWithDOSPointers(void);
// Same as above, but in case 32-bit far pointers are saved/loaded instead
// (the exact segment depends on location of DOS EXE in memory so we allow a variable shift)
void BE_Cross_Compat_FillObjStatesWithRelocatedDOSPointers(uint_fast16_t segoffset);
// Return a void* rather than statetype* only because of current header inclusion mess...
void* BE_Cross_Compat_GetObjStatePtrFromDOSPointer(uint_fast32_t dosptr);
// Returns an offset that should be added to the 16-bit segments of 32-bit
// far pointers present in The Catacomb Armageddon/Apocalypse saved games
// (in the case of the original DOS exes, it depends on the locations of
// modified copies of them in memory)
uint16_t BE_Cross_Compat_GetFarPtrRelocationSegOffset(void);

// Alternatives for Borland's randomize and random macros used in Catacomb Abyss
void BE_Cross_Brandomize(void);
int16_t BE_Cross_Brandom(int16_t num);

// Hack for Catacomb Abyss' INTRO and LOADSCN
inline int32_t BE_Mem_FarCoreLeft(void)
{
	return 500000;
}

// Use this in cases an original DOS program attempts to access contents of
// segment no. 0 for some reason
extern uint8_t g_be_cross_dosZeroSeg[];

#endif // BE_CROSS_H
