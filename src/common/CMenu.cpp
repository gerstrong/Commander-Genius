//MENU.C
//  The main menu, intro, and other such stuff.

#include "CMenu.h"
#include "CObject.h"
#include "infoscenes/CStory.h"

#include "../StringUtils.h"
#include "../CGameControl.h"
#include "../vorticon/CPassive.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

#define SELMOVE_SPD         3

CMenu::CMenu( char menu_mode, std::string &GamePath, char &Episode, CMap &Map ) :
m_Episode(Episode), m_GamePath(GamePath), m_Map(Map)
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
	{
		initMainMenu();
	}
	else if( m_menu_type == QUIT )
	{
		initConfirmMenu();
	}
	else if( m_menu_type == ENDGAME )
	{
		initConfirmMenu();
	}
	else if( m_menu_type == NEW )
	{
		initNumPlayersMenu();
	}
	else if( m_menu_type == DIFFICULTY )
	{
		initDifficultyMenu();
	}
	else if( m_menu_type == CONFIGURE )
	{
		initConfigureMenu();
	}
	else if( m_menu_type == CONTROLPLAYERS)
	{
		initNumControlMenu();
	}
	else if( m_menu_type == CONTROLS)
	{
		initControlMenu();
	}
	else if( m_menu_type == F1)
	{
		initF1Menu();
	}
	else if( m_menu_type == SAVE)
	{
		initSaveMenu();
	}
	else if( m_menu_type == OVERWRITE)
	{
		initConfirmMenu();
	}
	
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
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 2, "Load Game");
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
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 2, "Load Game");
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
	
	mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 2, "Audio");
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
	int i;
	
	for(i=1;i<=10;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, "     EMPTY       ");
	}
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
		else if( m_menu_type == OVERWRITE ) processOverwriteMenu();
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
		//cleanup(); One cleanup too much here!
		if ( m_selection == 3 )
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
		cleanup();
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;	
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
			mp_Dialog->setObjectType(11, DLG_OBJ_OPTION_TEXT);
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

void CMenu::processSaveMenu()
{
	if( m_selection != -1)
	{
		if(mp_Dialog->m_key == 'u')
		{
			if(mp_Dialog->m_name == "     EMPTY       ")
			{
				mp_Dialog->m_name = "";
			}
			else if(mp_Dialog->m_name != "")
			{
				cleanup();
				init(OVERWRITE);

				m_command = GAME_STATE_SAVE;
				m_gamestate_file = mp_Dialog->m_name;
			}
			mp_Dialog->m_key = 't';
			m_selection = -1;
		}
		else if (mp_Dialog->m_key == 't')
		{
			if(mp_Dialog->m_name == "")
			{
				mp_Dialog->setObjectText(m_selection, "Untitled");
			}
			else
			{
				mp_Dialog->setObjectText(m_selection, mp_Dialog->m_name);
				
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
 Uint16 depth, zoom = 1, filter = 0, gl_filter = 0;
 bool fsmode, aspect;
 std::string buf;
 short retval = 0;
 Uint16 autoframeskip = 0;
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
 */
 
