/*
 * CSettingsMenu.h
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#ifndef CSETTINGSMENU_H_
#define CSETTINGSMENU_H_

#include "CBaseMenu.h"
#include "../options.h"

class CSettingsMenu : public CBaseMenu
{
public:
	CSettingsMenu( Uint8 dlgtheme, stOption *p_option, bool &restartVideo );
	void processSpecific();
	virtual ~CSettingsMenu();

private:
	CBaseMenu *mp_SubMenu;
	stOption *mp_option;
	bool &m_restartVideo;
};

#endif /* CSETTINGSMENU_H_ */
