/* FILEIO.C
 * Author : Gerhard Stein (gerstrong@gmail.com)
 */

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <stdio.h>

#include <base/video/CVideoDriver.h>
#include "sdl/audio/Audio.h"
#include "fileio.h"
#include <base/GsLogging.h>
#include <base/utils/StringUtils.h>
#include <base/utils/Debug.h>
#include <base/utils/FindFile.h>

#ifdef TARGET_WIN32
#define uint unsigned int
#define ulong unsigned long
#endif

#ifdef __APPLE__
#define uint unsigned int
#define ulong unsigned long
#endif

unsigned int fgeti(FILE *fp) {
	unsigned int lsb, msb;
	lsb = fgetc(fp);
	msb = fgetc(fp);
	return (msb<<8) | lsb;
}

void fputi(uint word, FILE *fp)
{
	fputc(word&255, fp);
	fputc(word/256, fp);
}

unsigned long fgetl(FILE *fp) {
	unsigned int temp1, temp2, temp3, temp4;
	temp1 = fgetc(fp);
	temp2 = fgetc(fp);
	temp3 = fgetc(fp);
	temp4 = fgetc(fp);
	return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

unsigned long fgetl(std::ifstream &file) {
	int temp1, temp2, temp3, temp4;
	temp1 = file.get();
	temp2 = file.get();
	temp3 = file.get();
	temp4 = file.get();
	return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

word fgetw(std::ifstream &file) {
	int temp1, temp2;
	temp1 = file.get();
	temp2 = file.get();
	return (temp2<<8) | temp1;
}


void fputl(unsigned long word, FILE *fp)
{
	unsigned long a,b,c,d;
	a=b=c=d = word;
	a &= 0xFF000000; a >>= 24;
	b &= 0x00FF0000; b >>= 16;
	c &= 0x0000FF00; c >>= 8;
	d &= 0x000000FF;
	fputc(d, fp);
	fputc(c, fp);
	fputc(b, fp);
	fputc(a, fp);
}
