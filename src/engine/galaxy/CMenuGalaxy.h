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
#include "dialog/CDialog.h"

#include "common/CMenu.h"

namespace galaxy
{

class CMenuGalaxy : public CMenu
{
public:
	CMenuGalaxy( char menu_mode, CSavedGame &SavedGame,
			 bool &restartVideo );
	bool init( menutypes menu_type = MAIN );

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
