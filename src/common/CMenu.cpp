//MENU.C
//  The main menu, intro, and other such stuff.

#include "CMenu.h"
#include "CObject.h"

#include "../vorticon/infoscenes/CStory.h"
#include "../vorticon/infoscenes/CCredits.h"
#include "../vorticon/infoscenes/COrderingInfo.h"
#include "../vorticon/infoscenes/CAbout.h"
#include "../vorticon/infoscenes/CHelp.h"

#include "Menu/CVideoSettings.h"

#include "../StringUtils.h"
#include "../CGameControl.h"
#include "../vorticon/CPassive.h"
#include "../sdl/CInput.h"
#include "../sdl/CTimer.h"
#include "../sdl/CSettings.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CVideoDriver.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

#define SELMOVE_SPD         3

CMenu::CMenu( char menu_mode, std::string &GamePath,
			 char &Episode, CMap &Map, CSavedGame &SavedGame,
			 stOption *pOption ) :
m_Episode(Episode),
m_GamePath(GamePath),
m_Map(Map),
m_SavedGame(SavedGame),
mp_option(pOption)
{
	// Create the Main Menu
	mp_MenuSurface = g_pVideoDriver->FGLayerSurface;
	m_menu_mode = menu_mode;
	m_Difficulty = -1; // no difficulty chosen...
	m_NumPlayers = 0; // no player chosen...
	
	m_lastselect = -1;
	m_demoback = false;
	m_overwrite = false;
	m_quit = false;
	m_choosegame = false;
	m_goback = false;
	m_Endgame = false;
	mp_Dialog = NULL;
	mp_InfoScene = NULL;
	m_hideobjects = false;

	// Special Pointer used for Menus that are declared in separated classes
	mp_Menu = NULL;
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
	{
		mp_Menu = new CVideoSettings(m_menu_type);
		//initGraphicsMenu();
		return true;
	}
	else if( m_menu_type == OPTIONS )
		initOptionsMenu();
	else if( m_menu_type == AUDIO )
		initAudioMenu();
	
	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_OLDSCHOOL );
	
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
	mp_Dialog = new CDialog(mp_MenuSurface, 18, 9);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Menu");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "About CG");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Credits");
	
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
	std::string text;
	mp_Dialog = new CDialog(mp_MenuSurface, 0, 0, 22, 12, 'u');
	
	// Load the state-file list
	m_StateFileList = m_SavedGame.getSlotList();
	
	for(Uint32 i=1;i<=10;i++)
	{
		text = "";
		if(i <= m_StateFileList.size())
		{
			text = m_StateFileList.at(i-1);
			mp_Dialog->m_name = text;
		}
		if(text == "")
			text = "     EMPTY       ";
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, text);
	}
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
/// Main Process method fo the menu
void CMenu::process()
{
	// Information Mode
	if(!mp_InfoScene) // show a normal menu
	{
		if( g_pInput->getPressedCommand(IC_HELP) )
		{
			cleanup();
			init(F1);
		}

		if( mp_Menu )
		{
			mp_Menu->processCommon();

			mp_Menu->processSpecific();

			mp_Menu->postProcess();

			if(mp_Menu->mustClose())
			{
				SAFE_DELETE(mp_Menu);
				init(m_menu_type);
			}
		}
		else
		{
			// Get Input for selection
			if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
			{
				m_selection = mp_Dialog->getSelection();
			}
			else if( g_pInput->getPressedCommand(IC_QUIT) )
			{
				m_goback = true;
			}
			mp_Dialog->processInput();

			// Process the Menu Type logic.
			// Which menu is open and what do we have to do?

			// NOTE: Some menus are now inherited classes by a base-class
			// Some variables must be hidden, and it can be achieved through that method
			// I (Gerstrong) would recommend making the others menu also this way.
			// You'd have more files but small ones and better to control than a big one, where
			// you have to seek and seek

			if( m_menu_type == MAIN ) processMainMenu();
			else if( m_menu_type == NEW ) processNumPlayersMenu();
			else if( m_menu_type == DIFFICULTY ) processDifficultyMenu();
			else if( m_menu_type == CONFIGURE ) processConfigureMenu();
			else if( m_menu_type == CONTROLPLAYERS ) processNumControlMenu();
			//else if( m_menu_type == CONTROLS ) processControlMenu();
			else if( m_menu_type == F1 ) processF1Menu();
			else if( m_menu_type == QUIT ) processQuitMenu();
			else if( m_menu_type == ENDGAME ) processEndGameMenu();
			else if( m_menu_type == SAVE ) processSaveMenu();
			else if( m_menu_type == LOAD ) processLoadMenu();
			else if( m_menu_type == OVERWRITE ) processOverwriteMenu();
			//else if( m_menu_type == GRAPHICS ) processGraphicsMenu();
			//else if( m_menu_type == AUDIO ) processAudioMenu();

			// Draw the menu
			if(!mp_Menu) mp_Dialog->draw();
		}
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
		m_selection = -1;
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
			//mp_Dialog->setObjectType(11, DLG_OBJ_DISABLED);
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
			//mp_Dialog->setObjectType(NUM_OPTIONS, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(NUM_OPTIONS+1, "Cancel");
		}
		else
		{
			if(m_selection == NUM_OPTIONS)
			{
				CSettings Settings(mp_option); // Pressed Save,
				Settings.saveGameCfg();
			}
			m_goback = true;
		}
		m_selection = -1;
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
			//mp_Dialog->setObjectType(3, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(4, "Cancel");
		}
		else
		{
			if(m_selection == 3)
			{
				CSettings Settings(mp_option);
				g_pSound->destroy();
				g_pSound->setSoundmode(rate, mode ? true : false, format);
				Settings.saveDrvCfg();
				g_pSound->init();
				ok = g_pSound->loadSoundData(m_Episode, m_GamePath);
			}
			m_goback = true;
		}
		m_selection = -1;
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
			case 0:
				mp_InfoScene = new CHelp(m_GamePath, m_Episode, "Menu");
				break;
			case 1:
				mp_InfoScene = new CHelp(m_GamePath, m_Episode, "Game");
				break;
			case 2:
				m_Map.m_animation_enabled = false;
				mp_InfoScene = new CStory(m_GamePath, m_Episode);
				break;
			case 3:
				mp_InfoScene = new COrderingInfo(m_GamePath, m_Episode);
				break;
			case 4:
				mp_InfoScene = new CAbout(m_GamePath, m_Episode, "ID");
				break;
			case 5:
				mp_InfoScene = new CAbout(m_GamePath, m_Episode, "CG");
				break;
			case 6:
				mp_InfoScene = new CCredits(m_GamePath, m_Episode);
				break;
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
	std::string text;
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
			else if(m_overwrite == true)
			{
				mp_Dialog->processInput(int(m_lastselect));
				mp_Dialog->m_name = "";
				mp_Dialog->m_key = 't';
				m_selection = -1;
				m_lastselect = -1;
				m_overwrite = false;
			}
			else if(mp_Dialog->m_name != "")
			{
				m_lastselect = m_selection;
				cleanup();
				init(OVERWRITE);
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
				m_saveslot = int(m_selection) + 1;
				m_SavedGame.prepareSaveGame(m_saveslot, mp_Dialog->m_name);
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
	else
	{
		m_selection = m_lastselect;
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
	if( m_selection != -1)
	{
		m_saveslot = int(m_selection) + 1;
		m_SavedGame.prepareLoadGame(m_saveslot);
		m_selection = -1;
	}
	
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
	// Close the old menu
	SAFE_DELETE(mp_Menu);
	SAFE_DELETE(mp_Dialog);
}

CMenu::~CMenu()
{
	SAFE_DELETE(mp_Menu);
	SAFE_DELETE(mp_InfoScene);
}
