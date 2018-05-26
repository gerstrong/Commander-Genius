#include "vgamepadsimple.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <base/GsLogging.h>
#include <fileio/ResourceMgmt.h>


bool TouchButton::loadPicture(const std::string &picFile)
{
    const std::string buttonFname = getResourceFilename(picFile, "", true, true);
    if(buttonFname == "") return false;

    mTexture.load(GetFullFileName(buttonFname), gVideoDriver.Renderer());
    if( !mTexture )
    {
        gLogging.ftextOut("Failed to load the texture: %s!\n", picFile.c_str());
        return false;
    }

    mTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    return true;
}

VirtualKeenControl::~VirtualKeenControl()
{}

bool VirtualKeenControl::init()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    /*GsWeakSurface blit(gVideoDriver.getBlitSurface());
    SDL_PixelFormat *format = blit.getSDLSurface()->format;

    const int buttonSize = 50;*/


    // Create the overlay surface and fill it with alpha 0
    /*
    mOverlay.create(0, blit.width(), blit.height(), 32, 0, 0, 0, 0);

    mOverlay.setBlendMode(SDL_BLENDMODE_BLEND);
    mOverlay.setAlpha(uint8_t(255.0f*mTranslucency));

    mOverlay.fill(SDL_MapRGBA(format, 0, 0, 0, 0 ));

    /// Draw a D-Pad

    // Left arrow
    const GsRect<Uint16> upRect(0, blit.height()-buttonSize, buttonSize, buttonSize);
    mOverlay.fill(upRect, SDL_MapRGBA(format, 128, 0, 0, 128 ));
    */


    /// Load The buttons images
    {
        // Start with the Background for the gamepad
        if(!mPadBackground.loadPicture("bckgrnd.png")) return false;

        // Directional pad
        if(!mDPad.loadPicture("dpad.png")) return false;

        if(!mConfirmButton.loadPicture("confirm.png")) return false;
        mConfirmButton.invisible = false;

        if(!mStartButton.loadPicture("start.png")) return false;
        mStartButton.invisible = true;

        if(!mJumpButton.loadPicture("1.png")) return false;
        mJumpButton.invisible = true;

        if(!mPogoButton.loadPicture("2.png")) return false;
        mPogoButton.invisible = true;

        if(!mShootButton.loadPicture("3.png")) return false;
        mShootButton.invisible = true;

        if(!mStatusButton.loadPicture("4.png")) return false;
        mStatusButton.invisible = true;
    }

#endif

    return true;
}

bool VirtualKeenControl::ponder()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    const float bottom = 0.975f;
    const float top = 0.025f;
    const float left = 0.025f;
    const float right = 0.975f;

    if(!mPadBackground.invisible)
    {
        const GsRect<float> dpadRect(0.0f, 0.75f,
                                     1.0f, 0.25f);

        mPadBackground.setRect(dpadRect);

        mPadBackground.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mDPad.invisible)
    {
        const float dpadSize = 0.2f;

        const GsRect<float> dpadRect(left, bottom-dpadSize,
                                      dpadSize, dpadSize);

        mDPad.setRect(dpadRect);

        mDPad.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mConfirmButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> confirmRect(right-2.0f*buttonSize,
                                        bottom-2.0f*buttonSize,
                                         buttonSize, buttonSize);

        mConfirmButton.setRect(confirmRect);

        mConfirmButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mStatusButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> statusRect(right-4.0f*buttonSize,
                                       bottom-2.0f*buttonSize,
                                       buttonSize, buttonSize);

        mStatusButton.setRect(statusRect);

        mStatusButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mStartButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> startRect(right-3.0f*buttonSize,
                                      bottom-2.0f*buttonSize,
                                      buttonSize, buttonSize);

        mStartButton.setRect(startRect);

        mStartButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mJumpButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> jumpRect(right-2.0f*buttonSize,
                                     bottom-1.0f*buttonSize,
                                      buttonSize, buttonSize);

        mJumpButton.setRect(jumpRect);

        mJumpButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mPogoButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> pogoRect(right-1.0f*buttonSize,
                                     bottom-1.0f*buttonSize,
                                      buttonSize, buttonSize);

        mPogoButton.setRect(pogoRect);

        mPogoButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mShootButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> shootRect(right-2.0f*buttonSize,
                                      bottom-2.0f*buttonSize,
                                      buttonSize, buttonSize);

        mShootButton.setRect(shootRect);

        mShootButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

#endif

    return true;
}

void VirtualKeenControl::hideAllButtons()
{
    mPadBackground.invisible = true;
    mDPad.invisible = true;
    mConfirmButton.invisible = true;
    mStartButton.invisible = true;

    mJumpButton.invisible = true;
    mShootButton.invisible = true;
    mPogoButton.invisible = true;
    mStatusButton.invisible = true;
}


void VirtualKeenControl::render(GsWeakSurface &sfc)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    //GsRect<Uint16> clickGameArea = gVideoDriver.mpVideoEngine->getActiveAreaRect();

    auto addTexture = [](TouchButton &button) -> void
    {
        if(!button.invisible)
        {
            gVideoDriver.addTextureRefToRender(button.mTexture, button.Rect());
        }
    };

    addTexture(mPadBackground);

    addTexture(mDPad);

    addTexture(mConfirmButton);
    addTexture(mStartButton);

    addTexture(mStatusButton);
    addTexture(mShootButton);
    addTexture(mJumpButton);
    addTexture(mPogoButton);

#endif
}


bool VirtualKeenControl::mouseState(const Vector2D<float> &Pos, const bool down)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)    

    if( !mPadBackground.isInside(Pos.x, Pos.y) )
        return false;

    auto bindButtonCommand = [&](const TouchButton &button,
            const InputCommand &cmd)
    {
        if(button.invisible)
            return;

        if( button.Rect().HasPoint(Pos) )
        {
            gInput.setCommand(0, cmd, down);
        }
    };        

    SDL_Event ev;
    ev.type = (down ? SDL_KEYDOWN : SDL_KEYUP);

    if( !mDPad.invisible &&
        mDPad.isInside(Pos.x, Pos.y) )
    {
        // Size of the buttons on the dpad
        const float dpadSizePiece = 0.3f*mDPad.w;

        // Y-Direction
        // Up presses
        if(Pos.y<mDPad.y+dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&ev);
        }
        // Down presses
        else if(Pos.y>=1.0f-dpadSizePiece)
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
        else if(Pos.x>=mDPad.w-dpadSizePiece)
        {
            ev.key.keysym.sym = SDLK_RIGHT;
            SDL_PushEvent(&ev);
        }
    }

    bindButtonCommand(mConfirmButton, IC_JUMP);
    bindButtonCommand(mStartButton, IC_JUMP);
    bindButtonCommand(mStatusButton, IC_STATUS);
    bindButtonCommand(mShootButton, IC_FIRE);
    bindButtonCommand(mJumpButton, IC_JUMP);
    bindButtonCommand(mPogoButton, IC_POGO);

#endif

    return true;

}
