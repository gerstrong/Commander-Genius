/*
 * CInput.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>
#include <stdio.h>

#include "CInput.h"
#include "CVideoDriver.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "StringUtils.h"
#include "fileio/CConfiguration.h"
#include "common/CSettings.h"

#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
#include "sys/wizgp2x.h"
#endif

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#define MOUSEWRAPPER 1
#endif

CInput::CInput()
{
#if defined(WIZ) || defined(GP2X)
    volume_direction = VOLUME_NOCHG;
	volume = 60-VOLUME_CHANGE_RATE;
	WIZ_AdjustVolume(VOLUME_UP);
#endif
	g_pLogFile->ftextOut("Starting the input driver...<br>");
	memset(InputCommand, 0, NUM_INPUTS*MAX_COMMANDS*sizeof(stInputCommand));

	for(size_t c=1 ; c<= NUM_INPUTS ; c++)
		resetControls(c);
	memset(&Event,0,sizeof(Event));
#if !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
	loadControlconfig(); // we want to have the default settings in all cases
	startJoyDriver(); // not for iPhone for now, could cause trouble (unwanted input events)
#endif
}

/**
 * \brief This will reset the player controls how they saved before.
 * 		  The Default controls are going to be restored when this function
 * 		  is executed
 * \param	player	Number of player of which the controls will be reset (1-4)
 */
void CInput::resetControls(int player) {
	int i;

	if(player == 0)
	{
		player = 1;
		g_pLogFile->textOut("Warning when resetting controls. The function has been used incorrectly, please report that the developer!");
	}
	// not a good idea, beause it would write twice in one array, and forget about the last one. (for example 4)
	// At least this warning will tell the people, that something is not right here!

	m_exit = false;
	m_cmdpulse = 0;
	m_joydeadzone = 1024;

	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);

	for(i=0 ; i<MAX_COMMANDS ; i++)
		InputCommand[player][i].active = false;

	// These are the default keyboard commands
	i=player-1;
	InputCommand[i][IC_LEFT].keysym = SDLK_LEFT;
	InputCommand[i][IC_UP].keysym = SDLK_UP;
	InputCommand[i][IC_RIGHT].keysym = SDLK_RIGHT;
	InputCommand[i][IC_DOWN].keysym = SDLK_DOWN;

	InputCommand[i][IC_UPPERLEFT].keysym = SDLK_HOME;
	InputCommand[i][IC_UPPERRIGHT].keysym = SDLK_PAGEUP;
	InputCommand[i][IC_LOWERLEFT].keysym = SDLK_END;
	InputCommand[i][IC_LOWERRIGHT].keysym = SDLK_PAGEDOWN;

	InputCommand[i][IC_JUMP].keysym = SDLK_LCTRL;
	InputCommand[i][IC_POGO].keysym = SDLK_LALT;
	InputCommand[i][IC_FIRE].keysym = SDLK_SPACE;
	InputCommand[i][IC_STATUS].keysym = SDLK_RETURN;

	InputCommand[i][IC_HELP].keysym = SDLK_F1;
	InputCommand[i][IC_QUIT].keysym = SDLK_ESCAPE;

	// And those are the default joystick handlings, but they are disabled by default
	InputCommand[i][IC_LEFT].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_LEFT].joyaxis = 0;
	InputCommand[i][IC_LEFT].joyvalue = -32767;
	InputCommand[i][IC_LEFT].which = 0;
	InputCommand[i][IC_UP].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_UP].joyaxis = 1;
	InputCommand[i][IC_UP].joyvalue = -32767;
	InputCommand[i][IC_UP].which = 0;
	InputCommand[i][IC_RIGHT].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_RIGHT].joyaxis = 0;
	InputCommand[i][IC_RIGHT].joyvalue = 32767;
	InputCommand[i][IC_RIGHT].which = 0;
	InputCommand[i][IC_DOWN].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_DOWN].joyaxis = 1;
	InputCommand[i][IC_DOWN].joyvalue = 32767;
	InputCommand[i][IC_DOWN].which = 0;

	InputCommand[i][IC_JUMP].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_JUMP].joybutton = 0;
	InputCommand[i][IC_JUMP].which = 0;
	InputCommand[i][IC_POGO].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_POGO].joybutton = 1;
	InputCommand[i][IC_POGO].which = 0;
	InputCommand[i][IC_FIRE].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_FIRE].joybutton = 2;
	InputCommand[i][IC_FIRE].which = 0;
	InputCommand[i][IC_STATUS].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_STATUS].joybutton = 3;
	InputCommand[i][IC_STATUS].which = 0;
	InputCommand[i][IC_HELP].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_HELP].joybutton = 4;
	InputCommand[i][IC_HELP].which = 0;
	InputCommand[i][IC_QUIT].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_QUIT].joybutton = 5;
	InputCommand[i][IC_QUIT].which = 0;

	#ifdef ANDROID
	// Joystick input is broken in main menu and dialogs, so disabled for now
	/*
	InputCommand[i][IC_LEFT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_RIGHT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_UP].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_DOWN].joyeventtype = ETYPE_JOYAXIS;
	*/
	#endif

	setTwoButtonFiring(i, false);
}

/**
 * \brief	This will start the joystick driver and search for all the controls attached
 * 			to your computer
 * \return	false, if the driver couldn't be started, else true
 */
bool CInput::startJoyDriver()
{
	g_pLogFile->textOut("JoyDrv_Start() : ");

	if (SDL_Init( SDL_INIT_JOYSTICK ) < 0)
	{
		g_pLogFile->ftextOut("JoyDrv_Start() : Couldn't initialize SDL: %s<br>", SDL_GetError());
		return 1;
	}
	else
	{
		int i=0;
		size_t joynum;
		joynum = SDL_NumJoysticks();
		if(joynum)
		{
			g_pLogFile->ftextOut("Detected %i joystick(s).<br>\n", joynum );
			g_pLogFile->textOut("The names of the joysticks are:<br>");

			for( i=0; i < SDL_NumJoysticks(); i++ )
				g_pLogFile->ftextOut("    %s<br>", SDL_JoystickName(i));

			SDL_JoystickEventState(SDL_ENABLE);

			for(size_t c=0 ; c<joynum ; c++)
			{
				SDL_Joystick *p_Joystick = SDL_JoystickOpen(c);
				mp_Joysticks.push_back(p_Joystick);
			}
		}
		else
		{
			g_pLogFile->ftextOut("No joysticks were found.<br>\n");
		}
	}

	return 0;
}

/**
 * \brief	This will load input settings that were saved previously by the user at past session.
 */
void CInput::loadControlconfig(void)
{
	CConfiguration Configuration(CONFIGFILENAME);
	if(Configuration.Parse())
	{
		std::string section;
		for(size_t i=0 ; i<NUM_INPUTS ; i++)
		{
			// setup input from proper string
			section = "input" + itoa(i);

			std::string value;
			Configuration.ReadString( section, "Left", value, "Key 276 (left)");
			setupInputCommand( InputCommand[i], IC_LEFT, value );
			Configuration.ReadString( section, "Up", value, "Key 273 (up)");
			setupInputCommand( InputCommand[i], IC_UP, value );
			Configuration.ReadString( section, "Right", value, "Key 275 (right)");
			setupInputCommand( InputCommand[i], IC_RIGHT, value );
			Configuration.ReadString( section, "Down", value, "Key 274 (down)");
			setupInputCommand( InputCommand[i], IC_DOWN, value );

			Configuration.ReadString( section, "Lower-Left", value, "Key 279 (end)");
			setupInputCommand( InputCommand[i], IC_LOWERLEFT, value );
			Configuration.ReadString( section, "Lower-Right", value, "Key 281 (page down)");
			setupInputCommand( InputCommand[i], IC_LOWERRIGHT, value );
			Configuration.ReadString( section, "Upper-Left", value, "Key 278 (home)");
			setupInputCommand( InputCommand[i], IC_UPPERLEFT, value );
			Configuration.ReadString( section, "Upper-Right", value, "Key 280 (page up)");
			setupInputCommand( InputCommand[i], IC_UPPERRIGHT, value );

			Configuration.ReadString( section, "Jump", value, "Key 306 (left ctrl)");
			setupInputCommand( InputCommand[i], IC_JUMP, value );
			Configuration.ReadString( section, "Pogo", value, "Key 308 (left alt)");
			setupInputCommand( InputCommand[i], IC_POGO, value );
			Configuration.ReadString( section, "Fire", value, "Key 32 (space)");
			setupInputCommand( InputCommand[i], IC_FIRE, value );
			Configuration.ReadString( section, "Status", value, "Key 13 (return)");
			setupInputCommand( InputCommand[i], IC_STATUS, value );
			Configuration.ReadString( section, "Help", value, "Key 282 (f1)");
			setupInputCommand( InputCommand[i], IC_HELP, value );
			Configuration.ReadString( section, "Quit", value, "Key 27 (escape)");
			setupInputCommand( InputCommand[i], IC_QUIT, value );
			Configuration.ReadKeyword( section, "TwoButtonFiring", &TwoButtonFiring[i], false);
		}
	}
	else
	{
		for(size_t c=1 ; c<= NUM_INPUTS ; c++)
			resetControls(c);
	}
}

/**
 * \brief	This will save input settings according to how the user did map the buttons,
 * 			axes or keys to the commands.
 */
void CInput::saveControlconfig()
{
	CConfiguration Configuration(CONFIGFILENAME);
	Configuration.Parse();

	std::string section;
	for(size_t i=0 ; i<NUM_INPUTS ; i++)
	{
		section = "input" + itoa(i);

		Configuration.WriteString(section, "Left", getEventName(IC_LEFT, i));
		Configuration.WriteString(section, "Up", getEventName(IC_UP, i));
		Configuration.WriteString(section, "Right", getEventName(IC_RIGHT, i));
		Configuration.WriteString(section, "Down", getEventName(IC_DOWN, i));
		Configuration.WriteString(section, "Upper-Left", getEventName(IC_UPPERLEFT, i));
		Configuration.WriteString(section, "Upper-Right", getEventName(IC_UPPERRIGHT, i));
		Configuration.WriteString(section, "Lower-Left", getEventName(IC_LOWERLEFT, i));
		Configuration.WriteString(section, "Lower-Right", getEventName(IC_LOWERRIGHT, i));
		Configuration.WriteString(section, "Jump", getEventName(IC_JUMP, i));
		Configuration.WriteString(section, "Pogo", getEventName(IC_POGO, i));
		Configuration.WriteString(section, "Fire", getEventName(IC_FIRE, i));
		Configuration.WriteString(section, "Status", getEventName(IC_STATUS, i));
		Configuration.WriteString(section, "Help", getEventName(IC_HELP, i));
		Configuration.WriteString(section, "Quit", getEventName(IC_QUIT, i));
		Configuration.SetKeyword(section, "TwoButtonFiring", TwoButtonFiring[i]);
	}
	Configuration.saveCfgFile();
}

/**
 * \brief	This checks what event has been assigned to the chosen command and builds a string calling it
 * 			a standardized way.
 * \param	command		command where you are looking for the event
 * \param	input		number of input chosen. it's normal the number of the player
 * \return 	returns the assigned event as a std::string
 */

std::string CInput::getEventName(int command, unsigned char input)
{
	std::string buf;
	if(InputCommand[input][command].joyeventtype == ETYPE_JOYAXIS)
	{
		buf = "Joy" + itoa(InputCommand[input][command].which) + "-A" + itoa(InputCommand[input][command].joyaxis);
		if(InputCommand[input][command].joyvalue < 0)
			buf += "-";
		else
			buf += "+";
	}
	else if(InputCommand[input][command].joyeventtype == ETYPE_JOYBUTTON)
	{
		buf = "Joy" + itoa(InputCommand[input][command].which) + "-B" + itoa(InputCommand[input][command].joybutton);
	}
	else // In case only keyboard was triggered
	{
		buf = "Key ";
		buf += itoa(InputCommand[input][command].keysym);
		buf += " (";
		buf += SDL_GetKeyName(InputCommand[input][command].keysym);
		buf += ")";
	}

	return buf;
}

void CInput::setupInputCommand( stInputCommand *pInput, int action, const std::string &string )
{
	std::string buf = string;

	TrimSpaces(buf);
	if(buf == "") return;

	if(strCaseStartsWith(string, "Joy"))
	{
		std::string buf2;
		buf = buf.substr(3);
		size_t pos = buf.find('-');
		buf2 = buf.substr(0, pos);
		pInput[action].which = atoi(buf2);
		buf = buf.substr(pos+1);
		buf2 = buf.substr(0,1);
		buf = buf.substr(1);

		if(buf2 == "A")
		{
			pInput[action].joyeventtype = ETYPE_JOYAXIS;
			pos = buf.size()-1;
			buf2 = buf.substr(0,pos);
			pInput[action].joyaxis = atoi(buf2);
			buf2 = buf.substr(pos);
			pInput[action].joyvalue = (buf2 == "+") ? +1 : -1;
		}
		else // Should normally be B
		{
			pInput[action].joyeventtype = ETYPE_JOYBUTTON;
			pInput[action].joybutton = atoi(buf);
		}
		return;
	}

	if(strCaseStartsWith(string, "Key"))
	{
		pInput[action].joyeventtype = ETYPE_KEYBOARD;
		buf = buf.substr(3);
		TrimSpaces(buf);
		pInput[action].keysym = (SDLKey) atoi(buf);
		return;
	}
}

/**
 * \brief	This checks if some event was triggered to get the new input command
 * \param	device		input of which we are trying to read the event
 * \param	command		command for which we want to assign the event
 * \return 	returns true, if an event was triggered, or false if not.
 */
bool CInput::readNewEvent(Uint8 device, int command)
{
	stInputCommand &lokalInput = InputCommand[device][command];

	// This function is used to configure new input keys.
	// For iPhone, we have emulation via touchpad and we don't want to have custom keys.
	// We should fix the menu for iPhone so that this function doesn't get called.
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	printf("WARNING: called readNewEvent on iphone\n");
	return true;
#endif

	memset(&lokalInput, 0, sizeof(stInputCommand));

	while( SDL_PollEvent( &Event ) )
	{
		switch ( Event.type )
		{
			case SDL_QUIT:
				g_pLogFile->textOut("SDL: Got quit event in readNewEvent!");
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
				// on iPhone, we just want to quit in this case
				exit(0);
#endif
				break;
			case SDL_KEYDOWN:
				lokalInput.joyeventtype = ETYPE_KEYBOARD;
				lokalInput.keysym = Event.key.keysym.sym;
				return true;
				break;
			case SDL_JOYBUTTONDOWN:
#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
				WIZ_EmuKeyboard( Event.jbutton.button, 1 );
				return false;
#else
				lokalInput.joyeventtype = ETYPE_JOYBUTTON;
				lokalInput.joybutton = Event.jbutton.button;
				lokalInput.which = Event.jbutton.which;
				return true;
#endif
				break;
			case SDL_JOYAXISMOTION:
				lokalInput.joyeventtype = ETYPE_JOYAXIS;
				lokalInput.joyaxis = Event.jaxis.axis;
				lokalInput.which = Event.jaxis.which;
				lokalInput.joyvalue = (Event.jaxis.value>0) ? 32767 : -32767;
				return true;
				break;
		}
	}
	return false;
}

bool CInput::getExitEvent(void) {	return m_exit;	}
void CInput::cancelExitEvent(void) { m_exit=false; }

bool CInput::getTwoButtonFiring(int player) { return TwoButtonFiring[player]; }
void CInput::setTwoButtonFiring(int player, bool value) { TwoButtonFiring[player]=value; }

/**
 * \brief	Called every logic cycle. This triggers the events that occur and process them trough various functions
 */
void CInput::pollEvents()
{
	// copy all the input of the last poll to a space for checking pressing or holding a button
	memcpy(last_immediate_keytable, immediate_keytable, KEYTABLE_SIZE*sizeof(char));

	for(int i=0 ; i<MAX_COMMANDS ; i++)
		for(int j=0 ; j<NUM_INPUTS ; j++)
			InputCommand[j][i].lastactive = InputCommand[j][i].active;

	// While there's an event to handle
	while( SDL_PollEvent( &Event ) )
	{
		switch( Event.type )
		{
		case SDL_QUIT:
			g_pLogFile->textOut("SDL: Got quit event!");
			m_exit = true;
			break;
		case SDL_KEYDOWN:
			processKeys(1);
			break;
		case SDL_KEYUP:
			processKeys(0);
			break;
		case SDL_JOYAXISMOTION:
			processJoystickAxis();
			break;
		case SDL_JOYBUTTONDOWN:
			processJoystickButton(1);
			break;
		case SDL_JOYBUTTONUP:
			processJoystickButton(0);
			break;
#ifdef MOUSEWRAPPER
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			processMouse(Event);
			break;
#endif

		case SDL_VIDEORESIZE:
			CRect<Uint16> newSize(Event.resize.w, Event.resize.h);
			g_pVideoDriver->mp_VideoEngine->resizeDisplayScreen(newSize);
			break;
		}
	}
#ifdef MOUSEWRAPPER
	// Handle mouse emulation layer
	processMouse();
#endif

	for(unsigned int i = 0; i < KEYTABLE_SIZE; ++i)
		firsttime_immediate_keytable[i]
		= !last_immediate_keytable[i] && immediate_keytable[i];

	for(int i=0 ; i<MAX_COMMANDS ; i++)
		for(int j=0 ; j<NUM_INPUTS ; j++)
			InputCommand[j][i].firsttimeactive
			= !InputCommand[j][i].lastactive && InputCommand[j][i].active;

#ifndef MOUSEWRAPPER

	// TODO: I'm not sure, if that should go here...
	// Check, if LALT+ENTER was pressed
	if((getHoldedKey(KALT)) && getPressedKey(KENTER))
	{
		bool value;
		value = g_pVideoDriver->getFullscreen();
		value = !value;
		g_pLogFile->textOut(GREEN,"Fullscreen mode triggered by user!<br>");
		g_pVideoDriver->isFullscreen(value);

		// initialize/activate all drivers
		g_pVideoDriver->stop();
		g_pLogFile->ftextOut("Restarting graphics driver...<br>");
		if ( g_pVideoDriver->applyMode() && g_pVideoDriver->start() )
		{
			g_pLogFile->ftextOut(PURPLE, "Toggled Fullscreen quick shortcut...<br>");
		}
		else
		{
			value = !value;
			g_pLogFile->ftextOut(PURPLE, "Couldn't change the resolution, Rolling back...<br>");
			g_pVideoDriver->applyMode();
			g_pVideoDriver->start();
		}

		if(value) g_pVideoDriver->AddConsoleMsg("Fullscreen enabled");
		else g_pVideoDriver->AddConsoleMsg("Fullscreen disabled");

		g_pInput->flushAll();
	}

	// Check, if LALT+Q or LALT+F4 was pressed
	if(getHoldedKey(KALT) && (getPressedKey(KF4) || getPressedKey(KQ)) )
	{
		g_pLogFile->textOut("User exit request!");
		m_exit = true;
	}
#endif

#if defined(WIZ) || defined(GP2X)
	WIZ_AdjustVolume( volume_direction );
#endif

    // Fix up settings if everything gets messed up
	if (g_pInput->getHoldedKey(KF) &&
		g_pInput->getHoldedKey(KI) &&
		g_pInput->getHoldedKey(KX))
	{
		g_pSettings->loadDefaultGraphicsCfg();
		g_pSettings->saveDrvCfg();
		g_pVideoDriver->stop();
		g_pVideoDriver->start();
	}
}

/**
 * \brief	This will tell if any joystick axes haven been moved and if they triggered a command by doing so...
 */
void CInput::processJoystickAxis(void)
{
	for(int j=0 ; j<NUM_INPUTS ; j++)
	{
		for(int i=0 ; i<MAX_COMMANDS ; i++)
		{
			if(InputCommand[j][i].joyeventtype == ETYPE_JOYAXIS)
			{
				// Axis are configured for this commmand
				if(Event.jaxis.axis == InputCommand[j][i].joyaxis && Event.jaxis.which == InputCommand[j][i].which )
				{
					// Deadzone
					if((Event.jaxis.value > m_joydeadzone && InputCommand[0][i].joyvalue > 0) ||
					   (Event.jaxis.value < -m_joydeadzone && InputCommand[0][i].joyvalue < 0))
					{
						InputCommand[j][i].active = true;
						InputCommand[j][i].joymotion = Event.jaxis.value;
					}
					else
						InputCommand[j][i].active = false;
				}
			}
		}
	}
}

/**
 * \brief	This will tell if any joystick button has been pressed and if they triggered a command by doing so...
 */
void CInput::processJoystickButton(int value)
{
#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
	WIZ_EmuKeyboard( Event.jbutton.button, value );
#else
	for(int j=0 ; j<NUM_INPUTS ; j++)
	{
		for(int i=0 ; i<MAX_COMMANDS ; i++)
		{
			// TODO: Check all NUM_INPUTS, if they can be reduced to another variable
			if(InputCommand[j][i].joyeventtype == ETYPE_JOYBUTTON)
			{
				// Joystick buttons are configured for this event !!
				if(Event.jbutton.button == InputCommand[j][i].joybutton && Event.jbutton.which == InputCommand[j][i].which )
					InputCommand[j][i].active = value;
			}
		}
	}
#endif
}

/**
 * Sends a key to the Commander Genius engine
 */
void CInput::sendKey(int key){	immediate_keytable[key] = true;	}

/**
 * \brief	This will tell if any key was pressed and it fits the to command array, so we can tell, the command
 * 			was triggered.
 * \param	keydown	this parameter tells if the keys is down or has already been released.
 */
void CInput::processKeys(int keydown)
{
	// Input for player commands
	for(int i=0 ; i<MAX_COMMANDS ; i++)
	{
		for(int j=0 ; j<NUM_INPUTS ; j++)
		{
			if(InputCommand[j][i].keysym == Event.key.keysym.sym &&
					InputCommand[j][i].joyeventtype == ETYPE_KEYBOARD)
				InputCommand[j][i].active = (keydown) ? true : false;
		}
	}

	// ... and for general keys
    switch(Event.key.keysym.sym)
	{
			// These are only used for ingame stuff or menu, but not for controlling the player anymore
		case SDLK_LEFT: 	immediate_keytable[KLEFT]	= keydown;  break;
		case SDLK_UP:	immediate_keytable[KUP]		= keydown;  break;
		case SDLK_RIGHT:	immediate_keytable[KRIGHT]	= keydown;  break;
		case SDLK_DOWN:	immediate_keytable[KDOWN]	= keydown;  break;

			// Page Keys
		case SDLK_PAGEUP:	immediate_keytable[KPGUP]	= keydown;  break;
		case SDLK_PAGEDOWN:	immediate_keytable[KPGDN]		= keydown;  break;

		case SDLK_RETURN:immediate_keytable[KENTER]	= keydown;  break;
		case SDLK_RCTRL:immediate_keytable[KCTRL]	= keydown;  break;
		case SDLK_LCTRL:immediate_keytable[KCTRL]	= keydown;  break;
		case SDLK_SPACE:immediate_keytable[KSPACE]	= keydown;  break;
		case SDLK_RALT:immediate_keytable[KALT]		= keydown;  break;
		case SDLK_LALT:immediate_keytable[KALT]		= keydown;  break;
		case SDLK_TAB:immediate_keytable[KTAB]		= keydown;  break;
		case SDLK_LSHIFT:immediate_keytable[KSHIFT]	= keydown;  break;
		case SDLK_RSHIFT:immediate_keytable[KSHIFT]	= keydown;  break;
		case SDLK_ESCAPE:immediate_keytable[KQUIT]	= keydown;  break;

		case SDLK_BACKSPACE:immediate_keytable[KBCKSPCE] = keydown; break;

		case SDLK_QUOTE:immediate_keytable[KQUOTE]	= keydown;  break;
		case SDLK_COMMA:immediate_keytable[KCOMMA]	= keydown;  break;
		case SDLK_PERIOD:immediate_keytable[KPERIOD]	= keydown;  break;
		case SDLK_SLASH:immediate_keytable[KSLASH]	= keydown;  break;
		case SDLK_SEMICOLON:immediate_keytable[KSEMICOLON]	= keydown;  break;
		case SDLK_EQUALS:immediate_keytable[KEQUAL]	= keydown;  break;
		case SDLK_LEFTBRACKET:immediate_keytable[KLEFTBRACKET]	= keydown;  break;
		case SDLK_BACKSLASH:immediate_keytable[KBACKSLASH]	= keydown;  break;
		case SDLK_RIGHTBRACKET:immediate_keytable[KRIGHTBRACKET]	= keydown;  break;
		case SDLK_BACKQUOTE:immediate_keytable[KBACKQUOTE]	= keydown;  break;

		case SDLK_a:immediate_keytable[KA]	= keydown;  break;
		case SDLK_b:immediate_keytable[KB]	= keydown;  break;
		case SDLK_c:immediate_keytable[KC]	= keydown;  break;
		case SDLK_d:immediate_keytable[KD]	= keydown;  break;
		case SDLK_e:immediate_keytable[KE]	= keydown;  break;
		case SDLK_f:immediate_keytable[KF]	= keydown;  break;
		case SDLK_g:immediate_keytable[KG]	= keydown;  break;
		case SDLK_h:immediate_keytable[KH]	= keydown;  break;
		case SDLK_i:immediate_keytable[KI]	= keydown;  break;
		case SDLK_j:immediate_keytable[KJ]	= keydown;  break;
		case SDLK_k:immediate_keytable[KK]	= keydown;  break;
		case SDLK_l:immediate_keytable[KL]	= keydown;  break;
		case SDLK_m:immediate_keytable[KM]	= keydown;  break;
		case SDLK_n:immediate_keytable[KN]	= keydown;  break;
		case SDLK_o:immediate_keytable[KO]	= keydown;  break;
		case SDLK_p:immediate_keytable[KP]	= keydown;  break;
		case SDLK_q:immediate_keytable[KQ]	= keydown;  break;
		case SDLK_r:immediate_keytable[KR]	= keydown;  break;
		case SDLK_s:immediate_keytable[KS]	= keydown;  break;
		case SDLK_t:immediate_keytable[KT]	= keydown;  break;
		case SDLK_u:immediate_keytable[KU]	= keydown;  break;
		case SDLK_v:immediate_keytable[KV]	= keydown;  break;
		case SDLK_w:immediate_keytable[KW]	= keydown;  break;
		case SDLK_x:immediate_keytable[KX]	= keydown;  break;
		case SDLK_y:immediate_keytable[KY]	= keydown;  break;
		case SDLK_z:immediate_keytable[KZ]	= keydown;  break;

		case SDLK_F1:immediate_keytable[KF1]	= keydown;  break;
		case SDLK_F2:immediate_keytable[KF2]	= keydown;  break;
		case SDLK_F3:immediate_keytable[KF3]	= keydown;  break;
		case SDLK_F4:immediate_keytable[KF4]	= keydown;  break;
		case SDLK_F5:immediate_keytable[KF5]	= keydown;  break;
		case SDLK_F6:immediate_keytable[KF6]	= keydown;  break;
		case SDLK_F7:immediate_keytable[KF7]	= keydown;  break;
		case SDLK_F8:immediate_keytable[KF8]	= keydown;  break;
		case SDLK_F9:immediate_keytable[KF9]	= keydown;  break;
		case SDLK_F10:immediate_keytable[KF10]	= keydown;  break;

		case SDLK_0:immediate_keytable[KNUM0] = keydown;  break;
		case SDLK_1:immediate_keytable[KNUM1] = keydown;  break;
		case SDLK_2:immediate_keytable[KNUM2] = keydown;  break;
		case SDLK_3:immediate_keytable[KNUM3] = keydown;  break;
		case SDLK_4:immediate_keytable[KNUM4] = keydown;  break;
		case SDLK_5:immediate_keytable[KNUM5] = keydown;  break;
		case SDLK_6:immediate_keytable[KNUM6] = keydown;  break;
		case SDLK_7:immediate_keytable[KNUM7] = keydown;  break;
		case SDLK_8:immediate_keytable[KNUM8] = keydown;  break;
		case SDLK_9:immediate_keytable[KNUM9] = keydown;  break;

		case SDLK_EXCLAIM:immediate_keytable[KEXCLAIM]	= keydown;  break;
		case SDLK_QUOTEDBL:immediate_keytable[KDBLQUOTE]	= keydown;  break;
		case SDLK_HASH:immediate_keytable[KHASH]	= keydown;  break;
		case SDLK_DOLLAR:immediate_keytable[KDOLLAR]	= keydown;  break;
		case SDLK_AMPERSAND:immediate_keytable[KAMPERSAND]	= keydown;  break;
		case SDLK_ASTERISK:immediate_keytable[KAST]	= keydown;  break;
		case SDLK_LEFTPAREN:immediate_keytable[KLEFTPAREN]	= keydown;  break;
		case SDLK_RIGHTPAREN:immediate_keytable[KRIGHTPAREN]	= keydown;  break;
		case SDLK_COLON:immediate_keytable[KCOLON]	= keydown;  break;
		case SDLK_LESS:immediate_keytable[KLESS]	= keydown;  break;
		case SDLK_GREATER:immediate_keytable[KGREATER]	= keydown;  break;
		case SDLK_QUESTION:immediate_keytable[KQUESTION]	= keydown;  break;
		case SDLK_AT:immediate_keytable[KAT]	= keydown;  break;
		case SDLK_CARET:immediate_keytable[KCARET]	= keydown;  break;
		case SDLK_UNDERSCORE:immediate_keytable[KUNDERSCORE]	= keydown;  break;
		case SDLK_MINUS:immediate_keytable[KMINUS]	= keydown;  break;
		case SDLK_PLUS:immediate_keytable[KPLUS]	= keydown;  break;

		default: break;
	}

	if(getHoldedKey(KSHIFT))
	{
		if(getPressedKey(KBACKQUOTE)) immediate_keytable[KTILDE] = keydown;
		if(getPressedKey(KNUM1)) immediate_keytable[KEXCLAIM] = keydown;
		if(getPressedKey(KNUM2)) immediate_keytable[KAT] = keydown;
		if(getPressedKey(KNUM3)) immediate_keytable[KHASH] = keydown;
		if(getPressedKey(KNUM4)) immediate_keytable[KDOLLAR] = keydown;
		if(getPressedKey(KNUM5)) immediate_keytable[KPERCENT] = keydown;
		if(getPressedKey(KNUM6)) immediate_keytable[KCARET] = keydown;
		if(getPressedKey(KNUM7)) immediate_keytable[KAMPERSAND] = keydown;
		if(getPressedKey(KNUM8)) immediate_keytable[KAST] = keydown;
		if(getPressedKey(KNUM9)) immediate_keytable[KLEFTPAREN] = keydown;
		if(getPressedKey(KNUM0)) immediate_keytable[KRIGHTPAREN] = keydown;
		if(getPressedKey(KMINUS)) immediate_keytable[KUNDERSCORE] = keydown;
		if(getPressedKey(KEQUAL)) immediate_keytable[KPLUS] = keydown;
		if(getPressedKey(KBACKSLASH)) immediate_keytable[KLINE] = keydown;
		if(getPressedKey(KLEFTBRACKET)) immediate_keytable[KLEFTBRACE] = keydown;
		if(getPressedKey(KRIGHTBRACKET)) immediate_keytable[KRIGHTBRACE] = keydown;
		if(getPressedKey(KSEMICOLON)) immediate_keytable[KCOLON] = keydown;
		if(getPressedKey(KQUOTE)) immediate_keytable[KDBLQUOTE] = keydown;
		if(getPressedKey(KCOMMA)) immediate_keytable[KLESS] = keydown;
		if(getPressedKey(KPERIOD)) immediate_keytable[KGREATER] = keydown;
		if(getPressedKey(KSLASH)) immediate_keytable[KQUESTION] = keydown;
	}
}

#ifdef MOUSEWRAPPER
static bool checkMousewrapperKey(int& key);
#endif
/**
 * \brief	returns if certain key is being held
 * \param	key the key to be held
 */
bool CInput::getHoldedKey(int key)
{
#ifdef MOUSEWRAPPER
	if(!checkMousewrapperKey(key)) return true;
#endif
	if(immediate_keytable[key])
		return true;

	return false;
}

/**
 * \brief	returns if certain key is being pressed
 * \param	key	the key to be pressed
 */
bool CInput::getPressedKey(int key)
{
#ifdef MOUSEWRAPPER
	if(!checkMousewrapperKey(key)) return true;
#endif
	if(firsttime_immediate_keytable[key])
	{
		firsttime_immediate_keytable[key] = false;
		return true;
	}

	return false;
}

/**
 * \brief	returns if certain key is being held for msecs
 * \param	key the key to be pressed
 * \param	msec Milliseconds the key can be held before it is a pulse
 */

bool CInput::getPulsedKey(int key, int msec)
{
	if(immediate_keytable[key])
	{
		bool value = true;

		if(m_cmdpulse % msec != 0)
		{
			value = false;
		}
		m_cmdpulse++;
		return value;
	}
	if(!immediate_keytable[key] && last_immediate_keytable[key])
		m_cmdpulse = 0;

	return false;
}

/**
 * \brief	normally called, when a save game or high score entry is being made. It will return a charakter that was typed.
 * \return	character as std::string, which was entered
 */
std::string CInput::getPressedTypingKey(void)
{
	int i;
	std::string buf;

	for(i=KA ; i<=KZ ; i++)
	{
		if (getHoldedKey(KSHIFT) && getPressedKey(i))
		{
			buf = 'A' + i - KA;
			return buf;
		}
		else if(getPressedKey(i))
		{
			buf = 'a' + i - KA;
			return buf;
		}
	}
	for(i=KSPACE ; i<=KAT ; i++)
	{
		if(getPressedKey(i))
		{
			buf = 32 + i - KSPACE;
			return buf;
		}
	}
		for(i=KLEFTBRACKET ; i<=KBACKQUOTE ; i++)
	{
		if(getPressedKey(i))
		{
			buf = '[' + i - KLEFTBRACKET;
			return buf;
		}
	}
		for(i=KLEFTBRACE ; i<=KTILDE ; i++)
	{
		if(getPressedKey(i))
		{
			buf = '{' + i - KLEFTBRACE;
			return buf;
		}
	}
	return buf;
}

/**
 * \brief	returns if a numerical key was pressed
 * \return	number as std::string, which was entered
 */
std::string CInput::getPressedNumKey(void)
{
	int i;
	std::string buf;

	for(i=KNUM0 ; i<=KNUM9 ; i++)
	{
		if(getPressedKey(i))
		{
			buf = '0' + i - KNUM0;
			return buf;
		}
	}
	return buf;
}

/**
 * \brief	tells if the pressed key was for typing
 * \return	true if yes, and false for no.
 */
bool CInput::getPressedIsTypingKey(void)
{
	int i;

	if(getHoldedKey(KSHIFT))
		return true;
	else
	{
		for(i=KSPACE ; i<=KAT ; i++)
		{
			if(getHoldedKey(i))
				return true;
		}
		for(i=KLEFTBRACKET ; i<=KTILDE ; i++)
		{
			if(getHoldedKey(i))
				return true;
		}
		return false;
	}
}

bool CInput::getPressedIsNumKey(void)
{
	int i;

		for(i=KNUM0 ; i<=KNUM9 ; i++)
		{
			if(getHoldedKey(i))
				return true;
		}
		return false;
}

bool CInput::getPressedAnyKey(void)
{
	for(unsigned int key=0 ; key<KEYTABLE_SIZE ; key++)
	{
		if(firsttime_immediate_keytable[key])
		{
			firsttime_immediate_keytable[key] = false;
			return true;
		}
	}
	return false;
}

bool CInput::getHoldedCommand(int command)
{
	bool retval = false;
	for( int player=0; player<NUM_INPUTS ; player++ )
		retval |= getHoldedCommand(player, command);
	return retval;
}

bool CInput::isJoystickAssgmnt(const int player, const int command)
{
	return (InputCommand[player][command].joyeventtype == ETYPE_JOYAXIS);
}

bool CInput::getHoldedCommand(int player, int command)
{
	return InputCommand[player][command].active;
}

int CInput::getJoyValue(int player, int command)
{
	int newval = InputCommand[player][command].joymotion;
	newval = (newval*101)>>15;
	if( newval > 100 )
		newval = 100;
	if( newval < -100 )
		newval = -100;
	return newval;
}


bool CInput::getPressedCommand(int command)
{
	bool retval = false;
	for(int player=0; player<NUM_INPUTS ; player++ )
		retval |= getPressedCommand(player, command);
	return retval;
}

bool CInput::getPressedCommand(int player, int command)
{
	if(InputCommand[player][command].firsttimeactive)
	{
		InputCommand[player][command].firsttimeactive = false;
		return true;
	}

	return false;
}

bool CInput::getPulsedCommand(int command, int msec)
{
	bool retval = false;
	for(int player=0; player<NUM_INPUTS ; player++ )
		retval |= getPulsedCommand(player, command, msec);
	return retval;
}

bool CInput::getPulsedCommand(int player, int command, int msec)
{
	if(InputCommand[player][command].active)
	{
		bool value = true;

		if(m_cmdpulse % msec != 0)
		{
			value = false;
		}
		m_cmdpulse++;
		return value;
	}
	if(!InputCommand[player][command].active && InputCommand[player][command].lastactive )
		m_cmdpulse = 0;

	return false;
}

bool CInput::getPressedAnyCommand()
{
	bool retval = false;
	for(int player=0 ; player<NUM_INPUTS ; player++)
		retval |= getPressedAnyCommand(player);
	return retval;
}

bool CInput::getPressedAnyCommand(int player)
{
	for(int i=0 ; i<MAX_COMMANDS ; i++)
		if(getPressedCommand(player,i))
			return true;

	return false;
}

/**
 * \brief	This will forget every command that was triggered
 */
void CInput::flushCommands(void)
{
	for(int i=0 ; i<NUM_INPUTS ; i++)
		for(int j=0 ; j<MAX_COMMANDS ; j++)
			flushCommand( i, j );
}

void CInput::flushCommand(int command)
{
	for( int player=0; player<NUM_INPUTS ; player++ )
		flushCommand(player, command);
}

void CInput::flushCommand(int player, int command)
{
	InputCommand[player][command].active =
	InputCommand[player][command].lastactive =
	InputCommand[player][command].firsttimeactive = false;
}

/**
 * \brief	this will forget every key that was typed before
 */
void CInput::flushKeys(void)
{
	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);
	memset(firsttime_immediate_keytable,false,KEYTABLE_SIZE);
}

struct TouchButton {
	stInputCommand* cmd;
	int immediateIndex;
	int x, y, w, h;

	bool isInside(int _x, int _y) const {
		return
		x <= _x && _x < x + w &&
		y <= _y && _y < y + h;
	}
};

static const int w = 320, h = 200;

#define KSHOWHIDECTRLS	(-10)

#if defined(MOUSEWRAPPER)
static TouchButton* getPhoneButtons(stInputCommand InputCommand[NUM_INPUTS][MAX_COMMANDS]) {
	static const int middlex = w / 2;
	static const int middley = h / 2;

	static TouchButton phoneButtons[] = {
		{ &InputCommand[0][IC_LEFT],	KLEFT,	0, middley, w / 6, h / 2},
		{ &InputCommand[0][IC_UP],		KUP,	w / 6, middley, w / 6, h / 4},
		{ &InputCommand[0][IC_RIGHT],	KRIGHT,	w / 3, middley, w / 6, h / 2},
		{ &InputCommand[0][IC_DOWN],	KDOWN,	w / 6, middley + h / 4, w / 6, h / 4},

		{ &InputCommand[0][IC_JUMP],	-1,		middlex, middley, w / 6, h / 2},
		{ &InputCommand[0][IC_POGO],	-1,		middlex + w / 6, middley, w / 6, h / 2},
		{ &InputCommand[0][IC_FIRE],	KSPACE,	middlex + w / 3, middley, w / 6, h / 2},

		{ &InputCommand[0][IC_STATUS],	KENTER,	0, 0, w/2, h/4},
		{ &InputCommand[0][IC_QUIT],	KQUIT,	5*w/6, 0, w/6, h/6},
		{ NULL,							KSHOWHIDECTRLS,	4*w/6, 0, w/6, h/6},
	//	{ NULL,							KF3 /* save dialog, see gamedo_HandleFKeys */, 3*w/6, 0, w/6, h/6},
	};

	return phoneButtons;
}



static const int phoneButtonN = 10;
typedef std::set<int> MouseIndexSet;

static Uint32 phoneButtonLasttime[phoneButtonN] = {0,0,0,0,0,0,0,0,0,0};
static MouseIndexSet phoneButton_MouseIndex[phoneButtonN];


static TouchButton* getPhoneButton(int x, int y, TouchButton phoneButtons[]) {
	for(int i = 0; i < phoneButtonN; ++i) {
		TouchButton& b = phoneButtons[i];
		if(b.isInside(x, y)) return &b;
	}
	return NULL;
}


static bool checkMousewrapperKey(int& key) {
	switch(key) {
		case KLEFT: case KRIGHT: case KUP: case KDOWN:
		case KENTER: case KSPACE: case KQUIT: case KF3:
			return true;
	}

	if(key == KY) { key = KENTER; return true; }
	if(key == KN) { key = KQUIT; return true; }

	//errors << "checkMousewrapperKey: key " << key << " not useable for iPhone" << endl;
	//return false;
	// just too many keys ...
	return true;
}

void CInput::processMouse() {
	TouchButton* phoneButtons = getPhoneButtons(InputCommand);

	for(int i = 0; i < phoneButtonN; ++i) {
		bool down = phoneButton_MouseIndex[i].size() > 0;

		TouchButton& b = phoneButtons[i];

		if(b.cmd)
			b.cmd->active = down;

		// handle immediate keys
		if(b.immediateIndex >= 0)
			immediate_keytable[b.immediateIndex] = down;
	}
}

void CInput::processMouse(SDL_Event& ev) {

#if SDL_VERSION_ATLEAST(1, 3, 0)
	SDL_Rect screenRect;

	if(SDL_GetDisplayBounds(0, &screenRect) == 0) {
		// transform mouse coordinates
		// WARNING: I don't really understand that. It's probably somehow iPhoneRotateScreen + SDL stuff.
		ev.button.y -= screenRect.h - 200;
	}
#endif

	// NOTE: The ev.button.which / the multitouch support was removed in SDL 1.3 trunk
	// with changeset 4465:3e69e077cb95 on May09. It is planned to add a real multitouch API
	// at some later time (maybe Aug2010).
	// As long as we don't have that, we must use the old SDL 1.3 revision 4464.

	switch(ev.type) {
		case SDL_MOUSEBUTTONDOWN:
			processMouse(ev.button.x, ev.button.y, true, ev.button.which);
			break;

		case SDL_MOUSEBUTTONUP:
			processMouse(ev.button.x, ev.button.y, false, ev.button.which);
			break;

		case SDL_MOUSEMOTION:
			processMouse(ev.motion.x - ev.motion.xrel, ev.motion.y - ev.motion.yrel, false, ev.motion.which);
			processMouse(ev.motion.x, ev.motion.y, true, ev.motion.which);
			break;
	}
}

void CInput::processMouse(int x, int y, bool down, int mouseindex) {
	TouchButton* phoneButtons = getPhoneButtons(InputCommand);

	for(int i = 0; i < phoneButtonN; ++i) {
		TouchButton& b = phoneButtons[i];
		if(b.isInside(x, y)) {
			phoneButtonLasttime[i] = down ? SDL_GetTicks() : 0;
			if(down)	phoneButton_MouseIndex[i].insert(mouseindex);
			else		phoneButton_MouseIndex[i].erase(mouseindex);

			break;
		}
	}

}

#ifdef USE_OPENGL
static void drawButton(TouchButton& button, bool down) {
	// similar mysterious constant as in renderTexture/initGL
	//glViewport(0,255,w,h);

	float w = 512.0f, h = 256.0f;

	int crop = 2;
	float x1 = float(button.x + crop) / w;
	float x2 = float(button.x+button.w - crop) / w;
	float y1 = float(button.y + crop) / h;
	float y2 = float(button.y+button.h - crop) / h;

	GLfloat vertices[] =
	{
		x1, y1,
		x2, y1,
		x2, y2,
		x1, y2,
	};

	//Render the vertices by pointing to the arrays.
    glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glEnable(GL_BLEND);
	if(down)
		glColor4f(0,0,0, 0.5);
	else
		glColor4f(0,0,0, 0.2);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ZERO);

	//Finally draw the arrays.
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_BLEND);

}
#endif

#endif

void CInput::renderOverlay() {
#ifdef USE_OPENGL // only ogl supported yet (and probably worth)
#if defined(MOUSEWRAPPER)
	static bool showControls = true;
	static bool buttonShowHideCtrlWasDown = false;

	TouchButton* phoneButtons = getPhoneButtons(InputCommand);

	for(int i = phoneButtonN - 1; i >= 0; --i) {
		TouchButton& b = phoneButtons[i];
		bool down = phoneButton_MouseIndex[i].size() > 0;
		if(showControls) drawButton(b, down);

		if(b.immediateIndex == KSHOWHIDECTRLS) {
			if(buttonShowHideCtrlWasDown && !down)
				showControls = !showControls;
			buttonShowHideCtrlWasDown = down;
		}
	}
#endif
#endif
}

/**
 * \brief	flushes both key and commands queue
 */
void CInput::flushAll(void){ flushKeys(); flushCommands(); }

/**
 * \brief	shuts down the input driver.
 */
CInput::~CInput() {
	// Shutdown Joysticks
	while(!mp_Joysticks.empty())
	{
		SDL_JoystickClose(mp_Joysticks.back());
		mp_Joysticks.pop_back();
	}
}


