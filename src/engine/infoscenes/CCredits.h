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
#include "../../common/CMap.h"

class CCredits : public CInfoScene {
public:
	CCredits(const std::string &datadirectory, const char &episode);
	virtual ~CCredits();
	
	void process();
	
private:
	std::string m_scrolltext[54];
	int m_mid[54];
	int m_timer;
	int m_scrolly;
	
	SmartPointer<CMap> mpMap;
	SDL_Surface *mp_Scrollsurface;
};

#endif /* CCREDITS_H_ */
