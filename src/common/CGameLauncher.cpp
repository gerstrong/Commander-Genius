/*
 * CGameLauncher.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameLauncher.h"
#include "../CLogFile.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"
#include "../common/CMapLoader.h"
#include "../StringUtils.h"
#include "../FindFile.h"
#include <dirent.h>
#include <iostream>
#include <fstream>

CGameLauncher::CGameLauncher() {
    m_mustquit = false;
    m_hasbeenchosen = false;
    m_numGames = 0;
    m_chosenGame = 0;
    mp_map = NULL;
    mp_LaunchMenu = NULL;
    m_ep1slot = -1;

    m_ExeList.clear();
    m_ExeList.push_back( KEENEXE1 );
    m_ExeList.push_back( KEENEXE2 );
    m_ExeList.push_back( KEENEXE3 );
}

CGameLauncher::~CGameLauncher() {
    // TODO Auto-generated destructor stub
}

////
// Initialization Routine
////
bool CGameLauncher::init()
{
    bool gamedetected = false;
    std::string root;

    // Initialize the menu
    mp_LaunchMenu = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 20);

    // Scan for games...
    gamedetected = false;
    m_DirList.clear();
    m_EpiList.clear();

    g_pLogFile->ftextOut("Game Autodetection Started<br>" );

    mp_LaunchMenu->addObject(DLG_OBJ_DISABLED,1,1, "Episode Mod Ver Folder");

    root = ".";
    if (scanExecutables(root))
        gamedetected = true;

    if (scanSubDirectories(root))
        gamedetected = true;

    if(!gamedetected)
        return false;

    m_numGames = m_DirList.size();
    mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,m_numGames+2, "Quit");

    g_pLogFile->ftextOut("Game Autodetection Finished<br>" );

    return true;
}

bool CGameLauncher::drawMenu()
{
    // Use the standard Menu-Frame used in the old DOS-Games
    mp_LaunchMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );

    // Load the map for the background
    mp_map = new CMap(g_pVideoDriver->getScrollSurface(), g_pGfxEngine->Tilemap);
    CMapLoader MapLoader(mp_map);

    if(!MapLoader.load(1, 90, m_DirList.at(m_ep1slot))) return false;

    mp_map->gotoPos(32,32);

    // Draw Background. This is only needed once, since Scrollsurface
    // won't be cleared every update screen
    mp_map->drawAll();

    return true;
}

struct FileListAdder {
    void operator()(std::set<std::string>& dirs, const std::string& path) {
        std::string basepath = GetBaseFilename(path);
        if(basepath != "" && basepath[0] != '.') {
            dirs.insert(basepath);
        }
    }
};

Uint8 CGameLauncher::scanDirectories(std::string& path, DirList& dir)
{
    std::set<std::string> dirs;
    FileListAdder fileListAdder;

    GetFileList(dirs, fileListAdder, path, false, FM_DIR);
    dir = DirList(dirs.begin(), dirs.end());

    return dir.size();
}

bool CGameLauncher::scanSubDirectories( std::string& root )
{
    unsigned int i;
    bool gamedetected = false;
    std::string path;
    DirList dirlist;

    scanDirectories(root, dirlist);
    for ( i=0; i<dirlist.size(); i++ )
    {
        path = root + '/' + dirlist.at(i);
        if (scanExecutables(path))
            gamedetected = true;

        if (scanSubDirectories(path))
            gamedetected = true;
    }
    return gamedetected;
}

bool CGameLauncher::scanExecutables(std::string& path)
{
    bool result, original;
    int version, episode;
    unsigned int i=0;
    std::string file, label;
    CExeFile* executable;

    g_pLogFile->ftextOut("Search: %s<br>", path.c_str() );

    result = false;
    for (i=0; i < m_ExeList.size(); i++)
    {
        file = path + '/' + m_ExeList.at(i);
        if (IsFileAvailable(file))
        {
            executable = new CExeFile(i+1, path);

            executable->readData();
            original = executable->GetEXEOriginal();
            version = executable->getEXEVersion();
            episode = i+1;
            m_DirList.push_back(path);
            m_EpiList.push_back(episode);

                                   //Episode
            label = itoa(episode) + "       ";
            if (original) {
                        //Mod
                label += "No  ";
            } else {
                label += "Yes ";
            }
                                    //Ver Folder
            label += itoa(version) + " " + path;
            mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,m_DirList.size()+1, label);

            g_pLogFile->ftextOut("Detected game Name: %s Version: %d Original: %d<br>", file.c_str()
                                                                        , version
                                                                        , original );
            // The original episode 1 exe is needed to load gfx's for game launcher menu
            if ( m_ep1slot <= -1 && original == true )
            {
                m_ep1slot = m_DirList.size()-1;
                g_pLogFile->ftextOut("   Using for in-game menu resources<br>" );
            }
            delete executable;

            result = true;
        }
    }

    return result;
}

////
// Process Routine
////
void CGameLauncher::process()
{
    // Gather input states
    if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedKey(KENTER) )
    {
        signed char selection = mp_LaunchMenu->getSelection()-1;
        if( selection >= m_numGames )
        {
            // outside the number of games, that exist. This means exit was triggered.
            m_mustquit = true;
        }
        else if( selection >= 0 )
        {
            m_hasbeenchosen = true;
            m_chosenGame = selection;
        }
    }

    // Did the user press (X)?
    if( g_pInput->getExitEvent() )
        m_mustquit = true;

    // Process Menu Input
    mp_LaunchMenu->processInput();

    // Animate the tiles of the map
    g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);

    // Blit the background
    g_pVideoDriver->blitScrollSurface(mp_map->m_scrollx_buf, mp_map->m_scrolly_buf);

    // Draw the Start-Menu
    mp_LaunchMenu->draw();
}

/*
// When the game is chosen, read the episode number by looking which exe file is present
Uint8 CGameLauncher::retrievetEpisode(short chosengame)
{
    if(IsFileAvailable("games/" + m_DirList.at(chosengame) + "/keen1.exe")) return 1;
    if(IsFileAvailable("games/" + m_DirList.at(chosengame) + "/keen2.exe")) return 2;
    if(IsFileAvailable("games/" + m_DirList.at(chosengame) + "/keen3.exe")) return 3;

    return 0;
}
*/

////
// Cleanup Routine
////
void CGameLauncher::cleanup()
{
    // destroy the map
    if (mp_map) delete mp_map, mp_map = NULL;

    // destroy the menu
    if (mp_LaunchMenu) delete mp_LaunchMenu, mp_LaunchMenu = NULL;
}

