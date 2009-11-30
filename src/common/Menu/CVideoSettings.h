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

class CVideoSettings : public CBaseMenu {
public:
	CVideoSettings(char &menu_type);
	void processSpecific();
	virtual ~CVideoSettings();

private:
	Uint16 m_Zoom, m_ScaleXFilter, m_OGL_filter, m_Autoframeskip;
	bool m_FSmode, m_AspectCorrection, m_Opengl;
	st_resolution m_Resolution;
};

#endif /* CVIDEOSETTINGS_H_ */
