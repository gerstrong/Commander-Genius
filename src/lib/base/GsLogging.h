/*
 * CLogFile.h
 *
 *  Created on: 20.04.2009
 *      Author: gerstrong
 */

#ifndef CLOGFILE_H_
#define CLOGFILE_H_

#include <base/Singleton.h>
#include <string>

const std::string APP_NAME = "Commander Genius";

#define gLogging	CLogFile::get()

const int MAX_BUFFER = 1024;

enum FONTCOLORS
{
	BLACK,
	RED,
	GREEN,
	BLUE,
	PURPLE
};

class CLogFile : public GsSingleton<CLogFile>
{
public:
	
	CLogFile();
	virtual ~CLogFile();
	
    void CreateLogfile	(const char *LogName,
                         const std::string &appName,
                         const std::string &version);
	void WriteTopic		(const char *Topic, int Size);
	void textOut		(const std::string& Text);
	void textOut		(FONTCOLORS Color, const std::string& Text);
	void textOut		(FONTCOLORS Color, bool List, const std::string& Text);
	void ftextOut		(const char *Text, ...);
	void ftextOut		(FONTCOLORS Color, const char *Text, ...);
	void fltextOut		(FONTCOLORS Color, bool List, const char *Text, ...);
	void FunctionResult	(const char *Name, bool Result);
	
private:
	
	FILE *m_Logfile;
	
    std::string removeHTML(const std::string& input);
};

#endif /* CLOGFILE_H_ */
