/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "CBaseMenu.h"
#include "../options.h"

class COptions : public CBaseMenu {
public:
	COptions(char &menu_type, stOption *p_option);
	void processSpecific();
	virtual ~COptions();

private:
	stOption *mp_option;
};

#endif /* COPTIONS_H_ */
