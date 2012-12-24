/*
 * CStatusScreenGalaxyEp6.h
 *
 *  Created on: 24.12.2012
 *      Author: gerstrong
 *
 *  The specific Statusscreen for Keen 4 only!
 */

#ifndef CSTATUSSCREENGALAXYEP6_H_
#define CSTATUSSCREENGALAXYEP6_H_

#include "CStatusScreenGalaxy.h"

class CStatusScreenGalaxyEp6: public CStatusScreenGalaxy
{
public:
	CStatusScreenGalaxyEp6(const stItemGalaxy& Item, const std::string &LevelName);

	void GenerateStatus();
};

#endif /* CSTATUSSCREENGALAXYEP6_H_ */
