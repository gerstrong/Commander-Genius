/*
 * CInput.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>
#include <stdio.h>

#include <base/InputEvents.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <base/PointDevice.h>
#include <fileio/CConfiguration.h>

// Input Events

bool pollLocked = false;
SDL_sem *pollSem = nullptr;


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
	gLogging.ftextOut("Starting the input driver...<br>");
	memset(InputCommand, 0, NUM_INPUTS*MAX_COMMANDS*sizeof(stInputCommand));

	for(size_t c=1 ; c<= NUM_INPUTS ; c++)
		resetControls(c);
	memset(&Event,0,sizeof(Event));
#if !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
	loadControlconfig(); // we want to have the default settings in all cases
	startJoyDriver(); // not for iPhone for now, could cause trouble (unwanted input events)
#endif


     //Create the semaphor
    pollSem = SDL_CreateSemaphore(1);
}

/**
 * \brief This will reset the player controls how they saved before.
 * 		  The Default controls are going to be restored when this function
 * 		  is executed
 * \param	player	Number of player of which the controls will be reset (1-4)
 */
void CInput::resetControls(int player)
{
	int i;

	if(player == 0)
	{
		player = 1;
		gLogging.textOut("Warning when resetting controls. The function has been used incorrectly, please report that the developer!");
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

	InputCommand[i][IC_CAMLEAD].keysym = SDLK_c;
	InputCommand[i][IC_HELP].keysym = SDLK_F1;
	InputCommand[i][IC_BACK].keysym = SDLK_ESCAPE;

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
	InputCommand[i][IC_CAMLEAD].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_CAMLEAD].joybutton = 4;
	InputCommand[i][IC_CAMLEAD].which = 0;
	InputCommand[i][IC_HELP].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_HELP].joybutton = 5;
	InputCommand[i][IC_HELP].which = 0;
	InputCommand[i][IC_BACK].joyeventtype = ETYPE_KEYBOARD;
	InputCommand[i][IC_BACK].joybutton = 6;
	InputCommand[i][IC_BACK].which = 0;

	setTwoButtonFiring(i, false);
}

/**
 * \brief	This will start the joystick driver and search for all the controls attached
 * 			to your computer
 * \return	false, if the driver couldn't be started, else true
 */
bool CInput::startJoyDriver()
{
	gLogging.textOut("JoyDrv_Start() : ");

    if (SDL_Init( SDL_INIT_JOYSTICK ) < 0)
	{
		gLogging.ftextOut("JoyDrv_Start() : Couldn't initialize SDL: %s<br>", SDL_GetError());
		return 1;
	}
    else
	{
		const size_t joyNum = SDL_NumJoysticks();
		if( joyNum > 0 )
		{
			SDL_JoystickEventState(SDL_ENABLE);
			gLogging.ftextOut("Detected %i joystick(s).<br>\n", joyNum );
			gLogging.textOut("The names of the joysticks are:<br>");

			for( size_t i=0; i < joyNum; i++ )
			{
#if SDL_VERSION_ATLEAST(2, 0, 0)
                gLogging.ftextOut("    %s<br>", SDL_JoystickNameForIndex(i));
#else
                gLogging.ftextOut("    %s<br>", SDL_JoystickName(i));
#endif

				SDL_Joystick *pJoystick = SDL_JoystickOpen(i);
				mp_Joysticks.push_back(pJoystick);

				gLogging.ftextOut("     Axes: %i<br>", SDL_JoystickNumAxes(pJoystick));
				gLogging.ftextOut("     Buttons: %i <br>", SDL_JoystickNumButtons(pJoystick));
				gLogging.ftextOut("     Balls: %i <br>", SDL_JoystickNumBalls(pJoystick));
				gLogging.ftextOut("     Hats: %i<br>", SDL_JoystickNumHats(pJoystick));
			}
		}
		else
		{
			gLogging.ftextOut("No joysticks were found.<br>\n");
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
            Configuration.ReadString( section, "Left", value, "Left");
			setupInputCommand( InputCommand[i], IC_LEFT, value );
            Configuration.ReadString( section, "Up", value, "Up");
			setupInputCommand( InputCommand[i], IC_UP, value );
            Configuration.ReadString( section, "Right", value, "Right");
			setupInputCommand( InputCommand[i], IC_RIGHT, value );
            Configuration.ReadString( section, "Down", value, "Down");
			setupInputCommand( InputCommand[i], IC_DOWN, value );

            Configuration.ReadString( section, "Lower-Left", value, "End");
			setupInputCommand( InputCommand[i], IC_LOWERLEFT, value );
            Configuration.ReadString( section, "Lower-Right", value, "Page Down");
			setupInputCommand( InputCommand[i], IC_LOWERRIGHT, value );
            Configuration.ReadString( section, "Upper-Left", value, "Home");
			setupInputCommand( InputCommand[i], IC_UPPERLEFT, value );
            Configuration.ReadString( section, "Upper-Right", value, "Page Up");
			setupInputCommand( InputCommand[i], IC_UPPERRIGHT, value );

            Configuration.ReadString( section, "Jump", value, "Left ctrl");
			setupInputCommand( InputCommand[i], IC_JUMP, value );
            Configuration.ReadString( section, "Pogo", value, "Left alt");
			setupInputCommand( InputCommand[i], IC_POGO, value );
            Configuration.ReadString( section, "Fire", value, "Space");
			setupInputCommand( InputCommand[i], IC_FIRE, value );
            Configuration.ReadString( section, "Status", value, "Return");
			setupInputCommand( InputCommand[i], IC_STATUS, value );
            Configuration.ReadString( section, "Camlead", value, "c");
			setupInputCommand( InputCommand[i], IC_CAMLEAD, value );
            Configuration.ReadString( section, "Help", value, "F11");
			setupInputCommand( InputCommand[i], IC_HELP, value );
            Configuration.ReadString( section, "Back", value, "Escape");
			setupInputCommand( InputCommand[i], IC_BACK, value );

			Configuration.ReadKeyword( section, "TwoButtonFiring", &TwoButtonFiring[i], false);
			Configuration.ReadKeyword( section, "Analog", &mAnalogAxesMovement[i], false);
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
		Configuration.WriteString(section, "Camlead", getEventName(IC_CAMLEAD, i));
		Configuration.WriteString(section, "Help", getEventName(IC_HELP, i));
		Configuration.WriteString(section, "Back", getEventName(IC_BACK, i));
		Configuration.SetKeyword(section, "TwoButtonFiring", TwoButtonFiring[i]);
		Configuration.SetKeyword(section, "Analog", mAnalogAxesMovement[i]);
	}
	Configuration.saveCfgFile();
}

/**
 * Gets the event name from the last mapped event
 */
std::string CInput::getNewMappedEvent(int &rPos, unsigned char &rInp)
{
    rPos = remapper.mapPosition;
    rInp = remapper.mapDevice;
    return getEventShortName(remapper.mapPosition, remapper.mapDevice);
}


std::string CInput::getEventShortName(int command, unsigned char input)
{
	std::string buf;
	if(InputCommand[input][command].joyeventtype == ETYPE_JOYAXIS)
	{
	  buf = "J" + itoa(InputCommand[input][command].which) + "A" + itoa(InputCommand[input][command].joyaxis);
	  if(InputCommand[input][command].joyvalue < 0)
	    buf += "-";
	  else
	    buf += "+";
	}
	else if(InputCommand[input][command].joyeventtype == ETYPE_JOYBUTTON)
	{
	  buf = "J" + itoa(InputCommand[input][command].which) + "B" + itoa(InputCommand[input][command].joybutton);
	}
	else if(InputCommand[input][command].joyeventtype == ETYPE_JOYHAT)
	{
	  buf = "J" + itoa(InputCommand[input][command].which) + "H" + itoa(InputCommand[input][command].joyhatval);
	}
	else // In case only keyboard was triggered
	{
	  buf = SDL_GetKeyName(InputCommand[input][command].keysym);
	}

	return buf;
}

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
	else if(InputCommand[input][command].joyeventtype == ETYPE_JOYHAT)
	{
	  buf = "Joy" + itoa(InputCommand[input][command].which) + "-H" + itoa(InputCommand[input][command].joyhatval);
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
		else if(buf2 == "B")
		{
			pInput[action].joyeventtype = ETYPE_JOYBUTTON;
			pInput[action].joybutton = atoi(buf);
		}
		else // Should normally be H
		{
			pInput[action].joyeventtype = ETYPE_JOYHAT;
			pInput[action].joyhatval = atoi(buf);
		}
		return;
	}

	if(strCaseStartsWith(string, "Key"))
	{
		pInput[action].joyeventtype = ETYPE_KEYBOARD;
		buf = buf.substr(3);
		TrimSpaces(buf);
#if SDL_VERSION_ATLEAST(2, 0, 0)
		pInput[action].keysym = (SDL_Keycode) atoi(buf);
#else
        pInput[action].keysym = (SDLKey) atoi(buf);
#endif
		return;
	}
}

/**
 * \brief	Prepares the Input Controller to map a new command
 * \param	device		input of which we are trying to read the event
 * \param	command		command for which we want to assign the event
 * \return 	returns true, if an event was triggered, or false if not.
 */
void CInput::setupNewEvent(Uint8 device, int position)
{
    remapper.mapDevice = device;
    remapper.mapPosition = position;
    remapper.mappingInput = true;
}

/**
 * \brief	This checks if some event was triggered to get the new input command
 * \param	device		input of which we are trying to read the event
 * \param	command		command for which we want to assign the event
 * \return 	returns true, if an event was triggered, or false if not.
 */
void CInput::readNewEvent()
{

	stInputCommand &lokalInput = InputCommand[remapper.mapDevice][remapper.mapPosition];

	// This function is used to configure new input keys.
	// For iPhone, we have emulation via touchpad and we don't want to have custom keys.
	// We should fix the menu for iPhone so that this function doesn't get called.
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	printf("WARNING: called readNewEvent on iphone\n");
	return;
#endif

	memset(&lokalInput, 0, sizeof(stInputCommand));
	if(!m_EventList.empty())
		m_EventList.clear();

	while( SDL_PollEvent( &Event ) )
	{
		switch ( Event.type )
		{
			case SDL_QUIT:
				gLogging.textOut("SDL: Got quit event in readNewEvent!");
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
				// on iPhone, we just want to quit in this case
				exit(0);
#endif
                break;

			case SDL_KEYDOWN:
				lokalInput.joyeventtype = ETYPE_KEYBOARD;
				lokalInput.keysym = Event.key.keysym.sym;
				remapper.mappingInput = false;
				break;

			case SDL_JOYBUTTONDOWN:
#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
				WIZ_EmuKeyboard( Event.jbutton.button, 1 );
				return false;
#else
				lokalInput.joyeventtype = ETYPE_JOYBUTTON;
				lokalInput.joybutton = Event.jbutton.button;
				lokalInput.which = Event.jbutton.which;
				remapper.mappingInput = false;
#endif
				break;

			case SDL_JOYAXISMOTION:

				// Deadzone check. Double, because being a
				// new event to be read it should make better to configure
				if( (Event.jaxis.value > 2*m_joydeadzone ) ||
				    (Event.jaxis.value < -2*m_joydeadzone ) )
				{
					lokalInput.joyeventtype = ETYPE_JOYAXIS;
					lokalInput.joyaxis = Event.jaxis.axis;
					lokalInput.which = Event.jaxis.which;
					lokalInput.joyvalue = (Event.jaxis.value>0) ? 32767 : -32767;
					remapper.mappingInput = false;
				}

				break;

			case SDL_JOYHATMOTION:
				lokalInput.joyeventtype = ETYPE_JOYHAT;
				lokalInput.joyhatval = Event.jhat.value;
				lokalInput.which = Event.jhat.which;
				remapper.mappingInput = false;
				break;
		}

        flushAll();
    }
}

bool CInput::getTwoButtonFiring(int player) { return TwoButtonFiring[player]; }
void CInput::setTwoButtonFiring(int player, bool value) { TwoButtonFiring[player]=value; }

bool CInput::isAnalog(const int player) { return mAnalogAxesMovement[player]; }
void CInput::enableAnalog(const int player, const bool value) { mAnalogAxesMovement[player]=value; }


void CInput::transMouseRelCoord(Vector2D<float> &Pos,
								const SDL_MouseMotionEvent motion,
								const GsRect<Uint16> &transformRect)
{
    Pos.x = ( static_cast<float>(motion.x-transformRect.x)/static_cast<float>(transformRect.w) );
    Pos.y = ( static_cast<float>(motion.y-transformRect.y)/static_cast<float>(transformRect.h) );
}




/**
 * \brief	Called every logic cycle. This triggers the events that occur and process them through various functions
 */
void CInput::pollEvents()
{
    // Semaphore
    SDL_SemWait( pollSem );

    if(remapper.mappingInput)
    {
        readNewEvent();
        SDL_SemPost( pollSem );
        return;
    }

    Vector2D<float> Pos;
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
    GsRect<Uint16> Res(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
#endif

	// copy all the input of the last poll to a space for checking pressing or holding a button
	memcpy(last_immediate_keytable, immediate_keytable, KEYTABLE_SIZE*sizeof(char));

	for(int i=0 ; i<MAX_COMMANDS ; i++)
		for(int j=0 ; j<NUM_INPUTS ; j++)
			InputCommand[j][i].lastactive = InputCommand[j][i].active;


    GsRect<Uint16> clickGameArea = gVideoDriver.mpVideoEngine->getAspectCorrRect();

    auto &dispRect = gVideoDriver.getVidConfig().m_DisplayRect;


/*#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
    //if( !gVideoDriver.isOpenGL() )
#endif*/
    {
        clickGameArea.x = 0;
        clickGameArea.y = 0;
    }



	// While there's an event to handle
	while( SDL_PollEvent( &Event ) )
	{
        bool passSDLEventVec = true;

		switch( Event.type )
		{
		case SDL_QUIT:
			gLogging.textOut("SDL: Got quit event!");
			m_exit = true;
			break;
        case SDL_KEYDOWN:
            passSDLEventVec = processKeys(1);
			break;
		case SDL_KEYUP:
            passSDLEventVec = processKeys(0);
            break;
		case SDL_JOYAXISMOTION:
            passSDLEventVec = true;
			processJoystickAxis();
			break;
		case SDL_JOYBUTTONDOWN:
            passSDLEventVec = true;
			processJoystickButton(1);
			break;
		case SDL_JOYBUTTONUP:
            passSDLEventVec = true;
			processJoystickButton(0);
			break;

		case SDL_JOYHATMOTION:
            passSDLEventVec = true;
			processJoystickHat();
			break;

#if SDL_VERSION_ATLEAST(2, 0, 0)
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
		case SDL_FINGERMOTION:
			processMouse(Event);
			break;

        case SDL_WINDOWEVENT:
            if(Event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                gVideoDriver.mpVideoEngine->resizeDisplayScreen(
                        GsRect<Uint16>(Event.window.data1,
                                      Event.window.data2) );
                dispRect.w = Event.window.data1;
                dispRect.h = Event.window.data2;
            }
            break;
#else
		case SDL_VIDEORESIZE:
            gVideoDriver.mpVideoEngine->resizeDisplayScreen(
					GsRect<Uint16>(Event.resize.w, Event.resize.h) );
            dispRect.w = Event.resize.w;
            dispRect.h = Event.resize.h;
			break;
#endif

		case SDL_MOUSEBUTTONDOWN:

            if(Event.button.button <= 3)
            {
                transMouseRelCoord(Pos, Event.motion, clickGameArea);
                m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                gPointDevice.mPointingState.mActionButton = 1;
                gPointDevice.mPointingState.mPos = Pos;
            }
            else if(Event.button.button == 4) // scroll up
            {
                gEventManager.add( new MouseWheelEvent( Vector2D<float>(0.0, -1.0) ) );
            }
            else if(Event.button.button == 5) // scroll down
            {
                gEventManager.add( new MouseWheelEvent( Vector2D<float>(0.0, 1.0) ) );
            }

			break;

		case SDL_MOUSEBUTTONUP:
            passSDLEventVec = true;
            transMouseRelCoord(Pos, Event.motion, clickGameArea);
            m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONUP ) );
            gPointDevice.mPointingState.mActionButton = 0;
            gPointDevice.mPointingState.mPos = Pos;

			break;

		case SDL_MOUSEMOTION:
            transMouseRelCoord(Pos, Event.motion, clickGameArea);
            m_EventList.add( new PointingDevEvent( Pos, PDE_MOVED ) );
            gPointDevice.mPointingState.mPos = Pos;
			break;
		}

        if(passSDLEventVec)
        {
            mSDLEventVec.push_back(Event);
        }
        else
        {
            mBackEventBuffer.push_back(Event);
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
        value = gVideoDriver.getFullscreen();
		value = !value;
		gLogging.textOut(GREEN,"Fullscreen mode triggered by user!<br>");
        gVideoDriver.isFullscreen(value);

		// initialize/activate all drivers
		gLogging.ftextOut("Restarting graphics driver...<br>");
        if ( gVideoDriver.applyMode() && gVideoDriver.start() )
		{
			gLogging.ftextOut(PURPLE, "Toggled Fullscreen quick shortcut...<br>");
		}
		else
		{
			value = !value;
			gLogging.ftextOut(PURPLE, "Couldn't change the resolution, Rolling back...<br>");
            gVideoDriver.applyMode();
            gVideoDriver.start();
		}

		gInput.flushAll();
	}

	// Check, if LALT+Q or LALT+F4 was pressed
	if(getHoldedKey(KALT) && (getPressedKey(KF4) || getPressedKey(KQ)) )
	{
		gLogging.textOut("User exit request!");
		m_exit = true;
	}
#endif

#if defined(WIZ) || defined(GP2X)
	WIZ_AdjustVolume( volume_direction );
#endif

    SDL_SemPost( pollSem );
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

void CInput::processJoystickHat()
{
	for(int j=0 ; j<NUM_INPUTS ; j++)
	{
		for(int i=0 ; i<MAX_COMMANDS ; i++)
		{
			stInputCommand &command = InputCommand[j][i];

			if( command.joyeventtype == ETYPE_JOYHAT &&
				command.which == Event.jhat.which )
			{
				command.active = false;
				// Check if Joystick hats are configured for this event
				if(
					(Event.jhat.value & command.joyhatval) )
				{
					command.active = true;
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
bool CInput::processKeys(int keydown)
{
    bool passSDLEventVec = true;

	// Input for player commands
    for(int j=0 ; j<NUM_INPUTS ; j++)
	{
        for(int i=0 ; i<MAX_COMMANDS ; i++)
		{
			if(InputCommand[j][i].keysym == Event.key.keysym.sym &&
					InputCommand[j][i].joyeventtype == ETYPE_KEYBOARD)
            {
				InputCommand[j][i].active = (keydown) ? true : false;

                if(i == IC_BACK)
                {
                    passSDLEventVec = false;
                }
            }
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

    return passSDLEventVec;

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
 * \brief	normally called, when a save game or high score entry is being made. It will return a character that was typed.
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


bool CInput::mouseClicked()
{
    // Check event queue for the clicked button
	std::deque< std::shared_ptr<CEvent> >::iterator it = m_EventList.begin();

	for( ; it != m_EventList.end() ; it++ )
	{
        if( PointingDevEvent *mouseevent = dynamic_cast<PointingDevEvent*> (it->get()) )
		{
			// Here we check if the mouse-cursor/Touch entry clicked on our Button
            if(mouseevent->Type == PDE_BUTTONUP)
			{
				m_EventList.erase(it);
				return true;
			}
		}
	}
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

bool CInput::getPressedAnyButtonCommand(const int player)
{
	for(int i=IC_JUMP ; i<MAX_COMMANDS ; i++)
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

/**
  * @brief flushEvent This will clear the whole event list
  */
void CInput::flushEvents()
{
    m_EventList.clear();
}

struct TouchButton
{
	stInputCommand* cmd;
	int immediateIndex;
	int x, y, w, h;
    bool invisible;

	bool isInside(int _x, int _y) const {
		return
		x <= _x && _x < x + w &&
		y <= _y && _y < y + h;
	}
};

static const int w = 480, h = 320;

#define KSHOWHIDECTRLS	(-10)

//#if defined(MOUSEWRAPPER)
#if SDL_VERSION_ATLEAST(2, 0, 0)
static TouchButton* getPhoneButtons(stInputCommand InputCommand[NUM_INPUTS][MAX_COMMANDS]) {

	static TouchButton phoneButtons[] = {
		{ &InputCommand[0][IC_LEFT],        KLEFT,	0,      h*5/8,  w / 6,  h / 4},
        { &InputCommand[0][IC_UPPERLEFT],	-1,     0,      h / 2,  w / 6,  h / 8,  true},
		{ &InputCommand[0][IC_UP],          KUP,	w / 6,  h*2/4,  w / 6,  h / 4},
        { &InputCommand[0][IC_UPPERRIGHT],	-1,     w / 3,  h / 2,  w / 6,  h / 8,  true},
		{ &InputCommand[0][IC_RIGHT],       KRIGHT,	w / 3,  h*5/8,  w / 6,  h / 4},
        { &InputCommand[0][IC_LOWERRIGHT],	-1,     w / 3,  h*7/8,  w / 6,  h / 8,  true},
		{ &InputCommand[0][IC_DOWN],        KDOWN,	w / 6,  h*3/4,  w / 6,  h / 4},
        { &InputCommand[0][IC_LOWERLEFT],	-1,     0,      h*7/8,  w / 6,  h / 8,  true},

		{ &InputCommand[0][IC_JUMP],        KCTRL,	w / 2,  h*2/3,  w / 6,  h / 3},
		{ &InputCommand[0][IC_POGO],        KALT,	w*2/3,  h*2/3,  w / 6,  h / 3},
		{ &InputCommand[0][IC_FIRE],        KSPACE,	w*5/6,  h*2/3,  w / 6,  h / 3},

		{ &InputCommand[0][IC_BACK],        KQUIT,	0,      0,      w / 6,  h / 6},
		{ &InputCommand[0][IC_STATUS],      KENTER, 5*w/6,  h / 6,  w / 6,  h / 6},
        { &InputCommand[0][IC_HELP],        KF1,	0,      h / 6,  w / 6,  h / 6},
		{ NULL,                     KSHOWHIDECTRLS,	5*w/6,  0,      w / 6,  h / 6},
	};

	return phoneButtons;
}



static const int phoneButtonN = 15;
typedef std::set<int> MouseIndexSet;

static Uint32 phoneButtonLasttime[phoneButtonN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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

void CInput::processMouse()
{
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

	SDL_Rect screenRect;
    //SDL_Touch* touch = SDL_GetTouch(ev.tfinger.touchId);
    SDL_Finger* touch = SDL_GetTouchFinger(ev.tfinger.touchId, 0);
    int x, y, dx, dy, w, h;

	if(SDL_GetDisplayBounds(0, &screenRect) == 0) {
		w = screenRect.w;
        h = screenRect.h;
	}

    if(touch == NULL) return; //The touch has been removed

    //float fx = ((float)ev.tfinger.x)/touch->xres;
    //float fy = ((float)ev.tfinger.y)/touch->yres;
    float fx = ((float)ev.tfinger.x)/touch->x;
    float fy = ((float)ev.tfinger.y)/touch->y;
    x = (int)(fx*w); y = (int)(fy*h);



	switch(ev.type) {
		case SDL_FINGERDOWN:
			processMouse(x, y, true, ev.tfinger.fingerId);
			break;

		case SDL_FINGERUP:
			processMouse(x, y, false, ev.tfinger.fingerId);
			break;

		case SDL_FINGERMOTION:
            //float fdx = ((float)ev.tfinger.dx)/touch->xres;
            //float fdy = ((float)ev.tfinger.dy)/touch->yres;
            float fdx = ((float)ev.tfinger.dx)/touch->x;
            float fdy = ((float)ev.tfinger.dy)/touch->y;
            dx = (int)(fdx*w); dy = (int)(fdy*h);
			processMouse(x - dx, y - dy, false, ev.tfinger.fingerId);
			processMouse(x, y, true, ev.tfinger.fingerId);
			break;
	}
//#endif
}

void CInput::processMouse(int x, int y, bool down, int mouseindex)
{

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

	float w = 480.0f, h = 320.0f;

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

void CInput::renderOverlay()
{
#ifdef USE_OPENGL // only ogl supported yet (and probably worth)
#if defined(MOUSEWRAPPER)
	static bool showControls = true;
	static bool buttonShowHideCtrlWasDown = false;

	TouchButton* phoneButtons = getPhoneButtons(InputCommand);

	for(int i = phoneButtonN - 1; i >= 0; --i) {
		TouchButton& b = phoneButtons[i];
		bool down = phoneButton_MouseIndex[i].size() > 0;
        if(i==0)
        {
            if (phoneButton_MouseIndex[1].size() > 0 || phoneButton_MouseIndex[7].size() > 0)
                down = true;
        }
        if(i==2 || i==4 || i == 6)
        {
            if (phoneButton_MouseIndex[i-1].size() > 0 || phoneButton_MouseIndex[i+1].size() > 0)
                down = true;
        }
		if((showControls && !b.invisible) || b.immediateIndex == KSHOWHIDECTRLS) drawButton(b, down);

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
 * \brief	flushes all the input events
 */
void CInput::flushAll()
{
    flushKeys();
    flushCommands();
    flushEvents();
}


/**
 * \brief	shuts down the input driver.
 */
void CInput::shutdown()
{
	// Shutdown Joysticks
	while(!mp_Joysticks.empty())
	{
		SDL_JoystickClose(mp_Joysticks.back());
		mp_Joysticks.pop_back();
	}
}

bool CInput::readSDLEventVec(std::vector<SDL_Event> &evVec)
{
    SDL_SemWait( pollSem );

    if(mSDLEventVec.empty())
    {
        SDL_SemPost( pollSem );
        return false;
    }

    evVec = mSDLEventVec;

    mSDLEventVec.clear();

    SDL_SemPost( pollSem );

    return true;
}

void CInput::pushBackButtonEventExtEng()
{
    SDL_SemWait( pollSem );

    if(mBackEventBuffer.empty())
    {
        SDL_SemPost( pollSem );
        return;
    }

    // Take one event and send an down and up event
    for( SDL_Event ev : mBackEventBuffer )
    {
        ev.type = SDL_KEYDOWN;
        mSDLEventVec.push_back(ev);
        ev.type = SDL_KEYUP;
        mSDLEventVec.push_back(ev);
        break;
    }

    mBackEventBuffer.clear();

    SDL_SemPost( pollSem );
}

