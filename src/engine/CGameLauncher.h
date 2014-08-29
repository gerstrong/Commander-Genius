/*
 * CGameLauncher.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHER_H_
#define CGAMELAUNCHER_H_

#include <base/GsEngine.h>

#include <widgets/GsDialog.h>
#include <widgets/GsText.h>
#include <widgets/GsTextSelectionList.h>

#include <string>
#include <vector>
#include <ostream>

#include "core/CResourceLoader.h"


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
    short version;
    bool supported;
    Uint16 episode;
    bool crcpass;

    GameEntry() :  version(0),
    			   supported(false),
    			   episode(0),
    			   crcpass(false) {}

};

class CGameLauncher : public GsEngine
{
public:
    CGameLauncher(const bool first_time,
                  const int start_game_no = -1,
                  const int start_level = -1);

	typedef std::vector<std::string> DirList;

    bool loadResources();
    void cleanup();

    void start();


    void showMessageBox(const std::string &text);
    void setupModsDialog();

    void setupDosExecDialog();

    void pumpEvent(const CEvent *evPtr);
    void ponderGameSelDialog(const float deltaT);
    void ponderPatchDialog();
    void ponder(const float deltaT);
    void render();

	int getChosengame(){ return m_chosenGame; }
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

	bool m_mustquit;
	int m_chosenGame;

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

    std::unique_ptr<CGUIDialog> mpPatchDialog;
    std::unique_ptr<CGUIDialog> mpDosExecDialog;


    // This dialog is used for some messages serving as some sort of feedback
    std::unique_ptr<CGUIDialog> mpMsgDialog;
    CResourceLoaderBackground mGameScanner;

    std::shared_ptr<CGUIBitmap> mCurrentBmp;
    std::vector< std::shared_ptr<GsBitmap> > mpPrevievBmpVec;
	CGUIText *mpEpisodeText;
	CGUIText *mpVersionText;

    CGUITextSelectionList *mpSelList;
    CGUITextSelectionList *mpPatchSelList;
    CGUITextSelectionList *mpDosExecSelList;

    std::vector<std::string> mPatchStrVec;

    std::vector<std::string> mDosExecStrVec;

    int mSelection;

    bool m_firsttime;
    int m_start_game_no;
    int m_start_level;

    bool scanSubDirectories(const std::string& path,
                            const size_t maxdepth,
                            const size_t startPermil,
                            const size_t endPermil);

    std::string filterGameName(const std::string &path);

	bool scanExecutables(const std::string& path);

    void getLabels();
    std::string scanLabels(const std::string& path);
    void putLabels();
};


// Events
// This event switches to the GameLauncher
struct GMSwitchToGameLauncher : SwitchEngineEvent
{
    GMSwitchToGameLauncher(	const int ChosenGame=-1, const int StartLevel=-1 ) :
        SwitchEngineEvent( new CGameLauncher(false, ChosenGame, ChosenGame) )
        { }
};

#endif /* CGAMELAUNCHER_H_ */
