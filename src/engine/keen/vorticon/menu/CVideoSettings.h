/*
 * CVideoSettings.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CVIDEOSETTINGS_H_
#define CVIDEOSETTINGS_H_

#include <SDL.h>
#include <string>

#include <widgets/GsBaseMenu.h>
#include <widgets/GsComboSelection.h>
#include <widgets/GsSwitch.h>
#include <widgets/GsNumberControl.h>

#include "sdl/CVidConfig.h"


class CVideoSettings : public CBaseMenu
{
public:
	CVideoSettings();

	void init();

	void release();

	CVidConfig mUserVidConf;

private:


	CGUINumberControl	*mpFPSSelection;

#if defined(USE_OPENGL)
	CGUISwitch			*mpOpenGLSwitch;
	CGUIComboSelection	*mpOGLFilterSelection;
#endif

    CGUIComboSelection	*mpFilterSelection;
	CGUIComboSelection	*mpAspectSelection;
	CGUISwitch		*mpVSyncSwitch;
	CGUISwitch		*mpSFXSwitch;
	GsButton		*mpFullScreenSwitch;
    CGUIComboSelection	*mpResolutionSelection;
    CGUIComboSelection	*mpGameResSelection;

};

#endif /* CVIDEOSETTINGS_H_ */
