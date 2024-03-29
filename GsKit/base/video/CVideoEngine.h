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


#include "CVidConfig.h"
#include <graphics/GsSurface.h>
#include <graphics/GsScrollsurface.h>
#include <graphics/GsTexture.h>
#include <base/interface/Color.h>
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include <functional>
#include <map>

#include <SDL.h>


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
	virtual ~CVideoEngine();


	virtual bool init();

    /**
     * @brief resizeDisplayScreen Resize the display screen so it fits
     *                            to the given Dimensions.
     * @param newDim Display screen dimensions to consider
     */
    virtual void resizeDisplayScreen(const GsRect<Uint16>& newDim) = 0;


    /**
     * @brief updateActiveArea Updates the mActiveAreaRect which is used when rendering the screen
     * @param displayRes The dimension of the window or resolution of the display when in Fullscreen
     * @param aspWidth wished aspect width
     * @param aspHeight wished aspect height
     */
    void updateActiveArea(const GsRect<Uint16>& displayRes, const GsVec2D<int> asp);

    virtual void transformScreenToDisplay() = 0;
	virtual void shutdown();


    bool createSurfaces();
    bool createSurfaces(const GsRect<Uint16> &gamerect,
                        const unsigned int numScrollSfcs);

    virtual bool initOverlaySurface(const Uint16 width,
                                    const Uint16 height ) { return true; }

	void fetchStartScreenPixelPtrs(Uint8 *&ScreenPtr, Uint8 *&BlitPtr, unsigned int &width, unsigned int &height);
	virtual void collectSurfaces() = 0;
	virtual void clearSurfaces() = 0;

    void processVideoTasks();

    void stop();

    virtual void setLightIntensity(const float intensity) = 0;

    SDL_Surface *getBlitSurface()
    {
        return mGameSfc.getSDLSurface();
    }

    GsSurface &gameSfc() { return mGameSfc; }

    GsSurface &getScreenSurface() { return *mpScreenSfc; }

    GsRect<Uint16> getActiveAreaRect() const;

    void drawHorizBorders();

    void scaleAndFilter();

    void blitScrollSurfaces(GsWeakSurface &blitSfc);

    void resetScrollBuffers();

    void updateScrollBuffers(const Sint16 SBufferX, const Sint16 SBufferY);

    GsScrollSurface& getScrollSfc(const int idx);



#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_GLContext glcontext;

    /**
     * @brief mRenderTextures queue that manages pointers to textures defined somewhere in the to be rendered.
     *        This functionality only works with SDL 2.0 so but might get extended later
     * @note  Do not manage the memory of these pointers. The owers of the textures have to do that!
     */
    std::queue< std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> > > mRenderTexturePtrs;

#endif

    GsRect<int> mRelativeVisGameArea;
    GsRect<int> mRelativeBlendVisGameArea;

    void setClearColor(const GsColor &color)
    {
        mClearColor = color;
    }


    std::vector<GsScrollSurface> &getScrollSurfaceVec();

    typedef std::tuple< GsTexture&, const GsRect<Uint16>, const GsRect<Uint16> > targetedTexture;

    //std::vector< std::vector< targetedTexture > > mScrollbufferTextures;
    std::vector< std::vector<targetedTexture> > mScrollbufferTextures;


    void addVideoTask(const std::function<void()> &task);

protected:

    bool allocateScrollSurfaces(const unsigned int numSfc);


#if SDL_VERSION_ATLEAST(2, 0, 0)
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> mpMainScreenTexture;
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

    // Through a pointer called mpScreenSfc it is setup how the stuff will be rendered on screen
    GsSurface *mpScreenSfc = nullptr;


    std::vector<GsScrollSurface> mScrollSurfaceVec;       	// Squared scroll buffer

	const CVidConfig &m_VidConfig;


    unsigned int m_Mode = 0;

    // A sub-rectangle where an aspect-corrected frame is displayed.
    // This is already scaled up. Nothing outside this area should be drawn.
    GsRect<Uint16> mActiveAreaRect;


    /**
     * @brief mClearColor   Color used for clearing the Screen
     *                      0 means black, but through a config the use might change this
     */

    GsColor mClearColor = { 0x00, 0x00, 0x00};

    std::queue< std::function<void()> > mVideoTasks;
};

#endif /* CVIDEOENGINE_H_ */

