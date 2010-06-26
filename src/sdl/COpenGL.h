/*
 * COpenGL.h
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 *
 *  This class is used for passing all the OpenGL function through the
 *  library in order to render on accelerated video hardware
 *
 */

#ifndef __CG_COPENGL_H__
#define __CG_COPENGL_H__

#include <SDL.h>

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <SDL_opengl.h>
#endif

#include "../scale2x/scalebit.h"

// Structure used for dimensions
struct stDims{
	Uint16 w,h;
	stDims(Uint16 width, Uint16 height) :
		w(width),h(height){};
};

Uint16 getPowerOfTwo(Uint16 value);

class COpenGL {
public:
	COpenGL(Uint16 Width, Uint16 Height, unsigned char Depth,
			unsigned char scalex,SDL_Rect &gamestdrect);

	void render();
	bool initGL(GLint oglfilter, float aspect);

	void setSurface(SDL_Surface *blitsurface);
	void reloadBG(SDL_Surface* surf);
	void reloadFG(SDL_Surface* surf);

	virtual ~COpenGL();

private:
	SDL_Surface	*m_blitsurface;
	char*		m_opengl_buffer;	// Used for internal ScaleX filtering

	unsigned char	m_Depth;
	unsigned char	m_ScaleX;
	unsigned int	m_texparam;

	float	m_aspectratio;

	stDims m_GamePOTBaseDim;
	stDims m_GamePOTVideoDim;

	GLuint	m_texture;
};

#endif
