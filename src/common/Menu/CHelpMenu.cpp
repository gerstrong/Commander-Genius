/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"

#include "engine/infoscenes/CStory.h"
#include "engine/infoscenes/CCredits.h"
#include "engine/infoscenes/COrderingInfo.h"
#include "engine/infoscenes/CAbout.h"
#include "engine/infoscenes/CHelp.h"
#include "engine/infoscenes/CPreviews.h"


CHelpMenu::CHelpMenu() :
CBaseMenu( CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries
	mpMenuDialog->addControl(new CGUIButton( "The Menu",
												new StartInfoSceneEvent( new CHelp("Menu") ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "The Game",
												new StartInfoSceneEvent( new CHelp("Game") ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "The Story",
												new StartInfoSceneEvent( new CStory ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Ordering Info",
												new StartInfoSceneEvent( new COrderingInfo ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "About ID",
												new StartInfoSceneEvent( new CAbout("ID") ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "About CG",
												new StartInfoSceneEvent( new CAbout("CG") ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Credits",
												new StartInfoSceneEvent( new CCredits() ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Previews!",
												new StartInfoSceneEvent( new CPreviews() ),
												CGUIButton::VORTICON ) );

}


