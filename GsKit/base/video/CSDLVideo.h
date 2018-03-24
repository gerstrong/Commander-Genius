/*
 * CSDLVideo.h
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 *
 *  This class
 */

#ifndef CSDLVIDEO_H_
#define CSDLVIDEO_H_

#include <base/video/CVideoEngine.h>
#include <graphics/GsSurface.h>

class CSDLVideo : public CVideoEngine
{
public:
	CSDLVideo(const CVidConfig& VidConfig);

    bool init();

	bool resizeDisplayScreen(const GsRect<Uint16>& newDim);
	void collectSurfaces();
	void clearSurfaces();

    void setLightIntensity(const float intensity);

    void scaleNoFilter(	SDL_Surface *srcSfc,
                        const SDL_Rect *scrrect,
                        SDL_Surface *dstSfc,
                        const SDL_Rect *dstrect );


    bool initOverlaySurface( const Uint16 width,
                             const Uint16 height );

    void transformScreenToDisplay();

    void hackIt();

private:

    GsSurface mOverlaySurface;
};

#endif /* CSDLVIDEO_H_ */
