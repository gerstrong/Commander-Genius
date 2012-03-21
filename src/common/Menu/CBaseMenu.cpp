/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "CBaseMenu.h"
#include "common/Menu/CMenuController.h"

CBaseMenu::CBaseMenu(const Uint8 dlgTheme, const CRect<float>& rect) :
mpMenuDialog( new CGUIDialog(rect) )
{

	CGUIButton*	pButton	= new CGUIButton( "x", new CloseMenuEvent(), CGUIButton::NONE );

	mpMenuDialog->addControl( pButton, CRect<float>(0.0f, 0.0f, 0.06f/rect.w, 0.06f/rect.h) );

	mpReturnButton = pButton;

}

void CBaseMenu::process()
{

	// Command (Keyboard/Joystick) are handled here
	if( g_pInput->getPressedCommand(IC_LEFT) )
	{
		sendEvent(new CommandEvent(IC_LEFT));
	}
	else if( g_pInput->getPressedCommand(IC_RIGHT) )
	{
		sendEvent(new CommandEvent(IC_RIGHT));
	}
	else if( g_pInput->getPressedCommand(IC_UP) )
	{
		sendEvent(new CommandEvent(IC_UP));
	}
	else if( g_pInput->getPressedCommand(IC_DOWN) )
	{
		sendEvent(new CommandEvent(IC_DOWN));
	}
	else if( g_pInput->getPressedCommand(IC_STATUS) )
	{
		sendEvent(new CommandEvent(IC_STATUS));
	}

	mpMenuDialog->processLogic();

}
