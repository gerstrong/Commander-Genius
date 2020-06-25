/*
 * CSaveMenu.h
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#ifndef CSAVEMENU_H_
#define CSAVEMENU_H_

#include "GameMenu.h"

class CSaveMenu : public GameMenu
{
public:
    CSaveMenu(const Style &style);
    void refresh() override;
    void processSpecific(){}
    void ponder(const float deltaT) override;
    void sendEvent(std::shared_ptr<CEvent> &command) override;
    void release() override;

private:
    bool mIsTyping = false;
};

#endif /* CSAVEMENU_H_ */
