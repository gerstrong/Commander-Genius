/*
 * CFinaleStaticScene.cpp
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#include "CFinaleStaticScene.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CTimer.h"
#include "../../graphics/CGfxEngine.h"
#include "../../funcdefs.h"

CFinaleStaticScene::CFinaleStaticScene(const std::string &game_path, const std::string &scene_file):
	mp_current_tb(NULL), m_mustclose(false), m_count(0)
{
	mp_SceneSurface = SDL_CreateRGBSurface( g_pVideoDriver->getBlitSurface()->flags, 320, 240, 8, 0, 0, 0, 0);
	SDL_SetColors( mp_SceneSurface, g_pGfxEngine->Palette.m_Palette, 0, 255);
	finale_draw( mp_SceneSurface, scene_file, game_path);

	mp_DlgFrame = new CDlgFrame(0, 127, 40, 9);

	SDL_BlitSurface( mp_SceneSurface, NULL, g_pVideoDriver->ScrollSurface, NULL );
}

void CFinaleStaticScene::push_string(const std::string &text, Uint32 delay)
{
	CTextBox *p_Textbox = new CTextBox(134, 100, getstring(text));
	p_Textbox->setDelay(delay);
	p_Textbox->setAttribs(2, LETTER_TYPE_NORMAL);

	mp_textbox_list.push_back(p_Textbox);

	mp_current_tb = mp_textbox_list.front();
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

void CFinaleStaticScene::process()
{
	if( mp_textbox_list.empty() ) { m_mustclose = true; return; }

	mp_current_tb = mp_textbox_list.front();

	// If time up, or user pressed any key goto next text
	if( mp_current_tb->hasFinished() )
	{
		delete mp_current_tb;
		m_count++;
		mp_textbox_list.pop_front();
		if(!mp_textbox_list.empty())
		{
			mp_current_tb = mp_textbox_list.front();
			mp_current_tb->resetTimer();
		}
	}
	else
	{
		// Draw any requested Bitmap
		for( std::vector<bitmap_structure>::iterator i=m_BitmapVector.begin() ;
			 i!=m_BitmapVector.end() ; i++ )
		{
			if( m_count >= i->from_count && m_count <= i->to_count ) // It is in the interval?
			{ // show it!
				i->p_bitmap->draw(g_pVideoDriver->ScrollSurface, i->dest_rect.x, i->dest_rect.y);
			}
		}

		// Draw Frame and the text like type writing
		mp_DlgFrame->draw(g_pVideoDriver->FGLayerSurface);
		mp_current_tb->process();
	}
}

CFinaleStaticScene::~CFinaleStaticScene() {
	CTextBox *p_textbox;
	while(!mp_textbox_list.empty())
	{
		p_textbox = &*mp_textbox_list.front();
		delete p_textbox;
		mp_textbox_list.pop_front();
	}

	delete mp_DlgFrame;
	SDL_FreeSurface(mp_SceneSurface);
}
