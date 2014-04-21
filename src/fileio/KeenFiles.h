/*
 * ResourceMgmt.h
 *
 *  Created on: 18.04.2014
 *      Author: gerstrong
 */

#ifndef __KEENFILES_H__
#define __KEENFILES_H__

#include <string>
#include <set>
#include <base/utils/StringUtils.h>
#include "CSingleton.h"
#include "CExeFile.h"

#define gpKeenFiles CResource::Get()

// TODO: We should rename that class...
struct CResource : public CSingleton<CResource>
{
	std::string egagraphFilename;
	std::string egaheadFilename;
	std::string egadictFilename;
	std::string audioFilename;
	std::string audioHedFilename;
	std::string audioDictFilename;
	std::string mapheadFilename;
	std::string gamemapsFilename;    
    CExeFile exeFile;

	void setupFilenames(const unsigned int episode)
	{
	    const std::string epStr = itoa(episode);
	    
	    if(episode <= 6)
	    {
            // Only the files which are standard can be set.
            // If other files are in the directory, the patch files have assign that!
            egagraphFilename = "egagraph.ck" + epStr;
            egaheadFilename = "egahead.ck" + epStr;
            egadictFilename = "egadct.ck" + epStr;
            audioFilename = "audio.ck" + epStr;
            audioHedFilename = "audiohed.ck" + epStr;
            audioDictFilename = "audiodct.ck" + epStr;
            gamemapsFilename = "gamemaps.ck" + epStr;
            mapheadFilename = "maphead.ck" + epStr;
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


#endif /* __KEENFILES_H__ */
