/*
 * DisplaySettings.h
 *
 *  Created on: 09.01.2018
 *      Author: gerstrong
 */

#ifndef DISPLAYSETTINGS_H_INCLUDED
#define DISPLAYSETTINGS_H_INCLUDED

#include <SDL.h>
#include <string>

#include <widgets/GsBaseMenu.h>
#include "widgets/ComboSelection.h"
#include "widgets/Switch.h"
#include "widgets/NumberControl.h"
#include "widgets/Button.h"

#include "GameMenu.h"

#include <base/video/CVidConfig.h>


class DisplaySettings : public GameMenu
{
public:
	DisplaySettings(const GsControl::Style style);

    void refresh();

	void release();

    CVidConfig mUsersConf;

private:

#if defined(USE_OPENGL)
    Switch			*mpOpenGLSwitch = nullptr;
#endif

    ComboSelection	*mpRenderScaleQualitySel = nullptr;
    ComboSelection	*mpFilterSelection = nullptr;
    ComboSelection	*mpAspectSelection = nullptr;
    Switch		    *mpVSyncSwitch = nullptr;
    ComboSelection	*mpResolutionSelection = nullptr;
    GameButton	    *mpFullScreenSwitch = nullptr;            

};


#endif /* DISPLAYSETTINGS_H_INCLUDED */
