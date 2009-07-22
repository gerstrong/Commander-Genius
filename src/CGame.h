/*
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include <string.h>
#include "vorticon/CEGAGraphics.h"
#include "vorticon/CMessages.h"

class CGame {
public:
	CGame();
	virtual ~CGame();

	int loadResources(unsigned short Episode, char *DataDirectory);
	void freeResources(void);
	short runCycle(stCloneKeenPlus *pCKP);
	void preallocateCKP(stCloneKeenPlus *pCKP);

private:
	static const unsigned short MAX_TEXT_LENGTH = 256;
	unsigned short m_Episode;
	char m_DataDirectory[MAX_TEXT_LENGTH];
	int current_demo;

	CEGAGraphics *EGAGraphics;
	CMessages *m_Messages;
	CTileLoader *TileLoader;
};

#endif /* CGAME_H_ */
