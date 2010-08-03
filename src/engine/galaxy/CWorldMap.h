/*
 * CWorldMap.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 *
 *  This is in charge of the ai and physics on the worldmap
 */

#ifndef CWORLDMAP_H_
#define CWORLDMAP_H_

#include "../../common/CObject.h"
#include "../../fileio/CExeFile.h"
#include <vector>

namespace galaxy {

class CWorldMap {
public:
	CWorldMap(CExeFile &ExeFile);
	bool isActive();
	void setActive(bool value);

	void init();
	void process();

	virtual ~CWorldMap();

private:
	std::vector<CObject*> m_ObjectPtr;
	bool m_active;

	CMap m_Map;

	Uint32 m_posx;
	Uint32 m_posy;

	CExeFile &m_ExeFile;

	bool m_newLevel;
};

}

#endif /* CWORLDMAP_H_ */
