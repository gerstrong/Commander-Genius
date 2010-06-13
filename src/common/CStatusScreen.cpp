/*
 * CStatusScreen.cpp
 *
 *  Created on: 15.10.2009
 *      Author: gerstrong
 *
 *  This Class is only used for the Status screen. For now it only draw it to the screen
 *  deciding between the episodes. In future we might expand its functions or add support for
 *  more games.
 */

#include "../funcdefs.h"
#include "CStatusScreen.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"
#include "../StringUtils.h"
#include "../common/Playerdefines.h"
#include "../common/CBehaviorEngine.h"
#include "../graphics/effects/CColorMerge.h"

#define FADE_SPEED	10

CStatusScreen::CStatusScreen
(char episode, stInventory *p_inventory, bool *p_level_completed, int ankhtime, int baseframe) :
 mp_level_completed(p_level_completed),
 m_ankhtime(ankhtime),
 m_closing(false),
 m_closed(false)
{
	m_episode = episode;
	mp_inventory = p_inventory;
	m_baseframe = baseframe;
	if(m_baseframe<0) m_baseframe=0;

	if(g_pVideoDriver->getSpecialFXConfig())
	{
		g_pGfxEngine->pushEffectPtr(new CColorMerge(16));
	}
}

void CStatusScreen::draw()
{
	// Fade in and out process
	if(m_closing)
	{
		if(g_pVideoDriver->getSpecialFXConfig() && !g_pGfxEngine->runningEffect())
			m_closed = true;
		else if(!g_pVideoDriver->getSpecialFXConfig())
			m_closed = true;
	}


	if(!m_closed)
	{
		// draw the episode-specific stuff
		if (m_episode==1)
			drawInventoryEp1();
		else if (m_episode==2)
			drawInventoryEp2();
		else if (m_episode==3)
			drawInventoryEp3();
	}
}


void CStatusScreen::drawInventoryEp1()
{
	int x,t,i,j;
	std::string tempbuf;
	int dlgX,dlgY,dlgW,dlgH;
	CFont &Font = g_pGfxEngine->getFont(0);
	
	SDL_Surface *p_surface = g_pVideoDriver->FGLayerSurface;
	CTilemap &Tilemap = g_pGfxEngine->getTileMap(0);

	dlgX = 5;
	dlgY = 5;
	dlgW = 29;
	dlgH = 15;
	
	g_pGfxEngine->drawDialogBox( p_surface, dlgX,dlgY,dlgW,dlgH, SDL_MapRGB(p_surface->format, 172, 172, 172));
	Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP1_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, true);
	
	// Now draw some white rects. Those are the holders for items, numbers, etc.
	SDL_Rect rect;
	rect.x = (dlgX+1)*8;	rect.w = 12*8; // Score
	rect.y = (dlgY+2)*8;	rect.h = 1*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = dlgX*8+14*8;	rect.w = 15*8; // Extra keen at
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 14*8; // Keens
	rect.y = (dlgY+4)*8;	rect.h = 3*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+16)*8;	rect.w = 13*8; // Ship Parts
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 8*8; // Raygun
	rect.y = (dlgY+8)*8;	rect.h = 3*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+10)*8;	rect.w = 6*8; // Pogo
	rect.h = 5*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+17)*8;	rect.w = 12*8; // Keycards
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 8*8; // Charge
	rect.y = (dlgY+12)*8;	rect.h = 1*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	// fill in what we have
	// 321: joystick/battery/vacuum/fuel not gotten
	// 414: raygun, 415, pogo
	// 424: yellow/red/green/blue cards
	// 448: ship parts, gotten
	// raygun icon
	Tilemap.drawTile(p_surface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 414);
	// pogo
	if (mp_inventory->HasPogo) Tilemap.drawTile(p_surface, (dlgX+12)<<3, ((dlgY+9)<<3)+3, 415);
	// cards
	if (mp_inventory->HasCardYellow)
	{
		Tilemap.drawTile(p_surface, (dlgX+20)<<3, ((dlgY+8)<<3)+3, 424);
		if(mp_inventory->HasCardYellow > 1)
			Font.drawFont( p_surface, itoa(mp_inventory->HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3, false);
	}
	if (mp_inventory->HasCardRed)
	{
		Tilemap.drawTile(p_surface, (dlgX+24)<<3, ((dlgY+8)<<3)+3, 425);
		
		if(mp_inventory->HasCardRed > 1)
			Font.drawFont( p_surface, itoa(mp_inventory->HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3, false);
	}
	if (mp_inventory->HasCardGreen)
	{
		Tilemap.drawTile(p_surface, (dlgX+20)<<3, ((dlgY+10)<<3)+4, 426);
		
		if (mp_inventory->HasCardGreen > 1)
			Font.drawFont( p_surface, itoa(mp_inventory->HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3, false);
	}
	if (mp_inventory->HasCardBlue)
	{
		Tilemap.drawTile(p_surface, (dlgX+24)<<3, ((dlgY+10)<<3)+4, 427);
		
		if(mp_inventory->HasCardBlue > 1)
			Font.drawFont( p_surface, itoa(mp_inventory->HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3, false);
	}
	// ship parts
	if (mp_inventory->HasJoystick) t=448; else t=321;
	Tilemap.drawTile(p_surface, (dlgX+17)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasBattery) t=449; else t=322;
	Tilemap.drawTile(p_surface, (dlgX+20)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasVacuum) t=450; else t=323;
	Tilemap.drawTile(p_surface, (dlgX+23)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasWiskey) t=451; else t=324;
	Tilemap.drawTile(p_surface, (dlgX+26)<<3, ((dlgY+4)<<3)+3, t);
	// ray gun charges
	i = mp_inventory->charges;
	if (i>999) i=999;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+4)<<3, (dlgY+12)<<3, false);
	
	// score
	i = mp_inventory->score;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+11-tempbuf.size())<<3, (dlgY+2)<<3, false);
	// extra life at
	i = mp_inventory->extralifeat;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, (dlgY+2)<<3, false);
	// lives
	i = mp_inventory->lives;
	x = ((dlgX+2)<<3);
	if (i>6) i=6;
	for(j=0;j<i;j++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(m_baseframe);
		Sprite.drawSprite(  p_surface, x, (dlgY+4)<<3);
		x += Sprite.getWidth();
	}
}

void CStatusScreen::drawInventoryEp2()
{
	int x,i,j;
	std::string tempbuf;
	int dlgX,dlgY,dlgW,dlgH;
	CFont &Font = g_pGfxEngine->getFont(0);

	SDL_Surface *p_surface = g_pVideoDriver->FGLayerSurface;
	CTilemap &Tilemap = g_pGfxEngine->getTileMap(0);

	dlgX = 5;
	dlgY = 5;
	dlgW = 29;
	dlgH = 14;

	tempbuf = g_pBehaviorEngine->getString("EP2_StatusBox");
	g_pGfxEngine->drawDialogBox( p_surface, dlgX,dlgY,dlgW,dlgH, SDL_MapRGB(p_surface->format, 172, 172, 172));
	Font.drawFont( p_surface, tempbuf, (dlgX+1)<<3, (dlgY+1)<<3, true);

	// Now draw some white rects. Those are the holders for items, numbers, etc.
	SDL_Rect rect;
	rect.x = (dlgX+1)*8;	rect.w = 12*8; // Score
	rect.y = (dlgY+2)*8;	rect.h = 1*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = dlgX*8+14*8;	rect.w = 15*8; // Extra keen at
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 18*8; // Keens
	rect.y = (dlgY+4)*8;	rect.h = 3*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+20)*8;	rect.w = 9*8; // Pistol
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 18*8; // Targets Saved
	rect.y = (dlgY+8)*8;	rect.h = 4*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+20)*8;	rect.w = 9*8; // Keys
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	// raygun icon
	Tilemap.drawTile(p_surface, (dlgX+21)<<3, ((dlgY+5)<<3), 414);

	// cards
	if (mp_inventory->HasCardYellow)
	{
		Tilemap.drawTile(p_surface, (dlgX+21)<<3, ((dlgY+8)<<3), 424);

		if(mp_inventory->HasCardYellow > 1)
			  Font.drawFont( p_surface, itoa(mp_inventory->HasCardYellow),(dlgX+21)<<3,(dlgY+8)<<3);
	}
	if (mp_inventory->HasCardRed)
	{
		Tilemap.drawTile(p_surface, (dlgX+25)<<3, ((dlgY+8)<<3), 425);

		if(mp_inventory->HasCardRed > 1)
			  Font.drawFont( p_surface, itoa(mp_inventory->HasCardRed),(dlgX+25)<<3,(dlgY+8)<<3);

	}
	if (mp_inventory->HasCardGreen)
	{
		Tilemap.drawTile(p_surface, (dlgX+21)<<3, (dlgY+10)<<3, 426);

		if(mp_inventory->HasCardGreen > 1)
			  Font.drawFont( p_surface, itoa(mp_inventory->HasCardGreen),(dlgX+21)<<3,(dlgY+10)<<3);
	}
	if (mp_inventory->HasCardBlue)
	{
		Tilemap.drawTile(p_surface, (dlgX+25)<<3, (dlgY+10)<<3, 427);

		if(mp_inventory->HasCardBlue > 1)
			  Font.drawFont( p_surface, itoa(mp_inventory->HasCardBlue),(dlgX+25)<<3,(dlgY+10)<<3);
	}

	// pistol charges
	i = mp_inventory->charges;
	if (i>999) i=999;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, ((dlgY+5)<<3)-1);

	// score
	i = mp_inventory->score;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+11-tempbuf.size())<<3, (dlgY+2)<<3);
	// extra life at
	i = mp_inventory->extralifeat;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, (dlgY+2)<<3);
	// lives
	i = mp_inventory->lives;
	x = ((dlgX + 1)<<3)+4;
	if (i>7) i=7;
	for(j=0;j<i;j++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(m_baseframe);
		Sprite.drawSprite( p_surface, x, (dlgY+4)<<3 );
		x += Sprite.getWidth();
	}

	// cities saved
	if (mp_level_completed[4]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL4_TargetName"), (dlgX+1)<<3, (dlgY+8)<<3);
	if (mp_level_completed[6]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL6_TargetName"), (dlgX+8)<<3, (dlgY+8)<<3);
	if (mp_level_completed[7]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL7_TargetName"), (dlgX+1)<<3, (dlgY+9)<<3);
	if (mp_level_completed[13]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL13_TargetName"), (dlgX+8)<<3, (dlgY+9)<<3);
	if (mp_level_completed[11]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL11_TargetName"), (dlgX+1)<<3, (dlgY+10)<<3);
	if (mp_level_completed[9]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL9_TargetName"), (dlgX+8)<<3, (dlgY+10)<<3);
	if (mp_level_completed[15]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL15_TargetName"), (dlgX+1)<<3, (dlgY+11)<<3);
	if (mp_level_completed[16]) Font.drawFont( p_surface, g_pBehaviorEngine->getString("EP2_LVL16_TargetName"), (dlgX+8)<<3, (dlgY+11)<<3);
}

void CStatusScreen::drawInventoryEp3()
{
	int x,i,j;
	std::string tempbuf;
	int dlgX,dlgY,dlgW,dlgH;
	CFont &Font = g_pGfxEngine->getFont(0);

	SDL_Surface *p_surface = g_pVideoDriver->FGLayerSurface;
	CTilemap &Tilemap = g_pGfxEngine->getTileMap(0);

	dlgX = 5;
	dlgY = 5;
	dlgW = 29;
	dlgH = 13;

	tempbuf = g_pBehaviorEngine->getString("EP3_StatusBox");
	g_pGfxEngine->drawDialogBox( p_surface, dlgX,dlgY,dlgW,dlgH, SDL_MapRGB(p_surface->format, 172, 172, 172));
	Font.drawFont( p_surface, tempbuf, (dlgX+1)<<3, (dlgY+1)<<3, true);

	// Now draw some white rects. Those are the holders for items, numbers, etc.
	SDL_Rect rect;
	rect.x = (dlgX+1)*8;	rect.w = 12*8; // Score
	rect.y = (dlgY+2)*8;	rect.h = 1*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = dlgX*8+14*8;	rect.w = 15*8; // Extra keen at
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 18*8; // Keens
	rect.y = (dlgY+4)*8;	rect.h = 3*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+20)*8;	rect.w = 9*8; // Pistol
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+1)*8;	rect.w = 10*8; // Ankh
	rect.y = (dlgY+8)*8;	rect.h = 3*8;
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	rect.x = (dlgX+12)*8;	rect.w = 17*8; // Keys
	SDL_FillRect(p_surface,&rect,0xFFFFFF);

	// raygun icon
	Tilemap.drawTile(p_surface, (dlgX+21)<<3, ((dlgY+5)<<3)-5, 216);

	// calculate % ankh time left
	int ankhtimepercent = (int)((float)m_ankhtime / (PLAY_ANKH_TIME/100));
	// ankh time
	Tilemap.drawTile(p_surface, (dlgX+3)<<3, ((dlgY+8)<<3)+3, 214);
	 
	Font.drawFont( p_surface, itoa(ankhtimepercent), (dlgX+7)<<3, ((dlgY+8)<<3)+7);
	

	// cards
	if (mp_inventory->HasCardYellow)
	{
		Tilemap.drawTile(p_surface, ((dlgX+13)<<3)+4, ((dlgY+8)<<3)+4, 217);

		if(mp_inventory->HasCardYellow > 1)
			Font.drawFont( p_surface, itoa(mp_inventory->HasCardYellow),(dlgX+12)<<3,((dlgY+8)<<3)+3);
	}
	if (mp_inventory->HasCardRed)
	{
		Tilemap.drawTile(p_surface, ((dlgX+17)<<3)+4, ((dlgY+8)<<3)+4, 218);

		if(mp_inventory->HasCardRed > 1)
			Font.drawFont(p_surface, itoa(mp_inventory->HasCardRed),(dlgX+16)<<3,((dlgY+8)<<3)+3);
	}
	if (mp_inventory->HasCardGreen)
	{
		Tilemap.drawTile(p_surface, ((dlgX+21)<<3)+4, ((dlgY+8)<<3)+4, 219);

		if(mp_inventory->HasCardGreen > 1)
			Font.drawFont(p_surface, itoa(mp_inventory->HasCardGreen),(dlgX+20)<<3,((dlgY+8)<<3)+3);
	}
	if (mp_inventory->HasCardBlue)
	{
		Tilemap.drawTile(p_surface, ((dlgX+25)<<3)+4, ((dlgY+8)<<3)+4, 220);

		if(mp_inventory->HasCardBlue > 1)
			Font.drawFont(p_surface, itoa(mp_inventory->HasCardBlue),(dlgX+24)<<3,((dlgY+8)<<3)+3);
	}

	// pistol charges
	i = mp_inventory->charges;
	if (i>999) i=999;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, ((dlgY+5)<<3)-1);

	// score
	i = mp_inventory->score;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+11-tempbuf.size())<<3, (dlgY+2)<<3);
	// extra life at
	i = mp_inventory->extralifeat;
	tempbuf = itoa(i);
	Font.drawFont( p_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, (dlgY+2)<<3);
	// lives
	i = mp_inventory->lives;
	x = ((dlgX + 1)<<3)+4;
	if (i>7) i=7;
	for(j=0;j<i;j++)
	{
		CSprite &Sprite = g_pGfxEngine->getSprite(m_baseframe);
		Sprite.drawSprite(  p_surface, x, (dlgY+4)<<3);
		x += Sprite.getWidth();
	}
}

CStatusScreen::~CStatusScreen() {
}

