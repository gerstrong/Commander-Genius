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

#ifndef COPENGL_H_
#define COPENGL_H_

#include <SDL.h>
#include <SDL_opengl.h>
#include "../scale2x/scalebit.h"

class COpenGL {
public:
	COpenGL();
	virtual ~COpenGL();

	void render(void);
	bool initGL(unsigned Width, unsigned Height, unsigned char Depth,
				 				GLint oglfilter, unsigned char scalex);

	void setSurface(SDL_Surface *blitsurface);

private:
	SDL_Surface	*m_blitsurface;
	char*		m_opengl_buffer;	// Used for internal ScaleX filtering
	unsigned char	m_Depth;
	unsigned char	m_ScaleX;
	unsigned int	m_texparam;
};

#endif /* COPENGL_H_ */
