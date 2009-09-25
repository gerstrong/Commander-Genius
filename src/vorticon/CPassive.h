/*
 * CPassive.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 *
 *  This Class controls the 3 screens that are shown
 *  when the game is started. THese are:
 *  Intro, Title and Demos.
 *  In that mode you can press any key to open the menu
 */

#ifndef CPASSIVE_H_
#define CPASSIVE_H_

#include "CIntro.h"
#include "../common/CMenu.h"
#include "../common/CMap.h"
#include <string>


class CPassive {
public:
	enum Modes{
		INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
	};

	CPassive(char Episode, std::string DataDirectory);
	virtual ~CPassive();

	bool init(char mode = INTRO);

	void process();
	bool getExitEvent() { return (m_mode==SHUTDOWN); }

	void cleanup();

private:
	CIntro *mp_IntroScreen;
	CMenu *mp_menu;
	CMap *mp_map;

	CTilemap *mp_Tilemap;
	SDL_Surface *mp_Scrollsurface;

	char m_Episode;
	std::string m_DataDirectory;

	char m_mode;
};
#endif /* CPASSIVE_H_ */
