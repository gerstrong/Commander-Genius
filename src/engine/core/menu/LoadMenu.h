/*
 * CLoadMenu.h
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#ifndef CLOADMENU_H_
#define CLOADMENU_H_

#include "GameMenu.h"

class CLoadMenu : public GameMenu
{
public:
        CLoadMenu(const Style &style);

        void sendEvent(std::shared_ptr<CEvent> &command);

    void refresh();
};

#endif /* CLOADMENU_H_ */
