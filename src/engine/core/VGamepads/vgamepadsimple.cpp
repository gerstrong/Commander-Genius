
#if defined(USE_VIRTUALPAD)

#include "vgamepadsimple.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <base/GsLogging.h>
#include <fileio/ResourceMgmt.h>

#include "dpad.h"
#include "button1.h"
#include "button2.h"
#include "button3.h"
#include "button4.h"
#include "buttonConfirm.h"
#include "buttonStart.h"
#include "buttonMenu.h"

////////////// TouchButton Implementation part /////////////////////

bool TouchButton::loadEmdbeddedPicture(const unsigned char *data,
                                       const unsigned int size)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    const auto &renderer = gVideoDriver.Renderer();

    const auto ok = mTexture.loadFromMem(data, size, renderer);

    if(!ok) return false;

    int h, w;

    SDL_QueryTexture(mTexture.getPtr(),
                     nullptr, nullptr,
                     &w, &h);

    this->w = w;    this->h = h;

    // Now derive a picture which would be seen upon interaction
    GsSurface negSfc;
    negSfc.loadImgInternal(data, "", size);
    negSfc.tintColor( GsColor(0xFF, 0xFF, 0xFF) );

    mHighlightTexture.loadFromSurface(negSfc, renderer);

    return ok;

#else
    return false;
#endif
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
void TouchButton::clearFingers()
{
    mFingerSet.clear();
}

void TouchButton::insertFingerId(const Sint64 fid)
{
    mFingerSet.insert(fid);
}

void TouchButton::removeFingerId(const Sint64 fid)
{
    auto it = mFingerSet.find(fid);
    if(it != mFingerSet.end())
    {
        mFingerSet.erase(fid);
    }
}

#endif

bool TouchButton::loadPicture(const std::string &picFile)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
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
#else
    return false;
#endif
}

bool TouchButton::handleFingerEvent(const GsVec2D<float> &Pos,
                                    const Sint64 fingerID,
                                    const bool fingerDown)
{
    if(invisible) // Because we don't see it, it won't interact
        return false;


    // Decide how to handle that finger/mouse cursor press/release
    if(fingerDown)
    {
        if(mFingerSet.find(fingerID) == mFingerSet.end())
        {
            if(!isInside(Pos))
                return false;

            mFingerSet.insert(fingerID);
        }

        isDown = true;
    }
    else
    {
        if(mFingerSet.find(fingerID) == mFingerSet.end())
            return false;

        auto it = mFingerSet.find(fingerID);
        if(it != mFingerSet.end())
        {
            mFingerSet.erase(fingerID);
        }

        if(mFingerSet.empty())
            isDown = false;
    }

    return true;
}

////////////// VirtualKeenControl Implementation part /////////////////////


VirtualKeenControl::~VirtualKeenControl()
{}

bool VirtualKeenControl::init()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    // Check if the virtual game pad image exist. If not download the picture package
    mDiscTexture.createCircle(0.05, 10, 0xFF, 0xFF, 0xFF);

    const GsRect<float> discRect(0.0f, 0.0f, 0.1f, 0.1f);

    /// Load The buttons images
    {
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

        if(!mMenuButton.loadEmdbeddedPicture(gButtonMenuPng, sizeof(gButtonMenuPng))) return false;
        mMenuButton.invisible = true;
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

    const auto iSizeW = gVideoDriver.getVidConfig().mVPadWidth;
    const auto iSizeH = gVideoDriver.getVidConfig().mVPadHeight;

    const float sizeWFactor = float(iSizeW)/200.0f;
    const float sizeHFactor = float(iSizeH)/200.0f;

    if(!mDPad.invisible)
    {
        const float dpadWSize = 0.2f*sizeWFactor;
        const float dpadHSize = 0.2f*sizeHFactor;

        const GsRect<float> dpadRect(left, bottom-dpadHSize*1.05f,
                                      dpadWSize, dpadHSize);

        mDPad.setRect(dpadRect);
        mDPad.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));

        // Ensure that the disc is positioned to the dpad correctly
        auto discRect = mDiscTexture.Rect();
        if(discRect.pos < dpadRect.pos ||
           discRect.pos > dpadRect.pos+dpadRect.dim-discRect.dim )
        {
            GsVec2D<float> centDiscPos(dpadRect.pos +
                                       (dpadRect.dim-discRect.dim)*0.5f);

            mDiscTexture.setPos(centDiscPos);
        }
    }

    if(!mConfirmButton.invisible)
    {
        const float buttonSizeW = 0.1f*sizeWFactor;
        const float buttonSizeH = 0.1f*sizeHFactor;

        const GsRect<float> confirmRect(right-2.0f*buttonSizeW,
                                        bottom-2.0f*buttonSizeH,
                                         buttonSizeW, buttonSizeH);

        mConfirmButton.setRect(confirmRect);

        mConfirmButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mMenuButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> menuBtnRect(left+3.5f*buttonWSize,
                                        bottom-1.5f*buttonHSize,
                                        buttonWSize, buttonHSize);

        mMenuButton.setRect(menuBtnRect);

        mMenuButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }


    if(!mStatusButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> statusRect(right-3.4f*buttonWSize,
                                       bottom-2.2f*buttonHSize,
                                       buttonWSize, buttonHSize);

        mStatusButton.setRect(statusRect);

        mStatusButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mStartButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> startRect(right-3.4f*buttonWSize,
                                      bottom-1.0f*buttonHSize,
                                      buttonWSize, buttonHSize);

        mStartButton.setRect(startRect);

        mStartButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mJumpButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> jumpRect(right-2.2f*buttonWSize,
                                     bottom-1.0f*buttonHSize,
                                      buttonWSize, buttonHSize);

        mJumpButton.setRect(jumpRect);

        mJumpButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mPogoButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> pogoRect(right-1.0f*buttonWSize,
                                     bottom-1.0f*buttonHSize,
                                      buttonWSize, buttonHSize);

        mPogoButton.setRect(pogoRect);

        mPogoButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

    if(!mShootButton.invisible)
    {
        const float buttonWSize = 0.1f*sizeWFactor;
        const float buttonHSize = 0.1f*sizeHFactor;

        const GsRect<float> shootRect(right-2.2f*buttonWSize,
                                      bottom-2.2f*buttonHSize,
                                      buttonWSize, buttonHSize);

        mShootButton.setRect(shootRect);

        mShootButton.mTexture.setAlpha(uint8_t(255.0f*mTranslucency));
    }

#endif

    return true;
}


void VirtualKeenControl::processConfig()
{
    assert(0);
}


void VirtualKeenControl::renderConfig()
{
    assert(0);
}


void VirtualKeenControl::hideEverything()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    mDPad.invisible = true;
    mConfirmButton.invisible = true;
    mStartButton.invisible = true;

    mJumpButton.invisible = true;
    mShootButton.invisible = true;
    mPogoButton.invisible = true;
    mStatusButton.invisible = true;
    mMenuButton.invisible = true;
#endif
}

void VirtualKeenControl::hideAllButtons()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    mConfirmButton.invisible = true;
    mStartButton.invisible = true;

    mJumpButton.invisible = true;
    mShootButton.invisible = true;
    mPogoButton.invisible = true;
    mStatusButton.invisible = true;
#endif
}



void VirtualKeenControl::render(GsWeakSurface &)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    auto &vidDrv = gVideoDriver;

    auto addTexture = [&vidDrv](TouchButton &button) -> void
    {
        if(!button.invisible &&
            button.w > 0.0f && button.h > 0.0f)
        {
            if(button.isDown)
                vidDrv.addTextureRefToRender(button.mHighlightTexture, button.Rect());
            else
                vidDrv.addTextureRefToRender(button.mTexture, button.Rect());
        }
    };

    if(!mDPad.invisible &&
        mDPad.w > 0.0f && mDPad.h > 0.0f)
    {
        vidDrv.addTextureRefToRender(mDPad.mTexture, mDPad.Rect());
    }

    addTexture(mConfirmButton);
    addTexture(mStartButton);

    addTexture(mStatusButton);
    addTexture(mMenuButton);
    addTexture(mShootButton);
    addTexture(mJumpButton);
    addTexture(mPogoButton);    

    if(!mDPad.invisible &&
        mDPad.x > 0.0f && mDPad.y > 0.0f )
    {
        gVideoDriver.addTextureRefToRender(mDiscTexture);
    }


#endif
}


void VirtualKeenControl::flush()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    mConfirmButton.clearFingers();
    mStartButton.clearFingers();
    mStatusButton.clearFingers();
    mMenuButton.clearFingers();
    mShootButton.clearFingers();
    mJumpButton.clearFingers();
    mPogoButton.clearFingers();
    mDPad.clearFingers();
#endif
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
bool VirtualKeenControl::allInvisible()
{
    bool allInvisible = true;

    allInvisible &= mDPad.invisible;
    allInvisible &= mConfirmButton.invisible;
    allInvisible &= mStartButton.invisible;
    allInvisible &= mMenuButton.invisible;

    allInvisible &= mJumpButton.invisible;
    allInvisible &= mShootButton.invisible;
    allInvisible &= mPogoButton.invisible;
    allInvisible &= mStatusButton.invisible;

    return allInvisible;
}


bool VirtualKeenControl::handleDPad(const GsVec2D<float> &Pos,
                                    const Sint64 fingerID,
                                    const bool fingerDown)
{
    SDL_Event ev;
    ev.type = (fingerDown ? SDL_KEYDOWN : SDL_KEYUP);

    const auto discW = mDiscTexture.Rect().dim.x;
    const auto discH = mDiscTexture.Rect().dim.y;

    bool ok = false;


    if(!mDPad.handleFingerEvent(Pos, fingerID, fingerDown))
        return false;


    bool discCentered = true;
    GsVec2D<float> discPos(mDPad.x+(mDPad.w-discW)*0.5f,
                           mDPad.y+(mDPad.h-discH)*0.5f);

    auto releaseAllDir = [&]() // Release all dpad direction presses
    {
        SDL_Event evUp;
        evUp.type = SDL_KEYUP;

        evUp.key.keysym.scancode = SDL_SCANCODE_UP;
        evUp.key.keysym.sym = SDLK_UP;
        SDL_PushEvent(&evUp);

        evUp.key.keysym.scancode = SDL_SCANCODE_LEFT;
        evUp.key.keysym.sym = SDLK_LEFT;
        SDL_PushEvent(&evUp);

        evUp.key.keysym.scancode = SDL_SCANCODE_RIGHT;
        evUp.key.keysym.sym = SDLK_RIGHT;
        SDL_PushEvent(&evUp);

        evUp.key.keysym.scancode = SDL_SCANCODE_DOWN;
        evUp.key.keysym.sym = SDLK_DOWN;
        SDL_PushEvent(&evUp);

        mDPad.isDown = false;
    };

    if( mDPad.isDown )
    {
        // Size of the buttons on the dpad
        const float dpadSizePieceW = 0.4f*mDPad.w;
        const float dpadSizePieceH = 0.4f*mDPad.h;

        // Y-Direction
        // Up presses
        if(Pos.y<mDPad.y+dpadSizePieceH)
        {
            ev.key.keysym.scancode = SDL_SCANCODE_UP;
            ev.key.keysym.sym = SDLK_UP;
            SDL_PushEvent(&ev);

            discPos.x = Pos.x - discW*0.5;
            discPos.y = Pos.y - discH*0.5;
            discCentered = false;

            {
                SDL_Event evUp;
                evUp.key.keysym.scancode = SDL_SCANCODE_DOWN;
                evUp.key.keysym.sym = SDLK_DOWN;
                evUp.type = SDL_KEYUP;
                SDL_PushEvent(&evUp);
            }
        }
        // Down presses
        else if(Pos.y>=mDPad.y+mDPad.h-dpadSizePieceH)
        {

            ev.key.keysym.scancode = SDL_SCANCODE_DOWN;
            ev.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&ev);

            discCentered = false;

            {
                SDL_Event evUp;
                evUp.key.keysym.scancode = SDL_SCANCODE_UP;
                evUp.key.keysym.sym = SDLK_UP;
                evUp.type = SDL_KEYUP;
                SDL_PushEvent(&evUp);
            }
        }

        // X-Direction
        // Left presses
        if(Pos.x<mDPad.x+dpadSizePieceW)
        {
            ev.key.keysym.sym = SDLK_LEFT;
            ev.key.keysym.scancode = SDL_SCANCODE_LEFT;
            SDL_PushEvent(&ev);

            discCentered = false;

            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.scancode = SDL_SCANCODE_RIGHT;
                evUp.key.keysym.sym = SDLK_RIGHT;
                SDL_PushEvent(&evUp);
            }
        }
        // Right presses
        else if(Pos.x>=mDPad.x+mDPad.w-dpadSizePieceW)
        {
            ev.key.keysym.sym = SDLK_RIGHT;
            ev.key.keysym.scancode = SDL_SCANCODE_RIGHT;
            SDL_PushEvent(&ev);

            discCentered = false;

            {
                SDL_Event evUp;
                evUp.type = SDL_KEYUP;
                evUp.key.keysym.scancode = SDL_SCANCODE_LEFT;
                evUp.key.keysym.sym = SDLK_LEFT;
                SDL_PushEvent(&evUp);
            }
        }
    }

    if(!discCentered)
    {
        discPos.x = Pos.x - discW*0.5;
        discPos.y = Pos.y - discH*0.5;
    }
    else
    {
        releaseAllDir();
    }

    discPos.x = std::min(discPos.x, mDPad.x+mDPad.w-discW);
    discPos.x = std::max(discPos.x, mDPad.x);
    discPos.y = std::min(discPos.y, mDPad.y+mDPad.h-discH);
    discPos.y = std::max(discPos.y, mDPad.y);

    mDiscTexture.setPos(discPos);

    return ok;
}

bool VirtualKeenControl::mouseFingerState(const GsVec2D<float> &Pos,
                                          const bool isFinger,
                                          const SDL_TouchFingerEvent &touchFingerEvent,
                                          const bool down)
{
    if(allInvisible())
        return false;

    bool ok = false;

    // If it is the mouse cursor the simulated finger gets a simple zero value.
    const Sint64 fingerID = isFinger ? touchFingerEvent.fingerId : 0;

    if(handleDPad(Pos, fingerID, down))
        return false;


    auto bindButtonCommand = [&](TouchButton &button,
                                 const InpCmd &cmd) -> bool
    {
        if(button.invisible)
            return false;


        bool stateChanged = false;
        if(button.Rect().HasPoint(Pos) && down && !button.isDown)
        {
           button.isDown = true;
           stateChanged = true;
        }
        else if(!down && button.isDown)
        {
            button.isDown = false;
            stateChanged = true;
        }

        if(stateChanged)
        {
            gInput.setCommand(0, cmd, down);

            if(isFinger)
            {
                if(down)
                {
                    button.insertFingerId(touchFingerEvent.fingerId);
                }
                else
                {
                    button.removeFingerId(touchFingerEvent.fingerId);
                }
            }

            return true;
        }
        /*
        else if(isFinger)
        {
            auto it = button.mFingerSet.find(touchFingerEvent.fingerId);

            if( it != button.mFingerSet.end() )
            {
                gInput.setCommand(0, cmd, false);
                button.mFingerSet.erase(it);
            }            
        }
*/
        return false;
    };

    ok |= bindButtonCommand(mConfirmButton, IC_JUMP);
    ok |= bindButtonCommand(mStartButton, IC_JUMP);
    ok |= bindButtonCommand(mStatusButton, IC_STATUS);
    ok |= bindButtonCommand(mMenuButton, IC_BACK);
    ok |= bindButtonCommand(mShootButton, IC_FIRE);
    ok |= bindButtonCommand(mJumpButton, IC_JUMP);
    ok |= bindButtonCommand(mPogoButton, IC_POGO);

#endif

    return ok;
}

bool VirtualKeenControl::mouseState(const GsVec2D<float> &Pos, const bool down)
{
    return mouseFingerState(Pos, false, SDL_TouchFingerEvent(), down);
}
#endif
