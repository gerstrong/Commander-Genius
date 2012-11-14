/*
 * CPatcher.cpp
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#include "CPatcher.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include "FindFile.h"
#include "StringUtils.h"
#include "CLogFile.h"
#include "fileio/ResourceMgmt.h"
#include "common/CBehaviorEngine.h"


CPatcher::CPatcher(CExeFile &ExeFile, bool &is_a_mod) :
m_is_a_mod(is_a_mod)
{
	m_episode = ExeFile.getEpisode();
	m_version = ExeFile.getEXEVersion();
	m_data = ExeFile.getRawData();
	m_datadirectory = ExeFile.getDataDirectory();
	m_datasize = ExeFile.getExeDataSize();
	m_is_a_mod = false;
}


void CPatcher::process()
{
	if(!loadPatchfile()) return;

	// If the file was found and read into the m_TextList,
	// then read out of the list the patch commands and apply them to the
	// Exe-file data m_data
	
	g_pLogFile->textOut("Trying to load and apply the patch we found...<br>");
	m_is_a_mod = true;
	
	filterPatches(m_TextList);

	patch_item PatchItem;

	std::string dumpfilename = "";

	// TODO: Extend this part further with more commands
	while(readNextPatchItem(PatchItem, m_TextList) == true)
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
					g_pLogFile->textOut("Error! The patch-file does not match the opened episode!!!<br>");
					return;
				}
			}
		}
		else if(PatchItem.keyword == "patchfile")
		{
			std::string newbuf = PatchItem.value.front();

			replace(newbuf, "\"", ""); // In case some patches use the \" remove it!

			// Seperate the offset and the filename
			size_t p = newbuf.find(' ');

			long offset;
			if( readIntValue(newbuf.substr(0,p), offset) )
			{
				std::string patch_file_name = newbuf.substr(p);
				TrimSpaces(patch_file_name);

				patchMemfromFile(m_datadirectory + "/" + patch_file_name,offset);
			}
		}
		else if(PatchItem.keyword == "egahead")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
			gpResource->egaheadFilename = newfileName;
		}
		else if(PatchItem.keyword == "egadict")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
			
			gpResource->egadictFilename = newfileName;
		}
		else if(PatchItem.keyword == "gamemaps")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
			gpResource->gamemapsFilename = newfileName;
		}
		else if(PatchItem.keyword == "maphead")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
			gpResource->mapheadFilename = newfileName;
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
						// In this case we have a number
						m_data[offset] = number;
						offset++;
					}
					else if(readPatchString(textline, patchtext))
					{
						size_t textsize = patchtext.size();

						if(offset + textsize > m_datasize)
						{
							g_pLogFile->textOut("Patch addresses exceed the file size<br>");
							break;
						}

						memcpy( m_data+offset, patchtext.c_str(), textsize);
						offset += textsize;
					}
				}
			}
			else
				g_pLogFile->textOut("Something is wrong with the \%patch command. A number was expected<br>");
		}
		else if(PatchItem.keyword == "level.hint" )
		{
			// Patch the level hints
			std::string textline = readPatchItemsNextValue(PatchItem.value);
			long number = 0;

			if(readIntValue(textline, number))
			{
				// You have a level hint. Very good, lets read it and patch!
				PatchLevelhint(number,PatchItem.value);
			}
		}
		else if(PatchItem.keyword == "dump" )
		{
			std::string textline = readPatchItemsNextValue(PatchItem.value);
			if(readPatchString(textline, dumpfilename))
				g_pLogFile->textOut("CG will dump out the exe-content to \""+ dumpfilename +"\"<br>" );
		}
		else if(PatchItem.keyword == "end" )
		{
			g_pLogFile->textOut("End of Patchfile!<br>");
			break;
		}
		else
		{
		    // If not recognized put the patch on the post-list. In the later Process it will 
		    // judge valid status
		    mPostPatchItems.push_back(PatchItem);
		}

		if(dumpfilename!="")
		{
			std::ofstream DumpFile;
			if(OpenGameFileW(DumpFile,dumpfilename,std::ios::binary))
			{
				DumpFile.write(reinterpret_cast<const char*>(m_data),m_datasize);
				DumpFile.close();
			}
		}

		PatchItem.keyword.clear();
		PatchItem.value.clear();
	}
}


void CPatcher::postProcess()
{
  // Mods only!
  if(!m_is_a_mod)
    return;
	
	auto it = mPostPatchItems.begin();
	
	for( ; it != mPostPatchItems.end() ; it++ )
	{
		if(it->keyword == "level.entry" )
		{
			// Patch the entry level text
			std::string textline = readPatchItemsNextValue(it->value);
			long number = 0;

			if(readIntValue(textline, number))
			{
				// Got the number, patch it!
				PatchLevelentry(number, it->value);
			}
		}
		else
		{
		    g_pLogFile->textOut("The Keyword \"" + it->keyword + "\" is unknown to Commander Genius.");
		}

		it->keyword.clear();
		it->value.clear(); 
	}
}



struct PatchListFiller
{
	std::set<std::string> list;
	
	bool operator() (const std::string& filename) {
		std::string ext = GetFileExtension(filename);
		if (stringcaseequal(ext, "pat"))
			list.insert(filename);
		
		return true;
	}
};

/**
 * \brief this reads the patch into the m_TextList and m_PostTextList
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
		g_pLogFile->textOut(PURPLE,"Multiple Patches are not yet supported! Please remove a file. Taking one File.<br>");
	
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
	
	long counter = 0;
	while(!Patchfile.eof())
	{
		byte = (unsigned char) Patchfile.get();
		memcpy(buf_to_patch+counter,&byte,1); // one byte every time ;-)
		counter++;
	}
	
	Patchfile.close();
}

void CPatcher::PatchLevelhint(const int level, std::list<std::string> &input)
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
		case 11: offset = 0x152E8; end = 0x1533F; break;
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

	// Fill everything with zeros, so the old text won't be shown
	if(end > offset)
		memset( p_patch, 0, end-offset);

	std::string buf;
	do
	{
		buf = input.front();
		memcpy(p_patch, buf.c_str(), buf.size());
		input.pop_front();
		p_patch += buf.size()-1;
		if(*p_patch != '\r')
			p_patch++;
		p_patch[0] = 0x0A;
		p_patch[1] = 0x00;
		p_patch += 2;
		if( p_patch == m_data+end ) break;
	} while( !input.empty() );
}


void CPatcher::PatchLevelentry(const int level, std::list<std::string> &input)
{
	std::string buf;
	std::string levelStr;
	
	if(level == 0)
	  levelStr = "WORLDMAP_LOAD_TEXT";
	else
	  levelStr = "LEVEL"+ itoa(level) +"_LOAD_TEXT";	
	
	while( !input.empty() )
	{
	    buf += input.front();
	    input.pop_front();
	    
	    if( input.size() > 1 ) 
	      buf += "\n";  
	} 
	
	g_pBehaviorEngine->setMessage(levelStr, buf);
}


