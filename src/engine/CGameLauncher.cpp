/*
 * CGameLauncher.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameLauncher.h"
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <base/GsApp.h>
#include <widgets/GsTextSelectionList.h>
#include <widgets/GsBanner.h>
#include <widgets/GsButton.h>
#include <widgets/GsText.h>
#include <graphics/GsGraphics.h>
#include <base/utils/FindFile.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>
#include <base/GsArguments.h>

#include "core/mode/CGameMode.h"
#include "sdl/audio/Audio.h"
#include <fileio/ResourceMgmt.h>
#include "fileio/KeenFiles.h"

#include "../version.h"

#include <iostream>
#include <fstream>
#include <SDL_image.h>

#include "keen/vorticon/VorticonEngine.h"
#include "keen/galaxy/GalaxyEngine.h"
#include "dbfusion/dbFusionNgine.h"

bool disallowDBFusion = false;

CGameLauncher::CGameLauncher(const bool first_time,
                             const int start_game_no,
                             const int start_level) :
mDonePatchSelection(false),
mDoneExecSelection(false),
mLauncherDialog(CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND)),
mGameScanner(),
m_firsttime(first_time),
m_start_game_no(start_game_no),
m_start_level(start_level)
{	
    g_pSound->unloadSoundData();
    // The last menu has been removed. Restore back the game status
    g_pBehaviorEngine->setPause(false);

    gMenuController.clearMenuStack();
    letchooseagain();
}

////
// Initialization Routine
////
bool CGameLauncher::loadResources()
{
    //g_pBehaviorEngine->setEpisode(0);
    m_mustquit      = false;
    mDonePatchSelection = false;
    m_chosenGame    = -1;
    m_ep1slot       = -1;
    mLauncherDialog.initEmptyBackground();
    mSelection      = -1;

    bool gamedetected = false;

    // Scan for games...
    m_DirList.clear();
    m_Entries.clear();

    gLogging.ftextOut("Game Autodetection Started<br>" );

    // Process any custom labels
    getLabels();

    // Scan VFS DIR_ROOT for exe's
    if (scanExecutables(DIR_ROOT))
        gamedetected = true;
    mGameScanner.setPermilage(100);

    // Recursivly scan into DIR_GAMES subdir's for exe's
    if (scanSubDirectories(DIR_GAMES, DEPTH_MAX_GAMES, 200, 900))
        gamedetected = true;

    mpSelList = new CGUITextSelectionList();

    // Save any custom labels
    putLabels();

    // Create an empty Bitmap control
    mLauncherDialog.addControl( new CGUIBitmap(),
                                GsRect<float>(0.51f, 0.07f, 0.48f, 0.48f) );

    mCurrentBmp = std::dynamic_pointer_cast< CGUIBitmap >
                  ( mLauncherDialog.getControlList().back() );

    mpPrevievBmpVec.resize(m_Entries.size());

	std::vector<GameEntry>::iterator it = m_Entries.begin();
    unsigned int i=0;
    for( ; it != m_Entries.end() ; it++	)
    {
    	mpSelList->addText(it->name);

        // And try to add a preview bitmap
        std::string fullfilename = "preview.bmp";
        fullfilename = getResourceFilename(fullfilename, it->path, false);
        fullfilename = GetFullFileName(fullfilename);

        if(IsFileAvailable(fullfilename))
        {
            SDL_Surface *pPrimBmp = SDL_LoadBMP(GetFullFileName(fullfilename).c_str());
            std::shared_ptr<SDL_Surface> bmpSfcPtr( pPrimBmp );
            std::shared_ptr<GsBitmap> pBmp(new GsBitmap(bmpSfcPtr));
            mpPrevievBmpVec[i] = pBmp;
        }
        i++;
    }

    mpSelList->setConfirmButtonEvent(new GMStart());
    mpSelList->setBackButtonEvent(new GMQuit());

    mLauncherDialog.addControl(new CGUIText("Pick a Game"), GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f));
    mLauncherDialog.addControl(new GsButton( "x", new GMQuit() ), GsRect<float>(0.0f, 0.0f, 0.07f, 0.07f) );
    mLauncherDialog.addControl(mpSelList, GsRect<float>(0.01f, 0.07f, 0.49f, 0.79f));


    mLauncherDialog.addControl(new GsButton( "Start >", new GMStart() ), GsRect<float>(0.65f, 0.865f, 0.3f, 0.07f) );

#ifdef DBFUSION

    GsButton *fusionShellBtn = new GsButton( "DosFusion Shell >", new GMDBFusionStart() );
    GsButton *fusionBtn = new GsButton( "DosFusion! >", new GMDosGameFusionStart() );

    if(disallowDBFusion)
    {
        fusionShellBtn->enable(false);
        fusionBtn->enable(false);
    }

    mLauncherDialog.addControl( fusionShellBtn, GsRect<float>(0.01f, 0.865f, 0.3f, 0.07f) );
    mLauncherDialog.addControl( fusionBtn, GsRect<float>(0.35f, 0.865f, 0.3f, 0.07f) );
#endif

    mpEpisodeText = new CGUIText("Game");
    mpVersionText = new CGUIText("Version");
    mLauncherDialog.addControl(mpEpisodeText, GsRect<float>(0.5f, 0.75f, 0.5f, 0.05f));
    mLauncherDialog.addControl(mpVersionText, GsRect<float>(0.5f, 0.80f, 0.5f, 0.05f));

    // This way it goes right to the selection list.
    mLauncherDialog.setSelection(2);

    mGameScanner.setPermilage(1000);

    gLogging.ftextOut("Game Autodetection Finished<br>" );
    // Banner. TODO: Create a class for that...
    CGUIBanner *banner = new CGUIBanner("Commander Genius " CGVERSION "\n"
                    "By Gerstrong,\n"
                    "Hagel,\n"
                    "Tulip,\n"
                    "NY00123,\n"
                    "Pelya,\n"
					"and the CG Contributors\n");
    mLauncherDialog.addControl( banner, GsRect<float>(0.0f, 0.95f, 1.0f, 0.05f) );

    if(!gamedetected)
        return false;

    const std::string gameDir = gArgs.getValue("dir");
    if(!gameDir.empty())
    {
        int chosenGame = 0;
        bool found=false;

        // Check if the given parameter makes one game start.
        for( GameEntry &entry : m_Entries)
        {
            if(entry.path == gameDir)
            {
                // found!
                m_chosenGame = chosenGame;
                gLogging.textOut("Launching game from directory: \"" + gameDir + "\"\n");
                gArgs.removeTag("dir");

                setupModsDialog();
                // Nothing else to do, break the loop
                found = true;
                break;
            }
            chosenGame++;
        }

        if(!found)
        {
            const std::string err = "The game from directory: \"" + gameDir + "\" cannot the launched." +
                    "Maybe it's missing or not compatible. Please check if you can run that throught the game launcher.\n";

            gLogging.textOut(err);

            showMessageBox("Given path :\"" + gameDir + "\" unknown.\nPlease check the CGLog File!");
        }
    }

    return true;
}


bool CGameLauncher::scanSubDirectories(const std::string& path,
                                       const size_t maxdepth,
                                       const size_t startPermil,
                                       const size_t endPermil)
{
    bool gamedetected = false;

	std::set<std::string> dirs;
	FileListAdder fileListAdder;
	GetFileList(dirs, fileListAdder, path, false, FM_DIR);

    size_t interval = dirs.size();

    if(interval <= 0)
        interval = 1;

    size_t deltaPerMil = (endPermil-startPermil)/interval;
    size_t permil = startPermil;

    if(deltaPerMil < 1)
        deltaPerMil = 1;

    mGameScanner.setPermilage(startPermil);

	for(std::set<std::string>::iterator i = dirs.begin(); i != dirs.end(); ++i)
	{
		std::string newpath = path + '/' +  *i;

		if(scanExecutables(newpath))
			gamedetected = true;

        size_t lastPermil = permil + deltaPerMil;
        if(lastPermil>endPermil)
            lastPermil = endPermil;

        if(maxdepth > 1 && scanSubDirectories(newpath, maxdepth - 1, permil, lastPermil))
			gamedetected = true;

        permil = lastPermil;
        mGameScanner.setPermilage(permil);
	}

    mGameScanner.setPermilage(endPermil);

    return gamedetected;
}

std::string CGameLauncher::filterGameName(const std::string &path)
{
    size_t pos = 0;

    std::string text = path;

    while(1)
    {
        if( (pos = text.find("\\")) == text.npos)
        {
            if( (pos = text.find("/")) == text.npos)
            {
                break;
            }
        }

        text = text.substr(pos+1);
    }

    return text;
}

bool CGameLauncher::scanExecutables(const std::string& path)
{
    bool result = false;

    gLogging.ftextOut("Search: %s<br>", path.c_str() );

    for(int i = 1; i <= 6; ++i)
    {
		CExeFile executable;
		// Load the exe into memory
		if(!executable.readData(i, path))
			continue;

		// Process the exe for type
		GameEntry newentry;
		newentry.crcpass = executable.getEXECrc();
		newentry.version = executable.getEXEVersion();
		newentry.supported = executable.Supported();
		newentry.episode = i;
		newentry.path    = path;
		newentry.exefilename = executable.getFileName();
		// Check for an existing custom label for the menu
		newentry.name    = scanLabels(executable.getFileName());

		std::string verstr;
		std::string gamespecstring = "Detected game Name: " + executable.getFileName();
		if( newentry.version<=0 ) // Version couldn't be read!
		{
			verstr = "unknown";
			gamespecstring += " (Unknown Version)<br>";
		}
		else
		{
			verstr = "v" + itoa(newentry.version/100) + "." + itoa(newentry.version%100);
			gamespecstring += " Version: ";
			gamespecstring += verstr;
			gamespecstring += "<br>";
		}



		if( newentry.name.length() <= 0 )
		{
            newentry.name = filterGameName(newentry.path);
		}

		newentry.name += " ";

		// Save the type information about the exe
		m_Entries.push_back(newentry);

        gLogging.textOut(gamespecstring);

		// The original episode 1 exe is needed to load gfx's for game launcher menu
		if ( m_ep1slot <= -1 && newentry.crcpass == true )
		{
			m_ep1slot = m_Entries.size()-1;
            gLogging.ftextOut("   Using for in-game menu resources<br>" );
		}
		result = true;
	}

    return result;
}


void CGameLauncher::start()
{
    // Here it always makes sense to have the mouse cursor active
    SDL_ShowCursor(SDL_ENABLE);

    // Set the native resolution
    gVideoDriver.setNativeResolution(gVideoDriver.getVidConfig().m_DisplayRect);

    // In some cases especially when another game was running, the scene wasn't cleaned up.
    // We do this here
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    blit.fillRGB(0, 0, 0);

    // Load the graphics for menu and background.
    // Resources for the main menu
    // This is only for the menu. We only need one fontmap for the list of games and some buttons
    gGraphics.createEmptyFontmaps(1);
    GsFont &Font = gGraphics.getFont(0);

    Font.loadinternalFont();

    struct GamesScan : public Action
    {
        CGameLauncher &mGameLauncher;

        GamesScan(CGameLauncher &launcher) :
            mGameLauncher(launcher) {}

        int handle()
        {
            if(!mGameLauncher.loadResources())
            {
                gLogging.textOut(RED,"No game can be launched, because game data files are missing.<br>");
                return 0;
            }

            return 1;
        }
    };


    mGameScanner.setStyle(PROGRESS_STYLE_TEXT);
    mGameScanner.RunLoadActionBackground(new GamesScan(*this));
    mGameScanner.start();
}


struct PatchListFiller
{
    std::set<std::string> list;

    bool operator() (const std::string& filename) {
        std::string ext = GetFileExtension(filename);
        if (stringcaseequal(ext, "pat"))
        {
            list.insert(filename);
        }

        return true;
    }
};


struct CloseBoxEvent : CEvent
{};



void CGameLauncher::showMessageBox(const std::string &text)
{
    std::vector<std::string> strVec = explode(text, "\n");

    mpMsgDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND));

    mpMsgDialog->initEmptyBackground();

    float yStart = 0.1f;
    for( auto &txtItem : strVec)
    {
        mpMsgDialog->addControl(new CGUIText(txtItem), GsRect<float>(0.1f, yStart, 0.8f, 0.05f));
        yStart += 0.05f;
    }



    mpMsgDialog->addControl(new GsButton("Ok", new CloseBoxEvent()), GsRect<float>(0.4f, 0.85f, 0.2f, 0.05f) );

}

void CGameLauncher::setupModsDialog()
{
    const std::string dataDir = getDirectory( m_chosenGame );

    // TODO: fetch the List of available patch files
    // Get the list of ".pat" files
    PatchListFiller patchlist;
    FindFiles(patchlist, dataDir, false, FM_REG);

    if( patchlist.list.empty() )
    {
        mPatchFilename = "";
        mDonePatchSelection=true;
        return;
    }

    // If the there are not at least 2 mods to select, do not create the patch selection dialog
    if( patchlist.list.size() == 1 )
    {
        mPatchFilename = *(patchlist.list.begin());
        mDonePatchSelection=true;
        return;
    }

    mpPatchDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND));
    mpPatchDialog->initEmptyBackground();

    if(!mPatchStrVec.empty())
        mPatchStrVec.clear();

    mpPatchSelList = new CGUITextSelectionList();


    for( auto &elem : patchlist.list )
    {
        const std::string dirname = GetDirName(elem);
        std::string name = elem.substr(dirname.size()+1);
        name = name.substr(0, name.size()-4);
        mPatchStrVec.push_back(elem);
        mpPatchSelList->addText(name);
    }

    mpPatchSelList->setConfirmButtonEvent(new GMPatchSelected());
    mpPatchSelList->setBackButtonEvent(new GMQuit());

    mpPatchDialog->addControl(new CGUIText("Choose your patch:"), GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f));
    mpPatchDialog->addControl(mpPatchSelList, GsRect<float>(0.01f, 0.07f, 0.49f, 0.87f));


    mpPatchDialog->addControl(new GsButton( "Start >", new GMPatchSelected() ), GsRect<float>(0.65f, 0.865f, 0.3f, 0.07f) );
}



struct DosExecListFiller
{
    std::set<std::string> list;

    bool operator() (const std::string& filename) {
        std::string ext = GetFileExtension(filename);
        if (stringcaseequal(ext, "exe"))
        {
            list.insert(filename);
        }
        if (stringcaseequal(ext, "bat"))
        {
            list.insert(filename);
        }

        return true;
    }
};


void CGameLauncher::setupDosExecDialog()
{
    const std::string dataDir = getDirectory( m_chosenGame );

    // TODO: fetch the List of available patch files
    // Get the list of ".pat" files
    DosExecListFiller dosExecList;
    FindFiles(dosExecList, dataDir, false, FM_REG);

    if( dosExecList.list.empty() )
    {
        mExecFilename = "";
        mDoneExecSelection=true;
        return;
    }

    // If the there are not at least 2 mods to select, do not create the patch selection dialog
    if( dosExecList.list.size() == 1 )
    {
        mExecFilename = *(dosExecList.list.begin());
        mDoneExecSelection=true;
        return;
    }

    mpDosExecDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f), CGUIDialog::EXPAND)),
    mpDosExecDialog->initEmptyBackground();


    if(!mDosExecStrVec.empty())
        mDosExecStrVec.clear();

    mpDosExecSelList = new CGUITextSelectionList();


    for( auto &elem : dosExecList.list )
    {
        const std::string dirname = GetDirName(elem);
        std::string name = elem.substr(dirname.size()+1);
        mDosExecStrVec.push_back(elem);
        mpDosExecSelList->addText(name);
    }

    mpDosExecSelList->setConfirmButtonEvent(new GMDosExecSelected());
    mpDosExecSelList->setBackButtonEvent(new GMQuit());

    mpDosExecDialog->addControl(new CGUIText("Choose your executable:"), GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f));
    mpDosExecDialog->addControl(mpDosExecSelList, GsRect<float>(0.01f, 0.07f, 0.49f, 0.87f));


    mpDosExecDialog->addControl(new GsButton( "Start >", new GMDosExecSelected() ), GsRect<float>(0.65f, 0.865f, 0.3f, 0.07f) );
}


void CGameLauncher::pumpEvent(const CEvent *evPtr)
{

#ifdef DBFUSION
    if( dynamic_cast<const GMDBFusionStart*>(evPtr) )
    {
        gEventManager.add( new StartDBFusionEngine() );
        disallowDBFusion = true;
    }
    else if( dynamic_cast<const GMDosGameFusionStart*>(evPtr) )
    {
        setChosenGame(mpSelList->getSelection());

        if(m_chosenGame >= 0)
        {
            setupDosExecDialog();
        }
    }
    else if( dynamic_cast<const GMDosExecSelected*>(evPtr) )
    {
        mExecFilename = mDosExecStrVec[mpDosExecSelList->getSelection()];
        mDoneExecSelection = true;
    }
    else
#endif
    if( dynamic_cast<const GMStart*>(evPtr) )
    {
        setChosenGame(mpSelList->getSelection());

        if(m_chosenGame >= 0)
        {
            setupModsDialog();
        }
    }
    else if( dynamic_cast<const GMPatchSelected*>(evPtr) )
    {
        mPatchFilename = mPatchStrVec[mpPatchSelList->getSelection()];
        mDonePatchSelection = true;
    }
    else if( dynamic_cast<const CloseBoxEvent*>(evPtr) )
    {
        mpMsgDialog = nullptr;
    }


    // Check Scroll events happening on this Launcher
    if( const MouseWheelEvent *mwe = dynamic_cast<const MouseWheelEvent*>(evPtr) )
    {
        // Wrapper for the simple mouse scroll event
        if(mwe->amount.y < 0.0)
        {
            mLauncherDialog.sendEvent(new CommandEvent( IC_UP ));
        }
        else if(mwe->amount.y > 0.0)
        {
            mLauncherDialog.sendEvent(new CommandEvent( IC_DOWN ));
        }
    }
}



void CGameLauncher::ponderGameSelDialog(const float deltaT)
{
    // If GameScanner is running, don't do anything else
    if(mGameScanner.isRunning())
    {
        mGameScanner.run(deltaT);
        return;
    }

    // Did the user press (X)?
    if( gInput.getExitEvent() )
    {
        m_mustquit = true;
        return;
    }

    // Command (Keyboard/Joystick) are handled here
    for( int cmd = IC_LEFT ; cmd < MAX_COMMANDS ; cmd++ )
    {
        if( gInput.getPressedCommand(cmd) )
        {
            mLauncherDialog.sendEvent(new CommandEvent( static_cast<InputCommands>(cmd) ));
            break;
        }
    }

    // Check if the selection changed. Update the right data panel
    if(mSelection != mpSelList->getSelection())
    {
        mSelection = mpSelList->getSelection();
        const std::string nameText = "Episode " + itoa(m_Entries[mSelection].episode);
        mpEpisodeText->setText(nameText);
        float fVer = m_Entries[mSelection].version;
        fVer /= 100.0f;
        mpVersionText->setText("Version: " + ftoa(fVer));

        // Now update the bitmap
        mCurrentBmp->setBitmapPtr(mpPrevievBmpVec[mSelection]);
    }

    mLauncherDialog.processLogic();
}


void CGameLauncher::ponderPatchDialog()
{
    if(mpPatchDialog)
        mpPatchDialog->processLogic();

    if(mpDosExecDialog)
        mpDosExecDialog->processLogic();

    // Launch the code of the Startmenu here in case a game has been chosen
    if( mDonePatchSelection ) // Means a game has been selected
    {
        mpPatchDialog = nullptr;

        //// Game has been chosen. Launch it!
        // Get the path were to Launch the game
        const std::string DataDirectory = getDirectory( m_chosenGame );

        // We have to check which Episode will be used
        const int episode = getEpisode( m_chosenGame );

        g_pBehaviorEngine->mPatchFname = mPatchFilename;

        if( episode > 0 ) // The game has to have a valid episode!
        {
            // Get the EXE-Data of the game and load it into the memory.
            if(!gKeenFiles.exeFile.readData(episode, DataDirectory))
            {
                letchooseagain();
            }
            else
            {
                if(episode >= 1 && episode <= 7)
                {
                    // Now let's decide which engine we have to start.
                    if(episode >= 1 && episode <= 3)
                    {
                        gEventManager.add( new StartVorticonEngine(false, episode, DataDirectory) );
                    }
                    else if(episode >= 4 && episode <= 7)
                    {
                        gEventManager.add( new StartGalaxyEngine(false, episode, DataDirectory) );
                    }

                }

                else // Everything cannot be
                {
                    gLogging.textOut("Something is wrong with your Episode configuration! Please check the game your chose");
                }
            }
        }
        else
        {
            letchooseagain();
            gLogging.textOut(RED,"No Suitable game was detected in this path! Please check its contents!\n");
        }
    }


#ifdef DBFUSION
    if(mDoneExecSelection)
    {
        mpDosExecDialog = nullptr;
        gEventManager.add( new StartDBFusionEngine(mExecFilename) );
        disallowDBFusion = true;
    }
#endif

}


////
// Process Routine
////
void CGameLauncher::ponder(const float deltaT)
{
    if(mpMsgDialog)
    {
        mpMsgDialog->processLogic();
        return;
    }


    if(!mDonePatchSelection && m_chosenGame < 0)
    {
        ponderGameSelDialog(deltaT);
    }
    else if(m_chosenGame >= 0)
    {
        ponderPatchDialog();
    }
    else if(getQuit())
    {
        // User chose "exit". So make CG quit...
        gEventManager.add( new GMQuit() );
    }
}


void CGameLauncher::render()
{
    if(mpMsgDialog)
    {
        mpMsgDialog->processRendering();
        return;
    }

    if(!mDonePatchSelection && m_chosenGame < 0)
    {
        // If GameScanner is running, don't do anything else
        if(mGameScanner.isRunning())
        {
            mGameScanner.render();
            return;
        }

        // Do the rendering of the dialog
        mLauncherDialog.processRendering();
    }

    // Do the rendering of the dialog
    if(mpPatchDialog)
        mpPatchDialog->processRendering();

    if(mpDosExecDialog)
        mpDosExecDialog->processRendering();

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

            if (strncmp(line.c_str(),GAMESCFG_DIR,strlen(GAMESCFG_DIR)) == 0)
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

                        std::string gamePath = line.substr(strlen(GAMESCFG_NAME));

                        m_Names.push_back( filterGameName(gamePath) );
                    }
                }
            }
        }
        gamescfg.close();
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
        for ( i=0; i<m_Entries.size(); i++ )
        {
            line = GAMESCFG_DIR + m_Entries.at(i).exefilename;
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
    // destroy the menu

}

