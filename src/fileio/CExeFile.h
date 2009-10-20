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
	unsigned char* getData();

private:
	int m_datasize;
	int m_episode;
	unsigned int m_crc;
	unsigned char *m_data;
	std::string m_datadirectory;

	int get_bit(int *p_bit_count, unsigned char *fin, int *posin);
	int unlzexe(unsigned char *fin, std::vector<unsigned char> *outbuffer);
};

#endif /* CEXEFILE_H_ */
