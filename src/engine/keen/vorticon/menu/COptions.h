/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "VorticonMenu.h"
#include "widgets/Switch.h"
#include <list>

#include "engine/core/CBehaviorEngine.h"

namespace vorticon
{

class COptions : public VorticonMenu
{
public:
	COptions();

    void refresh();

    void ponder(const float deltaT);

	void release();

private:
    std::list<Switch*> mpOptionList;
};

}

#endif /* COPTIONS_H_ */
