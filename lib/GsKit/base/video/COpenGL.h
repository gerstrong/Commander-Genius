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

#ifdef USE_OPENGL

#include <SDL.h>

//#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
//#include <OpenGLES/ES1/gl.h>
//#include <OpenGLES/ES1/glext.h>
//#else
#include <SDL_opengl.h>
//#endif

#include <base/video/CVideoEngine.h>

// Structure used for dimensions
struct stDims{
	Uint16 w,h;
	stDims(Uint16 width, Uint16 height) :
		w(width),h(height){};
};

Uint16 getPowerOfTwo(Uint16 value);

class COpenGL : public CVideoEngine
{
public:
	COpenGL(const CVidConfig &VidConfig);

	void setUpViewPort(const GsRect<Uint16> &newDim);
	bool resizeDisplayScreen(const GsRect<Uint16>& newDim);
	void loadSurface(GLuint texture, SDL_Surface* surface);
	void collectSurfaces();
	void clearSurfaces();

    void setLightIntensity(const float intensity);

    void transformScreenToDisplay();
	bool init();

private:
	unsigned int	m_texparam;

    //float	m_aspectratio;

	stDims m_GameScaleDim, m_GamePOTScaleDim;

	GLuint	m_texture;
	GLuint	m_texFX;
};

#endif

#endif
