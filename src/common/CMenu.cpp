//MENU.C
//  The main menu, intro, and other such stuff.


#include "CMenu.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"

#define SELMOVE_SPD         3

CMenu::CMenu( char menu_mode )
{
	// Create the Main Menu
	mp_MenuSurface = g_pVideoDriver->FGLayerSurface;
	m_menu_mode = menu_mode;
	m_Difficulty = 0; // easy if none chosen
	m_NumPlayers = 0; // no player chosen...
	m_goback = false;
	m_Endgame = false;
	mp_Dialog = NULL;
}

////
// Initialization Routines
////
bool CMenu::init( char menu_type )
{
	m_menu_type = menu_type;
	m_selection = -1; // Nothing has been selected yet.

	if( m_menu_type == MAIN )
	{
		initMainMenu();
	}
	else if( m_menu_type == START )
	{
		initNumPlayersMenu();
	}

	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_OLDSCHOOL );
	return true;
}

void CMenu::initMainMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 18, 13);

	// When in Intro, Title, Demo mode
	if( m_menu_mode == PASSIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Start");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 2, "Load");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 3, "Story");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 4, "Highscores");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 5, "Options");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 6, "Back to Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 7, "Back to Title");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 8, "About CG");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 9, "Ordering Info");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Quit");
	}

	// When Player is playing
	// TODO: This still must be adapted to ingame situation
	if( m_menu_mode == ACTIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "End Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 2, "Load");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 3, "Story");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 4, "Highscores");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 5, "Options");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 6, "Back to Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 7, "Back to Title");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 8, "About CG");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 9, "Ordering Info");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Quit");
	}
}

void CMenu::initNumPlayersMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 18, 13);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Single");
	mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 2, "Two Players");
	mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 3, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 4, "Back");
}

////
// Process Routines
////
void CMenu::process()
{
	// Get Input for selection
	if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
	{
		m_selection = mp_Dialog->getSelection();
	}
	else if( g_pInput->getPressedKey(KQUIT) )
	{
		m_goback = true;
	}
	mp_Dialog->processInput();

	// Draw the menu
	mp_Dialog->draw();

	// Process the Menu Type logic.
	// Which menu is open and what do we have to do?
	if( m_menu_type == MAIN ) processMainMenu();
	else if( m_menu_type == START ) processNumPlayersMenu();
}

void CMenu::processMainMenu()
{
	if( m_menu_mode == PASSIVE )
	{
		if( m_selection == 0 ) // Start Game
		{
			cleanup();
			init(START);
		}
	}
	else if( m_menu_mode == ACTIVE )
	{
		if( m_selection == 0 ) // End Game
		{
			cleanup();
			m_Endgame = true;
		}
	}


	if( m_selection == 9 ) // Quit
	{
		cleanup();
		m_menu_type = QUIT;
	}
}

void CMenu::processNumPlayersMenu()
{
	if( m_selection == -1) return;

	cleanup();
	if( m_selection < 2 )
	{
		m_NumPlayers = m_selection + 1;	
	}
	else
	{
		m_goback = true;
	}

	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}

}

////
// Cleanup Routines
////
void CMenu::cleanup()
{
	delete mp_Dialog;
	mp_Dialog = NULL;
}

CMenu::~CMenu()
{
}

////
// Old Structures, they still have to be adapted
////
/*int CMenu::getChoice(stCloneKeenPlus *pCKP,int defaultopt)
{


	do
	{
		gamedo_AnimatedTiles();
		MainMenu.processlogic();

		if(g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedKey(KENTER))
			break;
		if (g_pInput->getPressedKey(KQUIT))
		{
			QuitState = NO_QUIT;
			return MAINMNU_QUIT;
		}
		MainMenu.draw();
	} while(!g_pInput->getExitEvent());

    if(g_pInput->getExitEvent())
	{
		QuitState = QUIT_PROGRAM;
    	return 0;
	}

	selection = MainMenu.getSelection();

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
    else if(selection == MAINMNU_STORY)
    {
    	char *text;
    	int textsize;

    	textsize = readStoryText(&text,
								 pCKP->Control.levelcontrol.episode,
								 pCKP->Resources.GameDataDirectory); // Read text from
		// and store it at the text pointer

    	if(textsize > 0)
    	{
    		showmapatpos(90, STORYBOARD_X, STORYBOARD_Y, pCKP);
			showPage(text,textsize);

			free(text);
		}
    	else if(textsize == 0)
    	{
    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. Are you sure that there is any story text?");
    	}
    }
	else if (selection==MAINMNU_QUIT)
    {
		QuitState = QUIT_PROGRAM;
    	return MAINMNU_QUIT;
    }
}

int getDifficulty(stCloneKeenPlus *pCKP)
{
	int x;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");


	// Prepare the Games Menu
	CDialog DifficultyMenu(g_pVideoDriver->FGLayerSurface, 14, 6);

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
		DifficultyMenu.draw();
	} while(true);

	return DifficultyMenu.getSelection();
}

int AudioDlg(stCloneKeenPlus *pCKP)
{
	int selection;
	int ok=0;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE");	// Load the Title Bitmap

	int rate = 0;
	Uint16 format = 0;
	short mode=0;

	// Prepare the Games Menu
	CDialog AudioMenu(g_pVideoDriver->FGLayerSurface, 32, 8);

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
                CSettings Settings;
				g_pSound->destroy();
				g_pSound->setSoundmode(rate, mode ? true : false, format);
				Settings.saveDrvCfg();
				g_pSound->init();
				ok = g_pSound->loadSoundData(pCKP->Control.levelcontrol.episode,
											  pCKP->Resources.GameDataDirectory);
				break;
			}
			if(selection == 4)
				break;

		}
		AudioMenu.processlogic();
		AudioMenu.draw();
	} while(1);

	return ok;
}

void OptionsDlg(stCloneKeenPlus *pCKP)
{
	int selection;
	int x,i;
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE"); 	// Load the Title Bitmap

	std::string buf;

	// Prepare the Games Menu
	CDialog OptionsMenu(g_pVideoDriver->FGLayerSurface, 34, 13);

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
		OptionsMenu.draw();
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
	CBitmap *bm_title = g_pGfxEngine->getBitmap("TITLE"),
			*bm_f1help = g_pGfxEngine->getBitmap("F1HELP");

	x = (320/2)-(bm_title->getWidth()/2);

	width  = g_pVideoDriver->getWidth();
	height = g_pVideoDriver->getHeight();
	depth  = g_pVideoDriver->getDepth();

	zoom   = g_pVideoDriver->getZoomValue();
	filter = g_pVideoDriver->getFiltermode();
	
	g_pVideoDriver->initResolutionList();
	
	g_pVideoDriver->setMode(width, height, depth);

	// Prepare the Games Menu
	CDialog DisplayMenu(g_pVideoDriver->FGLayerSurface, 32, 12);

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

	if(opengl)
	DisplayMenu.addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);
	else
	DisplayMenu.addObject(DLG_OBJ_DISABLED, 1, 7, buf);

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
				fsmode = !fsmode;
				if(fsmode)
					g_pVideoDriver->isFullscreen(true);
				else
					g_pVideoDriver->isFullscreen(false);
				if(fsmode)
				{
					DisplayMenu.setObjectText(1,"Fullscreen mode");
					g_pVideoDriver->initResolutionList();
					buf = "Resolution: " + itoa(g_pVideoDriver->m_Resolutionlist.back().width) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.back().height) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.back().depth);
					DisplayMenu.setObjectText(0,buf);
					g_pVideoDriver->setMode(g_pVideoDriver->m_Resolutionlist.back().width, g_pVideoDriver->m_Resolutionlist.back().height, g_pVideoDriver->m_Resolutionlist.back().depth);
				}
				else
				{
					DisplayMenu.setObjectText(1,"Windowed mode");
					g_pVideoDriver->initResolutionList();
					buf = "Resolution: " + itoa(g_pVideoDriver->m_Resolutionlist.front().width) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.front().height) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.front().depth);
					DisplayMenu.setObjectText(0,buf);
				}
				if(!fsmode)
					g_pVideoDriver->isFullscreen(true);
				else
					g_pVideoDriver->isFullscreen(false);
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
					if(filter>0)
                        filter = zoom-1;
				}
				DisplayMenu.setObjectText(2,buf);

                if(filter == 0)
					DisplayMenu.setObjectText(3,"No Filter");
				else if(filter == 1)
					DisplayMenu.setObjectText(3,"Scale2x Filter");
				else if(filter == 2)
					DisplayMenu.setObjectText(3,"Scale3x Filter");
				else if(filter == 3)
					DisplayMenu.setObjectText(3,"Scale4x Filter");
			}
			else if(selection == 3)
			{
                if(opengl)
				{
                    filter = (filter >= 3) ? 0 : filter+1;
				}
				else
				{
                    filter = (filter > 0) ? 0 : zoom-1;
				}

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

				CSettings Settings;
				Settings.saveDrvCfg();
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
		DisplayMenu.draw();
	} while(1);

	return retval;
}

char configmenu(stCloneKeenPlus *pCKP,int ingame)
{
	int selection;
	int x;
	CBitmap *title_bitmap = g_pGfxEngine->getBitmap("TITLE"),
			*bm_f1help = g_pGfxEngine->getBitmap("F1HELP");

	// Prepare the Games Menu
	CDialog OptionsMenu(g_pVideoDriver->FGLayerSurface, 14, 8);

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
			OptionsMenu.setSDLSurface(g_pVideoDriver->FGLayerSurface);
		}
		OptionsMenu.processlogic();
		OptionsMenu.draw();
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

	// Prepare the Games Menu
	CDialog ControlsMenu(g_pVideoDriver->FGLayerSurface, 38, 22);

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
					ControlsMenu.draw();
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
					ControlsMenu.draw();
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
		ControlsMenu.draw();
	} while(1);
	return 0;
}

// This function shows the Story of Commander Keen!
void showPage(const std::string& str_text, int textsize)
{
	CTextViewer TextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 136);
	TextViewer.loadText(str_text);

	AllPlayersInvisible();

	TextViewer.processCycle();
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
	    	Uint16 f = PMAPDOWNFRAME-(episode==3);
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

}*/
