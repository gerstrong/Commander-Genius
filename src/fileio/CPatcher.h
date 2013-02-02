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

#ifdef __APPLE__
#define ulong unsigned long
#define uint unsigned int
#endif

class CPatcher {
public:
	CPatcher(CExeFile &ExeFile, bool &is_a_mod);

	void process();
	void postProcess();
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
	bool readIntValueAndWidth(const std::string &input, size_t &output, size_t &width);
	bool readIntValue(const std::string &input, size_t &output);
	bool readPatchString(const std::string &input, std::string &output);
	void filterPatches(std::list<std::string> &textlist);
	bool readNextPatchItem(patch_item &PatchItem, std::list<std::string> &textList);

	bool loadPatchfile();

	int m_episode;
	int m_version;
	unsigned char *m_data;
	std::string m_datadirectory;
	size_t m_datasize;
	bool &m_is_a_mod;

	std::list<std::string> m_TextList;

	// This is a list of patch items which go through the post process of the engine
	// Some items need to be patched after all resources have been loaded.
	// It makes some modding much more flexible
	std::list<patch_item> mPostPatchItems;
};

#endif /* CPATCHER_H_ */
