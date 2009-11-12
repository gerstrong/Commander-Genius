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
					StringMap.insert( extractString( "EP1_SHIP" , 0x1679B, 0x167D3 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x160D8, 0x160E5 ) );
					// Message hint Dialogs
					StringMap.insert( extractString( "EP1_YSIYM_LVL2" , 0x152C2, 0x15313 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL6" , 0x1531A, 0x153B3 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL9" , 0x153B4, 0x1544D ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL10" , 0x1544E, 0x154E7 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL11" , 0x154E8, 0x1553F ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL12" , 0x15540, 0x155DA ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL15" , 0x155DB, 0x1565E ) );
					// Finale Text
					StringMap.insert( extractString( "EP1_ESEQ_PART1" , 0x15E24, 0x15EB0 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE1" , 0x15EBD, 0x15EFA ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE2" , 0x15EFB, 0x15F2F ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE3" , 0x15F30, 0x15F7F ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE4" , 0x15F80, 0x15F9B ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE5" , 0x15F9C, 0x15FE1 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE6" , 0x15FE2, 0x15FF5 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE7" , 0x15FF6, 0x16006 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE8" , 0x16007, 0x16071 ) );
					StringMap.insert( extractString( "TO_BE_CONTINUED" , 0x16072, 0x16085 ) );
				} break;
			}
		} break;
		case 2:
		{
			createPredefinedStringsEp2(StringMap);
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "EP2_StatusBox" , 0x15052, 0x150F5 ) );
					StringMap.insert( extractString( "EP1_SHIP" , 0x15052, 0x150F5 ) );
					StringMap.insert( extractString( "EP1_StatusBox" , 0x15052, 0x150F5 ) );
				} break;
			}
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
	g_pLogFile->ftextOut("Loaded %d strings from the exe-file.<br>", numStrings);
	return true;
}

CMessages::~CMessages() {
	// TODO Auto-generated destructor stub
}
