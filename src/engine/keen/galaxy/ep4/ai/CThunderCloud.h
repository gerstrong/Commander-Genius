/*
 * CThunderCloud.h
 *
 *  Created on: 12.07.2012
 *      Author: gerstong
 */

#ifndef CTHUNDERCLOUD_H_
#define CTHUNDERCLOUD_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy
{


class CThunderBolt: public CGalaxySpriteObject
{
public:
    CThunderBolt(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y, const int sprVar);
	void getTouchedBy(CSpriteObject &theObject);

	void process();

};



class CThunderCloud: public CGalaxySpriteObject
{
public:
    CThunderCloud(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar);

	void process();

	void setActionForce(const size_t ActionNumber);
	bool isNearby(CSpriteObject &theObject);


private:

	void processAsleep();
	void processWaking();
	void processMoving();
	void processStriking();

	void (CThunderCloud::*mpProcessState)();
	std::map< size_t, void (CThunderCloud::*)() > mActionMap;
    int mTimer = 0;
    CThunderBolt *mpBolt = nullptr;
    bool mSecondTry = false;
};


} /* namespace galaxy */
#endif /* CTHUNDERCLOUD_H_ */
