/*
 * CBaseMenu.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <widgets/GsMenuController.h>


#include "GsBaseMenu.h"
#include "GsBitmap.h"

CBaseMenu::CBaseMenu(const GsRect<float>& rect,
                     GsButton *returnButton,
                     const CGUIDialog::FXState fx) :
mpMenuDialog( new CGUIDialog(rect, fx) ),
mpReturnButton(returnButton)
{}


void CBaseMenu::select(const size_t value)
{
	if(value >= mpMenuDialog->getControlList().size())
		return;

	for(size_t c=0 ; c<value ; c++)
		mpMenuDialog->sendEvent( new CommandEvent(IC_DOWN) );
}


void CBaseMenu::setMenuLabel(const std::string &label)
{
    GsRect<float> rect(-0.08f, -0.08f, 1.0f, 1.0f);
    CGUIBitmap* control = new CGUIBitmap(label);
    mpMenuDialog->addControl(control, rect);
    control->enable(false);
}


void CBaseMenu::ponder(const float deltaT)
{
    // If IC_BACK is invoked, make the menu controller close the controller
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseMenuEvent() );
        return;
    }

	// Command (Keyboard/Joystick) are handled here
	for( int cmd = IC_LEFT ; cmd < MAX_COMMANDS ; cmd++ )
	{
		if( gInput.getPressedCommand(cmd) )
		{
		    std::shared_ptr<CEvent> command(new CommandEvent( static_cast<InputCommands>(cmd) ));
            sendEvent(command);
		    break;
		}
	}

	mpMenuDialog->processLogic();
}


void CBaseMenu::render()
{
    mpMenuDialog->processRendering();
}
