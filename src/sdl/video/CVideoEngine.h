/*
 * CVideoEngine.h
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 *
 *  This is a base class which will process the video settings on your system.
 *  CVideoDriver calls that Engine through an overloaded Extra class like COpenGL or CSDLVideo
 */

#ifndef CVIDEOENGINE_H_
#define CVIDEOENGINE_H_

class CVideoEngine {
public:

	virtual void updateScreen() = 0;

protected:
//	void scale2xnofilter(char *dest, char *src, short bbp);
//	void scale3xnofilter(char *dest, char *src, short bbp);
//	void scale4xnofilter(char *dest, char *src, short bbp);

};

#endif /* CVIDEOENGINE_H_ */
