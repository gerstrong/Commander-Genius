 /* CStartScreen.C
  The title screen for each game is displayed through this.
*/

#include "keen.h"
#include "include/CStartScreen.h"
#include "include/misc.h"
#include "sdl/CVideoDriver.h"
#include "game.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "include/fileio.h"
#include "include/gm_pdowm.h"
#include "include/gamedo.h"
#include "include/main.h"
#include "sdl/video/colourtable.h"
#include "include/gui/dialog.h"
#include "sdl/CInput.h"
#include "dialog/CDialog.h"
#include "CLogFile.h"
#include "sdl/CSettings.h"
#include "dialog/CTextViewer.h"
#include "graphics/CGfxEngine.h"

#include <SDL.h>
#include <iostream>
#include <fstream>
#include "StringUtils.h"
#include "FindFile.h"
#include <vector>

#define SELMOVE_SPD         3

short openDlgStruct(stDlgStruct *pDlgStruct, stCloneKeenPlus *pCKP);

// TODO: This function is old. Try to remove it for future implementations
/*void showmapatpos(int level, int xoff, int yoff, stCloneKeenPlus *pCKP)
{
	int i;
	std::string levelname;
	g_pLogFile->ftextOut("showmapatpos(%d, %d, %d);<br>",level,xoff,yoff);
	pCKP->Control.levelcontrol.dark = false;

	initgame( &(pCKP->Control.levelcontrol) );           // reset scroll
	levelname = "level" + FixedWidthStr_LeftFill(itoa(level), 2, '0') + ".ck" + itoa(pCKP->Control.levelcontrol.episode);

	short numsel;
	if(pCKP->Resources.GameSelected == 0 ) // First time startup. No game has been chosen
		numsel = 0;
	else
		numsel = pCKP->Resources.GameSelected-1;

	if(loadmap(levelname, pCKP->GameData[numsel].DataDirectory, level, &(pCKP->Control.levelcontrol) ) != 0)
		return;

	drawmap();
	for(i=0;i<xoff;i++) map_scroll_right();
	for(i=0;i<yoff;i++) map_scroll_down();

	gamedo_frameskipping_blitonly();
}*/

/*short loadResourcesforStartMenu(stCloneKeenPlus *pCKP, CGame *Game)
{
	std::string line;

	std::ifstream gamescfg; OpenGameFileR(gamescfg, "games/games.cfg");

    if (gamescfg.is_open())
	{
	   while ( !gamescfg.eof() && pCKP->numGames < 20 )
	   {
	     getline (gamescfg,line);

		 if(line != "" && line[0] == '[')
		 {
			 pCKP->numGames++;
			 stGameData* NewGameData = new stGameData[pCKP->numGames];
			 for(int i = 0; i < pCKP->numGames-1; ++i)
				 NewGameData[i] = pCKP->GameData[i];

			 delete[] pCKP->GameData;

			 pCKP->GameData = NewGameData;
		 }
		 if(strncmp(line.c_str(),"Name=",strlen("Name=")) == 0)
		 {
			 pCKP->GameData[pCKP->numGames-1].Name = line.substr(strlen("Name="));
		 }
		 if(strncmp(line.c_str(),"Episode=",strlen("Episode=")) == 0)
		 {
		 	sscanf(line.c_str(),"Episode=%hd", &(pCKP->GameData[pCKP->numGames-1].Episode));
		 }
		 if(strncmp(line.c_str(),"Path=",strlen("Path=")) == 0)
		 {
			 unsigned short l = strlen("Path=");
			 pCKP->GameData[pCKP->numGames-1].DataDirectory = line.substr(l);
 		 }
	   }
	   gamescfg.close();
	}
    else
    {
    	g_pLogFile->ftextOut(RED,"loadResourcesforStartMenu(): \"games/games.cfg\" could not be read! Assure, that the directory can be accessed.");
    	return -1;
    }

	if(  pCKP->numGames >= 20 )
		g_pLogFile->ftextOut(PURPLE,"parseTheGames(): Warning! Number of games limit in \"games/games.cfg\" reached.");

	if(pCKP->numGames == 0)
	{
		g_pLogFile->ftextOut(PURPLE,"parseTheGames(): In the file \"games/games.cfg\" no games were found.");
		return -1;
	}

	unsigned short c=0;
	for(c=0 ; c < pCKP->numGames ; c++)
	{
		checkConsistencyofGameData(&(pCKP->GameData[c]));
	}

	//   Load the graphics of the first game for displaying the menu */ /* Graphics of the first Episode are taken
	if(!pCKP->Control.skipstarting)
		pCKP->Control.levelcontrol.episode = 1;
	else
		pCKP->Control.levelcontrol.episode = pCKP->GameData[pCKP->Resources.GameSelected-1].Episode;

	//Game->loadResources(pCKP->Control.levelcontrol.episode, pCKP->GameData[0].DataDirectory);

	//mp_player[0].x = mp_player[0].y = 0;
	if(initgamefirsttime(pCKP, 0) != 0)
	{
		return 1;
	}
	initgame( &(pCKP->Control.levelcontrol) );

	return 0;
}*/


#define MAINMENU_GOTO_DEMO_TIME      4000

bool loadStartMenu(stCloneKeenPlus *pCKP)
{
	/*
    // Prepare the Games Menu
	CDialog GamesMenu(g_pVideoDriver->FGLayerSurface,  36, 20);


	//showmapatpos(90, 104<<4, 32, pCKP);

	// Use the standard Menu-Frame used in the old DOS-Games
	GamesMenu.setFrameTheme( DLG_THEME_OLDSCHOOL );

	// Show me the games you detected!
	for( int i=0 ; i < pCKP->numGames ; i++ )
		GamesMenu.addObject(DLG_OBJ_OPTION_TEXT,1,i+1, pCKP->GameData[i].Name);

	do
	{
		gamedo_AnimatedTiles();
		GamesMenu.processInput();
		GamesMenu.draw();
	} while( !g_pInput->getPressedCommand(IC_JUMP) && !g_pInput->getPressedCommand(IC_STATUS) && !g_pInput->getExitEvent() );


	pCKP->Resources.GameSelected = GamesMenu.getSelection()+1;
	pCKP->Control.levelcontrol.episode = pCKP->GameData[GamesMenu.getSelection()].Episode;
	pCKP->Resources.GameDataDirectory = pCKP->GameData[GamesMenu.getSelection()].DataDirectory;
*/
	return true;

}
