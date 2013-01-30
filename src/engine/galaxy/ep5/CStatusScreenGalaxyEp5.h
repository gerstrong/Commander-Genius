/*
 * CStatusScreenGalaxyEp5.h
 *
 *  Created on: 28.09.2011
 *      Author: gerstrong
 *
 *  The specific Statusscreen for Keen 5 only!
 */

#ifndef CSTATUSSCREENGALAXYEP5_H_
#define CSTATUSSCREENGALAXYEP5_H_

#include "engine/galaxy/common/CStatusScreenGalaxy.h"

class CStatusScreenGalaxyEp5: public CStatusScreenGalaxy
{
public:
	CStatusScreenGalaxyEp5(const stItemGalaxy& Item, const std::string &LevelName);
    
	void GenerateStatus();
};

#endif /* CSTATUSSCREENGALAXYEP5_H_ */
