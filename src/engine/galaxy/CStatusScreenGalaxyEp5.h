/*
 * CStatusScreenGalaxyEp5.h
 *
 *  Created on: 28.07.2011
 *      Author: gerstrong
 *
 *  The specific Statusscreen for Keen 4 only!
 */

#ifndef CSTATUSSCREENGALAXYEP5_H_
#define CSTATUSSCREENGALAXYEP5_H_

#include "CStatusScreenGalaxy.h"

class CStatusScreenGalaxyEp5: public CStatusScreenGalaxy
{
public:
	CStatusScreenGalaxyEp5(const stItemGalaxy& Item, const std::string &LevelName);

	void GenerateStatus();
};

#endif /* CSTATUSSCREENGALAXYEP4_H_ */
