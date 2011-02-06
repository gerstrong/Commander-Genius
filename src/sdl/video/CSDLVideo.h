/*
 * CSDLVideo.h
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 *
 *  This class
 */

#ifndef CSDLVIDEO_H_
#define CSDLVIDEO_H_

#include "sdl/video/CVideoEngine.h"

class CSDLVideo : public CVideoEngine {
public:
	CSDLVideo(const CVidConfig& VidConfig);

	void updateScreen();

private:

};

#endif /* CSDLVIDEO_H_ */
