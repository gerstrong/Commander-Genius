/*
 * CVideoDriver.h
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 *
 *  The driver is in charge of driving the card the card. This means it will do everthing
 *  needed to get the stuff rendered properly. It will also do some checks wether it's possible to accomplish something
 *  or not.
 */

#ifndef CVIDEODRIVER_H_
#define CVIDEODRIVER_H_

#include <base/Singleton.h>
#include <base/video/CVideoEngine.h>
#include <base/GsEvent.h>
#include <graphics/GsTextureElem.h>
#include <graphics/GsTexture.h>
#include <queue>
#include <set>

extern bool useScrollTexels;

#ifdef USE_OPENGL
    #include <base/video/COpenGL.h>
#endif


#define gVideoDriver CVideoDriver::get()

#include <SDL.h>
#include <iostream>
#include <list>
#include <memory>
#include <tuple>
#include <functional>

class CVideoDriver : public GsSingleton<CVideoDriver>
{
public:
    ~CVideoDriver();

    bool init();

    bool applyMode();
    SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );


    void addVideoTask(const std::function<void()> &task);

    /**
     * @brief getGameResFactors return scale dimesions relative to the original resolution
     * @return the resolution scale
     */
    GsRect<float> getGameResFactors()
    {
        const float wFac = float(getGameResolution().dim.x)/320.0f;
        const float hFac = float(getGameResolution().dim.y)/200.0f;

        GsRect<float> scaledDownRect(0.0f, 0.0f, wFac, hFac);
        return scaledDownRect;
    }

    /**
     * @brief setNativeResolution   Setup the native resolution.
     *                              In the end it is the unscaled surface of
     *                              using within the application
     * @param dispRect              Resolution as Rect
     * @param numScrollSfcs         Number of scroll buffered surfaces. Good for background scrolling.
     * @return true if all went good, otherwise false
     */
    bool setNativeResolution(const GsRect<Uint16> &dispRect,
                             const unsigned int numScrollSfcs = 2);

    void stop();
    bool start();
    void isFullscreen(bool value);

    void blitScrollSurfaces();
    void updateScrollBuffer(const Sint16 SBufferX, const Sint16 SBufferY);

    void collectSurfaces();
    void clearSurfaces();
    void updateDisplay();

    void setLightIntensity(const float intensity)
    {
        mpVideoEngine->setLightIntensity(intensity);
    }

    // Drawing related stuff
    SDL_Rect toBlitRect(const GsRect<float> &rect);

    /**
     * \description This function saves the given camera bounds. It is usually called
     * 				by a menu item.
     * \param		CameraBounds	The Camera Bound configuration as input.
     * 								Those might be corrected depending on how the user
     * 								chose them.
     */
    void saveCameraBounds(st_camera_bounds &CameraBounds);

    CVidConfig &getVidConfig();

    short getZoomValue()
    {
        return static_cast<short>(mVidConfig.Zoom);
    }


    bool getFullscreen();
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned short getDepth() const;


    GsRect<Uint16> getGameResolution() const
    {
        return GsRect<Uint16>(getBlitSurface()->clip_rect);
    }

    /**
     * @brief getOptimalScaling Returns an optimal scaling value depending on the blit resolution given
     * @return the optimal scaling to use
     */
    int getOptimalScaling();

    SDL_Surface *getBlitSurface() const { return mpVideoEngine->getBlitSurface(); }

    GsSurface &gameSfc() const { return mpVideoEngine->gameSfc(); }

    GsSurface &getScreenSurface() const { return mpVideoEngine->getScreenSurface(); }

    SDL_Surface *convertThroughBlitSfc( SDL_Surface *sfc );

    bool isOpenGL(void) { return mVidConfig.mOpengl; }

    bool isVsync(void) { return mVidConfig.mVSync; }

    void resetScrollBuffers();

    std::vector<GsScrollSurface> &getScrollSurfaceVec();
    GsScrollSurface &getScrollSurface(const int idx);

    std::set<std::string> getResolutionStrSet();
    std::set<std::string> getGameResStrSet();
    std::set<std::string> getAspectStrSet();

    void setVidConfig(const CVidConfig& VidConf);
    void setMode(const int width, const int height);
    void setMode(const GsRect<Uint16>& res);
    void setFilter(const VidFilter value);
    void setScaleType(bool IsNormal);
    void setZoom(short vale);
#ifdef USE_OPENGL
    void enableOpenGL(bool value) { mVidConfig.mOpengl = value; }
#else
    void enableOpenGL(bool ) { mVidConfig.mOpengl = false; }
#endif

    void setRenderQuality(const CVidConfig::RenderQuality &value)
    { mVidConfig.mRenderScQuality = value; }

    /*
     * \brief Check whether this resolution is okay to be used or needs some adjustments if possible.
     * 		  It could be that, the screen dim can be used but instead of 32bpp 16bpp. This function
     * 		  will check and adapt it to the resolution your supports
     * \param resolution The resolution structure of the mode it is desired to be used
     * \param SDL uses some flags like Fullscreen or HW Acceleration, those need to be passed in order to verify
     *        the mode properly.
     * \return nothing. It does not return because it always adapts the resolution to some working mode.
     *         If video cannot be opened at all, another function of LibSDL will find that out.
     */
    void verifyResolution( GsVec2D<Uint16>& resolution, const int flags );
    GsVec2D<Uint16> getResolution() const { return *mResolutionPos; }

    bool initResolutionList();

    void setAspectCorrection(const int w, const int h)
    {
      mVidConfig.mAspectCorrection.dim.x = w;
      mVidConfig.mAspectCorrection.dim.y = h;
    }

#ifdef USE_VIRTUALPAD
    bool VGamePadEnabled() { return mVidConfig.mVPad;   }
#endif

    bool getRefreshSignal() { return m_mustrefresh; }
    void setRefreshSignal(const bool value) { m_mustrefresh = value;  }

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Renderer& getRendererRef()
    {
        return *(mpVideoEngine->renderer);
    }

    SDL_Renderer* Renderer()
    {
        return mpVideoEngine->renderer;
    }

    void setClearColor(const GsColor &color)
    {
        mpVideoEngine->setClearColor(color);
    }


    /**
     * @brief addTextureToRender add texture ptr to the that will be renderered
     * @param texturePtr    pointer to the SDL Texture
     */
    void addTextureRefToVirtPadRender(GsTexture& textureRef)
    {
        std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> >
                triple( textureRef, {0, 0, 0, 0}, {0, 0, 0, 0 } );

        mpVideoEngine->mRenderTexturePtrs.push(triple);
    }


    void addTextureRefToVirtPadRender(GsTextureElem& textureElemRef);

    /**
     * @brief addTextureRefToVirtPadRender add texture ptr to the that will be renderered
     * @param textureRef    pointer to the SDL Texture
     * @param dstRect       Rect where to put this texture on
     */
    void addTextureRefToVirtPadRender(GsTexture& textureRef, const GsRect<float> &dstRect);

    void pushTextureRef(GsTexture& textureRef,
                        const SDL_Rect &src_rect,
                        const SDL_Rect &dst_rect);


#endif

    st_camera_bounds &getCameraBounds();

    std::unique_ptr<CVideoEngine> mpVideoEngine;

    std::set< GsVec2D<Uint16> > mResolutionSet;
    std::set< GsVec2D<Uint16> > :: iterator mResolutionPos;

    std::set< GsVec2D<Uint16> > mGameReslist;

    std::set<std::string> mAspectSet;        

private:

    CVidConfig mVidConfig;
    bool m_mustrefresh = false;
    bool mSDLImageInUse = false;

};
#endif /* CVIDEODRIVER_H_ */
