/*
 * CVarPlatform.h
 *
 *  Created on: Sep 6, 2012
 *      Author: gerstrong
 */

#ifndef CVARPLATFORM_H_
#define CVARPLATFORM_H_

#include "../CMoveTarget.h"

#include "CPlatform.h"

#include <base/interface/property_tree/property_tree.h>

namespace galaxy
{

class CVarPlatform : public CPlatform, public CMoveTarget
{
public:
    CVarPlatform(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y,
                 const direction_t horidir,
                 const direction_t vertdir,
                 const int actionOffset, const int sprVar);

    void process();

    void draw();

    void deserialize(CSaveGameController &savedGame);

    void serialize(CSaveGameController &savedGame);

    void serialize(GsKit::ptree &node);

    void deserialize(GsKit::ptree &node);

private:
    bool drawGik = false;

    int m_GikSprite;
	
};

}

#endif /* CVARPLATFORM_H_ */
