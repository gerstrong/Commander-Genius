/*
 * CResourceLoader.cpp
 *
 *  Created on: 23.10.2010
 *      Author: gerstrong
 */

#include "CResourceLoader.h"
#include "graphics/GsGraphics.h"

#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <base/utils/StringUtils.h>
#include <base/video/GsEffectController.h>
#include <cassert>


CResourceLoaderBackground::CResourceLoaderBackground() :
m_permil(0),
m_permiltarget(0),
m_min_permil(0),
m_max_permil(1000),
mRunning(false),
m_style(PROGRESS_STYLE_TEXT)
{
    SDL_Rect rect;
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    rect.x = 0;		rect.y = 0;
    rect.w = gameRes.w;	rect.h = gameRes.h;

    mProgressSfc.createRGBSurface(rect);
    mProgressSfc.makeBlitCompatible();
}


CResourceLoaderBackground::~CResourceLoaderBackground()
{}

/**
 * This will start up the thread for the load display and process the display of loading
 * and then return
 */
void CResourceLoaderBackground::RunLoadActionBackground(Action* act,
                                   const int min_permil,
                                   const int max_permil)
{
    mpAction = act;
    mRunning = true;
    m_max_permil = max_permil;
    m_min_permil = min_permil;
    m_permil = m_min_permil;
    m_permiltarget = m_min_permil;
}

void CResourceLoaderBackground::start()
{
    mpThread = threadPool->start(mpAction, "Loading Resources");
}



/**
 * Set the percentage of progress
 */
void CResourceLoaderBackground::setPermilageForce(const int permil)
{
    if(permil<m_max_permil && permil>m_min_permil)
        m_permil = permil;
    else
        m_permil = m_max_permil;
}



void CResourceLoaderBackground::setPermilage(const int permil)
{
    if(permil<m_max_permil && permil>=m_min_permil)
    {
        m_permiltarget = permil;
    }
    else
    {
        m_permil = m_permiltarget = m_max_permil;
    }
}





void CResourceLoaderBackground::run(const float)
{
    int ret;
    if(threadPool->finalizeIfReady(mpThread, &ret))
    {
        mRunning = false;
        mpThread = nullptr;
    }

    if(m_permil >= m_permiltarget)
    {
        setPermilage(m_permil+1);
    }
    else
    {
        int delta_permil = (m_permiltarget-m_permil)/2;

        if(delta_permil == 0)
            setPermilageForce(m_permil+1);
        else
            setPermilageForce(m_permil+delta_permil);
    }
}



/**
 * Progress the shown graphic here
 */
void CResourceLoaderBackground::render()
{
    mProgressSfc.fillRGB(0, 0, 0);

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int gameWidth = gameRes.w;
    const int gameHeight = gameRes.h;
    const int halfWidth = gameWidth/2;

    if(m_style == PROGRESS_STYLE_TEXT)
    {
        // Draw Loading Font... here!
        GsFont &Font = gGraphics.getFont(0);
        int percent = m_permil/10;
        int rest = m_permil%10;
        std::string text = "Loading ... " + itoa(percent)+"."+ itoa(rest)+" \%";

        GsRect<Uint16> textRect;
        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        const float scaleUpW = float(gameRes.w)/320.0f;
        const float scaleUpH = float(gameRes.h)/200.0f;
        textRect.x = (int)(80.0*scaleUpW);        textRect.y = (int)(100.0*scaleUpH);
        textRect.w = 200;        textRect.h = 10;

        mProgressSfc.fillRGB(textRect, 0, 0, 0);
        Font.drawFont(mProgressSfc.getSDLSurface(), text , textRect.x, textRect.y, true);
    }
    else if(m_style == PROGRESS_STYLE_BITMAP)
    {
        GsBitmap &Bitmap = *gGraphics.getBitmapFromStr(0, "ONEMOMEN");
        SDL_Rect rect;
        int width = Bitmap.width();
        int height = Bitmap.height();
        Bitmap._draw( (gameWidth-width)/2, (gameHeight-height)/2, mProgressSfc.getSDLSurface());

        rect.x = (gameWidth-width)/2;
        rect.y = (gameHeight+height)/2;

        rect.w = (width*m_permil)/1000;
        rect.h = 4;

        // Fade from yellow to green with this formula
        mProgressSfc.fillRGB(rect, 200-(200*m_permil)/1000, 200, 0);
    }
    else if(m_style == PROGRESS_STYLE_BAR)
    {
        SDL_Rect rect;
        SDL_Rect bgRect;
        rect.x = (gameWidth-halfWidth)/2;
        rect.y = gameHeight/2;

        rect.w = (halfWidth*m_permil)/1000;
        rect.h = gameHeight/50;

        bgRect = rect;
        bgRect.x--;
        bgRect.y--;
        bgRect.w = halfWidth+2;
        bgRect.h = rect.h+2;

        const Uint8 r = 255-(255*m_permil)/1000;
        const Uint8 g = (255*m_permil)/1000;
        const Uint8 b = 0;

        // Fade from yellow to green with this formula
        mProgressSfc.fillRGB(bgRect, 128, 128, 128);
        mProgressSfc.fillRGB(rect, r, g, b);
    }

    // In there is garbage of other drawn stuff clean it up.
    auto blit = gVideoDriver.getBlitSurface();
    BlitSurface( mProgressSfc.getSDLSurface(), nullptr, blit, nullptr );
}



