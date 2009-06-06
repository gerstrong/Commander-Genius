/*
 * CLogFile.h
 *
 *  Created on: 20.04.2009
 *      Author: gerstrong
 */

#ifndef CLOGFILE_H_
#define CLOGFILE_H_

#define REVISION        "CloneKeenPlus Beta v0.2.9.0 (Commander Genius)"

#include <stdio.h>
#include "CSingleton.h"
#define g_pLogFile	CLogFile::Get()

#define MAX_BUFFER 1024

enum FONTCOLORS
{
	BLACK,
	RED,
	GREEN,
	BLUE,
	PURPLE
};

class CLogFile : public CSingleton <CLogFile>
{
public:

	CLogFile();
	virtual ~CLogFile();

	void CreateLogfile	(const char *LogName);
	void WriteTopic		(const char *Topic, int Size);
	void textOut		(const char *Text);
	void textOut		(int Color, const char *Text);
	void textOut		(int Color, bool List, const char *Text);
	void ftextOut		(const char *Text, ...);
	void ftextOut		(int Color, const char *Text, ...);
	void ftextOut		(int Color, bool List, const char *Text, ...);
	void FunctionResult	(const char *Name, bool Result);

private:

	FILE *m_Logfile;
};

#endif /* CLOGFILE_H_ */
