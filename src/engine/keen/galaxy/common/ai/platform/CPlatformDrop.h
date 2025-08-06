/*
 * CPlatformDrop.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 *
 *  This is the code of the dropping platforms.
 *  In Galaxy this platform falls but can return. If it falls too fast it won't return.
 */

#ifndef CPLATFORMDROP_H_
#define CPLATFORMDROP_H_

#include "CPlatform.h"

namespace galaxy
{

class CPlatformDrop : public CPlatform
{
public:
    CPlatformDrop(std::shared_ptr<CMap> pMap, const Uint16 foeID,
              const Uint32 x, const Uint32 y, const int actionOff, const int sprVar);

    // Most accurate platform drop movement, hopefully
    void procPlatdrop();

	/**
	 * \brief Processes the platforms AI. It's just simple moving around of course
	 */
    void process() override;

    void deserialize(GsKit::ptree &node) override;

    void serialize(GsKit::ptree &node) override;


private:
    int m_delay_for_drop = 0;
    int m_drop_speed = 0;

    // Times the cycle has to run through until Speed is raised
    int dropSpeedAcc;

    // Speed at what the Platform might return to the original position
    int hoverSpeed;

    GsVec2D<int> m_Origin;

    bool mAllowReturn = false;
};

}

#endif /* CPLATFORMDROP_H_ */
