/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "CBaseMenu.h"
#include <SDL.h>

class CAudioSettings : public CBaseMenu {
public:
	CAudioSettings(char &menu_type);
	void processSpecific();
	virtual ~CAudioSettings();

private:
	int m_Rate;
	Uint16 m_Format;
	char m_Mode;
};

#endif /* CAUDIOSETTINGS_H_ */
