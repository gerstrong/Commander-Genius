#include "vgamepadsimple.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <fileio/ResourceMgmt.h>

bool VirtualKeenControl::init()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    SDL_PixelFormat *format = blit.getSDLSurface()->format;

    const int buttonSize = 50;


    // Create the overlay surface and fill it with alpha 0
    mOverlay.create(0, blit.width(), blit.height(), 32, 0, 0, 0, 0);

    mOverlay.setBlendMode(SDL_BLENDMODE_BLEND);
    mOverlay.setAlpha(uint8_t(255.0f*mTranslucency));

    mOverlay.fill(SDL_MapRGBA(format, 0, 0, 0, 0 ));

    /// Draw a D-Pad

    // Left arrow
    const GsRect<Uint16> upRect(0, blit.height()-buttonSize, buttonSize, buttonSize);
    mOverlay.fill(upRect, SDL_MapRGBA(format, 128, 0, 0, 128 ));

    /// Load Textures
    {
        // Dpad
        const std::string dpadFname = getResourceFilename("dpad.png", "", true, true);
        if(dpadFname == "") return false;

        mDPadTexture.load(GetFullFileName(dpadFname), gVideoDriver.getRendererRef());
        if( !mDPadTexture )
        {
            printf( "Failed to load texture image for dpad!\n" );
            return false;
        }

        mDPadTexture.setBlendMode(SDL_BLENDMODE_BLEND);

        // Confirm Button
        const std::string confirmFname = getResourceFilename("confirm.png", "", true, true);
        if(confirmFname == "") return false;

        mConfirmButtonTexture.load(GetFullFileName(confirmFname), gVideoDriver.getRendererRef());
        if( !mConfirmButtonTexture )
        {
            printf( "Failed to load texture image confirm!\n" );
            return false;
        }

        mConfirmButtonTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    }

#endif

    return true;
}

void VirtualKeenControl::render(GsWeakSurface &sfc)
{
    GsRect<Uint16> dispRect = gVideoDriver.getVidConfig().m_DisplayRect;

    GsRect<Uint16> clickGameArea = gVideoDriver.mpVideoEngine->getAspectCorrRect();

    /// DPad
    if(mShowDPad)
    {
        const float dpadSize = 0.2f;

        const Uint16 dpadWidth = clickGameArea.w * dpadSize;
        const Uint16 dpadHeight = clickGameArea.h * dpadSize;

        const GsRect<Uint16> dpadRect(0, dispRect.h-dpadHeight, dpadWidth, dpadHeight);
        mDPadTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mDPadTexture, dpadRect);
    }

    /// Confirm Button
    if(mButtonMode == OK)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        const GsRect<Uint16> confirmRect(dispRect.w-2*width, dispRect.h-2*height, width, height);
        mConfirmButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mConfirmButtonTexture, confirmRect);
    }

    if(mButtonMode == WMAP && !mHideEnterButton)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        const GsRect<Uint16> confirmRect(dispRect.w-2*width, dispRect.h-2*height, width, height);
        mConfirmButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mConfirmButtonTexture, confirmRect);
    }
}


void VirtualKeenControl::mouseState(const Vector2D<float> &Pos, const bool down)
{
    /// Menu Control process of one mouse state
    const float dpadSize = 0.2f;

    // Size of the buttons on the dpad
    const float dpadSizePiece = 0.3f*dpadSize;

    const float yBottom = 1.0f;
    const float yTop = yBottom-dpadSize;


    if(Pos.y >= yTop && Pos.y < 1.0f)
    {
        SDL_Event ev;
        ev.type = (down ? SDL_KEYDOWN : SDL_KEYUP);

        /// Dpad presses
        if(Pos.x >= 0.0f && Pos.x < dpadSize)
        {
            if(mShowDPad)
            {
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
        else
        {
            /// Then, if any other button was pressed...
            const float buttonSize = 0.1f;

            if(mButtonMode == OK)
            {
                // Was the Ok button pressed?
                GsRect<float> confirmRect(1.0f-2.0f*buttonSize, 1.0f-2.0f*buttonSize, buttonSize, buttonSize);

                if( confirmRect.HasPoint(Pos) )
                {
                    ev.key.keysym.sym = SDLK_RETURN;
                    SDL_PushEvent(&ev);
                }
            }

            if(mButtonMode == WMAP && !mHideEnterButton)
            {
                // Was the Ok button pressed?
                GsRect<float> confirmRect(1.0f-2.0f*buttonSize, 1.0f-2.0f*buttonSize, buttonSize, buttonSize);

                if( confirmRect.HasPoint(Pos) )
                {
                    gInput.setCommand(0, IC_JUMP);
                }
            }
        }
    }


}
