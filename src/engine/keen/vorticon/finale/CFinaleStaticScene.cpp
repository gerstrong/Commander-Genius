/*
 * CFinaleStaticScene.cpp
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#include "CFinaleStaticScene.h"
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include "sdl/audio/Audio.h"
#include "graphics/GsGraphics.h"


CFinaleStaticScene::CFinaleStaticScene(const std::string &game_path, const std::string &scene_file):
m_mustclose(false),
m_timer(0)
{        
    SDL_Rect rect;

    rect.x = 0;     rect.y = 0;
    rect.w = 320;   rect.h = 200;

    mSceneBmp.createSurface(0, rect, gGraphics.Palette.m_Palette);

    if( finale_draw( mSceneBmp.getSDLSurface(), scene_file, game_path) )
	{
        //mSceneBmp = GsBitmap(sceneSfc);

        GsWeakSurface blit( gVideoDriver.getBlitSurface() );

        const GsRect<Uint16> scaleRect(0, 0, blit.width(), blit.height());

        mSceneBmp.scaleTo(scaleRect);
    }
	else
	{
		m_mustclose = true;
    }
}


void CFinaleStaticScene::showBitmapAt(const std::string &bitmapname, Uint16 from_count, Uint16 to_count, Uint16 x, Uint16 y)
{
    const int scaleFactor = mSceneBmp.getWidth() / 320;

	bitmap_structure bmp_struct;

    bmp_struct.bitmap = *gGraphics.getBitmapFromStr(bitmapname);
    bmp_struct.dest_rect.x = x*scaleFactor;
    bmp_struct.dest_rect.y = y*scaleFactor;
    bmp_struct.dest_rect.w = bmp_struct.bitmap.getWidth();
    bmp_struct.dest_rect.h = bmp_struct.bitmap.getHeight();    

    GsRect<Uint16> bmpDim;
    bmpDim.w = bmp_struct.dest_rect.w*scaleFactor;
    bmpDim.h = bmp_struct.dest_rect.h*scaleFactor;
    bmpDim.x = 0; bmpDim.y = 0;

    bmp_struct.bitmap.scaleTo(bmpDim);

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
    if(!mSceneBmp.empty())
    {
        mSceneBmp.draw(0, 0);
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
