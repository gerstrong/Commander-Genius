/*
 * CDialog.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "../include/gamedo.h"
#include "../sdl/CTimer.h"
#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "CDialog.h"

CDialog::CDialog(SDL_Surface *DialogSurface, Uint16 w, Uint16 h)
{
	m_x = (300/2)-(w*4)+10;	m_y = (200/2)-(h*4);
	m_w = w;	m_h = h;

	m_twirl.posy = m_y;
	m_twirl.frame = 0;
	m_twirl.timer = 0;

	m_selected_ID = 0;
	m_switch = 0;
	m_scroll = 0;
	m_Frame = NULL;
	m_alpha = 0;
	m_DialogSurface = DialogSurface;
}

CDialog::~CDialog(){

	for(Uint8 i=0 ; i<m_dlgobject.size() ; i++ )
		delete m_dlgobject[i]; // the first vector element must be cleared

	while(!m_dlgobject.empty())
		m_dlgobject.pop_back();

	m_alpha = 225;
	SDL_SetAlpha(m_DialogSurface, SDL_SRCALPHA, m_alpha );

	if(m_Frame) delete m_Frame;
}

///
// Creation Routines for Objects
///
void CDialog::setFrameTheme(Uint8 theme)
{
	m_Frame = new CDlgFrame(m_x, m_y, m_w, m_h, theme);
}

void CDialog::addObject(Uint8 type, Uint16 x, Uint16 y,const std::string text)
{
	CDlgObject *DlgObject = new CDlgObject();
	DlgObject->create( type, m_dlgobject.size(), m_x+(x*8), m_y+(y*8), text, m_w-((x-m_x)/8)-5 );
	m_dlgobject.push_back(DlgObject);

	// Check if the ID is not out of bounds.
	if( m_selected_ID < m_dlgobject.size() ) return;

	for(Uint8 i=0 ; i<m_dlgobject.size() ; i++) // go the next selectable item
	{
		if(!m_dlgobject.at(m_selected_ID)->m_selectable)
		{
			m_selected_ID++;
		}
	}
}

void CDialog::setObjectText(Uint8 ID, const std::string &text)
{
	if( m_dlgobject[ID]->m_type == DLG_OBJ_OPTION_TEXT )
		m_dlgobject[ID]->change( text, m_w-((m_dlgobject[ID]->m_x-m_x)/8)-5 );
}

///
// Retrieval functions
///
int CDialog::getSelection()
{	return m_selected_ID;
}

///
// Process routine
///
void CDialog::processInput(char dir)
{
	if( dir == 't' )
	{
	// This cycle will wait for the input of name and hit of enter
    /*bool blink = true;
    int blinkctr = 0;
	do
	{
	    bm_highscore->draw( m_sfc, x1, y1 );
	    bm_name->draw( m_sfc, x2, y2 );
	    bm_score->draw( m_sfc, x3, y2);
	    if( bm_extra ) bm_extra->draw( m_sfc, x4, y3);

		// Blit all the text and images
		for(i=KA ; i<=KZ ; i++)
		{
			if (g_pInput->getHoldedKey(KSHIFT) && g_pInput->getPressedKey(i) && WrittenName.length() < 13)
			{
					sprintf(buf,"%c",'A' + i - KA);
					WrittenName.append(buf);
					copy(WrittenName.data(),WrittenName.data()+WrittenName.length(),Name[place]);
					WrittenName.copy(buf,WrittenName.length(),0);
			}
			else if(g_pInput->getPressedKey(i) && WrittenName.length() < 13)
			{
					sprintf(buf,"%c",'a' + i - KA);
					WrittenName.append(buf);
					copy(WrittenName.data(),WrittenName.data()+WrittenName.length(),Name[place]);
					WrittenName.copy(buf,WrittenName.length(),0);
			}

		}
		if(g_pInput->getPressedKey(KBCKSPCE) && (WrittenName.length() > 0))
		{
			memset(buf,0,256);
			g_pGfxEngine->Font->drawFont(m_sfc, "              ",40,64+(place<<4), LETTER_TYPE_RED);

			WrittenName.erase(WrittenName.length()-1);
			WrittenName.copy(buf,WrittenName.length(),0);
			memset(Name[place],0,16);
			WrittenName.copy(Name[place],WrittenName.length(),0);
		}

		// Here it must be split up in Episodes 1, 2 and 3.
		// Print the labels
		for( i=0 ; i<7 ; i++ )
		{
			if(pCKP->Control.levelcontrol.episode == 3)
			{
				if(i != place)
					g_pGfxEngine->Font->drawFont(m_sfc, Name[i],69,64+(i<<4), LETTER_TYPE_RED);
				else
				{
					g_pGfxEngine->Font->drawFont(m_sfc, buf,69,64+(i<<4), LETTER_TYPE_RED);
					
					if(blink)
						g_pGfxEngine->Font->drawFont(m_sfc, "_",69+(strlen(buf)<<3),64+(i<<4), LETTER_TYPE_RED);
					else
						g_pGfxEngine->Font->drawFont(m_sfc, " ",69+(strlen(buf)<<3),64+(i<<4), LETTER_TYPE_RED);
				}
				g_pGfxEngine->Font->drawFont(m_sfc, Score[i],253-(strlen(Score[i])<<3),64+(i<<4), LETTER_TYPE_RED);
				
			}
			else
			{
			if(i != place)
				g_pGfxEngine->Font->drawFont(m_sfc, Name[i],40,64+(i<<4), LETTER_TYPE_RED);
			else
			{
				g_pGfxEngine->Font->drawFont(m_sfc, buf,40,64+(i<<4), LETTER_TYPE_RED);

				if(blink)
					g_pGfxEngine->Font->drawFont(m_sfc, "_",40+(strlen(buf)<<3),64+(i<<4), LETTER_TYPE_RED);
				else
					g_pGfxEngine->Font->drawFont(m_sfc, " ",40+(strlen(buf)<<3),64+(i<<4), LETTER_TYPE_RED);
			}
			g_pGfxEngine->Font->drawFont(m_sfc, Score[i],200-(strlen(Score[i])<<3),64+(i<<4), LETTER_TYPE_RED);
			}

			if(pCKP->Control.levelcontrol.episode == 1)
			{
				if(Extra[i][0])
					g_pGfxEngine->Tilemap->drawTile(m_sfc, 224,60+(i<<4),ItemTiles[0]);
				if(Extra[i][1])
					g_pGfxEngine->Tilemap->drawTile(m_sfc, 240,60+(i<<4),ItemTiles[1]);
				if(Extra[i][2])
					g_pGfxEngine->Tilemap->drawTile(m_sfc, 256,60+(i<<4),ItemTiles[2]);
				if(Extra[i][3])
					g_pGfxEngine->Tilemap->drawTile(m_sfc, 272,60+(i<<4),ItemTiles[3]);
			}
			else if(pCKP->Control.levelcontrol.episode == 2)
			{
				for( i=0 ; i<7 ; i++ )
					g_pGfxEngine->Font->drawFont(m_sfc, itoa(Cities[i]) ,252,64+(i<<4), LETTER_TYPE_RED);
			}
		}

		gamedo_AnimatedTiles();
		g_pInput->pollEvents();

		blinkctr++; // The blinking cursor
		if(blinkctr > 100)
		{
			blink = !blink;
			blinkctr = 0;
		}

		gamedo_RenderScreen();
	}while(!g_pInput->getPressedKey(KENTER));*/
	}
	else
	{
		do
		{
		if(!m_dlgobject.at(m_selected_ID)->m_selectable)
		{
			m_selected_ID++;
		}
		}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
	if(g_pInput->getPulsedCommand((dir == 'u') ? IC_DOWN : IC_RIGHT, 60))
	{
		do
		{
			if(m_selected_ID >= m_dlgobject.size()-1)
			{
				m_switch=1;
				m_scroll=0;
				if(m_selected_ID >= m_h-2+m_scroll)
				m_selected_ID = m_dlgobject.size()-1;
				else
				m_selected_ID = 0;
			}
			else
			{
				m_selected_ID++;

				if(m_selected_ID >= m_h-2+m_scroll)
					m_scroll++;
				
				if(!m_dlgobject.at(m_selected_ID)->m_selectable)
				{
					m_selected_ID++;
				}
			}

		}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
	}
	else if(g_pInput->getPulsedCommand((dir == 'u') ? IC_UP : IC_LEFT, 60))
	{
		do
		{
			if(m_selected_ID <= 1 )
			{
				m_switch=2;
				
				if(m_dlgobject.size() > m_h)
					m_scroll = m_selected_ID-(m_h-2)+1;
				m_selected_ID = m_dlgobject.size()-1;
			}
			else
			{
				m_selected_ID--;

				if(m_selected_ID < m_scroll)
					m_scroll--;
				
				if(!m_dlgobject.at(m_selected_ID)->m_selectable)
				{
					m_selected_ID--;
				}
			}
		}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
	}
	}
	}

void CDialog::draw()
{
	if(m_alpha < 230)
	{
		SDL_SetAlpha(m_DialogSurface, SDL_SRCALPHA, m_alpha );
		m_alpha+=10;
	}

	// Render the empty Dialog frame if any
	if(m_Frame) m_Frame->draw(m_DialogSurface);

	// Draw the to icon up or down accordingly
	if( m_scroll>0 ) // Up Arrow
	{
		g_pGfxEngine->Font->drawCharacter(m_DialogSurface, 15,
									m_Frame->m_x+m_Frame->m_w-16,
									m_Frame->m_y+8);
	}
	if( ( m_h-2 < (Uint8) m_dlgobject.size() )  &&
		( m_scroll != m_dlgobject.size()-m_h+2 )) // Down Arrow
	{
		g_pGfxEngine->Font->drawCharacter(m_DialogSurface , 19,
									m_Frame->m_x+m_Frame->m_w-16,
									m_Frame->m_y+m_Frame->m_h-16);
	}

	Uint8 max;
	if( m_h+m_scroll-2 > (Uint8)m_dlgobject.size() )
		max = m_dlgobject.size();
	else
		max = m_h+m_scroll-2;

	// they are processed by ID of course
	for(Uint16 i=m_scroll ;	i<max ; i++)
	{
		m_dlgobject[i]->render(m_DialogSurface, m_scroll, 2*(i==m_selected_ID) );
	}

	// Render the twirl
	drawTwirl();
}

#define TWIRL_TIME	5
void CDialog::drawTwirl()
{
	if( m_twirl.timer >= TWIRL_TIME )
	{
		m_twirl.timer = 0;

		if(m_twirl.frame >= 7)
			m_twirl.frame=0;
		else
			m_twirl.frame++;
	}
	else m_twirl.timer++;

	if(m_dlgobject[m_selected_ID]->m_y < m_twirl.posy or m_switch == 1)
	{
		if(m_switch == 1)
		{
			m_switch = 0;

			m_twirl.posy = m_dlgobject[m_selected_ID]->m_y;
		}
		else if(m_twirl.posy-m_dlgobject[m_selected_ID]->m_y > 8)
		{
			m_twirl.posy = m_twirl.posy-2;
		}
		else
			m_twirl.posy--;
		
	}
	else if(m_dlgobject[m_selected_ID]->m_y > m_twirl.posy or m_switch == 2)
	{
		if(m_switch == 2)
		{
			m_switch = 0;
			m_twirl.posy = m_dlgobject[m_selected_ID]->m_y;
		}
		else if(m_dlgobject[m_selected_ID]->m_y-m_twirl.posy > 8)
		{
			m_twirl.posy = m_twirl.posy+2;
		}
		else
			m_twirl.posy++;
		
	}

	g_pGfxEngine->Font->drawTwirl( m_DialogSurface, m_twirl.frame,
									m_dlgobject[m_selected_ID]->m_x,
									m_twirl.posy );
}

///
// Misc Routines
///

