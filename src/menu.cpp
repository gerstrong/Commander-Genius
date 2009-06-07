/* MENU.C
  The main menu, intro, and other such stuff.
*/

#include "keen.h"
#include "pressf10.h"
#include "include/menu.h"
#include "include/misc.h"
#include "sdl/CVideoDriver.h"
#include "include/game.h"
#include "include/gamedo.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "include/eseq_ep2.h"
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
#include <SDL.h>
#include <iostream>
#include <fstream>
using namespace std;

#define SELMOVE_SPD         3

short openDlgStruct(stDlgStruct *pDlgStruct, stCloneKeenPlus *pCKP);

void showmapatpos(int level, int xoff, int yoff, int wm, stCloneKeenPlus *pCKP)
{
int i;
char levelname[MAX_STRING_LENGTH];
g_pLogFile->ftextOut("showmapatpos(%d, %d, %d, %d);<br>",level,xoff,yoff,wm);
  pCKP->Control.levelcontrol.dark = 0;
  g_pGraphics->initPalette(pCKP->Control.levelcontrol.dark);

  initgame(pCKP);           // reset scroll
  memset(levelname,0,MAX_STRING_LENGTH*sizeof(char));
  sprintf(levelname, "level%02d.ck%d", level, pCKP->Control.levelcontrol.episode);


  short numsel;
  if(pCKP->Resources.GameSelected == 0 ) // First time startup. No game has been chosen
	  numsel = 0;
  else
	  numsel = pCKP->Resources.GameSelected-1;

  loadmap(levelname, pCKP->GameData[numsel].DataDirectory, level, wm, pCKP);

  drawmap();
  for(i=0;i<xoff;i++) map_scroll_right();
  for(i=0;i<yoff;i++) map_scroll_down();

  g_pVideoDriver->sb_blit();
}

short loadResourcesforStartMenu(stCloneKeenPlus *pCKP, CGame *Game)
{
	string line;

	ifstream gamescfg("data/games.cfg");

    if (gamescfg.is_open())
	{
	   while ( !gamescfg.eof() && pCKP->numGames < 20 )
	   {
	     getline (gamescfg,line);

		 if(strncmp(line.data(),"[",strlen("[")) == 0)
		 {
			 stGameData *NewGameData;

			 pCKP->numGames++;
			 NewGameData = new stGameData[pCKP->numGames];
			 memset(NewGameData,0,pCKP->numGames*sizeof(stGameData));
			 memcpy(NewGameData,pCKP->GameData,(pCKP->numGames-1)*sizeof(stGameData));

			 delete[] pCKP->GameData;

			 pCKP->GameData = NewGameData;
		 }
		 if(strncmp(line.data(),"Name=",strlen("Name=")) == 0)
		 {
			 line.copy(pCKP->GameData[pCKP->numGames-1].Name,line.length()-strlen("Name="),strlen("Name="));
		 }
		 if(strncmp(line.data(),"Episode=",strlen("Episode=")) == 0)
		 {
		 	sscanf(line.data(),"Episode=%hd", &(pCKP->GameData[pCKP->numGames-1].Episode));
		 }
		 if(strncmp(line.data(),"Path=",strlen("Path=")) == 0)
		 {
			 unsigned short l = strlen("Path=");
			 line.copy(pCKP->GameData[pCKP->numGames-1].DataDirectory,line.length()-l,l);
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
	if (Game->getLatch()->loadGraphics(pCKP->Control.levelcontrol.episode, pCKP->GameData[0].DataDirectory)) return abortCKP(pCKP);

	player[0].x = player[0].y = 0;
	if(initgamefirsttime(pCKP, 0) != 0)
	{
		return 1;
	}
	initgame(pCKP);

	return 0;
}


#define MAINMENU_GOTO_DEMO_TIME      4000

extern char fade_black;
short loadStartMenu(stCloneKeenPlus *pCKP)
{
	CDialog *GamesMenu;
	int i;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;
	showmapatpos(90, (104 << 2)+256+256+80, 32-4, 0, pCKP);

	// Prepare the Games Menu
	GamesMenu = new CDialog();

	GamesMenu->setDimensions(2,2,36,5);

	// Show me the games you detected!
	for( i=0 ; i < pCKP->numGames ; i++ )
	{
		GamesMenu->addOptionText(pCKP->GameData[i].Name);
	}

	GamesMenu->animateDialogBox(true);

	do
	{
		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			GamesMenu->setVisible(true);

		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			GamesMenu->setNextSelection();

		if(g_pInput->getPressedCommand(IC_UP))
			GamesMenu->setPrevSelection();

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			fade.dir = FADE_OUT;
			fade.curamt = PAL_FADE_SHADES;
			fade.fadetimer = 0;
			fade.rate = FADE_NORM;
			fade.mode = FADE_GO;
			pCKP->Resources.GameSelected = GamesMenu->getSelection()+1;
			pCKP->Control.levelcontrol.episode = pCKP->GameData[pCKP->Resources.GameSelected-1].Episode;
			break;
		}

		// Render the Games-Menu
		GamesMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);
	} while(!g_pInput->getExitEvent());

	delete GamesMenu;

	return 0;
}

int mainmenu(stCloneKeenPlus *pCKP,int defaultopt)
{

	CDialog *MainMenu;
	int bmnum;
	int x;
	int selection;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;
	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

	// Prepare the Games Menu
	MainMenu = new CDialog();

	MainMenu->setDimensions(11,9,18,11);

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
	MainMenu->addOptionText("Quit");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	MainMenu->animateDialogBox(true);

	do
	{

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			MainMenu->setVisible(true);

		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			MainMenu->setNextSelection();

		if(g_pInput->getPressedCommand(IC_UP))
			MainMenu->setPrevSelection();

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = MainMenu->getSelection();
			break;
		}


		// Render the Games-Menu
		MainMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		if(g_pInput->getExitEvent())
			return MAINMNU_QUIT;

	} while(1);

    if (selection==MAINMNU_LOADGAME)
    {
    	int diff;
    	diff = getDifficulty(pCKP);
    	if(diff>2)
    	{
    		return BACK2MAINMENU;
    	}

    	options[OPT_MEAN].value = diff;

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

        	options[OPT_MEAN].value = diff;
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

void initialiazeDlgStruct(stDlgStruct *pDlgStruct)
{
	pDlgStruct->OptionSwitch = (stOptionSwitch*) malloc(pDlgStruct->num_OptionSwitches*sizeof(stOptionSwitch));
	pDlgStruct->Separator = (stSeparator*) malloc(pDlgStruct->num_Separators*sizeof(stSeparator));
	pDlgStruct->StarterSwitch = (stStarterSwitch*) malloc(pDlgStruct->num_StarterSwitch*sizeof(stStarterSwitch));
	pDlgStruct->TextLine = (stTextLine*) malloc(pDlgStruct->num_TextLines*sizeof(stTextLine));
}

int getDifficulty(stCloneKeenPlus *pCKP)
{
	CDialog *DifficultyMenu;
	int bmnum;
	int selection;
	int x;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	DifficultyMenu = new CDialog();

	DifficultyMenu->setDimensions(15,4,14,6);

	DifficultyMenu->addOptionText("Normal");
	DifficultyMenu->addOptionText("Hard");
	DifficultyMenu->addSeparator();
	DifficultyMenu->addOptionText("Cancel");

	DifficultyMenu->animateDialogBox(true);

	do
	{
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			DifficultyMenu->setVisible(true);

		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			DifficultyMenu->setNextSelection();

		if(g_pInput->getPressedCommand(IC_UP))
			DifficultyMenu->setPrevSelection();

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = DifficultyMenu->getSelection();
			break;
		}
		// Render the Games-Menu
		DifficultyMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

	} while(1);

	delete DifficultyMenu;

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

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

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
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			AudioMenu->setVisible(true);
		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			AudioMenu->setNextSelection();

		if(g_pInput->getPressedCommand(IC_UP))
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
				saveDriverConfiguration(pCKP);
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

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

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

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
	x = (320/2)-(bitmaps[bmnum].xsize/2);
	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	OptionsMenu = new CDialog();
	OptionsMenu->setDimensions(4,4,32,11);

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
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			OptionsMenu->setVisible(true);
		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			OptionsMenu->setNextSelection();
		if(g_pInput->getPressedCommand(IC_UP))
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
			    SaveConfig(pCKP->Option);
				break;
			}
		}
		// Render the Games-Menu
		OptionsMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

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
	unsigned short frameskip = 0;
	bool opengl = false;
	unsigned char gl_filter = 0;
	bool fsmode;
	char buf[256];
	short retval = 0;
	unsigned char autoframeskip = 0;

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");
	x = (320/2)-(bitmaps[bmnum].xsize/2);
	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	DisplayMenu = new CDialog();
	DisplayMenu->setDimensions(4,4,32,11);

	width  = g_pVideoDriver->getWidth();
	height = g_pVideoDriver->getHeight();
	depth  = g_pVideoDriver->getDepth();
	sprintf(buf,"Resolution: %dx%dx%d",width,height,depth);

	zoom   = g_pVideoDriver->getZoomValue();
	filter = g_pVideoDriver->getFiltermode();
	frameskip = g_pVideoDriver->getFrameskip();

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

	opengl = g_pVideoDriver->isOpenGL();
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

	sprintf(buf,"Frameskip: %d", frameskip);
	DisplayMenu->addOptionText(buf);

	if(opengl)
		DisplayMenu->addOptionText("OpenGL Acceleration");
	else
		DisplayMenu->addOptionText("Software Rendering");

	autoframeskip = g_pVideoDriver->getTargetFPS();
	switch(autoframeskip)
	{
	case 60:
		DisplayMenu->addOptionText("Auto-Frameskip : 60 fps"); break;
	case 50:
		DisplayMenu->addOptionText("Auto-Frameskip : 50 fps"); break;
	case 40:
		DisplayMenu->addOptionText("Auto-Frameskip : 40 fps"); break;
	case 30:
		DisplayMenu->addOptionText("Auto-Frameskip : 30 fps"); break;
	case 20:
		DisplayMenu->addOptionText("Auto-Frameskip : 20 fps"); break;
	default:
		DisplayMenu->addOptionText("Auto-Frameskip disabled"); break;
	};


	DisplayMenu->addSeparator();
	DisplayMenu->addOptionText("Save and return");
	DisplayMenu->addOptionText("Cancel");

	DisplayMenu->animateDialogBox(true);

	do
	{
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			DisplayMenu->setVisible(true);
		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			DisplayMenu->setNextSelection();
		if(g_pInput->getPressedCommand(IC_UP))
			DisplayMenu->setPrevSelection();

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = DisplayMenu->getSelection();

			if(selection == 0)
			{
				// Now the part of the resolution list
				if(width == 1024)
				{
					width  = 320;
					height = 240;
				}
				else if(width == 320)
				{
					width  = 640;
					height = 480;
				}
				else if(width == 640)
				{
					width  = 800;
					height = 600;
				}
				else if(width == 800)
				{
					width  = 1024;
					height = 768;
				}

				// TODO: Replace this by the available modes list!

				sprintf(buf,"Resolution: %dx%dx%d",width,height,depth);
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
					if(zoom >= 3)
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
					DisplayMenu->setOptionText(3,"Scale4x Filter (OpenGL)");
			}
			else if(selection == 4)
			{
				frameskip++;

				if(frameskip > 20)
					frameskip = 0;

				sprintf(buf,"Frameskip: %d",frameskip);
				DisplayMenu->setOptionText(4,buf);
			}
			else if(selection == 5)
			{
				opengl = opengl ? false : true; // switch the mode!!

				if(opengl)
					DisplayMenu->setOptionText(5,"OpenGL Acceleration");
				else
					DisplayMenu->setOptionText(5,"Software Rendering");
			}
			else if(selection == 6)
			{
				if(autoframeskip < 60 && autoframeskip >= 0)
					autoframeskip += 10;
				else
					autoframeskip = 0;

				switch(autoframeskip)
				{
				case 60:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 60 fps"); break;
				case 50:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 50 fps"); break;
				case 40:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 40 fps"); break;
				case 30:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 30 fps"); break;
				case 20:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 20 fps"); break;
				case 10:
					DisplayMenu->setOptionText(6,"Auto-Frameskip : 10 fps"); break;
				default:
					DisplayMenu->setOptionText(6,"Auto-Frameskip disabled"); break;
				};
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
				g_pVideoDriver->setMode(width,height,depth);
				g_pVideoDriver->setZoom(zoom);
				g_pVideoDriver->setFilter(filter);
				g_pVideoDriver->setFrameskip(frameskip);
				g_pVideoDriver->setTargetFPS(autoframeskip);
				saveDriverConfiguration(pCKP);
				g_pGraphics->allocScrollBufmem();

				// initialize/activate all drivers
				g_pLogFile->ftextOut("Starting graphics driver...<br>");
				if (g_pVideoDriver->start())
					retval = 1;

				showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

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

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

	} while(1);

	delete DisplayMenu;

	return retval;
}


// This function shows the Story of Commander Keen!
void showPage(char *text, stCloneKeenPlus *pCKP, int textsize)
{
	unsigned int i, j, k;
	int exit=0;
	int textpos;
	//char lastupstate, lastdnstate;
	int enter;
	unsigned int dlgX,dlgY,dlgW,dlgH;
	//unsigned char lastkeytable[KEYTABLE_SIZE+1];
	unsigned int scroll, maxscroll;
	char buffer[200][40];

	showmapatpos(90, STORYBOARD_X, STORYBOARD_Y, 0, pCKP);

    fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	scroll=0;
	maxscroll=0;

	j=0;
	k=0;

	AllPlayersInvisible();

	  dlgX = 0;
	  dlgY = 0;
	  dlgW = 39;
	  dlgH = 15;

	  textpos=0;
	  memset(buffer,0,200*40*sizeof(char));
	  // Prepare the buffer

	  char sbuf[256];
	  unsigned int totnumline=0;

	  for(i=0;i<200;i++)
	  {
	  	for(j=0;j<dlgW-1;j++)
	   	{
	   		sscanf(text+textpos,"%s",sbuf);

	   		if(((strlen(sbuf) + j) > dlgW-2))
	   		{
	   			if(text[textpos] == ' ')
	   			{
	   				textpos++;
	   				break;
	   			}
	   		}

	   		if(text[textpos]=='\n' )
	   		{
	   			textpos++;
				break;
	   		}

	   		if(text[textpos]==31 ) 	// I don't know, what they do,
									//but original version seems to ignore them!
	   		{
	   			text[textpos]=' ';
	   		}


    		buffer[i][j]=text[textpos];
    		textpos++;
    		if(textpos >= textsize)
    			break;
    	}
		if(textpos >= textsize)
		{
			totnumline+=3;
			break;
		}

		totnumline++;
	  }
	  buffer[i][j] = ' '; // Last character is empty!

	  char coverline[39];
	  memset(coverline,2,38*sizeof(char)); // for the upper and lower edges
	  coverline[38]=0;

	  do
	  {
	    gamedo_fades();

	    gamedo_AnimatedTiles();

	    sb_dialogbox(dlgX, dlgY, dlgW, dlgH);

	    k=0;

	    // Draw the text
	    for(i=0;i<dlgH-1;i++)
	    {
	    	if(buffer[i+(scroll>>3)][0]=='~') // Special Background Colour
	    	{
		    	char temp[39];
		    	memset(temp,' ',38*sizeof(char));
		    	temp[38]=0;
		    	g_pGraphics->sb_color_font_draw((unsigned char*) temp, (dlgX+1)<<3, (((dlgY+i+1)<<3) -(scroll%8)),COLOUR_DARKRED,COLOUR_GREY);
		    	g_pGraphics->sb_color_font_draw((unsigned char*) buffer[i+(scroll>>3)]+1, (dlgX+1)<<3, (((dlgY+i+1)<<3) -(scroll%8)),COLOUR_DARKRED,COLOUR_GREY);
	    	}
	    	else
	    	{
	    		g_pGraphics->sb_font_draw((unsigned char*) buffer[i+(scroll>>3)], (dlgX+1)<<3, (((dlgY+i+1)<<3) -(scroll%8)));
	    	}
	    }
	    g_pGraphics->sb_font_draw((unsigned char*) coverline, (dlgX+1)<<3, dlgY); // Upper and lower edge Update
	    g_pGraphics->sb_font_draw((unsigned char*) coverline, (dlgX+1)<<3, (dlgY+dlgH-1)<<3);

	    // If user presses up or down
	    if (g_pInput->getHoldedCommand(IC_DOWN))
	    {
			if(scroll < (totnumline-dlgH)<<3)
				scroll++;
			SDL_Delay(2);
	    }
		else if (g_pInput->getHoldedCommand(IC_UP))
		{
			if(scroll > 0)
				scroll--;
			SDL_Delay(2);
		}

	    enter = (g_pInput->getPressedCommand(IC_STATUS));//||g_pInput->getPressedCommand(KCTRL)||g_pInput->getPressedCommand(KALT));
	    if (enter)
	    {
	    	exit=1;
	        fade.dir = FADE_OUT;
	        fade.curamt = PAL_FADE_SHADES;
	        fade.fadetimer = 0;
	        fade.rate = FADE_NORM;
	        fade.mode = FADE_GO;
	    }

	    gamedo_frameskipping(pCKP);

	    g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();


	    if(exit==1 && fade.mode==FADE_COMPLETE)
	    	break;

	    if (g_pInput->getPressedCommand(KQUIT)) break;
	  } while(!crashflag);
	  return;
}

char configmenu(stCloneKeenPlus *pCKP)
{
	CDialog *OptionsMenu;
	int bmnum;
	int selection;
	int x;

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

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
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			OptionsMenu->setVisible(true);

		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			OptionsMenu->setNextSelection();

		if(g_pInput->getPressedCommand(IC_UP))
			OptionsMenu->setPrevSelection();

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			selection = OptionsMenu->getSelection();
			break;
		}
		// Render the Games-Menu
		OptionsMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

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

	showmapatpos(90, MAINMENU_X, MENUS_Y, 0, pCKP);

	// Load the Title Bitmap
	bmnum = g_pGraphics->getBitmapNumberFromName("TITLE");

	x = (320/2)-(bitmaps[bmnum].xsize/2);

	g_pGraphics->drawBitmap(x, 0, bmnum);

	// Prepare the Games Menu
	ControlsMenu = new CDialog();

	ControlsMenu->setDimensions(1,1,38,24);

	g_pInput->getEventName(IC_LEFT,buf2);
	sprintf(buf,"Left:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_UP,buf2);
	sprintf(buf,"Up:     %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_RIGHT,buf2);
	sprintf(buf,"Right:  %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_DOWN,buf2);
	sprintf(buf,"Down:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_JUMP,buf2);
	sprintf(buf,"Jump:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_POGO,buf2);
	sprintf(buf,"Pogo:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_FIRE,buf2);
	sprintf(buf,"Fire:   %s",buf2);
	ControlsMenu->addOptionText(buf);
	g_pInput->getEventName(IC_STATUS,buf2);
	sprintf(buf,"Status: %s",buf2);
	ControlsMenu->addOptionText(buf);
	ControlsMenu->addSeparator();
	ControlsMenu->addOptionText("Return");

	ControlsMenu->animateDialogBox(true);

	do
	{
		// do fades
		gamedo_fades();
		if(fade.mode == FADE_COMPLETE)
			ControlsMenu->setVisible(true);
		gamedo_AnimatedTiles();

		// Check the Input
		if(g_pInput->getPressedCommand(IC_DOWN))
			ControlsMenu->setNextSelection();
		if(g_pInput->getPressedCommand(IC_UP))
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
				 case 0: sprintf(buf,"Left:   "); break;
				 case 1: sprintf(buf,"Up:     "); break;
				 case 2: sprintf(buf,"Right:  "); break;
				 case 3: sprintf(buf,"Down:   "); break;
				 case 4: sprintf(buf,"Jump:   "); break;
				 case 5: sprintf(buf,"Pogo:   "); break;
				 case 6: sprintf(buf,"Fire:   "); break;
				 case 7: sprintf(buf,"Status: "); break;
				}

				strcpy(buf2,buf);
				strcat(buf2,"*Waiting for Input*");
				ControlsMenu->setOptionText(selection,buf2);

				while(!g_pInput->readNewEvent(item))
				{
					ControlsMenu->renderDialog();
					gamedo_frameskipping_blitonly(pCKP);
				}

				g_pInput->getEventName(item,buf2);
				strcat(buf,buf2);
				ControlsMenu->setOptionText(selection,buf);
			}
			else
			{
				g_pInput->saveControlconfig();
				break;
			}
		}
		// Render the Menu
		ControlsMenu->renderDialog();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly(pCKP);

		g_pInput->pollEvents();
	    g_pTimer->SpeedThrottle();

	} while(1);

	delete ControlsMenu;
	return 0;
}

int intro(stCloneKeenPlus *pCKP)
{
int x,y;
unsigned int i;
int yb;

int curPage, changedPage;
int textVisible;
char stStringName[40];
char *stStringData;
char stTextLine1[80];
char stTextLine2[80];
char stTextLine3[80];
char SlowPrintBuffer[80];
int x1,x2,x3,x4,x5;
int y1,y2,y3,y4,y5;
char *copyPtr, copyIndex;

int ontime, offtime, textTimer;
int y1adj, y2adj, y3adj;
int lastpage;
int SlowPrint, SlowPrintTimer, SlowPrintIndex;
int SlowPrintSpeed;
char keypressed;
char lastkeypressed;
char *SlowPrintPtr;

const char *ScrollText1      = "Presenting";
const char *ScrollTextErase  = "               ";
const char *ScrollText2      = "CloneKeenPlus";
const char *ScrollText4      = "by Caitlin Shaw";
const char *ScrollText5      = "and Gerstrong";
char ScrollText3[20];
char ScrollTextTimer;
int STimer;
char SState;
char ShowPressF10;
#define PRESSF10_X      (160 - (PRESSF10_WIDTH/2))
#define PRESSF10_Y      3
//#define PRESSF10_X      (315 - PRESSF10_WIDTH)
//#define PRESSF10_Y      (195 - PRESSF10_HEIGHT)

  if (!g_pVideoDriver->getFullscreen())
    ShowPressF10 = 0;           // 0 = mask hide all
  else
    ShowPressF10 = 15;          // 15 = mask show all

  // display the starry background and initiate a fade-in
  showmapatpos(90, 104<<4, 32, 0, pCKP);
  fade.mode = FADE_GO;
  fade.rate = FADE_NORM;
  fade.dir = FADE_IN;
  fade.curamt = 0;
  fade.fadetimer = 0;

  player[0].hideplayer = 1;

  #define SCROLL_STOP_Y          75
  #define F10_STOP_Y             210

  /* move "Presenting" up the screen */
  g_pGraphics->setFadeBlack(1);
  sprintf(ScrollText3, "Episode %d", pCKP->Control.levelcontrol.episode);
  x1 = (320/2) - ((strlen(ScrollText1)*8)/2);
  y1 = 235;
  x2 = (320/2) - ((strlen(ScrollText2)*8)/2);
  y2 = SCROLL_STOP_Y+16;
  x3 = (320/2) - ((strlen(ScrollText3)*8)/2);
  y3 = y2+24;
  x4 = (320/2) - ((strlen(ScrollText4)*8)/2);
  y4 = y3+16;
  x5 = (320/2) - ((strlen(ScrollText5)*8)/2);
  y5 = y4+16;
  SState = 0;
  STimer = 0;
  ScrollTextTimer = 0;

  lastkeypressed = 1;
  do
  {
    // do fades
    gamedo_fades();
    gamedo_AnimatedTiles();

    if (fade.dir != FADE_OUT)
    {
       // if user pressed a key cancel the intro
       keypressed = 0;
       for(i=0;i<KEYTABLE_SIZE;i++)
       {
          if (g_pInput->getPressedKey(i))
          {
             keypressed = i;
             break;
          }
       }
       if (keypressed && !lastkeypressed)
       {
             fade.dir = FADE_OUT;
             fade.curamt = PAL_FADE_SHADES;
             fade.fadetimer = 0;
             fade.rate = FADE_NORM;
             fade.mode = FADE_GO;
             IntroCanceled = 1;
       }
       if (keypressed) lastkeypressed = 1; else lastkeypressed = 0;

    }  // end if(fade.dir!=FADE_OUT)

    g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollText1, x1, y1);

    if (SState >= 1)
    {
    	g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollText2, x2, y2);
    	if (SState==1)
    	{
    		if (STimer > 200)
    		{
    			SState = 2;
    			STimer = 0;
    		}
    		else STimer++;
    	}
    }

    if (SState >= 2)
    {
    	g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollText3, x3, y3);
    	if (SState==2)
    	{
    		if (STimer > 200)
    		{
    			SState = 3;
    			STimer = 0;
    		}
    		else STimer++;
    	}
    }

    if (SState >= 3)
    {
    	g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollText4, x4, y4);
      if (SState==3)
      {
         if (STimer > 200)
         {
            SState = 4;
            STimer = 0;
         }
         else STimer++;
      }
    }

    if (SState >= 4)
    {
    	g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollText5, x5, y5);
      if (STimer > 1000)
      {
    	  g_pGraphics->setFadeBlack(0);
        break;
      }
      else STimer++;
    }

    // if Press F10 message is still visible, display it
    if (ShowPressF10)
    {
      if (y1 <= F10_STOP_Y) ShowPressF10 = 0;
      if (g_pVideoDriver->getFullscreen()) ShowPressF10 = 0;

      i = 0;
      for(y=0;y<PRESSF10_HEIGHT;y++)
      {
        yb = ((PRESSF10_Y+y+scrolly_buf)&511)<<9;
        for(x=0;x<PRESSF10_WIDTH;x++)
        {
          g_pGraphics->getScrollbuffer()[yb+((PRESSF10_X+x+scrollx_buf)&511)] = (pressf10_image[i++] & ShowPressF10);
        }
      }
    }

    // blit the scrollbuffer to the display
    gamedo_frameskipping_blitonly(pCKP);

    if (SState==0)
    {
    	g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x1, y1);

       if (y1 > SCROLL_STOP_Y)
       {
          if (ScrollTextTimer > 10)
          {
            y1--;
            ScrollTextTimer = 0;
          }
          else ScrollTextTimer++;
       }
       else
       {
          if (STimer > 200)
          {
            SState = 1;
            STimer = 0;
          }
          else STimer++;
       }
    }

    // when fade is complete we're done!
    if (fade.dir==FADE_OUT && fade.mode==FADE_COMPLETE)
    {
    	g_pGraphics->setFadeBlack(0);
      return 0;
    }

    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
    /*if (immediate_keytable[KQUIT] || crashflag)
    {
       fade_black = 0;
       return 1;
    }*/
  } while(!g_pInput->getExitEvent());

  // TODO: This invokes Segfault. Please check, where!
  if(g_pInput->getExitEvent())
	  return 1;

  g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x1, y1);
  g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x2, y2);
  g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x3, y3);
  g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x4, y4);
  g_pGraphics->sb_font_draw_inverse( (unsigned char*) ScrollTextErase, x5, y5);

  /* the appearing/disapearing text */
  curPage = 1;
  changedPage = 1;
  do
  {
    // do fades
    gamedo_fades();
    gamedo_AnimatedTiles();

       // need to load a new page from the strings file?
       if (changedPage)
       {
          // load the data for the new page
          sprintf(stStringName, "Tribute_Page_%d", curPage);
          stStringData = getstring(stStringName);
          ontime = GetStringAttribute(stStringName, "ONTIME");
          offtime = GetStringAttribute(stStringName, "OFFTIME");
          y1adj = GetStringAttribute(stStringName, "Y1ADJ");
          y2adj = GetStringAttribute(stStringName, "Y2ADJ");
          y3adj = GetStringAttribute(stStringName, "Y3ADJ");
          lastpage = GetStringAttribute(stStringName, "LASTPAGE");
          SlowPrint = GetStringAttribute(stStringName, "SLOWPRINT");

          // if no SLOWPRINT attribute disable slowprinting
          if (SlowPrint==-1)
          {
            SlowPrint = 0;
          }
          else
          {
            SlowPrintSpeed = GetStringAttribute(stStringName, "SLOWPRINTSPD");
          }

          // for y adjustments that weren't specified use 0
          if (y1adj==-1) y1adj = 0;
          if (y2adj==-1) y2adj = 0;
          if (y3adj==-1) y3adj = 0;

          stTextLine1[0] = stTextLine2[0] = stTextLine3[0] = 0;

          // we have the text as three CR-terminated lines, now split it
          // up into the 3 buffers
          copyIndex = 0;
          copyPtr = stTextLine1;
          for(i=0;i<strlen(stStringData);i++)
          {
            if (stStringData[i] != 13)
            {
              *copyPtr = stStringData[i];
              copyPtr++;
            }
            else
            {  // hit a CR
              // null-terminate
              *copyPtr = 0;
              // start copying to next buffer
              if (copyIndex==0) copyPtr = stTextLine2;
              else if (copyIndex==1) copyPtr = stTextLine3;
              else if (copyIndex==2) break;
              copyIndex++;
            }
          }
          *copyPtr = 0;    // null-terminate

          // figure out what X position these lines to be drawn in order
          // to be centered.
          x1 = (320/2)-((strlen(stTextLine1)*8)/2);
          x2 = (320/2)-((strlen(stTextLine2)*8)/2);
          x3 = (320/2)-((strlen(stTextLine3)*8)/2);
          // figure out their Y positions
          y1 = 90 + y1adj;
          y2 = 98 + y2adj;
          y3 = 106 + y3adj;

          // if we're going to do slowprinting then copy the line we're going
          // to slowprint into the slowprint buffer and fill it with spaces
          if (SlowPrint)
          {
            // set up a pointer to the line we're going to slowprint
            if (SlowPrint==1) SlowPrintPtr = &stTextLine1[0];
            else if (SlowPrint==2) SlowPrintPtr = &stTextLine2[0];
            else SlowPrintPtr = &stTextLine3[0];

            // copy the text line into the slow print buffer.
            // replace '@''s with the episode number
            for(i=0;i<strlen(SlowPrintBuffer)+1;i++)
            {
              if (SlowPrintPtr[i]=='@')
              {
                SlowPrintBuffer[i] = pCKP->Control.levelcontrol.episode + '0';
              }
              else
              {
                SlowPrintBuffer[i] = SlowPrintPtr[i];
              }
            }

            // clear out the text line
            for(i=0;i<strlen(SlowPrintPtr);i++) SlowPrintPtr[i] = ' ';
          }

          // set up some variables
          textVisible = 1;
          textTimer = ontime;
          changedPage = 0;
          SlowPrintTimer = 0;
          SlowPrintIndex = 0;
       }

       // handle slowprinting
       if (SlowPrint)
       {
          if (SlowPrintTimer > SlowPrintSpeed)
          {  // time to print the next character of the line we're slowprinting
             if (SlowPrintBuffer[SlowPrintIndex])
             {
               SlowPrintPtr[SlowPrintIndex] = SlowPrintBuffer[SlowPrintIndex];
               SlowPrintIndex++;
             }
             else
             { // reached the NULL (slowprint complete)
               SlowPrint = 0;
             }

             SlowPrintTimer = 0;
          }
          else SlowPrintTimer++;
       }

    if (fade.dir != FADE_OUT)
    {
       if (!textTimer)
       {  // time to either invisibilize the text, or go to the next page
         if (textVisible)
         {
           // erase the text
           for(i=0;i<strlen(stTextLine1);i++) stTextLine1[i] = ' ';
           for(i=0;i<strlen(stTextLine2);i++) stTextLine2[i] = ' ';
           for(i=0;i<strlen(stTextLine3);i++) stTextLine3[i] = ' ';
           textVisible = 0;
           textTimer = offtime;
         }
         else
         {
           // time for more text
           if (lastpage != 1)
           {
             curPage++;
             changedPage = 1;
           }
           else
           {  // reached last page...initiate fadeout
             fade.dir = FADE_OUT;
             fade.curamt = PAL_FADE_SHADES;
             fade.fadetimer = 0;
             fade.rate = FADE_NORM;
             fade.mode = FADE_GO;
             IntroCanceled = 0;
           }
         }
       }
       else textTimer--;

       // if user pressed a key cancel the intro
       keypressed = 0;
       for(i=0;i<KEYTABLE_SIZE;i++)
       {
          if (g_pInput->getPressedKey(i))
          {
             keypressed = 1;
             break;
          }
       }
       if (keypressed && !lastkeypressed)
       {
             fade.dir = FADE_OUT;
             fade.curamt = PAL_FADE_SHADES;
             fade.fadetimer = 0;
             fade.rate = FADE_NORM;
             fade.mode = FADE_GO;
             IntroCanceled = 1;
       }
       lastkeypressed = keypressed;


    }  // end if(fade.dir!=FADE_OUT)

    // draw/erase all three lines of text
    g_pGraphics->sb_font_draw_inverse( (unsigned char*) stTextLine1, x1, y1);
    g_pGraphics->sb_font_draw_inverse( (unsigned char*) stTextLine2, x2, y2);
    g_pGraphics->sb_font_draw_inverse( (unsigned char*) stTextLine3, x3, y3);

    // blit the scrollbuffer to the display
    gamedo_frameskipping_blitonly(pCKP);

    // when fade is complete we're done!
    if (fade.dir==FADE_OUT && fade.mode==FADE_COMPLETE)
    {
      return 0;
    }

    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
  } while(!g_pInput->getPressedCommand(KQUIT) && !crashflag);
  return 1;
}

void keensleft(stCloneKeenPlus *pCKP)
{
int enter, lastenterstate;
unsigned int p;
int x,y,i;
int boxY, boxH;
int boxtimer;
int ep3;

stLevelControl *p_levelcontrol;

	p_levelcontrol = &(pCKP->Control.levelcontrol);

  // on episode 3 we have to subtract one from the map tiles
  // because the tiles start at 31, not 32 like on the other eps
  ep3 = 0;
  if (p_levelcontrol->episode==3) ep3 = 1;

  #define KEENSLEFT_TIME        400

  for(i=0;i<MAX_PLAYERS;i++)
  {
    if (player[i].isPlaying)
    {
      gamepdo_wm_SelectFrame(i, pCKP);
      player[i].hideplayer = 0;
    }
  }
  gamedo_RenderScreen(pCKP);

  #define KEENSLEFT_X        7
  #define KEENSLEFT_Y        11
  #define KEENSLEFT_W        24
  #define KEENSLEFT_H        4

  boxY = KEENSLEFT_Y - (numplayers);
  boxH = KEENSLEFT_H + (numplayers * 2);

  dialogbox(KEENSLEFT_X,boxY,KEENSLEFT_W,boxH);
  g_pGraphics->drawFont( (unsigned char*) getstring("LIVES_LEFT_BACKGROUND"),(KEENSLEFT_X+1)*8,(boxY+1)*8,0);
  g_pGraphics->drawFont( (unsigned char*) getstring("LIVES_LEFT"),((KEENSLEFT_X+7)*8)+4,(boxY+1)*8,0);
  y = ((boxY+2)*8)+4;
  if (numplayers>1) y--;
  for(p=0;p<numplayers;p++)
  {
    x = ((KEENSLEFT_X+1)*8)+4;
    for(i=0;i<player[p].inventory.lives&&i<=10;i++)
    {
    	g_pGraphics->drawSprite_direct(x, y, PMAPDOWNFRAME+playerbaseframes[p]-ep3);
      x+=16;
    }
    y+=18;
  }
  g_pVideoDriver->update_screen();

  g_pSound->playSound(SOUND_KEENSLEFT, PLAY_NOW);

  boxtimer = 0;
  do
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
    g_pTimer->SpeedThrottle();
  } while(!crashflag);

}
