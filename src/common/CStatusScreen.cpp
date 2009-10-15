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

CStatusScreen::CStatusScreen() {
	// TODO Auto-generated constructor stub

}

void CStatusScreen::draw()
{
	// TODO: And here goes the code for the different Status Screens. Hopefully it will be enhanced for more games
	/*stLevelControl *p_levelcontrol;
	p_levelcontrol = &(pCKP->Control.levelcontrol);

  // draw the episode-specific stuff
  if (p_levelcontrol->episode==1)
    inventory_draw_ep1(p);

  else if (p_levelcontrol->episode==2)
    inventory_draw_ep2(p, pCKP->Control.levelcontrol.levels_completed);

  else if (p_levelcontrol->episode==3)
    inventory_draw_ep3(p);


  g_pVideoDriver->update_screen();

  // wait for any button pressed or any action triggered
  bool close=false;

  g_pInput->flushAll();

  while(!close)
  {
	g_pInput->pollEvents();

	if(g_pInput->getPressedAnyCommand(p) || g_pInput->getPressedAnyKey()) close=true;
  }*/
}

CStatusScreen::~CStatusScreen() {
	// TODO Auto-generated destructor stub
}
