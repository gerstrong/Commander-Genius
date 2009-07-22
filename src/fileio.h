#ifndef __CG_FILEIO2_H__
#define __CG_FILEIO2_H__

#include <string>
#include <cstdio>

char loadstrings(const std::string& fname);
unsigned int fgeti(FILE *fp);
unsigned long fgetl(FILE *fp);
	std::string formatPathString(const std::string& path);

#endif
