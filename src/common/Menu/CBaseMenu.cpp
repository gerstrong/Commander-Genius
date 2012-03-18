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

	mpMenuDialog->processLogic();

}
