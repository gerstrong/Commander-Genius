/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "common/CSettings.h"


COptions::COptions( const Uint8 dlg_theme ) :
CBaseMenu( dlg_theme, CRect<float>(0.1f, 0.24f, 0.8f, NUM_OPTIONS*0.05f) ),
mpOption(g_pBehaviorEngine->m_option)
{

	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	for( int i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		mpOptionList.push_back( new CGUISwitch(mpOption[i].menuname,
											   CGUISwitch::VORTICON) );
		mpMenuDialog->addControl( mpOptionList.back() );
	}

}

void COptions::init()
{
	std::list<CGUISwitch*>::iterator it = mpOptionList.begin();

	for( int i=0 ; it != mpOptionList.end() ; it++, i++ )
		(*it)->enable( mpOption[i].value );

}


void COptions::process()
{
	CBaseMenu::process();

	std::list<CGUISwitch*>::iterator it = mpOptionList.begin();

	for( int i=0 ; it != mpOptionList.end() ; it++, i++ )
		mpOption[i].value = (*it)->isEnabled();
}
