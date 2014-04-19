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
//#include "StringUtils.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include "fileio/ResourceMgmt.h"

CExeFile::CExeFile() :
m_datasize(0),
m_headersize(0),
m_episode(0),
m_crc(0),
m_headerdata(NULL),
m_rawdata(NULL),
m_datadirectory("")
{

	// Setup support map
	m_supportmap[100274][1] = true;
	m_supportmap[100484][1] = true;
	m_supportmap[398][1] = false;

	m_supportmap[118626][2] = true;
	m_supportmap[118672][2] = true;

	m_supportmap[127598][3] = true;
	m_supportmap[127616][3] = true;

	m_supportmap[263488][4] = true;
	m_supportmap[259232][4] = false;
	m_supportmap[258064][4] = false;

	m_supportmap[266096][5] = true;
	m_supportmap[262176][5] = false;

	m_supportmap[236112][6] = false;
	m_supportmap[271696][6] = false;


	// TODO: Setup version map

	// TODO: Setup CRC-Map

}

char CExeFile::getEpisode() const
{ return m_episode;	}

std::string CExeFile::getDataDirectory() const
{ return m_datadirectory;	}

size_t CExeFile::getExeDataSize() const
{ return m_datasize;	}

unsigned long CExeFile::fetchUncompressedHeaderSize(void *m_headerdata)
{
	byte *hdata = reinterpret_cast<byte*>(m_headerdata);
	for( unsigned long c=0 ; c<m_datasize ; c++)
	{
		if( hdata[c] == 0xBA && c % 0x200 == 0 )
			return c;
	}

	gLogging.textOut("The Exe file has an invalid header size!");

	return 0;
}

void CExeFile::dumpFile(const std::string& filename)
{
	std::ofstream ofile( filename.c_str() );
	ofile.write( reinterpret_cast<char*>(m_headerdata), m_datasize );
}

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

	unsigned char* m_data_temp = new unsigned char[m_datasize];
	File.read((char*)m_data_temp, m_datasize);

	File.close();

	Cunlzexe UnLZEXE;

	std::vector<unsigned char> decdata;
	if(UnLZEXE.decompress(m_data_temp, decdata))
	{
		m_datasize = decdata.size();
		mData.resize(m_datasize);
		m_headersize = UnLZEXE.HeaderSize();
		memcpy(mData.data(), &decdata[0], m_datasize);
	}
	else
	{
		mData.resize(m_datasize);
		memcpy(mData.data(), m_data_temp,m_datasize);
	}

	m_headerdata = mData.data();
	m_headersize = UnLZEXE.HeaderSize();
	if(!m_headersize)
		m_headersize = fetchUncompressedHeaderSize(m_headerdata);

	m_rawdata = mData.data() + m_headersize;

	const size_t offset_map[] = {
			/*Dummy:*/ 0x0,
			/*Keen 1:*/ 0x13050,
			/*Keen 2:*/ 0x17780,
			/*Keen 3:*/ 0x19820,
			/*Keen 4:*/ 0x2EE70,
			/*Keen 5:*/ 0x30340,
			/*Keen 6:*/ 0x30D30,
			/*Keen D:*/ 0x23A70
	};
	m_data_segment = m_rawdata+offset_map[m_episode];

	delete[] m_data_temp;

	m_crc = getcrc32( mData.data(), m_datasize );

	gLogging.ftextOut( "EXE processed with size of %d and crc of %X\n", m_datasize, m_crc );

	return true;
}

bool CExeFile::Supported()
{

	if( m_supportmap.find(m_datasize) == m_supportmap.end())
		return false;

	if( m_supportmap[m_datasize].find(m_episode) == m_supportmap[m_datasize].end() )
		return false;

	return  m_supportmap[m_datasize][m_episode];
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

		// For Keen 4
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
		case 258064:
			if(m_episode != 4)
				return -1;
			else
				return 100;

		// For Keen 5
		case 266096:
			if(m_episode != 5)
				return -1;
			else
				return 140;

		case 262176:
			if(m_episode != 5)
				return -1;
			else
				return 100;

		// For Keen 6
		case 236112:
			if(m_episode != 6)
				return -1;
			else
				return 100;

		case 271696:
			if(m_episode != 6)
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
bool CExeFile::readExeImageSize(unsigned char *p_data_start, unsigned long *imglen, unsigned long *headerlen) const
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

byte* CExeFile::getRawData() const
{	return m_rawdata;	}

void* CExeFile::getHeaderData() const
{	return m_headerdata;	}

byte* CExeFile::getDSegPtr() const
{	return m_data_segment; }


