/*
 * CPatcher.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CPatcher.h"
#include <dirent.h>
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
	
	std::string line;
	bool ignorelines=false; // ignore the lines which are read. This happens, when patch files are created
	// for multiple keen versions (1.1, 1.34)
	
	// TODO: Extend this part further with more commands
	while(!m_TextList.empty())
	{
		// Get the next line
		line = *m_TextList.begin();
		
		if( (strCaseStartsWith(line,"\%version 1.1") && m_version == 131) ||
		   (strCaseStartsWith(line,"\%version 1.31") && m_version == 110) )
			ignorelines = true; // If %version detected and no match ignore the other lines
		
		if( (strCaseStartsWith(line,"\%version 1.31") && m_version == 131) ||
		   (strCaseStartsWith(line,"\%version 1.1") && m_version == 110) ||
		   strCaseStartsWith(line,"\%version ALL")							)
			ignorelines = false; // If the line matches don't ignore them anymore
		
		if( !ignorelines )
		{
			// Now we really start to process the commands
			if( strCaseStartsWith(line,"\%patchfile") )
			{
				std::string newbuf = line.substr(strlen("\%patchfile"));
				TrimSpaces(newbuf);
				size_t p = newbuf.find(' ');
				if(p != std::string::npos) {
					
					long offset = 0;
					sscanf(newbuf.substr(0,p).c_str(), "%lx", &offset); // Only hexadecimal numbers supported
					std::string patch_file_name = newbuf.substr(p+1);
					TrimSpaces(patch_file_name);
					
					p = patch_file_name.find(' ');
					if( p != std::string::npos )
						patch_file_name.erase(p);
					
					patchMemfromFile(m_datadirectory + "/" + patch_file_name,offset);
				}
			}
			else if( strCaseStartsWith(line,"\%patch") )
			{
				std::string newbuf = line.substr(strlen("\%patch"));
				TrimSpaces(newbuf);
				size_t p = newbuf.find(' ');

				if( strCaseStartsWith(newbuf,"0x") ){
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
				}
			}
			else if( strCaseStartsWith(line,"\%level.hint") )
			{
				// You have a level hint. Very good, lets read it and patch!
				std::string newbuf = line.substr(strlen("\%level.hint"));
				TrimSpaces(newbuf);
				PatchLevelhint(atoi(newbuf));
			}

		}
		
		if(!m_TextList.empty())
			m_TextList.pop_front();
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

bool CPatcher::loadPatchfile()
{
	std::string path = m_datadirectory;
	
	//Get the list of ".pat" files
	PatchListFiller patchlist;
	FindFiles(patchlist, path, false, FM_REG);
	
	// Nothing to play, just quit
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

// This is used for patching. I didn't think we could get it that small.
void CPatcher::patchMemFromText(unsigned long offset, std::string &patchtext)
{
	memcpy( m_data+offset, patchtext.c_str(), patchtext.size());
	return;
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


