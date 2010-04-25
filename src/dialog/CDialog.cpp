/*
 * CDialog.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "../StringUtils.h"
#include "../CLogFile.h"
#include "../sdl/CTimer.h"
#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/sound/CSound.h"
#include "../engine/sounds.h"
#include "../graphics/CGfxEngine.h"
#include "CDialog.h"

// TODO: This class must get a super class and we need two new classes for galaxy and vorticon engines.
// Their menu display are way too different

CDialog::CDialog(Uint16 w, Uint16 h) :
m_Font_ID(0)
{
	m_x = (300/2)-(w*4)+10;	m_y = (200/2)-(h*4);
	m_w = w;	m_h = h;
	
	m_twirl.posy = m_y;
	m_twirl.frame = 0;
	m_twirl.timer = 0;
	
	m_key = 'u';
	m_blink = true;
	m_blinkctr = 0;
	m_selected_ID = 0;
	m_switch = 0;
	m_scroll = 0;
	m_Frame = NULL;
	m_alpha = 128;
}

CDialog::CDialog(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char key) :
m_Font_ID(0)
{
	m_x = (300/2)-(w*4)+10+x;	m_y = (200/2)-(h*4)+y;
	m_w = w;	m_h = h;
	
	m_twirl.posy = m_y;
	m_twirl.frame = 0;
	m_twirl.timer = 0;
	
	m_blink = true;
	m_blinkctr = 0;
	m_key = key;
	m_selected_ID = 0;
	m_length = 15;
	m_switch = 0;
	m_scroll = 0;
	m_Frame = NULL;
	m_alpha = 128;
}

///
// Creation Routines for Objects
///
void CDialog::setFrameTheme(Uint8 theme)
{
	m_Frame = new CDlgFrame(m_x, m_y, m_w, m_h, theme);
}

void CDialog::setGalaxyStyle(bool value)
{
	m_galaxy_style = value;
}

void CDialog::setSelection(Uint8 selection)
{
	m_selected_ID = selection;
}

void CDialog::addObject(Uint8 type, Uint16 x, Uint16 y,const std::string text)
{
	CDlgObject *DlgObject = new CDlgObject();
	DlgObject->create( type, m_dlgobject.size(),
						m_x+(x*8), m_y+(y*8),
						text, m_w-((x-m_x)/8)-5,
						m_Font_ID);
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
	Uint8 type = m_dlgobject[ID]->m_type;
	m_dlgobject[ID]->change( m_w-((m_dlgobject[ID]->m_x-m_x)/8)-4, text, type );
}

void CDialog::setObjectType(Uint8 ID, Uint8 type)
{
	std::string text = m_dlgobject[ID]->m_Option->m_text;
	m_dlgobject[ID]->change( m_w-((m_dlgobject[ID]->m_x-m_x)/8)-4, text, type );
}

void CDialog::setFontID(Uint8 value)
{	m_Font_ID = value;	}


///
// Process routine
///
void CDialog::processInput(int move)
{
	// 't' == text input
	// 'i' == integer input
	// 'n' == do nothing
	// 'u' == up/down movement, enter selection
	// 'l' == left/right movement, enter selection
	// 's' == slider, up/down movement
	// 'c' == counter, up/down movment
	// 'o' == option, left/right selection, up/down movement
	// 'w' == switch, on/off selection, up/down movement
	if( m_key == 't' )
	{
		// Get the input
		if(g_pInput->getPressedIsTypingKey() && (m_name.length() < m_length))
		{
			m_name.append(g_pInput->getPressedTypingKey());
		}
		
		if(g_pInput->getPulsedKey(KBCKSPCE, 5) && (m_name.length() > 0))
		{
			m_name.erase(m_name.length()-1);
		}
		
		if( m_blinkctr >= 30 )
		{
			m_blink = !m_blink;
			m_blinkctr = 0;
		}
		else m_blinkctr++;
		
		if(m_blink)
			setObjectText(m_selected_ID, m_name + "|");
		else if(m_name == "")
			setObjectText(m_selected_ID, "|");
		else
			setObjectText(m_selected_ID, m_name);
	}
	else if( m_key == 'i' )
	{
		// Get the num input
		if(g_pInput->getPressedIsNumKey() && (m_name.length() < m_length))
		{
			m_name.append(g_pInput->getPressedNumKey());
		}
		
		if(g_pInput->getPulsedKey(KBCKSPCE, 5) && (m_name.length() > 0))
		{
			m_name.erase(m_name.length()-1);
		}
		
		if( m_blinkctr >= 30 )
		{
			m_blink = !m_blink;
			m_blinkctr = 0;
		}
		else m_blinkctr++;
		
		if(m_blink)
			setObjectText(m_selected_ID, m_name + "|");
		else if(m_name == "")
			setObjectText(m_selected_ID, "|");
		else
			setObjectText(m_selected_ID, m_name);
	}
	else if(m_key != 'n')
	{
		if(move != 0)
			m_selected_ID += move;
		do
		{
			if(!m_dlgobject.at(m_selected_ID)->m_selectable)
			{
				m_selected_ID++;
				m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
			}
		}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
		
		if(g_pInput->getPulsedCommand((m_key == 'u' or m_key == 'c' or m_key == 's' or m_key == 'o' or m_key == 'w') ? IC_DOWN : IC_RIGHT, 60))
		{
			do
			{
				if(m_selected_ID >= m_dlgobject.size()-1)
				{
					m_switch=1;
					m_scroll=0;
					m_selected_ID = 0;
					m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
				}
				else
				{
					m_selected_ID++;
					m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
					
					if(m_selected_ID >= m_h-2+m_scroll)
					{
						m_scroll++;
					}
					
					if(!m_dlgobject.at(m_selected_ID)->m_selectable)
					{
						if(m_selected_ID+1 <= m_dlgobject.size()-1)
							m_selected_ID++;
						else
							m_selected_ID = 0;
						m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
					}
				}
				
			}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
		}
		else if(g_pInput->getPulsedCommand((m_key == 'u' or m_key == 'c' or m_key == 's') ? IC_UP : IC_LEFT, 60))
		{
			do
			{
				if(m_selected_ID <= 0 )
				{
					m_switch=2;
					
					if(m_dlgobject.size() > m_h-2)
						m_scroll = m_dlgobject.size()-m_h+2;
					m_selected_ID = m_dlgobject.size()-1;
					m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
				}
				else
				{
					m_selected_ID--;
					m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
					
					if(m_selected_ID < m_scroll)
						m_scroll--;
					
					if(!m_dlgobject.at(m_selected_ID)->m_selectable)
					{
						if(m_selected_ID-1 >= 0)
							m_selected_ID--;
						else
							m_selected_ID = m_dlgobject.size()-1;
						m_name = m_dlgobject.at(m_selected_ID)->m_Option->m_text;
					}
				}
			}while(!m_dlgobject.at(m_selected_ID)->m_selectable);
		}
		if(m_key == 's' or m_key == 'c')
		{
			//slider and counter
			if(m_key == 'c')
			{
				m_int = atoi(m_name);
				m_noise = false;
			}
			if(m_key == 's')
			{
				m_int = m_dlgobject.at(m_selected_ID)->m_Option->m_value;
				m_min = 0;
				m_max = 128;
			}

			if(g_pInput->getPulsedCommand(IC_RIGHT, 35))
			{
				if(m_int<m_max)
				{
					m_int++;
				if(m_noise)
				g_pSound->playSound(SOUND_GUN_CLICK, PLAY_FORCE);
				}
			}
			else if(g_pInput->getPulsedCommand(IC_LEFT, 35))
			{
				if(m_int>m_min)
				{
					m_int--;
				if(m_noise)
				g_pSound->playSound(SOUND_GUN_CLICK, PLAY_FORCE);
				}
			}

			if(m_key == 'c')
			{
				m_name = itoa(m_int);
				setObjectText(m_selected_ID, " "+m_name);
			}
			else if(m_key == 's')
			{
				m_dlgobject.at(m_selected_ID)->m_Option->m_value = m_int;
				m_dlgobject.at(m_selected_ID)->m_Option->m_FontMapID = 1;

				std::string asciislider;
				Uint8 i1 = 0;
				Uint8 i2 = (m_int+16)/8;
				int i3;

				if(m_int>1)
				{
					i1 = 0;
					i3 = 0;
					for(int i=0; i<16; i++)
					{
					if(m_int > 9*i)
						i3 = m_int-((9*i));
					}
					asciislider += 19;
					for(i1 ; i1<(m_int-2)/8; i1++)
					asciislider += 20;
					asciislider += 24+i3;
				}
				else if(m_int == 1)
				{
					i1 = 1;
					asciislider += 22;
					asciislider += 25;
				}
				else
				{
					i1 = 1;
					asciislider += 21;
					asciislider += 24;
				}

				// Why does it go from 0 to 16. If we have a resolution of 16 steps
				// It might have to go from 0 to 15
				//asciislider += 25;
				for(i2 ; i2<=16 ; i2++)
					asciislider += 36;
				asciislider += 37;

				setObjectText(m_selected_ID, asciislider);
			}
		}
	}
}

void CDialog::draw()
{
	SDL_Surface *dst_sfc = g_pVideoDriver->FGLayerSurface;

	if(m_alpha < 230)
	{
		SDL_SetAlpha(dst_sfc, SDL_SRCALPHA, m_alpha );
		m_alpha+=10;
	}
	
	// Render the empty Dialog frame if any
	if(m_Frame) m_Frame->draw(dst_sfc);

	CFont &Font = g_pGfxEngine->getFont(m_Font_ID);
	// Draw the to icon up or down accordingly
	if( m_scroll>0 ) // Up Arrow
	{
		Font.drawCharacter(dst_sfc, 15,
						   m_Frame->m_x+m_Frame->m_w-16,
						   m_Frame->m_y+8);
	}
	if( ( m_h-2 < (Uint8) m_dlgobject.size() )  &&
	   ( m_scroll+m_h-2 != m_dlgobject.size() )) // Down Arrow
	{
		Font.drawCharacter(dst_sfc , 19,
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
		m_dlgobject[i]->setSelection( i==m_selected_ID ? true : false);
		m_dlgobject[i]->render(dst_sfc, m_scroll, false );
	}
	Font.setFGColour(dst_sfc->format, 0x0); // Set black letter color for the other elements
	
	if(m_key == 'c')
	{
		if(m_int>m_min)
		Font.drawCharacter(dst_sfc, 21,
						   m_dlgobject[m_selected_ID]->m_x+16,
						   m_dlgobject[m_selected_ID]->m_y);
		if(m_int<m_max)
		Font.drawCharacter(dst_sfc, 17,
						   m_dlgobject[m_selected_ID]->m_x+16+m_dlgobject[m_selected_ID]->m_Option->m_text.length()*8,
						   m_dlgobject[m_selected_ID]->m_y);
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
	
	if(m_dlgobject[m_selected_ID]->m_y-8*m_scroll < m_twirl.posy or m_switch == 1)
	{
		if(m_switch == 1)
		{
			m_switch = 0;
			
			m_twirl.posy = m_dlgobject[m_selected_ID]->m_y-8*m_scroll;
		}
		else if(m_twirl.posy-m_dlgobject[m_selected_ID]->m_y > 8)
		{
			m_twirl.posy = m_twirl.posy-2;
		}
		else
			m_twirl.posy--;
		
	}
	else if(m_dlgobject[m_selected_ID]->m_y-8*m_scroll > m_twirl.posy or m_switch == 2)
	{
		if(m_switch == 2)
		{
			m_switch = 0;
			m_twirl.posy = m_dlgobject[m_selected_ID]->m_y-8*m_scroll;
		}
		else if(m_dlgobject[m_selected_ID]->m_y-m_twirl.posy > 8)
		{
			m_twirl.posy = m_twirl.posy+2;
		}
		else
			m_twirl.posy++;
		
	}
	
	g_pGfxEngine->getCursor()->draw( g_pVideoDriver->FGLayerSurface, m_twirl.frame,
									  m_dlgobject[m_selected_ID]->m_x,
									  m_twirl.posy );
}

///
// Destruction Routines
///
CDialog::~CDialog(){

	for(Uint8 i=0 ; i<m_dlgobject.size() ; i++ )
		delete m_dlgobject[i]; // the first vector element must be cleared

	while(!m_dlgobject.empty())
		m_dlgobject.pop_back();

	m_alpha = 225;
	SDL_SetAlpha(g_pVideoDriver->FGLayerSurface, SDL_SRCALPHA, m_alpha );

	if(m_Frame) delete m_Frame;
}
