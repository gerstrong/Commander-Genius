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
#include "../../fileio/CExeFile.h"

class COrderingInfo : public CInfoScene {
public:
	COrderingInfo( CExeFile &ExeFile );
	virtual ~COrderingInfo();
	
	void process();
	
private:
	std::vector<std::string> m_Textline;
	
	int m_starty;			// start of y-coordinate in textheights
	int m_numberoflines;	// number of lines to print
	
	SmartPointer<CMap> mpMap;
	SDL_Surface *mp_Scrollsurface;
};

#endif /* CORDERINGINFO_H_ */
