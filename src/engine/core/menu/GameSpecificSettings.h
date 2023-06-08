/*
 * GameSpecificSetting.h
 *
 *  Created on: 08.06.2023
 *      Author: gerstrong
 *
 * Settings that are only valid for the currently running game
 */

#ifndef CGAMESPECSETTINGS_H_
#define CGAMESPECSETTINGS_H_

#include <SDL.h>
#include <string>

#include <widgets/GsBaseMenu.h>
#include "widgets/ComboSelection.h"

#include "GameMenu.h"

#include <base/video/CVidConfig.h>
#include <memory>


class CGameSpecificSettings : public GameMenu
{
public:
    CGameSpecificSettings(const Style style);

    void refresh();

    void release();

    CVidConfig mMyNewConf;

private:

    std::shared_ptr<ComboSelection>	mpGameResSelection;

};


#endif /* CGAMESPECSETTINGS_H_ */
