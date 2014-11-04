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

#include "CVidConfig.h"
#include <graphics/GsSurface.h>
#include <memory>
#include <queue>


struct SDL_Surface_Deleter
{
    void operator()(SDL_Surface* sfc)
    {
        SDL_FreeSurface(sfc);
    }
};


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

    //SDL_Surface *createSurface(std::string name, bool alpha, int width, int height, int bpp, int mode);
    bool createSurfaces();
    bool createSurfaces(const GsRect<Uint16> &gamerect);

    virtual bool initOverlaySurface(const Uint16 width,
                                    const Uint16 height ) { return true; }

	void fetchStartScreenPixelPtrs(Uint8 *&ScreenPtr, Uint8 *&BlitPtr, unsigned int &width, unsigned int &height);
	virtual void collectSurfaces() = 0;
	virtual void clearSurfaces() = 0;
	void blitScrollSurface();
	void stop();

    virtual void setLightIntensity(const float intensity) = 0;

    SDL_Surface *getBlitSurface() { return mGameSfc.getSDLSurface(); }

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

    /**
     * @brief mRenderTextures queue that manages pointers to textures defined somewhere in the to be rendered.
     *        This functionality only works with SDL 2.0 so but might get extended later
     * @note  Do not manage the memory of these pointers. The owers of the textures have to do that!
     */
    std::queue< std::tuple< SDL_Texture*, const GsRect<Uint16>, const GsRect<Uint16> > > mRenderTexturePtrs;

#endif

    GsRect<int> mRelativeVisGameArea;

protected:


#if SDL_VERSION_ATLEAST(2, 0, 0)
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> mpSDLScreenTexture;
#else

    // it is what you see on your monitor in the end in your window or on fullscreen
    GsWeakSurface mDisplaySfc;      // the actual video memory/window
#endif

    // Where all the game is rendered
    GsSurface mGameSfc;

    // Were the game is transformed to. This is used for internal transformations
    // before it is shown on the display.
    // it is not required in case no transformations should performed.
    GsSurface mFilteredSfc;

    // So the transformation goes in two steps
    // mGameSfc -> mFilteredSfc -> mDisplaySurface or Texture or similar
    // Sometimes you have this situation:
    // mGameSfc -> mDisplaySurface or Texture or similar
    // This case happens when you don't want to use any software filtering within the game
    // before you pass it to the view.

    // Through a pointer called mpScreenSfc it is set how the stuff will be rendered on screen
    GsSurface *mpScreenSfc;


    SDL_Surface *ScrollSurface;       	// Squared scroll buffer

	const CVidConfig &m_VidConfig;

	Sint16 mSbufferx;
	Sint16 mSbuffery;

	unsigned int m_Mode;

	// A sub-rectangle where an aspect-corrected frame is displayed
    GsRect<Uint16> mAspectCorrectionRect;
};

#endif /* CVIDEOENGINE_H_ */

