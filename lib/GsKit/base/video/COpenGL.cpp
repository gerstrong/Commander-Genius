/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#include "../Configurator.h"

#ifdef USE_OPENGL

#include "COpenGL.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h> // for CInput::renderOverlay
#include "graphics/GsGraphics.h"
#include <base/GsLogging.h>


// gamerect is the base resolution for the game which is scaled with the filter
// depending on what resolution has been chosen, it is mostly 320x200 or 320x240
COpenGL::COpenGL(const CVidConfig &VidConfig) :
CVideoEngine(VidConfig),
m_texparam(GL_TEXTURE_2D),
m_GameScaleDim(m_VidConfig.m_GameRect.w*m_VidConfig.m_ScaleXFilter,
				m_VidConfig.m_GameRect.h*m_VidConfig.m_ScaleXFilter),
m_GamePOTScaleDim(getPowerOfTwo(m_GameScaleDim.w), getPowerOfTwo(m_GameScaleDim.h))
{}

void COpenGL::setUpViewPort(const GsRect<Uint16> &newDim)
{
    const float width = static_cast<float>(newDim.w);
    const float height = static_cast<float>(newDim.h);
    const float ypos = static_cast<float>(newDim.y);
    const float xpos = static_cast<float>(newDim.x);

    glViewport(xpos, ypos, width, height);
}

bool COpenGL::resizeDisplayScreen(const GsRect<Uint16>& newDim)
{
	// NOTE: try not to free the last SDL_Surface of the screen, this is freed automatically by SDL		  
    const int w = m_VidConfig.mAspectCorrection.w;
    const int h = m_VidConfig.mAspectCorrection.h;

    // Render a black surface which cleans the screen, in case there already is some content in the screen    
    if(mpScreenSfc->empty())
    {
        clearSurfaces();
        transformScreenToDisplay();
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)        
  
    updateAspectRect(newDim, w, h);

    setUpViewPort(mAspectCorrectionRect);

#else
    mDisplaySfc.setPtr(SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode ));

    if (mDisplaySfc.empty())
	{
		gLogging.textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

    updateAspectRect(newDim, w, h);

    setUpViewPort(mAspectCorrectionRect);

#endif


	return true;
}

void COpenGL::collectSurfaces()
{

}

void COpenGL::clearSurfaces()
{
    mpScreenSfc->fillRGB(0,0,0);
}


static void createTexture(GLuint& tex, GLint oglfilter, GLsizei potwidth, GLsizei potheight, bool withAlpha = false)
{
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

bool COpenGL::init()
{
	CVideoEngine::init();	

#if SDL_VERSION_ATLEAST(2, 0, 0)    

    Uint32 flags = SDL_WINDOW_OPENGL;

    if(m_VidConfig.Fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    else
        flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    window = SDL_CreateWindow("Commander Genius",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_VidConfig.m_DisplayRect.w,
                              m_VidConfig.m_DisplayRect.h,
                              flags);

    glcontext = SDL_GL_CreateContext(window);
    
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

    // Setup the view port for the first time
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	glViewport(0, 0, 480, 320);
#else
    setUpViewPort(mAspectCorrectionRect);
#endif

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
	glEnable(m_texparam);
    

    GLint oglfilter = GL_LINEAR;
    if(m_VidConfig.mRenderScQuality == "nearest")
        const GLint oglfilter = GL_NEAREST;


    createTexture(m_texture, oglfilter, m_GamePOTScaleDim.w, m_GamePOTScaleDim.h);
	
	if(m_VidConfig.m_ScaleXFilter <= 1)
	{	// In that case we can do a texture based rendering
		createTexture(m_texFX, oglfilter, m_GamePOTScaleDim.w, m_GamePOTScaleDim.h, true);
	}
#else // not SDL 2.0
	// Setup the view port for the first time
    setUpViewPort(mAspectCorrectionRect);

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
	glEnable(m_texparam);
	
	createTexture(m_texture, oglfilter, m_GamePOTScaleDim.w, m_GamePOTScaleDim.h);
	
    //if(m_VidConfig.m_ScaleXFilter <= 1)
	{ // In that case we can do a texture based rendering
	  createTexture(m_texFX, oglfilter, m_GamePOTScaleDim.w, m_GamePOTScaleDim.h, true);
	} 

#endif
	
	// If there were any errors
	int error;
	error = glGetError();
	if( error != GL_NO_ERROR)
	{
		gLogging.ftextOut("OpenGL Init(): %d<br>",error);
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_DestroyWindow(window);
        SDL_GL_DeleteContext(glcontext);
#endif
		return false;
	}

    gLogging.ftextOut("OpenGL Init(): Interface successfully opened!<br>");


    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
	return true;
}

static void renderTexture(GLuint texture, bool withAlpha = false)
{
	if(withAlpha)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_ONE, GL_ONE);
	}
	else
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBlendFunc(GL_ONE, GL_ZERO);
	}
	
	//Finally draw the arrays of the surface.
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void COpenGL::loadSurface(GLuint texture, SDL_Surface* surface)
{
	glBindTexture (m_texparam, texture);
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

//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else

    mpScreenSfc->lock();

	// First apply the conventional filter if any (GameScreen -> FilteredScreen)
    /*if(m_VidConfig.m_ScaleXFilter > 1) //ScaleX
	{
        SDL_LockSurface(mpScreenSfc.get());
		SDL_LockSurface(surface);
		Scaler.scaleUp(FilteredSurface, surface, SCALEX, aspectCorrectionRect);
        SDL_UnlockSurface(surface);
    }*/
    //else // Otherwise, blit to a POT-sized surface
    /*{
		// While blitting, no involved surface should be locked.
        BlitSurface(surface, NULL, FilteredSurface, NULL);

        SDL_LockSurface(FilteredSurface);
    }*/

	glTexImage2D(m_texparam, 0, internalFormat,
                mpScreenSfc->width(),
                mpScreenSfc->height(),
				0, externalFormat,
                GL_UNSIGNED_BYTE, mpScreenSfcgetSDLSurface()->pixels);

    mpScreenSfc->unlock();
//#endif
}

void COpenGL::transformScreenToDisplay()
{
	glEnable(GL_TEXTURE_2D);
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

    loadSurface(m_texture, *mpScreenSfc.getSDLSurface());
	renderTexture(m_texture);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	gInput.renderOverlay();

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_GL_SwapWindow(window);
#else
    SDL_GL_SwapBuffers();
#endif

    clearSurfaces();
}

void COpenGL::setLightIntensity(const float intensity)
{
    float r,g,b;

    glDisable(GL_LIGHT0);

    // TODO: This a temporary solution for just now.
    // We should pass the different color channels as parameters in future
    r = intensity;
    g = intensity;
    b = intensity;

    GLfloat light_ambient[] = { r, g, b, 1.0 };
    GLfloat light_diffuse[] = { r, g, b, 1.0 };
    GLfloat light_specular[] = { r, g, b, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT0);
}

#endif // USE_OPENGL
