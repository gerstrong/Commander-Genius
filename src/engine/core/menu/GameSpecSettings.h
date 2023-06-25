/*
 * GameSpecSettings.h
 *
 *  Created on: 2009-11-28
 *  Revised on: 2023-06-11
 *      Author: gerstrong
 *
 *  This class is meant to manage video settings that are game specific,
 *  maybe even others, let's see in future.
 *  For global video settings we call that display here, so please refer to display
 *  settings if you are interested in that.
 */

#ifndef CGAMESPECSETTINGS_H_
#define CGAMESPECSETTINGS_H_

#include <SDL.h>
#include <string>

#include <widgets/GsBaseMenu.h>
#include "widgets/ComboSelection.h"
#include "widgets/Switch.h"
#include "widgets/NumberControl.h"

#include "GameMenu.h"

#include <base/video/CVidConfig.h>
#include <memory>
#include <string_view>


class GameSpecSettings : public GameMenu
{
public:
    GameSpecSettings(const Style style,
                   const std::string_view &engine_name);

    void refresh() override;

    void release() override;

	CVidConfig mUsersConf;

private:

    std::shared_ptr<NumberControl>  mpFPSSelection;
    std::shared_ptr<ComboSelection>	mpGameResSelection;
    std::shared_ptr<Switch>		    mpBorderColorSwitch;
    std::shared_ptr<NumberControl>	mpHorizBordersSelection;

    std::string mEngineName;
};


#endif /* CGAMESPECSETTINGS_H_ */
