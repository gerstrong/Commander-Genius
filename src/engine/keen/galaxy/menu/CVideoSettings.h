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
#include "widgets/ComboSelection.h"
#include "widgets/Switch.h"
#include "widgets/NumberControl.h"
#include "widgets/Button.h"

#include <base/video/CVidConfig.h>

namespace galaxy
{

class CVideoSettings : public GalaxyMenu
{
public:
	CVideoSettings();

	void init();

	void release();

	CVidConfig mUserVidConf;

private:


    NumberControl	*mpFPSSelection;

#if defined(USE_OPENGL)
    Switch			*mpOpenGLSwitch;
    ComboSelection	*mpOGLFilterSelection;
#endif

    ComboSelection	*mpFilterSelection;
    ComboSelection	*mpAspectSelection;
    Switch		*mpVSyncSwitch;
    Switch		*mpSFXSwitch;
    GalaxyButton		*mpFullScreenSwitch;
    ComboSelection	*mpResolutionSelection;
    ComboSelection	*mpGameResSelection;

};

}

#endif /* CVIDEOSETTINGS_H_ */
