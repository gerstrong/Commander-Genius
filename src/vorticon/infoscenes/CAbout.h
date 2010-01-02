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
#include "CInfoScene.h"
#include "../../graphics/CBitmap.h"
#include "../../common/CMap.h"

class CAbout : public CInfoScene {
public:
	CAbout(std::string &datadirectory, char &episode, const std::string& type);
	virtual ~CAbout();
	
	void process();
	
private:
	int m_numberoflines;	// number of lines to print
	CBitmap* mp_bmp;
	
	CMap *mp_Map;
	SDL_Surface *mp_Scrollsurface;
	SDL_Surface *mp_LogoBMP;
	SDL_Rect m_logo_rect;
	std::vector<std::string> m_lines;
	std::string m_type;
};

#endif /* CABOUT_H_ */
