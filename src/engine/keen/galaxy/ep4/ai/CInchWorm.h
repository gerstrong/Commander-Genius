/*
 * CInchWorm.h
 *
 *  Created on: Sep 26, 2012
 *      Author: gerstrong
 *
 *  This class handles the inchworms found in the pyramid of moons
 */

#ifndef CINCHWORM_H_
#define CINCHWORM_H_

#include "../../common/CGalaxySpriteObject.h"

struct EventSpawnFoot : CEvent {

    const int x,y;
    const int foeID;

    EventSpawnFoot( const int lx, const int ly, const int lfoe ) :
                      x(lx), y(ly), foeID(lfoe) {}
};


namespace galaxy
{



class CInchWorm: public CGalaxySpriteObject
{
public:
	CInchWorm(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

private:
	int mInchWormContacts;
};

} /* namespace galaxy */

#endif /* CINCHWORM_H_ */
