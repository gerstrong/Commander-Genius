#include "GsVirtualinput.h"

#include <base/video/CVideoDriver.h>
#include <SDL_image.h>
#include <cstdio>



GsVirtualInput::GsVirtualInput() :
mEnabled(true),
mTranslucency(0.5f)
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

    mDPadTexture.setAlpha(uint8_t(255.0f*mTranslucency));

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
    //const int buttonSize = 50;

    GsRect<Uint16> clickGameArea = gVideoDriver.mpVideoEngine->getAspectCorrRect();

    const float dpadSize = 0.2;

    const Uint16 dpadWidth = clickGameArea.w * dpadSize;
    const Uint16 dpadHeight = clickGameArea.h * dpadSize;

    const GsRect<Uint16> dpadRect(0, blit.height()-dpadHeight, dpadWidth, dpadHeight);

    gVideoDriver.addTextureRefToRender(mDPadTexture, dpadRect);
}


void GsVirtualInput::mouseState(const Vector2D<float> &Pos, const bool down)
{
    /// Dpad presses
    const float dpadSize = 0.2f;

    // Size of the buttons on the dpad
    const float dpadSizePiece = 0.3f*dpadSize;

    const float yBottom = 1.0f;
    const float yTop = yBottom-dpadSize;
    if(Pos.y >= yTop && Pos.y < 1.0f &&
       Pos.x >= 0.0f && Pos.x < dpadSize )
    {
        SDL_Event ev;
        ev.type = (down ? SDL_KEYDOWN : SDL_KEYUP);

        // Y-Direction
        // Up presses
        if(Pos.y<yTop+dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&ev);
        }
        // Down presses
        else if(Pos.y>=yBottom-dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&ev);
        }

        // X-Direction
        // Left presses
        if(Pos.x<dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_LEFT;
            SDL_PushEvent(&ev);
        }
        // Right presses
        else if(Pos.x>=dpadSize-dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_RIGHT;
            SDL_PushEvent(&ev);
        }
    }
}

void GsVirtualInput::mouseDown(const Vector2D<float> &Pos)
{
    mouseState(Pos, true);
}

void GsVirtualInput::mouseUp(const Vector2D<float> &Pos)
{
    mouseState(Pos, false);
}
