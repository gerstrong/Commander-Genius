/*
 * CGameLauncher.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#ifndef CGAMELAUNCHER_H_
#define CGAMELAUNCHER_H_

#include "../dialog/CDialog.h"
#include "../common/CMap.h"
#include "../fileio/CExeFile.h"
#include <string>
#include <vector>

#define KEENEXE1 "keen1.exe"
#define KEENEXE2 "keen2.exe"
#define KEENEXE3 "keen3.exe"

class CGameLauncher {
public:
	CGameLauncher();
	virtual ~CGameLauncher();

	typedef std::vector<std::string> DirList;
	typedef std::vector<std::string> ExeList;
	typedef std::vector<Uint8> EpiList;

	bool init();
	Uint8 scanDirectories(std::string& path, DirList& dir);
	bool scanSubDirectories( std::string& root );
	bool scanExecutables(std::string& path);

    bool drawMenu();
	void process();
	Uint8 retrievetEpisode(short chosengame);

	void cleanup();

	unsigned char getChosengame(){ return m_chosenGame; }
	bool setChosenGame(unsigned char chosengame) { m_chosenGame = chosengame; return (m_hasbeenchosen=true); }

	bool waschosen(){ return m_hasbeenchosen; }
	void letchooseagain() { m_hasbeenchosen=false; }

	bool getQuit(){ return m_mustquit; }
	std::string getDirectory(Uint8 slot) { return m_DirList.at(slot); }
	Uint8 getEpisode(Uint8 slot) { return m_EpiList.at(slot); }
	std::string getEP1Directory() { return m_DirList.at(m_ep1slot); }

	Uint8 m_numGames;

private:
	bool m_mustquit;
	short m_chosenGame;
	bool m_hasbeenchosen;
	Uint8 m_episode;
	DirList m_DirList;
	ExeList m_ExeList;
	EpiList m_EpiList;
	Sint8 m_ep1slot;

	CDialog *mp_LaunchMenu;
	CMap *mp_map;
};

#endif /* CGAMELAUNCHER_H_ */
