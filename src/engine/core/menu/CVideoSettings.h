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

#include "GameMenu.h"

#include <base/video/CVidConfig.h>


class CVideoSettings : public GameMenu
{
public:
	CVideoSettings(const GsControl::Style style);

    void refresh();

	void release();

	CVidConfig mUsersConf;

private:

    Switch          *mpTiltScreenSwitch = nullptr;
    Switch          *mpShowCursorSwitch = nullptr;
    Switch			*mpVPadSwitch  = nullptr;
    NumberControl	*mpFPSSelection  = nullptr;
    Switch		    *mpFrameSkip  = nullptr;    
    ComboSelection	*mpGameResSelection = nullptr;        
    Switch		    *mpBorderColorSwitch = nullptr;
    NumberControl	*mpHorizBordersSelection = nullptr;
    

};


#endif /* CVIDEOSETTINGS_H_ */
