/*
 * CExeFile.h
 *
 *  Created on: 17.07.2009
 *      Author: gerstrong
 *
 *  This special class reads the whole exe-file
 *  into the memory and decompresses if necessary
 */

#ifndef CEXEFILE_H_
#define CEXEFILE_H_

#include "../fileio/crc.h"

#include <vector>
#include <string>

class CExeFile {
public:
	CExeFile(int episode, const std::string& datadirectory);
	virtual ~CExeFile();
	
	bool readData();
	int getEXEVersion();
	int getEXECrc();
	
	unsigned char* getRawData();
	unsigned char* getHeaderData();

private:
	int m_datasize;
	int m_episode;
	unsigned int m_crc;
	unsigned char *m_data;
	unsigned char *m_headerdata;
	unsigned char *m_rawdata;
	std::string m_datadirectory;
};

#endif /* CEXEFILE_H_ */
