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
	COptions(Uint8 dlg_theme, stOption *p_option);
	void processSpecific();
	virtual ~COptions();

private:
	stOption *mp_option;
	int m_select;
};

#endif /* COPTIONS_H_ */
