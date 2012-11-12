/*
 * CPatcher.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CPATCHER_H_
#define CPATCHER_H_

#include <list>
#include <string>

#include "CExeFile.h"

class CPatcher {
public:
	CPatcher(CExeFile &ExeFile, bool &is_a_mod);
	
	void patchMemory();
	void patchMemfromFile(const std::string& patch_file_name, long offset);
	void PatchLevelhint(const int level, std::list<std::string> &input);
	void PatchLevelentry(const int level, std::list<std::string> &input);	
	
private:
	
	struct patch_item
	{
		std::string keyword;
		std::list<std::string> value;
	};

	std::string readPatchItemsNextValue(std::list<std::string> &input);
	bool readIntValue(const std::string &input, long &output);
	bool readPatchString(const std::string &input, std::string &output);
	void filterPatches();
	bool readNextPatchItem(patch_item &PatchItem);

	bool loadPatchfile();
	
	int m_episode;
	int m_version;
	unsigned char *m_data;
	std::string m_datadirectory;
	size_t m_datasize;
	bool &m_is_a_mod;
	
	std::list<std::string> m_TextList;
};

#endif /* CPATCHER_H_ */
