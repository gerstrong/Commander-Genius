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
VorticonMenu( GsRect<float>(0.1f, 0.14f, 0.8f, NUM_OPTIONS*0.07f) ),
mpOption(g_pBehaviorEngine->m_option)
{

	for( int i = 0 ; i < NUM_OPTIONS ; i++ )
	{
        mpOptionList.push_back( new Switch(mpOption[i].menuname) );
		mpMenuDialog->addControl( mpOptionList.back() );
	}
}

void COptions::refresh()
{
    std::list<Switch*>::iterator it = mpOptionList.begin();

	for( int i=0 ; it != mpOptionList.end() ; it++, i++ )
		(*it)->enable( mpOption[i].value );

}


void COptions::ponder(const float deltaT)
{
    CBaseMenu::ponder(0);

    auto it = mpOptionList.begin();

	for( int i=0 ; it != mpOptionList.end() ; it++, i++ )
		mpOption[i].value = (*it)->isEnabled();
}

void COptions::release()
{
	g_pSettings->saveGameOptions();
}

}
