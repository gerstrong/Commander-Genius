#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <string>
#include <cstdio>
#include <fstream>
#include <base/TypeDefinitions.h>

unsigned int fgeti(FILE *fp);
unsigned long fgetl(FILE *fp);
unsigned long fgetl(std::ifstream &file);
word fgetw(std::ifstream &file);
void fputi(unsigned int word, FILE *fp);
void fputl(unsigned long word, FILE *fp);

#endif
