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
#include <SDL_opengl.h>
#include "../scale2x/scalebit.h"

class COpenGL {
public:
	COpenGL(SDL_Rect &gamestdrect);

	void render(void);
	bool initGL(float Width, float Height, unsigned char Depth,
				GLint oglfilter, unsigned char scalex, float aspect);

	void setSurface(SDL_Surface *blitsurface);

	virtual ~COpenGL();

private:
	SDL_Surface	*m_blitsurface;
	char*		m_opengl_buffer;	// Used for internal ScaleX filtering
	unsigned char	m_Depth;
	unsigned char	m_ScaleX;
	unsigned int	m_texparam;
	float	m_aspectratio;
	SDL_Rect m_GameStdRect;
};

#endif
