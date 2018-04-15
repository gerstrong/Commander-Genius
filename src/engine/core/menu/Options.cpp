/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "Options.h"
#include <base/CInput.h>
#include "engine/core/CSettings.h"


COptions::COptions(const GsControl::Style style) :
GameMenu( GsRect<float>(0.1f, 0.14f, 0.8f,
                        gBehaviorEngine.mOptions.size()*0.07f),
                        style)
{
    for( const auto &option :  gBehaviorEngine.mOptions )
	{
        mpOptionList.push_back( new Switch( option.second.menuname, style ) );
		mpMenuDialog->addControl( mpOptionList.back() );
	}    

	setMenuLabel("OPTIONSMENULABEL");
}

void COptions::refresh()
{
    std::list<Switch*>::iterator it = mpOptionList.begin();

    for( const auto &option :  gBehaviorEngine.mOptions )
    {
        (*it)->enable( option.second.value );
        it++;
    }
}


void COptions::ponder(const float deltaT)
{
    GameMenu::ponder(0);

    auto it = mpOptionList.begin();

    for( auto &option :  gBehaviorEngine.mOptions )
    {
        option.second.value = (*it)->isEnabled();
        it++;
    }
}

void COptions::release()
{
	gSettings.saveGameOptions();
}

