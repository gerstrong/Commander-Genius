/*
 * CCredits.h
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#ifndef CCREDITS_H_
#define CCREDITS_H_

#include <vector>
#include <string>
#include "CInfoScene.h"
#include "common/CMap.h"

class CCredits : public CInfoScene {
public:
	
	void init();
	void process();
	void teardown();
	
private:
	std::string m_scrolltext[54];
	int m_mid[54];
	int m_timer;
	int m_scrolly;
	
	SmartPointer<CMap> mpMap;
	SmartPointer<SDL_Surface> mpDrawSfc;
};

#endif /* CCREDITS_H_ */
