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

CPatcher::CPatcher(int episode, int version,unsigned char *data, const std::string& datadir) {
	m_episode = episode;
	m_version = version;
	m_data = data;
	m_datadirectory = datadir;
}

CPatcher::~CPatcher() {
	while(!m_TextList.empty())
	{
		delete *(m_TextList.begin());
		m_TextList.pop_front();
	}
}

void CPatcher::patchMemory()
{
	if(!loadPatchfile()) return;

	// If the file was found and read into the m_TextList,
	// then read out of the list the patch commands and apply them to the
	// Exe-file data m_data

	// change to the proper directory
	chdir("data");
	chdir(m_datadirectory.c_str());

	// TODO: Extend this part further with more commands
	while(!m_TextList.empty())
	{
		char line[256];

		strcpy(line,*(m_TextList.begin()));

		if(strncmp(line,"\%version",strlen("\%version")) == 0)
		{
			char *verstring;
			char detected_version[5];

			verstring = line + strlen("\%version");

			sscanf(verstring,"%s",detected_version);

			if((!strcmp(detected_version,"1.31") && m_version == 131 )
				|| (!strcmp(detected_version,"1.1") && m_version == 110 )
				|| !strcmp(detected_version,"ALL"))
			{
				while(!m_TextList.empty())
				{
					// Get the next line
					strcpy(line,*(m_TextList.begin()));

					// Now we really start to process the commands
					if( strncmp(line,"\%patchfile",strlen("\%patchfile")) == 0 )
					{
						unsigned long offset;
						char *newbuf;
						char patch_file_name[256];
						newbuf = line + strlen("\%patchfile");
						sscanf(newbuf,"%lx %s",&offset,patch_file_name); // Only hexadecimal numbers supported
						patchMemfromFile((const char*)patch_file_name,offset);
					}

					if(!m_TextList.empty())
					{
						delete *(m_TextList.begin());
						m_TextList.pop_front();
					}
				}
			}
		}

		if(!m_TextList.empty())
		{
			delete *(m_TextList.begin());
			m_TextList.pop_front();
		}
	}

	// change back to "data" dir
	char curdir[256];
	while(1)
	{
		char *reldir;
		getcwd(curdir,256);
		reldir = curdir+strlen(curdir)-strlen("data");
		if(strcmp(reldir,"data"))
			chdir("..");
		else
			break;
	}
	chdir("..");
}

bool CPatcher::loadPatchfile()
{
	bool ret = false;
	chdir("data");
	chdir(m_datadirectory.c_str());
	// Detect the patchfile
	DIR *dir = opendir(".");
	struct dirent *dp;

	if(dir)
	{
		while( ( dp = readdir(dir) ) )
		{
			if(strstr(dp->d_name,".pat"))
			{
				// The file was found! now read it into the memory!

				char* buf;
				std::ifstream Patchfile(dp->d_name);

				while(!Patchfile.eof())
				{
					buf = new char[256];
					Patchfile.getline(buf,256);
					m_TextList.push_back(buf);
				}

				Patchfile.close();

				ret = true;
				break;
			}
		}
	}

	char curdir[256];
	while(1)
	{
		char *reldir;
		getcwd(curdir,256);
		reldir = curdir+strlen(curdir)-strlen("data");
		if(strcmp(reldir,"data"))
			chdir("..");
		else
			break;
	}

	chdir("..");
	closedir(dir);

	return ret;
}

void CPatcher::patchMemfromFile(const std::string& patch_file_name, int offset)
{
	unsigned char *buf_to_patch;
	unsigned char byte;

	std::ifstream Patchfile(patch_file_name.c_str(), std::ios::binary);

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
