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

#ifndef CG_COPENGL_H
#define CG_COPENGL_H

#ifdef USE_OPENGL

#include <SDL.h>

#include <SDL_opengl.h>

#include <base/video/CVideoEngine.h>

// Structure used for dimensions
struct stDims{
	Uint16 w,h;
    stDims(Uint16 width,
           Uint16 height) :
        w(width),h(height){}
};

Uint16 getPowerOfTwo(Uint16 value);

class COpenGL : public CVideoEngine
{
public:
	COpenGL(const CVidConfig &VidConfig);

    void setUpViewPort(const GsRect<Uint16> &viewRect);
    void resizeDisplayScreen(const GsRect<Uint16>& newRect);
	void loadSurface(GLuint texture, SDL_Surface* surface);
	void collectSurfaces();
	void clearSurfaces();

    void setLightIntensity(const float intensity);

    void transformScreenToDisplay();
	bool init();

private:
	unsigned int	m_texparam;

	stDims m_GameScaleDim, m_GamePOTScaleDim;

	GLuint	m_texture;
	GLuint	m_texFX;
};

#endif // USE_OPENGL

#endif // CG_COPENGL_H
