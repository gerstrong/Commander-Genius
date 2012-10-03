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
#include <SDL.h>
#include <string>

#include "gui/CGUIComboSelection.h"
#include "gui/CGUISwitch.h"
#include "gui/CGUINumberControl.h"

class CVideoSettings : public CBaseMenu
{
public:
	CVideoSettings();

	void init();

	void release();

	CVidConfig 			mUserVidConf;

private:


	CGUINumberControl	*mpFPSSelection;

#if defined(USE_OPENGL)
	CGUISwitch			*mpOpenGLSwitch;
	CGUIComboSelection	*mpOGLFilterSelection;
#endif

	CGUIComboSelection	*mpScalerSelection;
	CGUISwitch			*mpAspectSwitch;
	CGUISwitch			*mpVSyncSwitch;
	CGUISwitch			*mpShowFPSSwitch;
	CGUISwitch			*mpSFXSwitch;
	CGUIButton			*mpFullScreenSwitch;
	CGUIComboSelection	*mpResolutionSelection;

};

#endif /* CVIDEOSETTINGS_H_ */
