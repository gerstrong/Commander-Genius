/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"

#include "../CStory.h"
#include "../CCredits.h"
#include "../COrderingInfo.h"
#include "../CAbout.h"
#include "../CHelp.h"
#include "../CPreviews.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/KeenFiles.h"
#include "widgets/Button.h"
#include <base/utils/StringUtils.h>

namespace vorticon
{

CHelpMenu::CHelpMenu() :
VorticonMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries

    mpMenuDialog->addControl(new Button( "The Game",
						new StartInfoSceneEvent( new CHelp("Game") ) ) );

    mpMenuDialog->addControl(new Button( "The Story",
						new StartInfoSceneEvent( new CStory ) ) );

    mpMenuDialog->addControl(new Button( "Ordering Info",
						new StartInfoSceneEvent( new COrderingInfo ) ) );

    mpMenuDialog->addControl(new Button( "About ID",
						new StartInfoSceneEvent( new CAbout("ID") ) ) );

    mpMenuDialog->addControl(new Button( "About CG",
						new StartInfoSceneEvent( new CAbout("CG") ) ) );

    mpMenuDialog->addControl(new Button( "Credits",
						new StartInfoSceneEvent( new CCredits() ) ) );
	
	std::string filename = "previews.ck";
	filename += itoa(g_pBehaviorEngine->getEpisode());	
    filename = getResourceFilename( filename, gKeenFiles.gameDir, false, false);

	if( filename != "" )
	{
        mpMenuDialog->addControl(new Button( "Previews!",
					new StartInfoSceneEvent( new CPreviews() ) ) );
		    
	}	


}


}
