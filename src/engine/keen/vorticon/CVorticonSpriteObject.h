/*
 * CVorticonSpriteObject.h
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#ifndef CVORTICONSPRITEOBJECT_H_
#define CVORTICONSPRITEOBJECT_H_

#include "engine/core/CSpriteObject.h"
#include <base/GsEvent.h>

class CVorticonSpriteObject : public CSpriteObject
{
public:
    CVorticonSpriteObject(CMap *pmap, const int x, const int y,
                          const object_t type, const int sprVar=0);

    void setupObjectType(const int Episode);
    bool checkforScenario();

        bool calcVisibility() override;

        virtual bool isNearby(CSpriteObject &) override { return true; }

        virtual void getTouchedBy(CSpriteObject &) override {}

        virtual void pumpEvent(const std::shared_ptr<CEvent> &evPtr) override
        { CSpriteObject::pumpEvent(evPtr); }

    object_t m_type;        		// yorp, vorticon, etc.

    unsigned char touchPlayer;      // 1=hit detection with player
    unsigned char touchedBy;        // which player was hit

    bool deadly;
};

#endif /* CVORTICONSPRITEOBJECT_H_ */
