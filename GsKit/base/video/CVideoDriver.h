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
#include <graphics/GsTexture.h>
#include <queue>

#ifdef USE_OPENGL
    #include <base/video/COpenGL.h>
#endif


#define gVideoDriver CVideoDriver::get()

#include <SDL.h>
#include <iostream>
#include <list>
#include <memory>
#include <tuple>

class CVideoDriver : public GsSingleton<CVideoDriver>
{
public:
	CVideoDriver();
	~CVideoDriver();

    bool init();
	
	bool applyMode();
	SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );
	

    /**
     * @brief getGameResFactors return scale dimesions relative to the original resolution
     * @return the resolution scale
     */
    GsRect<float> getGameResFactors()
    {
        const float wFac = float(getGameResolution().w)/320.0f;
        const float hFac = float(getGameResolution().h)/200.0f;

        GsRect<float> scaledDownRect(0.0f, 0.0f, wFac, hFac);
        return scaledDownRect;
    }

    bool setNativeResolution(const GsRect<Uint16> &dispRect);

	bool start();
	void isFullscreen(bool value);

	void blitScrollSurface();
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
        return mVidConfig.Zoom;
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

    GsSurface &getScreenSurface() const { return mpVideoEngine->getScreenSurface(); }

    SDL_Surface *convertThroughBlitSfc( SDL_Surface *sfc );

	bool isOpenGL(void) { return mVidConfig.mOpengl; }

	SDL_Surface *getScrollSurface(void);

	void setVidConfig(const CVidConfig& VidConf);
	void setMode(int width, int height,int depth);
	void setMode(const GsRect<Uint16>& res);
	void setSpecialFXMode(bool SpecialFX);
    void setFilter(const filterOptionType value);
	void setScaleType(bool IsNormal);
	void setZoom(short vale);
#ifdef USE_OPENGL
	void enableOpenGL(bool value) { mVidConfig.mOpengl = value; }	
#else
    void enableOpenGL(bool value) { mVidConfig.mOpengl = false; }
#endif

    void setRenderQuality(const std::string &value) { mVidConfig.mRenderScQuality = value; }

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
	void verifyResolution( GsRect<Uint16>& resolution, const int flags );
	GsRect<Uint16>& getResolution() const { return *m_Resolution_pos; }

	void initResolutionList();

	void setAspectCorrection(const int w, const int h) 
	{ 
	  mVidConfig.mAspectCorrection.w = w; 
	  mVidConfig.mAspectCorrection.h = h; 
	}

	bool getSpecialFXConfig(void) { return mVidConfig.m_special_fx; }

    bool VGamePadEnabled() { return mVidConfig.mVPad;   }

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


    /**
     * @brief addTextureToRender add texture ptr to the that will be renderered
     * @param texturePtr    pointer to the SDL Texture
     */
    void addTextureRefToRender(GsTexture& textureRef)
    {
        std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> >
                triple( textureRef, {0, 0, 0, 0}, {0, 0, 0, 0 } );

        mpVideoEngine->mRenderTexturePtrs.push(triple);
    }


    /**
     * @brief addTextureRefToRender add texture ptr to the that will be renderered
     * @param textureRef    pointer to the SDL Texture
     * @param dstRect       Rect where to put this texture on
     */
    void addTextureRefToRender(GsTexture& textureRef, const GsRect<Uint16> &dstRect)
    {
        std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> >
                triple( textureRef, {0, 0, 0, 0}, dstRect );

        mpVideoEngine->mRenderTexturePtrs.push(triple);
    }


#endif

	st_camera_bounds &getCameraBounds();

	std::unique_ptr<CVideoEngine> mpVideoEngine;

	std::list< GsRect<Uint16> > m_Resolutionlist;
	std::list< GsRect<Uint16> > :: iterator m_Resolution_pos;


private:

	CVidConfig mVidConfig;
	bool m_mustrefresh;
	bool mSDLImageInUse;    

};
#endif /* CVIDEODRIVER_H_ */
