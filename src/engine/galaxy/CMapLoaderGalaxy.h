/*
 * CMapLoaderGalaxy.h
 *
 *  Created on: 29.05.2010
 *      Author: gerstrong
 */

#ifndef CMAPLOADERGALAXY_H_
#define CMAPLOADERGALAXY_H_

#include "../../fileio/CExeFile.h"

class CMapLoaderGalaxy
{
public:
	CMapLoaderGalaxy(CExeFile &ExeFile);
	bool loadMap();
	virtual ~CMapLoaderGalaxy();
};

#endif /* CMAPLOADERGALAXY_H_ */
