/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "engine/core/menu/GameMenu.h"
#include "widgets/Switch.h"
#include <list>

#include "engine/core/CBehaviorEngine.h"


class COptions : public GameMenu
{
public:
    COptions(const GsControl::Style style);

    void refresh();

    void ponder(const float deltaT);

	void release();

private:
    std::list<Switch*> mpOptionList;
};

#endif /* COPTIONS_H_ */
