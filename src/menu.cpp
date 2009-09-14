 /* MENU.C
  The main menu, intro, and other such stuff.
*/

#include "keen.h"
#include "pressf10.h"
#include "include/menu.h"
#include "include/misc.h"
#include "sdl/CVideoDriver.h"
#include "include/game.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "include/fileio.h"
#include "include/gm_pdowm.h"
#include "include/gamedo.h"
#include "include/main.h"
#include "CGraphics.h"
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

void showmapatpos(int level, int xoff, int yoff, stCloneKeenPlus *pCKP)
{
	int i;
	std::string levelname;
	g_pLogFile->ftextOut("showmapatpos(%d, %d, %d);<br>",level,xoff,yoff);
	pCKP->Control.levelcontrol.dark = 0;
	
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
}

short loadResourcesforStartMenu(stCloneKeenPlus *pCKP, CGame *Game)
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

	//  /* Load the graphics of the first game for displaying the menu */ /* Graphics of the first Episode are taken*/
	if(!pCKP->Control.skipstarting)
		pCKP->Control.levelcontrol.episode = 1;
	else
		pCKP->Control.levelcontrol.episode = pCKP->GameData[pCKP->Resources.GameSelected-1].Episode;

	Game->loadResources(pCKP->Control.levelcontrol.episode, pCKP->GameData[0].DataDirectory);

	player[0].x = player[0].y = 0;
	if(initgamefirsttime(pCKP, 0) != 0)
	{
		return 1;
	}
	initgame( &(pCKP->Control.levelcontrol) );

	return 0;
}


#define MAINMENU_GOTO_DEMO_TIME      4000

bool loadStartMenu(stCloneKeenPlus *pCKP)
{
	CDialog *GamesMenu = new CDialog(g_pVideoDriver->FGLayerSurface, /*16, 8,*/ 36, 20);
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

	delete GamesMenu;

	return true;
}

int loadmainmenu(stCloneKeenPlus *pCKP,int defaultopt)
{
	CDialog *MainMenu;
	int selection;
	
	map_redraw();
	
	// Prepare the Games Menu
	MainMenu = new CDialog(g_pVideoDriver->FGLayerSurface, /*88, 56,*/ 18, 13);
	
	// Use the standard Menu-Frame used in the old DOS-Games
	MainMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );
	
	// Show me the games you detected!
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1-Player Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "2-Player Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Load Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Options");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Back To Title");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, "Quit");
	
	do
	{
		gamedo_AnimatedTiles();
		MainMenu->processlogic();
		
		if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedKey(KENTER))
			break;
		if (g_pInput->getPressedKey(KQUIT))
		{
			QuitState = NO_QUIT;
			return MAINMNU_QUIT;
		}
		MainMenu->render();
	} while(!g_pInput->getExitEvent());
	
    if(g_pInput->getExitEvent())
	{
		QuitState = QUIT_PROGRAM;
    	return 0;
	}
	
	selection = MainMenu->getSelection();
	
	delete MainMenu;
	
    if (selection==MAINMNU_LOADGAME)
    {
    	int diff;
    	diff = getDifficulty(pCKP);
    	if(diff>=2)
    		return BACK2MAINMENU;
		
    	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;
		
    	loadslot = save_slot_box(0, pCKP, 1);
    }
    else if (selection==MAINMNU_OPTIONS)
    {
		if (configmenu(pCKP,1))
		{    // need to restart game
			return RESTART_GAME;
		}
	}
    else if(selection==MAINMNU_1PLAYER || selection==MAINMNU_2PLAYER)
    {
		
    	{
        	int diff;
        	diff = getDifficulty(pCKP);
			
        	if(diff>=2)
        		return BACK2MAINMENU;
			
        	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;
    	}
    }
	else if (selection==MAINMNU_QUIT)
    {
		QuitState = QUIT_PROGRAM;
    	return MAINMNU_QUIT;
    }
	
	return selection;
}


int mainmenu(stCloneKeenPlus *pCKP,int defaultopt)
{
	CDialog *MainMenu;
	CBitmap *bm_title, *bm_f1help;
	int x;
	int selection;
	SDL_Surface *sfc;

	for(unsigned int cp=0 ; cp<numplayers ; cp++)	// in some situations. the player is shown for a short time.
	{
		player[cp].x = 0;
		player[cp].y = 0;
	}

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Prepare the Games Menu
	MainMenu = new CDialog(g_pVideoDriver->FGLayerSurface, /*88, 56,*/ 18, 13);

	// Load the Title Bitmap
	sfc = g_pVideoDriver->getScrollSurface();
	bm_title = g_pGfxEngine->getBitmap("TITLE");
	bm_f1help = g_pGfxEngine->getBitmap("F1HELP");

	// Use the standard Menu-Frame used in the old DOS-Games
	MainMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );

	// Show me the games you detected!
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1-Player Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "2-Player Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Load Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Story");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "High Scores");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Options");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Demo");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Change Game");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "About CG");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Ordering Info");
	MainMenu->addObject(DLG_OBJ_OPTION_TEXT, 1, 11, "Quit");

	x = (320/2)-(bm_title->getWidth()/2);
	bm_title->draw( sfc, x+scroll_x, 1+scroll_y);

	// Draw Help Text-Screen
	if(pCKP->Control.levelcontrol.episode == 3)
		bm_f1help->draw( sfc, 128+scroll_x, 181+scroll_y);
	else
		bm_f1help->draw( sfc, 96+scroll_x, 181+scroll_y);

	do
	{
		gamedo_AnimatedTiles();
		MainMenu->processlogic();

		if(g_pInput->getPressedKey(KF1))
			showF1HelpText(pCKP->Control.levelcontrol.episode, pCKP->Resources.GameDataDirectory);
		else if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedKey(KENTER))
			break;
		MainMenu->render();
	} while(!g_pInput->getExitEvent());

    if(g_pInput->getExitEvent())
    	return MAINMNU_QUIT;

	selection = MainMenu->getSelection();

	delete MainMenu;

    if (selection==MAINMNU_LOADGAME)
    {
    	int diff;
    	diff = getDifficulty(pCKP);
    	if(diff>=2)
    		return BACK2MAINMENU;

    	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;

    	loadslot = save_slot_box(0, pCKP, 0);

    	x = (320/2)-(bm_title->getWidth()/2);
    	bm_title->draw( sfc, x+scroll_x, scroll_y+1);
		if(pCKP->Control.levelcontrol.episode == 3)
			bm_f1help->draw( sfc, 128+scroll_x, 181+scroll_y);
		else
			bm_f1help->draw( sfc, 96+scroll_x, 181+scroll_y);
    }
    else if (selection==MAINMNU_OPTIONS)
    {
      if (configmenu(pCKP,0))
      {    // need to restart game
         return RESTART_GAME;
      }
    }
    else if(selection==MAINMNU_1PLAYER || selection==MAINMNU_2PLAYER)
    {

    	{
        	int diff;
        	diff = getDifficulty(pCKP);

        	if(diff>=2)
        		return BACK2MAINMENU;

        	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;
    	}
    }

	return selection;
}

int getDifficulty(stCloneKeenPlus *pCKP)
{
	int x;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");

	//x = (320/2)-(bm_title->getWidth()/2);

	//bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );

	// Prepare the Games Menu
	CDialog DifficultyMenu(g_pVideoDriver->FGLayerSurface, /*120,32,*/ 14, 6);

	DifficultyMenu.setFrameTheme(DLG_THEME_OLDSCHOOL);

	DifficultyMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Normal");
	DifficultyMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Hard");
	DifficultyMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Cancel");

	do
	{
		// Render the Games-Menu
		DifficultyMenu.processlogic();
		if( g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP))
			break;

		g_pInput->pollEvents();
		gamedo_AnimatedTiles();
		DifficultyMenu.render();
	} while(true);

	return DifficultyMenu.getSelection();
}

int AudioDlg(stCloneKeenPlus *pCKP)
{
	int selection;
	int x;
	int ok=0;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");

	int rate = 0;
	Uint16 format = 0;
	short mode=0;

	// Load the Title Bitmap
	//x = (320/2)-(bm_title->getWidth()/2);
	//bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );

	// Prepare the Games Menu
	CDialog AudioMenu(g_pVideoDriver->FGLayerSurface, /*32, 32,*/ 32, 8);

	AudioMenu.setFrameTheme(DLG_THEME_OLDSCHOOL);

	rate = g_pSound->getAudioSpec().freq;
	AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Rate: " + itoa(rate) +" kHz");

	format = g_pSound->getAudioSpec().format;
	std::string buf;
	if(format == AUDIO_S16)
		buf = "Format: 16 bits";
	else
		buf = "Format: 8 bits";
	AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	mode = g_pSound->getAudioSpec().channels - 1;
	if(mode == 1)
		AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Mode: Stereo");
	else
		AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Mode: Mono");

	AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Save and go back");
	AudioMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Cancel");


	do
	{
		gamedo_AnimatedTiles();

		if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP))
		{
			selection = AudioMenu.getSelection();

			if(selection == 0)
			{
				switch(rate)
				{
				case 22050: rate = 44100; break;
				case 11025: rate = 22050; break;
				default: rate = 11025; break;
				}
				AudioMenu.setObjectText(0, "Rate: " + itoa(rate) + " kHz");
			}

			if(selection == 1)
			{
				if( format == AUDIO_S16 ) format = AUDIO_U8;
				else if( format == AUDIO_U8 ) format = AUDIO_S16;
				if(format == AUDIO_S16)
					buf = "Format: 16 bits";
				else
					buf = "Format: 8 bits";
				AudioMenu.setObjectText(1, buf);
			}

			if(selection == 2)
			{
				mode = !mode;
				if(!mode)
					AudioMenu.setObjectText(2,"Mode: Mono");
				else
					AudioMenu.setObjectText(2,"Mode: Stereo");
			}

			if(selection == 3)
			{
				g_pSound->destroy();
				g_pSound->setSoundmode(rate, mode ? true : false, format);
				CSettings *Settings;
				Settings = new CSettings();
				Settings->saveDrvCfg();
				delete Settings; Settings = NULL;
				g_pSound->init();
				ok = g_pSound->loadSoundData(pCKP->Control.levelcontrol.episode,
											  pCKP->Resources.GameDataDirectory);
				break;
			}
			if(selection == 4)
				break;

		}
		AudioMenu.processlogic();
		AudioMenu.render();
	} while(1);

	return ok;
}

void OptionsDlg(stCloneKeenPlus *pCKP)
{
	int selection;
	int x,i;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");

	std::string buf;

	// Load the Title Bitmap
	//x = (320/2)-(bm_title->getWidth()/2);
	//bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );

	// Prepare the Games Menu
	CDialog OptionsMenu(g_pVideoDriver->FGLayerSurface, /*24, 24,*/ 34, 13);

	OptionsMenu.setFrameTheme(DLG_THEME_OLDSCHOOL);

	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		buf = options[i].name + ": ";

		if(options[i].value)
			buf += "Enabled";
		else
			buf += "Disabled";

		OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, buf);
	}

	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, i+2, "Save and Continue");
	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, i+3, "Cancel");

	do
	{
		gamedo_AnimatedTiles();

		if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP))
		{
			selection = OptionsMenu.getSelection();

			if(selection < NUM_OPTIONS)
			{
				buf = options[selection].name + ": ";

				if(options[selection].value)
				{
					options[selection].value = 0;
					buf += "Disabled";
				}
				else
				{
					options[selection].value = 1;
					buf += "Enabled";
				}

				OptionsMenu.setObjectText(selection, buf);
			}
			else if(selection < NUM_OPTIONS+1)
			{
				CSettings Settings; // Pressed Save,
				Settings.saveGameCfg(pCKP->Option);
				break;
			}
			else
			{
				// Pressed Cancel, don't save
				break;
			}
		}

		OptionsMenu.processlogic();
		OptionsMenu.render();
	} while(1);
}

short GraphicsDlg(stCloneKeenPlus *pCKP, int ingame)
{
	int selection;
	int x;
	Uint16 width, height;
	Uint8 depth, zoom = 1, filter = 0, gl_filter = 0;
	bool fsmode, aspect;
	std::string buf;
	short retval = 0;
	Uint8 autoframeskip = 0;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE"), *bm_f1help = g_pGfxEngine->getBitmap("F1HELP");

	// Load the Title Bitmap
	x = (320/2)-(bm_title->getWidth()/2);
	//bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );

	width  = g_pVideoDriver->getWidth();
	height = g_pVideoDriver->getHeight();
	depth  = g_pVideoDriver->getDepth();

	zoom   = g_pVideoDriver->getZoomValue();
	filter = g_pVideoDriver->getFiltermode();

	// Prepare the Games Menu
	CDialog DisplayMenu(g_pVideoDriver->FGLayerSurface, /*4, 4,*/ 32, 12);

	// Use the standard Menu-Frame used in the old DOS-Games
	DisplayMenu.setFrameTheme( DLG_THEME_OLDSCHOOL );

	buf = "Resolution: " + itoa(width) + "x" + itoa(height) + "x" + itoa(depth);
	DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
	if(g_pVideoDriver->getFullscreen())
	{
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Fullscreen mode");
		fsmode = true;
	}
	else
	{
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Windowed mode");
		fsmode = false;
	}

	bool opengl = g_pVideoDriver->isOpenGL();
	if(!opengl)
	{
		zoom = g_pVideoDriver->getZoomValue();

		if(zoom == 1)
			DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "No Scale");
		else
			DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Scale: " + itoa(zoom) );
	}
	else
	{
		gl_filter = g_pVideoDriver->getOGLFilter();

		if(gl_filter == 1)
			DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "OGL Filter: Linear");
		else
			DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "OGL Filter: Nearest");
	}

	filter = g_pVideoDriver->getFiltermode();
	if(filter == 0)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "No Filter");
	else if(filter == 1)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale2x Filter");
	else if(filter == 2)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale3x Filter");
	else if(filter == 3)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale4x Filter");
	else
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Unknown Filter");

	if(opengl)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "OpenGL Acceleration");
	else
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Software Rendering");

	autoframeskip = g_pTimer->getFrameRate();

	if(autoframeskip)
		DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Auto-Frameskip : " + itoa(autoframeskip) + " fps");

	aspect = g_pVideoDriver->getAspectCorrection();

	buf = "OGL Aspect Ratio ";
	buf += aspect ? "enabled" : "disabled";

	DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

	DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 9, "Save and return");
	DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Cancel");

	do
	{
		gamedo_AnimatedTiles();
		
		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = DisplayMenu.getSelection();

			if(selection == 0)
			{
				// Now the part of the resolution list
				st_resolution Resolution;
				Resolution = g_pVideoDriver->setNextResolution();

				buf = "Resolution: " + itoa(Resolution.width) + "x" + itoa(Resolution.height) + "x" + itoa(Resolution.depth);
				DisplayMenu.setObjectText(0,buf);
			}
			else if(selection == 1)
			{
				if(!fsmode)
					DisplayMenu.setObjectText(1,"Fullscreen mode");
				else
					DisplayMenu.setObjectText(1,"Windowed mode");
				fsmode = !fsmode;
			}
			else if(selection == 2)
			{
				if(opengl)
				{
					gl_filter = (gl_filter==1) ? 0 : 1;
					buf = (gl_filter == 1) ? "OGL Filter: Linear" : "OGL Filter: Nearest";
					DisplayMenu.setObjectText(2,buf);
				}
				else
				{
					zoom = (zoom >= 4) ? 1 : zoom+1;
					buf = (zoom == 1) ? "No scale" : "Scale: " + itoa(zoom);
				}
				DisplayMenu.setObjectText(2,buf);
			}
			else if(selection == 3)
			{
				filter = (filter >= 3) ? 0 : filter+1;

				if(filter == 0)
					DisplayMenu.setObjectText(3,"No Filter");
				else if(filter == 1)
					DisplayMenu.setObjectText(3,"Scale2x Filter");
				else if(filter == 2)
					DisplayMenu.setObjectText(3,"Scale3x Filter");
				else if(filter == 3)
					DisplayMenu.setObjectText(3,"Scale4x Filter");
			}
			else if(selection == 4)
			{
				opengl = !opengl; // switch the mode!!

				if(opengl)
					DisplayMenu.setObjectText(4,"OpenGL Acceleration");
				else
					DisplayMenu.setObjectText(4,"Software Rendering");
			}
			else if(selection == 5)
			{
				if(autoframeskip < 120)
					autoframeskip += 10;
				else
					autoframeskip = 10;
				buf = "Auto-Frameskip : " + itoa(autoframeskip) + " fps";

				DisplayMenu.setObjectText(5, buf);
			}
			else if(selection == 6)
			{
				aspect = !aspect;

				if(aspect)
					DisplayMenu.setObjectText(6,"OGL Aspect Ratio Enabled");
				else
					DisplayMenu.setObjectText(6,"OGL Aspect Ratio Disabled");

			}
			else if(selection == 7)
			{
				g_pVideoDriver->stop();

				if(fsmode)
					g_pVideoDriver->isFullscreen(true);
				else
					g_pVideoDriver->isFullscreen(false);

				g_pVideoDriver->enableOpenGL(opengl);
				g_pVideoDriver->setOGLFilter(gl_filter);
				g_pVideoDriver->setZoom(zoom);
				g_pVideoDriver->setFilter(filter);
				g_pTimer->setFrameRate(autoframeskip);
				g_pVideoDriver->setAspectCorrection(aspect);

				// initialize/activate all drivers
				g_pLogFile->ftextOut("Restarting graphics driver... (Menu)<br>");
				if (g_pVideoDriver->start())
					retval = 1;

				CSettings *Settings;
				Settings = new CSettings();

				Settings->saveDrvCfg();
				delete Settings; Settings = NULL;
				map_redraw();
				if (ingame == 0)
				{
				bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );
				if(pCKP->Control.levelcontrol.episode == 3)
					bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 128+scroll_x, 181+scroll_y);
				else
					bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 96+scroll_x, 181+scroll_y);
				}
				break;
			}
			else
				break;
		}
		// Render the Games-Menu
		DisplayMenu.processlogic();
		// blit the scrollbuffer to the display
		DisplayMenu.render();
	} while(1);

	return retval;
}

char configmenu(stCloneKeenPlus *pCKP,int ingame)
{
	int selection;
	int x;
	CBitmap *title_bitmap = g_pGfxEngine->getBitmap("TITLE"), *bm_f1help = g_pGfxEngine->getBitmap("F1HELP");

	// Load the Title Bitmap

	//x = (320/2)-(title_bitmap->getWidth()/2);
	//title_bitmap->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );
	
	// Draw Help Text-Screen
	//if(pCKP->Control.levelcontrol.episode == 3)
	//	bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 128+scroll_x, 181+scroll_y);
	//else
	//	bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 96+scroll_x, 181+scroll_y);

	// Prepare the Games Menu
	CDialog OptionsMenu(g_pVideoDriver->FGLayerSurface, /*120, 32,*/ 14, 8);

	OptionsMenu.setFrameTheme(DLG_THEME_OLDSCHOOL);

	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Game");
	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");

	OptionsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Back");

	do
	{
		gamedo_AnimatedTiles();

		if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP))
		{
			selection = OptionsMenu.getSelection();

			if(selection == 4)
				break;

			switch(selection)
			{
			case 0:
				GraphicsDlg(pCKP,ingame);
				break;

			case 1:
				AudioDlg(pCKP);
				break;

			case 2:
				OptionsDlg(pCKP);
				break;

			case 3:
				controlsmenu();
				break;

			default:
				break;
			}
			//map_redraw();
			//title_bitmap->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );
			//if(pCKP->Control.levelcontrol.episode == 3)
			//	bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 128+scroll_x, 181+scroll_y);
			//else
			//	bm_f1help->draw( g_pVideoDriver->getScrollSurface(), 96+scroll_x, 181+scroll_y);
			OptionsMenu.setSDLSurface(g_pVideoDriver->FGLayerSurface);
		}
		OptionsMenu.processlogic();
		OptionsMenu.render();
	} while(1);

	return 0;
}

char controlsmenu()
{
	int selection;
	int x;
	std::string buf;
	std::string buf2;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");

	//x = (320/2)-(bm_title->getWidth()/2);
	//bm_title->draw( g_pVideoDriver->getScrollSurface(), x+scroll_x, scroll_y+1 );

	// Prepare the Games Menu
	CDialog ControlsMenu(g_pVideoDriver->FGLayerSurface, /*8, 16,*/ 38, 22);

	// Use the standard Menu-Frame used in the old DOS-Games
	ControlsMenu.setFrameTheme( DLG_THEME_OLDSCHOOL );

	g_pInput->getEventName(IC_LEFT, 0, buf2);
	buf = "P1 Left:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);

	g_pInput->getEventName(IC_UP, 0, buf2);
	buf = "P1 Up:     " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);

	g_pInput->getEventName(IC_RIGHT, 0, buf2);
	buf = "P1 Right:  " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);

	g_pInput->getEventName(IC_DOWN, 0, buf2);
	buf = "P1 Down:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);

	g_pInput->getEventName(IC_JUMP, 0, buf2);
	buf = "P1 Jump:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);

	g_pInput->getEventName(IC_POGO, 0, buf2);
	buf = "P1 Pogo:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);

	g_pInput->getEventName(IC_FIRE, 0, buf2);
	buf = "P1 Fire:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);

	g_pInput->getEventName(IC_STATUS, 0, buf2);
	buf = "P1 Status: " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 8, buf);


	g_pInput->getEventName(IC_LEFT, 1, buf2);
	buf = "P2 Left:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 9, buf);

	g_pInput->getEventName(IC_UP, 1, buf2);
	buf = "P2 Up:     " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 10, buf);

	g_pInput->getEventName(IC_RIGHT, 1, buf2);
	buf = "P2 Right:  " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 11, buf);

	g_pInput->getEventName(IC_DOWN, 1, buf2);
	buf = "P2 Down:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 12, buf);

	g_pInput->getEventName(IC_JUMP, 1, buf2);
	buf = "P2 Jump:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 13, buf);

	g_pInput->getEventName(IC_POGO, 1, buf2);
	buf = "P2 Pogo:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 14, buf);

	g_pInput->getEventName(IC_FIRE, 1, buf2);
	buf = "P2 Fire:   " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 15, buf);

	g_pInput->getEventName(IC_STATUS, 1, buf2);
	buf = "P2 Status: " + buf2;
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 16, buf);

	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 2, 17, "Reset Controls");
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 19, "Save and return");
	ControlsMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 20, "Cancel");

	do
	{
		gamedo_AnimatedTiles();
		
		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = ControlsMenu.getSelection();

			if(selection < MAX_COMMANDS)
			{
				int item=0;
				if(selection < 4)
					item = selection + 4;
				else
					item = selection - 4;

				switch(selection)
				{
				  case 0: buf = "P1 Left:   "; break;
				  case 1: buf = "P1 Up:     "; break;
				  case 2: buf = "P1 Right:  "; break;
				  case 3: buf = "P1 Down:   "; break;
				  case 4: buf = "P1 Jump:   "; break;
				  case 5: buf = "P1 Pogo:   "; break;
				  case 6: buf = "P1 Fire:   "; break;
				  case 7: buf = "P1 Status: "; break;
				}

				buf2 = buf;
				buf2 += "*Waiting for Input*";
				ControlsMenu.setObjectText(selection, buf2);

				while(!g_pInput->readNewEvent(0,item))
				{
					ControlsMenu.processlogic();
					ControlsMenu.render();
				}

				g_pInput->getEventName(item, 0, buf2);
				buf += buf2;
				ControlsMenu.setObjectText(selection,buf);
			}
			else if(selection >= MAX_COMMANDS && selection < MAX_COMMANDS*2)
			{
				int item=0;
				if(selection < (4 + MAX_COMMANDS))
					item = selection + 4 - MAX_COMMANDS;
				else
					item = selection - 4 - MAX_COMMANDS;

				switch(selection)
				{
				  case 0+ MAX_COMMANDS: buf = "P2 Left:   "; break;
				  case 1+ MAX_COMMANDS: buf = "P2 Up:     "; break;
				  case 2+ MAX_COMMANDS: buf = "P2 Right:  "; break;
				  case 3+ MAX_COMMANDS: buf = "P2 Down:   "; break;
				  case 4+ MAX_COMMANDS: buf = "P2 Jump:   "; break;
				  case 5+ MAX_COMMANDS: buf = "P2 Pogo:   "; break;
				  case 6+ MAX_COMMANDS: buf = "P2 Fire:   "; break;
				  case 7+ MAX_COMMANDS: buf = "P2 Status: "; break;
				}

				buf2 = buf;
				buf2 += "*Waiting for Input*";
				ControlsMenu.setObjectText(selection,buf2);

				while(!g_pInput->readNewEvent(1,item))
				{
					ControlsMenu.processlogic();
					ControlsMenu.render();
				}

				g_pInput->getEventName(item, 1, buf2);
				buf += buf2;
				ControlsMenu.setObjectText(selection,buf);
			}
			else if(selection == MAX_COMMANDS*2)
			{
				// Reset Controls here!
				g_pInput->resetControls();
				g_pInput->saveControlconfig();
				break;
			}
			else if(selection == MAX_COMMANDS*2+1)
			{
				g_pInput->saveControlconfig();
				break;
			}
			else
			{
				break;
			}
		}
		// Render the Menu
		ControlsMenu.processlogic();
		ControlsMenu.render();
	} while(1);
	return 0;
}

// This function shows the Story of Commander Keen!
void showPage(const std::string& str_text, int textsize)
{
	CTextViewer *TextViewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 136);
	TextViewer->loadText(str_text);

	AllPlayersInvisible();

	TextViewer->processCycle();
	delete TextViewer;

    return;
}

void keensleft(int episode)
{
bool enter;
unsigned int p;
int x,y,i;
int boxY, boxH;
int boxtimer;

  SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

  #define KEENSLEFT_TIME        400

  for(i=0;i<MAX_PLAYERS;i++)
  {
    if (player[i].isPlaying)
    {
      gamepdo_wm_SelectFrame(i, episode);
      player[i].hideplayer = 0;
    }
  }

  #define KEENSLEFT_X        7
  #define KEENSLEFT_Y        11
  #define KEENSLEFT_W        24
  #define KEENSLEFT_H        4

  boxY = KEENSLEFT_Y - (numplayers);
  boxH = KEENSLEFT_H + (numplayers * 2);

  g_pSound->playSound(SOUND_KEENSLEFT, PLAY_NOW);

  boxtimer = 0;
  do
  {
	  if( g_pGfxEngine->Palette.m_fade_in_progess )
		  g_pGfxEngine->Palette.applyFade();

	  g_pGfxEngine->drawDialogBox(g_pVideoDriver->FGLayerSurface, KEENSLEFT_X, boxY,KEENSLEFT_W,boxH);
	  g_pGfxEngine->Font->drawFont( boxsurface, getstring("LIVES_LEFT_BACKGROUND"),(KEENSLEFT_X+1)*8,(boxY+1)*8,0);
	  g_pGfxEngine->Font->drawFont( boxsurface, getstring("LIVES_LEFT"),((KEENSLEFT_X+7)*8)+4,(boxY+1)*8,0);
	  y = ((boxY+2)*8)+4;
	  if (numplayers>1) y--;
	  for(p=0;p<numplayers;p++)
	  {
	    x = ((KEENSLEFT_X+1)*8)+4;
	    for(i=0;i<player[p].inventory.lives&&i<=10;i++)
	    {
	    	Uint16 f = PMAPDOWNFRAME+playerbaseframes[p]-(episode==3);
	    	g_pGfxEngine->Sprite[f]->drawSprite(g_pVideoDriver->FGLayerSurface, x, y );
		      x+=16;
	    }
	    y+=18;
	  }
	  //g_pVideoDriver->update_screen();
	  if (boxtimer > KEENSLEFT_TIME)
	  {
		  break;
	  } else boxtimer++;
	  enter = g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_FIRE)||
			  g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_POGO);
	  if (enter)
		  break;
	  if (g_pInput->getPressedKey(KQUIT))
		  return;

	  g_pInput->pollEvents();

	  gamedo_RenderScreen();
  } while(!enter);

}
