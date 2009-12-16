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
#include "../../common/CMap.h"
#include "../../vorticon/ai/CEGABitmap.h"

class CAbout : public CInfoScene {
public:
	CAbout(std::string &datadirectory, char &episode, std::string type);
	virtual ~CAbout();
	
	void process();
	
private:
	CMap *mp_Map;
	SDL_Surface *mp_Scrollsurface;
	SDL_Surface *mp_LogoBMP;
};

#endif /* CABOUT_H_ */
