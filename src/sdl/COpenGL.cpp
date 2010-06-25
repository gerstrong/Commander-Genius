/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#ifdef USE_OPENGL

#include "COpenGL.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"

// gamerect is the base resolution for the game which is scaled with the filter
// depending on what resolution has been chosen, it is mostly 320x200 or 320x240
COpenGL::COpenGL(float Width, float Height, unsigned char Depth,
				unsigned char scalex,SDL_Rect &gamestdrect) :
m_blitsurface(NULL),
m_opengl_buffer(NULL),
m_Width(Width),
m_Height(Height),
m_Depth(Depth),
m_ScaleX(scalex),
m_texparam(GL_TEXTURE_2D),
m_aspectratio(Width/Height),
m_GameStdRect(gamestdrect)
{}

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

bool COpenGL::initGL(GLint oglfilter, float aspect)
{
	// Set the proper resolution for OpenGL. Very important, when user changes the resolution
	if(aspect == 8.0f/5.0f)
	{
	if(m_aspectratio < 8.0f/5.0f)
		glViewport(0,(m_Height-((m_Width/320)*200))/2,m_Width, (m_Width/320)*200);
	else if(m_aspectratio > 8.0f/5.0f)
		glViewport((m_Width-((m_Height/200)*320))/2,0, (m_Height/200)*320, m_Height);
	else
		glViewport(0,0,m_Width, m_Height);
	}
	
	// Set clear colour
	glClearColor(0,0,0,0);
	
	// Set projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0 , 1, 1, 0 , -1 , 1 );
	
	// Now Initialize modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	// Enable Texture loading for the blit screen
	glEnable (m_texparam);
	glBindTexture(m_texparam, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexParameteri(m_texparam, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (m_texparam, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	glTexParameteri (m_texparam, GL_TEXTURE_MAG_FILTER, oglfilter);
	glTexParameteri (m_texparam, GL_TEXTURE_MIN_FILTER, oglfilter);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	
	if(m_ScaleX > 1)
		m_opengl_buffer = new char[m_GameStdRect.h*m_GameStdRect.w*m_ScaleX*m_Depth];
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

void COpenGL::render(void)
{
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);		// Clear The Screen
	
	LockSurface(m_blitsurface);
	
	if(m_ScaleX == 2) //Scale 2x
	{

		//unsigned m_dst_slice = m_Width*m_blitsurface->format->BytesPerPixel;
		unsigned m_dst_slice = m_GameStdRect.w*2*m_blitsurface->format->BytesPerPixel;
		unsigned m_src_slice = m_GameStdRect.w*m_blitsurface->format->BytesPerPixel;

		scale(m_ScaleX, m_opengl_buffer, m_dst_slice, m_blitsurface->pixels,
				m_src_slice, m_blitsurface->format->BytesPerPixel,
				m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(m_texparam, 0, GL_RGBA, m_GameStdRect.w<<1, m_GameStdRect.h<<1, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else if(m_ScaleX == 3) //Scale 3x
	{
		scale(m_ScaleX, m_opengl_buffer, (m_GameStdRect.w*3)<<2, m_blitsurface->pixels,
			  m_GameStdRect.w<<2, 4, m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w*3, m_GameStdRect.h*3, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else if(m_ScaleX == 4) //Scale 4x
	{
		scale(m_ScaleX, m_opengl_buffer, (m_GameStdRect.w<<2)<<2, m_blitsurface->pixels,
			  m_GameStdRect.w<<2, 4, m_GameStdRect.w, m_GameStdRect.h);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w<<2, m_GameStdRect.h<<2, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_opengl_buffer);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameStdRect.w, m_GameStdRect.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_blitsurface->pixels);
	}
	
	glBindTexture (GL_TEXTURE_2D, 1);
	
	glBegin (GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.0, 0.0, 0.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (1.0, 0.0, 0.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (1.0, 1.0, 0.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 1.0, 0.0);
	glEnd();
	
	UnlockSurface(m_blitsurface);
	
	// Reset (Position?)
	glLoadIdentity();
	
	SDL_GL_SwapBuffers();
}

COpenGL::~COpenGL() {
	if(m_opengl_buffer){ delete[] m_opengl_buffer; m_opengl_buffer = NULL; }
}

#endif
