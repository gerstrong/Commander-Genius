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
#include "../../fileio/CExeFile.h"

class CSettingsMenu : public CBaseMenu
{
public:
	CSettingsMenu( Uint8 dlgtheme, CExeFile &ExeFile, stOption *p_option, bool &restartVideo );
	void processSpecific();

private:
	CExeFile &m_ExeFile;
	CBaseMenu *mp_SubMenu;
	stOption *mp_option;
	bool &m_restartVideo;
	Uint8 m_profileselection;
};

#endif /* CSETTINGSMENU_H_ */
