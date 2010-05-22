/*
 * CAudioSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CAUDIOSETTINGS_H_
#define CAUDIOSETTINGS_H_

#include "CBaseMenu.h"
#include "CVolumeSettings.h"
#include <SDL.h>
#include <string>

class CAudioSettings : public CBaseMenu {
public:
	CAudioSettings(Uint8 dlg_theme);
	void processSpecific();
	virtual ~CAudioSettings();

private:

	int m_Rate;
	Uint16 m_Format;
	char m_Mode;
	CVolumeSettings *mp_VolumeMenu;
};

#endif /* CAUDIOSETTINGS_H_ */
