/*
 * CSoundSlot.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#define MAX_STRING_LENGTH	256
#define MAX_SOUNDS				50

#define MAX_STACK_SPACE      	1024
#define SOUND_FREQ_DIVISOR    	1363634 // The past original one

#include <stdio.h>
#include <string.h>
#include "CSoundSlot.h"
#include "../../CLogFile.h"
#include "../../fileio.h"

CSoundSlot::CSoundSlot() {
	m_priority = 0;
	m_sounddata = NULL;
	m_hqsound.enabled = false;
	m_hqsound.sound_buffer = NULL;
	m_soundlength = 0;
}

CSoundSlot::~CSoundSlot() {
	HQSndDrv_Unload(&m_hqsound);
	if(m_sounddata){ delete[] m_sounddata; m_sounddata = NULL; }
	m_priority = 0;
	m_hqsound.enabled = false;
}

// loads sound searchname from file fname, into sounds[] entry loadnum
// return value is nonzero on failure
bool CSoundSlot::loadSound(const char *fname, const char *searchname, unsigned int loadnum)
{
	// Unload the sound if any was previously loaded
	if(m_sounddata){ delete[] m_sounddata; m_sounddata = NULL; }

	// If a high quality sound file is available, try to open it.
	// Otherwise open the classic sounds from the original data files
	if(HQSndDrv_Load(m_pAudioSpec, &m_hqsound, searchname) == 0)
	{
		return true;
	}
	else
	{
		FILE *fp;
		int curheader = 0x10;
		int i,j;
		int offset, priority, garbage, nr_of_sounds;
		int sndbyte;
		char name[12];

		for(i=0;i<12;i++)
			name[i] = 0;

		fp = fopen(fname, "rb");
		if (!fp)
		{
			g_pLogFile->ftextOut("loadSound : Sounds file '%s' unopenable attempting load of '%s'<br>", fname, searchname);
			return false;
		}

		fseek(fp, 0x6, SEEK_SET);
		nr_of_sounds = fgeti(fp);

		j=0;
		for(j=0;(j<nr_of_sounds)||feof(fp);j++)
		{
			fseek(fp, curheader, SEEK_SET);
			offset = fgeti(fp);
			priority = fgetc(fp);
			garbage = fgetc(fp);
			for(i=0;i<12;i++) name[i] = fgetc(fp);
			if (!strcmp(name, searchname)) goto sound_found;

			curheader += 0x10;
		}
		// sound could not be found
		g_pLogFile->ftextOut("loadSound : sound %s could not be found in %s.<br>", searchname, fname);
		fclose(fp);
		return false;

		sound_found: ;
		// use stacking method for reading sound
		unsigned int tempstack[MAX_STACK_SPACE];

		m_soundlength = 0;
		fseek(fp, offset, SEEK_SET);
		for(i=0;i<MAX_STACK_SPACE;i++)
		{
			sndbyte = fgeti(fp);
			if (sndbyte != 0x0000 && sndbyte != 0xFFFF)
			{
				tempstack[i] = SOUND_FREQ_DIVISOR / sndbyte;
			}
			else
			{
				tempstack[i] = sndbyte;
			}
			if (sndbyte==0xffff)
			{
				tempstack[i] = sndbyte;
				m_soundlength++;
				break;
			}

			m_soundlength++;
		}
		// copy the data to the real m_sounddata block and reduce the space!
		m_sounddata = new unsigned int[m_soundlength];

		memcpy(m_sounddata, tempstack, m_soundlength*sizeof(unsigned int));

		g_pLogFile->ftextOut("loadSound : loaded sound %s of %d bytes.<br>", searchname, m_soundlength);
		m_hqsound.enabled = false;

		fclose(fp);
		return true;
	}
}

