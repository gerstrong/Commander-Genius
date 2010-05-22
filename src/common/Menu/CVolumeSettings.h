/*
 * CVolumeSettings.h
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#ifndef CVOLUMESETTINGS_H_
#define CVOLUMESETTINGS_H_

#include "CBaseMenu.h"

#include <SDL.h>

class CVolumeSettings : public CBaseMenu
{
public:
	CVolumeSettings(Uint8 dlg_theme);
	void processSpecific();
	virtual ~CVolumeSettings();
};

#endif /* CVOLUMESETTINGS_H_ */
