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

	CVidConfig 			mUserVidConf;

private:

	CGUIComboSelection	*mpOGLFilterSelection;
	CGUINumberControl	*mpFPSSelection;
	CGUISwitch			*mpOpenGLSwitch;
	CGUIComboSelection	*mpScalerSelection;
	CGUISwitch			*mpShowFPSSwitch;
	CGUISwitch			*mpSFXSwitch;
	CGUIButton			*mpCameraButton;
	CGUIButton			*mpFullScreenSwitch;
	CGUIComboSelection	*mpResolutionSelection;

};

#endif /* CVIDEOSETTINGS_H_ */
