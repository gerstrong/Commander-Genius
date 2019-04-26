/*
 * CGameLauncher.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHER_H_
#define CGAMELAUNCHER_H_

#include <base/GsEngine.h>
#include <base/GsVirtualinput.h>
#include <base/utils/ThreadPool.h>

#include <engine/core/menu/SettingsMenu.h>

#include <widgets/GsDialog.h>
#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <widgets/GsTextSelectionList.h>
#include <widgets/GsProgressbar.h>

#include <string>
#include <vector>
#include <ostream>
#include <memory>

#include "core/CResourceLoader.h"
#include "gamedownloader.h"

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


struct GameEntry
{
    std::string path;
    std::string exefilename;
    std::string name;
    short version = 0;
    bool supported = false;
    Uint16 episode = 0;
    bool demo = false;
    bool crcpass = false;
};



class CGameLauncher : public GsEngine
{
public:
    CGameLauncher();

    ~CGameLauncher() override;

	typedef std::vector<std::string> DirList;

    /**
     * @brief setupMenu This will setup the whole menu by scanning all the games in the path and building up the widgets for the selection screen
     * @return true, if everything went fine, otherwise false.
     */
    bool setupMenu();

    bool start() override;

    void showMessageBox(const std::string &text);

    void pullGame(const int selection);
    void setupDownloadDialog();

    void setupModsDialog();

    void pumpEvent(const CEvent *evPtr) override;
    void ponderGameSelDialog(const float deltaT);

    void verifyGameStore();
    void ponderDownloadDialog();

    void ponderPatchDialog();
    void ponder(const float deltaT) override;
    void render() override;

    int getChosengame()
    { return m_chosenGame; }

	bool setChosenGame(int chosengame) { m_chosenGame = chosengame; return waschosen();  }
	bool waschosen(){ return (m_chosenGame>=0); }
    void letchooseagain()
    {
        m_chosenGame=-1;
        mDonePatchSelection = false;
        mPatchFilename = "";
    }

	bool getQuit(){ return m_mustquit; }
	std::string getDirectory(Uint8 slot) { return m_Entries.at(slot).path; }
	Uint8 getEpisode(Uint8 slot) { return m_Entries.at(slot).episode; }
	std::string getEP1Directory() { return m_Entries.at(m_ep1slot).path; }

private:

    bool m_mustquit = false;
    int m_chosenGame = -1;

    bool mFinishedDownload = true;
    bool mDownloading = false;

    bool mDonePatchSelection; // Tells if the Patch file has been selected if any
    std::string mPatchFilename;

    bool mDoneExecSelection; // Tells if the Patch file has been selected if any
    std::string mExecFilename;

	Uint8 m_episode;
	DirList m_DirList;
	Sint8 m_ep1slot;
	std::vector<GameEntry> m_Entries;
	std::vector<std::string> m_Paths;
	std::vector<std::string> m_Names;
    CGUIDialog mLauncherDialog;

    // The Start-Button should change depending on the taken actions
    std::shared_ptr<GsButton> mpStartButton;

    std::unique_ptr<CGUIDialog> mpPatchDialog;

    //// Download Dialog Section. TODO: Make it external
    int mLastStoreSelection = -1;
    std::unique_ptr<CGUIDialog> mpGameStoreDialog;
    std::shared_ptr<CGUIText> mpDloadTitleText;
    std::shared_ptr<CGUIText> mpDDescriptionText;
    std::shared_ptr<GsButton> mpDloadBack;
    std::shared_ptr<GsButton> mpDloadCancel;
    std::shared_ptr<GsButton> mpDloadDownload;
    std::shared_ptr<GsProgressBar> mpDloadProgressCtrl;
    std::shared_ptr<CGUIBitmap> mpCurrentDownloadBmp;
    std::vector< std::shared_ptr<GsBitmap> > mpDownloadPrevievBmpVec;
    std::vector<GameCatalogueEntry> mGameCatalogue;


    // This dialog is used for some messages serving as some sort of feedback
    std::unique_ptr<CGUIDialog> mpMsgDialog;
    CResourceLoaderBackground mGameScanner;

    std::shared_ptr<CGUIBitmap> mCurrentBmp;
    std::vector< std::shared_ptr<GsBitmap> > mPreviewBmpPtrVec;

    std::shared_ptr<CGUIText> mpEpisodeText;
    std::shared_ptr<CGUIText> mpDemoText;
    std::shared_ptr<CGUIText> mpVersionText;

    std::shared_ptr<CGUITextSelectionList> mpGSSelList;

    CGUITextSelectionList *mpPatchSelList;
    CGUITextSelectionList *mpDosExecSelList;

    std::vector<std::string> mPatchStrVec;

    std::vector<std::string> mDosExecStrVec;

    int mSelection = -1;

    int mDownloadProgress = 0;
    int mDownloadErrorCode = 0;
    bool mCancelDownload = false;

    ThreadPoolItem* mpGameDownloadThread = nullptr;

    bool scanSubDirectories(const std::string& path,
                            const size_t maxdepth,
                            const size_t startPermil,
                            const size_t endPermil);

    std::string filterGameName(const std::string &path);

	bool scanExecutables(const std::string& path);

    void getLabels();
    std::string scanLabels(const std::string& path);
    void putLabels();

    int m_DownloadProgress = 0;
    int m_DownloadProgressError = 0;
    bool m_DownloadCancel = false;
};


// Events
// This event switches to the GameLauncher
struct GMSwitchToGameLauncher : SwitchEngineEvent
{
    GMSwitchToGameLauncher() :
        SwitchEngineEvent( new CGameLauncher() )
        { }
};

struct CloseBoxEvent : CEvent
{};

struct CancelDownloadEvent : CEvent
{};


#endif /* CGAMELAUNCHER_H_ */
