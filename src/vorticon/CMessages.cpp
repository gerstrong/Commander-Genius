/*
 * CMessages.cpp
 *
 *  Created on: 16.07.2009
 *      Author: Pizza2004 and Gerstrong
 *
 *  This special class reads all the strings
 *  out of an exe-file. It also can identify
 *  what string the program needs.
 */

#include "CMessages.h"
#include "../StringUtils.h"
#include "../CLogFile.h"
#include "../fileio/CExeFile.h"

CMessages::CMessages(unsigned char *p_exebuf, char episode, int version) :
	mp_exe(p_exebuf)
{
	m_episode = episode;
	m_version = version;
}

// This function is used for the strings conversion to this interpreter
// This function is very tricky because it bases on guesses in how some
// menus are drawn
void CMessages::formatString(std::string &Text)
{
	for( int i=0 ; i<Text.size() ; i++ )
	{
		if(Text[i] == 10) // 10 means next line, pass it to...
			Text[i] = 13;
	}
}

// This function reads the strings specified between the offsets,
// and creates a pair for for the map
std::pair<std::string, std::string>
CMessages::extractString( std::string matchingstring, unsigned long start, unsigned long end )
{
	std::string Text;

	for(unsigned long pos=start-512 ; pos<end-512 ; pos++)
	{
		while(mp_exe[pos] != 0)
		{
			Text += mp_exe[pos];
			pos++;
		}
	}
	formatString(Text);

	return make_pair(matchingstring, Text);
}

bool CMessages::extractGlobalStrings()
{
	std::map<std::string, std::string> StringMap; // Structure which stores all the extracted string

	// Here we begin to extract all the proper Strings
	switch(m_episode)
	{
		case 1:
		{
			createPredefinedStringsEp1(StringMap);
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "EP1_StatusBox" , 0x15052, 0x150F5 ) );
				} break;
			}
		} break;
		case 2:
		{
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
		} break;
		case 3:
		{
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
		} break;
		default:
		{
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
			return false;
		}
	}
	

	// Now pass all the Map to the global text structure
	// Still a bad idea, because it's global string.
	std::map<std::string, std::string>::iterator i = StringMap.begin();
	for(int c=0 ; c<MAX_STRINGS ; c++)
	{
		strings[c].name = i->first;
		strings[c].stringptr = i->second;

		i++;
		if( i == StringMap.end() )
			break;
	}
	numStrings = StringMap.size();
	return true;
}

CMessages::~CMessages() {
	// TODO Auto-generated destructor stub
}
