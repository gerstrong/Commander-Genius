/*
 * CMenu.h
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 */

#ifndef CMENU_GALAXY_H_
#define CMENU_GALAXY_H_

#include "graphics/CBitmap.h"
#include "graphics/CFont.h"
#include "fileio/CSaveGameController.h"

namespace galaxy
{

class CMenuGalaxy //: public CMenu
{
public:
	CMenuGalaxy( char menu_mode, CSaveGameController &SavedGame,
			 bool &restartVideo );

	void drawMenuBorder();

	void process();
	void processMainMenu();

	virtual ~CMenuGalaxy();

	void (CMenuGalaxy::*processPtr)();
	CBitmap m_TitleBmp;

	struct
	{
		int x;
		int y;
	} m_title_coord;
};

}

#endif /* CMENU_GALAXY_H_ */
