/*
 * CFinaleStaticScene.cpp
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#include "CFinaleStaticScene.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "graphics/CGfxEngine.h"
#include "sdl/extensions.h"

CFinaleStaticScene::CFinaleStaticScene(const std::string &game_path, const std::string &scene_file):
m_mustclose(false),
m_timer(0)
{
	const SDL_Rect resrect =  g_pVideoDriver->getGameResolution().SDLRect();
	const Uint32 flags = g_pVideoDriver->getBlitSurface()->flags;

    mpSceneSurface.reset(SDL_CreateRGBSurface( flags, resrect.w, resrect.h, 8, 0, 0, 0, 0),
                         &SDL_FreeSurface);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(mpSceneSurface->format->palette, g_pGfxEngine->Palette.m_Palette, 0, 255);
    SDL_SetColorKey(mpSceneSurface.get(), SDL_TRUE, COLORKEY);
#else
    SDL_SetColors(mpSceneSurface.get(), Palette, 0, 255);
    SDL_SetColorKey(mpSceneSurface.get(), SDL_SRCCOLORKEY, COLORKEY);
#endif

    if( finale_draw( mpSceneSurface.get(), scene_file, game_path) )
	{
#if SDL_VERSION_ATLEAST(2, 0, 0)
        
#else
        mpSceneSurface.reset(g_pVideoDriver->convertThroughBlitSfc(mpSceneSurface.get()), &SDL_FreeSurface);
#endif
    }
	else
	{
		m_mustclose = true;
    }
}


void CFinaleStaticScene::showBitmapAt(const std::string &bitmapname, Uint16 from_count, Uint16 to_count, Uint16 x, Uint16 y)
{
	bitmap_structure bmp_struct;
    bmp_struct.bitmap = *g_pGfxEngine->getBitmap(bitmapname);
	bmp_struct.dest_rect.x = x;
	bmp_struct.dest_rect.y = y;
    bmp_struct.dest_rect.w = bmp_struct.bitmap.getWidth();
    bmp_struct.dest_rect.h = bmp_struct.bitmap.getHeight();
	bmp_struct.from_count = from_count;
	bmp_struct.to_count = to_count;
	m_BitmapVector.push_back(bmp_struct);
}

void CFinaleStaticScene::ponder()
{    
    if(m_timer >= 0)
        m_timer--;
}

void CFinaleStaticScene::render(const int step)
{
    if(mpSceneSurface)
    {
        SDL_BlitSurface(mpSceneSurface.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr);
    }

    if(m_timer <= 0)
    {
        // Draw any requested extra bitmap
        for( std::vector<bitmap_structure>::iterator i=m_BitmapVector.begin() ;
             i!=m_BitmapVector.end() ; i++ )
        {
            if( step >= i->from_count && step <= i->to_count ) // Is it within this interval?
            { // show it!
                i->bitmap.draw(i->dest_rect.x, i->dest_rect.y);
            }
        }
    }
}
