/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "CBaseMenu.h"

class CControlsettings : public CBaseMenu {
public:
	CControlsettings(const Uint8 m_DlgTheme);

	//void createControlItems();
	//void processSpecific();

private:

	//void processWaitInput();
	//void processSelection();

	Uint8 m_chosenPlayer;
	bool m_waiting_for_input;
	CBaseMenu *mp_SubMenu;

};

#endif /* CCONTROLSETTINGS_H_ */
