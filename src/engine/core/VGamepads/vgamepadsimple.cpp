#include "vgamepadsimple.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <base/GsLogging.h>
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
        auto loadButtonTexture = [&](const std::string &fname, GsTexture &texture) -> bool
        {
            const std::string buttonFname = getResourceFilename(fname, "", true, true);
            if(buttonFname == "") return false;

            texture.load(GetFullFileName(buttonFname), gVideoDriver.getRendererRef());
            if( !texture )
            {
                gLogging.ftextOut("Failed to load the texture: %s!\n", fname.c_str());
                return false;
            }

            texture.setBlendMode(SDL_BLENDMODE_BLEND);
            return true;
        };

        // Dpad
        if( !loadButtonTexture("dpad.png", mDPadTexture) )
            return false;

        // Confirm Button
        if( !loadButtonTexture("confirm.png", mConfirmButtonTexture) )
            return false;

        // Start Button
        if( !loadButtonTexture("start.png", mStartButtonTexture) )
            return false;

        // Jump Button
        if( !loadButtonTexture("1.png", mJumpButtonTexture) )
            return false;

        // Pogo Button
        if( !loadButtonTexture("2.png", mPogoButtonTexture) )
            return false;

        // Shoot Button
        if( !loadButtonTexture("3.png", mShootButtonTexture) )
            return false;

        // Status Button
        if( !loadButtonTexture("4.png", mStatusButtonTexture) )
            return false;


    }

#endif

    return true;
}

void VirtualKeenControl::render(GsWeakSurface &sfc)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
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

    // On map, show the start button if keen approaches a level
    if(mButtonMode == WMAP && !mHideEnterButton)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        const GsRect<Uint16> confirmRect(dispRect.w-2*width, dispRect.h-2*height, width, height);
        mStartButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mStartButtonTexture, confirmRect);
    }

    if(mButtonMode == ACTION)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        // Main controls
        const GsRect<Uint16> shootButtonRect(dispRect.w-2*width, dispRect.h-2*height, width, height);
        mShootButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mShootButtonTexture, shootButtonRect);

        const GsRect<Uint16> jumpButtonRect(dispRect.w-2*width, dispRect.h-height, width, height);
        mJumpButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mJumpButtonTexture, jumpButtonRect);

        const GsRect<Uint16> pogoButtonRect(dispRect.w-width, dispRect.h-height, width, height);
        mPogoButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mPogoButtonTexture, pogoButtonRect);

        const GsRect<Uint16> statusButtonRect(dispRect.w*0.5f, dispRect.h-height, width, height);
        mStatusButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mStatusButtonTexture, statusButtonRect);
    }
#endif
}


void VirtualKeenControl::mouseState(const Vector2D<float> &Pos, const bool down)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    /// Menu Control process of one mouse state
    const float dpadSize = 0.2f;

    // Size of the buttons on the dpad
    const float dpadSizePiece = 0.3f*dpadSize;

    const float yBottom = 1.0f;
    const float yTop = yBottom-dpadSize;

    auto verifyButtonMatch = [&](const GsRect<float> &buttonRect, InputCommands cmd)
    {
        if( buttonRect.HasPoint(Pos) )
        {
            gInput.setCommand(0, cmd, down);
        }
    };


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

            // On map if we can enter a level, let's enter!
            if(mButtonMode == WMAP && !mHideEnterButton)
            {
                // Was the Ok button pressed?
                GsRect<float> confirmRect(1.0f-2.0f*buttonSize, 1.0f-2.0f*buttonSize, buttonSize, buttonSize);

                verifyButtonMatch(confirmRect, IC_JUMP);
            }


            if(mButtonMode == ACTION)
            {
                // Was the Shoot button pressed?
                const GsRect<float> shootButtonRect(1.0f-2.0f*buttonSize, 1.0f-2.0f*buttonSize, buttonSize, buttonSize);
                verifyButtonMatch(shootButtonRect, IC_FIRE);

                // Was the Jump button pressed?
                const GsRect<float> jumpButtonRect(1.0f-2.0f*buttonSize, 1.0f-buttonSize, buttonSize, buttonSize);
                verifyButtonMatch(jumpButtonRect, IC_JUMP);

                // Was the Pogo button pressed?
                const GsRect<float> pogoButtonRect(1.0f-buttonSize, 1.0f-buttonSize, buttonSize, buttonSize);
                verifyButtonMatch(pogoButtonRect, IC_POGO);

                // Was the Run button pressed?
                const GsRect<float> statusButtonRect(0.5f, 1.0f-buttonSize, buttonSize, buttonSize);
                verifyButtonMatch(statusButtonRect, IC_RUN);
            }
        }
    }
#endif

}
