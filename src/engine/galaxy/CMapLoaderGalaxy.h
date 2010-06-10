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
	size_t getMapheadOffset();
	bool gotoSignature(std::ifstream &MapFile);
	bool loadMap(CMap &Map, Uint8 level);
	virtual ~CMapLoaderGalaxy();

private:
	void unpackPlaneData(std::vector<word> &Plane, std::ifstream &MapFile,
			CMap &Map, size_t PlaneNumber,
			longword offset, longword length,
			word magic_word);

	CExeFile &m_ExeFile;
};

#endif /* CMAPLOADERGALAXY_H_ */
