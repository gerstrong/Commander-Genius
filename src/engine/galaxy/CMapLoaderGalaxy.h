/*
 * CMapLoaderGalaxy.h
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXY_H_
#define CMAPLOADERGALAXY_H_

#include "../../fileio/CExeFile.h"
#include "../../fileio/TypeDefinitions.h"
#include "../../common/CMap.h"

#include <string>
#include <SDL.h>

class CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxy(CExeFile &ExeFile);
	byte *getMapheadPtr();
	bool loadMap(CMap &Map, Uint8 level);
	virtual ~CMapLoaderGalaxy();

private:
	CExeFile &m_ExeFile;
};

#endif /* CMAPLOADERGALAXY_H_ */
