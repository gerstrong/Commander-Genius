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
CMessages::extractString( std::string matchingstring, unsigned long start, unsigned long end, long offset )
{
	std::string Text;

	for(unsigned long pos=start+offset ; pos<end+offset ; pos++)
	{
		while(mp_exe[pos] != 0)
		{
			Text += mp_exe[pos];
			if(pos<end+offset) pos++;
			else break;
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
					StringMap.insert( extractString( "EP1_StatusBox" , 0x15052, 0x150F5, -512 ) );
					StringMap.insert( extractString( "EP1_SHIP" , 0x1679B, 0x167D3, -512 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x160D8, 0x160E5, -512 ) );
					// Message hint Dialogs
					StringMap.insert( extractString( "EP1_YSIYM_LVL2" , 0x15280, 0x15313, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL6" , 0x1531A, 0x153B3, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL9" , 0x153B4, 0x1544D, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL10" , 0x1544E, 0x154E7, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL11" , 0x154E8, 0x1553F, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL12" , 0x15540, 0x155DA, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL15" , 0x155DB, 0x1565E, -512 ) );
					// Finale Text
					StringMap.insert( extractString( "EP1_ESEQ_PART1" , 0x15E24, 0x15EB0, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE1" , 0x15EBD, 0x15EFA, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE2" , 0x15EFB, 0x15F2F, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE3" , 0x15F30, 0x15F7F, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE4" , 0x15F80, 0x15F9B, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE5" , 0x15F9C, 0x15FE1, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE6" , 0x15FE2, 0x15FF5, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE7" , 0x15FF6, 0x16006, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE8" , 0x16007, 0x16071, -512 ) );
					StringMap.insert( extractString( "TO_BE_CONTINUED" , 0x16072, 0x16085, -512 ) );
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
					StringMap.insert( extractString( "EP2_StatusBox" , 0x19C40, 0x19CCE, -775 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x1A9B2, 0x1A9BF, -775 ) );
					StringMap.insert( extractString( "EP2_VE_NOJUMPINDARK" , 0x1A2D2, 0x1A391, -775 ) );
					StringMap.insert( extractString( "EP2_VE_EVILBELTS" , 0x1A391, 0x1A4A6, -775) );
					StringMap.insert( extractString( "EP2_LVL4_TargetName" , 0x19CCF, 0x19CD5, -775) );
					StringMap.insert( extractString( "EP2_LVL6_TargetName" , 0x19CD6, 0x19CDB, -775) );
					StringMap.insert( extractString( "EP2_LVL7_TargetName" , 0x19CDC, 0x19CE2, -775) );
					StringMap.insert( extractString( "EP2_LVL9_TargetName" , 0x19CE3, 0x19CEB, -775) );
					StringMap.insert( extractString( "EP2_LVL11_TargetName" , 0x19CEC, 0x19CF1, -775) );
					StringMap.insert( extractString( "EP2_LVL13_TargetName" , 0x19CF2, 0x19CF6, -775) );
					StringMap.insert( extractString( "EP2_LVL15_TargetName" , 0x19CF7, 0x19CFD, -775) );
					StringMap.insert( extractString( "EP2_LVL16_TargetName" , 0x19CFE, 0x19D07, -775) );
					// Message Boxes
					StringMap.insert( extractString( "EP2_ESEQ_PART1" , 0x1A4A7, 0x1A558, -775) );
					StringMap.insert( extractString( "EP2_ESEQ_PART2" , 0x1A559, 0x1A5F1, -775) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A5FD, 0x1A638, -775) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A639, 0x1A6CE, -775) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A6CF, 0x1A72E, -775) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A72F, 0x1A739, -775) );
					StringMap.insert( extractString( "TO_BE_CONTINUED" , 0x1A73A, 0x1A74D, -775 ) );
				} break;
			}
		} break;
		case 3:
		{
			createPredefinedStringsEp3(StringMap);
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "EP3_StatusBox" , 0x1BE38, 0x1BEC3 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x1CAEE, 0x1CAFB ) );
					// Ship Messages
					StringMap.insert( extractString( "EP3_SHIP1" , 0x1D100, 0x1D173 ) );
					StringMap.insert( extractString( "EP3_SHIP2" , 0x1D174, 0x1D1D1) );
					StringMap.insert( extractString( "EP3_SHIP3" , 0x1D1D2, 0x1D24C) );
					StringMap.insert( extractString( "EP3_SHIP4" , 0x1D24D, 0x1D2C4) );
					//Mortimer
					StringMap.insert( extractString( "EP3_MORTIMER" , 0x1C3B3, 0x1C3D8) );
					StringMap.insert( extractString( "EP3_MORTIMER2" , 0x1C3D9, 0x1C49B) );
					StringMap.insert( extractString( "EP3_MORTIMER3" , 0x1C49C, 0x1C53F) );
					StringMap.insert( extractString( "EP3_MORTIMER4" , 0x1C540, 0x1C607) );
					StringMap.insert( extractString( "EP3_MORTIMER5" , 0x1C608, 0x1C692) );
					// Ending Pages
					StringMap.insert( extractString( "EP3_ESEQ_PAGE1" , 0x1C693, 0x1C730) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE2" , 0x1C731, 0x1C7A7) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE3" , 0x1C7A8, 0x1C813) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE4" , 0x1C813, 0x1C821) );
					StringMap.insert( extractString( "THE_END" , 0x1C82D, 0x1C847) );
				} break;
			}
		} break;
		default:
		{
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
			return false;
		}
	}
	

	// Now pass all the Map to the global text structure
	// Still a bad idea, because it's global string.
	if(!StringMap.empty())
	{
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
	return false;
}

CMessages::~CMessages() {
	// TODO Auto-generated destructor stub
}
