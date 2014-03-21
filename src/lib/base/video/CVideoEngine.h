/*
 * CVideoEngine.h
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 *
 *  This is a base class which will process the video settings on your system.
 *  CVideoDriver calls that Engine through an overloaded Extra class like COpenGL or CSDLVideo
 */

#ifndef CVIDEOENGINE_H_
#define CVIDEOENGINE_H_

#include <SDL.h>
#include <string>

#include "sdl/CVidConfig.h"
#include "sdl/extensions.h"
#include <memory>


#if SDL_VERSION_ATLEAST(2, 0, 0)
struct SDL_Texture_Deleter
{
    void operator()(SDL_Texture* sfc)
    {
        SDL_DestroyTexture(sfc);
    }
};
#endif





/**
 * This function calculates an equivalent value near by the power of two. That is needed so we support POT Textures
 */
Uint16 getPowerOfTwo(const Uint16 value);


class CVideoEngine
{
public:


    /**
     * @brief CVideoEngine
     * @param VidConfig Video Configuration flags used to setup the Video Engine
     */
	CVideoEngine(const CVidConfig& VidConfig);
	~CVideoEngine();


	virtual bool init();

	virtual bool resizeDisplayScreen(const GsRect<Uint16>& newDim) = 0;


    /**
     * @brief updateAspectRect Updates the mAspectCorrectionRect which is used when rendering the screen
     * @param displayRes The dimension of the window or resolution of the display when in Fullscreen
     * @param aspWidth wished aspect width
     * @param aspHeight wished aspect height
     */
    void updateAspectRect(const GsRect<Uint16>& displayRes, const int aspWidth, const int aspHeight);

    virtual void transformScreenToDisplay() = 0;
	virtual void shutdown();

    SDL_Surface *createSurface(std::string name, bool alpha, int width, int height, int bpp, int mode);
    bool createSurfaces();
    bool createSurfaces(const GsRect<Uint16> &gamerect);

    virtual bool initOverlaySurface( const bool useAlpha,
                             const Uint16 width,
                             const Uint16 height ) { return true; }

	void fetchStartScreenPixelPtrs(Uint8 *&ScreenPtr, Uint8 *&BlitPtr, unsigned int &width, unsigned int &height);
	virtual void collectSurfaces() = 0;
	virtual void clearSurfaces() = 0;
	void blitScrollSurface();
	void stop();

    virtual void setLightIntensity(const float intensity) = 0;

    SDL_Surface *getBlitSurface() { return mpGameSfc.get(); }

	SDL_Surface *getScrollSurface() { return ScrollSurface; }

    GsRect<Uint16> &getAspectCorrRect() { return mAspectCorrectionRect; }

	void resetScrollbuffer()
	{
		mSbufferx = 0;
		mSbuffery = 0;
	}

    void filterUp();

    inline void UpdateScrollBufX(const Sint16 SBufferX, const int drawMask)
    {		mSbufferx = SBufferX&drawMask;	}

    inline void UpdateScrollBufY(const Sint16 SBufferY, const int drawMask)
    {		mSbuffery = SBufferY&drawMask;	}

	void readScrollBuffer(Sint16 &x, Sint16 &y)
	{	x = mSbufferx; y = mSbuffery;}

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GLContext glcontext;
#endif

    GsRect<int> mRelativeVisGameArea;

protected:


#if SDL_VERSION_ATLEAST(2, 0, 0)
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> mpSdlTexture;
#else
    SDL_Surface *mDisplaySfc;                // the actual video memory/window
#endif

    std::shared_ptr<SDL_Surface> mpGameSfc;
    std::shared_ptr<SDL_Surface> mpScreenSfc;

    //SDL_Surface *BlitSurface;
    //SDL_Surface *FilteredSurface;
    SDL_Surface *ScrollSurface;       	// Squared scroll buffer

    //CScaler Scaler;

	const CVidConfig &m_VidConfig;

	Sint16 mSbufferx;
	Sint16 mSbuffery;

	// Those variables are used for the rendering process, so they don't need to be recalculated
	unsigned m_dst_slice, m_src_slice;

	unsigned int m_Mode;

	// A sub-rectangle where an aspect-corrected frame is displayed
    GsRect<Uint16> mAspectCorrectionRect;
};

#endif /* CVIDEOENGINE_H_ */

