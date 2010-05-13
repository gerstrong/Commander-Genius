/*
 * CVideoSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CVIDEOSETTINGS_H_
#define CVIDEOSETTINGS_H_

#include "CBaseMenu.h"
#include "../../sdl/CVideoDriver.h"
#include "../../dialog/CDialog.h"
#include <SDL.h>
#include <string>

class CVideoSettings : public CBaseMenu {
public:
	CVideoSettings(char &menu_type, Uint8 dlg_theme);
	void processSpecific();
	virtual ~CVideoSettings();

private:
	Uint16 m_Zoom, m_ScaleXFilter, m_OGL_filter, m_Autoframeskip;
	bool m_SpecialFX;
	bool m_FSmode, m_Opengl, m_changed;
	int m_left, m_right, m_up, m_down, m_speed, m_changeint;
	st_resolution m_Resolution;
	std::string m_usedSoftwareVideoDriver;
};

#endif /* CVIDEOSETTINGS_H_ */
