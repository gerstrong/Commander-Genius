/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/CGameLauncher.h"

#include "fileio/KeenFiles.h"
#include "base/CInput.h"
//#include "base/video/GsEffectController.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>
#include <widgets/GsMenuController.h>


CPassive::CPassive()
{
    auto &keenFiles = gKeenFiles;
    CExeFile &ExeFile = keenFiles.exeFile;
    m_Episode = ExeFile.getEpisode();
    m_DataDirectory = keenFiles.gameDir;
}


CPassive::~CPassive()
{
#ifdef USE_VIRTUALPAD
    gInput.mpVirtPad = nullptr;
#endif
}

void CPassive::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( std::dynamic_pointer_cast<const GMSwitchToGameLauncher>(evPtr) )
    {
        // The last menu has been removed. Restore back the game status
        gBehaviorEngine.setPause(false);
        gMenuController.clearMenuStack();
        mBackToGameLauncher = true;
    }
}


void CPassive::createOutlinedText(const int splash_x, const int splash_y, const std::string &text)
{
    auto &Font = gGraphics.getFontLegacy(1);
    SDL_Surface *sfc = mTextSfc.getSDLSurface();
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
