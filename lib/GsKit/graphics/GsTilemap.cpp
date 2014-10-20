/*
 * GsTilemap.cpp
 *
 *  Created on: 29.08.2009
 *      Author: gerstrong
 */

//#include "fileio/CTileLoader.h"
#include <fileio/ResourceMgmt.h>
#include <base/video/CVideoDriver.h>
#include <SDL_image.h>
#include <base/utils/FindFile.h>
#include "GsTilemap.h"
#include "GsPalette.h"
#include <base/GsLogging.h>
#include <stdlib.h>

GsTilemap::GsTilemap() :
m_Tilesurface(NULL),
m_EmptyBackgroundTile(143),
m_numtiles(0),
m_pbasesize(0),
m_column(0)
{ }

bool GsTilemap::CreateSurface(SDL_Color *Palette, Uint32 Flags,
				Uint16 numtiles, Uint16 pbasesize, Uint16 column)
{
	m_numtiles = numtiles;
	m_pbasesize = pbasesize;
	m_column = column;
    m_Tilesurface = SDL_CreateRGBSurface(Flags, m_column<<m_pbasesize,
                                        (m_numtiles/m_column)<<m_pbasesize, 8, 0, 0, 0, 0);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(m_Tilesurface->format->palette, Palette, 0, 255);
    SDL_SetColorKey(m_Tilesurface, SDL_TRUE, COLORKEY);
#else
    SDL_SetColors(m_Tilesurface, Palette, 0, 255);
	SDL_SetColorKey(m_Tilesurface, SDL_SRCCOLORKEY, COLORKEY);
#endif
	
	return ( m_Tilesurface != NULL );
}

std::string exts[] = { "png", "bmp", "tif", "jpg" };

bool GsTilemap::loadHiresTile( const std::string& filename, const std::string& path )
{  
	std::string fullfilename;  
	// Cycle through possible filename extensions, when more formats are supported
	for( auto &ext : exts )
	{
	    fullfilename = filename + "." + ext;
	    fullfilename = getResourceFilename(fullfilename, path, false);  
	    if(!fullfilename.empty())
	      break;
	}	
	  
	if(!IsFileAvailable(fullfilename))
		return false;

    // For some odd readon, SDL 1.2 image seems to be broken. For now, it is disabled.
#if SDL_VERSION_ATLEAST(2, 0, 0)
	if(m_Tilesurface)
	{	  	  
        SDL_Surface *temp_surface = IMG_Load(GetFullFileName(fullfilename).c_str());
		if(temp_surface)
		{
			SDL_FreeSurface(m_Tilesurface);
			m_Tilesurface = temp_surface;
			return true;
		}
		else
		{
		  gLogging.textOut(RED, "IMG_Load: %s\n", IMG_GetError());
		  gLogging.textOut(RED, "IMG_Load: CG will ignore those images\n");
		}
	}
#else
    // TODO: Code for older SDL 1.2
#endif
	
	return false;
}

bool GsTilemap::optimizeSurface()
{
	if(m_Tilesurface)
	{
		SDL_Surface *temp_surface;
//#if SDL_VERSION_ATLEAST(2, 0, 0)
        
//#else
        temp_surface = gVideoDriver.convertThroughBlitSfc(m_Tilesurface);
//#endif
		SDL_FreeSurface(m_Tilesurface);
		m_Tilesurface = temp_surface;
		return true;
	}
	else
		return false;
}

///////////////////////////////////
///// Initialization Routines /////
///////////////////////////////////
SDL_Surface *GsTilemap::getSDLSurface()
{
	return m_Tilesurface;
}



////////////////////////////
/////    Getters       /////
////////////////////////////


int GsTilemap::getDimension()
{
	return (1<<m_pbasesize);
}

////////////////////////////
///// Drawing Routines /////
////////////////////////////

#ifdef DEBUG_COLLISION

#define CSF    9
#define TILE_S			4
#define STC (CSF-TILE_S)

void FillSlopeRect(SDL_Surface *dst, const SDL_Rect dst_rect, Uint32 color, Sint8 blocked)
{
	/**
	 * So far only used in Galaxy. This is the code for sloped tiles downside
	 * This is performed when Keen walks into a sloped tile
	 *
	 * 0	Fall through		1	Flat
	 * 2	Top -> Middle		3	Middle -> bottom
	 * 4	Top -> bottom		5	Middle -> top
	 * 6	Bottom -> middle	7	Bottom -> top
	 * 8	Unused			9	Deadly, can't land on in God mode
	 */

	int yb1, yb2;

	if(!blocked)
		return;

	if( blocked == 2 )
		yb1 = 0,	yb2 = 256;
	else if( blocked == 3 )
		yb1 = 256,	yb2 = 512;
	else if( blocked == 4 )
		yb1 = 0,	yb2 = 512;
	else if( blocked == 5 )
		yb1 = 256,	yb2 = 0;
	else if( blocked == 6 )
		yb1 = 512,	yb2 = 256;
	else if( blocked == 7 )
		yb1 = 512,	yb2 = 0;
	else
		yb1 = 0, yb2 = 0;

	SDL_Rect sloperect = dst_rect;
	sloperect.w = 1;

	if( blocked > 1 )
	{
		for( int c = 0 ; c<512 ; c++ )
		{
			sloperect.x = dst_rect.x + (c>>STC);
			sloperect.y = dst_rect.y + ((yb1+c*(yb2-yb1)/512)>>STC);
			sloperect.h = dst_rect.h - sloperect.y;
			SDL_FillRect( dst, &sloperect, 0xFFFFFFFF);
		}
	}

	SDL_Rect line1_rect = dst_rect;
	line1_rect.h = 1;
	SDL_FillRect( dst, &line1_rect, 0xFF545454);
	SDL_Rect line2_rect = dst_rect;
	line2_rect.w = 1;
	SDL_FillRect( dst, &line2_rect, 0xFF545454);
}
#endif

void GsTilemap::drawTile(SDL_Surface *dst, int x, int y, Uint16 t)
{
    SDL_Rect src_rect, dst_rect;
	src_rect.x = (t%m_column)<<m_pbasesize;
	src_rect.y = (t/m_column)<<m_pbasesize;
	const int size = 1<<m_pbasesize;
	src_rect.w = src_rect.h = dst_rect.w = dst_rect.h = size;

	dst_rect.x = x;		dst_rect.y = y;

    if( dst_rect.y + src_rect.h > dst->h )
    {
        src_rect.h = dst->h - dst_rect.y;
    }

    if( dst_rect.x + src_rect.w > dst->w )
    {
        src_rect.w = dst->w - dst_rect.x;
    }

    BlitSurface(m_Tilesurface, &src_rect, dst, &dst_rect);

#ifdef DEBUG_COLLISION
	//std::vector<CTileProperties> &TileProp = g_pBehaviorEngine->getTileProperties(1);
	//FillSlopeRect(dst, dst_rect, 0xFFFFFFFF, TileProp[t].bup);
#endif

}

void GsTilemap::applyGalaxyMask()
{
    const SDL_PixelFormat *format = m_Tilesurface->format;
    const Uint32 maskColor = SDL_MapRGB(format, 0x0, 0xFF, 0xFF);
    SDL_SetColorKey(m_Tilesurface, SDL_TRUE, maskColor);

    SDL_LockSurface(m_Tilesurface);

    // Pointer of start pixel
    Uint8 *pxStart = static_cast<Uint8*>(m_Tilesurface->pixels);

    // From 0 to half width for every row ...
    int midRow = m_Tilesurface->w/2;


    for( int y=0 ; y<m_Tilesurface->h ; y++ )
    {
        Uint8 *pxRow = pxStart + y*m_Tilesurface->pitch;
        for( int x=0 ; x<midRow ; x++ )
        {
            Uint8 *px = pxRow + x*format->BytesPerPixel;
            Uint8 *pxMask = px + midRow*format->BytesPerPixel;

            const Uint32 pix = *((Uint32*)(px));
            const Uint32 mask = *((Uint32*)(pxMask));

            // Get the mask part
            Uint8 mask_r, mask_g, mask_b;
            SDL_GetRGB(mask, format, &mask_r, &mask_g, &mask_b);

            // Get the color
            Uint8 r, g, b;
            SDL_GetRGB(pix, format, &r, &g, &b);

            // Calculate the new alpha, which will do the transparency and even allow translucency
            const Uint8 alpha = 255-(( (mask_r<<16) + (mask_g<<8) + mask_b ) >> 16);

            Uint32 *newColorPtr = (Uint32*)(px);
            //*newColorPtr = SDL_MapRGBA( format, r, g, b, alpha );

            if(alpha < 128)
            {
                *newColorPtr = maskColor;
            }
        }
    }

    SDL_UnlockSurface(m_Tilesurface);
}

GsTilemap::~GsTilemap() 
{
	if(m_Tilesurface) 
	    SDL_FreeSurface(m_Tilesurface);
}
