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
#include "../CLogFile.h"
#include "../fileio/CExeFile.h"

CMessages::CMessages() {

}

CMessages::~CMessages() {
	// TODO Auto-generated destructor stub
}

// buf -  is the uncompressed buffer of the exe-file (keen1.exe for example)
// episode - the game's episode
// version - version of the exe-file
bool CMessages::readData(unsigned char *p_exe, int episode, int version, const std::string& DataDirectory)
{
	long offset_start = 0;
	long offset_end = 0;
	//unsigned char *text_data = NULL;
	//std::string Text;
	
	// TODO: This function still has bugs when reading the text. Check this closer!
	// This part of switches is used to get the proper addresses of the message text.
	switch(episode)
	{
		case 1:
			switch(version)
		{
			case 131:
				offset_start = 85419;
				//offset_start = 85600;
				//offset_end = 85650;
				offset_end = 89729;
				break;
		}
			break;
		case 2:
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
			break;
		case 3:
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
			break;
		default:
			g_pLogFile->textOut(RED,"This version of the game is not supported!");
			return false;
	}
	
	/*for(unsigned long i=offset_start ; i<offset_end ; i++ )
	{
		Text += text_data[i];
		//Text.push_back(text_data[i]);
	g_pLogFile->textOut(RED,Text);
		}
	
	delete ExeFile;*/

	// Now read the stuff and store it to a list
	for(int pos=offset_start ; pos<offset_end ; pos++)
	{
		std::string Text;

		while(p_exe[pos] != 0)
		{
			Text += p_exe[pos];
			pos++;
		}

		if(!Text.empty()) // not empty
		{
			StringList.push_back(Text);
		g_pLogFile->textOut(RED,Text);
		}
	}

	std::list<std::string> :: iterator i;

#include <iostream>
	for(i=StringList.begin() ; i!=StringList.end() ; i++)
	{
		std::cout << *i << std::endl;
	}
	return true;
}

char *CMessages::getString(const char *IDtext)
{
	return 0;
}
