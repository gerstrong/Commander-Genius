/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#include "COpenGL.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"

#define GAME_STD_WIDTH            320
#define GAME_STD_HEIGHT           200

COpenGL::COpenGL() {
	m_blitsurface = NULL;
	m_Depth = 4; // 32-bit colour depth
}

COpenGL::~COpenGL() {
	if(m_opengl_buffer){ delete[] m_opengl_buffer; m_opengl_buffer = NULL; }
}

static void createTexture(GLuint& tex, int oglfilter, bool withAlpha = false) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, oglfilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, oglfilter);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(withAlpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

bool COpenGL::initGL(unsigned Width, unsigned Height, unsigned char Depth,
									GLint oglfilter, unsigned char scalex, bool aspect)
{
	m_Depth = Depth;
	m_ScaleX = scalex;
	//m_texparam = GL_TEXTURE_RECTANGLE_ARB;
	m_texparam = GL_TEXTURE_2D;

	// Set the proper resolution for OpenGL. Very important, when user changes the resolution
	if(aspect)
		glViewport(0,(Height-(Height*200)/240)/2,Width, (Height*200)/240);
	else
		glViewport(0,0,Height, Width);

	// Set clear colour
	glClearColor(0,0,0,0);

	// Set projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrthof( 0.0f , 1.0f, 1.0f, 0.0f , -1.0f , 1.0f );

	// Now Initialize modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Enable Texture loading for the blit screen
	/*  uint textureID;

	  glEnable (GL_TEXTURE_2D);
	  glGenTextures(1, &textureID);
	  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
*/

    /*Using the standard OpenGL API for specifying a 2D texture
    image: glTexImage2D, glSubTexImage2D, glCopyTexImage2D,
    and glCopySubTexImage2D.  The target for these commands is
    GL_TEXTURE_RECTANGLE_ARB though.

    This is similar to how the texture cube map functionality uses the 2D
    texture image specification API though with its own texture target.

    The texture target GL_TEXTURE_RECTANGLE_ARB should also
    be used for glGetTexImage, glGetTexLevelParameteriv, and
    glGetTexLevelParameterfv.*/


	glEnable(GL_TEXTURE_2D);
	createTexture(m_texture, oglfilter);
	createTexture(m_texBG, oglfilter);
	createTexture(m_texFG, oglfilter, true);
	
	if(scalex > 1)
		m_opengl_buffer = new char[GAME_STD_HEIGHT*GAME_STD_WIDTH*scalex*Depth];
	else
		m_opengl_buffer = NULL;

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

void COpenGL::setSurface(SDL_Surface *blitsurface)
{	m_blitsurface = blitsurface; }

static void loadSurface(GLuint texture, SDL_Surface* surface) {
	glBindTexture(GL_TEXTURE_2D, texture);
	LockSurface(surface);
	if(surface->format->BitsPerPixel == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	else {
		// we assume RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);		
	}
	UnlockSurface(surface);
}

void COpenGL::reloadBG(SDL_Surface* surf) {
	loadSurface(m_texBG, surf);
}

void COpenGL::reloadFG(SDL_Surface* surf) {
	loadSurface(m_texFG, surf);	
}

static void renderTexture(GLuint texture, bool withAlpha = false) {
	glViewport(0,200,512, 256);
	glLoadIdentity();
	//glOrthof(0, 1, 0, 1, 0, 1);
	//glOrthof(0, 1, 0, 1, 0, 1);
	
	
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc(GL_ONE, GL_ZERO /*GL_SRC_COLOR*/);
	
	glBindTexture (GL_TEXTURE_2D, texture);
	
	// Set the texture parameters to use a linear filter when minifying.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//Finally draw the arrays.
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_BLEND);
	
}

void COpenGL::render(bool withFG)
{	
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	renderTexture(m_texBG);
	
	   LockSurface(m_blitsurface);

	glBindTexture (GL_TEXTURE_2D, m_texture);
	
	   if(m_ScaleX == 2) //Scale 2x
	   {
		   scale(2, m_opengl_buffer, (GAME_STD_WIDTH<<1)*(m_Depth>>3), m_blitsurface->pixels,
			     GAME_STD_WIDTH*(m_Depth>>3), (m_Depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);

		   glTexImage2D(m_texparam, 0, GL_RGBA, GAME_STD_WIDTH<<1, GAME_STD_HEIGHT<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
		   //glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, GAME_STD_WIDTH<<1, GAME_STD_HEIGHT<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
		   //glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, GAME_STD_WIDTH<<1, GAME_STD_HEIGHT<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	   }
	   else if(m_ScaleX == 3) //Scale 3x
	   {
		   scale(3, m_opengl_buffer, (GAME_STD_WIDTH*3)*(m_Depth>>3), m_blitsurface->pixels,
				   GAME_STD_WIDTH*(m_Depth>>3), (m_Depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);

		   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GAME_STD_WIDTH*3, GAME_STD_HEIGHT*3, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	   }
	   else if(m_ScaleX == 4) //Scale 4x
	   {
		   scale(4, m_opengl_buffer, (GAME_STD_WIDTH<<2)*(m_Depth>>3), m_blitsurface->pixels,
				   GAME_STD_WIDTH*(m_Depth>>3), (m_Depth>>3), GAME_STD_WIDTH, GAME_STD_HEIGHT);

		   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GAME_STD_WIDTH<<2, GAME_STD_HEIGHT<<2, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	   }
	   else
		   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, m_blitsurface->pixels);

	UnlockSurface(m_blitsurface);

	renderTexture(m_texture);

	if(withFG)
		renderTexture(m_texFG, true);
	
	SDL_GL_SwapBuffers();
}
