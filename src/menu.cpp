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
#include "vorticon/CDialog.h"
#include "CLogFile.h"
#include "sdl/CSettings.h"
#include "dialog/CTextViewer.h"

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
	g_pGraphics->initPalette(pCKP->Control.levelcontrol.dark);
	
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
	
	g_pVideoDriver->sb_blit();
}

short loadResourcesforStartMenu(stCloneKeenPlus *pCKP, CGame *Game)
{
	std::string line;

	std::ifstream gamescfg; OpenGameFileR(gamescfg, "data/games.cfg");

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
    	g_pLogFile->ftextOut(RED,"loadResourcesforStartMenu(): \"data/games.cfg\" could not be read! Assure, that the directory can be accessed.");
    	return -1;
    }

	if(  pCKP->numGames >= 20 )
		g_pLogFile->ftextOut(PURPLE,"parseTheGames(): Warning! Number of games limit in \"data/games.cfg\" reached.");

	if(pCKP->numGames == 0)
	{
		g_pLogFile->ftextOut(PURPLE,"parseTheGames(): In the file \"data/games.cfg\" no games were found.");
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

    //if (latch_loadgraphics(pCKP->Control.levelcontrol.episode, pCKP->GameData[0].DataDirectory)) return abortCKP(pCKP);
	//if (Game->getLatch()->loadGraphics(pCKP->Control.levelcontrol.episode, pCKP->GameData[0].DataDirectory)) return abortCKP(pCKP);
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

extern char fade_black;
bool loadStartMenu(stCloneKeenPlus *pCKP)
{
	CDialog *GamesMenu;
	bool ret = true;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;
	showmapatpos(90, (104 << 2)+256+256+80, 32-4, pCKP);

	// Prepare the Games Menu
	GamesMenu = new CDialog();

	GamesMenu->setDimensions(2,2,36,15);

	// Show me the games you detected!
	for( int i=0 ; i < pCKP->numGames ; i++ )
		GamesMenu->addOptionText(pCKP->GameData[i].Name);

	GamesMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			g_pInput->pollEvents();

			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				GamesMenu->setVisible(true);

			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				GamesMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				GamesMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(0, IC_STATUS))
			{
				fade.dir = FADE_OUT;
				fade.curamt = PAL_FADE_SHADES;
				fade.fadetimer = 0;
				fade.rate = FADE_NORM;
				fade.mode = FADE_GO;

				if( GamesMenu->getSelection() < pCKP->numGames )
				{
					ret = true;
					pCKP->Resources.GameSelected = GamesMenu->getSelection()+1;
					pCKP->Control.levelcontrol.episode = pCKP->GameData[pCKP->Resources.GameSelected-1].Episode;
				}
				else ret = false;
				break;
			}
			// Render the Games-Menu
			GamesMenu->renderDialog();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while(!g_pInput->getExitEvent());

	delete GamesMenu;

	return ret;
}

int mainmenu(stCloneKeenPlus *pCKP,int defaultopt)
{

	CDialog *MainMenu;
	int bmnum;
	int x;
	int selection;

	for(unsigned int cp=0 ; cp<numplayers ; cp++)	// in some situations. the player is shown for a short time.
	{
		player[cp].x = 0;
		player[cp].y = 0;
	}

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;
	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Prepare the Games Menu
	MainMenu = new CDialog();

	MainMenu->setDimensions(11,7,18,13);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	MainMenu->addOptionText("1-Player Game");
	MainMenu->addOptionText("2-Player Game");
	MainMenu->addOptionText("Load Game");
	MainMenu->addOptionText("Story");
	MainMenu->addOptionText("High Scores");
	MainMenu->addOptionText("Options");
	MainMenu->addOptionText("Demo");
	MainMenu->addOptionText("Change Game");
	MainMenu->addOptionText("About CG");
	MainMenu->addOptionText("Ordering Info");
	MainMenu->addOptionText("Quit");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	MainMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			g_pInput->pollEvents();

			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				MainMenu->setVisible(true);

			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				MainMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				MainMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = MainMenu->getSelection();
				break;
			}


			// Render the Games-Menu
			MainMenu->renderDialog();

			if(g_pInput->getExitEvent())
			{
				delete MainMenu;
				return MAINMNU_QUIT;
			}
		}

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while(1);

    if (selection==MAINMNU_LOADGAME)
    {
    	int diff;
    	diff = getDifficulty(pCKP);
    	if(diff>2)
    		return BACK2MAINMENU;

    	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;

    	loadslot = save_slot_box(0, pCKP);
    	if (loadslot)
    	{
    		fade.dir = FADE_OUT;
    		fade.curamt = PAL_FADE_SHADES;
    		fade.fadetimer = 0;
    		fade.rate = FADE_NORM;
    		fade.mode = FADE_GO;
    	}
    	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
    	x = (320/2)-(bitmaps[bmnum].xsize/2);
    	g_pGraphics->drawBitmap(x, 0, bmnum);
    }
    else if (selection==MAINMNU_OPTIONS)
    {
      if (configmenu(pCKP))
      {    // need to restart game
         return RESTART_GAME;
      }
    }
    else
    {
    	if(selection==MAINMNU_1PLAYER || selection==MAINMNU_2PLAYER)
    	{
        	int diff;
        	diff = getDifficulty(pCKP);

        	if(diff>2)
        	{
        		delete MainMenu;
        		return BACK2MAINMENU;
        	}
        	pCKP->Control.levelcontrol.hardmode = (diff == 1) ? true : false;
    	}

    	fade.dir = FADE_OUT;
    	fade.curamt = PAL_FADE_SHADES;
    	fade.fadetimer = 0;
    	fade.rate = FADE_NORM;
    	fade.mode = FADE_GO;
    }

	delete MainMenu;

	return selection;
}

int getDifficulty(stCloneKeenPlus *pCKP)
{
	int bmnum;
	int selection;
	int x;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	CDialog DifficultyMenu;

	DifficultyMenu.setDimensions(15,4,14,6);

	DifficultyMenu.addOptionText("Normal");
	DifficultyMenu.addOptionText("Hard");
	DifficultyMenu.addSeparator();
	DifficultyMenu.addOptionText("Cancel");

	DifficultyMenu.animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				DifficultyMenu.setVisible(true);

			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				DifficultyMenu.setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				DifficultyMenu.setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = DifficultyMenu.getSelection();
				break;
			}
			// Render the Games-Menu
			DifficultyMenu.renderDialog();
			g_pInput->pollEvents();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(true);

	return selection;
}

int AudioDlg(stCloneKeenPlus *pCKP)
{
	CDialog *AudioMenu;
	int bmnum;
	int selection;
	int x;
	int ok=0;

	int rate=0;
	short mode=0;

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
	x = (320/2)-(bitmaps[bmnum].xsize/2);
	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	AudioMenu = new CDialog();
	AudioMenu->setDimensions(4,4,32,7);

	char buf[256];
	rate = g_pSound->getAudioSpec().freq;
	sprintf(buf,"Rate: %d kHz",rate);
	AudioMenu->addOptionText(buf);
	mode = g_pSound->getAudioSpec().channels - 1;
	if(mode == 1)
		AudioMenu->addOptionText("Mode: Stereo");
	else
		AudioMenu->addOptionText("Mode: Mono");
	AudioMenu->addSeparator();
	AudioMenu->addOptionText("Save and go back");
	AudioMenu->addOptionText("Cancel");

	AudioMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				AudioMenu->setVisible(true);
			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				AudioMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				AudioMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = AudioMenu->getSelection();

				if(selection == 0)
				{
					switch(rate)
					{
					case 44100: rate = 48000; break;
					case 22050: rate = 44100; break;
					case 11000: rate = 22050; break;
					default: rate = 11000; break;
					}

					sprintf(buf,"Rate: %d kHz",rate);
					AudioMenu->setOptionText(0,buf);
				}

				if(selection == 1)
				{
					mode = !mode;
					if(!mode)
						AudioMenu->setOptionText(1,"Mode: Mono");
					else
						AudioMenu->setOptionText(1,"Mode: Stereo");
				}

				if(selection == 3)
				{
					g_pSound->destroy();
					g_pSound->setSoundmode(rate, mode ? true : false);
					CSettings *Settings;
					Settings = new CSettings();
					Settings->saveDrvCfg();
					delete Settings; Settings = NULL;
					g_pSound->init();
					ok = g_pSound->loadSoundData(pCKP->Control.levelcontrol.episode,
							pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);

					break;
				}
				if(selection == 4)
					break;

			}
			// Render the Games-Menu
			AudioMenu->renderDialog();
			g_pInput->pollEvents();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while(1);

	delete AudioMenu;
	return ok;
}

void OptionsDlg(stCloneKeenPlus *pCKP)
{
	CDialog *OptionsMenu;
	int bmnum;
	int selection;
	int x,i;

	char buf[256];

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
	x = (320/2)-(bitmaps[bmnum].xsize/2);
	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	OptionsMenu = new CDialog();
	OptionsMenu->setDimensions(3,3,34,12);

	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		sprintf(buf,"%s: ",options[i].name);
		if(options[i].value)
			strcat(buf,"Enabled");
		else
			strcat(buf,"Disabled");

		OptionsMenu->addOptionText(buf);
	}

	OptionsMenu->addSeparator();
	OptionsMenu->addOptionText("Return");

	OptionsMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				OptionsMenu->setVisible(true);
			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				OptionsMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				OptionsMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = OptionsMenu->getSelection();

				if(selection < NUM_OPTIONS)
				{
					sprintf(buf,"%s: ",options[selection].name);

					if(options[selection].value)
					{
						options[selection].value = 0;
						strcat(buf,"Disabled");
					}
					else
					{
						options[selection].value = 1;
						strcat(buf,"Enabled");
					}

					OptionsMenu->setOptionText(selection,buf);
				}
				else
				{
					CSettings Settings;
					Settings.saveGameCfg(pCKP->Option);
					break;
				}
			}
			// Render the Games-Menu
			OptionsMenu->renderDialog();
			g_pInput->pollEvents();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(1);

	delete OptionsMenu;
}

short GraphicsDlg(stCloneKeenPlus *pCKP)
{
	CDialog *DisplayMenu;
	int bmnum;
	int selection;
	int x;
	unsigned int width;
	unsigned int height;
	unsigned short depth;
	unsigned short zoom = 1;
	unsigned short filter = 0;
	unsigned char gl_filter = 0;
	bool fsmode;
	char buf[256];
	short retval = 0;
	unsigned char autoframeskip = 0;
	bool aspect;

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
	x = (320/2)-(bitmaps[bmnum].xsize/2);
	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	DisplayMenu = new CDialog();
	DisplayMenu->setDimensions(4,3,32,13);

	width  = g_pVideoDriver->getWidth();
	height = g_pVideoDriver->getHeight();
	depth  = g_pVideoDriver->getDepth();
	sprintf(buf,"Resolution: %dx%dx%d",width,height,depth);

	zoom   = g_pVideoDriver->getZoomValue();
	filter = g_pVideoDriver->getFiltermode();

	DisplayMenu->addOptionText(buf);
	if(g_pVideoDriver->getFullscreen())
	{
		DisplayMenu->addOptionText("Fullscreen mode");
		fsmode = true;
	}
	else
	{
		DisplayMenu->addOptionText("Windowed mode");
		fsmode = false;
	}

	bool opengl = g_pVideoDriver->isOpenGL();
	if(!opengl)
	{
		zoom = g_pVideoDriver->getZoomValue();

		if(zoom == 1)
			sprintf(buf,"No scale");
		else
			sprintf(buf,"Scale: %d", zoom);
		DisplayMenu->addOptionText(buf);
	}
	else
	{
		gl_filter = g_pVideoDriver->getOGLFilter();

		if(gl_filter == 1)
			sprintf(buf,"OGL Filter: Linear");
		else
			sprintf(buf,"OGL Filter: Nearest");
		DisplayMenu->addOptionText(buf);
	}

	filter = g_pVideoDriver->getFiltermode();
	if(filter == 0)
		DisplayMenu->addOptionText("No Filter");
	else if(filter == 1)
		DisplayMenu->addOptionText("Scale2x Filter");
	else if(filter == 2)
		DisplayMenu->addOptionText("Scale3x Filter");
	else if(filter == 3)
		DisplayMenu->addOptionText("Scale4x Filter");
	else
		DisplayMenu->addOptionText("Unknown Filter");

	if(opengl)
		DisplayMenu->addOptionText("OpenGL Acceleration");
	else
		DisplayMenu->addOptionText("Software Rendering");

	autoframeskip = g_pTimer->getFrameskip();

	if(autoframeskip)
		sprintf(buf,"Auto-Frameskip : %d fps",autoframeskip);

	DisplayMenu->addOptionText(buf);

	aspect = g_pVideoDriver->getAspectCorrection();

	if(aspect)
		DisplayMenu->addOptionText("OGL Aspect Ratio Enabled");
	else
		DisplayMenu->addOptionText("OGL Aspect Ratio Disabled");

	DisplayMenu->addSeparator();
	DisplayMenu->addOptionText("Save and return");
	DisplayMenu->addOptionText("Cancel");
	DisplayMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				DisplayMenu->setVisible(true);
			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				DisplayMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				DisplayMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = DisplayMenu->getSelection();

				if(selection == 0)
				{
					// Now the part of the resolution list
					st_resolution Resolution;
					Resolution = g_pVideoDriver->setNextResolution();

					sprintf(buf,"Resolution: %dx%dx%d", Resolution.width, Resolution.height, Resolution.depth);
					DisplayMenu->setOptionText(selection,buf);
				}
				else if(selection == 1)
				{
					if(!fsmode)
						DisplayMenu->setOptionText(1,"Fullscreen mode");
					else
						DisplayMenu->setOptionText(1,"Windowed mode");
					fsmode = !fsmode;
				}
				else if(selection == 2)
				{
					if(opengl)
					{
						gl_filter = (gl_filter==1) ? 0 : 1;

						if(gl_filter == 1)
							sprintf(buf,"OGL Filter: Linear");
						else
							sprintf(buf,"OGL Filter: Nearest");

						DisplayMenu->setOptionText(2,buf);
					}
					else
					{
						if(zoom >= 4)
							zoom = 1;
						else
							zoom++;

						if(zoom == 1)
							sprintf(buf,"No scale");
						else
							sprintf(buf,"Scale: %d", zoom);
					}

					DisplayMenu->setOptionText(2,buf);
				}

				else if(selection == 3)
				{
					if(filter >= 3)
						filter = 0;
					else
						filter++;

					if(filter == 0)
						DisplayMenu->setOptionText(3,"No Filter");
					else if(filter == 1)
						DisplayMenu->setOptionText(3,"Scale2x Filter");
					else if(filter == 2)
						DisplayMenu->setOptionText(3,"Scale3x Filter");
					else if(filter == 3)
						DisplayMenu->setOptionText(3,"Scale4x Filter");
				}
				else if(selection == 4)
				{
					opengl = !opengl; // switch the mode!!

					if(opengl)
						DisplayMenu->setOptionText(4,"OpenGL Acceleration");
					else
						DisplayMenu->setOptionText(4,"Software Rendering");
				}
				else if(selection == 5)
				{
					if(autoframeskip < 70)
						autoframeskip += 10;
					else
						autoframeskip = 10;
					sprintf(buf,"Auto-Frameskip : %d fps", autoframeskip);

					DisplayMenu->setOptionText(5, buf);
				}
				else if(selection == 6)
				{
					aspect = !aspect;

					if(aspect)
						DisplayMenu->setOptionText(6,"OGL Aspect Ratio Enabled");
					else
						DisplayMenu->setOptionText(6,"OGL Aspect Ratio Disabled");

				}
				else if(selection == 8)
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
					g_pTimer->setFrameskip(autoframeskip);
					g_pVideoDriver->setAspectCorrection(aspect);

					// initialize/activate all drivers
					g_pLogFile->ftextOut("Restarting graphics driver... (Menu)<br>");
					if (g_pVideoDriver->start())
						retval = 1;

					CSettings *Settings;
					Settings = new CSettings();

					Settings->saveDrvCfg();
					delete Settings; Settings = NULL;

					showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

					fade.mode = FADE_GO;
					fade.dir = FADE_IN;
					fade.curamt = 0;
					fade.rate = FADE_NORM;
					fade.fadetimer = 0;
					gamedo_fades();
					break;
				}
				else
					break;
			}
			// Render the Games-Menu
			DisplayMenu->renderDialog();
			g_pInput->pollEvents();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();


	} while(1);

	delete DisplayMenu;

	return retval;
}

// This function shows the Story of Commander Keen!
void showPage(const std::string& str_text, stCloneKeenPlus *pCKP, int textsize)
{
	bool cancel = false;

	CTextViewer *TextViewer = new CTextViewer(0,0,320,136);
	TextViewer->loadText(str_text);

	showmapatpos(90, STORYBOARD_X, STORYBOARD_Y, pCKP);

    fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	AllPlayersInvisible();

	char timer=0;
	do
	{
	    if(g_pTimer->TimeToRunLogic())
		{
			gamedo_fades();
			gamedo_AnimatedTiles();

			g_pInput->pollEvents();

			// Normal Keys/Axes
			if( g_pInput->getHoldedCommand(IC_DOWN) )
			{
				timer++;
				if(timer >= 5)
					TextViewer->scrollDown();
			}
			if( g_pInput->getHoldedCommand(IC_UP) )
			{
				timer++;
				if(timer >= 5)
					TextViewer->scrollUp();
			}

			// Page Keys
			if( g_pInput->getHoldedKey(KPGDN) )
			{
				timer++;
				if(timer >= 5)
					TextViewer->setNextPos();
			}
			if( g_pInput->getHoldedKey(KPGUP) )
			{
				timer++;
				if(timer >= 5)
					TextViewer->setPrevPos();
			}

			if(timer>=10) timer=0;

			cancel = g_pInput->getPressedKey(KQUIT);
		}
	     if (g_pTimer->TimeToRender())
	     {
	    	gamedo_RenderScreen();
		    TextViewer->renderBox(); // This comes after, because it does transparent overlay
	     }

	} while(!cancel);

	  delete TextViewer;

	  return;
}


char configmenu(stCloneKeenPlus *pCKP)
{
	CDialog *OptionsMenu;
	int bmnum;
	int selection;
	int x;

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	OptionsMenu = new CDialog();

	OptionsMenu->setDimensions(15,4,14,8);

	OptionsMenu->addOptionText("Graphics");
	OptionsMenu->addOptionText("Audio");
	OptionsMenu->addOptionText("Game");
	OptionsMenu->addOptionText("Controls");
	OptionsMenu->addSeparator();
	OptionsMenu->addOptionText("Back");

	OptionsMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				OptionsMenu->setVisible(true);

			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				OptionsMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				OptionsMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = OptionsMenu->getSelection();
				break;
			}
			// Render the Games-Menu
			OptionsMenu->renderDialog();
			g_pInput->pollEvents();
		}

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while(1);

	switch(selection)
	{
	case 0:
		GraphicsDlg(pCKP);
		break;

	case 1:
		AudioDlg(pCKP);
		break;

	case 2:
		OptionsDlg(pCKP);
		break;

	case 3:
		controlsmenu(pCKP);
		break;

	default:
		break;
	}

	delete OptionsMenu;

	return 0;
}

char controlsmenu(stCloneKeenPlus *pCKP)
{
	CDialog *ControlsMenu;
	int bmnum;
	int selection;
	int x;
	char buf[256];
	char buf2[256];

	showmapatpos(90, MAINMENU_X, MENUS_Y, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	ControlsMenu = new CDialog();

	ControlsMenu->setDimensions(1,2,38,21);

	g_pInput->getEventName(IC_LEFT, 0, buf2);
	sprintf(buf,"P1 Left:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_UP, 0, buf2);
	sprintf(buf,"P1 Up:     %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_RIGHT, 0, buf2);
	sprintf(buf,"P1 Right:  %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_DOWN, 0, buf2);
	sprintf(buf,"P1 Down:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_JUMP, 0, buf2);
	sprintf(buf,"P1 Jump:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_POGO, 0, buf2);
	sprintf(buf,"P1 Pogo:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_FIRE, 0, buf2);
	sprintf(buf,"P1 Fire:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_STATUS, 0, buf2);
	sprintf(buf,"P1 Status: %s",buf2);
	ControlsMenu->addOptionText(buf);

	g_pInput->getEventName(IC_LEFT, 1, buf2);
	sprintf(buf,"P2 Left:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_UP, 1, buf2);
	sprintf(buf,"P2 Up:     %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_RIGHT, 1, buf2);
	sprintf(buf,"P2 Right:  %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_DOWN, 1, buf2);
	sprintf(buf,"P2 Down:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_JUMP, 1, buf2);
	sprintf(buf,"P2 Jump:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_POGO, 1, buf2);
	sprintf(buf,"P2 Pogo:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_FIRE, 1, buf2);
	sprintf(buf,"P2 Fire:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_STATUS, 1, buf2);
	sprintf(buf,"P2 Status: %s",buf2);
	ControlsMenu->addOptionText(buf);
	ControlsMenu->addSeparator();
	ControlsMenu->addOptionText("Reset Controls");
	ControlsMenu->addOptionText("Return");

	ControlsMenu->animateDialogBox(true);

	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// do fades
			gamedo_fades();
			if(fade.mode == FADE_COMPLETE)
				ControlsMenu->setVisible(true);
			gamedo_AnimatedTiles();

			// Check the Input
			if(g_pInput->getPulsedCommand(IC_DOWN, 80))
				ControlsMenu->setNextSelection();
			if(g_pInput->getPulsedCommand(IC_UP, 80))
				ControlsMenu->setPrevSelection();

			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				selection = ControlsMenu->getSelection();

				if(selection < MAX_COMMANDS)
				{
					int item=0;
					if(selection < 4)
						item = selection + 4;
					else
						item = selection - 4;

					switch(selection)
					{
					 case 0: sprintf(buf,"P1 Left:   "); break;
					 case 1: sprintf(buf,"P1 Up:     "); break;
					 case 2: sprintf(buf,"P1 Right:  "); break;
					 case 3: sprintf(buf,"P1 Down:   "); break;
					 case 4: sprintf(buf,"P1 Jump:   "); break;
					 case 5: sprintf(buf,"P1 Pogo:   "); break;
					 case 6: sprintf(buf,"P1 Fire:   "); break;
					 case 7: sprintf(buf,"P1 Status: "); break;
					}

					strcpy(buf2,buf);
					strcat(buf2,"*Waiting for Input*");
					ControlsMenu->setOptionText(selection,buf2);

					while(!g_pInput->readNewEvent(0,item))
					{
						if(g_pTimer->TimeToRunLogic())
							ControlsMenu->renderDialog();
						gamedo_frameskipping_blitonly();
					}

					g_pInput->getEventName(item, 0, buf2);
					strcat(buf,buf2);
					ControlsMenu->setOptionText(selection,buf);
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
					 case 0+ MAX_COMMANDS: sprintf(buf,"P2 Left:   "); break;
					 case 1+ MAX_COMMANDS: sprintf(buf,"P2 Up:     "); break;
					 case 2+ MAX_COMMANDS: sprintf(buf,"P2 Right:  "); break;
					 case 3+ MAX_COMMANDS: sprintf(buf,"P2 Down:   "); break;
					 case 4+ MAX_COMMANDS: sprintf(buf,"P2 Jump:   "); break;
					 case 5+ MAX_COMMANDS: sprintf(buf,"P2 Pogo:   "); break;
					 case 6+ MAX_COMMANDS: sprintf(buf,"P2 Fire:   "); break;
					 case 7+ MAX_COMMANDS: sprintf(buf,"P2 Status: "); break;
					}

					strcpy(buf2,buf);
					strcat(buf2,"*Waiting for Input*");
					ControlsMenu->setOptionText(selection,buf2);

					while(!g_pInput->readNewEvent(1,item))
					{
						if(g_pTimer->TimeToRunLogic())
							ControlsMenu->renderDialog();
						gamedo_frameskipping_blitonly();
					}

					g_pInput->getEventName(item, 1, buf2);
					strcat(buf,buf2);
					ControlsMenu->setOptionText(selection,buf);
				}
				else if(selection == MAX_COMMANDS*2+1)
				{
					// Reset Controls here!
					g_pInput->resetControls();
					g_pInput->saveControlconfig();
					break;
				}
				else
				{
					g_pInput->saveControlconfig();
					break;
				}
			}
			// Render the Menu
			ControlsMenu->renderDialog();
			g_pInput->pollEvents();
		}
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(1);

	delete ControlsMenu;
	return 0;
}

void keensleft(int episode)
{
int enter, lastenterstate;
unsigned int p;
int x,y,i;
int boxY, boxH;
int boxtimer;

  #define KEENSLEFT_TIME        400

  for(i=0;i<MAX_PLAYERS;i++)
  {
    if (player[i].isPlaying)
    {
      gamepdo_wm_SelectFrame(i, episode);
      player[i].hideplayer = 0;
    }
  }
  gamedo_RenderScreen();

  #define KEENSLEFT_X        7
  #define KEENSLEFT_Y        11
  #define KEENSLEFT_W        24
  #define KEENSLEFT_H        4

  boxY = KEENSLEFT_Y - (numplayers);
  boxH = KEENSLEFT_H + (numplayers * 2);

  dialogbox(KEENSLEFT_X,boxY,KEENSLEFT_W,boxH);
  g_pGraphics->drawFont( getstring("LIVES_LEFT_BACKGROUND"),(KEENSLEFT_X+1)*8,(boxY+1)*8,0);
  g_pGraphics->drawFont( getstring("LIVES_LEFT"),((KEENSLEFT_X+7)*8)+4,(boxY+1)*8,0);
  y = ((boxY+2)*8)+4;
  if (numplayers>1) y--;
  for(p=0;p<numplayers;p++)
  {
    x = ((KEENSLEFT_X+1)*8)+4;
    for(i=0;i<player[p].inventory.lives&&i<=10;i++)
    {
    	g_pGraphics->drawSprite_direct(x, y, PMAPDOWNFRAME+playerbaseframes[p]-
    			(episode==3));
    	// (episode==3) TODO: Check whether this is necessary
      x+=16;
    }
    y+=18;
  }
  g_pVideoDriver->update_screen();

  g_pSound->playSound(SOUND_KEENSLEFT, PLAY_NOW);

  boxtimer = 0;
  do
  {
	if(g_pTimer->TimeToRunLogic())
	{
		gamedo_fades();

		if (boxtimer > KEENSLEFT_TIME)
		{
		  break;
		} else boxtimer++;

		enter = g_pInput->getPressedCommand(IC_STATUS)||g_pInput->getPressedCommand(IC_FIRE)||
		g_pInput->getPressedCommand(IC_JUMP)||g_pInput->getPressedCommand(IC_POGO);
		if (enter)
		{
		  break;
		}
		if (g_pInput->getPressedCommand(KQUIT))
		{
		  return;
		}

		lastenterstate = enter;
		g_pInput->pollEvents();
	}
  } while(!crashflag);

}
