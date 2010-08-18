/*
 * CLevelPlay.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 *
 *  This class will manage the entire gameplay in one level
 */

#ifndef CLEVELPLAY_H_
#define CLEVELPLAY_H_

#include "common/CObject.h"
#include "fileio/CExeFile.h"
#include <vector>

namespace galaxy {

class CLevelPlay {
public:
	CLevelPlay(CExeFile &ExeFile);
	bool isActive();
	void setActive(bool value);

	bool loadLevel(Uint16 level);

	void process();

private:
	std::vector<CObject*> m_ObjectPtr;
	bool m_active;

	CMap m_Map;
	CExeFile &m_ExeFile;
};

}

#endif /* CLEVELPLAY_H_ */
