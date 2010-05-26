#ifndef __CG_FILEIO2_H__
#define __CG_FILEIO2_H__

#include <string>
#include <cstdio>

char loadstrings();
unsigned int fgeti(FILE *fp);
unsigned long fgetl(FILE *fp);
void fputi(unsigned int word, FILE *fp);
void fputl(unsigned long word, FILE *fp);

#endif
