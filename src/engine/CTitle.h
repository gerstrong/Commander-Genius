/*
 * CTitle.h
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 * This class manages the titlescreen of the game which
 * is very simple and maybe the class is a bit
 * overloaded for such task. Nevertheless we do it
 * that way because if more games are added,
 * it may get more complicated as the title may
 * differ from other passive modes.
 */


#ifndef CTITLE_H_
#define CTITLE_H_

#include <SDL.h>
#include <vector>
#include "graphics/CGfxEngine.h"
#include "common/CObject.h"
#include "common/CMap.h"
#include "SmartPointer.h"

class CTitle
{
public:

	CTitle(CMap &map);

	bool init(int Episode);
	void process();

	bool isFinished()
	{ return mFinished; }

private:
	std::vector< SmartPointer<CObject> > mObjects;
	bool mFinished;
	unsigned int mTime;
	CMap &mMap;
};
#endif /* CTITLE_H_ */
