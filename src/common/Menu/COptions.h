/*
 * COptions.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef COPTIONS_H_
#define COPTIONS_H_

#include "CBaseMenu.h"
#include "gui/CGUISwitch.h"
#include "common/CBehaviorEngine.h"
#include <list>

class COptions : public CBaseMenu {
public:
	COptions( const Uint8 dlg_theme );

	void init();

	void process();

private:
	std::list<CGUISwitch*> mpOptionList;
	stOption *mpOption;
};

#endif /* COPTIONS_H_ */
