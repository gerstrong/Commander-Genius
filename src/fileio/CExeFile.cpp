/*
 * CExeFile.cpp
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 */

#include "CExeFile.h"
#include "compression/Cunlzexe.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include "../StringUtils.h"
#include "../FindFile.h"
#include "../CLogFile.h"
#include "../fileio/ResourceMgmt.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x=NULL; }

CExeFile::CExeFile() :
m_datasize(0),
m_episode(0),
m_crc(0),
m_data(NULL),
m_headerdata(NULL),
m_rawdata(NULL),
m_datadirectory("")
{}

char CExeFile::getEpisode()
{ return m_episode;	}

std::string CExeFile::getDataDirectory()
{ return m_datadirectory;	}

size_t CExeFile::getExeDataSize()
{ return m_datasize;	}

bool CExeFile::readData(const char episode, const std::string& datadirectory)
{
	crc32_init();

	std::string filename = datadirectory + "/keen" + itoa(episode) + ".exe";

	std::ifstream File;
	OpenGameFileR(File, filename, std::ios::binary);

	if(!File)
	{
		// try another filename (Used in Episode 4-6)
		filename = datadirectory + "/keen" + itoa(episode) + "e.exe";
		OpenGameFileR(File, filename, std::ios::binary);
	}

	if(!File)
	{
		// try another filename (Used in Episode 4-6) for demo versions
		filename = datadirectory + "/k" + itoa(episode) + "demo.exe";
		OpenGameFileR(File, filename, std::ios::binary);
	}
	
	if(!File)
		return false;	

	m_filename = filename;
	m_episode = episode;
	m_datadirectory = datadirectory;
	if( m_datadirectory != "") if(*(m_datadirectory.end()-1) != '/') m_datadirectory += "/";
		
	File.seekg(0,std::ios::end);
	m_datasize = File.tellg();
	File.seekg(0,std::ios::beg);

	SAFE_DELETE_ARRAY(m_data);
	unsigned char* m_data_temp = new unsigned char[m_datasize];
	File.read((char*)m_data_temp, m_datasize);

	File.close();

	Cunlzexe UnLZEXE;

	std::vector<unsigned char> decdata;
	unsigned long Headersize = 0;
	if(UnLZEXE.decompress(m_data_temp, decdata))
	{
		m_datasize = decdata.size();
		m_data = new unsigned char[m_datasize];
		Headersize = UnLZEXE.HeaderSize();
		memcpy(m_data, &decdata[0], m_datasize);
	}
	else
	{
		m_data = new unsigned char[m_datasize];
		memcpy(m_data, m_data_temp,m_datasize);
	}

	m_headerdata = m_data;
	Headersize = UnLZEXE.HeaderSize();
	if(!Headersize) Headersize = 512;
	m_rawdata = m_data + Headersize;

	delete[] m_data_temp;

	m_crc = getcrc32( m_data, m_datasize );

	g_pLogFile->ftextOut( "EXE processed with size of %d and crc of %X\n", m_datasize, m_crc );

	return true;
}

int CExeFile::getEXEVersion()
{
    switch (m_datasize)
    {
		case 100274:
			if(m_episode != 1)
				return -1;
			else
				return 110;
		case 100484:
			if(m_episode != 1)
				return -1;
			else
				return 131;
		case 398:
			if(m_episode != 1)
				return -1;
			else
				return 134;

		case 118626:
			if(m_episode != 2)
				return -1;
			else
				return 100;
		case 118672:
			if(m_episode != 2)
				return -1;
			else
				return 131;

		case 127598:
			if(m_episode != 3)
				return -1;
			else
				return 100;
		case 127616:
			if(m_episode != 3)
				return -1;
			else
				return 131;

		case 263488:
			if(m_episode != 4)
				return -1;
			else
				return 140;

		case 259232:
			if(m_episode != 4)
				return -1;
			else
				return 110;

		default: return -2;
    }
}

int CExeFile::getEXECrc()
{
    int version = getEXEVersion();

    switch( m_episode )
    {
        case 1:
            switch( version )
		{
			case 110:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x195771AE)
					return -1;
				else
					return true;
			case 134:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			default: return -2;
		}
        case 2:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x94E464B4)
					return -1;
				else
					return true;
		}
        case 3:
            switch( version )
		{
			case 100:
				if(m_crc != 1)
					return -1;
				else
					return 1;
			case 131:
				if(m_crc != 0x70D3264D)
					return -1;
				else
					return 1;
		}
        case 4:
		case 5:
		case 6:
        {
        	// TODO: CRC-Flags for Episode 4 must be implemented here!
        	return 1;
        }
		default: return -2;
    }
}

const unsigned short EXEMZ = 0x5A4D;
const unsigned short EXEZM = 0x4D5A;

/* SM: Modified so we can give a value of "headerlen" we're expecting... this way
 * we might support any exe file in the future */
bool CExeFile::readExeImageSize(unsigned char *p_data_start, unsigned long *imglen, unsigned long *headerlen)
{
	EXE_HEADER head;

	/* Read the header from the file if we can */
	memcpy(&head, p_data_start,sizeof(EXE_HEADER));
	/* Check that the 'MZ' id is present */
	p_data_start += sizeof(EXE_HEADER);
	if(head.mzid == EXEMZ || head.mzid == EXEZM)
	{
		/* Calculate the image size */
		if (!*headerlen) {
			*imglen = ((unsigned long)head.image_h - 1) * 512L + head.image_l - (unsigned long)head.header_size * 16L;
			*headerlen = (unsigned long)head.header_size * 16L;
		}
		else *imglen = ((unsigned long)head.image_h - 1) * 512L + head.image_l - *headerlen;
		return true;
	}

	// If we got here, something failed
	*imglen = *headerlen = 0;
	return false;
}

unsigned char* CExeFile::getRawData()
{	return m_rawdata;	}

unsigned char* CExeFile::getHeaderData()
{	return m_headerdata;	}

/**
 * \brief This function returns the pointer to the data-segment of our opened exe-file
 */
unsigned char *CExeFile::getDSegOffset()
{
	// TODO: I think those are the offsets of some versions. We should figure out how to scan those
	//		 offsets.

	const size_t offset_map[] = {
			/*Keen 1:*/ 0x13050,
			/*Keen 2:*/ 0x17780,
			/*Keen 3:*/ 0x19820,
			/*Keen 4:*/ 0x2EE70,
			/*Keen 5:*/ 0x30340,
			/*Keen 6:*/ 0x30D30,
			/*Keen D:*/ 0x23A70
	};

	return m_data+offset_map[m_episode];
}

CExeFile::~CExeFile() {
	SAFE_DELETE_ARRAY(m_data);
}

