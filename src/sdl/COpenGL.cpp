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
#include "CInput.h" // for CInput::renderOverlay

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

	glEnable(GL_TEXTURE_2D);
	createTexture(m_texture, oglfilter);
	

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

static void renderTexture(GLuint texture, bool withAlpha = false) {

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

	// strange constants here; 225 seems good for pc. 200 is better for iphone
	// the size is the same as the texture buffers
	//glViewport(0,200,512, 256);
	//glLoadIdentity();
	//glOrthof(0, 1, 0, 1, 0, 1);
	//glOrthof(0, 1, 0, 1, 0, 1);


	// Set up an array of values to use as the sprite vertices.
	/*GLfloat vertices[] =
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
		glBlendFunc(GL_ONE, GL_ZERO /*GL_SRC_COLOR*//*);

	glBindTexture (GL_TEXTURE_2D, texture);
<<<<<<< HEAD

=======
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
>>>>>>> 3d8ea00f311ea30b9eaf3aebb3b5118ba45f38f4
	// Set the texture parameters to use a linear filter when minifying.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Finally draw the arrays.
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);*/
}

void COpenGL::render(void)
{
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//renderTexture(m_texBG);
	
	LockSurface(m_blitsurface);

	glBindTexture (GL_TEXTURE_2D, m_texture);
	
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


	UnlockSurface(m_blitsurface);

	renderTexture(m_texture);

	//if(withFG)
		//renderTexture(m_texFG, true);
	
	g_pInput->renderOverlay();
	
	SDL_GL_SwapBuffers();

}

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

/*void COpenGL::reloadBG(SDL_Surface* surf) {
	loadSurface(m_texBG, surf);
}

void COpenGL::reloadFG(SDL_Surface* surf) {
	loadSurface(m_texFG, surf);
}*/

COpenGL::~COpenGL() {
	if(m_opengl_buffer) delete[] m_opengl_buffer; m_opengl_buffer = NULL; }
#endif
