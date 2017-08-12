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
#include <fstream>

const std::string APP_NAME = "Commander Genius";

// Singleton macro
#define  gLogging (CLogFile::get())

const int MAX_BUFFER = 1024;

enum class FONTCOLORS
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
	
    /**
     * @brief CreateLogfile     Open the file for creating some content of general information like
     *                          about the system and the application.
     * @param logFName          Name of the log file
     * @param appName           Name of the application
     * @param version           Version of the application
     * @return                  true if everything went fine, otherwise false
     */
    bool CreateLogfile	(const std::string &logFName,
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

    CLogFile & operator << (const char *txt);
    CLogFile & operator << (const std::string &str);
    CLogFile & operator << (const int val);

private:

    std::ofstream mLogStream;
	
    std::string removeHTML(const std::string& input);
};

#endif /* CLOGFILE_H_ */
