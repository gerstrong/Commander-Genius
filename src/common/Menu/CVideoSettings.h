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

#include "gui/CGUIComboSelection.h"
#include "gui/CGUISwitch.h"
#include "gui/CGUINumberControl.h"

class CVideoSettings : public CBaseMenu {
public:
	CVideoSettings(const Uint8 dlg_theme);

	void init();

	void release();

private:

	CGUIComboSelection	*mpOGLFilterSelection;
	CGUINumberControl	*mpFPSSelection;
	CGUISwitch			*mpOpenGLSwitch;
	CGUIComboSelection	*mpScalerSelection;
	CGUISwitch			*mpFullScreenSwitch;
	CGUISwitch			*mpShowFPSSwitch;
	CGUISwitch			*mpSFXSwitch;
	CGUIButton			*mpCameraButton;


	/*CVidConfig m_Vidconfig;
	std::list< CRect<Uint16> > m_Resolutionlist;
	std::list< CRect<Uint16> >::iterator m_Resolution_pos;
	int m_current; //used to show what is currently selected without actually changing the value
	bool m_changed;
	std::string m_usedSoftwareVideoDriver;
	bool &m_restartVideo;
	CCameraSettings	*mp_CameraSettings;
	int m_Autoframeskip;*/
};

#endif /* CVIDEOSETTINGS_H_ */
