/*
 * CMenu.h
 *
 *  Created on: 16.09.2009
 *      Author: gerstrong
 */

#ifndef CMENU_H_
#define CMENU_H_

#include "../keen.h"

class CMenu {
public:
	CMenu();
	virtual ~CMenu();

	bool loadStartMenu(stCloneKeenPlus *pCKP);
};

#endif /* CMENU_H_ */
