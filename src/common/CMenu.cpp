/*
 * CMenu.cpp
 *
 *  Created on: 16.09.2009
 *      Author: gerstrong
 */

#include "CMenu.h"

CMenu::CMenu() {
	// TODO Auto-generated constructor stub

}

CMenu::~CMenu() {
	// TODO Auto-generated destructor stub
}

bool CMenu::loadStartMenu(stCloneKeenPlus *pCKP)
{
	/*CDialog *GamesMenu = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 20);
	showmapatpos(90, 104<<4, 32, pCKP);

	// Use the standard Menu-Frame used in the old DOS-Games
	GamesMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );

	// Show me the games you detected!
	for( int i=0 ; i < pCKP->numGames ; i++ )
		GamesMenu->addObject(DLG_OBJ_OPTION_TEXT,1,i+1, pCKP->GameData[i].Name);

	do
	{
		gamedo_AnimatedTiles();
		GamesMenu->processlogic();
		GamesMenu->render();
	} while( !g_pInput->getPressedCommand(IC_JUMP) && !g_pInput->getPressedCommand(IC_STATUS) && !g_pInput->getExitEvent() );


	pCKP->Resources.GameSelected = GamesMenu->getSelection()+1;
	pCKP->Control.levelcontrol.episode = pCKP->GameData[GamesMenu->getSelection()].Episode;
	pCKP->Resources.GameDataDirectory = pCKP->GameData[GamesMenu->getSelection()].DataDirectory;

	delete GamesMenu;*/

	return true;
}
