/*
 * CParser.cpp
 *
 *  Created on: 22.05.2009
 *      Author: gerstrong
 */

#include "CParser.h"
#include "../CLogFile.h"
#include "../StringUtils.h"
#include "../sdl/CSettings.h"
#include "../FindFile.h"
#include <cstdio>
#include <cstdlib>


CParser::CParser() {
	m_configfile = CONFIGFILENAME;
	m_isOpen = false;
}

CParser::~CParser() {}

// replaced by sscanf, since that function is slower and leaks memory
std::string CParser::parseline(FILE *fp)
{
	int c = 0;
	std::string line;
	while( (c = fgetc(fp)) >= 0 )
	{
		if(c == '\n')
			break;
		else
			line += (char)c;
	}
	return line;
}


// Opens the text which can be parsed. This will read all the stuff in to the memory.
bool CParser::loadParseFile() // Open, read the list and close the file
{
	FILE *fp;
	
	if((fp=OpenGameFile(m_configfile.c_str(),"rt")))
	{
		while(!feof(fp))
		{
			std::string line = parseline(fp);
			m_filebuffer.push_back(line);
			m_isOpen = true;
		}
		fclose(fp);
		return true;
	}
	
	g_pLogFile->ftextOut(GREEN,"Parser : The file has not been found. When CKP is trying to save the file it will create a new one.");
	return false;
}

// Close the text which was parsed. This will also copy all stuff of the memory to the file.
bool CParser::saveParseFile() // open, write on the file and close
{
	FILE *fp;
	
	if((fp=OpenGameFile(m_configfile.c_str(),"wt")))
	{
		for(std::list<std::string>::iterator i = m_filebuffer.begin() ; i != m_filebuffer.end() ; ++i )
			fprintf(fp,"%s\n",i->c_str());
		
		fclose(fp);
		return true;
	}
	
	g_pLogFile->ftextOut(RED,"Parser : Error opening the file for write operations. Check your permissions or if the disk is full");
	return false;
}

// read the value of the to be seeked keyword and returns it as an int.
// If no value was detected, it returns -1;
// If something was detected, the file is also rewinded!
std::string CParser::getValue(const std::string& keyword, const std::string& category)
{
	std::string def = "";
	// The getter will search for category and than for keyword. After that, read the value and return it!
	for(std::list<std::string>::iterator line = m_filebuffer.begin() ; line != m_filebuffer.end() ; ++line )
	{
		if(*line == "") continue;
		if((*line)[0]=='[')	// found category
		{
			// category found !! let's see if the keyword exists
			if(stringcaseequal(*line, "[" + category + "]")) // is it the category we are looking for?
			{
				for(line++; line != m_filebuffer.end(); line++) {
					if(*line == "") continue;
					if((*line)[0] == '[') break;
					
					if(subStrCaseEqual(*line, keyword + " =", keyword.size() + 2))
						return line->substr(keyword.size() + 2);
				}
				break;
			}
		}
	}
	return def;
}

// This function saves the value of a keyword. If the value already exists in the file
// it will be overwritten.
// If something was written the file is also rewinded!
void CParser::saveValue(const std::string& keyword, const std::string& category, const std::string& value)
{
	// Three cases:
	// 1.- category doesn't exist
	// 2.- category exists, but keyword not
	// 3.- category and keyword exist, only the value must be changed
	
	std::string newline = keyword + " = " + value;
	
	for(std::list<std::string>::iterator line = m_filebuffer.begin() ; line != m_filebuffer.end() ; ++line )
	{
		if(*line == "") continue;
		if((*line)[0]=='[')	// found category
		{			
			// category found !! let's see if the keyword exists
			if(stringcaseequal(*line, "[" + category + "]")) // is it the category we are looking for?
			{
				std::list<std::string>::iterator lastnonempty = line;
				
				for(line++; line != m_filebuffer.end(); line++) {
					if(*line == "") continue;
					if((*line)[0] == '[') break;
					
					lastnonempty = line;
					if(subStrCaseEqual(*line, keyword + " =", keyword.size() + 2))
					{
						*line = newline;
						return;
					}
				}
				
				// not found! case 2: category found, but no keyword
				
				lastnonempty++;
				m_filebuffer.insert(lastnonempty, newline);
				return;
				
			}
		}
	}
	
	// First case: Category doesn't exist! Create a new one, and as the keyword also cannot exist, create it too!
	m_filebuffer.push_back("[" + category + "]");
	m_filebuffer.push_back(newline);
	m_filebuffer.push_back("");
}


int CParser::getIntValue(const std::string& keyword, const std::string& category, int def) {
	std::string str = getValue(keyword, category);
	if(str == "") return def;
	
	bool fail = false;
	int v = from_string<int>(str, fail);
	if(fail) return def;
	
	return v;
}

void CParser::saveIntValue(const std::string& keyword, const std::string& category,int value) {
	saveValue(keyword, category, itoa(value));
}
