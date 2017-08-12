/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"

/*#include "engine/infoscenes/CStory.h"
#include "engine/infoscenes/CCredits.h"
#include "engine/infoscenes/COrderingInfo.h"
#include "engine/infoscenes/CAbout.h"
#include "engine/infoscenes/CHelp.h"
#include "engine/infoscenes/CPreviews.h"
#include "fileio/ResourceMgmt.h"


CHelpMenu::CHelpMenu() :
GalaxyMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries

	mpMenuDialog->addControl(new GsButton( "The Game",
						new StartInfoSceneEvent( new CHelp("Game") ) ) );

	mpMenuDialog->addControl(new GsButton( "The Story",
						new StartInfoSceneEvent( new CStory ) ) );

	mpMenuDialog->addControl(new GsButton( "Ordering Info",
						new StartInfoSceneEvent( new COrderingInfo ) ) );

	mpMenuDialog->addControl(new GsButton( "About ID",
						new StartInfoSceneEvent( new CAbout("ID") ) ) );

	mpMenuDialog->addControl(new GsButton( "About CG",
						new StartInfoSceneEvent( new CAbout("CG") ) ) );

	mpMenuDialog->addControl(new GsButton( "Credits",
						new StartInfoSceneEvent( new CCredits() ) ) );
	
	std::string filename = "previews.ck";
	filename += itoa(gBehaviorEngine.getEpisode());	
	filename = getResourceFilename( filename, gBehaviorEngine.m_ExeFile.getDataDirectory(), false, false);

	if( filename != "" )
	{
	    mpMenuDialog->addControl(new GsButton( "Previews!",
					new StartInfoSceneEvent( new CPreviews() ) ) );
		    
	}	


}

*/
