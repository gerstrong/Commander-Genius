/*
 * CPassiveVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVEVORT_H_
#define CPASSIVEVORT_H_

#include "engine/core/CPassive.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/CIntro.h"
#include "CTitle.h"
#include <memory>

namespace vorticon
{

class CPassiveVort : public CPassive
{
public:

    bool init();

    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

private:
	std::unique_ptr<CIntro> mpIntroScreen;
    std::unique_ptr<Title> mpTitleScreen;
	std::shared_ptr<CMap> mpMap;
};

}

#endif /* CPASSIVEVORT_H_ */
