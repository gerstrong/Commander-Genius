/*
 * ActionFormat.cpp
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 *
 *  Action-Format is released as CXX Module because of some dependencies
 */


#include "ActionFormat.h"
#include "engine/core/CBehaviorEngine.h"
#include "fileio/KeenFiles.h"

#include <base/GsLogging.h>
#include <base/utils/FindFile.h>

#include <fstream>
#include <vector>

std::vector<byte> actionFormatData;

bool loadActionFile(const std::string &actFilePath)
{
    std::ifstream file(actFilePath.c_str());

    if(!file)
    {
        return false;
    }

    file.seekg(0, file.end);
    const int fileSize = file.tellg();
    file.seekg(0, file.beg);

    actionFormatData.resize(fileSize);

    file.read(reinterpret_cast<char*>(actionFormatData.data()), fileSize);

    return true;
}

byte *getActionBasePtr()
{
    auto &exeFile = gKeenFiles.exeFile;

    if(exeFile.isPythonScript())
    {
        if(actionFormatData.empty())
        {
            int episode = exeFile.getEpisode();

            const std::string fname = "keen" + itoa(episode) + ".act";

            const auto actFilePath = JoinPaths(gKeenFiles.gameDir,fname);
            const auto actFullFilePath = GetFullFileName(actFilePath);

            gLogging << "Loading Action file " << actFullFilePath;

            if(actFullFilePath == "")
            {
                gLogging << "Error Loading Action file " << actFullFilePath;
                return nullptr;
            }

            loadActionFile(actFullFilePath);
        }

        return actionFormatData.data();
    }
    else
    {
        return exeFile.getDSegPtr();
    }
}

void ActionFormatType::setActionFormat( const size_t sprite_offset )
{
    byte *ptr = getActionBasePtr();
	ptr += sprite_offset;
	memcpy( this, ptr, 15*sizeof(int16_t) );	
}





void ActionFormatType::setNextActionFormat()
{
	setActionFormat(Next_action);
}



bool ActionFormatType::getActionFormat( const size_t sprite_offset )
{
    byte *ptr = getActionBasePtr();
	ptr += sprite_offset;
	return (memcmp( this, ptr, 15*sizeof(int16_t) ) == 0);
}


bool dumpActionFormatToFile(const std::string &fileName,
                            const size_t numChunks)
{
    byte *ptr = getActionBasePtr();

    //std::vector<byte> actionData(numChunks*30, 0);
    std::vector<byte> actionData;
    actionData.resize(numChunks*30);

    memcpy( actionData.data(), ptr, 30*numChunks );

    std::ofstream actionFile(fileName.c_str());

    if(!actionFile)
        return false;

    actionFile.write(reinterpret_cast<const char*>(actionData.data()),
                     int(actionData.size()));

    return true;
}
