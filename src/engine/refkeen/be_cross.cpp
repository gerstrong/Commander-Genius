#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "refkeen_config.h" // MUST precede other contents due to e.g., endianness-based ifdefs

#include "be_cross.h"

// C99
char *BE_Cross_ultoa_dec(uint32_t n, char *buffer);
char *BE_Cross_ltoa_dec(int32_t n, char *buffer);
char *BE_Cross_itoa_dec(int16_t n, char *buffer);

// Used for debugging
void BE_Cross_LogMessage(BE_Log_Message_Class_T msgClass, const char *format, ...)
{
	// TODO: For now we simply do this.
	va_list args;
	va_start(args, format);
	switch (msgClass)
	{
	case BE_LOG_MSG_NORMAL:
		vprintf(format, args);
		break;
	case BE_LOG_MSG_WARNING:
		fprintf(stderr, "Warning: ");
		vfprintf(stderr, format, args);
		break;
	case BE_LOG_MSG_ERROR:
		fprintf(stderr, "Error: ");
		vfprintf(stderr, format, args);
		break;
	}
	va_end(args);
}

// C99
int BE_Cross_toupper(int c);
int BE_Cross_tolower(int c);
int BE_Cross_isupper(int c);
char *BE_Cross_safeandfastcstringcopy(char *dest, char *destEnd, const char *src);
char *BE_Cross_safeandfastcstringcopy_2strs(char *dest, char *destEnd, const char *src0, const char *src1);
char *BE_Cross_safeandfastcstringcopy_3strs(char *dest, char *destEnd, const char *src0, const char *src1, const char *src2);

int BE_Cross_strcasecmp(const char *s1, const char *s2)
{
	unsigned char uc1, uc2;
	/* This one is easy. We don't care if a value is signed or not. */
	/* All that matters here is consistency (everything is signed). */
	for (; (*s1) && (BE_Cross_toupper(*s1) == BE_Cross_toupper(*s2)); s1++, s2++);
	/* But now, first we cast from int to char, and only *then* to */
	/* unsigned char, so the correct difference can be calculated. */
	uc1 = (unsigned char)((char)(BE_Cross_toupper(*s1)));
	uc2 = (unsigned char)((char)(BE_Cross_toupper(*s2)));
	/* We should still cast back to int, for a signed difference. */
	/* Assumption: An int can store any unsigned char value.      */
	return ((int)uc1 - (int)uc2);
}

// Technically a little hack...
#if (defined REFKEEN_VER_CATARM) || (defined REFKEEN_VER_CATAPOC)
uint16_t BE_Cross_Compat_GetFarPtrRelocationSegOffset(void)
{
	uint16_t BE_ST_Compat_GetFarPtrRelocationSegOffset(void);
	return BE_ST_Compat_GetFarPtrRelocationSegOffset();
}
#endif

// C99
int32_t BE_Mem_FarCoreLeft(void);


void BE_Cross_Wrapped_Add(uint8_t *segPtr, uint8_t **offInSegPtrPtr, uint16_t count)
{
	*offInSegPtrPtr += count;
	if (*offInSegPtrPtr - segPtr >= 0x10000)
	{
		*offInSegPtrPtr -= 0x10000;
	}
}

void BE_Cross_Wrapped_Inc(uint8_t *segPtr, uint8_t **offInSegPtrPtr)
{
	++(*offInSegPtrPtr);
	if (*offInSegPtrPtr - segPtr >= 0x10000)
	{
		*offInSegPtrPtr -= 0x10000;
	}
}

void BE_Cross_LinearToWrapped_MemCopy(uint8_t *segDstPtr, uint8_t *offDstPtr, const uint8_t *linearSrc, uint16_t num)
{
	uint16_t bytesToEnd = (segDstPtr+0x10000)-offDstPtr;
	if (num <= bytesToEnd)
	{
		memcpy(offDstPtr, linearSrc, num);
	}
	else
	{
		memcpy(offDstPtr, linearSrc, bytesToEnd);
		memcpy(segDstPtr, linearSrc+bytesToEnd, num-bytesToEnd);
	}
}

void BE_Cross_WrappedToLinear_MemCopy(uint8_t *linearDst, const uint8_t *segSrcPtr, const uint8_t *offSrcPtr, uint16_t num)
{
	uint16_t bytesToEnd = (segSrcPtr+0x10000)-offSrcPtr;
	if (num <= bytesToEnd)
	{
		memcpy(linearDst, offSrcPtr, num);
	}
	else
	{
		memcpy(linearDst, offSrcPtr, bytesToEnd);
		memcpy(linearDst+bytesToEnd, segSrcPtr, num-bytesToEnd);
	}
}

void BE_Cross_WrappedToWrapped_MemCopy(uint8_t *segCommonPtr, uint8_t *offDstPtr, const uint8_t *offSrcPtr, uint16_t num)
{
	uint16_t srcBytesToEnd = (segCommonPtr+0x10000)-offSrcPtr;
	uint16_t dstBytesToEnd = (segCommonPtr+0x10000)-offDstPtr;
	if (num <= srcBytesToEnd)
	{
		// Source is linear: Same as BE_Cross_LinearToWrapped_MemCopy here
		if (num <= dstBytesToEnd)
		{
			memcpy(offDstPtr, offSrcPtr, num);
		}
		else
		{
			memcpy(offDstPtr, offSrcPtr, dstBytesToEnd);
			memcpy(segCommonPtr, offSrcPtr+dstBytesToEnd, num-dstBytesToEnd);
		}
		return;
	}
	// Otherwise, check if at least the destination is linear
	if (num <= dstBytesToEnd)
	{
		// Destination is linear: Same as BE_Cross_WrappedToLinear_MemCopy, non-linear source
		memcpy(offDstPtr, offSrcPtr, srcBytesToEnd);
		memcpy(offDstPtr+srcBytesToEnd, segCommonPtr, num-srcBytesToEnd);

		return;
	}
	// BOTH buffers have wrapping. We don't check separately if
	// srcBytesToEnd==dstBytesToEnd (in such a case offDstPtr==offSrcPtr...)
	if (srcBytesToEnd <= dstBytesToEnd)
	{
		memcpy(offDstPtr, offSrcPtr, srcBytesToEnd);
		memcpy(offDstPtr+srcBytesToEnd, segCommonPtr, dstBytesToEnd-srcBytesToEnd);
		memcpy(segCommonPtr, segCommonPtr+(dstBytesToEnd-srcBytesToEnd), num-dstBytesToEnd);
	}
	else // srcBytesToEnd > dstBytesToEnd
	{
		memcpy(offDstPtr, offSrcPtr, dstBytesToEnd);
		memcpy(segCommonPtr, offSrcPtr+dstBytesToEnd, srcBytesToEnd-dstBytesToEnd);
		memcpy(segCommonPtr+(srcBytesToEnd-dstBytesToEnd), segCommonPtr, num-srcBytesToEnd);
	}
}

void BE_Cross_Wrapped_MemSet(uint8_t *segPtr, uint8_t *offInSegPtr, int value, uint16_t num)
{
	uint16_t bytesToEnd = (segPtr+0x10000)-offInSegPtr;
	if (num <= bytesToEnd)
	{
		memset(offInSegPtr, value, num);
	}
	else
	{
		memset(offInSegPtr, value, bytesToEnd);
		memset(segPtr, value, num-bytesToEnd);
	}
}

// Alternatives for Borland's randomize and random macros used in Catacomb Abyss
// A few pages about the algorithm:
// http://en.wikipedia.org/wiki/Linear_congruential_generator
// http://stackoverflow.com/questions/14672358/implemenation-for-borlandc-rand

static uint32_t g_crossRandomSeed = 0x015A4E36;

static int16_t BEL_Cross_rand(void)
{
	g_crossRandomSeed = 0x015A4E35*g_crossRandomSeed + 1;
	return ((int16_t)(g_crossRandomSeed >> 16) & 0x7FFF);
}

static void BEL_Cross_srand(uint16_t seed)
{
	g_crossRandomSeed = seed;
	BEL_Cross_rand();
}

int16_t BE_Cross_Brandom(int16_t num)
{
	// Cast to unsigned so integer overflow in multiplication is
	// well-defined, but division should still be signed
	return (((int32_t)((uint32_t)BEL_Cross_rand()*(uint32_t)num))/0x8000);
}

void BE_Cross_Brandomize(void)
{
	BEL_Cross_srand(time(NULL));
}
