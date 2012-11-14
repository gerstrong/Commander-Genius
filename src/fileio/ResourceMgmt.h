/*
 * ResourceMgmt.h
 *
 *  Created on: 16.05.2010
 *      Author: gerstrong
 */

#ifndef RESOURCEMGMT_H_
#define RESOURCEMGMT_H_

#include <string>
#include <set>
#include "StringUtils.h"
#include "CSingleton.h"


#define gpResource CResource::Get()

struct CResource : public CSingleton<CResource>
{
	std::string egadictFilename;
	std::string egaheadFilename;
	std::string egagraphFilename;
	std::string audioFilename;
	std::string mapheadFilename;
	std::string gamemapsFilename;

	void setupFilenames(const unsigned int episode)
	{
	    const std::string epStr = itoa(episode);
	    
	    if(episode <= 6)
	    {
		egadictFilename = "egadict.ck" + epStr;
		egagraphFilename = "egagraph.ck" + epStr;
		audioFilename = "audio.ck" + epStr;
		gamemapsFilename = "gamemaps.ck" + epStr;
	    }
	    else // Keen Dreams
	    {
		egadictFilename = "KDREAMSDICT.EGA";
	    }
	}
};



struct FileListAdder {
    void operator()(std::set<std::string>& dirs, const std::string& path) {
        std::string basepath = GetBaseFilename(path);
        if(basepath != "" && basepath[0] != '.') {
            dirs.insert(basepath);
        }
    }
};

int getRessourceID(const std::string& filename, const std::string& namefilter);

void FilterFilelist(std::set<std::string>& filelist, const std::string& namefilter);

std::string getResourceFilename(const std::string& filename, const std::string& gamepath = "",
											bool essential = true, bool can_be_global = false);

#endif /* RESOURCEMGMT_H_ */
