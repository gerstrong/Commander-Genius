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
#include "base/interface/StringUtils.h"

#include <base/GsLogging.h>
#include <base/interface/FindFile.h>

#include "engine/core/CBehaviorEngine.h"
#include <fileio/KeenFiles.h>

#include <cstring>

CMessages::CMessages(unsigned char *p_exebuf,
                     const unsigned int exebufSize,
                     const int episode,
                     const bool demo,
                     const int version)
{
    if(exebufSize>0 && p_exebuf!=nullptr)
    {
        mp_exe.resize(exebufSize);
        memcpy(mp_exe.data(), p_exebuf, exebufSize);
    }
	m_episode = episode;
	m_demo = demo;
	m_version = version;
}

void CMessages::setDecodeOffset(const unsigned int off)
{
    mOffset = off;
}

std::pair<std::string, std::string>
CMessages::extractNextString( const std::string matchingstring )
{
	std::string Text;

    for(unsigned long pos = mOffset ; pos<mp_exe.size() ; pos++)
    {
		if(mp_exe[pos] == 0x0)
		{
            while(mp_exe[pos] == 0x0)
            {
                pos++;

                if(pos>=mp_exe.size())
                    break;
            }
			
		    mOffset = pos;
		    break;
		}
		Text += static_cast<char>(mp_exe[pos]);
	}

	return make_pair(matchingstring, Text);    
}

// This function reads the strings specified between the offsets,
// and creates a pair for for the map
std::pair<std::string, std::string>
CMessages::extractString( const std::string matchingstring,
                          const unsigned long start,
                          const unsigned long end,
                          const long offset )
{
	std::string Text;

    for(unsigned long pos = start+offset ; pos < end+offset ; pos++)
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

std::pair<std::string, std::string>
CMessages::extractStringOff( const std::string matchingstring, unsigned long start )
{
	std::string Text;

    for(unsigned long pos=start ; pos<mp_exe.size() ; pos++)
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

bool CMessages::extractEp4Strings(std::map<std::string, std::string>& StringMap)
{
	switch(m_version)
	{
		case 140:
		{
            // Level loading Texts (2E00)

                int offset;
                int lower = 0x0;
                int higher = 0x0;

                // Read Level strings
                // Note: Level 0 is always the world map
                offset = 0x307C8;
                for(int i=0 ; i<20 ; i++)
                {
                    memcpy( &lower, &mp_exe[offset], 2);
                    memcpy( &higher, &mp_exe[offset+0x2], 2);

                    int address = lower + (higher << 4);

                    setDecodeOffset(address);

                    const std::string levelLoadKey = "LEVEL" + itoa(i) + "_LOAD_TEXT";

                    const auto strPair = extractNextString( levelLoadKey );
                    StringMap.insert( strPair );

                    offset += 0x4;
                }

                // Janitor text
                auto readJanitorText = [&](const int offset, const int janitorIdx)
                {
                    memcpy( &lower, &mp_exe[offset], 2);
                    int address = lower<<4;

                    setDecodeOffset(address);

                    const std::string janitorKeyText = "JANITOR_TEXT" + itoa(janitorIdx);

                    const auto strPair = extractNextString( janitorKeyText );
                    StringMap.insert( strPair );
                };

                readJanitorText(0xF146, 0);
                readJanitorText(0xF1B7, 1);
                readJanitorText(0xF22A, 2);
                readJanitorText(0xF29B, 3);

            // Now the spoken Messages of some Characters like Lindsey and the elders
            StringMap.insert( extractString( "LINDSEY_TEXT1", 0x3094B, 0x30997 ) );
            StringMap.insert( extractString( "LINDSEY_TEXT2", 0x30999, 0x309E1 ) );

            StringMap.insert( extractString( "LINDSEY_END_TEXT1", 0x309E3, 0x309F9 ) );
            StringMap.insert( extractString( "LINDSEY_END_TEXT2", 0x309FA, 0x30A22 ) );
            StringMap.insert( extractString( "LINDSEY_START_TEXT", 0x30A23, 0x30A3B ) );

            StringMap.insert( extractString( "CANT_SWIM_TEXT", 0x30A3D, 0x30A4A ) );
            StringMap.insert( extractString( "SWIM_SUIT_TEXT", 0x30A4B, 0x30A72 ) );


            StringMap.insert( extractString( "KEEN_NOSWEAT_TEXT", 0x30A71, 0x30A91 ) );
            StringMap.insert( extractString( "KEEN_BEARDED_ONE_TEXT", 0x30A92, 0x30AB2 ) );
            StringMap.insert( extractString( "KEEN_NO_PROBLEMO_TEXT", 0x30AB3, 0x30ABF ) );
            StringMap.insert( extractString( "KEEN_GREAT_TEXT", 0x30AC0, 0x30AC7 ) );
            StringMap.insert( extractString( "KEEN_LOOKS_LIKE_SAME_GUY_TEXT", 0x30AC8, 0x30AFF ) );
            StringMap.insert( extractString( "KEEN_GOOD_IDEA_GRAMPS", 0x30B00, 0x30B12 ) );
            StringMap.insert( extractString( "KEEN_ROAD_RISE_FEET_TEXT", 0x30B13, 0x30B43 ) );
            StringMap.insert( extractString( "KEEN_WISE_PLAN_TEXT", 0x30B44, 0x30B6A ) );
            StringMap.insert( extractString( "KEEN_LAST_ELDER_TEXT", 0x30B6B, 0x30BB2 ) );
            StringMap.insert( extractString( "ELDERS_UNDERWATER_TEXT", 0x30BB3, 0x30C16 ) );
            StringMap.insert( extractString( "ELDERS_TEXT",  0x30C17, 0x30C6A ) );
            StringMap.insert( extractString( "ELDERS_TEXT",  0x30C17, 0x30C6A ) );

            setDecodeOffset(0x2F41A);
            StringMap.insert( extractNextString( "WARP_LEVEL_TEXT" ) );

            setDecodeOffset(0x1EE40);
            StringMap.insert( extractNextString( "STORY_TEXT" ) );

			return true;
        }
	}
	return false;
}

bool CMessages::extractEp5Strings(std::map<std::string, std::string>& stringMap)
{
	switch(m_version)
	{
		case 140:
		{
            // Level loading Texts (Base is usually at 0x2C00)
            int entryPtr = 0x31B18;
            for(int i=0 ; i<=18 ; i++)
            {
                auto levelOffset = MERGERLOFFSET( GETLONGWORD(&(mp_exe[entryPtr])) );

                setDecodeOffset(levelOffset);
                const std::string levelKey = "LEVEL" + to_string(i) + "_LOAD_TEXT";
                stringMap.insert( extractNextString( levelKey ) );

                entryPtr += 4;
            }

            // Fuse text. This text is loaded when you break one of the fuses
            setDecodeOffset(0x31BFB);
            stringMap.insert( extractNextString( "FUSE_WONDER" ) );
            stringMap.insert( extractNextString( "FUSE_CASUAL" ) );

            setDecodeOffset(0x1FDE0);
            stringMap.insert( extractNextString( "STORY_TEXT" ) );


			return true;
		} break;
	}
	return false;
}

bool CMessages::extractEp6Strings(std::map<std::string, std::string>& StringMap)
{
  switch(m_version)
  {
    case 140:
    {
      // Level loading Texts
      StringMap.insert( extractStringOff( "LEVEL0_LOAD_TEXT",  0x1F110 ) );
      StringMap.insert( extractStringOff( "LEVEL1_LOAD_TEXT",  0x1F130 ) );
      StringMap.insert( extractStringOff( "LEVEL2_LOAD_TEXT",  0x1F160 ) );
      StringMap.insert( extractStringOff( "LEVEL3_LOAD_TEXT",  0x1F190 ) );
      StringMap.insert( extractStringOff( "LEVEL4_LOAD_TEXT",  0x1F1C0 ) );
      StringMap.insert( extractStringOff( "LEVEL5_LOAD_TEXT",  0x1F1F0 ) );
      StringMap.insert( extractStringOff( "LEVEL6_LOAD_TEXT",  0x1F220 ) );
      StringMap.insert( extractStringOff( "LEVEL7_LOAD_TEXT",  0x1F250 ) );
      StringMap.insert( extractStringOff( "LEVEL8_LOAD_TEXT",  0x1F270 ) );
      StringMap.insert( extractStringOff( "LEVEL9_LOAD_TEXT",  0x1F2A0 ) );
      StringMap.insert( extractStringOff( "LEVEL10_LOAD_TEXT", 0x1F2D0) );
      StringMap.insert( extractStringOff( "LEVEL11_LOAD_TEXT", 0x1F300) );
      StringMap.insert( extractStringOff( "LEVEL12_LOAD_TEXT", 0x1F350) );
      StringMap.insert( extractStringOff( "LEVEL13_LOAD_TEXT", 0x1F380) );
      StringMap.insert( extractStringOff( "LEVEL14_LOAD_TEXT", 0x1F3C0) );
      StringMap.insert( extractStringOff( "LEVEL15_LOAD_TEXT", 0x1F400) );
      StringMap.insert( extractStringOff( "LEVEL16_LOAD_TEXT", 0x1F440) );
      StringMap.insert( extractStringOff( "LEVEL17_LOAD_TEXT", 0x1F470) );
      StringMap.insert( extractStringOff( "LEVEL18_LOAD_TEXT", 0x1F4A0) );
      
      // Got Item Text.
      setDecodeOffset(0x325B6);
      StringMap.insert( extractNextString( "KEEN_GOT_SANDWICH" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_GRAPPLING_HOOK" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_SHIPCARD" ) );
      
      // Grabbiter Text
      setDecodeOffset(0x33019);
      StringMap.insert( extractNextString( "KEEN_GRABBITER_HUNGRY" ) );
      StringMap.insert( extractNextString( "KEEN_GRABBITER_SLEEPY" ) );
      StringMap.insert( extractNextString( "KEEN_KEYCARD_REQUIRED" ) );
      StringMap.insert( extractNextString( "KEEN_ROPE_REQUIRED" ) );

      setDecodeOffset(0x1ED50);
      StringMap.insert( extractNextString( "STORY_TEXT" ) );

      
      return true;
    } break;      
    case 150:
    {
      // Level loading Texts
      StringMap.insert( extractStringOff( "LEVEL0_LOAD_TEXT",  0x1ED70 ) );
      StringMap.insert( extractStringOff( "LEVEL1_LOAD_TEXT",  0x1ED90 ) );
      StringMap.insert( extractStringOff( "LEVEL2_LOAD_TEXT",  0x1EDC0 ) );
      StringMap.insert( extractStringOff( "LEVEL3_LOAD_TEXT",  0x1EDF0 ) );
      StringMap.insert( extractStringOff( "LEVEL4_LOAD_TEXT",  0x1EE20 ) );
      StringMap.insert( extractStringOff( "LEVEL5_LOAD_TEXT",  0x1EE50 ) );
      StringMap.insert( extractStringOff( "LEVEL6_LOAD_TEXT",  0x1EE80 ) );
      StringMap.insert( extractStringOff( "LEVEL7_LOAD_TEXT",  0x1EEB0 ) );
      StringMap.insert( extractStringOff( "LEVEL8_LOAD_TEXT",  0x1EED0 ) );
      StringMap.insert( extractStringOff( "LEVEL9_LOAD_TEXT",  0x1EF00 ) );
      StringMap.insert( extractStringOff( "LEVEL10_LOAD_TEXT", 0x1EF30) );
      StringMap.insert( extractStringOff( "LEVEL11_LOAD_TEXT", 0x1EF60) );
      StringMap.insert( extractStringOff( "LEVEL12_LOAD_TEXT", 0x1EFB0) );
      StringMap.insert( extractStringOff( "LEVEL13_LOAD_TEXT", 0x1EFE0) );
      StringMap.insert( extractStringOff( "LEVEL14_LOAD_TEXT", 0x1F020) );
      StringMap.insert( extractStringOff( "LEVEL15_LOAD_TEXT", 0x1F060) );
      StringMap.insert( extractStringOff( "LEVEL16_LOAD_TEXT", 0x1F0A0) );
      StringMap.insert( extractStringOff( "LEVEL17_LOAD_TEXT", 0x1F0D0) );
      StringMap.insert( extractStringOff( "LEVEL18_LOAD_TEXT", 0x1F100) );

      // Got Item Text.
      setDecodeOffset(0x32216);  // 0x325B6
      StringMap.insert( extractNextString( "KEEN_GOT_SANDWICH" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_GRAPPLING_HOOK" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_SHIPCARD" ) );

      // Grabbiter Text
      setDecodeOffset(0x32C79);
      StringMap.insert( extractNextString( "KEEN_GRABBITER_HUNGRY" ) );
      StringMap.insert( extractNextString( "KEEN_GRABBITER_SLEEPY" ) );
      StringMap.insert( extractNextString( "KEEN_KEYCARD_REQUIRED" ) );
      StringMap.insert( extractNextString( "KEEN_ROPE_REQUIRED" ) );

      setDecodeOffset(0x1E9B0);
      StringMap.insert( extractNextString( "STORY_TEXT" ) );

      return true;
    } break;
  }
  return false;
}

bool CMessages::extractEp6DemoStrings(std::map<std::string, std::string>& StringMap)
{
  switch(m_version)
  {
    case 100:
    {
      // Level loading Texts
      StringMap.insert( extractStringOff( "LEVEL0_LOAD_TEXT", 0x1B3B0 ) );
      StringMap.insert( extractStringOff( "LEVEL1_LOAD_TEXT",  0x1B3D0 ) );
      StringMap.insert( extractStringOff( "LEVEL2_LOAD_TEXT",  0x1B400 ) );
      StringMap.insert( extractStringOff( "LEVEL3_LOAD_TEXT",  0x1B446 ) );
      StringMap.insert( extractStringOff( "LEVEL4_LOAD_TEXT",  0x1B460 ) );
      StringMap.insert( extractStringOff( "LEVEL5_LOAD_TEXT",  0x1B490 ) );
      StringMap.insert( extractStringOff( "LEVEL6_LOAD_TEXT",  0x1B4C0 ) );
      StringMap.insert( extractStringOff( "LEVEL7_LOAD_TEXT",  0x1B4F0 ) );
      StringMap.insert( extractStringOff( "LEVEL8_LOAD_TEXT",  0x1B510 ) );
      StringMap.insert( extractStringOff( "LEVEL9_LOAD_TEXT",  0x1B540 ) );
      StringMap.insert( extractStringOff( "LEVEL10_LOAD_TEXT", 0x1B570 ) );
      StringMap.insert( extractStringOff( "LEVEL11_LOAD_TEXT", 0x1B5A0 ) );
      StringMap.insert( extractStringOff( "LEVEL12_LOAD_TEXT", 0x1B5F0 ) );
      StringMap.insert( extractStringOff( "LEVEL13_LOAD_TEXT", 0x1B620 ) );
      StringMap.insert( extractStringOff( "LEVEL14_LOAD_TEXT", 0x1B660 ) );
      StringMap.insert( extractStringOff( "LEVEL15_LOAD_TEXT", 0x1B6A0 ) );
      StringMap.insert( extractStringOff( "LEVEL16_LOAD_TEXT", 0x1B6E0 ) );
      StringMap.insert( extractStringOff( "LEVEL17_LOAD_TEXT", 0x1B710 ) );
      StringMap.insert( extractStringOff( "LEVEL18_LOAD_TEXT", 0x1B740 ) );
      
      // Got Item Text.
      setDecodeOffset(0x325B6);
      StringMap.insert( extractNextString( "KEEN_GOT_SANDWICH" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_GRAPPLING_HOOK" ) );
      StringMap.insert( extractNextString( "KEEN_GOT_SHIPCARD" ) );
      
      // Grabbiter Text
      setDecodeOffset(0x2C49A);
      // StringMap.insert( extractNextString( "KEEN_GRABBITER_HUNGRY" ) );
      // StringMap.insert( extractNextString( "KEEN_GRABBITER_SLEEPY" ) );
      // StringMap.insert( extractNextString( "KEEN_KEYCARD_REQUIRED" ) );
      StringMap.insert( extractNextString( "KEEN_ROPE_REQUIRED" ) );

      setDecodeOffset(0x1AFF0);
      StringMap.insert( extractNextString( "STORY_TEXT" ) );

      
      return true;
    } break;
  }
  return false;
}

bool CMessages::extractGlobalStringsUsingExe()
{
    std::map<std::string, std::string> stringMap; // Structure which stores all the extracted string

	// Here we begin to extract all the proper Strings
	switch(m_episode)
	{
		case 1:
		{
			switch(m_version)
			{
				case 131:
				{
                    stringMap.insert( extractString( "CTSPACECHEAT", 0x14DEE, 0x14E51 ) );
                    stringMap.insert( extractString( "GODMODEON", 0x14EF6, 0x14F06 ) );
                    stringMap.insert( extractString( "GODMODEOFF", 0x14F07, 0x14F18 ) );
                    stringMap.insert( extractString( "EP1_StatusBox" , 0x15052, 0x150F5, -512 ) );
                    stringMap.insert( extractString( "EP1_SHIP" , 0x1679B, 0x167D3, -512 ) );
                    stringMap.insert( extractString( "LIVES_LEFT" , 0x160D8, 0x160E5, -512 ) );
					// Message hint Dialogs
                    stringMap.insert( extractString( "EP1_YSIYM_LVL2" , 0x15280, 0x15314, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL6" , 0x1531A, 0x153B2, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL9" , 0x153B4, 0x1544C, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL10" , 0x1544E, 0x154E6, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL11" , 0x154E8, 0x1553E, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL12" , 0x15540, 0x155D9, -512 ) );
                    stringMap.insert( extractString( "EP1_YSIYM_LVL15" , 0x155DB, 0x1565D, -512 ) );
					// Finale Text
                    stringMap.insert( extractString( "EP1_ESEQ_PART1" , 0x15E24, 0x15EB0, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE1" , 0x15EBD, 0x15EFA, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE2" , 0x15EFB, 0x15F2F, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE3" , 0x15F30, 0x15F7F, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE4" , 0x15F80, 0x15F9B, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE5" , 0x15F9C, 0x15FE1, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE6" , 0x15FE2, 0x15FF5, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE7" , 0x15FF6, 0x16006, -512 ) );
                    stringMap.insert( extractString( "EP1_ESEQ_PART2_PAGE8" , 0x16007, 0x16071, -512 ) );
                    stringMap.insert( extractString( "TO_BE_CONTINUED" , 0x16072, 0x16085, -512 ) );
				} break;
			}
		} break;
		case 2:
		{
			switch(m_version)
			{
				case 131:
				{
                    stringMap.insert( extractString( "CTSPACECHEAT", 0x19AC6, 0x19B1B, -512 ) );
                    stringMap.insert( extractString( "GODMODEON", 0x19BE4, 0x19BF4, -512 ) );
                    stringMap.insert( extractString( "GODMODEOFF", 0x19BF5, 0x19C06, -512 ) );
                    stringMap.insert( extractString( "EP2_StatusBox" , 0x19C40, 0x19CAE, -804 ) );
                    stringMap.insert( extractString( "LIVES_LEFT" , 0x1A9B2, 0x1A9BF, -776 ) );
                    stringMap.insert( extractString( "EP2_VE_NOJUMPINDARK" , 0x1A2D2, 0x1A391, -774 ) );
                    stringMap.insert( extractString( "EP2_VE_EVILBELTS" , 0x1A392, 0x1A4A5, -774) );
                    stringMap.insert( extractString( "EP2_LVL4_TargetName" , 0x19CCF, 0x19CD5, -804) );
                    stringMap.insert( extractString( "EP2_LVL6_TargetName" , 0x19CD6, 0x19CDB, -804) );
                    stringMap.insert( extractString( "EP2_LVL7_TargetName" , 0x19CDC, 0x19CE2, -804) );
                    stringMap.insert( extractString( "EP2_LVL9_TargetName" , 0x19CE3, 0x19CEB, -804) );
                    stringMap.insert( extractString( "EP2_LVL11_TargetName" , 0x19CEC, 0x19CF1, -804) );
                    stringMap.insert( extractString( "EP2_LVL13_TargetName" , 0x19CF2, 0x19CF6, -804) );
                    stringMap.insert( extractString( "EP2_LVL15_TargetName" , 0x19CF7, 0x19CFD, -804) );
                    stringMap.insert( extractString( "EP2_LVL16_TargetName" , 0x19CFE, 0x19D07, -804) );
					// Message Boxes
                    stringMap.insert( extractString( "EP2_ESEQ_PART1" , 0x1A4A7, 0x1A558, -774) );
                    stringMap.insert( extractString( "EP2_ESEQ_PART2" , 0x1A559, 0x1A5F0, -774) );
                    stringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE1" , 0x1A5FD, 0x1A638, -774) );
                    stringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE2" , 0x1A639, 0x1A6CD, -774) );
                    stringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE3" , 0x1A6CF, 0x1A72D, -774) );
                    stringMap.insert( extractString( "EP2_ESEQ_PART3_PAGE4" , 0x1A72F, 0x1A739, -774) );
                    stringMap.insert( extractString( "TO_BE_CONTINUED" , 0x1A73A, 0x1A74D, -774 ) );
				} break;
			}
		} break;
		case 3:
		{
			switch(m_version)
			{
				case 131:
				{
                    stringMap.insert( extractString( "CTSPACECHEAT", 0x1BC9E, 0x1BCF3, -512 ) );
                    stringMap.insert( extractString( "GODMODEON", 0x1BD80, 0x1BD90, -512 ) );
                    stringMap.insert( extractString( "GODMODEOFF", 0x1BD91, 0x1BDA2, -512 ) );
                    stringMap.insert( extractString( "EP3_StatusBox" , 0x1BCF4, 0x1BD60, -512 ) );
                    stringMap.insert( extractString( "LIVES_LEFT" , 0x1CAEE, 0x1CAFB, -806 ) );
					// Ship Messages
                    stringMap.insert( extractString( "EP3_SHIP1" , 0x1D101, 0x1D173, -765) );
                    stringMap.insert( extractString( "EP3_SHIP2" , 0x1D175, 0x1D1D1, -765) );
                    stringMap.insert( extractString( "EP3_SHIP3" , 0x1D1D3, 0x1D24C, -765) );
                    stringMap.insert( extractString( "EP3_SHIP4" , 0x1D24E, 0x1D2C4, -765) );
					//Mortimer
                    stringMap.insert( extractString( "EP3_MORTIMER" , 0x1C3B2, 0x1C3D7, -805) );
                    stringMap.insert( extractString( "EP3_MORTIMER2" , 0x1C3D8, 0x1C499, -805) );
                    stringMap.insert( extractString( "EP3_MORTIMER3" , 0x1C49B, 0x1C53E, -805) );
                    stringMap.insert( extractString( "EP3_MORTIMER4" , 0x1C53F, 0x1C606, -805) );
                    stringMap.insert( extractString( "EP3_MORTIMER5" , 0x1C607, 0x1C629, -805) );
                    stringMap.insert( extractString( "EP3_MORTIMER6" , 0x1C62A, 0x1C692, -805) );
					// Ending Pages
                    stringMap.insert( extractString( "EP3_ESEQ_PAGE1" , 0x1C692, 0x1C72E, -805) );
                    stringMap.insert( extractString( "EP3_ESEQ_PAGE2" , 0x1C730, 0x1C7A5, -805) );
                    stringMap.insert( extractString( "EP3_ESEQ_PAGE3" , 0x1C7A7, 0x1C811, -805) );
                    stringMap.insert( extractString( "EP3_ESEQ_PAGE4" , 0x1C813, 0x1C81F, -805) );
                    stringMap.insert( extractString( "THE_END" , 0x1C82D, 0x1C845, -806) );
				} break;
			}
		} break;

		case 4:
		{
            if(!extractEp4Strings(stringMap))
				gLogging.textOut(FONTCOLORS::RED,"This version of the game is not supported!");
		} break;

		case 5:
		{
            if(!extractEp5Strings(stringMap))
				gLogging.textOut(FONTCOLORS::RED,"This version of the game is not supported!");
		} break;

		case 6:
		{
            if(!(m_demo ? extractEp6DemoStrings(stringMap) :
                    extractEp6Strings(stringMap)))
				gLogging.textOut(FONTCOLORS::RED,"This version of the game is not supported!");
		} break;

		default:
		{
			gLogging.textOut(FONTCOLORS::RED,"This version of the game is not supported!");
			return false;
		}
	}
	
	// Now pass all the Map to the global text structure
    if(!stringMap.empty())
	{
		std::map<std::string, std::string>::iterator i;
        for( i = stringMap.begin() ; i != stringMap.end() ; i++ )
		{
			gBehaviorEngine.setMessage(i->first, i->second);
		}
        gLogging.ftextOut("Loaded a total of %d strings from the Exe.<br>", stringMap.size());
		return true;
	}
	return false;
}


bool CMessages::extractGlobalStringsUsingLua()
{
    std::map<std::string, std::string> stringMap; // Structure which stores all the extracted string

    const auto fullFName = JoinPaths(gKeenFiles.gameDir, "messageMap.lua");
    mLua.loadFile(fullFName);

    if(mLua)
    {
        for(int i=0 ; i<=18 ; i++)
        {
            const std::string levelKey = "LEVEL" + to_string(i) + "_LOAD_TEXT";
            std::string value;

            const auto ok = mLua.runFunctionRetOneStr("getLevelLoadMsg", i, value);

            if(ok)
            {
                stringMap[levelKey] = value;
            }

        }
    }

    // Now pass all the Map to the global text structure
    if(!stringMap.empty())
    {
        std::map<std::string, std::string>::iterator i;
        for( i = stringMap.begin() ; i != stringMap.end() ; i++ )
        {
            gBehaviorEngine.setMessage(i->first, i->second);
        }
        gLogging.ftextOut("Loaded a total of %d strings using Lua.<br>", stringMap.size());
        return true;
    }
    return false;
}
