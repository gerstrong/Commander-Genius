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

CStatusScreen::CStatusScreen(char episode, stInventory *p_inventory, int baseframe) {
	// TODO Auto-generated constructor stub
	m_episode = episode;
	mp_surface = g_pVideoDriver->FGLayerSurface;
	mp_inventory = p_inventory;
	m_baseframe = baseframe;
}

void CStatusScreen::draw()
{
	// draw the episode-specific stuff
	if (m_episode==1)
		drawInventoryEp1();
	else if (m_episode==2)
		drawInventoryEp2();
	else if (m_episode==3)
		drawInventoryEp3();
}

CStatusScreen::~CStatusScreen() {
	// TODO Auto-generated destructor stub
}

void CStatusScreen::drawInventoryEp1()
{
	int x,t,i,j;
	std::string tempbuf;
	int dlgX,dlgY,dlgW,dlgH;
	
	dlgX = 5;
	dlgY = 5;
	dlgW = 30;
	dlgH = 15;
	
	g_pGfxEngine->drawDialogBox( mp_surface, dlgX,dlgY,dlgW,dlgH);
	g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP1_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);
	
	// fill in what we have
	// 321: joystick/battery/vacuum/fuel not gotten
	// 414: raygun, 415, pogo
	// 424: yellow/red/green/blue cards
	// 448: ship parts, gotten
	// raygun icon
	g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 414);
	// pogo
	if (mp_inventory->HasPogo) g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+12)<<3)+4, ((dlgY+9)<<3)+3, 415);
	// cards
	if (mp_inventory->HasCardYellow)
	{
		g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+21)<<3, ((dlgY+8)<<3)+3, 424);
		if(mp_inventory->HasCardYellow > 1)
			g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3,0);
	}
	if (mp_inventory->HasCardRed)
	{
		g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+25)<<3, ((dlgY+8)<<3)+3, 425);
		
		if(mp_inventory->HasCardRed > 1)
			g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3,0);
	}
	if (mp_inventory->HasCardGreen)
	{
		g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+21)<<3, ((dlgY+10)<<3)+4, 426);
		
		if (mp_inventory->HasCardGreen > 1)
			g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3,0);
	}
	if (mp_inventory->HasCardBlue)
	{
		g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+25)<<3, ((dlgY+10)<<3)+4, 427);
		
		if(mp_inventory->HasCardBlue > 1)
			g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3,0);
	}
	// ship parts
	if (mp_inventory->HasJoystick) t=448; else t=321;
	g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+18)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasBattery) t=449; else t=322;
	g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+21)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasVacuum) t=450; else t=323;
	g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+24)<<3, ((dlgY+4)<<3)+3, t);
	if (mp_inventory->HasWiskey) t=451; else t=324;
	g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+27)<<3, ((dlgY+4)<<3)+3, t);
	// ray gun charges
	i = mp_inventory->charges;
	if (i>999) i=999;
	tempbuf = itoa(i);
	g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+4)<<3, (dlgY+12)<<3, 0);
	
	// score
	i = mp_inventory->score;
	tempbuf = itoa(i);
	g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3, 0);
	// extra life at
	i = mp_inventory->extralifeat;
	tempbuf = itoa(i);
	g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3, 0);
	// lives
	i = mp_inventory->lives;
	x = ((dlgX+1)<<3)+4;
	if (i>7) i=7;
	for(j=0;j<i;j++)
	{
		g_pGfxEngine->Sprite[m_baseframe]->drawSprite(  mp_surface, x, (dlgY+4)<<3);
		// TODO: Playbaseframe is somewhere else. It's needed, so the player is displayed at right colour
		x += g_pGfxEngine->Sprite[0]->getWidth();
	}
}

void CStatusScreen::drawInventoryEp2()
{
	/*
	 int x,i,j;
	 std::string tempbuf;
	 int dlgX,dlgY,dlgW,dlgH;
	 
	 SDL_Surface *mp_surface = g_pVideoDriver->FGLayerSurface;
	 
	 dlgX = GetStringAttribute("EP2_StatusBox", "LEFT");
	 dlgY = GetStringAttribute("EP2_StatusBox", "TOP");
	 dlgW = GetStringAttribute("EP2_StatusBox", "WIDTH");
	 dlgH = GetStringAttribute("EP2_StatusBox", "HEIGHT");
	 
	 g_pGfxEngine->drawDialogBox( mp_surface, dlgX,dlgY,dlgW,dlgH);
	 g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);
	 
	 // cards
	 if (mp_inventory->HasCardYellow)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+21)<<3)-4, ((dlgY+8)<<3)+3, 424);
	 
	 if(mp_inventory->HasCardYellow > 1)
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3);
	 
	 }
	 if (mp_inventory->HasCardRed)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+25)<<3)-4, ((dlgY+8)<<3)+3, 425);
	 
	 if(mp_inventory->HasCardRed > 1)
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3,0);
	 
	 }
	 if (mp_inventory->HasCardGreen)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+21)<<3)-4, ((dlgY+10)<<3)+4, 426);
	 
	 if(mp_inventory->HasCardGreen > 1)
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3,0);
	 }
	 if (mp_inventory->HasCardBlue)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+25)<<3)-4, ((dlgY+10)<<3)+4, 427);
	 
	 if(mp_inventory->HasCardBlue > 1)
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3,0);
	 
	 }
	 // cities saved
	 if (levels_completed[4]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL4_TargetName"), (dlgX+1)<<3, (dlgY+8)<<3, 0);
	 if (levels_completed[6]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL6_TargetName"), (dlgX+8)<<3, (dlgY+8)<<3, 0);
	 if (levels_completed[7]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL7_TargetName"), (dlgX+1)<<3, (dlgY+9)<<3, 0);
	 if (levels_completed[13]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL13_TargetName"), (dlgX+8)<<3, (dlgY+9)<<3, 0);
	 if (levels_completed[11]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL11_TargetName"), (dlgX+1)<<3, (dlgY+10)<<3, 0);
	 if (levels_completed[9]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL9_TargetName"), (dlgX+8)<<3, (dlgY+10)<<3, 0);
	 if (levels_completed[15]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL15_TargetName"), (dlgX+1)<<3, (dlgY+11)<<3, 0);
	 if (levels_completed[16]) g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP2_LVL16_TargetName"), (dlgX+8)<<3, (dlgY+11)<<3, 0);
	 
	 // raygun icon
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+20)<<3, ((dlgY+5)<<3)-5, 414);
	 
	 // ray gun charges text
	 i = mp_inventory->charges;
	 if (i>999) i=999;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+27-tempbuf.size())<<3, ((dlgY+5)<<3)-1, 0);
	 
	 // score
	 i = mp_inventory->score;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3, 0);
	 
	 // extra life at
	 i = mp_inventory->extralifeat;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3, 0);
	 
	 // lives
	 i = mp_inventory->lives;
	 x = ((dlgX + 1)<<3)+4;
	 if (i>7) i=7;
	 for(j=0;j<i;j++)
	 {
	 g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
	 x += g_pGfxEngine->Sprite[0]->getWidth();
	 }
	 */
}

void CStatusScreen::drawInventoryEp3()
{
	//int x,y,t,i,j;
	/*int i,j,x;
	 int ankhtimepercent;
	 std::string tempbuf;
	 int dlgX,dlgY,dlgW,dlgH;
	 
	 SDL_Surface *mp_surface = g_pVideoDriver->FGLayerSurface;
	 
	 dlgX = GetStringAttribute("EP3_StatusBox", "LEFT");
	 dlgY = GetStringAttribute("EP3_StatusBox", "TOP");
	 dlgW = GetStringAttribute("EP3_StatusBox", "WIDTH");
	 dlgH = GetStringAttribute("EP3_StatusBox", "HEIGHT");
	 
	 g_pGfxEngine->drawDialogBox( mp_surface, dlgX,dlgY,dlgW,dlgH);
	 g_pGfxEngine->Font->drawFont( mp_surface, getstring("EP3_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);
	 
	 // calculate % ankh time left
	 ankhtimepercent = (int)((float)player[p].ankhtime / (PLAY_ANKH_TIME/100));
	 // ankh time
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 214);
	 
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(ankhtimepercent), (dlgX+8)<<3, ((dlgY+8)<<3)+7, 0);
	 
	 // raygun icon
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, (dlgX+23)<<3, ((dlgY+5)<<3)-5, 216);
	 
	 // ray gun charges text
	 i = mp_inventory->charges;
	 if (i>999) i=999;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( mp_surface, tempbuf, (dlgX+26)<<3, ((dlgY+5)<<3)-1, 0);
	 
	 // cards
	 if (mp_inventory->HasCardYellow)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+14)<<3)+4, ((dlgY+8)<<3)+4, 217);
	 
	 if(mp_inventory->HasCardYellow > 1)
	 g_pGfxEngine->Font->drawFont( mp_surface, itoa(mp_inventory->HasCardYellow),(dlgX+13)<<3,((dlgY+8)<<3)+3,0);
	 
	 }
	 if (mp_inventory->HasCardRed)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+18)<<3)+4, ((dlgY+8)<<3)+4, 218);
	 
	 if(mp_inventory->HasCardRed > 1)
	 g_pGfxEngine->Font->drawFont(mp_surface, itoa(mp_inventory->HasCardRed),(dlgX+17)<<3,((dlgY+8)<<3)+3,0);
	 
	 }
	 if (mp_inventory->HasCardGreen)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+22)<<3)+4, ((dlgY+8)<<3)+4, 219);
	 
	 if(mp_inventory->HasCardGreen > 1)
	 g_pGfxEngine->Font->drawFont(mp_surface, itoa(mp_inventory->HasCardGreen),(dlgX+21)<<3,((dlgY+8)<<3)+3,0);
	 
	 }
	 if (mp_inventory->HasCardBlue)
	 {
	 g_pGfxEngine->Tilemap->drawTile(mp_surface, ((dlgX+26)<<3)+4, ((dlgY+8)<<3)+4, 220);
	 
	 if(mp_inventory->HasCardBlue > 1)
	 g_pGfxEngine->Font->drawFont(mp_surface, itoa(mp_inventory->HasCardBlue),(dlgX+25)<<3,((dlgY+8)<<3)+3,0);
	 
	 }
	 
	 // score
	 i = mp_inventory->score;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont(mp_surface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3);
	 // extra life at
	 i = mp_inventory->extralifeat;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont(mp_surface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3);
	 // lives
	 i = mp_inventory->lives;
	 x = ((dlgX+1)<<3)+4;
	 if (i>9) i=9;
	 for(j=0;j<i;j++)
	 {
	 g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
	 x += g_pGfxEngine->Sprite[0]->getWidth();
	 }*/
	
}
