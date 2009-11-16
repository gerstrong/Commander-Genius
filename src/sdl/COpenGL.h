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

class COpenGL {
public:
	COpenGL();
	virtual ~COpenGL();

	void render(bool withFG);
	bool initGL(unsigned Width, unsigned Height, unsigned char Depth,
				GLint oglfilter, unsigned char scalex, bool aspect);

	void setSurface(SDL_Surface *blitsurface);
	void reloadBG(SDL_Surface* surf);
	void reloadFG(SDL_Surface* surf);

private:
	SDL_Surface	*m_blitsurface;
	char*		m_opengl_buffer;	// Used for internal ScaleX filtering
	unsigned char	m_Depth;
	unsigned char	m_ScaleX;
	unsigned int	m_texparam;
	GLuint	m_texture;
	
	GLuint	m_texBG, m_texFG;
	
};

#endif
