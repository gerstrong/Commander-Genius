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

#include "sdl/video/CVideoEngine.h"

class CSDLVideo : public CVideoEngine {
public:
	CSDLVideo(const CVidConfig& VidConfig, Sint16 *&p_sbufferx, Sint16 *&p_sbuffery);

	bool resizeDisplayScreen(const CRect<Uint16>& newDim);
	bool createSurfaces();
	void collectSurfaces();
	void clearSurfaces();

	void scaleNoFilter( 	SDL_Surface *srcSfc,
									const SDL_Rect *scrrect,
									SDL_Surface *dstSfc,
									const SDL_Rect *dstrect );


	void updateScreen();

private:

};

#endif /* CSDLVIDEO_H_ */
