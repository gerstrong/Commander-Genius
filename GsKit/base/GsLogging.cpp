/*
 * CLogFile.cpp
 *
 *  Created on: 20.04.2009
 *      Author: gerstrong
 */

#include <stdarg.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include "GsLogging.h"
#include "utils/FindFile.h"
#include "utils/Debug.h"

#define xstr(s) str(s)
#define str(s) #s

CLogFile::CLogFile() {}

const char *CLogFile::endl = "<br>\n";

bool CLogFile::initLogSystem(const std::string &logFName,
                             const std::string &appName,
                             const std::string &version,
                             const bool noFile)
{
    if(noFile)
    {
        // Open and empty the log file
        if( !OpenGameFileW(mLogStream, logFName) )
        {
            return false;
        }
    }
	
	// Write the head
	textOut("<html><head><title>LogFile</title></head>");
	textOut("<body><font face='courier new'>");
	WriteTopic("Logfile", 3);
	
    textOut(FONTCOLORS::BLUE, appName + " v" + version);

	// Mark the Build and Platform
#ifdef DEBUG
	textOut("BUILD: DEBUG<br>");
#else
	textOut("BUILD: RELEASE<br>");
#endif

    textOut("GIT_SHA: " xstr(GIT_SHA) "<br>\n");
    textOut("GIT_BRANCH: " xstr(GIT_BRANCH) "<br>\n");
	
    std::string platformText = "PLATFORM: ";
#if SDL_VERSION_ATLEAST(2, 0, 0)
    platformText += SDL_GetPlatform();
#endif
    platformText += "<br>";

    textOut(platformText);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    ftextOut("Compiled against SDL version %d.%d.%d ...",
           compiled.major, compiled.minor, compiled.patch);
    ftextOut("and linked against SDL version %d.%d.%d.",
           linked.major, linked.minor, linked.patch);
#else
    ftextOut("Using SDL 1.2...");
#endif

    // Ads
	textOut("<br>");
	std::string datestring = __DATE__;
	const std::string Teamyear = "by The Commander Genius Team " + datestring.substr(datestring.size()-4);
    textOut("Build date: " + datestring);
    textOut("<br>");
    textOut(Teamyear);
    textOut("<br>");
    textOut("BY FANS, FOR FANS. ALL \"COMMANDER KEEN\" GRAPHICS,<br>");
    textOut("SOUND, AND LEVEL FILES ARE THE PROPERTY OF ID SOFTWARE<br>");
    textOut("EXCEPT THOSE WHICH WERE MADE BY FANS FOR EXTENSION.<br>");
    textOut("<br>");
	
	// Show my e-mail adress
	textOut("<a href='mailto:gerstrong@gmail.com?subject=CG Logfile'>");
	textOut("Send E-Mail to me</a><br><br>");

    if(mLogStream.is_open())
        mLogStream.close();

    if(noFile)
    {
        // Reopen it in append mode for further writing.
        OpenGameFileW(mLogStream, logFName, std::ios_base::app);
    }


    return true;
}

void CLogFile::closeIt()
{
    *this << "<br><br>End of logfile</font></body></html>";
    if(mLogStream.is_open())
        mLogStream.close();
}

// Function for writing the topic
void CLogFile::WriteTopic(const char *Topic, int Size)
{    
	textOut("<table cellspacing='0' cellpadding='0' width='100%%' bgcolor='#DFDFE5'>\n");
	textOut("<tr>\n<tr>\n<td>\n");
	ftextOut("<font face='arial' size='+%i'>\n", Size);
	textOut(Topic);
	textOut("</font>\n</td>\n</tr>\n</table>\n<br>");
}

// The main textOut function
// Standard textOut (FONTCOLORS::BLACK color)


// Now with colors
void CLogFile::textOut(FONTCOLORS Color, const std::string& Text)
{
	textOut(Color, false, Text);
}

// Now the entire definition (with list and color)
void CLogFile::textOut(FONTCOLORS Color, bool List, const std::string& Text)
{
	if(List == true)
		textOut("<li>");
	
	// write color tag
	switch(Color)
	{
        case FONTCOLORS::BLACK:
			textOut("<font color=black>"); break;
        case FONTCOLORS::RED:
			textOut("<font color=red>"); break;
        case FONTCOLORS::GREEN:
			textOut("<font color=green>"); break;
        case FONTCOLORS::BLUE:
			textOut("<font color=blue>"); break;
        case FONTCOLORS::PURPLE:
			textOut("<font color=purple>"); break;
    }
	
	// Write the text
	textOut(Text);
	textOut("</font>");
	
	if (List == false)
		textOut("<br>");
	else
		textOut("</li>");
}


std::string CLogFile::removeHTML(const std::string& input)
{
    std::string::size_type left, right;
    std::string output;
	
    output = input;
    do {
        left  = output.find_first_of('<', 0);
        right = output.find_first_of('>', 0);
		
        if( left == std::string::npos || right == std::string::npos )
            break;
		
        output = output.erase( left, right-left+1 );
    } while( 1 );
	
    return output;
}

void CLogFile::textOut(const std::string& text)
{    
    std::string output;
	
    output = removeHTML(text);
    if( output.length() > 0 ) {
        notes << output << endl;
    }

    if(mLogStream.is_open())
        mLogStream << text;
}


void CLogFile::ftextOut(const char *Text, ...)
{
    if(Text == nullptr)
        return;

    buffer.fill(0);

	va_list pArgList;
	
	va_start(pArgList, Text);
    vsprintf(buffer.data(), Text, pArgList);
	va_end(pArgList);
	
    textOut(buffer.data());
}

void CLogFile::fltextOut(FONTCOLORS Color, bool List, const char *Text, ...)
{
    buffer.fill(0);
	va_list pArgList;
	
	va_start(pArgList, Text);
    vsprintf(buffer.data(), Text, pArgList);
	va_end(pArgList);
	
    textOut(Color, List, buffer.data());
}




void CLogFile::ftextOut(FONTCOLORS Color, const char *Text, ...)
{
    buffer.fill(0);
	va_list pArgList;
	va_start(pArgList, Text);
    vsprintf(buffer.data(), Text, pArgList);
    textOut(Color, buffer.data());
	va_end(pArgList);
}

CLogFile & CLogFile::operator << (const char *txt)
{
    textOut(std::string(txt));
    return *this;
}

CLogFile & CLogFile::operator << (const std::string &str)
{
    textOut(str);
    return *this;
}

CLogFile & CLogFile::operator << (const int val)
{
    textOut(to_string(val));
    return *this;
}

void CLogFile::FunctionResult (const char *Name,bool Result)
{
	if (Result == true)
	{
		textOut("<table width='100%%' cellSpacing='1' cellPadding='5'>");
		textOut(" border='0' bgcolor='C0C0C0'><tr><td bgcolor=");
		ftextOut("'#FFFFFF' width='35%%'>%s</TD>", Name);
		textOut("<td bgcolor='#FFFFFF' width='30%%'><font color =");
		textOut("'green'>OK</FONT></TD><td bgcolor='#FFFFFF' ");
		textOut("width='35%%'>-/-</TD></tr></table>");
	}
	else
	{
		textOut("<table width='100%%' cellSpacing='1' cellPadding='5'>");
		textOut(" border='0' bgcolor='C0C0C0'><tr><td bgcolor=");
		ftextOut("'#FFFFFF' width='35%%'>%s</TD>", Name);
		textOut("<td bgcolor='#FFFFFF' width='30%%'><font color =");
		textOut("'red'>ERROR</FONT></TD><td bgcolor='#FFFFFF' ");
		textOut("width='35%%'>-/-</TD></tr></table>");
	}
}

CLogFile::~CLogFile()
{
    if(mLogStream.is_open())
        closeIt();
}
