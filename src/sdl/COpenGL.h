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
	COpenGL(float Width, float Height, unsigned char Depth,
			unsigned char scalex,SDL_Rect &gamestdrect);

<<<<<<< HEAD
	void render();
	bool initGL(GLint oglfilter, float aspect);
=======
	void render(bool withFG);
	bool initGL(unsigned Width, unsigned Height, unsigned char Depth,
				GLint oglfilter, unsigned char scalex, bool aspect);
>>>>>>> b637f832d3c3074cf3ecb20c7851d8f7eb53a59f

	void setSurface(SDL_Surface *blitsurface);
	void reloadBG(SDL_Surface* surf);
	void reloadFG(SDL_Surface* surf);

	virtual ~COpenGL();

private:
	SDL_Surface	*m_blitsurface;
	char*		m_opengl_buffer;	// Used for internal ScaleX filtering
	unsigned short	m_Width;
	unsigned short	m_Height;
	unsigned char	m_Depth;
	unsigned char	m_ScaleX;
	unsigned int	m_texparam;
<<<<<<< HEAD
	float	m_aspectratio;
	SDL_Rect m_GameStdRect;
=======
	GLuint	m_texture;
	
	GLuint	m_texBG, m_texFG;
	
>>>>>>> b637f832d3c3074cf3ecb20c7851d8f7eb53a59f
};

#endif
