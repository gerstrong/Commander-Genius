/*
 * CControlsettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CCONTROLSETTINGS_H_
#define CCONTROLSETTINGS_H_

#include "CBaseMenu.h"

class CControlsettings : public CBaseMenu {
public:
	CControlsettings(char &menu_type);
	void processSpecific();
	virtual ~CControlsettings();
};

#endif /* CCONTROLSETTINGS_H_ */
