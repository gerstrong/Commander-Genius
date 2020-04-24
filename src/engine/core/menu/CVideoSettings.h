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
#include <memory>


class CVideoSettings : public GameMenu
{
public:
	CVideoSettings(const Style style);

    void refresh();

	void release();

	CVidConfig mUsersConf;

private:

    std::shared_ptr<Switch>         mpTiltScreenSwitch;
    std::shared_ptr<NumberControl>  mpFPSSelection;
    std::shared_ptr<Switch>		    mpFrameSkip;
    std::shared_ptr<ComboSelection>	mpGameResSelection;
    std::shared_ptr<Switch>		    mpBorderColorSwitch;
    std::shared_ptr<NumberControl>	mpHorizBordersSelection;

};


#endif /* CVIDEOSETTINGS_H_ */
