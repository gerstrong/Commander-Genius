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
#include "../graphics/CGfxEngine.h"
#include "../common/CObject.h"
#include "../common/CMap.h"

class CTitle {
public:

	CTitle(std::vector<CObject*> &Objects, CMap &map);
	virtual ~CTitle();

	bool init(int Episode);
	void process();

	bool isFinished()
	{ return m_finished; }

private:
	std::vector<CObject*> &m_objects;
	bool m_finished;
	unsigned int m_time;
	CMap &m_map;
};
#endif /* CTITLE_H_ */
