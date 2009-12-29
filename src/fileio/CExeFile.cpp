/*
 * CExeFile.cpp
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 */

#include "CExeFile.h"
#include "Cunlzexe.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include "../StringUtils.h"
#include "../FindFile.h"
#include "../CLogFile.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x=NULL; }

CExeFile::CExeFile(int episode, const std::string& datadirectory) {
	m_episode = episode;
	m_datadirectory = datadirectory;
	if( m_datadirectory != "") if(*(m_datadirectory.end()-1) != '/') m_datadirectory += "/";
	m_rawdata = NULL;
	m_data = NULL;
	m_headerdata = NULL;

	crc32_init();
}

CExeFile::~CExeFile() {
	SAFE_DELETE_ARRAY(m_data);
}

bool CExeFile::readData()
{
	std::string filename =  m_datadirectory + "keen" + itoa(m_episode) + ".exe";

	std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

	if(!File)
	{
		// try another filename (Used in Episode 4-6)
		std::string filename =  m_datadirectory + "keen" + itoa(m_episode) + "e.exe";
		OpenGameFileR(File, filename, std::ios::binary);
		if(!File)
		{
			g_pLogFile->textOut(RED,"Error the executable \"" + filename + "\" is missing!");
			return false;
		}
	}

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

		case 251712:
			if(m_episode != 4)
				return -1;
			else
				return 140;

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
        {
        	// TODO: CRC-Flags for Episode 4 must be implemented here!
        	return 1;
        }
		default: return -2;
    }
}

unsigned char* CExeFile::getRawData()
{	return m_rawdata;	}

unsigned char* CExeFile::getHeaderData()
{	return m_headerdata;	}

