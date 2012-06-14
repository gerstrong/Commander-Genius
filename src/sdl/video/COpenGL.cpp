/*
 * COpenGL.cpp
 *
 *  Created on: 04.06.2009
 *      Author: gerstrong
 */
#ifdef USE_OPENGL

#include "COpenGL.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h" // for CInput::renderOverlay
#include "graphics/CGfxEngine.h"
#include "CLogFile.h"
#include "graphics/PerSurfaceAlpha.h"

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
COpenGL::COpenGL(const CVidConfig &VidConfig) :
CVideoEngine(VidConfig),
m_texparam(GL_TEXTURE_2D),
m_aspectratio(m_VidConfig.m_DisplayRect.aspectRatio()),
m_GamePOTBaseDim(getPowerOfTwo(m_VidConfig.m_GameRect.w),
				getPowerOfTwo(m_VidConfig.m_GameRect.h)),
m_GamePOTVideoDim(getPowerOfTwo(m_VidConfig.m_DisplayRect.w),
				getPowerOfTwo(m_VidConfig.m_DisplayRect.h))
{}

void COpenGL::setUpViewPort(const CRect<Uint16> &GameRes, const CRect<Uint16> &newDim)
{
	// Calculate the proper viewport for any resolution
	float base_width = GameRes.w;
	float base_height = GameRes.h;

	float scale_width = (float)(newDim.w)/base_width;
	float scale_height = (float)(newDim.h)/base_height;

	float width = ((float)m_GamePOTBaseDim.w)*scale_width;
	float height = ((float)m_GamePOTBaseDim.h)*scale_height;
	float ypos = (base_height-m_GamePOTBaseDim.h)*scale_height;
	float xpos = 0.0f; // Not needed because the x-axis of ogl and sdl_surfaces are the same.

	// strange constants here; 225 seems good for pc. 200 is better for iphone
	// the size is the same as the texture buffers
	glViewport(xpos, ypos, width, height);
}

bool COpenGL::resizeDisplayScreen(const CRect<Uint16>& newDim)
{
	// NOTE: try not to free the last SDL_Surface of the screen, this is freed automatically by SDL
	screen = SDL_SetVideoMode( newDim.w, newDim.h, 32, m_Mode );

	if (!screen)
	{
		g_pLogFile->textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

	if(FilteredSurface)
	{
		Scaler.setDynamicFactor( float(FilteredSurface->w)/float(screen->w),
								 float(FilteredSurface->h)/float(screen->h));

		setUpViewPort(g_pVideoDriver->getGameResolution(), newDim);
	}


	return true;
}


bool COpenGL::createSurfaces()
{
	// This function creates the surfaces which are needed for the game.
	const CRect<Uint16> gamerect = m_VidConfig.m_GameRect;
    ScrollSurface = createSurface( "ScrollSurface", true,
								  512, 512,
								  32,
								  m_Mode, screen->format );

    g_pLogFile->textOut("Blitsurface = creatergbsurface<br>");

    BlitSurface = createSurface( "BlitSurface", true,
    		getPowerOfTwo(gamerect.w),
    		getPowerOfTwo(gamerect.h),
    		32,
    		m_Mode, screen->format );

    g_pLogFile->textOut("FilteredSurface = creatergbsurface<br>");

	FilteredSurface = createSurface( "FilteredSurface", true,
				BlitSurface->w*m_VidConfig.m_ScaleXFilter,
				BlitSurface->h*m_VidConfig.m_ScaleXFilter,
				32,
				m_Mode, screen->format );

	m_dst_slice = FilteredSurface->w*screen->format->BytesPerPixel;

	if(m_VidConfig.m_ScaleXFilter == 1)
	{
		FXSurface = createSurface( "FXSurface", true,
						getPowerOfTwo(gamerect.w),
						getPowerOfTwo(gamerect.h),
						32,
						m_Mode, screen->format );
	}
	else
	{
		FXSurface = createSurface( "FXSurface", false,
				gamerect.w,
				gamerect.h,
				32,
				m_Mode, screen->format );

		//Set surface alpha
	}

	g_pGfxEngine->Palette.setFXSurface( FXSurface );

	Scaler.setFilterFactor(m_VidConfig.m_ScaleXFilter);
	Scaler.setDynamicFactor( float(FilteredSurface->w)/float(screen->w),
							 float(FilteredSurface->h)/float(screen->h));


	return true;
}

void COpenGL::collectSurfaces()
{

}

void COpenGL::clearSurfaces()
{
	SDL_FillRect(FXSurface,NULL, 0x0);

	SDL_FillRect(BlitSurface,NULL, 0x0);
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
	const GLint oglfilter = m_VidConfig.m_opengl_filter;

	// Setup the view port for the first time
	setUpViewPort(g_pVideoDriver->getGameResolution(), g_pVideoDriver->getResolution());

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

	createTexture(m_texture, oglfilter, m_GamePOTVideoDim.w, m_GamePOTVideoDim.h);
	
	if(m_VidConfig.m_ScaleXFilter <= 1)
	{	// In that case we can do a texture based rendering
		createTexture(m_texFX, oglfilter, m_GamePOTVideoDim.w, m_GamePOTVideoDim.h, true);
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

void COpenGL::reloadFX(SDL_Surface* surf)
{
	loadSurface(m_texFX, surf);
}

static void renderTexture(GLuint texture, bool withAlpha = false) {
	
	if(withAlpha)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE);
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
	SDL_LockSurface(surface);
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

	if(m_VidConfig.m_ScaleXFilter > 1) //ScaleX
	{
		//const unsigned src_slice = m_GamePOTBaseDim.w*surface->format->BytesPerPixel;
		//const unsigned dst_slice = m_VidConfig.m_ScaleXFilter*src_slice;

			// First apply the conventional filter if any (GameScreen -> FilteredScreen)
			Scaler.scaleUp(FilteredSurface, surface, SCALEX);

			SDL_LockSurface(FilteredSurface);

			glTexImage2D(m_texparam, 0, internalFormat,
						FilteredSurface->w,
						FilteredSurface->h,
						0, externalFormat,
						GL_UNSIGNED_BYTE, FilteredSurface->pixels);

			SDL_UnlockSurface(FilteredSurface);
	}
	else
	{
			glTexImage2D(m_texparam, 0, internalFormat, m_GamePOTBaseDim.w, m_GamePOTBaseDim.h,
														0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
	}

	SDL_UnlockSurface(surface);
}

void COpenGL::updateScreen()
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

	if(m_VidConfig.m_ScaleXFilter > 1)
	{
		if(getPerSurfaceAlpha(FXSurface))
			SDL_BlitSurface(FXSurface, NULL, BlitSurface, NULL);
	}

	loadSurface(m_texture, BlitSurface);
	renderTexture(m_texture);


	if(m_VidConfig.m_ScaleXFilter == 1)
	{
		if(FXSurface && getPerSurfaceAlpha(FXSurface))
		{
			reloadFX(FXSurface);
			renderTexture(m_texFX, true);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	g_pInput->renderOverlay();

	SDL_GL_SwapBuffers();
}

#endif // USE_OPENGL
