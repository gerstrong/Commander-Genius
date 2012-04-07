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


CHelpMenu::CHelpMenu( const Uint8 dlgTheme ) :
CBaseMenu( dlgTheme, CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	/*mpMenuDialog->addControl(new CGUIButton( "The Menu",
												new OpenMenuEvent( new CLoadMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "The Game",
												new OpenMenuEvent( new CSaveMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "The Story",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Ordering Info",
												new StartHighscoresEvent(),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "About ID",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "About CG",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Credits",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Previews!",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );*/


}


/*void CHelpMenu::processSpecific()
{
	if( m_selection != NO_SELECTION)
	{
		//m_Map.m_animation_enabled = false;
		// no cleanups here, because later we return back to that menu
		switch(m_selection)
		{
		case 0:
			mp_InfoScene = new CHelp(m_ExeFile, "Menu");
			break;
		case 1:
			mp_InfoScene = new CHelp(m_ExeFile, "Game");
			break;
		case 2:
			mp_InfoScene = new CStory(m_ExeFile);
			break;
		case 3:
			mp_InfoScene = new COrderingInfo(m_ExeFile);
			break;
		case 4:
			mp_InfoScene = new CAbout(m_ExeFile, "ID");
			break;
		case 5:
			mp_InfoScene = new CAbout(m_ExeFile, "CG");
			break;
		case 6:
			mp_InfoScene = new CCredits(m_ExeFile.getDataDirectory(),
										m_ExeFile.getEpisode());
			break;
		case 7:
			mp_InfoScene = new CPreviews(m_ExeFile);
			break;
		}
		m_hideobjects = true;
		m_selection = NO_SELECTION;
		m_mustclose = true;
	}
}*/

