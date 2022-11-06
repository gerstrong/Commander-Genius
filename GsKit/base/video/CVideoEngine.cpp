/*
 * CVideoEngine.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 */

#include <base/GsTimer.h>
#include "CVideoEngine.h"
#include <base/GsLogging.h>
#include <base/utils/ThreadPool.h>
#include <SDL_syswm.h>
#include <base/video/scaler/scalebit.h>
#include <SDL_version.h>
//#include "graphics/GsGraphics.h"

Uint16 getPowerOfTwo(const Uint16 value)
{
    Uint16 output = 1;
    while (output<value)
        output <<= 1;
    return output;
}

CVideoEngine::CVideoEngine(const CVidConfig& VidConfig) :
m_VidConfig(VidConfig)
{}


CVideoEngine::~CVideoEngine()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

    mpMainScreenTexture = nullptr;

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    if(window)
    {
        SDL_DestroyWindow(window);
    }

#endif

}


bool CVideoEngine::init()
{
    // Setup mode depends on some systems.
#if defined(CAANOO) || defined(WIZ) || defined(DINGOO) || defined(NANONOTE) || defined(ANDROID) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
    m_Mode = SDL_SWSURFACE;
#elif defined(GP2X)
    m_Mode = SDL_HWSURFACE;
#else
#endif

#ifdef USE_OPENGL
    // Enable OpenGL if selected
    if(m_VidConfig.mOpengl && m_VidConfig.mVSync)
    {
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    }
#endif // USE_OPENGL

    // Although no real screen setup has happened yet
    // some initialization of active display can already be prepared
    const auto &asp = m_VidConfig.mAspectCorrection.dim;
    updateActiveArea(m_VidConfig.mDisplayRect, asp);

    #ifdef _WIN32 // So far this only works under windows
    {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

        int ok = SDL_GetWindowWMInfo(window,&info);

        if( ok )
        {
            if(ok > 0)
            {
                    SDL_ShowWindow(window);
            }
        }

    }
    #endif

    mClearColor = m_VidConfig.mBorderColors;

    return true;
}


void CVideoEngine::updateActiveArea(const GsRect<Uint16>& displayRes,
                                    const GsVec2D<int> asp)
{
    const int aspWidth  = asp.x;
    const int aspHeight = asp.y;

    if (aspWidth == 0 || aspHeight == 0)
    {
        mActiveAreaRect.pos = {0, 0};
        mActiveAreaRect.dim = displayRes.dim;
        return;
    }

    const int scaleFactorY = displayRes.dim.y/m_VidConfig.mGameRect.dim.y;
    const int scaleFactorX = displayRes.dim.x/m_VidConfig.mGameRect.dim.x;

    const int scaleFactorMin = std::min(scaleFactorY, scaleFactorX);

    if (aspHeight*displayRes.dim.x >= aspWidth*displayRes.dim.y) // Wider than width:height, so shrink width
    {
        if(m_VidConfig.mIntegerScaling && scaleFactorMin > 0)
        {
            mActiveAreaRect.dim.y = scaleFactorMin*m_VidConfig.mGameRect.dim.y;
            mActiveAreaRect.dim.x = scaleFactorMin*m_VidConfig.mGameRect.dim.x;
        }
        else
        {
            mActiveAreaRect.dim.y = displayRes.dim.y;
            mActiveAreaRect.dim.x = ((displayRes.dim.y*aspWidth)/aspHeight);

            mActiveAreaRect.pos = (displayRes.dim-mActiveAreaRect.dim)/2;
        }
    }
    else // Taller than width:height so adapt height
    {
        if(m_VidConfig.mIntegerScaling && scaleFactorMin > 0)
        {
            mActiveAreaRect.dim.y = scaleFactorMin*m_VidConfig.mGameRect.dim.y;
            mActiveAreaRect.dim.x = scaleFactorMin*m_VidConfig.mGameRect.dim.x;
        }
        else
        {
            mActiveAreaRect.dim.x = displayRes.dim.x;
            mActiveAreaRect.dim.y = (displayRes.dim.x*aspHeight)/aspWidth;
        }
        mActiveAreaRect.pos = (displayRes.dim-mActiveAreaRect.dim)/2;
    }
}



bool CVideoEngine::createSurfaces()
{
    return createSurfaces(m_VidConfig.mGameRect, mScrollSurfaceVec.size());
}

std::vector<GsScrollSurface> &CVideoEngine::getScrollSurfaceVec()
{
    return mScrollSurfaceVec;
}

void CVideoEngine::addVideoTask(const std::function<void()> &task)
{
    mVideoTasks.push(task);
}

bool CVideoEngine::allocateScrollSurfaces(const unsigned int numSfc)
{
    GsRect<Uint16> gamerect;

    gamerect.dim.x = mGameSfc.width();
    gamerect.dim.y = mGameSfc.height();

    const int squareSize = getPowerOfTwo( gamerect.dim.y > gamerect.dim.x ?
                                          gamerect.dim.y : gamerect.dim.x );

    gLogging.ftextOut("ScrollSurface creation of %dx%d!\n<br>",
                     squareSize, squareSize );


    mScrollSurfaceVec.clear();
    mScrollSurfaceVec.resize(numSfc);

    bool ok = true;

    for( auto &scrollSfc : mScrollSurfaceVec )
    {
        ok &= scrollSfc.create(m_Mode, squareSize);
    }

    return ok;
}

bool CVideoEngine::createSurfaces(const GsRect<Uint16> &gamerect,
                                  const unsigned int numScrollSfcs)
{
    int borderHUpper = 0;
    int borderHBottom = 0;

    borderHUpper = m_VidConfig.mHorizBorders;
    borderHBottom = m_VidConfig.mHorizBorders;

    gLogging.ftextOut("Gamesurface creation of %dx%d!\n<br>",
                     gamerect.dim.x, gamerect.dim.y );

    mGameSfc.create(m_Mode,
                    gamerect.dim.x,
                    gamerect.dim.y, RES_BPP,
                    0x00FF0000,
                    0x0000FF00,
                    0x000000FF,
                    0xFF000000);


#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode(mGameSfc.getSDLSurface(), SDL_BLENDMODE_NONE);
#endif

    allocateScrollSurfaces(numScrollSfcs);

    auto blit = mGameSfc.getSDLSurface();

    gLogging.ftextOut("ScaleX is at %d!\n<br>", m_VidConfig.m_ScaleXFilter );

    gLogging.ftextOut("ScreenSurface creation of %dx%d!\n<br>",
                     blit->w, blit->h );


    if(m_VidConfig.m_ScaleXFilter != VidFilter::NONE)
    {
        const auto iScale = int(m_VidConfig.m_ScaleXFilter);

        mFilteredSfc.create(m_Mode,
                            blit->w*iScale,
                            (blit->h+borderHUpper+borderHBottom)*iScale,
                            RES_BPP, 0, 0, 0, 0);

        mpScreenSfc = &mFilteredSfc;

        gLogging << "Given a Filter Screen pointer set to filtered surface" << CLogFile::endl;
    }
    else
    {
        mpScreenSfc = &mGameSfc;
        gLogging << "Screen pointer set to game surface directly" << CLogFile::endl;
    }

    initOverlaySurface(Uint16(blit->w), Uint16(blit->h));

#if SDL_VERSION_ATLEAST(2, 0, 0)

    const auto iScale = int(m_VidConfig.m_ScaleXFilter);

    const auto texW = gamerect.dim.x*iScale;
    const auto texH = gamerect.dim.y*iScale;

    gLogging << "Creation of main screen texture " <<
                      blit->w << "x" << blit->h << CLogFile::endl;

    mpMainScreenTexture.reset( SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   texW, texH) );

#endif

    return true;
}

GsRect<Uint16> CVideoEngine::getActiveAreaRect() const
{
    return mActiveAreaRect;
}

void CVideoEngine::drawHorizBorders()
{
    int borderHUpper = m_VidConfig.mHorizBorders;
    int borderHBottom = m_VidConfig.mHorizBorders;

    GsRect<Uint16> rect;

    rect.pos = {0, 0};
    rect.dim.x = mGameSfc.width();
    rect.dim.y = borderHUpper;

    const auto color = m_VidConfig.mBorderColors;

    // Upper part
    mGameSfc.fillRGB( rect, color.r, color.g, color.b );


    rect.pos.y = mGameSfc.height()-borderHBottom;

    // Lower Part
    mGameSfc.fillRGB( rect, color.r, color.g, color.b );

}

void CVideoEngine::processVideoTasks()
{
    while(!mVideoTasks.empty())
    {
        const auto &f = mVideoTasks.front();
        f();
        mVideoTasks.pop();
    }
}

void CVideoEngine::scaleAndFilter()
{
    const auto scaleXFilter = m_VidConfig.m_ScaleXFilter;

    // If ScaleX is enabled scaleup to screensurface
    // Otherwise screensurface point to gameSfc and nothing needs to be done
    if(scaleXFilter != VidFilter::NONE)
    {
        auto srcSfc = mGameSfc.getSDLSurface();
        auto dstSfc = mFilteredSfc.getSDLSurface();

        SDL_LockSurface( srcSfc );
        SDL_LockSurface( dstSfc );

        auto dstPixels = dstSfc->pixels;

        scale(int(scaleXFilter),
              dstPixels ,
              dstSfc->pitch,
              srcSfc->pixels,
              srcSfc->pitch,
              dstSfc->format->BytesPerPixel,
              srcSfc->w,
              srcSfc->h	);

        SDL_UnlockSurface( dstSfc );
        SDL_UnlockSurface( srcSfc );
    }
}

void CVideoEngine::blitScrollSurfaces(GsWeakSurface &blitSfc)
{
    for (auto &scrollSfc : mScrollSurfaceVec)
    {
        scrollSfc.blitScrollSurface(blitSfc);
    }

    if(!mScrollSurfaceVec.empty())
    {
        mScrollSurfaceVec[0].blitScrollTextures();
    }
}

void CVideoEngine::resetScrollBuffers()
{
    for (auto &scrollSfc : mScrollSurfaceVec)
    {
        scrollSfc.resetScrollbuffer();
    }
}

void CVideoEngine::updateScrollBuffers(const Sint16 SBufferX, const Sint16 SBufferY)
{
    for (auto &scrollSfc : mScrollSurfaceVec)
    {
        scrollSfc.UpdateScrollBufX(SBufferX);
        scrollSfc.UpdateScrollBufY(SBufferY);
    }
}

GsScrollSurface& CVideoEngine::getScrollSfc(const int idx)
{
    return mScrollSurfaceVec.at(idx);
}

void CVideoEngine::shutdown()
{
}
