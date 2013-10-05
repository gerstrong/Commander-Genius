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
m_count(0),
m_timer(0)
{
	const SDL_Rect resrect =  g_pVideoDriver->getGameResolution().SDLRect();
	const Uint32 flags = g_pVideoDriver->getBlitSurface()->flags;

	mpSceneSurface.reset(SDL_CreateRGBSurface( flags, resrect.w, resrect.h, 8, 0, 0, 0, 0), &SDL_FreeSurface);
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceColorMod( mpSceneSurface.get(), g_pGfxEngine->Palette.m_Palette->r, g_pGfxEngine->Palette.m_Palette->g, g_pGfxEngine->Palette.m_Palette->b);
#else
    SDL_SetColors( mpSceneSurface.get(), g_pGfxEngine->Palette.m_Palette, 0, 255);
#endif


	if( finale_draw( mpSceneSurface.get(), scene_file, game_path) )
	{
//#if SDL_VERSION_ATLEAST(2, 0, 0)
        
//#else
        mpSceneSurface.reset(g_pVideoDriver->convertThroughBlitSfc(mpSceneSurface.get()), &SDL_FreeSurface);
//#endif
	}
	else
	{
		m_mustclose = true;
	}

    SDL_FillRect(mpSceneSurface.get(),nullptr, SDL_MapRGB(mpSceneSurface.get()->format,255,0,255));
}


void CFinaleStaticScene::showBitmapAt(const std::string &bitmapname, Uint16 from_count, Uint16 to_count, Uint16 x, Uint16 y)
{
	bitmap_structure bmp_struct;
	bmp_struct.p_bitmap = g_pGfxEngine->getBitmap(bitmapname);
	bmp_struct.dest_rect.x = x;
	bmp_struct.dest_rect.y = y;
	bmp_struct.dest_rect.w = bmp_struct.p_bitmap->getWidth();
	bmp_struct.dest_rect.h = bmp_struct.p_bitmap->getHeight();
	bmp_struct.from_count = from_count;
	bmp_struct.to_count = to_count;
	m_BitmapVector.push_back(bmp_struct);
}

void CFinaleStaticScene::ponder()
{    
    if(m_timer >= 0)
        m_timer--;
}

void CFinaleStaticScene::render()
{
    if(mpSceneSurface)
    {
        SDL_BlitSurface(mpSceneSurface.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr);
    }

    if(m_timer <= 0)
    {
        // Draw any requested Bitmap
        for( std::vector<bitmap_structure>::iterator i=m_BitmapVector.begin() ;
             i!=m_BitmapVector.end() ; i++ )
        {
            if( m_count >= i->from_count && m_count <= i->to_count ) // Is it within this interval?
            { // show it!
                i->p_bitmap->draw(i->dest_rect.x, i->dest_rect.y);
            }
        }
    }
}
