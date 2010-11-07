/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#ifdef USE_OPENGL

#include "COpenGL.h"
#include "CVideoDriver.h"
#include "CLogFile.h"
#include "CInput.h" // for CInput::renderOverlay

/**
 * This function calculates an equivalent value near by the power of two. That is needed so we support POT Textures
 */
Uint16 getPowerOfTwo(Uint16 value)
{
	Uint16 output = 1;
	while (output<value)
		output <<= 1;
	return output;
}

// gamerect is the base resolution for the game which is scaled with the filter
// depending on what resolution has been chosen, it is mostly 320x200 or 320x240
COpenGL::COpenGL(Uint16 Width, Uint16 Height, unsigned char Depth,
				unsigned char scalex,SDL_Rect &gamestdrect) :
mp_blitsurface(NULL),
mp_fgsurface(NULL),
mp_fxsurface(NULL),
m_opengl_buffer(NULL),
m_Depth(Depth),
m_ScaleX(scalex),
m_texparam(GL_TEXTURE_2D),
m_aspectratio(Width/Height),
m_GamePOTBaseDim(getPowerOfTwo(gamestdrect.w),
				getPowerOfTwo(gamestdrect.h)),
m_GamePOTVideoDim(getPowerOfTwo(Width),
				getPowerOfTwo(Height))
{
}

static void createTexture(GLuint& tex, int oglfilter, GLsizei potwidth, GLsizei potheight, bool withAlpha = false) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, oglfilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, oglfilter);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(withAlpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potwidth, potheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, potwidth, potheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

bool COpenGL::initGL(GLint oglfilter)
{
	if(m_Depth != 32)
	{
		// TODO: I know, this is an issue, but I need to investigate, how pixels in SDL are stored when using
		// 16 bit depth copy it correctly to the OGL Texture
		g_pLogFile->textOut("Sorry, but OpenGL with 16 bpp is not supported! Please switch to 32 bpp mode!");
		return false;
	}

	// Calculate the proper viewport for any resolution
	float base_width = g_pVideoDriver->getGameResolution().w;
	float base_height = g_pVideoDriver->getGameResolution().h;

	float scale_width = (float)(g_pVideoDriver->getWidth())/base_width;
	float scale_height = (float)(g_pVideoDriver->getHeight())/base_height;

	float width = ((float)m_GamePOTBaseDim.w)*scale_width;
	float height = ((float)m_GamePOTBaseDim.h)*scale_height;
	float ypos = (base_height-m_GamePOTBaseDim.h)*scale_height;
	float xpos = 0.0f; // Not needed because the x-axis of ogl and sdl_surfaces are the same.

	// strange constants here; 225 seems good for pc. 200 is better for iphone
	// the size is the same as the texture buffers
	glViewport(xpos, ypos, width, height);

	// Set clear colour
	glClearColor(0,0,0,0);
	
	// Set projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)	// TODO: dont check for iphone but for opengles
#define glOrtho glOrthof
#endif
	glOrtho( 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f );

	// Now Initialize modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    /*Using the standard OpenGL API for specifying a 2D texture
    image: glTexImage2D, glSubTexImage2D, glCopyTexImage2D,
    and glCopySubTexImage2D.  The target for these commands is
    GL_TEXTURE_RECTANGLE_ARB though.

    This is similar to how the texture cube map functionality uses the 2D
    texture image specification API though with its own texture target.

    The texture target GL_TEXTURE_RECTANGLE_ARB should also
    be used for glGetTexImage, glGetTexLevelParameteriv, and
    glGetTexLevelParameterfv.*/

	// Enable Texture loading for the blit screen
	/*glEnable (m_texparam);
	glBindTexture(m_texparam, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(m_texparam, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (m_texparam, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexParameteri (m_texparam, GL_TEXTURE_MAG_FILTER, oglfilter);
	glTexParameteri (m_texparam, GL_TEXTURE_MIN_FILTER, oglfilter);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);*/

	glEnable(m_texparam);

	createTexture(m_texture, oglfilter, m_GamePOTVideoDim.w, m_GamePOTVideoDim.h);
	
	if(m_ScaleX > 1)
	{
		m_opengl_buffer = new char[m_GamePOTVideoDim.w*m_GamePOTVideoDim.h*m_ScaleX*m_Depth];
	}
	else
	{	// In that case we can do a texture based rendering
		createTexture(m_texFX, oglfilter, m_GamePOTVideoDim.w, m_GamePOTVideoDim.h, true);
		createTexture(m_texFG, oglfilter, m_GamePOTVideoDim.w, m_GamePOTVideoDim.h, true);
		m_opengl_buffer = NULL;
	}
	
	// If there were any errors
	int error;
	error = glGetError();
	if( error != GL_NO_ERROR)
	{
		g_pLogFile->ftextOut("OpenGL Init(): %d<br>",error);
		return false;
	}
	else
	{
		g_pLogFile->ftextOut("OpenGL Init(): Interface succesfully opened!<br>");
	}
	
	return true;
}

void COpenGL::setBlitSurface(SDL_Surface *blitsurface)
{	mp_blitsurface = blitsurface; }

void COpenGL::setFGSurface(SDL_Surface *fgsurface)
{	mp_fgsurface = fgsurface; }
void COpenGL::setFXSurface(SDL_Surface *fxsurface)
{	mp_fxsurface = fxsurface; }


/*static void loadSurface(GLuint texture, SDL_Surface* surface) {
	glBindTexture(GL_TEXTURE_2D, texture);
	LockSurface(surface);
	if(surface->format->BitsPerPixel == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	else {
		// we assume RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	}
	UnlockSurface(surface);
}*/

void COpenGL::reloadFX(SDL_Surface* surf) {
	loadSurface(m_texFX, surf);
}

void COpenGL::reloadFG(SDL_Surface* surf) {
	loadSurface(m_texFG, surf);
}

static void renderTexture(GLuint texture, bool withAlpha = false) {
	
	// Set up an array of values to use as the sprite vertices.
	GLfloat vertices[] =
	{
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	// Set up an array of values for the texture coordinates.
	GLfloat texcoords[] =
	{
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	//Render the vertices by pointing to the arrays.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	glEnable(GL_BLEND);
	if(withAlpha)
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc(GL_ONE, GL_ZERO );

	glBindTexture (GL_TEXTURE_2D, texture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Set the texture parameters to use a linear filter when minifying.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//Finally draw the arrays.
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
}

void COpenGL::loadSurface(GLuint texture, SDL_Surface* surface)
{
	glBindTexture (m_texparam, texture);
	LockSurface(surface);
	GLint internalFormat, externalFormat;
#if !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR) // iPhone always used 32 bits; also GL_BGR is not defined
	if(surface->format->BitsPerPixel == 24)
	{
		internalFormat = GL_RGB;
		externalFormat = GL_BGR;
	}
	else
#endif
	{	// we assume RGBA
		internalFormat = GL_RGBA;
		externalFormat = GL_BGRA;
	}

	if(m_ScaleX > 1) //ScaleX
	{
		unsigned m_src_slice = m_GamePOTBaseDim.w*surface->format->BytesPerPixel;
		unsigned m_dst_slice = m_ScaleX*m_src_slice;


		scale(m_ScaleX, m_opengl_buffer, m_dst_slice, surface->pixels,
				m_src_slice, surface->format->BytesPerPixel,
				m_GamePOTBaseDim.w, m_GamePOTBaseDim.h);

			glTexImage2D(m_texparam, 0, internalFormat, m_GamePOTBaseDim.w*m_ScaleX, m_GamePOTBaseDim.h*m_ScaleX,
														0, externalFormat, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else
	{
			glTexImage2D(m_texparam, 0, internalFormat, m_GamePOTBaseDim.w, m_GamePOTBaseDim.h,
														0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
	}

	UnlockSurface(surface);
}

void COpenGL::render()
{
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	loadSurface(m_texture, mp_blitsurface);
	renderTexture(m_texture);

	if(mp_fgsurface)
	{
		reloadFG(mp_fgsurface);
		renderTexture(m_texFG, true);
	}

	if(mp_fxsurface)
	{
		reloadFX(mp_fgsurface);
		renderTexture(m_texFG, true);
	}

	g_pInput->renderOverlay();

	SDL_GL_SwapBuffers();

}

COpenGL::~COpenGL() {
	if(m_opengl_buffer)
		delete[] m_opengl_buffer;
	m_opengl_buffer = NULL;
}

#endif // USE_OPENGL
