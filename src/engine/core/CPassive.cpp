/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"
#include "engine/core/CBehaviorEngine.h"
#include <base/video/CVideoDriver.h>
#include <widgets/GsMenuController.h>

#include "fileio/KeenFiles.h"


CPassive::CPassive()
{
    auto &keenFiles = gKeenFiles;
    CExeFile &ExeFile = keenFiles.exeFile;
    mEndEpisode = false;
	m_mode = INTRO;
	m_Episode = ExeFile.getEpisode();
    m_DataDirectory = gKeenFiles.gameDir;

}


CPassive::~CPassive()
{

    gInput.mpVirtPad = nullptr;

}

void CPassive::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const EventEndGamePlay*>(evPtr) )
    {
        // The last menu has been removed. Restore back the game status
        gpBehaviorEngine->setPause(false);
        gMenuController.clearMenuStack();
        mEndEpisode = true;
    }
}


void CPassive::createOutlinedText(const int splash_x, const int splash_y, const std::string &text)
{
	GsFont &Font = gGraphics.getFont(1);
	SDL_Surface *sfc = mpTextSfc.get();
	SDL_PixelFormat *format = sfc->format;

	const Uint32 oldcolor = Font.getFGColor();
	Uint32 forecolor = SDL_MapRGB( format, 255, 20, 0);
	Uint32 backcolor = SDL_MapRGB( format, 0, 0, 0);

	Font.setupColor( backcolor );

	Font.drawFont( sfc, text, splash_x-1, splash_y, false );
	Font.drawFont( sfc, text, splash_x, splash_y-1, false );
	Font.drawFont( sfc, text, splash_x, splash_y+1, false );
	Font.drawFont( sfc, text, splash_x+1, splash_y, false );

	Font.setupColor( forecolor );

	Font.drawFont( sfc, text, splash_x, splash_y, false );

	Font.setupColor( oldcolor );
}
