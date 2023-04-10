/*
 * CVideoDriver.cpp
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 *
 *  Driver which will handle the Video Functions. This Driver handle the window mode and resolution mgmt.
 *  The Rendering itself is performed under COpenGL or CSDLVideo Class depending on what is enabled.
 */
#include "CVideoDriver.h"

#include <base/video/CSDLVideo.h>
#include <base/video/COpenGL.h>

//#include "graphics/GsGraphics.h"
#include <base/GsLogging.h>
#include <base/GsApp.h>
#include <base/interface/FindFile.h>
#include <iostream>
#include <fstream>
#include <SDL_syswm.h>
#include <SDL_image.h>

bool useScrollTexels = false;


CVideoDriver::~CVideoDriver()
{
  if(mSDLImageInUse)
  {
    // unload the dynamically loaded image libraries
    IMG_Quit();
  }
}

#if SDL_VERSION_ATLEAST(2, 0, 0)

void CVideoDriver::addTextureRefToVirtPadRender(GsTextureElem& textureElemRef)
{
    addTextureRefToVirtPadRender(textureElemRef.Texture(), textureElemRef.Rect());
}

void CVideoDriver::pushTextureRef(GsTexture& textureRef,
                                  const SDL_Rect &src_rect,
                                  const SDL_Rect &dst_rect)
{
    SDL_Rect src_rect_final = src_rect;
    SDL_Rect dst_rect_final = dst_rect;

    const auto &gameSfc = mpVideoEngine->gameSfc();

    const auto gameRect = gameSfc.getSDLSurface()->clip_rect;
    const auto activeArea = mpVideoEngine->getActiveAreaRect();

    dst_rect_final.x = (dst_rect_final.x*activeArea.dim.x)/gameRect.w;
    dst_rect_final.y = (dst_rect_final.y*activeArea.dim.y)/gameRect.h;
    dst_rect_final.w = (dst_rect_final.w*activeArea.dim.x)/gameRect.w;
    dst_rect_final.h = (dst_rect_final.h*activeArea.dim.y)/gameRect.h;

    std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> >
            triple( textureRef, src_rect_final, dst_rect_final );

    mpVideoEngine->mRenderTexturePtrs.push(triple);
}

void CVideoDriver::addTextureRefToVirtPadRender(GsTexture& textureRef,
                                         const GsRect<float> &dstRect)
{
    const GsRect<Uint16> clickGameArea = mpVideoEngine->getActiveAreaRect();

    const auto dpadX  = Uint16(float(clickGameArea.dim.x) * dstRect.pos.x);
    const auto dpadY  = Uint16(float(clickGameArea.dim.y) * dstRect.pos.y);

    const auto dpadWidth  = Uint16(float(clickGameArea.dim.x) * dstRect.dim.x);
    const auto dpadHeight = Uint16(float(clickGameArea.dim.y) * dstRect.dim.y);

    const GsRect<Uint16> dpadRect(dpadX, dpadY,
                                  dpadWidth, dpadHeight);


    std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> >
            triple( textureRef, {0, 0, 0, 0}, dpadRect );

    mpVideoEngine->mRenderTexturePtrs.push(triple);
}
#endif


bool CVideoDriver::init()
{
    mVidConfig.reset();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
    {
        gLogging.textOut(FONTCOLORS::RED, "Could not initialize SDL: %s<br>", SDL_GetError());
        return false;
    }

    gLogging.textOut(FONTCOLORS::GREEN, "SDL was successfully initialized!<br>");

    initResolutionList();


    if(!mSDLImageInUse)
    {
      // load support for the JPG and PNG image formats
      const int flags = IMG_INIT_JPG|IMG_INIT_PNG;
      const int inittedFlags = IMG_Init(flags);
      if( (inittedFlags & flags) != flags)
      {
          gLogging.textOut(FONTCOLORS::RED, "IMG_Init: Failed to init required jpg and png support!\n");
          gLogging.textOut(FONTCOLORS::RED, "IMG_Init: %s\n", IMG_GetError());
          gLogging.textOut(FONTCOLORS::RED, "IMG_Init: CG will continue without it.\n");
      }
      else
      {
        mSDLImageInUse = true;
      }
    }

    // take the first default resolution. It might be changed if there is a config file already created
    // If there are at least two possible resolutions choose the second one, as this is normally one basic small resolution
    GsRect<Uint16> myMode;
    myMode.dim = *(mResolutionSet.begin());
    setMode(myMode);

    return true;
}

// initResolutionList() reads the local list of available resolution.
// This function can only be called internally
// TODO: This should return something!
bool CVideoDriver::initResolutionList()
{
    // This call will get the resolution we have right now and set it up for the system
    // On Handheld devices this means they will only take that resolution and that would it be.
    // On the PC, this is the current resolution but we add some more.

#if SDL_VERSION_ATLEAST(2, 0, 0)
    GsVec2D<Uint16> resolution = {1920, 1080};
#else
    GsRect<Uint16> resolution(SDL_GetVideoInfo());
#endif

#if defined(ANDROID)
    resolution.x = 320;
    resolution.y = 200;
#elif defined(__SWITCH__)
    resolution.x = 1280;
    resolution.y = 720;
#endif

    GsVec2D<Uint16> desktopResolution(resolution);

    // We have a resolution list, clear it and create a new one.
    if(!mResolutionSet.empty())
    {
        mResolutionSet.clear();
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)


    // For now we consider primary display
    const int dispIdx = 0;
    const int numDispModes = SDL_GetNumDisplayModes(dispIdx);


    for(int modeIdx=0 ; modeIdx<numDispModes ; modeIdx++)
    {
        SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };

        if (SDL_GetDisplayMode(dispIdx, modeIdx, &mode) != 0)
        {
            gLogging.ftextOut("SDL_GetDisplayMode failed: %s", SDL_GetError());
            continue;
        }

        mResolutionSet.insert(GsVec2D<Uint16>(mode.w, mode.h));
    }


#else

// TODO: Not sure if those defines are really needed anymore.
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
    resolution.x = 320; //  320;
    resolution.y = 200;//  480;
#elif defined(ANDROID)
    resolution.x = 320;
    resolution.y = 200;
#endif

    // Now on non-handheld devices let's check for more resolutions.
#if !defined(EMBEDDED)
    for (unsigned int c = 0; c < NUM_MAIN_RESOLUTIONS; c++) {
        // Depth won't be read anymore! Take the one the system is using actually
        if (sscanf(ResolutionsList[c], "%hux%hu", &resolution.dim.x, &resolution.h)
                >= 2) {
            // Now check if it's possible to use this resolution
            verifyResolution(resolution, SDL_FULLSCREEN);
        }
    }

    // In case there is no fullscreen, we will adapt the resolution it fits best to the window
    if (!mVidConfig.mFullscreen) {
        int e = 1;
        resolution.dim.x = 320;
        resolution.h = 200;

        int maxwidth = SDL_GetVideoInfo()->current_w;

        while (resolution.dim.x < maxwidth) {
            resolution.dim.x = 320 * e;
            resolution.h = 200 * e;

            // Now check if it's possible to use this resolution
            verifyResolution(resolution, 0);
            e++;
        }
    }
#endif


#endif

    /// The last resolution in the list is the desktop normally,
    /// therefore the highest one is what we setup
    mResolutionSet.insert(desktopResolution);

    mResolutionPos = mResolutionSet.begin();


    /// Game resolution part: The resolution used internally by the games

    mGameReslist =
    {
        {320,200},
        {320,240},
        {640,360},
        {640,480}
    };

    /// Aspect Ratio section
    mAspectSet =
    {
    "disabled",
    "4:3",
    "16:9",
    "16:10",
    "5:4"
    };

    return true;
}

void CVideoDriver::verifyResolution(GsVec2D<Uint16> &resolution,
                                    const int flags)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
    if (SDL_VideoModeOK(resolution.dim.x, resolution.h, 32, flags)) {
        std::list< GsRect<Uint16> >::iterator i;
        for (i = mResolutionSet.begin(); i != mResolutionSet.end(); i++) {
            if (*i == resolution)
                break;
        }

        if (i == mResolutionSet.end()) {
#ifdef DEBUG
            gLogging.ftextOut(FONTCOLORS::BLUE, "Resolution %ix%ix%i %X added\n",
                    resolution.dim.x, resolution.h, 32);
#endif
            mResolutionSet.push_back(resolution);
        }
    }
#endif
}

std::set<std::string> CVideoDriver::getResolutionStrSet()
{
    std::set<std::string> resSet;

    for (const auto &res : mResolutionSet)
    {
        resSet.insert(itoa(res.x) + "x" + itoa(res.y));
    }


    return resSet;
}

std::set<std::string> CVideoDriver::getGameResStrSet()
{
    std::set<std::string> resSet;

    for (const auto &res : mGameReslist)
    {
        resSet.insert(itoa(res.x) + "x" + itoa(res.y));
    }

    return resSet;
}


std::set<std::string> CVideoDriver::getAspectStrSet()
{
    std::set<std::string> aspectSet;

    for (const auto &res : mAspectSet)
    {
        aspectSet.insert(res);
    }

    return aspectSet;
}


void CVideoDriver::setVidConfig(const CVidConfig& VidConf)
{
    mVidConfig = VidConf;

    SDL_ShowCursor(mVidConfig.mShowCursor ? SDL_ENABLE : SDL_DISABLE);

    setMode(mVidConfig.mDisplayRect);
}

void CVideoDriver::setMode(const int width, const int height)
{
    GsRect<Uint16> res{ 0,0,
                        static_cast<unsigned short>(width),
                        static_cast<unsigned short>(height) };
    setMode(res);
}

void CVideoDriver::setMode(const GsRect<Uint16>& res)
{
    mVidConfig.setResolution(res.dim);

    // Cycle through the list until the matching resolution is matched. If it doesn't exist
    // add it;
    for (mResolutionPos = mResolutionSet.begin();
         mResolutionPos != mResolutionSet.end();
         mResolutionPos++)
    {
        if (*mResolutionPos == res.dim)
            break;
    }

    if (mResolutionPos == mResolutionSet.end())
    {
        mResolutionSet.insert(res.dim);
        mResolutionPos--;
    }
}

bool CVideoDriver::applyMode()
{
    const GsRect<Uint16> &Res = mVidConfig.mDisplayRect;
    const GsRect<Uint16> &GameRect = mVidConfig.mGameRect;

    // Before the resolution is set, check, if the zoom factor is too high!
    while (((Res.dim.x / GameRect.dim.x) < mVidConfig.Zoom
            || (Res.dim.y / GameRect.dim.y) < mVidConfig.Zoom)
            && (mVidConfig.Zoom > 1))
        mVidConfig.Zoom--;

    // Check if some zoom/filter modes are illogical and roll them back accordingly
    if ((mVidConfig.Zoom == 3 && mVidConfig.m_ScaleXFilter == VidFilter::NONE)
            && !mVidConfig.mOpengl)
        mVidConfig.Zoom = 2;

    if (mVidConfig.Zoom == 0)
        mVidConfig.Zoom = 1;

    mVidConfig.mDisplayRect.dim = *mResolutionPos;

    return true;
}

bool CVideoDriver::setNativeResolution(const GsRect<Uint16> &dispRect,
                                       const unsigned int numScrollSfcs)
{
    return mpVideoEngine->createSurfaces(dispRect, numScrollSfcs);
}

bool CVideoDriver::start()
{
    bool retval;

    auto &log = gLogging;

    const std::string caption = gApp.getName();
#if !SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_WM_SetCaption(caption.c_str(), caption.c_str());
#endif

    log << "Starting graphics driver..." << CLogFile::endl;

#ifdef USE_OPENGL
    if (mVidConfig.mOpengl) // Try to use OpenGL if enabled by the user
    {
        log << "Trying OpenGL... " ;
        mpVideoEngine.reset(new COpenGL(mVidConfig));
        retval = mpVideoEngine->init();

        if (!retval)
        {
            log << "failed." << CLogFile::endl;

            mVidConfig.mOpengl = false;
            applyMode();
            mpVideoEngine.reset(new CSDLVideo(mVidConfig));
            retval = mpVideoEngine->init();
            log << "Falling back to SDL Video...";
        }
        else
        {
            log << " ok." << CLogFile::endl;
        }
    }
    else
#endif
    {
        log << "Trying full SDL Video...";
        CSDLVideo *sdlVideoPtr = new CSDLVideo(mVidConfig);
        mpVideoEngine.reset(sdlVideoPtr);
        retval = mpVideoEngine->init();
    }

    if(!retval)
    {
        log << "failed." << CLogFile::endl;
        log << "Error creating a Video subsystem for the app." << CLogFile::endl;
        return false;
    }

    // Now SDL will tell if the bpp works or changes it, if not supported.
    // this value is updated here!
    retval &= mpVideoEngine->createSurfaces();
    m_mustrefresh = true;

    return retval;
}

void CVideoDriver::stop()
{
    if(mpVideoEngine)
    {
        mpVideoEngine = nullptr;
        SDL_Quit();
    }
}

void CVideoDriver::setFilter(const VidFilter value)
{
    mVidConfig.m_ScaleXFilter = value;
} // 1 means no filter

void CVideoDriver::setZoom(short value)
{
    mVidConfig.Zoom = value;
}

void CVideoDriver::setScaleType(bool IsNormal)
{ mVidConfig.m_normal_scale = IsNormal; }

// defines the scroll-buffer that is used for blitScrollSurface(). It's normally passed by a CMap Object
// it might have when a level-map is loaded.
void CVideoDriver::updateScrollBuffer(const Sint16 SBufferX, const Sint16 SBufferY)
{
    mpVideoEngine->updateScrollBuffers(SBufferX, SBufferY);
}

void CVideoDriver::blitScrollSurfaces() // This is only for tiles
                                        // Therefore the name should be changed
{
    mpVideoEngine->blitScrollSurfaces(gameSfc());
}

void CVideoDriver::collectSurfaces()
{
    mpVideoEngine->processVideoTasks();
    mpVideoEngine->collectSurfaces();
}

void CVideoDriver::clearSurfaces()
{
    mpVideoEngine->clearSurfaces();
}

void CVideoDriver::addVideoTask(const std::function<void()> &task)
{
    mpVideoEngine->addVideoTask(task);
}

void CVideoDriver::updateDisplay()
{
    if(mVidConfig.mHorizBorders > 0)
    {
        mpVideoEngine->drawHorizBorders();
    }

    mpVideoEngine->scaleAndFilter();
    mpVideoEngine->transformScreenToDisplay();

}

void CVideoDriver::saveCameraBounds(st_camera_bounds &CameraBounds)
{
    int &left = CameraBounds.left;
    int &up = CameraBounds.up;
    int &right = CameraBounds.right;
    int &down = CameraBounds.down;
    int &speed = CameraBounds.speed;

    if (left > right) {
        const int halfWidth = (left - right) / 2;
        left -= halfWidth;
        right += halfWidth;
        if (left > right)
            left--;
    }

    if (up > down) {
        const int halfHeight = (up - down) / 2;
        up -= halfHeight;
        down += halfHeight;
        if (up > down)
            up--;
    }

    bool invalid_value = (left < 50) || (up < 50) || (right < 50) || (down < 50)
            || (speed < 1) || (left > 270) || (up > 150) || (right > 270)
            || (down > 150) || (speed > 50);

    st_camera_bounds &cam = mVidConfig.m_CameraBounds;

    if (invalid_value)
        cam.reset();
    else
        cam = CameraBounds;
}

CVidConfig &CVideoDriver::getVidConfig()
{
    return mVidConfig;
}


void CVideoDriver::isFullscreen(const bool value) {
    mVidConfig.mFullscreen = value;
}

bool CVideoDriver::getFullscreen() {
    return mVidConfig.mFullscreen;
}

unsigned int CVideoDriver::getWidth() const {
    return mVidConfig.mDisplayRect.dim.x;
}

unsigned int CVideoDriver::getHeight() const {
    return mVidConfig.mDisplayRect.dim.y;
}

unsigned short CVideoDriver::getDepth() const
{
    return 32;
}

void CVideoDriver::resetScrollBuffers()
{
    mpVideoEngine->resetScrollBuffers();
}

std::vector<GsScrollSurface> &CVideoDriver::getScrollSurfaceVec()
{
    return mpVideoEngine->getScrollSurfaceVec();
}

GsScrollSurface &CVideoDriver::getScrollSurface(const int idx)
{
    return mpVideoEngine->getScrollSfc(idx);
}

st_camera_bounds &CVideoDriver::getCameraBounds()
{
    return mVidConfig.m_CameraBounds;
}

////
//// Drawing stuff related Stuff
////
SDL_Rect CVideoDriver::toBlitRect(const GsRect<float> &rect)
{
    GsRect<Uint16> GameRes = getGameResolution();
    GsRect<float> screenRect(0, 0, GameRes.dim.x, GameRes.dim.y);
    GsRect<float> RectDispCoordFloat = rect;

    // Limit the boundaries
    RectDispCoordFloat.pos.x = std::max(RectDispCoordFloat.pos.x, 0.0f);
    RectDispCoordFloat.pos.y = std::max(RectDispCoordFloat.pos.y, 0.0f);
    RectDispCoordFloat.dim.x = std::min(RectDispCoordFloat.dim.x, 1.0f);
    RectDispCoordFloat.dim.y = std::min(RectDispCoordFloat.dim.y, 1.0f);

    // Transform to the blit coordinates
    RectDispCoordFloat.transform(screenRect);

    GsRect<Uint16> RectDispCoord;
    RectDispCoord = RectDispCoordFloat;
    return RectDispCoord.SDLRect();
}


int CVideoDriver::getOptimalScaling()
{
    // Calculate a proper font size for the resolution
    const int desiredScaleW = getBlitSurface()->h/200;
    const int desiredScaleH = getBlitSurface()->w/320;

    int desiredScale = std::min(desiredScaleW, desiredScaleH);
    desiredScale = std::max(desiredScale, 1);
    desiredScale = std::min(desiredScale, 4);

    return desiredScale;
}



SDL_Surface *CVideoDriver::convertThroughBlitSfc( SDL_Surface *sfc )
{
    SDL_Surface *blit = mpVideoEngine->getBlitSurface();

    // Not sure why yet,
    // but we need to call this twice in order to have correct masks for blits

    SDL_Surface *newSfc  = SDL_ConvertSurface(sfc, blit->format, 0 );
    SDL_Surface *newSfc2 = SDL_ConvertSurface(newSfc, blit->format, 0 );

    SDL_FreeSurface(newSfc);

    return newSfc2;
}


