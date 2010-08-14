/*
 * CProfilesMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 */

#ifndef CPROFILESMENU_H_
#define CPROFILESMENU_H_

#include "CBaseMenu.h"

//class CProfilesMenu : public CBaseMenu {
class CProfilesMenu : public CBaseMenu {
public:
	CProfilesMenu(Uint8 dlg_theme);
	void processCommon() {}
	void processSpecific();
	void postProcess() {}
	void setDefaultClassic();
	void setDefaultEnhanced();

	virtual ~CProfilesMenu();

private:
	CBaseMenu *mp_SubMenu;
};

#endif /* CPROFILESMENU_H_ */
