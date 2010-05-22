/*
 * CCameraSettings.h
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#ifndef CCAMERASETTINGS_H_
#define CCAMERASETTINGS_H_

#include "CBaseMenu.h"

class CCameraSettings : public CBaseMenu
{
public:
	CCameraSettings(Uint8 dlg_theme);
	void processSpecific();
	virtual ~CCameraSettings();

private:
	int m_left, m_right, m_up, m_down, m_speed, m_changeint;
};

#endif /* CCAMERASETTINGS_H_ */
