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

#include "CStatusScreen.h"
#include "../sdl/CVideoDriver.h"

CStatusScreen::CStatusScreen(char episode, stInventory *p_inventory) {
	// TODO Auto-generated constructor stub
	m_episode = episode;
	mp_surface = g_pVideoDriver->FGLayerSurface;
	mp_inventory = p_inventory;
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
/*int x,t,i,j;
std::string tempbuf;
int dlgX,dlgY,dlgW,dlgH;

  dlgX = GetStringAttribute("EP1_StatusBox", "LEFT");
  dlgY = GetStringAttribute("EP1_StatusBox", "TOP");
  dlgW = GetStringAttribute("EP1_StatusBox", "WIDTH");
  dlgH = GetStringAttribute("EP1_StatusBox", "HEIGHT");

  g_pGfxEngine->drawDialogBox( boxsurface, dlgX,dlgY,dlgW,dlgH);
  g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP1_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);

  // fill in what we have
  // 321: joystick/battery/vacuum/fuel not gotten
  // 414: raygun, 415, pogo
  // 424: yellow/red/green/blue cards
  // 448: ship parts, gotten
  // raygun icon
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 414);
  // pogo
  if (player[p].inventory.HasPogo) g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+12)<<3)+4, ((dlgY+9)<<3)+3, 415);
  // cards
  if (player[p].inventory.HasCardYellow)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+21)<<3, ((dlgY+8)<<3)+3, 424);
	  if(player[p].inventory.HasCardYellow > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3,0);
  }
  if (player[p].inventory.HasCardRed)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+25)<<3, ((dlgY+8)<<3)+3, 425);

	  if(player[p].inventory.HasCardRed > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3,0);
  }
  if (player[p].inventory.HasCardGreen)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+21)<<3, ((dlgY+10)<<3)+4, 426);

	  if (player[p].inventory.HasCardGreen > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3,0);
  }
  if (player[p].inventory.HasCardBlue)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+25)<<3, ((dlgY+10)<<3)+4, 427);

	  if(player[p].inventory.HasCardBlue > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3,0);
  }
  // ship parts
  if (player[p].inventory.HasJoystick) t=448; else t=321;
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+18)<<3, ((dlgY+4)<<3)+3, t);
  if (player[p].inventory.HasBattery) t=449; else t=322;
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+21)<<3, ((dlgY+4)<<3)+3, t);
  if (player[p].inventory.HasVacuum) t=450; else t=323;
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+24)<<3, ((dlgY+4)<<3)+3, t);
  if (player[p].inventory.HasWiskey) t=451; else t=324;
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+27)<<3, ((dlgY+4)<<3)+3, t);
  // ray gun charges
  i = player[p].inventory.charges;
  if (i>999) i=999;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+4)<<3, (dlgY+12)<<3, 0);

  // score
  i = player[p].inventory.score;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3, 0);
  // extra life at
  i = player[p].inventory.extralifeat;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3, 0);
  // lives
  i = player[p].inventory.lives;
  x = ((dlgX+1)<<3)+4;
  if (i>7) i=7;
  for(j=0;j<i;j++)
  {
	  g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
	  x += g_pGfxEngine->Sprite[0]->getWidth();
  }*/
}

void CStatusScreen::drawInventoryEp2()
{
/*
int x,i,j;
std::string tempbuf;
int dlgX,dlgY,dlgW,dlgH;

  SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

  dlgX = GetStringAttribute("EP2_StatusBox", "LEFT");
  dlgY = GetStringAttribute("EP2_StatusBox", "TOP");
  dlgW = GetStringAttribute("EP2_StatusBox", "WIDTH");
  dlgH = GetStringAttribute("EP2_StatusBox", "HEIGHT");

  g_pGfxEngine->drawDialogBox( boxsurface, dlgX,dlgY,dlgW,dlgH);
  g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);

  // cards
  if (player[p].inventory.HasCardYellow)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+21)<<3)-4, ((dlgY+8)<<3)+3, 424);

	  if(player[p].inventory.HasCardYellow > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3);

  }
  if (player[p].inventory.HasCardRed)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+25)<<3)-4, ((dlgY+8)<<3)+3, 425);

	  if(player[p].inventory.HasCardRed > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3,0);

  }
  if (player[p].inventory.HasCardGreen)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+21)<<3)-4, ((dlgY+10)<<3)+4, 426);

	  if(player[p].inventory.HasCardGreen > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3,0);
  }
  if (player[p].inventory.HasCardBlue)
  {
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+25)<<3)-4, ((dlgY+10)<<3)+4, 427);

	  if(player[p].inventory.HasCardBlue > 1)
		  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3,0);

  }
  // cities saved
  if (levels_completed[4]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL4_TargetName"), (dlgX+1)<<3, (dlgY+8)<<3, 0);
  if (levels_completed[6]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL6_TargetName"), (dlgX+8)<<3, (dlgY+8)<<3, 0);
  if (levels_completed[7]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL7_TargetName"), (dlgX+1)<<3, (dlgY+9)<<3, 0);
  if (levels_completed[13]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL13_TargetName"), (dlgX+8)<<3, (dlgY+9)<<3, 0);
  if (levels_completed[11]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL11_TargetName"), (dlgX+1)<<3, (dlgY+10)<<3, 0);
  if (levels_completed[9]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL9_TargetName"), (dlgX+8)<<3, (dlgY+10)<<3, 0);
  if (levels_completed[15]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL15_TargetName"), (dlgX+1)<<3, (dlgY+11)<<3, 0);
  if (levels_completed[16]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL16_TargetName"), (dlgX+8)<<3, (dlgY+11)<<3, 0);

  // raygun icon
  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+20)<<3, ((dlgY+5)<<3)-5, 414);

  // ray gun charges text
  i = player[p].inventory.charges;
  if (i>999) i=999;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+27-tempbuf.size())<<3, ((dlgY+5)<<3)-1, 0);

  // score
  i = player[p].inventory.score;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3, 0);

  // extra life at
  i = player[p].inventory.extralifeat;
  tempbuf = itoa(i);
  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3, 0);

  // lives
  i = player[p].inventory.lives;
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

	  SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

	  dlgX = GetStringAttribute("EP3_StatusBox", "LEFT");
	  dlgY = GetStringAttribute("EP3_StatusBox", "TOP");
	  dlgW = GetStringAttribute("EP3_StatusBox", "WIDTH");
	  dlgH = GetStringAttribute("EP3_StatusBox", "HEIGHT");

	  g_pGfxEngine->drawDialogBox( boxsurface, dlgX,dlgY,dlgW,dlgH);
	  g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP3_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);

	  // calculate % ankh time left
	  ankhtimepercent = (int)((float)player[p].ankhtime / (PLAY_ANKH_TIME/100));
	  // ankh time
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 214);

	  g_pGfxEngine->Font->drawFont( boxsurface, itoa(ankhtimepercent), (dlgX+8)<<3, ((dlgY+8)<<3)+7, 0);

	  // raygun icon
	  g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+23)<<3, ((dlgY+5)<<3)-5, 216);

	  // ray gun charges text
	  i = player[p].inventory.charges;
	  if (i>999) i=999;
	  tempbuf = itoa(i);
	  g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+26)<<3, ((dlgY+5)<<3)-1, 0);

	  // cards
	  if (player[p].inventory.HasCardYellow)
	  {
		  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+14)<<3)+4, ((dlgY+8)<<3)+4, 217);

		  if(player[p].inventory.HasCardYellow > 1)
			  g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardYellow),(dlgX+13)<<3,((dlgY+8)<<3)+3,0);

	  }
	  if (player[p].inventory.HasCardRed)
	  {
		  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+18)<<3)+4, ((dlgY+8)<<3)+4, 218);

		  if(player[p].inventory.HasCardRed > 1)
			  g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardRed),(dlgX+17)<<3,((dlgY+8)<<3)+3,0);

	  }
	  if (player[p].inventory.HasCardGreen)
	  {
		  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+22)<<3)+4, ((dlgY+8)<<3)+4, 219);

		  if(player[p].inventory.HasCardGreen > 1)
			  g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardGreen),(dlgX+21)<<3,((dlgY+8)<<3)+3,0);

	  }
	  if (player[p].inventory.HasCardBlue)
	  {
		  g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+26)<<3)+4, ((dlgY+8)<<3)+4, 220);

		  if(player[p].inventory.HasCardBlue > 1)
			  g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardBlue),(dlgX+25)<<3,((dlgY+8)<<3)+3,0);

	  }

	  // score
	  i = player[p].inventory.score;
	  tempbuf = itoa(i);
	  g_pGfxEngine->Font->drawFont(boxsurface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3);
	  // extra life at
	  i = player[p].inventory.extralifeat;
	  tempbuf = itoa(i);
	  g_pGfxEngine->Font->drawFont(boxsurface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3);
	  // lives
	  i = player[p].inventory.lives;
	  x = ((dlgX+1)<<3)+4;
	  if (i>9) i=9;
	  for(j=0;j<i;j++)
	  {
		  g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
		  x += g_pGfxEngine->Sprite[0]->getWidth();
	  }*/

}
