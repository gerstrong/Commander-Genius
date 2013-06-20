/*
 * CPassiveVort.h
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#ifndef CPASSIVEVORT_H_
#define CPASSIVEVORT_H_

#include "engine/CPassive.h"
#include "common/CBehaviorEngine.h"
#include "engine/CIntro.h"
#include <memory>

namespace vorticon
{

class CPassiveVort : public CPassive
{
public:

	bool init(char mode = INTRO);

    void ponder();
    void render();

private:
	std::unique_ptr<CIntro> mpIntroScreen;
	std::unique_ptr<CTitle> mpTitleScreen;
	std::shared_ptr<CMap> mpMap;
};

}

#endif /* CPASSIVEVORT_H_ */
