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

#include <vector>
using namespace std;

class CExeFile {
public:
	CExeFile(int episode, char *datadirectory);
	virtual ~CExeFile();

	bool readData();
	int getEXEVersion();
	unsigned char* getData();

private:
	int m_datasize;
	int m_episode;
	unsigned char *m_data;
	char *m_datadirectory;

	int get_bit(int *p_bit_count, unsigned char *fin, int *posin);
	int unlzexe(unsigned char *fin, vector<unsigned char> *outbuffer);
};

#endif /* CEXEFILE_H_ */
