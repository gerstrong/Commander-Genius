/*
 * CPatcher.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CPatcher.h"
#include <string.h>
#include <fstream>
#include "../FindFile.h"
#include "../StringUtils.h"
#include "../CLogFile.h"

CPatcher::CPatcher(int episode, int version,unsigned char *data, const std::string& datadir) {
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_datadirectory = datadir;
}

void CPatcher::patchMemory()
{
	if(!loadPatchfile()) return;
	
	// If the file was found and read into the m_TextList,
	// then read out of the list the patch commands and apply them to the
	// Exe-file data m_data
	
	g_pLogFile->textOut("Trying to load and apply the patch it found...\n");
	
	filterPatches();

	patch_item PatchItem;

	// TODO: Extend this part further with more commands
	while(readNextPatchItem(PatchItem) == true)
	{
		// Now we really start to process the commands

		if(PatchItem.keyword == "ext")
		{
			// Check's if patch matches with the episode
			std::string extText = readPatchItemsNextValue(PatchItem.value);

			if(strCaseStartsWith(extText,"ck"))
			{
				if(atoi(extText.substr(2)) != m_episode)
				{
					g_pLogFile->textOut("Error! The patch-file does not match the opened episode!!!\n");
					return;
				}
			}
		}
		else if(PatchItem.keyword == "patchfile")
		{
			std::string newbuf = PatchItem.value.front();
			// TODO: Fix that part of the CODE!!!!
			// DON'T FORGET THAT!!!!!!!!!1
			// !!!!!!!!!!!!!!!!!!!
			// !!!!!!!!!!!!!!!!!!

			// Seperate the offset and the filename
			long offset;
			size_t p = newbuf.find(' ');
			sscanf(newbuf.substr(0,p).c_str(), "%lx", &offset);
			std::string patch_file_name = newbuf.substr(p);
			TrimSpaces(patch_file_name);

			patchMemfromFile(m_datadirectory + "/" + patch_file_name,offset);
		}

		else if(PatchItem.keyword == "patch" )
		{
			// first we need to get the offset
			long offset = 0;
			std::string textline = readPatchItemsNextValue(PatchItem.value);
			if(readIntValue(textline, offset))
			{
				while(!PatchItem.value.empty())
				{
					// after we have it, distinguish between text case and number case
					long number = 0;
					std::string patchtext = "";
					textline = readPatchItemsNextValue(PatchItem.value);

					if(readIntValue(textline, number))
					{
						// In this case we have number
						m_data[offset] = number;
						offset++;
					}
					else if(readPatchString(textline, patchtext))
					{
						size_t textsize = patchtext.size();
						memcpy( m_data+offset, patchtext.c_str(), textsize);
						offset += textsize;
					}
				}
			}
			else
				g_pLogFile->textOut("Something is wrong with the \%patch command. A number was expected\n");



			/*if( strCaseStartsWith(newbuf,"0x") ){
				long offset = 0;

				sscanf(newbuf.c_str() ,"%lx", &offset);

				newbuf = newbuf.substr(p);

				TrimSpaces(newbuf);

				if(strCaseStartsWith(newbuf,"\""))
				{
					std::string patchtext = newbuf.substr(1);
					patchtext = patchtext.substr(0, patchtext.find("\""));
					patchMemFromText(offset, patchtext);
				}
			}*/
		}
		/*else if( strCaseStartsWith(line,"\%level.hint") )
		{
			// You have a level hint. Very good, lets read it and patch!
			std::string newbuf = line.substr(strlen("\%level.hint"));
			TrimSpaces(newbuf);
			PatchLevelhint(atoi(newbuf));
		}*/
		else
			g_pLogFile->textOut("They Keyword " + PatchItem.keyword + " is not supported by CG yet!\n" );

		PatchItem.keyword.clear();
		PatchItem.value.clear();
	}
}

struct PatchListFiller {
	std::set<std::string> list;
	
	bool operator() (const std::string& filename) {
		std::string ext = GetFileExtension(filename);
		if (stringcaseequal(ext, "pat"))
			list.insert(filename);
		
		return true;
	}
};

/**
 * \brief this reads the patch into the m_TextList
 * \return	true if something could be read. Otherwise false
 */
bool CPatcher::loadPatchfile()
{
	std::string path = m_datadirectory;
	
	//Get the list of ".pat" files
	PatchListFiller patchlist;
	FindFiles(patchlist, path, false, FM_REG);
	
	// Nothing to patch, just quit
	if (!patchlist.list.size())
		return false;
	
	if (patchlist.list.size() > 1)
		g_pLogFile->textOut(PURPLE,"Multiple Patch are not yet supported! Please remove a file. Taking one File.<br>");
	
	while(!patchlist.list.empty())
	{
		std::string buf = *patchlist.list.begin();
		std::ifstream Patchfile; OpenGameFileR(Patchfile, buf);
		
		while(!Patchfile.eof())
		{
			char buf[256];
			Patchfile.getline(buf, sizeof(buf));
			fix_markend(buf);
			m_TextList.push_back(buf);
		}
		Patchfile.close();
		patchlist.list.clear();
	}
	
	return true;
}

void CPatcher::patchMemfromFile(const std::string& patch_file_name, long offset)
{
	unsigned char *buf_to_patch;
	unsigned char byte;
	
	std::ifstream Patchfile; OpenGameFileR(Patchfile, patch_file_name, std::ios::binary);
	
	if(!Patchfile) return;
	
	if(!m_data)
	{
		g_pLogFile->textOut(PURPLE,"Warning: The patchfile was wrongly read!<br>");
		return;
	}
	
	buf_to_patch = m_data + offset;
	
	// TODO: Add a routine which checks the sizes of the file.
	long counter = 0;
	
	while(!Patchfile.eof())
	{
		byte = (unsigned char) Patchfile.get();
		memcpy(buf_to_patch+counter,&byte,1); // one byte every time ;-)
		counter++;
	}
	
	Patchfile.close();
}

void CPatcher::PatchLevelhint(int level)
{
	unsigned char *p_patch;
	unsigned long offset=0;
	unsigned long end=0;

	// Check for which level is it for.
	if(m_episode == 1)
	{
		switch(level)
		{
		case 2:  offset = 0x15080; end = 0x15113; break;
		case 6:  offset = 0x1511A; end = 0x151B3; break;
		case 9:  offset = 0x151B4, end = 0x1524D; break;
		case 10: offset = 0x1524E; end = 0x152E7; break;
		case 11: offset = 0x152E8; end = 0x1523F; break;
		case 12: offset = 0x15340; end = 0x153DA; break;
		case 15: offset = 0x153DB; end = 0x1545E; break;
		}
	}
	else if(m_episode == 2)
	{
		switch(level)
		{
		case 8:  offset = 0x19FCC; end = 0x1A08B; break;
		case 12:  offset = 0x1A08B; end = 0x1A1A0; break;
		}
	}

	p_patch = m_data + offset;

	std::string buf;
	do
	{
		m_TextList.pop_front();
		buf = m_TextList.front();

		memcpy(p_patch, buf.c_str(), buf.size());
		p_patch += buf.size();
		if( p_patch == m_data+end ) break;
	} while( !m_TextList.empty() && !strCaseStartsWith(buf,"%") &&
			 !buf.empty() && !strCaseStartsWith(buf,"\r") );

	// Fill the rest with zeros, so the old won't be shown
	if(end > offset)


		memset( p_patch, 0, end-offset);
}

CPatcher::~CPatcher() {}


