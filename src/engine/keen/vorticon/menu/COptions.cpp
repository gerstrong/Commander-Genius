/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"
#include <base/CInput.h>
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/CSettings.h"

namespace vorticon
{

COptions::COptions() :
VorticonMenu( GsRect<float>(0.1f, 0.14f, 0.8f, gBehaviorEngine.mOptions.size()*0.07f) )
{
    for( auto &optionIt : gBehaviorEngine.mOptions)
	{
        mpOptionList.push_back( new Switch(optionIt.second.menuname) );
		mpMenuDialog->addControl( mpOptionList.back() );
	}
}

void COptions::refresh()
{
    std::list<Switch*>::iterator it = mpOptionList.begin();

    for( auto optIt = gBehaviorEngine.mOptions.begin() ;
         it != mpOptionList.end() ; it++, optIt++ )
    {
        (*it)->enable( optIt->second.value );
    }

}


void COptions::ponder(const float deltaT)
{
    CBaseMenu::ponder(0);

    auto it = mpOptionList.begin();

    for( auto optIt = gBehaviorEngine.mOptions.begin() ;
         it != mpOptionList.end() ; it++, optIt++ )
    {
        optIt->second.value = (*it)->isEnabled();
    }
}

void COptions::release()
{
	gSettings.saveGameOptions();
}

}
