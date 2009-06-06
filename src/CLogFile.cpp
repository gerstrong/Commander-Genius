/*
 * CLogFile.cpp
 *
 *  Created on: 20.04.2009
 *      Author: gerstrong
 */

#include <stdarg.h>
#include <iostream>
#include <string.h>
#include "CLogFile.h"
#include <fstream>

CLogFile::CLogFile() {}

CLogFile::~CLogFile() {
	// Logfile End
	textOut ("<br><br>End of logfile</font></body></html>");
	fclose (m_Logfile);
}

void CLogFile::CreateLogfile(const char *LogName)
{
	// Open and empty the log file
	m_Logfile = fopen(LogName, "wt");

	// Write the head
	textOut("<html><head><title>LogFile</title></head>");
	textOut("<body><font face='courier new'>");
	WriteTopic("Logfile", 3);
	textOut(BLUE,REVISION);

	// Mark the Build and Platform
	#ifdef DEBUG
		textOut("BUILD: DEBUG<br>");
	#else
		textOut("BUILD: RELEASE<br>");
	#endif

	#ifdef TARGET_LNX
		textOut("PLATFORM: LINUX<br>");
	#elif TARGET_WIN32
		textOut("PLATFORM: WINDOWS<br>");
	#else
		textOut("PLATFORM: UNKNOWN<br>");
	#endif

	// Show my e-mail adress
	textOut("<a href='mailto:gerstrong@gmail.com?subject=CG Logfile'>");
	textOut("Send E-Mail to me</a><br><br>");

	fclose(m_Logfile);
	m_Logfile = fopen(LogName, "at");
}

// Function for writing the topic
void CLogFile::WriteTopic(const char *Topic, int Size)
{
	textOut("<table cellspacing='0' cellpadding='0' width='100%%' ");
	textOut("bgcolor='#DFDFE5'>\n<tr>\n<tr>\n<td>\n<font face='arial' ");
	ftextOut("size='+%i'>\n", Size);
	textOut(Topic);
	textOut("</font>\n</td>\n</tr>\n</table>\n<br>");
	fflush(m_Logfile);
}

// The main textOut function
// Standard textOut (Black color)


// Now with colors
void CLogFile::textOut(int Color, const char *Text)
{
	textOut(Color, false, Text);
}

// Now the entire definition (with list and color)
void CLogFile::textOut(int Color, bool List, const char *Text)
{
	if(List == true)
		textOut("<li>");

	// write color tag
	switch(Color)
	{
	  case BLACK:
		textOut("<font color=black>"); break;
	  case RED:
		textOut("<font color=red>"); break;
	  case GREEN:
		textOut("<font color=green>"); break;
	  case BLUE:
		textOut("<font color=blue>"); break;
	  case PURPLE:
		textOut("<font color=purple>"); break;
	};

	// Write the text
	textOut(Text);
	textOut("</font>");

	if (List == false)
		textOut("<br>");
	else
		textOut("</li>");

}


void CLogFile::textOut(const char *Text)
{
	fprintf(m_Logfile,"%s",Text);
	fflush(m_Logfile);
}

void CLogFile::ftextOut(const char *Text, ...)
{
	char buffer[MAX_BUFFER];
	va_list pArgList;

	va_start(pArgList, Text);
	vsprintf(buffer, Text, pArgList);
	va_end(pArgList);

	textOut(buffer);
}

void CLogFile::ftextOut(int Color, bool List, const char *Text, ...)
{
	char buffer[MAX_BUFFER];
	va_list pArgList;

	va_start(pArgList, Text);
	vsprintf(buffer, Text, pArgList);
	va_end(pArgList);

	textOut(Color, List, buffer);
}

void CLogFile::ftextOut(int Color, const char *Text, ...)
{
	char buffer[MAX_BUFFER];
	va_list pArgList;

	va_start(pArgList, Text);
	vsprintf(buffer, Text, pArgList);
	va_end(pArgList);

	textOut(Color, buffer);
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
