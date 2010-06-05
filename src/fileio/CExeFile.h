/*
 * CExeFile.h
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 *
 *  This special class reads the whole exe-file
 *  into the memory and manages stuff
 *  In later Games, like Keen 4. The Exe Files must be opened many times
 *  for several reason. We are trying to avoid this here, by pushing all data
 *  of the exe-file into our memory and do the operations here!
 */

#ifndef CEXEFILE_H_
#define CEXEFILE_H_

#include "../fileio/crc.h"

#include <vector>
#include <string>

class CExeFile {
public:
	CExeFile();
	
	char getEpisode();
	std::string getDataDirectory();
	size_t getExeDataSize();

	bool readData(const char episode, const std::string& datadirectory);
	int getEXEVersion();
	int getEXECrc();
	bool readExeImageSize(unsigned char *p_data_start, unsigned long *imglen, unsigned long *headerlen);
	
	unsigned char* getRawData();
	unsigned char* getHeaderData();

	virtual ~CExeFile();

private:

	struct EXE_HEADER
	{
		unsigned short mzid;
		unsigned short image_l;
		unsigned short image_h;
		unsigned short num_relocs;
		unsigned short header_size;
		unsigned short min_paras;
		unsigned short max_paras;
		unsigned short init_ss;
		unsigned short init_sp;
		unsigned short checksum;
		unsigned short init_ip;
		unsigned short init_cs;
		unsigned short reloc_offset;
		unsigned short overlay_num;
	};

	size_t m_datasize;
	int m_episode;
	unsigned int m_crc;
	unsigned char *m_data;
	unsigned char *m_headerdata;
	unsigned char *m_rawdata;
	std::string m_datadirectory;
};

#endif /* CEXEFILE_H_ */
