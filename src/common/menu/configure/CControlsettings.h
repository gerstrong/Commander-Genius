/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "../CBaseMenu.h"

class CControlsettings : public CBaseMenu {
public:
	CControlsettings(int chosenPlayerNumber);
	void drawInitialCommands();

	void processSpecific();
	virtual ~CControlsettings();

private:

	void processWaitInput();
	void processSelection();

	int m_chosenPlayer;
	bool m_waiting_for_input;
};

#endif /* CCONTROLSETTINGS_H_ */
