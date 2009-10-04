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
#include <string>
#include <vector>

class CGameLauncher {
public:
	CGameLauncher();
	virtual ~CGameLauncher();

	bool init();
	Uint8 scanDirectories();

	void process();
	Uint8 retrievetEpisode(short chosengame);

	void cleanup();

	unsigned char getChosengame(){ return m_chosenGame; }

	bool waschosen(){ return m_hasbeenchosen; }
	void letchooseagain() { m_hasbeenchosen=false; }

	bool getQuit(){ return m_mustquit; }
	std::string getDirectory(Uint8 slot) { return m_DirList.at(slot); }

private:
	bool m_mustquit;
	Uint8 m_numGames;
	short m_chosenGame;
	bool m_hasbeenchosen;
	Uint8 m_episode;
	std::vector<std::string> m_DirList;

	CDialog *mp_LaunchMenu;
	CMap *mp_map;
};

#endif /* CGAMELAUNCHER_H_ */
