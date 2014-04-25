/*
 * CAbout.h
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#ifndef CABOUT_H_
#define CABOUT_H_

#include <vector>
#include <string>
#include <memory>
#include "CInfoScene.h"
#include "graphics/GsBitmap.h"
#include "engine/core/CMap.h"
#include "fileio/CExeFile.h"

class CAbout : public CInfoScene
{
public:
	CAbout(const std::string &type);
	
	void init();
    void ponder();
    void render();
	void teardown();
	
private:
	static const int m_numberoflines=11;	// number of lines to print
	GsBitmap* mp_bmp;
	
	std::shared_ptr <CMap> mpMap;
	std::shared_ptr<SDL_Surface> mpLogoBMP;
	SDL_Rect m_logo_rect;
	std::vector<std::string> m_lines;
	std::string m_type;
	std::shared_ptr<SDL_Surface> mpDrawSfc;
};

#endif /* CABOUT_H_ */
