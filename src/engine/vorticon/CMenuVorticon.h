/*
 * CMenuVorticon.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong and pizza2004
 *
 *	This is the superclass which will be used for getting
 *	a menu work and is extended with specific engine instructions
 * */
#ifndef CMENUVORTICON_H_
#define CMENUVORTICON_H_

#include <SDL.h>
#include "../../dialog/CDialog.h"
#include "../../dialog/CTextViewer.h"
#include "../../common/CMap.h"

#include "../../engine/infoscenes/CInfoScene.h"

#include "../../common/CMenu.h"

#define TWIRL_SPEED_SLOW        100
#define TWIRL_SPEED_FAST        20

#define MENUS_Y                 32
#define MAINMENU_X              32

#define STORYBOARD_X			672
#define STORYBOARD_Y			32

#define FONT_TWIRL              9
#define TWIRL_SPEED             30
#define TWIRL_NUM_FRAMES        5

class CMenuVorticon : public CMenu {

public:

	CMenuVorticon( char menu_mode, CExeFile &ExeFile, CMap &Map, CSavedGame &SavedGame,
			 stOption *pOption, bool &restartVideo, bool &hideobjects );

	void processSpecific();
	void processF1Menu();
	void processMainMenu();

	void cleanup();

	virtual ~CMenuVorticon();

private:

	CMap &m_Map;
	CInfoScene *mp_InfoScene;
	bool &m_hideobjects;
};

#endif /* CMENUVORTICON_H_ */
