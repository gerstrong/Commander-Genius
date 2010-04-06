/*
 * CMenu.h
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 */

#ifndef CMENU_GALAXY_H_
#define CMENU_GALAXY_H_

#include "../../graphics/CBitmap.h"

namespace galaxy
{

class CMenu
{
public:
	CMenu();
	void process();
	void processMainMenu();
	virtual ~CMenu();

	CBitmap m_BackgroundBitmap;
};

}

#endif /* CMENU_GALAXY_H_ */
