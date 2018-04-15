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

    /// Load The buttons images
    {
        // Directional pad
        if(!mDPad.loadPicture("dpad.png")) return false;

        if(!mConfirmButton.loadPicture("confirm.png")) return false;

        mConfirmButton.invisible = false;

        if(!mStartButton.loadPicture("start.png")) return false;

        if(!mJumpButton.loadPicture("1.png")) return false;

        if(!mPogoButton.loadPicture("2.png")) return false;

        if(!mStatusButton.loadPicture("3.png")) return false;

        if(!mStatusButton.loadPicture("4.png")) return false;
    }

#endif

    return true;
}

bool VirtualKeenControl::ponder()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    if(!mDPad.invisible)
    {
        const float dpadSize = 0.2f;

        const GsRect<float> dpadRect(0, 1.0f-dpadSize,
                                      dpadSize, dpadSize);

        mDPad.setRect(dpadRect);

        mDPad.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mConfirmButton.invisible)
    {
        const float buttonSize = 0.1f;

        const GsRect<float> confirmRect(1.0f-2.0f*buttonSize,
                                         1.0f-2.0f*buttonSize,
                                         buttonSize, buttonSize);

        mConfirmButton.setRect(confirmRect);

        mConfirmButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

#endif

    return true;
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

    addTexture(mDPad);


    //addTexture(mStatusButton);

    addTexture(mConfirmButton);

    /*if(mButtonMode == BUTTON_MODE::WMAP &&
            !mHideStartButton)
    {
        addTexture(mStartButton);
        addTexture(mStatusButton);
    }

    if(mButtonMode == BUTTON_MODE::ACTION)
    {
        addTexture(mShootButton);
        addTexture(mJumpButton);
        addTexture(mPogoButton);
    }
*/

/*
    // On map, show the start button if keen approaches a level
    if(mButtonMode == BUTTON_MODE::WMAP && !mHideStartButton)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        const GsRect<Uint16> confirmRect(clickGameArea.w-2*width, clickGameArea.h-2*height, width, height);
        mStartButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mStartButtonTexture, confirmRect);

        const GsRect<Uint16> statusButtonRect(clickGameArea.w*0.5f, clickGameArea.h-height, width, height);
        mStatusButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mStatusButtonTexture, statusButtonRect);
    }

    if(mButtonMode == BUTTON_MODE::ACTION)
    {
        const float buttonSize = 0.1f;

        const Uint16 width = clickGameArea.w * buttonSize;
        const Uint16 height = clickGameArea.h * buttonSize;

        // Main controls
        const GsRect<Uint16> shootButtonRect(clickGameArea.w-2*width, clickGameArea.h-2*height, width, height);
        mShootButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mShootButtonTexture, shootButtonRect);

        const GsRect<Uint16> jumpButtonRect(clickGameArea.w-2*width, clickGameArea.h-height, width, height);
        mJumpButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mJumpButtonTexture, jumpButtonRect);

        const GsRect<Uint16> pogoButtonRect(clickGameArea.w-width, clickGameArea.h-height, width, height);
        mPogoButtonTexture.setAlpha(uint8_t(255.0f*mTranslucency));
        gVideoDriver.addTextureRefToRender(mPogoButtonTexture, pogoButtonRect);
    }*/
#endif
}


void VirtualKeenControl::mouseState(const Vector2D<float> &Pos, const bool down)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    auto bindButtonCommand = [&](const GsRect<float> &buttonRect,
            const InputCommand &cmd)
    {
        if( buttonRect.HasPoint(Pos) )
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

    bindButtonCommand(mConfirmButton.Rect(), IC_JUMP);
    bindButtonCommand(mStatusButton.Rect(), IC_STATUS);
    bindButtonCommand(mShootButton.Rect(), IC_FIRE);
    bindButtonCommand(mJumpButton.Rect(), IC_JUMP);
    bindButtonCommand(mPogoButton.Rect(), IC_POGO);

#endif

}
