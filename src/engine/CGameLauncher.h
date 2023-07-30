/*
 * CGameLauncher.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHER_H_
#define CGAMELAUNCHER_H_

#include "gameselector.h"

#include "core/CResourceLoader.h"
#include "gamedownloader.h"
#include "downloadgui.h"

#include <base/GsVirtualinput.h>
#include <base/interface/ThreadPool.h>

#include <engine/core/menu/SettingsMenu.h>

#include <widgets/GsDialog.h>
#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <widgets/GsBitmapBox.h>
#include <widgets/GsTextSelectionList.h>
#include <widgets/GsProgressbar.h>

#include <string>
#include <vector>
#include <ostream>
#include <memory>


// The directory/path to start the search at
#define DIR_ROOT        "."
#define DIR_GAMES       "games"
// The number of sub directories to search below the starting directory
#define DEPTH_MAX_ROOT  1
#define DEPTH_MAX_GAMES 2
// Config file identifers
#define GAMESCFG_DIR    "&Dir="
#define GAMESCFG_NAME   "/Name="
// Filenames
#define GAMESCFG        "games.cfg"



class GameLauncher : public GameSelector
{
public:
    GameLauncher();

    virtual ~GameLauncher() override;

    /**
     * @brief setupMenu This will setup the whole menu by scanning all the games in the path and building up the widgets for the selection screen
     * @return true, if everything went fine, otherwise false.
     */
    bool setupMenu();

    bool start() override;

    void setupModsDialog();

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override;
    void ponderGameSelDialog(const float deltaT);
    void ponderPatchDialog();

    void ponder(const float deltaT) override;

    void renderMouseTouchState();

    void render() override;

    int getChosengame()
    { return mChosenGame; }

    bool setChosenGame(const int chosengame);
    bool waschosen(){ return (mChosenGame>=0); }
    void letchooseagain()
    {
        mChosenGame=-1;
        mDonePatchSelection = false;
        mPatchFilename.clear();
        mExecFilename.clear();
    }

    bool getQuit() const { return mMustquit; }
    std::string getDirectory(Uint8 slot) const
    {   return mEntries.at(slot).path; }
    Uint8 getEpisode(Uint8 slot) const { return mEntries.at(slot).episode; }

    typedef std::vector<std::string> DirList;

private:

    std::string mExecFilename;
    std::string mPatchFilename;

    int mChosenGame = -1;

    Uint8 mEpisode;
    std::vector<GameEntry> mEntries;
    std::vector<std::string> mGamePaths;
    std::vector<std::string> mGameNames;
    CGUIDialog mLauncherDialog;

    // The Start-Button should change depending on the taken actions
    std::shared_ptr<GsButton> mpStartButton;

    std::unique_ptr<CGUIDialog> mpPatchDialog;

    GsSurface mMouseTouchCurSfc;

    std::shared_ptr<GsButton> mpPlusMorebutton;

    // This dialog is used for some messages serving as some sort of feedback
    CResourceLoaderBackground mGameScanner;

    std::shared_ptr<GsBitmapBox> mCurrentBmp;
    std::vector< std::shared_ptr<GsBitmap> > mPreviewBmpPtrVec;

    std::shared_ptr<GsText> mpEpisodeText;
    std::shared_ptr<GsText> mpDemoText;
    std::shared_ptr<GsText> mpVersionText;

    std::shared_ptr<GsTextSelectionList> mpGSSelList;

    GsTextSelectionList *mpPatchSelList;
    GsTextSelectionList *mpDosExecSelList;

    std::vector<std::string> mPatchStrVec;

    std::vector<std::string> mDosExecStrVec;

    int mSelection = -1;

    bool scanSubDirectories(const std::string& path,
                            const size_t maxdepth,
                            const size_t startPermil,
                            const size_t endPermil);

    std::string filterGameName(const std::string &path);

    bool scanExecutables(const std::string& path);

    void getLabels();
    std::string scanLabels(const std::string& path);
    void putLabels();


    bool mMustquit = false;

    bool mDonePatchSelection = false; // Tells if the Patch file has been selected if any
    bool mDoneExecSelection = false; // Tells if the Patch file has been selected if any

    DownloadGui mDownloadGui;
};


// Events
// This event switches to the GameLauncher
struct GMSwitchToGameLauncher : SwitchEngineEvent
{
    GMSwitchToGameLauncher() :
        SwitchEngineEvent( new GameLauncher() )
        { }
};

#endif /* CGAMELAUNCHER_H_ */
