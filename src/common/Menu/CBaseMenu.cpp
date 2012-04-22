/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "sdl/input/CInput.h"
#include "CBaseMenu.h"
#include "common/Menu/CMenuController.h"
#include "gui/CGUIBitmap.h"

CBaseMenu::CBaseMenu(const CRect<float>& rect) :
mpMenuDialog( new CGUIDialog(rect) )
{
	const char closeChar = 0x1F;
	std::string closeString;
	closeString = closeChar;
	CGUIButton*	pButton;

	if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		pButton	= new CGUIButton( closeString, new CloseMenuEvent(), CGUIButton::GALAXY );
		mpMenuDialog->addControl( pButton, CRect<float>(0.0f, 0.0f, 0.03f/rect.w, 0.03f/rect.h) );
	}
	else
	{
		pButton	= new CGUIButton( closeString, new CloseMenuEvent(), CGUIButton::NONE );
		mpMenuDialog->addControl( pButton, CRect<float>(0.0f, 0.0f, 0.06f/rect.w, 0.06f/rect.h) );
	}

	mpReturnButton = pButton;

	if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		CRect<float> rect(0.35f, 0.28f, 0.5f, 0.5f);
		mpMenuDialog->setRect(rect);
	}

}

void CBaseMenu::setMenuLabel(const std::string &label)
{
	if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		CRect<float> rect(-0.08f, -0.08f, 1.0f, 1.0f);
		mpMenuDialog->addControl(new CGUIBitmap(label), rect);
	}
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
