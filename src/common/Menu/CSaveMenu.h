/*
 * CSaveMenu.h
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#ifndef CSAVEMENU_H_
#define CSAVEMENU_H_

#include "CConfirmMenu.h"

class CSaveMenu : public CBaseMenu
{
public:
	CSaveMenu();
	void processSpecific(){};
	void sendEvent(std::shared_ptr<CEvent> &command);
    
private:
	CConfirmMenu *mp_OverwriteMenu;
	bool m_overwrite;
};

#endif /* CSAVEMENU_H_ */
