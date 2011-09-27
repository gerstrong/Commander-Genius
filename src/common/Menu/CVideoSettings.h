/*
 * CVideoSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CVIDEOSETTINGS_H_
#define CVIDEOSETTINGS_H_

#include "CBaseMenu.h"
#include "CCameraSettings.h"
#include "sdl/CVideoDriver.h"
#include "dialog/CDialog.h"
#include <SDL.h>
#include <string>

class CVideoSettings : public CBaseMenu {
public:
	CVideoSettings(Uint8 dlg_theme, bool &restartVideo);
	void processSpecific();

private:

	CVidConfig m_Vidconfig;
	std::list<CRect> m_Resolutionlist;
	std::list<CRect>::iterator m_Resolution_pos;
	int m_current; //used to show what is currently selected without actually changing the value
	bool m_changed;
	std::string m_usedSoftwareVideoDriver;
	bool &m_restartVideo;
	CCameraSettings	*mp_CameraSettings;
	int m_Autoframeskip;
};

#endif /* CVIDEOSETTINGS_H_ */
