/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"
#include <base/GsLogging.h>
#include <base/GsApp.h>

#include "graphics/GsGraphics.h"


CSDLVideo::CSDLVideo(const CVidConfig& VidConfig) :
CVideoEngine(VidConfig)
{}



bool CSDLVideo::init()
{	
    if( !CVideoEngine::init() )
        return false;

#ifdef ANDROID
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
	gLogging.textOut(FONTCOLORS::BLACK,"Setting orientations: LandscapeLeft LandscapeRight<br>");
#endif

    if(m_VidConfig.mRenderScQuality == CVidConfig::RenderQuality::LINEAR)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		gLogging.textOut(FONTCOLORS::BLACK,"Render Quality linear<br>");
    }
    else
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
		gLogging.textOut(FONTCOLORS::BLACK,"Render Quality standard (0)<br>");
    }

    if(window)
    {
		gLogging.textOut(FONTCOLORS::BLACK,"Destroying SDL Window...<br>");
        SDL_DestroyWindow(window);
    }	

    Uint32 flags = 0;

    if(m_VidConfig.mFullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    }

	gLogging.textOut(FONTCOLORS::BLACK,
					"Creating SDL Window for"
					"%s ...<br>", 
					gApp.getName().c_str());					
	
#if __EMSCRIPTEN__
    window = SDL_CreateWindow(gApp.getName().c_str(),
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              m_VidConfig.mDisplayRect.dim.x, 
                              m_VidConfig.mDisplayRect.dim.y,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
#else
    window = SDL_CreateWindow(gApp.getName().c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_VidConfig.mDisplayRect.dim.x,
                              m_VidConfig.mDisplayRect.dim.y,
                              flags);
#endif    

	if(!window)
	{
		gLogging.textOut(FONTCOLORS::RED,"SDL_CreateWindow(): %s<br>", SDL_GetError());		
		return false;
	}	

    mpMainScreenTexture = nullptr;
	
    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    Uint32 rendererFlags = 0;

    if(m_VidConfig.mVSync)
    {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }					
		
    rendererFlags |= SDL_RENDERER_ACCELERATED;

    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
		
	if(!renderer)
	{
		gLogging.textOut(FONTCOLORS::RED,"SDL_CreateRenderer(): %s<br>", SDL_GetError());
		return false;
	}
	
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    resizeDisplayScreen(m_VidConfig.mDisplayRect);

	return true;
}



void CSDLVideo::resizeDisplayScreen(const GsRect<Uint16>& newDim)
{
    auto &log = gLogging;

    try
    {
        const auto &asp = m_VidConfig.mAspectCorrection.dim;
        updateActiveArea(newDim, asp);

        if(renderer == nullptr)
            throw "Error. Renderer not inited.";

        SDL_RenderSetLogicalSize(renderer,
                                 mActiveAreaRect.dim.x,
                                 mActiveAreaRect.dim.y);

        SDL_RenderSetIntegerScale(renderer,
                    (m_VidConfig.mIntegerScaling) ? SDL_TRUE : SDL_FALSE);

        SDL_Rect viewport;
        SDL_RenderGetViewport(renderer, &viewport);

        int dimx, dimy;
        SDL_RenderGetLogicalSize(renderer, &dimx, &dimy);

        mActiveAreaRect.pos.x = (viewport.w-dimx)/2;
        mActiveAreaRect.pos.y = (viewport.h-dimy)/2;
    }
    catch (const char* msg)
    {
        log << msg << CLogFile::endl;
    }
    catch (...)
    {
        log << "unhandled exception." << CLogFile::endl;
    }
}


bool CSDLVideo::initOverlaySurface(const Uint16 width,
                                   const Uint16 height )
{

    mOverlaySurface.create(m_Mode, width, height, RES_BPP,
                           0,0,0,0);


#if SDL_VERSION_ATLEAST(2, 0, 0)
    mOverlaySurface.setBlendMode(SDL_BLENDMODE_BLEND);
#endif
    mOverlaySurface.setAlpha(0);

    return true;
}


void CSDLVideo::setLightIntensity(const float intensity)
{
    Uint8 intense = Uint8(intensity*255.0f);

    mOverlaySurface.setAlpha(255-intense);
    mOverlaySurface.fillRGB(0, 0, 0);
}


void CSDLVideo::collectSurfaces()
{
    if( mOverlaySurface.getAlpha() > 0 )
    {
        // TODO: If horiz borders enabled we need to reposition the cursor
        mOverlaySurface.blitTo(mGameSfc);
    }
}

void CSDLVideo::clearSurfaces()
{
    // TODO: Clear color here!
    mOverlaySurface.fillRGB(0,0,0);
    mGameSfc.fillRGB(mClearColor.r, mClearColor.g, mClearColor.b);
}


/**
 * @brief tilt  tilt the rect (90 degree when squared) clockwise
 *              note: This happens on a squared coordinate system,
 *                    otherwise tilted as intended would never work
 * @param dpadRect  rect to rotate
 * @param rotPt     point of rotation
 * @return The rotated rect
 */
GsRect<int> tilt(const GsRect<int> &dpadRect,
                 const GsVec2D<int> &rotPt)
{
    // Because tilt with 90 degree only works if the plane is squared
    // the coordinate must be transformed
    if(rotPt.x <= 0 || rotPt.y <= 0)
        return GsRect<int>(0,0,0,0);

    const auto width = dpadRect.dim.x*rotPt.y;
    const auto height = dpadRect.dim.y*rotPt.x;

    const auto dpadMidX = dpadRect.pos.x*rotPt.y + width/2;
    const auto dpadMidY = dpadRect.pos.y*rotPt.x + height/2;

    const auto rotTransX = rotPt.x*rotPt.y;
    const auto rotTransY = rotPt.y*rotPt.x;

    const auto x1_rel = dpadMidX-rotTransX;
    const auto y1_rel = dpadMidY-rotTransY;
    const int x2_rel = -y1_rel;
    const int y2_rel = x1_rel;

    auto retX = (x2_rel+rotTransX-height/2)/rotPt.y;
    auto retY = (y2_rel+rotTransY-width/2)/rotPt.x;

    return GsRect<int>( retX, retY,
                        dpadRect.dim.x, dpadRect.dim.y);
}

extern GsTexture testTex;

void CSDLVideo::transformScreenToDisplay()
{

#if SDL_VERSION_ATLEAST(2, 0, 0) 

    const bool tiltVideo = m_VidConfig.mTiltedScreen;

    mpScreenSfc->lock();
    SDL_UpdateTexture(mpMainScreenTexture.get(),
                      nullptr,
                      mpScreenSfc->getSDLSurface()->pixels,
                      mpScreenSfc->width() * sizeof (Uint32));
    mpScreenSfc->unlock();


    SDL_SetRenderDrawColor(renderer,
                           mClearColor.r,
                           mClearColor.g,
                           mClearColor.b,
                           255);




    auto RenderCopy = [&] (SDL_Renderer * local_renderer,
                           SDL_Texture * local_texture,
                           const SDL_Rect * local_srcrect,
                           const SDL_Rect * local_dstrect,
                           const SDL_Point * pt)
    {
        if(tiltVideo)
        {
            if(!local_dstrect)
            {
                SDL_RenderCopyEx(local_renderer,
                                 local_texture,
                                 local_srcrect,
                                 nullptr,
                                 90.0,
                                 pt,
                                 SDL_FLIP_NONE);
            }
            else
            {

                auto adaptRect = *local_dstrect;

                adaptRect.h = local_dstrect->w;
                adaptRect.w = local_dstrect->h;

                const auto deltaX = (adaptRect.h-adaptRect.w)/2;
                const auto deltaY = (adaptRect.w-adaptRect.h)/2;

                adaptRect.x += deltaX; adaptRect.y += deltaY;


                SDL_RenderCopyEx(local_renderer,
                                 local_texture,
                                 local_srcrect,
                                 &adaptRect,
                                 90.0,
                                 pt,
                                 SDL_FLIP_NONE);
            }
        }
        else
        {
            SDL_RenderCopy(local_renderer,
                           local_texture,
                           local_srcrect,
                           local_dstrect);
        }
    };


    SDL_RenderClear(renderer);    

    const GsVec2D<int> rotPt = mActiveAreaRect.pos + mActiveAreaRect.dim/2;

    SDL_Rect mainDstrect = mActiveAreaRect.SDLRect();
    SDL_Point pt = {mainDstrect.h/2, mainDstrect.w/2};

    RenderCopy(renderer, mpMainScreenTexture.get(),
               nullptr, &mainDstrect, &pt);


    // Now render the textures which additionally sent over...
    while(!mRenderTexturePtrs.empty())
    {
        auto &triple = mRenderTexturePtrs.front();

        GsTexture &gsTexture = std::get<0>(triple);
        SDL_Texture *texture = gsTexture.getPtr();

        //assert(texture);
        if(!texture)
        {
            mRenderTexturePtrs.pop();
            continue;
        }

        const GsRect<Uint16> &src = std::get<1>(triple);
        const GsRect<Uint16> &dst = std::get<2>(triple);


        if(src.empty())
        {
            if(dst.empty())
            {
                RenderCopy(renderer, texture, nullptr, nullptr, nullptr);
            }
            else
            {
                SDL_Rect dstSDL = dst.SDLRect();

                // Transfrom the coordinates for the final screen.
                dstSDL.y += mActiveAreaRect.pos.y;
                dstSDL.x += mActiveAreaRect.pos.x;

                SDL_Point texPt = pt;

                texPt.x = dstSDL.h/2;
                texPt.y = dstSDL.w/2;

                if(tiltVideo)
                {
                    // If screenWxscreenH is square it works!. Need to transform on of the coords -> only thing left... :-)
                    dstSDL = tilt(dstSDL, rotPt).SDLRect();
                }

                RenderCopy(renderer, texture, nullptr, &dstSDL, &texPt);
            }
        }
        else
        {
            SDL_Rect srcSDL = src.SDLRect();
            if(dst.empty())
            {
                RenderCopy(renderer, texture, &srcSDL, nullptr, nullptr);
            }
            else
            {
                SDL_Rect dstSDL = dst.SDLRect();
                RenderCopy(renderer, texture, &srcSDL, &dstSDL, nullptr);
            }
        }

        mRenderTexturePtrs.pop();
    }

    SDL_RenderPresent(renderer);


#else

    // Blit the stuff
    mpScreenSfc->blitScaledTo(mDisplaySfc);

    // Flip the screen (We use double-buffering on some systems.)
    mDisplaySfc.flip();

#endif


}
