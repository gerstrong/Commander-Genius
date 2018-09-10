#include "vgamepadsimple.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <base/GsLogging.h>
#include <graphics/GsTexture.h>
#include <fileio/ResourceMgmt.h>

#include "dpad.h"
#include "button1.h"
#include "button2.h"
#include "button3.h"
#include "button4.h"
#include "buttonBg.h"
#include "buttonConfirm.h"
#include "buttonStart.h"
#include "buttonClose.h"


bool TouchButton::loadEmdbeddedPicture(const unsigned char *data,
                                       const unsigned int size)
{        
    bool ok = mTexture.loadFromMem(data, size, gVideoDriver.Renderer(), false);

    ok &= mTextureDark.loadFromMem(data, size, gVideoDriver.Renderer(), true);

    return ok;
}

void TouchButton::clearFingers()
{
    mFingerSet.clear();
}

void TouchButton::insertFingerId(const SDL_FingerID fid)
{
    mFingerSet.insert(fid);
}

void TouchButton::removeFingerId(const SDL_FingerID fid)
{
    auto it = mFingerSet.find(fid);
    if(it != mFingerSet.end())
    {
        mFingerSet.erase(fid);
    }
}

void TouchButton::render(const bool dark)
{
    if(dark)
    {
        gVideoDriver.addTextureRefToRender(mTextureDark, Rect());
    }
    else
    {
        gVideoDriver.addTextureRefToRender(mTexture, Rect());
    }

}


// --------------- End of Touch Button -------------------- //

VirtualKeenControl::~VirtualKeenControl()
{}

bool VirtualKeenControl::init()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    /// Load The buttons images
    {
        // Close for configuratin of Virtual pad
        if(!mCloseConfigButton.loadEmdbeddedPicture(gButtonClosePng, sizeof(gButtonClosePng))) return false;

        GsTexture &tex = mPadConfigBackground.mTexture;
        tex.fillRGB( gVideoDriver.Renderer(), 0x0, 0x0, 0x0);
        tex.setAlpha(128);
        mPadConfigBackground.invisible = true;

        // Directional pad
        if(!mDPad.loadEmdbeddedPicture(gDPadPng, sizeof(gDPadPng))) return false;

        if(!mConfirmButton.loadEmdbeddedPicture(gButtonConfirmPng, sizeof(gButtonConfirmPng))) return false;
        mConfirmButton.invisible = true;

        if(!mStartButton.loadEmdbeddedPicture(gButtonStartPng, sizeof(gButtonStartPng))) return false;
        mStartButton.invisible = true;

        if(!mJumpButton.loadEmdbeddedPicture(gButton1Png, sizeof(gButton1Png))) return false;
        mJumpButton.invisible = true;

        if(!mPogoButton.loadEmdbeddedPicture(gButton2Png, sizeof(gButton2Png))) return false;
        mPogoButton.invisible = true;

        if(!mShootButton.loadEmdbeddedPicture(gButton3Png, sizeof(gButton3Png))) return false;
        mShootButton.invisible = true;

        if(!mStatusButton.loadEmdbeddedPicture(gButton4Png, sizeof(gButton4Png))) return false;
        mStatusButton.invisible = true;
    }

#endif

    return true;
}

bool VirtualKeenControl::ponder()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    const float bottom = 0.975f;
    //const float top = 0.025f;
    const float left = 0.025f;
    const float right = 0.975f;

    if(!mPadConfigBackground.invisible)
    {
        const GsRect<float> dpadRect(0.0f, 0.0f,
                                     1.0f, 1.0f);

        mPadConfigBackground.setRect(dpadRect);
    }


    if(!mCloseConfigButton.invisible)
    {
        const GsRect<float> dpadRect(0.45f, 0.45f,
                                     0.1f, 0.1f);

        mCloseConfigButton.setRect(dpadRect);
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


void VirtualKeenControl::processConfig()
{
    // Opening section
    if(!mConfigOpened)
    {
        mConfigOpened = true;
        mPadConfigBackground.invisible = false;
        return;
    }

}

void VirtualKeenControl::processConfigTouchbuttons(const Vector2D<float> &Pos,
                                                   const bool down)
{
    // If there is a close button to hit, were are in configuration mode
    // and need to close that.
    if(!mCloseConfigButton.invisible)
    {
        // Was it hit?
        if(mCloseConfigButton.Rect().HasPoint(Pos) && down)
        {
            mConfigOpened = false;
            mPadConfigBackground.invisible = true;
            gInput.setInVPadConfigState(false);

            mDPad.invisible = true;
            mConfirmButton.invisible = true;
            mStartButton.invisible = true;
            mJumpButton.invisible = true;
            mShootButton.invisible = true;
            mPogoButton.invisible = true;
            mStatusButton.invisible = true;

            return;
        }
    }


    auto bindButtonCfg = [&](TouchButton &button) -> bool
    {
        if(button.invisible)
            return false;

        if( button.Rect().HasPoint(Pos) )
        {
            if(mpCurrentButton4Config == nullptr)
            {
                mpCurrentButton4Config = &button;
            }

            return true;
        }

        return false;
    };

    mpCurrentButton4Config = nullptr;

    bindButtonCfg(mDPad);

    bindButtonCfg(mConfirmButton);
    bindButtonCfg(mStartButton);

    bindButtonCfg(mStatusButton);
    bindButtonCfg(mShootButton);
    bindButtonCfg(mJumpButton);
    bindButtonCfg(mPogoButton);
}


void VirtualKeenControl::renderConfig()
{
    auto addTexture = [](TouchButton &button) -> void
    {
        button.invisible = false;

        if(!button.invisible &&
           button.w > 0.0f &&
           button.h > 0.0f)
        {
            button.render(false);
        }
    };

    mPadConfigBackground.mTexture.setAlpha(127);
    addTexture(mPadConfigBackground);

    addTexture(mCloseConfigButton);

    addTexture(mDPad);

    addTexture(mConfirmButton);
    addTexture(mStartButton);

    addTexture(mStatusButton);
    addTexture(mShootButton);
    addTexture(mJumpButton);
    addTexture(mPogoButton);

    // If there was a button for config selected
    if(mpCurrentButton4Config)
    {
        addTexture(*mpCurrentButton4Config);
    }
}


void VirtualKeenControl::hideEverything()
{
    mPadConfigBackground.invisible = true;
    mDPad.invisible = true;
    mConfirmButton.invisible = true;
    mStartButton.invisible = true;

    mJumpButton.invisible = true;
    mShootButton.invisible = true;
    mPogoButton.invisible = true;
    mStatusButton.invisible = true;
}

void VirtualKeenControl::hideAllButtons()
{
    mPadConfigBackground.invisible = false;
    mConfirmButton.invisible = true;
    mStartButton.invisible = true;

    mJumpButton.invisible = true;
    mShootButton.invisible = true;
    mPogoButton.invisible = true;
    mStatusButton.invisible = true;
}



void VirtualKeenControl::render(GsWeakSurface &)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    auto addTexture = [](TouchButton &button) -> void
    {
        if(!button.invisible &&
            button.w > 0.0f &&
            button.h > 0.0f)
        {
            button.render(button.hasFingersPressing());
        }
    };

    addTexture(mDPad);

    addTexture(mConfirmButton);
    addTexture(mStartButton);

    addTexture(mStatusButton);
    addTexture(mShootButton);
    addTexture(mJumpButton);
    addTexture(mPogoButton);

#endif
}


void VirtualKeenControl::flush()
{
    mConfirmButton.clearFingers();
    mStartButton.clearFingers();
    mStatusButton.clearFingers();
    mShootButton.clearFingers();
    mJumpButton.clearFingers();
    mPogoButton.clearFingers();
    mDPad.clearFingers();
}


bool VirtualKeenControl::mouseFingerState(const Vector2D<float> &Pos,
                                          const SDL_TouchFingerEvent &touchFingerEvent,
                                          const bool down)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    processConfigTouchbuttons(Pos, down);

    bool ok = false;

    auto bindButtonCommand = [&](TouchButton &button,
                                 const InputCommand &cmd) -> bool
    {
        if(button.invisible)
            return false;

        if( button.Rect().HasPoint(Pos) )
        {
            gInput.setCommand(0, cmd, down);

            if(down)
            {
                button.insertFingerId(touchFingerEvent.fingerId);
            }
            else
            {
                button.removeFingerId(touchFingerEvent.fingerId);
            }

            return true;
        }
        else
        {
            auto it = button.mFingerSet.find(touchFingerEvent.fingerId);

            if( it != button.mFingerSet.end() )
            {
                gInput.setCommand(0, cmd, false);
                button.mFingerSet.erase(it);
            }

            return false;
        }
    };

    SDL_Event ev;
    ev.type = (down ? SDL_KEYDOWN : SDL_KEYUP);

    if( !mDPad.invisible &&
        mDPad.isInside(Pos.x, Pos.y) )
    {
        // Size of the buttons on the dpad
        const float dpadSizePieceW = 0.4f*mDPad.w;
        const float dpadSizePieceH = 0.4f*mDPad.h;

        // Y-Direction
        // Up presses
        if(Pos.y<mDPad.y+dpadSizePieceH)
        {
            ev.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&ev);

            if(down)
            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.sym = SDLK_DOWN;
                SDL_PushEvent(&evUp);                
            }
        }
        // Down presses
        else if(Pos.y>=mDPad.y+mDPad.h-dpadSizePieceH)
        {
            ev.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&ev);

            if(down)
            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.sym = SDLK_UP;
                SDL_PushEvent(&evUp);
            }
        }

        // X-Direction
        // Left presses
        if(Pos.x<mDPad.x+dpadSizePieceW)
        {
            ev.key.keysym.sym = SDLK_LEFT;
            SDL_PushEvent(&ev);

            if(down)
            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.sym = SDLK_RIGHT;
                SDL_PushEvent(&evUp);
            }

        }
        // Right presses
        else if(Pos.x>=mDPad.x+mDPad.w-dpadSizePieceW)
        {
            ev.key.keysym.sym = SDLK_RIGHT;
            SDL_PushEvent(&ev);

            if(down)
            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.sym = SDLK_LEFT;
                SDL_PushEvent(&evUp);
            }

        }

        mDPad.insertFingerId(touchFingerEvent.fingerId);
        ok |= true;
    }
    else
    {
        auto it = mDPad.mFingerSet.find(touchFingerEvent.fingerId);

        if( it != mDPad.mFingerSet.end() )
        {
            mDPad.mFingerSet.erase(it);

            SDL_Event evUp;
            evUp.type = SDL_KEYUP;

            evUp.key.keysym.sym = SDLK_UP;    SDL_PushEvent(&evUp);
            evUp.key.keysym.sym = SDLK_LEFT;  SDL_PushEvent(&evUp);
            evUp.key.keysym.sym = SDLK_RIGHT; SDL_PushEvent(&evUp);
            evUp.key.keysym.sym = SDLK_DOWN;  SDL_PushEvent(&evUp);
        }

    }


    ok |= bindButtonCommand(mConfirmButton, IC_JUMP);
    ok |= bindButtonCommand(mStartButton, IC_JUMP);
    ok |= bindButtonCommand(mStatusButton, IC_STATUS);
    ok |= bindButtonCommand(mShootButton, IC_FIRE);
    ok |= bindButtonCommand(mJumpButton, IC_JUMP);
    ok |= bindButtonCommand(mPogoButton, IC_POGO);

#endif

    return ok;
}

bool VirtualKeenControl::mouseState(const Vector2D<float> &Pos, const bool down)
{

#if SDL_VERSION_ATLEAST(2, 0, 0)    

    processConfigTouchbuttons(Pos, down);

    if( mPadConfigBackground.invisible )
        return false;

    auto bindButtonCommand = [&](TouchButton &button,
                                 const InputCommand &cmd)
    {
        if(button.invisible)
            return;

        button.mMouseDown = false;

        if( button.Rect().HasPoint(Pos) )
        {
            if(mPadConfigBackground.invisible)
            {
                gInput.setCommand(0, cmd, down);
            }

            button.mMouseDown = true;
        }
    };

    SDL_Event ev;
    ev.type = (down ? SDL_KEYDOWN : SDL_KEYUP);

    if(mPadConfigBackground.invisible)
    {
        if( !mDPad.invisible &&
                mDPad.isInside(Pos.x, Pos.y) )
        {
            // Size of the buttons on the dpad
            const float dpadSizePieceW = 0.4f*mDPad.w;
            const float dpadSizePieceH = 0.4f*mDPad.h;

            // Y-Direction
            // Up presses
            if(Pos.y<mDPad.y+dpadSizePieceH)
            {
                ev.key.keysym.sym = SDLK_UP;
                SDL_PushEvent(&ev);

                if(down)
                {
                    SDL_Event evUp;
                    evUp.type = SDL_KEYUP;
                    evUp.key.keysym.sym = SDLK_DOWN;
                    SDL_PushEvent(&evUp);
                }
            }
            // Down presses
            else if(Pos.y>=mDPad.y+mDPad.h-dpadSizePieceH)
            {
                ev.key.keysym.sym = SDLK_DOWN;
                SDL_PushEvent(&ev);

                if(down)
                {
                    SDL_Event evUp;
                    evUp.type = SDL_KEYUP;
                    evUp.key.keysym.sym = SDLK_UP;
                    SDL_PushEvent(&evUp);
                }
            }

            // X-Direction
            // Left presses
            if(Pos.x<mDPad.x+dpadSizePieceW)
            {
                ev.key.keysym.sym = SDLK_LEFT;
                SDL_PushEvent(&ev);

                if(down)
                {
                    SDL_Event evUp;
                    evUp.type = SDL_KEYUP;
                    evUp.key.keysym.sym = SDLK_RIGHT;
                    SDL_PushEvent(&evUp);
                }

            }
            // Right presses
            else if(Pos.x>=mDPad.x+mDPad.w-dpadSizePieceW)
            {
                ev.key.keysym.sym = SDLK_RIGHT;
                SDL_PushEvent(&ev);

                if(down)
                {
                    SDL_Event evUp;
                    evUp.type = SDL_KEYUP;
                    evUp.key.keysym.sym = SDLK_LEFT;
                    SDL_PushEvent(&evUp);
                }

            }
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
