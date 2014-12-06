#include "GsVirtualinput.h"

#include <base/video/CVideoDriver.h>
#include <SDL_image.h>
#include <cstdio>



GsVirtualInput::GsVirtualInput() :
mEnabled(true)
{}

bool GsVirtualInput::init()
{
    #if SDL_VERSION_ATLEAST(2, 0, 0)
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    SDL_PixelFormat *format = blit.getSDLSurface()->format;

    const int buttonSize = 50;


    // Create the overlay surface and fill it with alpha 0
    mOverlay.create(0, blit.width(), blit.height(), 32, 0, 0, 0, 0);

//#if SDL_VERSION_ATLEAST(2, 0, 0)
    mOverlay.setBlendMode(SDL_BLENDMODE_BLEND);
//#endif

    mOverlay.fill(SDL_MapRGBA(format, 0, 0, 0, 0 ));

    /// Draw a D-Pad

    // Left arrow
    const GsRect<Uint16> upRect(0, blit.height()-buttonSize, buttonSize, buttonSize);
    mOverlay.fill(upRect, SDL_MapRGBA(format, 128, 0, 0, 128 ));

    mDPadTexture.load("dpad.png", gVideoDriver.getRendererRef());
    if( !mDPadTexture )
    {
        printf( "Failed to load texture image!\n" );
        return false;
    }

    // TODO: Up arrow

    // TODO: Right arrow

    // TODO: Down arrow

    /// TODO: Draw action and menu Buttons    
    #endif

    return true;
}

void GsVirtualInput::render(GsWeakSurface &sfc)
{
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    const int buttonSize = 50;

    const GsRect<Uint16> dpadRect(0, blit.height()-buttonSize, buttonSize, buttonSize);

    //gVideoDriver.addTextureRefToRender(*gTexture, dpadRect);
}

