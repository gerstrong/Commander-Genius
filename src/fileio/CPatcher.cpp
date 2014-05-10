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
#include <base/utils/FindFile.h>
//#include "StringUtils.h"
#include "fileio/KeenFiles.h"
#include <base/GsLogging.h>
#include "fileio/ResourceMgmt.h"
#include "fileio/KeenFiles.h"
#include "engine/core/CBehaviorEngine.h"


CPatcher::CPatcher(CExeFile &ExeFile, const std::string &patchFname) :
mPatchFname(patchFname)
{
	m_episode = ExeFile.getEpisode();
	m_version = ExeFile.getEXEVersion();
	m_data = ExeFile.getRawData();
	m_datasize = ExeFile.getExeDataSize();
}


// Some mods seems to have messy pat file! If a filename is given but some more mess
// this routine cuts it down!
void filterFilename(std::string &fileName)
{
    // Those filenames can only have extensions up to three chars. Cut it down...
    const size_t pos = fileName.find(".")+4;
    const std::string newFileName = fileName.substr(0,pos);
    fileName = newFileName;
}


void CPatcher::process()
{
    if(!loadPatchfile(mPatchFname)) return;

	// If the file was found and read into the m_TextList,
	// then read out of the list the patch commands and apply them to the
	// Exe-file data m_data

	gLogging.textOut("Trying to load and apply the patch we found...<br>");

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
					gLogging.textOut("Error! The patch-file does not match the opened episode!!!<br>");
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

			long unsigned int offset;
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
            gKeenFiles.egaheadFilename = newfileName;
		}
		else if(PatchItem.keyword == "egadict")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);

            gKeenFiles.egadictFilename = newfileName;
		}
		else if(PatchItem.keyword == "audiohed")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);

            gKeenFiles.audioHedFilename = newfileName;
		}
		else if(PatchItem.keyword == "audiodict")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);

            gKeenFiles.audioDictFilename = newfileName;
		}
		else if(PatchItem.keyword == "gamemaps")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
            gKeenFiles.gamemapsFilename = newfileName;
		}
		else if(PatchItem.keyword == "maphead")
		{
			std::string newfileName = PatchItem.value.front();
			TrimSpaces(newfileName);
            filterFilename(newfileName);
            gKeenFiles.mapheadFilename = newfileName;
		}
		else if(PatchItem.keyword == "patch" )
		{
			// first we need to get the offset
			long unsigned int offset = 0;
			std::string textline = readPatchItemsNextValue(PatchItem.value);						
			size_t width;
			if(readIntValue(textline, offset))
			{
                // Terror from outer-space workaround to avoid it crash!
                if(offset == 0x3D740 && m_episode == 4)
                {
                    gLogging.textOut("Appling TFOS mod workaround...");
                    continue;
                }

				while(!PatchItem.value.empty())
				{
					// after we have it, distinguish between text case and number case
					long unsigned int number = 0;
					std::string patchtext = "";
					textline = readPatchItemsNextValue(PatchItem.value);

					if(readIntValueAndWidth(textline, number, width))
					{
						// In this case we have a number
						memcpy(m_data+offset, &number, width);
						offset+=width;
					}
					else if(readPatchString(textline, patchtext))
					{
						size_t textsize = patchtext.size();

						if(offset + textsize > m_datasize)
						{
							gLogging.textOut("Patch addresses exceed the file size<br>");
							break;
						}

						memcpy( m_data+offset, patchtext.c_str(), textsize);
						offset += textsize;
					}
				}
			}
			else
				gLogging.textOut("Something is wrong with the \%patch command. A number was expected<br>");
		}
		else if(PatchItem.keyword == "level.hint" )
		{
			// Patch the level hints
			std::string textline = readPatchItemsNextValue(PatchItem.value);
			long unsigned int number = 0;

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
				gLogging.textOut("CG will dump out the exe-content to \""+ dumpfilename +"\"<br>" );
		}
		else if(PatchItem.keyword == "end" )
		{
			gLogging.textOut("End of Patchfile!<br>");
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
  if(mPatchFname.empty())
    return;

	auto it = mPostPatchItems.begin();

	for( ; it != mPostPatchItems.end() ; it++ )
	{
		if(it->keyword == "level.entry" )
		{
			// Patch the entry level text
			std::string textline = readPatchItemsNextValue(it->value);
			long unsigned int number = 0;

			if(readIntValue(textline, number))
			{
				// Got the number, patch it!
				PatchLevelentry(number, it->value);
			}
		}
		else
		{
		    gLogging.textOut("The Keyword \"" + it->keyword + "\" is unknown to Commander Genius.");
		}

		it->keyword.clear();
		it->value.clear();
	}
}



/**
 * \brief this reads the patch into the m_TextList and m_PostTextList
 * \return	true if something could be read. Otherwise false
 */
bool CPatcher::loadPatchfile(const std::string &patchFname)
{
    std::ifstream Patchfile;
    if(!OpenGameFileR(Patchfile, patchFname))
        return false;

    while(!Patchfile.eof())
    {
        char buf[256];
        Patchfile.getline(buf, sizeof(buf));
        fix_markend(buf);
        m_TextList.push_back(buf);
    }
    Patchfile.close();

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
		gLogging.textOut(PURPLE,"Warning: The patchfile was wrongly read!<br>");
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


