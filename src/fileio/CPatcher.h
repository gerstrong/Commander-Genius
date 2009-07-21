/*
 * CPatcher.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CPATCHER_H_
#define CPATCHER_H_

#include <list>
using namespace std;

class CPatcher {
public:
	CPatcher(int episode, int version,unsigned char *data, char *datadir);
	virtual ~CPatcher();

	void patchMemory();
	void patchMemfromFile(const char *patch_file_name, int offset);

private:

	bool loadPatchfile();

	int m_episode;
	int m_version;
	unsigned char *m_data;
	char m_datadirectory[256];

	list<char*> m_TextList;

};

#endif /* CPATCHER_H_ */
