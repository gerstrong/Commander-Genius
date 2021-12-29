/*
 * GsLogging.h
 *
 *  Created on: 20.04.2009
 *      Author: gerstrong
 */

#ifndef GSLOGGING_H
#define GSLOGGING_H

#include <base/Singleton.h>
#include <string>
#include <fstream>
#include <array>

// Singleton macro
#define  gLogging (CLogFile::get())


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
     * @param noFile            if true
     * @return                  true if everything went fine, otherwise false
     */
    bool initLogSystem	(const std::string &logFName,
                         const std::string &appName,
                         const std::string &version,
                         const bool noFile);

    void closeIt();

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

    static const char *endl;

private:

    std::ofstream mLogStream;

    std::string removeHTML(const std::string& input);

    std::array<char, 1024> buffer;
};


#endif /* CLOGFILE_H_ */
