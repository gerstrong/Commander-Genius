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
    m_mustquit      = false;
    m_hasbeenchosen = false;
    m_chosenGame    = 0;
    mp_map          = NULL;
    mp_LaunchMenu   = NULL;
    m_ep1slot       = -1;

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

    // Initialize the menu
    mp_LaunchMenu = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 20);

    // Scan for games...
    m_SearchDepth = 0;
    gamedetected = false;
    m_DirList.clear();
    m_Entries.clear();

    g_pLogFile->ftextOut("Game Autodetection Started<br>" );

    // Process any custom labels
    getLabels();

    // Scan VFS root for exe's
    if (scanExecutables(m_Root))
        gamedetected = true;

    // Recursivly scan into VFS subdir's for exe's
    if (scanSubDirectories(m_Root))
        gamedetected = true;

    // No games detected then quit
    if(!gamedetected)
        return false;

    // Save any custom labels
    putLabels();

    mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,m_Entries.size()+1, "Quit");

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

    if(!MapLoader.load(1, 90, m_Entries.at(m_ep1slot).path)) return false;

    mp_map->gotoPos(1664,32);

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

Uint8 CGameLauncher::scanDirectories(const std::string& path, DirList& dir)
{
    std::set<std::string> dirs;
    FileListAdder fileListAdder;

    GetFileList(dirs, fileListAdder, path, false, FM_DIR);
    dir = DirList(dirs.begin(), dirs.end());

    return dir.size();
}

bool CGameLauncher::scanSubDirectories(const std::string& path)
{
    unsigned int i;
    bool gamedetected = false;
    std::string newpath;
    DirList dirlist;

    m_SearchDepth++;

    if (m_SearchDepth<=SUBDIR_MAX)
    {
        scanDirectories(path, dirlist);
        for ( i=0; i<dirlist.size(); i++ )
        {
            newpath = path + '/' + dirlist.at(i);
            if (scanExecutables(newpath))
                gamedetected = true;

            if (scanSubDirectories(newpath))
                gamedetected = true;
        }
    }
    return gamedetected;
}

bool CGameLauncher::scanExecutables(const std::string& path)
{
    bool result;
    unsigned int i=0;
    std::string file;
    GameEntry newentry;
    CExeFile* executable;

    g_pLogFile->ftextOut("Search: %s<br>", path.c_str() );

    result = false;
    for (i=0; i < m_ExeList.size(); i++)
    {
        file = path + '/' + m_ExeList.at(i);
        if (IsFileAvailable(file))
        {
            // Load the exe into memory
            executable = new CExeFile(i+1, path);
            executable->readData();
            // Process the exe for type
            newentry.crcpass = executable->getEXECrc();
            newentry.version = executable->getEXEVersion();
            newentry.episode = i+1;
            newentry.path    = path;
            // Check for an existing custom label for the menu
            newentry.name    = scanLabels(file);

            if( newentry.name.length() <= 0 )
            {
                newentry.name = "Episode: " + itoa(newentry.episode);
                newentry.name += " v" +itoa(newentry.version) + " " + newentry.path;
            }
            // Save the type information about the exe
            m_Entries.push_back(newentry);
            // Add a new menu item
            mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, m_Entries.size(), newentry.name);

            g_pLogFile->ftextOut("Detected game Name: %s Version: %d<br>", file.c_str()
                                                                        , newentry.version );
            // The original episode 1 exe is needed to load gfx's for game launcher menu
            if ( m_ep1slot <= -1 && newentry.crcpass == true )
            {
                m_ep1slot = m_Entries.size()-1;
                g_pLogFile->ftextOut("   Using for in-game menu resources<br>" );
            }

            // Cleanup
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
    if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
    {
        Uint8 selection = mp_LaunchMenu->getSelection();
        if( selection >= m_Entries.size() )
        {
            // outside the number of games, that exist. This means exit was triggered.
            m_mustquit = true;
        }
        else
        {
            m_hasbeenchosen = true;
            m_chosenGame = selection;
        }
    }

    // Did the user press (X)?
    if( g_pInput->getExitEvent() )
        m_mustquit = true;

    // Process Menu Input
    mp_LaunchMenu->processInput('u');

    // Animate the tiles of the map
    g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);

    // Blit the background
    g_pVideoDriver->blitScrollSurface(mp_map->m_scrollx_buf, mp_map->m_scrolly_buf);

    // Draw the Start-Menu
    mp_LaunchMenu->draw();
}

void CGameLauncher::getLabels()
{
    bool found;
    Uint16 i;
    std::string line, dir;
    std::ifstream gamescfg;

    m_Names.clear();
    m_Paths.clear();

    OpenGameFileR(gamescfg, GAMESCFG);
    if (gamescfg.is_open())
    {
        while ( !gamescfg.eof() )
        {
            getline(gamescfg,line);

            if (strncmp(line.c_str(),GAMESCFG_ROOT,strlen(GAMESCFG_ROOT)) == 0)
            {
                if (m_Root.length()<=0)
                {
                    m_Root = line.substr(strlen(GAMESCFG_ROOT));
                }
            }
            else if (strncmp(line.c_str(),GAMESCFG_DIR,strlen(GAMESCFG_DIR)) == 0)
            {
                dir = line.substr(strlen(GAMESCFG_DIR));

                // Check for duplicates
                found = false;
                for ( i=0; i<m_Paths.size(); i++ )
                {
                    if (strncmp(m_Paths.at(i).c_str(),dir.c_str(),dir.length()) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                // If not a duplicate get the custom name
                if (!found)
                {
                    getline(gamescfg,line);
                    if (strncmp(line.c_str(),GAMESCFG_NAME,strlen(GAMESCFG_NAME)) == 0)
                    {
                        m_Paths.push_back( dir );
                        m_Names.push_back( line.substr(strlen(GAMESCFG_NAME)) );
                    }
                }
            }
        }
        gamescfg.close();
    }

    // Set the default root for the search
    if (m_Root.length()<=0)
    {
        m_Root = DEFAULT_ROOT;
    }
}

std::string CGameLauncher::scanLabels(const std::string& path)
{
    Uint16 i;

    for ( i=0; i<m_Paths.size(); i++ )
    {
        if (strncmp(m_Paths.at(i).c_str(),path.c_str(),path.length()) == 0)
        {
            return m_Names.at(i);
        }
    }
    return "";
}

void CGameLauncher::putLabels()
{
    Uint16 i;
    std::string line;
    std::ofstream gamescfg;

    OpenGameFileW(gamescfg, GAMESCFG);
    if (gamescfg.is_open())
    {
        line = GAMESCFG_ROOT + m_Root;
        gamescfg << line << std::endl << std::endl;

        for ( i=0; i<m_Entries.size(); i++ )
        {
            line = GAMESCFG_DIR + m_Entries.at(i).path + '/' + m_ExeList.at(m_Entries.at(i).episode-1);
            gamescfg << line << std::endl;
            line = GAMESCFG_NAME + m_Entries.at(i).name;
            gamescfg << line << std::endl << std::endl;
        }
        gamescfg.close();
    }
}

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

