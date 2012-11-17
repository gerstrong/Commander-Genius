/*
 * CThunderCloud.h
 *
 *  Created on: 12.07.2012
 *      Author: gerstong
 */

#ifndef CTHUNDERCLOUD_H_
#define CTHUNDERCLOUD_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy
{


class CThunderBolt: public CGalaxySpriteObject
{
public:
	CThunderBolt(CMap *pmap, Uint32 x, Uint32 y);
	void getTouchedBy(CSpriteObject &theObject);

	void process();

};



class CThunderCloud: public CGalaxySpriteObject
{
public:
	CThunderCloud(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

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
	int mTimer;
	CThunderBolt *mpBolt;
};


} /* namespace galaxy */
#endif /* CTHUNDERCLOUD_H_ */
