/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "Options.h"
#include <base/CInput.h>
#include "engine/core/CSettings.h"
#include "engine/core/options.h"
#include <string>
#include <set>

#if defined(EMBEDDED)
const std::set<GameOption> filteredOptions = { GameOption::NOTYPING };
#else
const std::set<GameOption> filteredOptions = {};
#endif

COptions::COptions(const Style style) :
GameMenu( GsRect<float>(0.1f, 0.14f, 0.8f,
                        gBehaviorEngine.mOptions.size()*0.07f),
                        style)
{


    for( const auto &option :  gBehaviorEngine.mOptions )
	{
        if(filteredOptions.find(option.first) != filteredOptions.end())
            continue;

        mpOptionList.push_back( new Switch( option.second.menuname, style ) );
		mpMenuDialog->add( mpOptionList.back() );
    }

	setMenuLabel("OPTIONSMENULABEL");

    mpMenuDialog->fit();
    select(1);
}

void COptions::refresh()
{
    std::list<Switch*>::iterator it = mpOptionList.begin();

    for( const auto &option :  gBehaviorEngine.mOptions )
    {
        if(filteredOptions.find(option.first) != filteredOptions.end())
            continue;

        (*it)->enable( option.second.value );
        it++;
    }
}


void COptions::ponder(const float )
{
    GameMenu::ponder(0);

    auto it = mpOptionList.begin();

    for( auto &option :  gBehaviorEngine.mOptions )
    {
        if(filteredOptions.find(option.first) != filteredOptions.end())
            continue;

        option.second.value = (*it)->isEnabled();
        it++;
    }
}

void COptions::release()
{
	gSettings.saveGameOptions();
}

