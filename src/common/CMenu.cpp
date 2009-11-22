//MENU.C
//  The main menu, intro, and other such stuff.

#include "CMenu.h"
#include "CObject.h"
#include "options.h"
#include "infoscenes/CStory.h"

#include "../StringUtils.h"
#include "../CGameControl.h"
#include "../vorticon/CPassive.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../sdl/CTimer.h"
#include "../sdl/CSettings.h"
#include "../sdl/sound/CSound.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

#define SELMOVE_SPD         3

CMenu::CMenu( char menu_mode, std::string &GamePath,
			  char &Episode, CMap &Map, CSavedGame &SavedGame ) :
			  m_Episode(Episode), m_GamePath(GamePath),
			  m_Map(Map), m_SavedGame(SavedGame)
{
	// Create the Main Menu
	mp_MenuSurface = g_pVideoDriver->FGLayerSurface;
	m_menu_mode = menu_mode;
	m_Difficulty = -1; // no difficulty chosen...
	m_NumPlayers = 0; // no player chosen...
	
	m_demoback = false;
	m_overwrite = false;
	m_quit = false;
	m_choosegame = false;
	m_goback = false;
	m_Endgame = false;
	mp_Dialog = NULL;
	mp_InfoScene = NULL;
	m_hideobjects = false;
}

////
// Initialization Routines
////
bool CMenu::init( char menu_type )
{
	m_menu_type = menu_type;
	m_goback = false;
	m_selection = -1; // Nothing has been selected yet.
	
	if( m_menu_type == MAIN )
		initMainMenu();
	else if( m_menu_type == QUIT )
		initConfirmMenu();
	else if( m_menu_type == ENDGAME )
		initConfirmMenu();
	else if( m_menu_type == NEW )
		initNumPlayersMenu();
	else if( m_menu_type == DIFFICULTY )
		initDifficultyMenu();
	else if( m_menu_type == CONFIGURE )
		initConfigureMenu();
	else if( m_menu_type == CONTROLPLAYERS )
		initNumControlMenu();
	else if( m_menu_type == CONTROLS )
		initControlMenu();
	else if( m_menu_type == F1 )
		initF1Menu();
	else if( m_menu_type == SAVE )
		initSaveMenu();
	else if( m_menu_type == LOAD )
		initSaveMenu();
	else if( m_menu_type == OVERWRITE )
		initConfirmMenu();
	else if( m_menu_type == GRAPHICS )
		initGraphicsMenu();
	else if( m_menu_type == OPTIONS )
		initOptionsMenu();
	else if( m_menu_type == AUDIO )
		initAudioMenu();
	
	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_OLDSCHOOL );
	
	// Load the state-file list
	m_StateFileList = m_SavedGame.getSlotList();

	return true;
}

void CMenu::initMainMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 17, 10);
	
	// When in Intro, Title, Demo mode
	if( m_menu_mode == PASSIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 3, "Save Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 4, "Highscores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Back to Demo");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Choose Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
	}
	
	// When Player is playing
	// TODO: This still must be adapted to ingame situation
	if( m_menu_mode == ACTIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 3, "Save Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED,  1, 4, "Highscores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 6, "Back to Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "End Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
	}
}

void CMenu::initNumPlayersMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, MAX_PLAYERS+2);
	int i;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1 Player");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, i, itoa(i)+" Player");
	}
}

void CMenu::initDifficultyMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 11, 5);
	
	mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 1, "Easy");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Normal");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Hard");
}

void CMenu::initConfigureMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, 6);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");
}

void CMenu::initNumControlMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, MAX_PLAYERS+2);
	int i;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Player 1");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, "Player "+itoa(i));
	}
}

void CMenu::initControlMenu()
{
	std::string buf;
	std::string buf2;
	int player = m_NumPlayers - 1;
	mp_Dialog = new CDialog(mp_MenuSurface, 36, 16);
	
	g_pInput->getEventName(IC_LEFT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Left:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
	
	g_pInput->getEventName(IC_UP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Up:     " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);
	
	g_pInput->getEventName(IC_RIGHT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Right:  " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, buf);
	
	g_pInput->getEventName(IC_DOWN, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Down:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, buf);
	
	g_pInput->getEventName(IC_JUMP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Jump:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, buf);
	
	g_pInput->getEventName(IC_POGO, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Pogo:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, buf);
	
	g_pInput->getEventName(IC_FIRE, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Fire:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);
	
	g_pInput->getEventName(IC_STATUS, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Status: " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, buf);
	
	g_pInput->getEventName(IC_HELP, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Help:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 9, buf);
	
	g_pInput->getEventName(IC_QUIT, player, buf2);
	buf = "P"+itoa(m_NumPlayers)+" Quit:   " + buf2;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, buf);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 2, 11, "Reset Controls");
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 13, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 14, "Return");
}

void CMenu::initF1Menu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 18, 7);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About CG");
	
	// In the Help system let's hide all objects like Bitmaps, players, enemies, etc.
	m_hideobjects = true;
}

void CMenu::initConfirmMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 0, 0, 22, 5, 'l');
	
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, "  Are you certain?  ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Yes");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 15, 3, "No");
}

void CMenu::initSaveMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 0, 0, 22, 12, 'u');
	
	for(int i=1;i<=10;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, "     EMPTY       ");
	}
}

void CMenu::initGraphicsMenu()
{
	Uint16 width = g_pVideoDriver->getWidth(), height = g_pVideoDriver->getHeight(), depth = g_pVideoDriver->getDepth();
	Uint16 zoom = g_pVideoDriver->getZoomValue(), filter = g_pVideoDriver->getFiltermode(), gl_filter = g_pVideoDriver->getOGLFilter(), autoframeskip = g_pTimer->getFrameRate();
	bool fsmode, aspect = g_pVideoDriver->getAspectCorrection(), opengl = g_pVideoDriver->isOpenGL();
	std::string buf;
	
	g_pVideoDriver->initResolutionList();
	
	g_pVideoDriver->setMode(width, height, depth);
	
	mp_Dialog = new CDialog(mp_MenuSurface, 32, 12);
	
	buf = "Resolution: " + itoa(width) + "x" + itoa(height) + "x" + itoa(depth);
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, buf);
	if(g_pVideoDriver->getFullscreen())
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Fullscreen mode");
		fsmode = true;
	}
	else
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Windowed mode");
		fsmode = false;
	}
	
	if(!opengl)
	{
		if(zoom == 1)
			mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "No Scale");
		else
			mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Scale: " + itoa(zoom) );
	}
	else
	{
		if(gl_filter == 1)
			mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "OGL Filter: Linear");
		else
			mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "OGL Filter: Nearest");
	}
	
	if(filter == 0)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "No Filter");
	else if(filter == 1)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale2x Filter");
	else if(filter == 2)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale3x Filter");
	else if(filter == 3)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Scale4x Filter");
	else
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Unknown Filter");
	
	if(opengl)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "OpenGL Acceleration");
	else
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Software Rendering");
	
	if(autoframeskip)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Auto-Frameskip : " + itoa(autoframeskip) + " fps");
	
	buf = "OGL Aspect Ratio ";
	buf += aspect ? "enabled" : "disabled";
	
	if(opengl)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, buf);
	else
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 7, buf);
	
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 9, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 10, "Return");
}

void CMenu::initOptionsMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 22, NUM_OPTIONS+5);
	int i;
	std::string buf;
	
	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		buf = mp_option[i].name + ": ";
		
		if(mp_option[i].value)
			buf += "Enabled";
		else
			buf += "Disabled";
		
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, buf);
	}
	
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, NUM_OPTIONS+2, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, NUM_OPTIONS+3, "Return");
}

void CMenu::initAudioMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 22, 8);
	int rate = 0;
	Uint16 format = 0;
	short mode=0;
	
	rate = g_pSound->getAudioSpec().freq;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Rate: " + itoa(rate) +" kHz");
	
	format = g_pSound->getAudioSpec().format;
	std::string buf;
	if(format == AUDIO_S16)
		buf = "Format: 16 bits";
	else
		buf = "Format: 8 bits";
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, buf);
	
	mode = g_pSound->getAudioSpec().channels - 1;
	if(mode == 1)
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Mode: Stereo");
	else
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Mode: Mono");
	
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 5, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Return");
}

////
// Process Routines
////
void CMenu::process()
{
	// Information Mode
	if(!mp_InfoScene) // show a normal menu
	{
		// Get Input for selection
		if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
		{
			m_selection = mp_Dialog->getSelection();
		}
		else if( g_pInput->getPressedCommand(IC_HELP) )
		{
			cleanup();
			init(F1);
		}
		else if( g_pInput->getPressedCommand(IC_QUIT) )
		{
			m_goback = true;
		}
		mp_Dialog->processInput();
		
		// Draw the menu
		mp_Dialog->draw();
		
		// Process the Menu Type logic.
		// Which menu is open and what do we have to do?
		if( m_menu_type == MAIN ) processMainMenu();
		else if( m_menu_type == NEW ) processNumPlayersMenu();
		else if( m_menu_type == DIFFICULTY ) processDifficultyMenu();
		else if( m_menu_type == CONFIGURE ) processConfigureMenu();
		else if( m_menu_type == CONTROLPLAYERS ) processNumControlMenu();
		else if( m_menu_type == CONTROLS ) processControlMenu();
		else if( m_menu_type == F1 ) processF1Menu();
		else if( m_menu_type == QUIT ) processQuitMenu();
		else if( m_menu_type == ENDGAME ) processEndGameMenu();
		else if( m_menu_type == SAVE ) processSaveMenu();
		else if( m_menu_type == LOAD ) processLoadMenu();
		else if( m_menu_type == OVERWRITE ) processOverwriteMenu();
		else if( m_menu_type == GRAPHICS ) processGraphicsMenu();
		else if( m_menu_type == AUDIO ) processAudioMenu();
	}
	else // InfoScene is enabled! show it instead of the menu
	{
		mp_InfoScene->process();
		
		if(mp_InfoScene->destroyed())
		{
			SAFE_DELETE(mp_InfoScene); // Destroy the InfoScene and go back to the menu!!!
									   // Restore the old map, that was hidden behind the scene
			g_pVideoDriver->setScrollBuffer(&m_Map.m_scrollx_buf, &m_Map.m_scrolly_buf);
			m_Map.drawAll();
			m_Map.m_animation_enabled = true;
		}
	}
}

void CMenu::processMainMenu()
{	
	if( m_selection != -1)
	{
		if( m_menu_mode == PASSIVE )
		{
			if( m_selection == 5 ) // Back to Demo
			{
				m_demoback = true;
			}
			if( m_selection == 6 ) // Choose Game
			{
				m_choosegame = true;
			}
		}
		else if( m_menu_mode == ACTIVE )
		{
			if( m_selection == 5 ) // Back to Game
			{
				m_goback = true;
			}
			if( m_selection == 6 ) // End Game
			{
				cleanup();
				init(ENDGAME);
			}
		}
		
		if( m_selection == 0 ) // Start Game
		{
			cleanup();
			init(NEW);
		}
		if( m_selection == 1 ) // Load Game
		{
			cleanup();
			init(LOAD);
		}
		if( m_selection == 2 ) // Save Game
		{
			cleanup();
			init(SAVE);
		}
		if( m_selection == 3 ) // Highscores
		{
		}
		if( m_selection == 4 ) // Options
		{
			cleanup();
			init(CONFIGURE);
		}
		if( m_selection == 7 ) // Quit
		{
			cleanup();
			init(QUIT);
		}
	}
	
	if( m_menu_mode == PASSIVE )
	{
		if(m_goback)
		{
			cleanup();
			init(QUIT);
		}
	}
}

void CMenu::processNumPlayersMenu()
{
	if( m_selection != -1)
	{
		cleanup();
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;	
			init(DIFFICULTY);
		}
		else
		{
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processDifficultyMenu()
{
	if( m_selection != -1)
	{
		cleanup();
		if( m_selection > 0 )
		{
			m_Difficulty = m_selection-1;	
		}
		else
		{
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(NEW);
	}
	return;
}

void CMenu::processConfigureMenu()
{
	if( m_selection != -1)
	{
		if ( m_selection == 0 )
		{
			cleanup();
			init(GRAPHICS);
		}
		else if ( m_selection == 1 )
		{
			cleanup();
			init(AUDIO);
		}
		else if ( m_selection == 2 )
		{
			cleanup();
			init(OPTIONS);
		}
		else if ( m_selection == 3 )
		{
			cleanup();
			init(CONTROLPLAYERS);
		}
		else
		{
			m_goback = true;	
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processNumControlMenu()
{
	if( m_selection != -1)
	{
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;	
			cleanup();
			init(CONTROLS);
		}
		else
		{
			m_goback = true;	
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}
	return;
}

void CMenu::processControlMenu()
{
	if( m_selection != -1)
	{
		if( m_selection < 11 )
		{
			if(m_selection < MAX_COMMANDS)
			{
				int item = m_selection;
				std::string buf;
				std::string buf2;
				
				buf = mp_Dialog->m_dlgobject[m_selection]->m_OptionText->m_text;
				buf = buf.erase(11);
				
				buf2 = "*Waiting for Input*";
				mp_Dialog->setObjectText(m_selection, buf + buf2);
				mp_Dialog->m_key = 'n';
				
				do
				{
					if(g_pInput->readNewEvent(m_NumPlayers-1,item))
					{
						g_pInput->getEventName(item, m_NumPlayers-1, buf2);
						mp_Dialog->setObjectText(m_selection, buf + buf2);
						mp_Dialog->m_key = 'u';
						m_selection = -1;
					}
				} while (buf2 == "*Waiting for Input*");
			}
			else
			{
				g_pInput->resetControls();
				cleanup();
				init(CONTROLS);
			}
			mp_Dialog->setObjectText(11, "Save and Return");
			mp_Dialog->setObjectType(11, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(12, "Cancel");
		}
		else
		{
			if(m_selection == 11)
			{
				g_pInput->saveControlconfig();
			}
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		m_NumPlayers = 0;
		init(CONTROLPLAYERS);
	}
}

void CMenu::processGraphicsMenu()
{
	Uint16 width = g_pVideoDriver->getWidth(), height = g_pVideoDriver->getHeight(), depth = g_pVideoDriver->getDepth();
	Uint16 zoom = g_pVideoDriver->getZoomValue(), filter = g_pVideoDriver->getFiltermode(), gl_filter = g_pVideoDriver->getOGLFilter(), autoframeskip = g_pTimer->getFrameRate();
	bool fsmode, aspect = g_pVideoDriver->getAspectCorrection(), opengl = g_pVideoDriver->isOpenGL();
	std::string buf;
	
	g_pVideoDriver->initResolutionList();
	
	g_pVideoDriver->setMode(width, height, depth);
	
	if( m_selection != -1)
	{
		if( m_selection < 7 )
		{
			if(m_selection == 0)
			{
				// Now the part of the resolution list
				st_resolution Resolution;
				Resolution = g_pVideoDriver->setNextResolution();
				
				buf = "Resolution: " + itoa(Resolution.width) + "x" + itoa(Resolution.height) + "x" + itoa(Resolution.depth);
				mp_Dialog->setObjectText(0,buf);
			}
			else if(m_selection == 1)
			{
				fsmode = !fsmode;
				if(fsmode)
					g_pVideoDriver->isFullscreen(true);
				else
					g_pVideoDriver->isFullscreen(false);
				if(fsmode)
				{
					mp_Dialog->setObjectText(1,"Fullscreen mode");
					g_pVideoDriver->initResolutionList();
					buf = "Resolution: " + itoa(g_pVideoDriver->m_Resolutionlist.back().width) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.back().height) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.back().depth);
					mp_Dialog->setObjectText(0,buf);
					g_pVideoDriver->setMode(g_pVideoDriver->m_Resolutionlist.back().width, g_pVideoDriver->m_Resolutionlist.back().height, g_pVideoDriver->m_Resolutionlist.back().depth);
				}
				else
				{
					mp_Dialog->setObjectText(1,"Windowed mode");
					g_pVideoDriver->initResolutionList();
					buf = "Resolution: " + itoa(g_pVideoDriver->m_Resolutionlist.front().width) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.front().height) + "x" + itoa(g_pVideoDriver->m_Resolutionlist.front().depth);
					mp_Dialog->setObjectText(0,buf);
				}
				if(!fsmode)
					g_pVideoDriver->isFullscreen(true);
				else
					g_pVideoDriver->isFullscreen(false);
			}
			else if(m_selection == 2)
			{
				if(opengl)
				{
					gl_filter = (gl_filter==1) ? 0 : 1;
					buf = (gl_filter == 1) ? "OGL Filter: Linear" : "OGL Filter: Nearest";
					mp_Dialog->setObjectText(2,buf);
				}
				else
				{
					zoom = (zoom >= 4) ? 1 : zoom+1;
					buf = (zoom == 1) ? "No scale" : "Scale: " + itoa(zoom);
					if(filter>0)
						filter = zoom-1;
				}
				mp_Dialog->setObjectText(2,buf);
				
				if(filter == 0)
					mp_Dialog->setObjectText(3,"No Filter");
				else if(filter == 1)
					mp_Dialog->setObjectText(3,"Scale2x Filter");
				else if(filter == 2)
					mp_Dialog->setObjectText(3,"Scale3x Filter");
				else if(filter == 3)
					mp_Dialog->setObjectText(3,"Scale4x Filter");
			}
			else if(m_selection == 3)
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
					mp_Dialog->setObjectText(3,"No Filter");
				else if(filter == 1)
					mp_Dialog->setObjectText(3,"Scale2x Filter");
				else if(filter == 2)
					mp_Dialog->setObjectText(3,"Scale3x Filter");
				else if(filter == 3)
					mp_Dialog->setObjectText(3,"Scale4x Filter");
			}
			else if(m_selection == 4)
			{
				opengl = !opengl; // switch the mode!!
				
				if(opengl)
					mp_Dialog->setObjectText(4,"OpenGL Acceleration");
				else
					mp_Dialog->setObjectText(4,"Software Rendering");
			}
			else if(m_selection == 5)
			{
				if(autoframeskip < 120)
					autoframeskip += 10;
				else
					autoframeskip = 10;
				buf = "Auto-Frameskip : " + itoa(autoframeskip) + " fps";
				
				mp_Dialog->setObjectText(5, buf);
			}
			else if(m_selection == 6)
			{
				aspect = !aspect;
				
				if(aspect)
					mp_Dialog->setObjectText(6,"OGL Aspect Ratio Enabled");
				else
					mp_Dialog->setObjectText(6,"OGL Aspect Ratio Disabled");
				
			}
			mp_Dialog->setObjectText(7, "Save and Return");
			mp_Dialog->setObjectType(7, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(8, "Cancel");
		}
		else
		{
			if(m_selection == 7)
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
				g_pTimer->setFrameRate(DEFAULT_LPS, autoframeskip, DEFAULT_SYNC);
				g_pVideoDriver->setAspectCorrection(aspect);
				
				CSettings Settings(0);
				Settings.saveDrvCfg();
			}
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}
}

void CMenu::processOptionsMenu()
{
	std::string buf;
	
	if( m_selection != -1)
	{
		if(m_selection < NUM_OPTIONS)
		{
			buf = mp_option[m_selection].name + ": ";
			
			if(mp_option[m_selection].value)
			{
				mp_option[m_selection].value = 0;
				buf += "Disabled";
			}
			else
			{
				mp_option[m_selection].value = 1;
				buf += "Enabled";
			}
			
			mp_Dialog->setObjectText(m_selection, buf);
			mp_Dialog->setObjectText(NUM_OPTIONS, "Save and Return");
			mp_Dialog->setObjectType(NUM_OPTIONS, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(NUM_OPTIONS+1, "Cancel");
		}
		else
		{
			if(m_selection == NUM_OPTIONS)
			{
				CSettings Settings(0); // Pressed Save,
				Settings.saveGameCfg();
			}
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}
}

void CMenu::processAudioMenu()
{
	int ok=0, rate = 0;
	Uint16 format = 0;
	short mode=0;
	
	std::string buf;
	
	if( m_selection != -1)
	{
		if(m_selection < 3)
		{
			if(m_selection == 0)
			{
				switch(rate)
				{
					case 22050: rate = 44100; break;
					case 11025: rate = 22050; break;
					default: rate = 11025; break;
				}
				mp_Dialog->setObjectText(0, "Rate: " + itoa(rate) + " kHz");
			}
			
			if(m_selection == 1)
			{
				if( format == AUDIO_S16 ) format = AUDIO_U8;
				else if( format == AUDIO_U8 ) format = AUDIO_S16;
				if(format == AUDIO_S16)
					buf = "Format: 16 bits";
				else
					buf = "Format: 8 bits";
				mp_Dialog->setObjectText(1, buf);
			}
			else if(m_selection == 2)
			{
				mode = !mode;
				if(!mode)
					mp_Dialog->setObjectText(2,"Mode: Mono");
				else
					mp_Dialog->setObjectText(2,"Mode: Stereo");
			}
			mp_Dialog->setObjectText(3, "Save and Return");
			mp_Dialog->setObjectType(3, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(4, "Cancel");
		}
		else
		{
			if(m_selection == 3)
			{
				CSettings Settings(0);
				g_pSound->destroy();
				g_pSound->setSoundmode(rate, mode ? true : false, format);
				Settings.saveDrvCfg();
				g_pSound->init();
				ok = g_pSound->loadSoundData(m_Episode, m_GamePath);
			}
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}
}
void CMenu::processF1Menu()
{
	if( m_selection != -1)
	{
		// no cleanups here, because later we return back to that menu
		switch(m_selection)
		{
				/*case 0:
				 mp_InfoScene = new CAboutGame();
				 break;*/
			case 1:
				m_Map.m_animation_enabled = false;
				mp_InfoScene = new CStory(m_GamePath, m_Episode);
				break;
				/*case 2:
				 mp_InfoScene = new COrderingInfo();
				 break;
				 case 3:
				 mp_InfoScene = new CAboutID();
				 break;
				 case 4:
				 mp_InfoScene = new CCredits();
				 break;*/
		}
		m_selection = -1;
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processQuitMenu()
{
	mp_Dialog->setObjectText(0, "   Quit the game?   ");
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			cleanup();
			m_quit = true;
		}
		else if ( m_selection == 2 )
		{
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processEndGameMenu()
{
	mp_Dialog->setObjectText(0, "   End your game?   ");
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			cleanup();
			m_Endgame = true;
		}
		else if ( m_selection == 2 )
		{
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

// TODO: PLease put more comments in order to understand what is supposed to be done.
void CMenu::processSaveMenu()
{
	if( m_selection != -1)
	{
		if(mp_Dialog->m_key == 'u')
		{
			if(mp_Dialog->m_name == "     EMPTY       ")
			{
				mp_Dialog->m_name = "";
				mp_Dialog->m_key = 't';
				m_selection = -1;
			}
			else if(mp_Dialog->m_name != "")
			{
				cleanup();
				init(OVERWRITE);
				// TODO: And what if we don't have to overwrite??
			}
			else
			{
				mp_Dialog->m_key = 't';
				m_selection = -1;
			}
		}
		else if (mp_Dialog->m_key == 't')
		{
			if(mp_Dialog->m_name == "")
			{
				mp_Dialog->setObjectText(m_selection, "Untitled");
				mp_Dialog->m_name = "Untitled";
			}
			else
			{
				mp_Dialog->setObjectText(m_selection, mp_Dialog->m_name);
				m_saveslot = int(m_selection) + 1;
				m_SavedGame.prepareSaveGame(m_saveslot, mp_Dialog->m_name);
			}
			mp_Dialog->m_key = 'u';
			m_selection = -1;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processLoadMenu()
{
	/*if( m_selection != -1)
	{
		m_saveload = 'l';
	}*/
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

// TODO: You really should see to get that dialog type (Yes/No) making it more universal and templatized.
// If you don't you may loose the your own insight into the code.
void CMenu::processOverwriteMenu()
{
	mp_Dialog->setObjectText(0, "Overwrite this save?");
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			m_overwrite = true;
			m_goback = true;
		}
		else if ( m_selection == 2 )
		{
			m_overwrite = false;
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(SAVE);
	}
	return;
}

////
// Cleanup Routines
////
void CMenu::cleanup()
{
	m_Map.m_animation_enabled = true;
	m_hideobjects = false;
	delete mp_Dialog;
	mp_Dialog = NULL;
}

CMenu::~CMenu()
{
}
