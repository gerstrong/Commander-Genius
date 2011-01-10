/*
 * CStory.h
 *
 *  Created on: 16.11.2009
 *      Author: gerstrong
 */

#ifndef CSTORY_H_
#define CSTORY_H_

#include "CInfoScene.h"
#include "common/CMap.h"
#include "dialog/CTextViewer.h"
#include "fileio/CExeFile.h"
#include <string.h>

class CStory : public CInfoScene {
public:
	CStory(CExeFile &ExeFile);

	void process();

	virtual ~CStory();

private:
	CMap *mp_Map;
	CTextViewer *mp_TextViewer;
	SDL_Surface *mp_Scrollsurface;
};

#endif /* CSTORY_H_ */
