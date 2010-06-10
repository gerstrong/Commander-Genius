/*
 * CEGAGraphics.h
 *
 *  Created on: 27.12.2009
 *      Author: gerstrong
 */

#ifndef CGALAXY_EGAGRAPHICS_H_
#define CGALAXY_EGAGRAPHICS_H_

#include <string>
#include <vector>
#include <SDL.h>
#include "../../fileio/CExeFile.h"
#include "../CEGAGraphics.h"

namespace galaxy
{

#include "EGAStructs.h"
//
class CEGAGraphicsGalaxy : public ::CEGAGraphics
{
public:
	CEGAGraphicsGalaxy(CExeFile &ExeFile);

	bool loadData();
	void extractPicture(SDL_Surface *sfc,
			std::vector<unsigned char> &data, size_t Width, size_t Height);
	bool begin();
	Uint8 getBit(unsigned char data, Uint8 leftshift);
	bool readfonts();
	bool readBitmaps();

	virtual ~CEGAGraphicsGalaxy();

private:
	std::vector<unsigned long> m_egahead;
	std::vector<ChunkStruct> m_egagraph;

	CExeFile &m_Exefile;
};

}

#endif /* CGALAXY_EGAGRAPHICS_H_ */
