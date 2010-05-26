/*
 * CGameLauncher.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHER_H_
#define CGAMELAUNCHER_H_

#include "../dialog/CDialog.h"
#include "../fileio/CExeFile.h"
#include <string>
#include <vector>
#include <ostream>

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
#define KEENEXE1        "keen1.exe"
#define KEENEXE2        "keen2.exe"
#define KEENEXE3        "keen3.exe"
#define KEENEXE4        "keen4.exe"
#define KEENEXE4E       "keen4e.exe"
#define KEENEXE5        "keen5.exe"
#define KEENEXE5E       "keen5e.exe"
#define KEENEXE6        "keen6.exe"
#define KEENEXE6E       "keen6e.exe"

struct GameEntry {
    std::string path;
    std::string name;
    short version;
    Uint16 episode;
    bool crcpass;
};

class CGameLauncher {
public:
	CGameLauncher();
	virtual ~CGameLauncher();

	typedef std::vector<std::string> DirList;
	typedef std::vector<std::string> ExeList;

	bool init();
    void cleanup();

	void process();
	Uint8 retrievetEpisode(short chosengame);

	unsigned char getChosengame(){ return m_chosenGame; }
	bool setChosenGame(unsigned char chosengame) { m_chosenGame = chosengame; return (m_hasbeenchosen=true); }
	bool waschosen(){ return m_hasbeenchosen; }
	void letchooseagain() { m_hasbeenchosen=false; }

	bool getQuit(){ return m_mustquit; }
	std::string getDirectory(Uint8 slot) { return m_Entries.at(slot).path; }
	Uint8 getEpisode(Uint8 slot) { return m_Entries.at(slot).episode; }
	std::string getEP1Directory() { return m_Entries.at(m_ep1slot).path; }

	Uint8 m_numGames;

private:

	bool m_mustquit;
	short m_chosenGame;
	bool m_hasbeenchosen;
	Uint8 m_episode;
	DirList m_DirList;
	ExeList m_ExeList;
	Sint8 m_ep1slot;
    std::vector<GameEntry> m_Entries;
	std::vector<std::string> m_Paths;
	std::vector<std::string> m_Names;
	CDialog *mp_LaunchMenu;

	bool scanSubDirectories(const std::string& root, size_t maxdepth = 0);
	bool scanExecutables(const std::string& path);

    void getLabels();
    std::string scanLabels(const std::string& path);
};

#endif /* CGAMELAUNCHER_H_ */
