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

#include "VorticonMenu.h"
#include "widgets/ComboSelection.h"
#include "widgets/Switch.h"
#include "widgets/NumberControl.h"
#include "widgets/Button.h"

#include <base/video/CVidConfig.h>

namespace vorticon
{

class CVideoSettings : public VorticonMenu
{
public:
	CVideoSettings();

    void refresh();

	void release();

	CVidConfig mUserVidConf;

private:


    NumberControl	*mpFPSSelection;
    Switch		*mpFrameSkip;

#if defined(USE_OPENGL)
    Switch			*mpOpenGLSwitch;
#endif

    Switch          *mpShowCursorSwitch = nullptr;

    Switch          *mpVPadSwitch = nullptr;

    ComboSelection	*mpRenderScaleQualitySel = nullptr;

    ComboSelection	*mpFilterSelection = nullptr;
    ComboSelection	*mpAspectSelection = nullptr;
    Switch		*mpVSyncSwitch = nullptr;
    Switch		*mpBorderColorSwitch = nullptr;
    NumberControl		*mpHorizBordersSelection = nullptr;
    Button		*mpFullScreenSwitch = nullptr;
    ComboSelection	*mpResolutionSelection = nullptr;
    ComboSelection	*mpGameResSelection = nullptr;

};

}

#endif /* CVIDEOSETTINGS_H_ */
