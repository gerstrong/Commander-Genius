/*
 * CStatusScreenGalaxyEp4.h
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 *
 *  The specific Statusscreen for Keen 4 only!
 */

#ifndef CSTATUSSCREENGALAXYEP4_H_
#define CSTATUSSCREENGALAXYEP4_H_

#include "../common/CStatusScreenGalaxy.h"

class CStatusScreenGalaxyEp4 : public CStatusScreenGalaxy {
public:
    CStatusScreenGalaxyEp4(const stItemGalaxy& Item);

	void GenerateStatus();
};

#endif /* CSTATUSSCREENGALAXYEP4_H_ */
