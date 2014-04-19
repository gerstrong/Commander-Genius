
#include "ResourceMgmt.h"
#include <base/utils/StringUtils.h>
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>

/**
 * \brief Filters a given Filelist using a pattern give in the parameter namefilter
 */
void FilterFilelist(std::set<std::string>& filelist, const std::string& namefilter)
{
	std::set<std::string> oldfilelist = filelist;
	filelist.clear();

	std::set<std::string>::iterator it = oldfilelist.begin();
	for( ; it != oldfilelist.end() ; it++ )
	{
		if(strCaseStartsWith(*it,namefilter))
			filelist.insert(*it);
	}
}

const std::string GLOBAL_DIR = "global";

/**
 * \brief	This function will look for certain files used in the game. It looks on the local dir
 * 			and is it didn't find the file, it will look on the global director GLOBAL_DIR. In case
 * 			it finds something there, it return that alternate path.
 *  \param	filename		the filename for which we are looking
 *  \param	gamepath		if a game started, path to that one
 *  \param	essential		essential means whether the file is really needed or the game can live without it.
 * 	\param	can_be_global	This means, that the file can be read from the global directory. In some cases it must not.
 */
std::string getResourceFilename(const std::string& filename, const std::string& gamepath,
                                bool essential, bool can_be_global)
{
    std::string text;
    std::string vfs_path;

    if(gamepath != "")
    {
        vfs_path = JoinPaths(gamepath, filename);
        text = "Looking for " + vfs_path + " ... ";
        // try to look at the local path of the game.
        if(!IsFileAvailable(vfs_path))
            vfs_path = "";
    }

    if(vfs_path == "" && can_be_global)
    {
        // if it didn't find the file try looking at the global resources
        vfs_path = JoinPaths(GLOBAL_DIR, filename);
        text = "Looking for " + vfs_path + " ... ";

        if(!IsFileAvailable(vfs_path))
            vfs_path = "";
    }

    if( vfs_path == "" )
    {
        if(essential)
        {
            text += "missing!\n";
            text += "The file " + filename + " was not found. Please provide that file!\n";
        }
        else
            text.clear();
    }
    else
        text += "found!\n";

    if(!text.empty())
        gLogging.textOut(GREEN, text);

    return vfs_path;
}
