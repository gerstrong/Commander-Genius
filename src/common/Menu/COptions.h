/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "CBaseMenu.h"

class COptions : public CBaseMenu {
public:
	COptions(char &menu_type);
	void processSpecific();
	virtual ~COptions();
};

#endif /* COPTIONS_H_ */
