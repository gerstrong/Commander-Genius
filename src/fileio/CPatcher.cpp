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

CPatcher::~CPatcher() {}

void CPatcher::patchMemory()
{
	if(!loadPatchfile()) return;

	// If the file was found and read into the m_TextList,
	// then read out of the list the patch commands and apply them to the
	// Exe-file data m_data

	// TODO: Extend this part further with more commands
	while(!m_TextList.empty())
	{
		std::string line = *m_TextList.begin();

		if(strCaseStartsWith(line,"\%version"))
		{
			std::string verstring = line.substr(strlen("\%version "));

			if((strCaseStartsWith(verstring,"1.31") && m_version == 131 )
				|| (strCaseStartsWith(verstring,"1.1") && m_version == 110 )
				|| strCaseStartsWith(verstring,"ALL"))
			{
				while(!m_TextList.empty())
				{
					// Get the next line
					line = *m_TextList.begin();

					// Now we really start to process the commands
					if( strCaseStartsWith(line,"\%patchfile") )
					{
						std::string newbuf = line.substr(strlen("\%patchfile"));
						TrimSpaces(newbuf);
						size_t p = newbuf.find(' ');
						if(p != std::string::npos) {
							size_t offset = 0;
							sscanf(newbuf.substr(0,p).c_str(), "%lx", &offset); // Only hexadecimal numbers supported
							std::string patch_file_name = newbuf.substr(p+1);
							TrimSpaces(patch_file_name);
							patchMemfromFile("data/" + m_datadirectory + "/" + patch_file_name,offset);
						}						
					}

					if(!m_TextList.empty())
					{
						m_TextList.pop_front();
					}
				}
			}
		}

		if(!m_TextList.empty())
		{
			m_TextList.pop_front();
		}
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
	std::string path = "data/" + m_datadirectory;

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

void CPatcher::patchMemfromFile(const std::string& patch_file_name, int offset)
{
	unsigned char *buf_to_patch;
	unsigned char byte;

	std::ifstream Patchfile; OpenGameFileR(Patchfile, patch_file_name, std::ios::binary);

	if(!Patchfile) return;

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
