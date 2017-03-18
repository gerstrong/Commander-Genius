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

#if defined(USE_OPENGL)
    Switch			*mpOpenGLSwitch;
#endif

    Switch          *mpVPadSwitch;

    ComboSelection	*mpRenderScaleQualitySel;

    ComboSelection	*mpFilterSelection;
    ComboSelection	*mpAspectSelection;
    Switch		*mpVSyncSwitch;
    Switch		*mpSFXSwitch;
    Switch		*mpBorderColorSwitch;
    Switch		*mpHorizBordersSwitch;
    Button		*mpFullScreenSwitch;
    ComboSelection	*mpResolutionSelection;
    ComboSelection	*mpGameResSelection;

};

}

#endif /* CVIDEOSETTINGS_H_ */
