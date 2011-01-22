/*
 * CSoundSlot.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include "CSoundSlot.h"
#include "CLogFile.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/TypeDefinitions.h"
#include "FindFile.h"

CSoundSlot::CSoundSlot() :
m_sounddata(NULL),
m_soundlength(0),
m_pAudioSpec(NULL)
{
	m_sounddata = NULL;
	m_soundlength = 0;
	m_pAudioSpec = NULL;
}

// loads sound searchname from file fname, into sounds[] entry loadnum
// return value is false on failure
bool CSoundSlot::loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum)
{
	// Unload the sound if any was previously loaded
	if(m_sounddata){ delete[] m_sounddata; }
	m_sounddata = NULL;
	
	// If a high quality sound file is available, try to open it.
	// Otherwise open the classic sounds from the original data files
	if(HQSndDrv_Load(m_pAudioSpec, &m_hqsound, m_gamepath, searchname) == 0)
	{
		return true;
	}
	else
	{
		int curheader = 0x10;
		int offset, priority, garbage, nr_of_sounds;
		char name[12];
		
		memset(name,0,12);
		Uint8 *buf_ptr = buffer+0x6;

		nr_of_sounds = READWORD(buf_ptr);

		for(int j=0; j<nr_of_sounds || (buf_ptr-buffer < buf_size) ; j++)
		{
			buf_ptr = buffer+curheader;
			offset = READWORD(buf_ptr);
			priority = *buf_ptr++;
			garbage = *buf_ptr++;

			for(int i=0;i<12;i++) name[i] = *buf_ptr++;
			if (name == searchname)
			{
				buf_ptr = buffer+offset;

				signed int sample;
				std::vector<unsigned int> waveform;
				do
				{
					sample = READWORD(buf_ptr);
					waveform.push_back( (sample != 0x0000 && sample != 0xFFFF) ? (0x1234DD/sample) : sample );
				}while (sample != 0xffff);

				m_soundlength = waveform.size();

				// copy the data to the real m_sounddata block and reduce fragmentation!
				m_sounddata = new unsigned int[m_soundlength];

				memcpy(m_sounddata, &waveform[0], waveform.size()*sizeof(unsigned int));

				g_pLogFile->ftextOut("loadSound : loaded sound %s of %d bytes.<br>", searchname.c_str(), m_soundlength);
				m_hqsound.enabled = false;

				return true;
			}
			curheader += 0x10;
		}
		// sound could not be found
		g_pLogFile->ftextOut("loadSound : sound \"%s\" could not be found.<br>", searchname.c_str());

		return false;
	}
}

void CSoundSlot::unload()
{
	HQSndDrv_Unload(&m_hqsound);
	if(m_sounddata){ delete[] m_sounddata; }
	m_sounddata = NULL;
	m_hqsound.enabled = false;
}

CSoundSlot::~CSoundSlot() {
	unload();
}


