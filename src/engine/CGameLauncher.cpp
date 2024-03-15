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
#include <base/PointDevice.h>
#include <base/InputEvents.h>
#include <base/GsApp.h>
#include <widgets/GsTextSelectionList.h>
#include <widgets/GsBanner.h>
#include <widgets/GsButton.h>
#include <widgets/GsText.h>

#include <graphics/GsGraphics.h>
#include <base/interface/FindFile.h>
#include <base/interface/StringUtils.h>
#include <base/audio/Audio.h>
#include <base/GsArguments.h>

#include <widgets/GsMenuController.h>

#include "core/VGamepads/vgamepadsimple.h"
#include "core/mode/CGameMode.h"
#include "core/menu/SettingsMenu.h"

#include "fileio/CConfiguration.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/KeenFiles.h"
#include "fileio/crc.h"

#include "../version.h"

#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <algorithm>

#include "keen/vorticon/VorticonEngine.h"
#include "keen/galaxy/GalaxyEngine.h"
#include "keen/dreams/dreamsengine.h"

#ifdef BUILD_COSMOS
#include "cosmos/CosmoEngine.h"
#endif // BUILD_COSMOS

#include <base/interface/property_tree/xml_parser.h>

/// Main Class implementation
GameLauncher::GameLauncher() :
mLauncherDialog(CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f),
                           CGUIDialog::FXKind::EXPAND)),
mGameScanner()
{}

GameLauncher::~GameLauncher()
{}

struct ExecutableListFiller
{
    std::set<std::string> list;

    bool operator() (const std::string& filename) {
        std::string ext = GetFileExtension(filename);
        if ( stringcaseequal(ext, "exe") ||
             stringcaseequal(ext, "lua") )
        {
            list.insert(stringtolower(filename));
        }

        return true;
    }
};


#include "gamelauncher.menu.h"

bool buildWidgets(CGUIDialog &dlg)
{
    // Create an empty property tree object
    using GsKit::ptree;
    ptree pt;

    try
    {
        // Load the XML file into the property tree. If reading fails
        // (cannot open file, parse error), an exception is thrown.
        read_xml(gamelauncher_menu, gamelauncher_menu_len, pt);

        gLogging << "Build Widgets." << CLogFile::endl;

        return buildDialogWidgetsFrom(dlg, pt.get_child("Dialog"));
    }
    catch(std::exception const& ex)
    {
        gLogging << "Exception while reading menu node: " << ex.what() << "\n";
        return false;
    }
    catch(...)
    {
        gLogging << "Unknown Exception while reading menu node\n.";
        return false;
    }
}

////
// Initialization Routine
////
bool GameLauncher::setupMenu()
{
    mMustquit      = false;
    mDonePatchSelection = false;
    mChosenGame    = -1;
    mLauncherDialog.initEmptyBackground();
    mSelection      = -1;


    // TODO: Put that scanning into a separate section so we can show a loading menu
    // Scan for games...
    mEntries.clear();

    gLogging.ftextOut("Game Autodetection Started<br>" );

    // Process any custom labels
    getLabels();

    bool gamesDetected = false;

    // Scan VFS DIR_ROOT for exe's
    gamesDetected |= scanExecutables(DIR_ROOT);
    mGameScanner.setPermilage(100);

    // Recursivly scan into DIR_GAMES subdir's for exe's
    gamesDetected |= scanSubDirectories(DIR_GAMES,
                                        DEPTH_MAX_GAMES,
                                        200, 900);

    // Save any custom labels
    putLabels();

    const auto openSettingsMenuEvent = [&]()
    {
        mLauncherDialog.enable(false);
        gEventManager.add( new OpenMenuEvent(
                               new SettingsMenu(Style::NONE, "Launcher") ) );
    };

    REGISTER_EV_FUNC(openSettingsMenuEvent);

    #ifdef DOWNLOADER
    const auto GMDownloadDlgOpen = [&]()
    {
        mDownloadGui.setupDownloadDialog();
    };

    REGISTER_EV_FUNC(GMDownloadDlgOpen);
    #endif // DOWNLOADER

    // Build widgets.
    try
    {
        if(!buildWidgets(mLauncherDialog))
            return false;
    }
    catch(...)
    {
        return false;
    }

    // Set the right references coming from the GsWidgets system
    mLauncherDialog.passTagToRef("currentBitmapBox", mCurrentBmp);
    mLauncherDialog.passTagToRef("GSSelList", mpGSSelList);
    mLauncherDialog.passTagToRef("plusMoreButton", mpPlusMorebutton);

#ifdef DOWNLOADER
    mDownloadGui.setPlusMoreButtonRef(mpPlusMorebutton);
#endif // DOWNLOADER

    mpPlusMorebutton->enable(false);

#ifndef DOWNLOADER
    mpPlusMorebutton->hide();
#endif // DOWNLOADER

    mpGSSelList->setBackgroundColor( GsColor(0xFF, 0xFF, 0xFF) );

    mPreviewBmpPtrVec.resize(mEntries.size());

    std::vector<GameEntry>::iterator it = mEntries.begin();
    unsigned int i=0;
    for( ; it != mEntries.end() ; it++	)
    {
        mpGSSelList->addText(it->name);

        // And try to add a preview bitmap
        std::string fullfilename = "preview.bmp";
        fullfilename = getResourceFilename(fullfilename, it->path, false);
        fullfilename = GetFullFileName(fullfilename);

        if(IsFileAvailable(fullfilename))
        {
            mPreviewBmpPtrVec[i].reset(new GsBitmap);
            mPreviewBmpPtrVec[i]->loadHQBitmap(fullfilename);
        }
        i++;
    }

    mpGSSelList->fitWithHeight(0.0f, 1.0f, 0.2f);

    mpGSSelList->setConfirmButtonEvent(new GMStart());
    mpGSSelList->setBackButtonEvent(new GMQuit());

    #ifdef DOWNLOADER

    const auto verifyGameStoreEvent = [&]()
    {
        mDownloadGui.verifyGameStore(mEntries);
    };

    REGISTER_EV_FUNC(verifyGameStoreEvent);

    verifyGameStoreEvent();
    mDownloadGui.tryDownloadCatalogueFile();
    #endif

    REGISTER_EV_FACTORY(GMStart);
    mLauncherDialog.passTagToRef("startButton", mpStartButton);

    mLauncherDialog.passTagToRef("EpisodeText", mpEpisodeText);
    mLauncherDialog.passTagToRef("VersionText", mpVersionText);
    mLauncherDialog.passTagToRef("DemoText", mpDemoText);


    // This way it goes right to the selection list.
    mLauncherDialog.setSelection(2);

    mGameScanner.setPermilage(1000);

    gLogging.ftextOut("Game Autodetection Finished<br>" );

    std::shared_ptr<CGUIBanner> guiBanner;
    mLauncherDialog.passTagToRef("GUIBannerText", guiBanner);

    guiBanner->setText("Commander Genius " CGVERSION "\n"
                       "by Gerstrong,\n"
                       "Zilem,\n"
                       "Tulip,\n"
                       "Albert Zeyer,\n"
                       "Pelya,\n"
                       "Elias Oenal,\n"
                       "Gagster,\n"
                       "and many CG Contributors\n");

    if(!gamesDetected)
    {
        std::stringstream ss;

        const auto searchPathGames =
                JoinPaths(GetFirstSearchPath(),"games");

        auto solvedPath = searchPathGames;
        ReplaceFileVariables(solvedPath);

        ss << "No games detected!\n";
        ss << "You can copy some into:\n";
        ss << "\"" << solvedPath << "\",\n";
        ss << "or download using \"+ More\" button\n";
        ss << "on the next page.\n";

        std::string msg(ss.str());

        showMessageBox(msg);

        return false;
    }

    // Set the first game selected and highlight the start button
    if(mpGSSelList) mpGSSelList->setSelection(0);

    mLauncherDialog.setSelection(3);

    std::string gameDir = gArgs.getValue("dir");
    gameDir.erase(std::remove(gameDir.begin(), gameDir.end(), '\"'),
                  gameDir.end());

    if(!gameDir.empty())
    {
        int chosenGame = 0;
        bool found = false;

        // Check if the given parameter makes one game start.
        for( GameEntry &entry : mEntries)
        {
            if(entry.path == gameDir)
            {
                // found!
                mChosenGame = chosenGame;
                gLogging.textOut("Launching game from directory: \"" + gameDir + "\"\n");
                gArgs.removeTag("dir");

                setupModsDialog();

                ExecutableListFiller exefileslist;
                const std::string dataDir = getDirectory( mChosenGame );
                FindFiles(exefileslist, dataDir, false, FM_REG);

                if( !exefileslist.list.empty() )
                {
                    mExecFilename = *(exefileslist.list.begin());
                }


                // Nothing else to do, break the loop
                found = true;
                break;
            }
            chosenGame++;
        }

        if(!found)
        {
            const std::string err = "The game from directory: \"" + gameDir + "\" cannot the launched." +
                    "Maybe it's missing or not compatible. Please check if you can run that through the game launcher.\n";

            gLogging.textOut(err);

            showMessageBox("Given path :\"" + gameDir + "\" unknown.\nPlease check the CGLog File!");
        }
    }

    return true;
}


bool GameLauncher::scanSubDirectories(const std::string& path,
                                       const size_t maxdepth,
                                       const size_t startPermil,
                                       const size_t endPermil)
{
    bool gamesDetected = false;

    std::set<std::string> dirs;
    FileListAdder fileListAdder;
    GetFileList(dirs, fileListAdder,
                path, false, FM_DIR);

    size_t interval = dirs.size();

    if(interval <= 0)
        interval = 1;

    size_t deltaPerMil = (endPermil-startPermil)/interval;
    size_t permil = startPermil;

    if(deltaPerMil < 1)
        deltaPerMil = 1;

    mGameScanner.setPermilage(int(startPermil));

    for( const auto &subdir : dirs )
    {
        std::string newpath = JoinPaths(path, subdir);

        gamesDetected |= scanExecutables(newpath);

        size_t lastPermil = permil + deltaPerMil;
        if(lastPermil>endPermil)
            lastPermil = endPermil;

        if(maxdepth > 1 && scanSubDirectories(newpath, maxdepth - 1, permil, lastPermil))
        {
            gamesDetected = true;
        }

        permil = lastPermil;
        mGameScanner.setPermilage(int(permil));
    }

    mGameScanner.setPermilage(int(endPermil));

    return gamesDetected;
}

std::string GameLauncher::filterGameName(const std::string &path)
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


enum class GAME_TYPE
{
    COMMANDER_KEEN = 0,
    COSMO,
    UNKNOWN,
    SIZE
};

bool detectCosmoGame(const std::string &curFname,
                     int &ep, bool &isLua)
{
    for(int i = 1; i <= 3; ++i)
    {
        std::string cosmoName = "cosmo" + itoa(i) + ".exe";
        if(curFname.find(cosmoName) != curFname.npos)
        {
            ep = i;
            return true;
        }
    }

    return false;
}


bool detectKeenGame(const std::string &curFname,
                    int &ep, bool &isLua)
{
    ep = -1;
    isLua = false;

    {
        // Episode 1-6 and 7 stands for Keen Dreams
        if(curFname.find("kdreams") != curFname.npos)
        {
            ep = 7;
        }

        for(int i = 1; i <= 6; ++i)
        {
            std::string keenName = "keen" + itoa(i) + "e.exe";
            if(curFname.find(keenName) != curFname.npos)
            {
                ep = i;
                break;
            }

            keenName = "keen" + itoa(i) + ".exe";
            if(curFname.find(keenName) != curFname.npos)
            {
                ep = i;
                break;
            }

            keenName = "keen" + itoa(i) + ".lua";
            if(curFname.find(keenName) != curFname.npos)
            {
                ep = i;
                isLua = true;
                break;
            }

            keenName = "k" + itoa(i) + "demo.exe";
            if(curFname.find(keenName) != curFname.npos)
            {
                ep = i;
                break;
            }
        }

        if(ep < 0)
            return false;
    }

    return true;
}


bool GameLauncher::scanExecutables(const std::string& path)
{
    bool result = false;

    gLogging.ftextOut("Search on vfs: %s<br>", path.c_str() );

    // First compare which matches are possible,
    // before you even try to open them in the loop.
    ExecutableListFiller execlist;
    FindFiles(execlist, path, false, FM_REG);


    for(const auto &curFname : execlist.list)
    {
        gLogging.ftextOut("Scanning %s \n<br>", curFname.c_str() );

        int ep = -1;
        bool isLua = false;


        CExeFile executable;

#ifdef BUILD_COSMOS
        if(detectCosmoGame(curFname, ep, isLua))
        {
            gLogging << "Cosmo's Cosmic Adventure game detected: " << curFname.c_str() << CLogFile::endl;

            if(!executable.readGenericExeData("cosmo", ep, path))
                continue;
        }
        else
#endif // BUILD_COSMOS
            if(detectKeenGame(curFname, ep, isLua))
        {
            gLogging.ftextOut("Commander Keen game detected: %s \n<br>", curFname.c_str() );

            // Load the exe into memory or a lua script
            if(isLua)
            {
                if(!executable.readMainLuaScript(static_cast<unsigned int>(ep),
                                                    path))
                    continue;
            }
            else
            {
                if(!executable.readKeenExeData(static_cast<unsigned int>(ep), path))
                    continue;
            }
        }
        else
        {
            continue;
        }

        // Process the exe for type
        GameEntry newentry;
        newentry.crcpass = executable.getEXECrc();
        newentry.version = static_cast<short>(executable.getEXEVersion());
        newentry.supported = executable.isSupported();
        newentry.episode = static_cast<Uint16>(ep);
        newentry.demo = executable.isDemo();
        newentry.path    = path;
        newentry.exefilename = executable.getFileName();
        // Check for an existing custom label for the menu
        newentry.name    = scanLabels(executable.getFileName());

        std::string verstr;
        std::string gamespecstring = "Detected game file: " + executable.getFileName();

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

        // Save the type information about the exe
        mEntries.push_back(newentry);

        gLogging.textOut(gamespecstring);

        result |= true;
    }

    return result;
}


bool GameLauncher::start()
{
    // CRC init when Launcher starts.
    crc32_init();

    SDL_ShowCursor(gVideoDriver.getVidConfig().mShowCursor ? SDL_ENABLE : SDL_DISABLE);

    // Make sure we use the right video settings for the launcher
    if(!setupNativeRes("Launcher", 2))
        return false;

    // In some cases especially when another game was running, the scene wasn't cleaned up.
    // We do this here
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    blit.fillRGB(0, 0, 0);

    // Load the graphics for menu and background.
    // Resources for the main menu
    // This is only for the menu. We only need one fontmap for the list of games and some buttons
    gGraphics.createEmptyFontmaps(1);
    auto &Font = gGraphics.getFontLegacy(0);

    const auto height = gVideoDriver.getHeight();

    // Depending on the height the font is loaded as scaled
    // so readability is better on higher resolutions.
    if(height >= 1920)
    {
        Font.loadinternalFont(8);
    }
    else if(height >= 960)
    {
        Font.loadinternalFont(4);
    }
    else if(height >= 480)
    {
        Font.loadinternalFont(2);
    }
    else
    {
        Font.loadinternalFont(1);
    }

    struct GamesScan : public Action
    {
        GameLauncher &mGameLauncher;

        GamesScan(GameLauncher &launcher) :
            mGameLauncher(launcher) {}

        int handle()
        {
            if(!mGameLauncher.setupMenu())
            {
                gLogging.textOut(FONTCOLORS::RED,"No game can be launched, because game data files are missing.<br>");
                return 0;
            }

            return 1;
        }
    };

    mGameScanner.setStyle(PROGRESS_STYLE_TEXT);
    mGameScanner.RunLoadActionBackground(new GamesScan(*this));
    mGameScanner.start();

    return true;
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





void GameLauncher::setupModsDialog()
{
    if(int(mEntries.size()) <= mChosenGame)
        return;

    const std::string dataDir = getDirectory( mChosenGame );

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

    mpPatchDialog.reset(new CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.85f),
                                       CGUIDialog::FXKind::EXPAND));
    mpPatchDialog->initEmptyBackground();

    if(!mPatchStrVec.empty())
        mPatchStrVec.clear();

    mpPatchSelList = new GsTextSelectionList(
                                GsRect<float>(0.01f, 0.07f, 0.49f, 0.87f));

    mpPatchSelList->setBackgroundColor( GsColor(0xFF, 0xFF, 0xFF) );

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

    mpPatchDialog->add(new GsText("Choose your patch:",
                                    GsRect<float>(0.0f, 0.0f, 1.0f, 0.05f)));
    mpPatchDialog->add(mpPatchSelList);


    mpPatchDialog->add(new GsButton( "Start >",
                                     GsRect<float>(0.65f, 0.865f, 0.3f, 0.07f),
                                     new GMPatchSelected() ));
}



bool GameLauncher::setChosenGame(const int chosengame)
{
    mChosenGame = chosengame;
    return waschosen();
}


void GameLauncher::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( std::dynamic_pointer_cast<const GMStart>(evPtr) )
    {
        const int chosengame = mpGSSelList->getSelection();
        setChosenGame(chosengame);

        // TODO: fetch the List of available patch files
        // Get the list of ".pat" files
        ExecutableListFiller exefileslist;
        const std::string dataDir = getDirectory( mChosenGame );
        FindFiles(exefileslist, dataDir, false, FM_REG);

        if( !exefileslist.list.empty() )
        {
            mExecFilename = *(exefileslist.list.begin());
        }


        if(mChosenGame >= 0)
        {
            setupModsDialog();
        }
    }
    else if( std::dynamic_pointer_cast<const GMPatchSelected>(evPtr) )
    {
        const auto sel = mpPatchSelList->getSelection();

        if(sel>0)
        {
            mPatchFilename = mPatchStrVec[sel];
            mDonePatchSelection = true;
        }
    }
/*
#ifdef USE_VIRTUALPAD
    else if( dynamic_cast<const OpenVGamePadSettingsEvent*>(evPtr) )
    {
        gInput.setInVPadConfigState(true);
    }
#endif
*/



    // Check Scroll events happening on this Launcher
    if( const auto mwe = std::dynamic_pointer_cast<const MouseWheelEvent>(evPtr) )
    {
        // Wrapper for the simple mouse scroll event
        if(mwe->amount.y > 0.0f)
        {
            const std::shared_ptr<CEvent> cmdEvent(new CommandEvent( IC_UP ));
            mLauncherDialog.sendEvent(cmdEvent);
        }
        else if(mwe->amount.y < 0.0f)
        {
            const std::shared_ptr<CEvent> cmdEvent(new CommandEvent( IC_DOWN ));
            mLauncherDialog.sendEvent(cmdEvent);
        }
        if(mwe->amount.x < 0.0f)
        {
            const std::shared_ptr<CEvent> cmdEvent(new CommandEvent( IC_RIGHT ));
            mLauncherDialog.sendEvent(cmdEvent);
        }
        else if(mwe->amount.x > 0.0f)
        {
            const std::shared_ptr<CEvent> cmdEvent(new CommandEvent( IC_LEFT ));
            mLauncherDialog.sendEvent(cmdEvent);
        }
    }
}



void GameLauncher::ponderGameSelDialog(const float deltaT)
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
        mMustquit = true;
        return;
    }

    // Command (Keyboard/Joystick) are handled here
    for( int cmd = IC_LEFT ; cmd < MAX_COMMANDS ; cmd++ )
    {
        if( gInput.getPressedCommand(cmd) )
        {
            const std::shared_ptr<CEvent> cmdEvent(
                        new CommandEvent( static_cast<InpCmd>(cmd) ));
            mLauncherDialog.sendEvent(cmdEvent);
            break;
        }
    }

    if(mpGSSelList)
    {
        // Check if the selection changed. Update the right data panel
        if(mSelection != mpGSSelList->getSelection() &&
           !mEntries.empty() )
        {
            mSelection = mpGSSelList->getSelection();

            if(mSelection != -1)
            {
                const auto sel = static_cast<unsigned int>(mSelection);

                // In case another list loaded, but we have an old selection
                if(sel >= mEntries.size())
                {
                    mSelection = 0;
                }
                else
                {
                    auto &entry = mEntries[sel];
                    const std::string nameText = "Episode " + itoa(entry.episode);
                    mpEpisodeText->setText(nameText);
                    float fVer = entry.version;
                    fVer /= 100.0f;
                    mpVersionText->setText("Version: " + ftoa(fVer));

                    if(mpDemoText)
                    {
                        mpDemoText->setText(entry.demo ? "Demo" : "");
                    }


                    // Now update the bitmap
                    if(mCurrentBmp)
                    {
                        mCurrentBmp->setBitmapPtr(mPreviewBmpPtrVec[sel]);
                    }
                }
            }
        }
    }

    mLauncherDialog.processLogic();
}


void GameLauncher::ponderPatchDialog()
{
    if(mpPatchDialog)
        mpPatchDialog->processLogic();

    auto &be = gBehaviorEngine;

    // Launch the code of the Startmenu here in case a game has been chosen
    if( mDonePatchSelection ) // Means a game has been selected
    {
        mpPatchDialog = nullptr;

        //// Game has been chosen. Launch it!
        // Get the path were to Launch the game
        const std::string DataDirectory = getDirectory( mChosenGame );

        // We have to check which Episode will be used
        const int episode = getEpisode( mChosenGame );

        be.mPatchFname = mPatchFilename;

        if( episode > 0 ) // The game has to have a valid episode!
        {
            // Get the EXE-Data of the game and load it into the memory.
            bool ok = false;
            auto gametype = GAME_TYPE::UNKNOWN;

            if(gKeenFiles.exeFile.readKeenExeData(episode, DataDirectory))
            {
               ok = true;
               gametype = GAME_TYPE::COMMANDER_KEEN;
            }
            else if(gKeenFiles.exeFile.readMainLuaScript(episode, DataDirectory))
            {
               ok = true;
               gametype = GAME_TYPE::COMMANDER_KEEN;
            }
            else if(gKeenFiles.exeFile.readGenericExeData("cosmo",
                                                          episode,
                                                          DataDirectory))
            {
               ok = true;
               gametype = GAME_TYPE::COSMO;
            }

            if(!ok)
            {
                letchooseagain();
            }
            else
            {
                gKeenFiles.gameDir = DataDirectory;

                if(gametype == GAME_TYPE::COMMANDER_KEEN)
                {
                    if(episode >= 1 && episode <= 7)
                    {
                        // Now let's decide which engine we have to start.
                        if(episode >= 1 && episode <= 3)
                        {
                            gEventManager.add( new StartVorticonEngine(false, episode, DataDirectory) );
                        }
                        else if(episode >= 4 && episode <= 6)
                        {
                            gEventManager.add( new StartGalaxyEngine(false, episode, DataDirectory) );
                        }
                        else if(episode == 7)
                        {
                            gEventManager.add( new StartDreamsEngine(false, DataDirectory) );
                        }
                    }
                    else // Everything else cannot be
                    {
                        gLogging.textOut("Something is wrong with your Episode configuration! Please check the game your chose");
                    }
                }
#ifdef BUILD_COSMOS
                else if(gametype == GAME_TYPE::COSMO)
                {
                    gEventManager.add( new StartCosmosEngine(false, episode, DataDirectory) );
                }
#endif // BUILD_COSMOS
                else
                {
                    gLogging.textOut("Unknown Game");
                    letchooseagain();
                }
            }
        }
        else
        {
            letchooseagain();
            gLogging.textOut(FONTCOLORS::RED,"No Suitable game was detected in this path! Please check its contents!\n");
        }
    }
}



////
// Process Routine
////
void GameLauncher::ponder(const float deltaT)
{
#ifdef DOWNLOADER
    mDownloadGui.ponder(deltaT);

    if(mDownloadGui.isOpen())
        return;
#endif // DOWNLOADER

    if(mGameScanner.isRunning())
    {
        mGameScanner.run(deltaT);
        return;
    }

    // In case after display/video setting changes, we need to reset the native resolution
    if(gVideoDriver.getRefreshSignal())
    {
        setupNativeRes("Launcher", 2);

        gVideoDriver.setRefreshSignal(false);
    }

    if(gMenuController.active())
    {
        mLauncherDialog.enable(false);
        return;
    }


    if(mpMsgDialog)
    {
        // Command (Keyboard/Joystick) events for the game center dialog
        for( int cmd = IC_LEFT ; cmd < MAX_COMMANDS ; cmd++ )
        {
            if( gInput.getPressedCommand(cmd) )
            {
                const std::shared_ptr<CEvent> cmdEvent(
                            new CommandEvent( static_cast<InpCmd>(cmd) ));
                mpMsgDialog->sendEvent(cmdEvent);
                break;
            }
        }

        mpMsgDialog->processLogic();
        return;
    }

    if(!mLauncherDialog.isEnabled())
    {
        mLauncherDialog.enable(true);
    }

    // Button should disabled unless a game was selected
    if(mpStartButton && mpGSSelList)
    {
        const auto selection = mpGSSelList->getSelection();

        if(selection >= 0 && selection < int(mEntries.size()))
        {
            mpStartButton->enable(true);
        }
        else
        {
            mpStartButton->enable(false);
        }
    }


    if(!mDonePatchSelection && mChosenGame < 0)
    {
        ponderGameSelDialog(deltaT);
    }
    else if(mChosenGame >= 0 && mChosenGame < int(mEntries.size()))
    {
        ponderPatchDialog();
    }
    else if(getQuit())
    {
        // User chose "exit", time to quit CG...
        gEventManager.add( new GMQuit() );
    }
}

void GameLauncher::renderMouseTouchState()
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    // Show mouse/touch cursor Pos and actions
    if(!mMouseTouchCurSfc)
    {
        const SDL_Rect sdlRect = gVideoDriver.getBlitSurface()->clip_rect;
        mMouseTouchCurSfc.create(0, sdlRect.w, 150, RES_BPP, 0, 0, 0, 0);
    }

    mMouseTouchCurSfc.fillRGB(0, 0, 0);

    auto &Font = gGraphics.getFontLegacy(0);

    const auto &pos = gPointDevice.mPointingState.mPos;
    const auto &act = gPointDevice.mPointingState.mActionButton;
    std::string tempbuf = "MTC: " + ftoa(pos.x) + "," +
                          ftoa(pos.y) + "->" + itoa(act);
    mMouseTouchCurSfc.fillRGB(0, 0, 0);
    Font.drawFont(mMouseTouchCurSfc.getSDLSurface(),
                  tempbuf,
                  0, 100, true);

    // In there is garbage of other drawn stuff clean it up.
    mMouseTouchCurSfc.blitTo(blit);
}

void GameLauncher::render()
{
#ifdef DOWNLOADER
    if(mDownloadGui.isOpen())
    {
        mDownloadGui.render();
        return;
    }
#endif // DOWNLOADER

    GsWeakSurface blit(gVideoDriver.getBlitSurface());

    if(mpMsgDialog)
    {
        mpMsgDialog->processRendering();
#ifdef DUMP_MTCS
        renderMouseTouchState();
#endif

        return;
    }

    if(!mDonePatchSelection && mChosenGame < 0)
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


    // Do the rendering of external dialogs
    if(mpPatchDialog)
        mpPatchDialog->processRendering();
}


void GameLauncher::getLabels()
{
    bool found;
    Uint16 i;
    std::string line, dir;
    std::ifstream gamescfg;

    mGameNames.clear();
    mGamePaths.clear();

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
                for ( i=0; i<mGamePaths.size(); i++ )
                {
                    if (strncmp(mGamePaths.at(i).c_str(),dir.c_str(),dir.length()) == 0)
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
                        mGamePaths.push_back( dir );

                        std::string gamePath = line.substr(strlen(GAMESCFG_NAME));

                        mGameNames.push_back( filterGameName(gamePath) );
                    }
                }
            }
        }
        gamescfg.close();
    }
}

std::string GameLauncher::scanLabels(const std::string& path)
{
    Uint16 i;

    for ( i=0; i<mGamePaths.size(); i++ )
    {
        if (strncmp(mGamePaths.at(i).c_str(),path.c_str(),path.length()) == 0)
        {
            return mGameNames.at(i);
        }
    }
    return "";
}

void GameLauncher::putLabels()
{
    Uint16 i;
    std::string line;
    std::ofstream gamescfg;

    OpenGameFileW(gamescfg, GAMESCFG);
    if (gamescfg.is_open())
    {
        for ( i=0; i<mEntries.size(); i++ )
        {
            line = GAMESCFG_DIR + mEntries.at(i).exefilename;
            gamescfg << line << std::endl;
            line = GAMESCFG_NAME + mEntries.at(i).name;
            gamescfg << line << std::endl << std::endl;
        }
        gamescfg.close();
    }
}
