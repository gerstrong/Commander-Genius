/*
 * COrderingInfo.h
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#ifndef CORDERINGINFO_H_
#define CORDERINGINFO_H_

#include <vector>
#include <string>
#include "CInfoScene.h"
#include "../../common/CMap.h"

class COrderingInfo : public CInfoScene {
public:
	COrderingInfo(const std::string &datadirectory, const char &episode);
	virtual ~COrderingInfo();
	
	void process();
	
private:
	std::vector<std::string> m_Textline;
	
	int m_starty;			// start of y-coordinate in textheights
	int m_numberoflines;	// number of lines to print
	
	CMap m_Map;
	SDL_Surface *mp_Scrollsurface;
};

#endif /* CORDERINGINFO_H_ */
