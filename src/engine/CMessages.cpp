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
#include "../common/CBehaviorEngine.h"

CMessages::CMessages(unsigned char *p_exebuf, char episode, int version) :
	mp_exe(p_exebuf)
{
	m_episode = episode;
	m_version = version;
}

// This function reads the strings specified between the offsets,
// and creates a pair for for the map
std::pair<std::string, std::string>
CMessages::extractString( std::string matchingstring, unsigned long start, unsigned long end, long offset )
{
	std::string Text;

	for(unsigned long pos=start+offset ; pos<end+offset ; pos++)
	{
		while(mp_exe[pos] == 0xA)
		{
			Text += mp_exe[pos];
			pos++;
			if(mp_exe[pos] == 0x0)
				pos++;
		}

		if(mp_exe[pos] == 0x0)
			break;

		Text += static_cast<char>(mp_exe[pos]);
	}

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
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "CTSPACECHEAT", 0x14DEE, 0x14E51 ) );
					StringMap.insert( extractString( "GODMODEON", 0x14EF6, 0x14F06 ) );
					StringMap.insert( extractString( "GODMODEOFF", 0x14F07, 0x14F18 ) );
					StringMap.insert( extractString( "EP1_StatusBox" , 0x15052, 0x150F5, -512 ) );
					StringMap.insert( extractString( "EP1_SHIP" , 0x1679B, 0x167D3, -512 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x160D8, 0x160E5, -512 ) );
					// Message hint Dialogs
					StringMap.insert( extractString( "EP1_YSIYM_LVL2" , 0x15280, 0x15314, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL6" , 0x1531A, 0x153B2, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL9" , 0x153B4, 0x1544C, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL10" , 0x1544E, 0x154E6, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL11" , 0x154E8, 0x1553E, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL12" , 0x15540, 0x155D9, -512 ) );
					StringMap.insert( extractString( "EP1_YSIYM_LVL15" , 0x155DB, 0x1565D, -512 ) );
					// Finale Text
					StringMap.insert( extractString( "EP1_ESEQ_PART1" , 0x15E24, 0x15EB0, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE1" , 0x15EBD, 0x15EFA, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE2" , 0x15EFB, 0x15F2F, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE3" , 0x15F30, 0x15F7E, -512 ) );
					StringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE4" , 0x15F7F, 0x15F9B, -512 ) );
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
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "CTSPACECHEAT", 0x19AC6, 0x19B1B, -512 ) );
					StringMap.insert( extractString( "GODMODEON", 0x19BE4, 0x19BF4, -512 ) );
					StringMap.insert( extractString( "GODMODEOFF", 0x19BF5, 0x19C06, -512 ) );
					StringMap.insert( extractString( "EP2_StatusBox" , 0x19C40, 0x19CCE, -804 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x1A9B2, 0x1A9BF, -776 ) );
					StringMap.insert( extractString( "EP2_VE_NOJUMPINDARK" , 0x1A2D2, 0x1A391, -774 ) );
					StringMap.insert( extractString( "EP2_VE_EVILBELTS" , 0x1A391, 0x1A4A6, -774) );
					StringMap.insert( extractString( "EP2_LVL4_TargetName" , 0x19CCF, 0x19CD5, -804) );
					StringMap.insert( extractString( "EP2_LVL6_TargetName" , 0x19CD6, 0x19CDB, -804) );
					StringMap.insert( extractString( "EP2_LVL7_TargetName" , 0x19CDC, 0x19CE2, -804) );
					StringMap.insert( extractString( "EP2_LVL9_TargetName" , 0x19CE3, 0x19CEB, -804) );
					StringMap.insert( extractString( "EP2_LVL11_TargetName" , 0x19CEC, 0x19CF1, -804) );
					StringMap.insert( extractString( "EP2_LVL13_TargetName" , 0x19CF2, 0x19CF6, -804) );
					StringMap.insert( extractString( "EP2_LVL15_TargetName" , 0x19CF7, 0x19CFD, -804) );
					StringMap.insert( extractString( "EP2_LVL16_TargetName" , 0x19CFE, 0x19D07, -804) );
					// Message Boxes
					StringMap.insert( extractString( "EP2_ESEQ_PART1" , 0x1A4A7, 0x1A558, -774) );
					StringMap.insert( extractString( "EP2_ESEQ_PART2" , 0x1A559, 0x1A5F0, -774) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A5FD, 0x1A638, -774) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE2" , 0x1A639, 0x1A6CD, -774) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE3" , 0x1A6CF, 0x1A72D, -774) );
					StringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE4" , 0x1A72F, 0x1A739, -774) );
					StringMap.insert( extractString( "TO_BE_CONTINUED" , 0x1A73A, 0x1A74D, -774 ) );
				} break;
			}
		} break;
		case 3:
		{
			switch(m_version)
			{
				case 131:
				{
					StringMap.insert( extractString( "CTSPACECHEAT", 0x1BC9E, 0x1BCF3, -512 ) );
					StringMap.insert( extractString( "GODMODEON", 0x1BD80, 0x1BD90, -512 ) );
					StringMap.insert( extractString( "GODMODEOFF", 0x1BD91, 0x1BDA2, -512 ) );
					StringMap.insert( extractString( "EP3_StatusBox" , 0x1BE38, 0x1BEC3, -837 ) );
					StringMap.insert( extractString( "LIVES_LEFT" , 0x1CAEE, 0x1CAFB, -806 ) );
					// Ship Messages
					StringMap.insert( extractString( "EP3_SHIP1" , 0x1D100, 0x1D173, -765) );
					StringMap.insert( extractString( "EP3_SHIP2" , 0x1D174, 0x1D1D1, -765) );
					StringMap.insert( extractString( "EP3_SHIP3" , 0x1D1D2, 0x1D24C, -765) );
					StringMap.insert( extractString( "EP3_SHIP4" , 0x1D24D, 0x1D2C4, -765) );
					//Mortimer
					StringMap.insert( extractString( "EP3_MORTIMER" , 0x1C3B2, 0x1C3D7, -805) );
					StringMap.insert( extractString( "EP3_MORTIMER2" , 0x1C3D8, 0x1C49A, -805) );
					StringMap.insert( extractString( "EP3_MORTIMER3" , 0x1C49B, 0x1C53F, -805) );
					StringMap.insert( extractString( "EP3_MORTIMER4" , 0x1C540, 0x1C607, -805) );
					StringMap.insert( extractString( "EP3_MORTIMER5" , 0x1C608, 0x1C62A, -805) );
					StringMap.insert( extractString( "EP3_MORTIMER6" , 0x1C62B, 0x1C692, -805) );
					// Ending Pages
					StringMap.insert( extractString( "EP3_ESEQ_PAGE1" , 0x1C692, 0x1C72E, -805) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE2" , 0x1C730, 0x1C7A5, -805) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE3" , 0x1C7A7, 0x1C811, -805) );
					StringMap.insert( extractString( "EP3_ESEQ_PAGE4" , 0x1C813, 0x1C81F, -805) );
					StringMap.insert( extractString( "THE_END" , 0x1C82D, 0x1C845, -806) );
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
		std::map<std::string, std::string>::iterator i;
		for( i = StringMap.begin() ; i != StringMap.end() ; i++ )
		{
			g_pBehaviorEngine->addMessage(i->first, i->second);
		}
		g_pLogFile->ftextOut("Loaded %d strings from the exe-file.<br>", StringMap.size());
		return true;
	}
	return false;
}
