/*
 * CSaveMenu.h
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#ifndef CSAVEMENU_H_
#define CSAVEMENU_H_

//#include "CConfirmMenu.h"
#include "GameMenu.h"

class CSaveMenu : public GameMenu
{
public:
    CSaveMenu(const GsControl::Style &style);
    void refresh();
    void processSpecific(){}
    void ponder(const float deltaT);
	void sendEvent(std::shared_ptr<CEvent> &command);
    void release();


private:
    bool m_overwrite = false;
};

#endif /* CSAVEMENU_H_ */
