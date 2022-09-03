/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"


//#include "engine/keen/vorticon/CStory.h"
//#include "engine/keen/vorticon/CCredits.h"
//#include "engine/keen/vorticon/COrderingInfo.h"
//#include "engine/keen/vorticon/CAbout.h"
//#include "engine/keen/vorticon/CHelp.h"
//#include "engine/keen/vorticon/CPreviews.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/KeenFiles.h"
#include <base/utils/StringUtils.h>

//#include "dialog/CDlgFrame.h"
#include "engine/core/CBehaviorEngine.h"


CHelpMenu::CHelpMenu(const Style style) :
GameMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.5f), style )
{

	// Create the Menu Dialog and entries

    mpMenuDialog->add(
                new GameButton( "The Game",
                            new StartHelpEv( StartHelpEv::Variant::THEGAME ),
                                style) );

    mpMenuDialog->add(new GameButton( "The Story",
                            new StartHelpEv( StartHelpEv::Variant::STORY ),
                                style ) );

    mpMenuDialog->add(
                new GameButton( "Ordering Info",
                            new StartHelpEv( StartHelpEv::Variant::ORDERING ),
                                style ) );

    mpMenuDialog->add(new GameButton( "About ID",
                            new StartHelpEv( StartHelpEv::Variant::ABOUT_ID ),
                                style ) );

    mpMenuDialog->add(new GameButton( "About CG",
                            new StartHelpEv( StartHelpEv::Variant::ABOUT_CG ),
                                style ) );

    mpMenuDialog->add(new GameButton( "Credits",
                            new StartHelpEv( StartHelpEv::Variant::CREDITS ),
                                style ) );


	std::string filename = "previews.ck";
	filename += itoa(gBehaviorEngine.getEpisode());	
    filename = getResourceFilename( filename, gKeenFiles.gameDir, false, false);

	if( filename != "" )
	{
        mpMenuDialog->add(new GameButton( "Previews!",
                    new StartHelpEv( StartHelpEv::Variant::PREVIEWS ), style ) );
		    
	}	

    mpMenuDialog->fit();
    select(1);
}



