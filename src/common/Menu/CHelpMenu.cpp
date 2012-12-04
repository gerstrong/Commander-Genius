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
#include "fileio/ResourceMgmt.h"


CHelpMenu::CHelpMenu() :
CBaseMenu( CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries

	mpMenuDialog->addControl(new CGUIButton( "The Game",
						new StartInfoSceneEvent( new CHelp("Game") ) ) );

	mpMenuDialog->addControl(new CGUIButton( "The Story",
						new StartInfoSceneEvent( new CStory ) ) );

	mpMenuDialog->addControl(new CGUIButton( "Ordering Info",
						new StartInfoSceneEvent( new COrderingInfo ) ) );

	mpMenuDialog->addControl(new CGUIButton( "About ID",
						new StartInfoSceneEvent( new CAbout("ID") ) ) );

	mpMenuDialog->addControl(new CGUIButton( "About CG",
						new StartInfoSceneEvent( new CAbout("CG") ) ) );

	mpMenuDialog->addControl(new CGUIButton( "Credits",
						new StartInfoSceneEvent( new CCredits() ) ) );
	
	std::string filename = "previews.ck";
	filename += itoa(g_pBehaviorEngine->getEpisode());	
	filename = getResourceFilename( filename, g_pBehaviorEngine->m_ExeFile.getDataDirectory(), false, false);

	if( filename != "" )
	{
	    mpMenuDialog->addControl(new CGUIButton( "Previews!",
					new StartInfoSceneEvent( new CPreviews() ) ) );
		    
	}	


}


