/*
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include <string.h>
#include "CLatch.h"

class CGame {
public:
	CGame();
	virtual ~CGame();

	int loadResources(unsigned short Episode, char *DataDirectory);
	short runCycle(stCloneKeenPlus *pCKP);
	void preallocateCKP(stCloneKeenPlus *pCKP);
	CLatch *getLatch(void);

private:
	static const unsigned short MAX_TEXT_LENGTH = 256;
	unsigned short m_Episode;
	char m_DataDirectory[MAX_TEXT_LENGTH];
	int current_demo;

	CLatch *Latch;
};

#endif /* CGAME_H_ */
