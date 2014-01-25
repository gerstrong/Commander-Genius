/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "GalaxyMenu.h"
#include "widgets/Switch.h"
#include <list>

#include "common/CBehaviorEngine.h"

namespace galaxy
{

class COptions : public GalaxyMenu
{
public:
	COptions();

	void init();

    void ponder(const float deltaT);

	void release();

private:
    std::list<Switch*> mpOptionList;
	stOption *mpOption;
};

}

#endif /* COPTIONS_H_ */
