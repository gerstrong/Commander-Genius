/*
 * CInput.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>

#include "InputEvents.h"
#include "CInput.h"
#include "video/CVideoDriver.h"
#include "GsLogging.h"
#include "utils/FindFile.h"
#include "PointDevice.h"
#include "GsTimer.h"

#include "fileio/CConfiguration.h"


#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
#include "sys/wizgp2x.h"
#endif

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
//#define MOUSEWRAPPER 1
#endif


CInput::~CInput()
{
    SDL_DestroySemaphore(mpPollSem);
}

CInput::CInput()
{

#if defined(WIZ) || defined(GP2X)
    volume_direction = VOLUME_NOCHG;
    volume = 60-VOLUME_CHANGE_RATE;
    WIZ_AdjustVolume(VOLUME_UP);
#endif
    gLogging.ftextOut("Starting the input driver...<br>");

    for(int c=0 ; c<NUM_INPUTS ; c++)
        resetControls(c);
    memset(&Event, 0, sizeof(Event));

#if !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
    loadControlconfig(""); // we want to have the default settings in all cases
    startJoyDriver(); // not for iPhone for now, could cause trouble (unwanted input events)
#endif

    mpPollSem = SDL_CreateSemaphore(1);

}


/**
 * \brief This will reset the player controls how they saved before.
 * 		  The Default controls are going to be restored when this function
 * 		  is executed
 * \param	player	Number of player of which the controls will be reset (1-4)
 */
void CInput::resetControls(const int player)
{
    m_exit = false;

    m_cmdpulse = 0;
    m_joydeadzone = 1024;

    immediate_keytable.fill(false);
    last_immediate_keytable.fill(false);

    auto &curInput = mInputCommands[player];

    for(auto &input : curInput)
    {
        input.active = false;
    }

#ifdef __SWITCH__
    // Switch gamepad mapping using two joycons
    curInput[IC_LEFT].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_LEFT].joybutton = 12;
    curInput[IC_LEFT].which = 0;
    curInput[IC_UP].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_UP].joybutton = 13;
    curInput[IC_UP].which = 0;
    curInput[IC_RIGHT].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_RIGHT].joybutton = 14;
    curInput[IC_RIGHT].which = 0;
    curInput[IC_DOWN].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_DOWN].joybutton = 15;
    curInput[IC_DOWN].which = 0;

    curInput[IC_JUMP].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_JUMP].joybutton = 0;
    curInput[IC_JUMP].which = 0;
    curInput[IC_POGO].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_POGO].joybutton = 2;
    curInput[IC_POGO].which = 0;
    curInput[IC_FIRE].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_FIRE].joybutton = 3;
    curInput[IC_FIRE].which = 0;
    curInput[IC_RUN].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_RUN].joybutton = 1;
    curInput[IC_RUN].which = 0;

    curInput[IC_STATUS].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_STATUS].joybutton = 11;
    curInput[IC_STATUS].which = 0;
    curInput[IC_CAMLEAD].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_CAMLEAD].joybutton = 7;
    curInput[IC_CAMLEAD].which = 0;
    curInput[IC_HELP].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_HELP].joybutton = 6;
    curInput[IC_HELP].which = 0;
    curInput[IC_BACK].joyeventtype = (unsigned int)EType::JOYBUTTON;
    curInput[IC_BACK].joybutton = 10;
    curInput[IC_BACK].which = 0;
#else
    // These are the default keyboard commands

    curInput[IC_LEFT].keysym = SDLK_LEFT;
    curInput[IC_UP].keysym = SDLK_UP;
    curInput[IC_RIGHT].keysym = SDLK_RIGHT;
    curInput[IC_DOWN].keysym = SDLK_DOWN;

    curInput[IC_UPPERLEFT].keysym = SDLK_HOME;
    curInput[IC_UPPERRIGHT].keysym = SDLK_PAGEUP;
    curInput[IC_LOWERLEFT].keysym = SDLK_END;
    curInput[IC_LOWERRIGHT].keysym = SDLK_PAGEDOWN;

    curInput[IC_JUMP].keysym = SDLK_LCTRL;
    curInput[IC_POGO].keysym = SDLK_LALT;
    curInput[IC_FIRE].keysym = SDLK_SPACE;
    curInput[IC_RUN].keysym = SDLK_UNKNOWN;

    curInput[IC_STATUS].keysym = SDLK_RETURN;

    curInput[IC_CAMLEAD].keysym = SDLK_c;
    curInput[IC_HELP].keysym = SDLK_F1;
    curInput[IC_BACK].keysym = SDLK_ESCAPE;

    curInput[IC_QUICKSAVE].keysym = SDLK_F6;
    curInput[IC_QUICKLOAD].keysym = SDLK_F9;

    // And those are the default joystick handlings, but they are disabled by default
    curInput[IC_LEFT].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_LEFT].joyaxis = 0;
    curInput[IC_LEFT].joyvalue = -32767;
    curInput[IC_LEFT].ctlraxis = 0;
    curInput[IC_LEFT].ctlraxisvalue = -32767;
    curInput[IC_LEFT].which = 0;
    curInput[IC_UP].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_UP].joyaxis = 1;
    curInput[IC_UP].joyvalue = -32767;
    curInput[IC_UP].ctlraxis = 1;
    curInput[IC_UP].ctlraxisvalue = -32767;
    curInput[IC_UP].which = 0;
    curInput[IC_RIGHT].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_RIGHT].joyaxis = 0;
    curInput[IC_RIGHT].joyvalue = 32767;
    curInput[IC_RIGHT].ctlraxis = 0;
    curInput[IC_RIGHT].ctlraxisvalue = 32767;
    curInput[IC_RIGHT].which = 0;
    curInput[IC_DOWN].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_DOWN].joyaxis = 1;
    curInput[IC_DOWN].joyvalue = 32767;
    curInput[IC_DOWN].ctlraxis = 1;
    curInput[IC_DOWN].ctlraxisvalue = 32767;
    curInput[IC_DOWN].which = 0;

    curInput[IC_JUMP].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_JUMP].joybutton = 0;
    curInput[IC_JUMP].ctlrbutton = 0;
    curInput[IC_JUMP].which = 0;
    curInput[IC_POGO].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_POGO].joybutton = 1;
    curInput[IC_POGO].ctlrbutton = 1;
    curInput[IC_POGO].which = 0;
    curInput[IC_FIRE].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_FIRE].joybutton = 2;
    curInput[IC_FIRE].ctlrbutton = 2;
    curInput[IC_FIRE].which = 0;
    curInput[IC_RUN].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_RUN].joybutton = 0;
    curInput[IC_RUN].ctlrbutton = 0;
    curInput[IC_RUN].which = 0;
    curInput[IC_STATUS].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_STATUS].joybutton = 4;
    curInput[IC_STATUS].ctlrbutton = 4;
    curInput[IC_STATUS].which = 0;
    curInput[IC_CAMLEAD].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_CAMLEAD].joybutton = 5;
    curInput[IC_CAMLEAD].ctlrbutton = 5;
    curInput[IC_CAMLEAD].which = 0;
    curInput[IC_HELP].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_HELP].joybutton = 6;
    curInput[IC_HELP].ctlrbutton = 6;
    curInput[IC_HELP].which = 0;
    curInput[IC_BACK].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_BACK].joybutton = 7;
    curInput[IC_BACK].ctlrbutton = 7;
    curInput[IC_BACK].which = 0;

    curInput[IC_QUICKSAVE].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_QUICKSAVE].joybutton = 8;
    curInput[IC_QUICKSAVE].ctlrbutton = 8;
    curInput[IC_QUICKSAVE].which = 0;
    curInput[IC_QUICKLOAD].joyeventtype = (unsigned int)EType::KEYBOARD;
    curInput[IC_QUICKLOAD].joybutton = 9;
    curInput[IC_QUICKLOAD].ctlrbutton = 9;
    curInput[IC_QUICKLOAD].which = 0;

#endif

    setTwoButtonFiring(player, false);
}


void CInput::openJoyAndPrintStats(const int idx)
{

    for(auto &curJoy : mp_Joysticks)
    {
        // Is joystick already added? If found one, don't read it.
        const auto curInstance = SDL_JoystickInstanceID(curJoy);
        const auto newInstance = SDL_JoystickGetDeviceInstanceID(idx);

        if(newInstance < 0)
        {
            break;
        }

        if(curInstance == newInstance)
        {
            return;
        }
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)
    gLogging << "Joystick/Gamepad detected:" << CLogFile::endl;
    gLogging << "    " << SDL_JoystickNameForIndex(idx) << CLogFile::endl;
#else
    gLogging << "    " << SDL_JoystickName(idx) << CLogFile::endl;
#endif

    SDL_Joystick *pJoystick = SDL_JoystickOpen(idx);

    if(!pJoystick)
    {
        gLogging << "     Error adding joystick " << idx << CLogFile::endl;
        return;
    }

    char guidStr[64];

    const auto guid = SDL_JoystickGetGUID(pJoystick);

    SDL_JoystickGetGUIDString(guid, guidStr, sizeof (guidStr));

    const auto id = SDL_JoystickInstanceID(pJoystick);
    mJoyIdToInputIdx[id] = mp_Joysticks.size();
    mp_Joysticks.push_back(pJoystick);

    gLogging << "     Axes: " << SDL_JoystickNumAxes(pJoystick) << CLogFile::endl;
    gLogging << "     Buttons: " << SDL_JoystickNumButtons(pJoystick) << CLogFile::endl;
    gLogging << "     Balls: " << SDL_JoystickNumBalls(pJoystick) << CLogFile::endl;
    gLogging << "     Hats: " << SDL_JoystickNumHats(pJoystick) << CLogFile::endl;
    gLogging << "     GUID: " << guidStr << CLogFile::endl;
}

void CInput::enableJoysticks()
{
    SDL_JoystickEventState(SDL_ENABLE);

    const auto joyNum = SDL_NumJoysticks();
    if( joyNum > int(mp_Joysticks.size()) )
    {
        gLogging << "Detected " << joyNum-mp_Joysticks.size() << " joystick(s)." << CLogFile::endl;
        gLogging << "The names of the joysticks are:" << CLogFile::endl;

        for( auto i=int(mp_Joysticks.size()); i < joyNum; i++ )
        {
            openJoyAndPrintStats(i);
        }
    }
    else
    {
        gLogging << "No joysticks were found." << CLogFile::endl;
    }
}

/**
 * \brief	This will start the joystick driver and search for all the controls attached
 * 			to your computer
 * \return	false, if the driver couldn't be started, else true
 */
bool CInput::startJoyDriver()
{
    if (SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) < 0)
    {
        gLogging << "JoyDrv_Start() : Couldn't initialize SDL: "
                 << SDL_GetError() << CLogFile::endl;
        return 1;
    }

    enableJoysticks();

    return 0;
}

/**
 * \brief	This will load input settings that were saved previously by the user at past session.
 */
void CInput::loadControlconfig(const std::string &presetName)
{
    CConfiguration Configuration;
    if(Configuration.Parse())
    {
        std::string section;
        for(size_t i=0 ; i<mInputCommands.size() ; i++)
        {
            // setup input from proper string
            section = "input" + itoa(i);

            if(!presetName.empty())
            {
                section += "-" + presetName;
            }


            std::string value;
            auto &curInput = mInputCommands[i];
            Configuration.ReadString( section, "Left", value, "Left");
            setupInputCommand( curInput, IC_LEFT, value );
            Configuration.ReadString( section, "Up", value, "Up");
            setupInputCommand( curInput, IC_UP, value );
            Configuration.ReadString( section, "Right", value, "Right");
            setupInputCommand( curInput, IC_RIGHT, value );
            Configuration.ReadString( section, "Down", value, "Down");
            setupInputCommand( curInput, IC_DOWN, value );

            Configuration.ReadString( section, "Lower-Left", value, "End");
            setupInputCommand( curInput, IC_LOWERLEFT, value );
            Configuration.ReadString( section, "Lower-Right", value, "Page Down");
            setupInputCommand( curInput, IC_LOWERRIGHT, value );
            Configuration.ReadString( section, "Upper-Left", value, "Home");
            setupInputCommand( curInput, IC_UPPERLEFT, value );
            Configuration.ReadString( section, "Upper-Right", value, "Page Up");
            setupInputCommand( curInput, IC_UPPERRIGHT, value );

            Configuration.ReadString( section, "Jump", value, "Left ctrl");
            setupInputCommand( curInput, IC_JUMP, value );
            Configuration.ReadString( section, "Pogo", value, "Left alt");
            setupInputCommand( curInput, IC_POGO, value );
            Configuration.ReadString( section, "Fire", value, "Space");
            setupInputCommand( curInput, IC_FIRE, value );
            Configuration.ReadString( section, "Run", value, "Shift");
            setupInputCommand( curInput, IC_RUN, value );
            Configuration.ReadString( section, "Status", value, "Return");
            setupInputCommand( curInput, IC_STATUS, value );
            Configuration.ReadString( section, "Camlead", value, "c");
            setupInputCommand( curInput, IC_CAMLEAD, value );
            Configuration.ReadString( section, "Help", value, "F11");
            setupInputCommand( curInput, IC_HELP, value );
            Configuration.ReadString( section, "Back", value, "Escape");
            setupInputCommand( curInput, IC_BACK, value );

            Configuration.ReadString( section, "Quicksave", value, "F6");
            setupInputCommand( curInput, IC_QUICKSAVE, value );
            Configuration.ReadString( section, "Quickload", value, "F9");
            setupInputCommand( curInput, IC_QUICKLOAD, value );

            Configuration.ReadKeyword( section, "TwoButtonFiring", &TwoButtonFiring[i], false);
            Configuration.ReadKeyword( section, "Analog", &mAnalogAxesMovement[i], false);
            Configuration.ReadKeyword( section, "SuperPogo", &mSuperPogo[i], false);
            Configuration.ReadKeyword( section, "ImpossiblePogo", &mImpPogo[i], true);
            Configuration.ReadKeyword( section, "AutoFire", &mFullyAutomatic[i], false);
        }
    }
    else
    {
        for(size_t c=0 ; c< NUM_INPUTS ; c++)
            resetControls(int(c));
    }
}

/**
 * \brief	This will save input settings according to how the user did map the buttons,
 * 			axes or keys to the commands.
 */
void CInput::saveControlconfig(const std::string &presetName)
{
    CConfiguration Configuration;
    Configuration.Parse();

    std::string section;
    for(size_t i=0 ; i<NUM_INPUTS ; i++)
    {
        section = "input" + itoa(i);

        if(!presetName.empty())
        {
            section += "-" + presetName;
        }

        const auto inputVal = static_cast<unsigned char>(i);

        // Movement section
        Configuration.WriteString(section, "Left", getEventName(IC_LEFT, inputVal));
        Configuration.WriteString(section, "Up", getEventName(IC_UP, inputVal));
        Configuration.WriteString(section, "Right", getEventName(IC_RIGHT, inputVal));
        Configuration.WriteString(section, "Down", getEventName(IC_DOWN, inputVal));
        Configuration.WriteString(section, "Upper-Left", getEventName(IC_UPPERLEFT, inputVal));
        Configuration.WriteString(section, "Upper-Right", getEventName(IC_UPPERRIGHT, inputVal));
        Configuration.WriteString(section, "Lower-Left", getEventName(IC_LOWERLEFT, inputVal));
        Configuration.WriteString(section, "Lower-Right", getEventName(IC_LOWERRIGHT, inputVal));

        // Button section
        Configuration.WriteString(section, "Jump", getEventName(IC_JUMP, inputVal));
        Configuration.WriteString(section, "Pogo", getEventName(IC_POGO, inputVal));
        Configuration.WriteString(section, "Fire", getEventName(IC_FIRE, inputVal));
        Configuration.WriteString(section, "Run", getEventName(IC_RUN, inputVal));
        Configuration.WriteString(section, "Status", getEventName(IC_STATUS, inputVal));
        Configuration.WriteString(section, "Camlead", getEventName(IC_CAMLEAD, inputVal));
        Configuration.WriteString(section, "Help", getEventName(IC_HELP, inputVal));
        Configuration.WriteString(section, "Back", getEventName(IC_BACK, inputVal));
        Configuration.WriteString(section, "Quicksave", getEventName(IC_QUICKSAVE, inputVal));
        Configuration.WriteString(section, "Quickload", getEventName(IC_QUICKLOAD, inputVal));

        // Optional tweaks
        Configuration.SetKeyword(section, "TwoButtonFiring", TwoButtonFiring[i]);
        Configuration.SetKeyword(section, "Analog", mAnalogAxesMovement[i]);
        Configuration.SetKeyword(section, "SuperPogo", mSuperPogo[i]);
        Configuration.SetKeyword(section, "ImpossiblePogo", mImpPogo[i]);
        Configuration.SetKeyword(section, "AutoFire", mFullyAutomatic[i]);
    }

    Configuration.saveCfgFile();

    gLogging << "Just saved the config file!" << CLogFile::endl;
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

    const auto &cmd = mInputCommands[input][command];
    const auto &evType = cmd.joyeventtype;

    switch(evType)
    {
    case (unsigned int)EType::JOYAXIS:
        buf = "J" + itoa(cmd.which) + "A" + itoa(cmd.joyaxis);
        if(mInputCommands[input][command].joyvalue < 0)
          buf += "-";
        else
          buf += "+";
        break;
    case (unsigned int)EType::JOYBUTTON:
        buf = "J" + itoa(cmd.which) + "B" + itoa(cmd.joybutton);
        break;
    case (unsigned int)EType::JOYHAT:
      buf = "J" + itoa(cmd.which) + "H" + itoa(cmd.joyhatval);
        break;
    case (unsigned int)EType::CTRLAXIS:
        buf = "C" + itoa(cmd.which) + "A" + itoa(cmd.ctlraxis);
        break;
    case (unsigned int)EType::CTRLBUTTON:
        buf = "C" + itoa(cmd.which) + "B" + itoa(cmd.ctlrbutton);
        break;
    case (unsigned int)EType::KEYBOARD:
        buf = SDL_GetKeyName(cmd.keysym);
        break;
    }

    return buf;
}


void CInput::render()
{
#ifdef USE_VIRTUALPAD
    if(!gVideoDriver.VGamePadEnabled())
        return;

    if(!mpVirtPad)
        return;

    if(!mpVirtPad->active())
        return;

    if(mVPadConfigState)
    {
        mpVirtPad->renderConfig();
    }


    GsWeakSurface blit(gVideoDriver.getBlitSurface());
    mpVirtPad->render(blit);
#endif

}

static std::string getEventNameOneCmd(const stInputCommand cmd)
{
    std::string buf;

    switch(cmd.joyeventtype)
    {
    case (unsigned int)EType::JOYAXIS:
        buf = "Joy" + itoa(cmd.which) + "-A" + itoa(cmd.joyaxis);
        if(cmd.joyvalue < 0)
          buf += "-";
        else
          buf += "+";
        break;
    case (unsigned int)EType::JOYBUTTON:
        buf = "Joy" + itoa(cmd.which) + "-B" + itoa(cmd.joybutton);
        break;
    case (unsigned int)EType::JOYHAT:
        buf = "Joy" + itoa(cmd.which) + "-H" + itoa(cmd.joyhatval);
        break;
    case (unsigned int)EType::KEYBOARD:
        buf = "Key ";
        buf += itoa(cmd.keysym);
        buf += " (";
        buf += SDL_GetKeyName(cmd.keysym);
        buf += ")";
        break;
    default:
        buf = "Unknown Ev";
        buf += itoa(cmd.joyeventtype);

        break;
    }

    return buf;
}

std::string CInput::getEventName(int command, unsigned char input)
{
    std::string buf;
    auto &curInput = mInputCommands[input];
    return getEventNameOneCmd(curInput[command]);
}

void CInput::setupInputCommand(std::array<stInputCommand, MAX_COMMANDS> &input,
                               int action, const std::string &string )
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
        input[action].which = atoi(buf2);
        buf = buf.substr(pos+1);
        buf2 = buf.substr(0,1);
        buf = buf.substr(1);

        if(buf2 == "A")
        {
            input[action].joyeventtype = (unsigned int)EType::JOYAXIS;
            pos = buf.size()-1;
            buf2 = buf.substr(0,pos);
            input[action].joyaxis = atoi(buf2);
            buf2 = buf.substr(pos);
            input[action].joyvalue = (buf2 == "+") ? +1 : -1;
        }
        else if(buf2 == "B")
        {
            input[action].joyeventtype = (unsigned int)EType::JOYBUTTON;
            input[action].joybutton = atoi(buf);
        }
        else // Should normally be H
        {
            input[action].joyeventtype = (unsigned int)EType::JOYHAT;
            input[action].joyhatval = atoi(buf);
        }
        return;
    }

    if(strCaseStartsWith(string, "Key"))
    {
        input[action].joyeventtype = (unsigned int)EType::KEYBOARD;
        buf = buf.substr(3);
        TrimSpaces(buf);
#if SDL_VERSION_ATLEAST(2, 0, 0)
        input[action].keysym = (SDL_Keycode) atoi(buf);
#else
        input[action].keysym = (SDLKey) atoi(buf);
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
    stInputCommand &readInput = mInputCommands[remapper.mapDevice][remapper.mapPosition];

    // This function is used to configure new input keys.
    // For iPhone, we have emulation via touchpad and we don't want to have custom keys.
    // We should fix the menu for iPhone so that this function doesn't get called.
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    printf("WARNING: called readNewEvent on iphone\n");
    return;
#endif

    readInput = stInputCommand();

    if(!m_EventList.empty())
        m_EventList.clear();

    const SDL_Keycode removeEvSym1 = SDLK_LALT;
    const SDL_Keycode removeEvSym2 = SDLK_BACKSPACE;

    while( SDL_PollEvent( &Event ) )
    {
        switch ( Event.type )
        {
            case SDL_QUIT:
                gLogging << "SDL: Got quit event in readNewEvent!" << CLogFile::endl;
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
                // on iPhone, we just want to quit in this case
                exit(0);
#endif
                break;

            case SDL_KEYDOWN:
                // Special for removing the assinged event
                if(mRemovalRunning && Event.key.keysym.sym == removeEvSym2)
                {
                    remapper.mappingInput = false;
                    mRemovalRunning = false;
                }
                else if(Event.key.keysym.sym == removeEvSym1)
                {
                    mRemovalRunning = true;
                    gLogging << "Removed "
                             << getEventNameOneCmd(readInput)
                             << CLogFile::endl;
                }
                else
                {
                    readInput.joyeventtype = (unsigned int)EType::KEYBOARD;
                    readInput.keysym = Event.key.keysym.sym;
                    gLogging << "Set "
                             << getEventNameOneCmd(readInput)
                             << CLogFile::endl;
                    remapper.mappingInput = false;
                    mRemovalRunning = false;
                }
            break;

            case SDL_JOYAXISMOTION:

            // Deadzone check. Double, because being a
            // new event to be read it should make better to configure
            if( (Event.jaxis.value > 2*m_joydeadzone ) ||
                (Event.jaxis.value < -2*m_joydeadzone ) )
            {
                readInput.joyeventtype = (unsigned int)EType::JOYAXIS;
                readInput.joyaxis = Event.jaxis.axis;
                readInput.which = mJoyIdToInputIdx[Event.jaxis.which];
                readInput.joyvalue = (Event.jaxis.value>0) ? 32767 : -32767;
                gLogging << "Set "
                         << getEventNameOneCmd(readInput)
                         << CLogFile::endl;
                remapper.mappingInput = false;
                mRemovalRunning = false;
            }
            break;

            case SDL_JOYHATMOTION:
                readInput.joyeventtype = (unsigned int)EType::JOYHAT;
                readInput.joyhatval = Event.jhat.value;
                readInput.which = mJoyIdToInputIdx[Event.jhat.which];
                gLogging << "Set "
                         << getEventNameOneCmd(readInput)
                         << CLogFile::endl;
                remapper.mappingInput = false;
                mRemovalRunning = false;
                break;

            case SDL_JOYBUTTONDOWN:
#if defined(CAANOO) || defined(WIZ) || defined(GP2X)
                WIZ_EmuKeyboard( Event.jbutton.button, 1 );
                return false;
#else
                readInput.joyeventtype = (unsigned int)EType::JOYBUTTON;
                readInput.joybutton = Event.jbutton.button;
                readInput.which = mJoyIdToInputIdx[Event.jbutton.which];
                gLogging << "Set "
                         << getEventNameOneCmd(readInput)
                         << CLogFile::endl;
                remapper.mappingInput = false;
#endif
                mRemovalRunning = false;
                break;

            case SDL_JOYDEVICEADDED:
                gLogging << "SDL: A Joystick just got added." << CLogFile::endl;;
                openJoyAndPrintStats(Event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                gLogging << "SDL: A Joystick just got removed." << CLogFile::endl;;

                auto joystick = SDL_JoystickFromInstanceID(Event.jdevice.which);
                mp_Joysticks.remove_if(
                            [joystick](SDL_Joystick* curPtr)
                              { return (curPtr == joystick); } );
                SDL_JoystickClose(joystick);

                break;
        }

        flushAll();
    }
}


void CInput::waitForAnyInput()
{
    float acc = 0.0f;
    float start = timerTicks();
    float elapsed = 0.0f;
    float curr = 0.0f;

    bool done = false;

    while(1)
    {
        const auto logicLatency = gTimer.LogicLatency();

        curr = timerTicks();

        if(gTimer.resetLogicSignal())
            start = curr;

        elapsed = curr - start;

        start = timerTicks();

        acc += elapsed;

        // Perform a pseudo game cycle
        while( acc > logicLatency )
        {
            if(getPressedAnyCommand())
            {
                done = true;
            }

            acc -= logicLatency;
        }

        if(done)
            break;


        elapsed = timerTicks() - start;

        const auto waitTime = int(logicLatency - elapsed);

        // wait time remaining in current loop
        if( waitTime > 0 )
        {
            timerDelay(waitTime);
        }

        // We must poll here, otherwise no new inputs will appear
        pollEvents();
    }

}


bool CInput::getTwoButtonFiring(const int player)
{ return TwoButtonFiring[player]; }
void CInput::setTwoButtonFiring(const int player, const bool value)
{ TwoButtonFiring[player]=value; }

bool CInput::isAnalog(const int player)
{ return mAnalogAxesMovement[player]; }
void CInput::enableAnalog(const int player, const bool value)
{ mAnalogAxesMovement[player]=value; }


GsRect<int> tiltBack(const GsRect<int> &screenRect,
                     const GsVec2D<int> &pt)
{
    const GsVec2D<int> rotPt(screenRect.dim.x/2, screenRect.dim.y/2);

    // Because tilt with 90 degree only works if the plane is squared
    // the coordinate must be transformed
    if(rotPt.x <= 0 || rotPt.y <= 0)
        return GsRect<int>(0,0,0,0);

    const auto rotTransX = rotPt.x*rotPt.y;
    const auto rotTransY = rotPt.y*rotPt.x;

    const auto x1_rel = (pt.x-rotPt.x)*rotPt.y;
    const auto y1_rel = (pt.y-rotPt.y)*rotPt.x;
    const int x2_rel = y1_rel;
    const int y2_rel = -x1_rel;

    auto retX = (x2_rel+rotTransX)/rotPt.y;
    auto retY = (y2_rel+rotTransY)/rotPt.x;

    return GsRect<int>( retX, retY,
                        screenRect.dim.x, screenRect.dim.y);
}

void CInput::transMouseRelCoord(GsVec2D<float> &Pos,
                                const GsVec2D<int> pointer,
                                const GsRect<Uint16> &activeArea,
                                const bool tiltedScreen)
{
    auto myCoord = pointer;

    if(tiltedScreen)
    {
        const auto tiltRect = tiltBack(activeArea, myCoord);
        myCoord = tiltRect.pos;
    }

    Pos.x = ( static_cast<float>(myCoord.x-activeArea.pos.x)/
              static_cast<float>(activeArea.dim.x) );
    Pos.y = ( static_cast<float>(myCoord.y-activeArea.pos.y)/
              static_cast<float>(activeArea.dim.y) );

}


void CInput::ponder()
{
#ifdef USE_VIRTUALPAD
    if(!mpVirtPad)
        return;

    if(!mpVirtPad->active())
        return;

    mpVirtPad->ponder();
#endif
}



void CInput::flushFingerEvents()
{
#ifdef USE_VIRTUALPAD
    if(!mpVirtPad)
        return;

    mpVirtPad->flush();
#endif
}


void CInput::pollEvents()
{
    // Semaphore
    SDL_SemWait( mpPollSem );

#ifdef USE_VIRTUALPAD
    if(mpVirtPad && mVPadConfigState)
    {
        mpVirtPad->processConfig();
    }
#endif

    if(remapper.mappingInput)
    {
        readNewEvent();
        SDL_SemPost( mpPollSem );
        return;
    }

    GsVec2D<float> Pos;
#if SDL_VERSION_ATLEAST(2, 0, 0)

#else
    GsRect<Uint16> Res(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
#endif

    // copy all the input of the last poll to a space for checking pressing or holding a button
    last_immediate_keytable = immediate_keytable;

    // Input for player commands
    for(unsigned int j=0 ; j<mInputCommands.size() ; j++)
    {
        auto &input = mInputCommands[j];
        for(unsigned int i=0 ; i<input.size() ; i++)
        {
            input[i].lastactive = input[i].active;
        }
    }


    GsRect<Uint16> activeArea = gVideoDriver.mpVideoEngine->getActiveAreaRect();

    auto &dispRect = gVideoDriver.getVidConfig().mDisplayRect;

    const bool tiltedScreen = gVideoDriver.getVidConfig().mTiltedScreen;

    SDL_Joystick *joystick = nullptr;

    // While there's an event to handle
    while( SDL_PollEvent( &Event ) )
    {
        bool passSDLEventVec = true;

        switch( Event.type )
        {
        case SDL_QUIT:
            gLogging << "SDL: Got quit event!" << CLogFile::endl;
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
        case SDL_JOYHATMOTION:
            passSDLEventVec = true;
            processJoystickHat();
            break;
        case SDL_JOYBUTTONDOWN:
            passSDLEventVec = true;
            processJoystickButton(1);
            break;
        case SDL_JOYBUTTONUP:
            passSDLEventVec = true;
            processJoystickButton(0);
            break;

        case SDL_JOYDEVICEADDED:
            gLogging.ftextOut("SDL: A Joystick just got added: Idx %d<br>\n", Event.jdevice.which);
            openJoyAndPrintStats(Event.jdevice.which);
            break;
        case SDL_JOYDEVICEREMOVED:
            gLogging.ftextOut("SDL: A Joystick just got removed: Idx %d<br>\n", Event.jdevice.which);
            joystick = SDL_JoystickFromInstanceID(Event.jdevice.which);
            mp_Joysticks.remove_if(
                        [joystick](SDL_Joystick* curPtr)
                          { return (curPtr == joystick); } );
            SDL_JoystickClose(joystick);

            break;

#if SDL_VERSION_ATLEAST(2, 0, 0)
        case SDL_FINGERDOWN:
        {
#ifdef USE_VIRTUALPAD
            // If Virtual gamepad takes control...
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
               mpVirtPad->active() )
            {
                GsVec2D<int> rotPt(Event.tfinger.x*float(activeArea.dim.x),
                                   Event.tfinger.y*float(activeArea.dim.y));

                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);

                if(!mpVirtPad->mouseFingerState(Pos, true, Event.tfinger, true))
                {
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                    gPointDevice.mPointingState.mActionButton = 1;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
#endif
            {
                const GsVec2D<int> rotPt(Event.tfinger.x*float(activeArea.dim.x),
                                         Event.tfinger.y*float(activeArea.dim.y));
                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);
                m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                gPointDevice.mPointingState.mActionButton = 1;
                gPointDevice.mPointingState.mPos = Pos;
            }
        }
        break;

        case SDL_FINGERUP:

#ifdef USE_VIRTUALPAD
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
               mpVirtPad->active())
            {
                GsVec2D<int> rotPt(Event.tfinger.x*float(activeArea.dim.x),
                                   Event.tfinger.y*float(activeArea.dim.y));

                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);
                if(!mpVirtPad->mouseFingerState(Pos, true, Event.tfinger, false))
                {
                    passSDLEventVec = true;
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONUP ) );
                    gPointDevice.mPointingState.mActionButton = 0;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
#endif
            {
                passSDLEventVec = true;

                GsVec2D<int> rotPt(Event.tfinger.x*float(activeArea.dim.x),
                                   Event.tfinger.y*float(activeArea.dim.y));

                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);
                m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONUP ) );
                gPointDevice.mPointingState.mActionButton = 0;
                gPointDevice.mPointingState.mPos = Pos;
            }
        break;


        case SDL_FINGERMOTION:
        {
            GsVec2D<int> rotPt(Event.tfinger.x*float(activeArea.dim.x),
                               Event.tfinger.y*float(activeArea.dim.y));

            // If Virtual gamepad takes control...
#ifdef USE_VIRTUALPAD
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
               mpVirtPad->active() )
            {
                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);

                if(!mpVirtPad->mouseFingerState(Pos, true, Event.tfinger, true))
                {
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                    gPointDevice.mPointingState.mActionButton = 1;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
#endif
            {
                const GsVec2D<int> rotPt(Event.tfinger.x, Event.tfinger.y);
                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);
                m_EventList.add(new PointingDevEvent(Pos, PDE_MOVED));
                gPointDevice.mPointingState.mPos = Pos;

                //processMouse(Event);
            }
        }
        break;

        case SDL_WINDOWEVENT:
            if(Event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                gVideoDriver.mpVideoEngine->resizeDisplayScreen(
                        GsRect<Uint16>(Event.window.data1,
                                       Event.window.data2) );
                dispRect.dim.x = Event.window.data1;
                dispRect.dim.y = Event.window.data2;
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

#ifndef ANDROID
        case SDL_MOUSEBUTTONDOWN:

            if(Event.button.button <= 3)
            {
#ifdef USE_VIRTUALPAD
                // If Virtual gamepad takes control...
                if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
                   mpVirtPad->active() )
                {
                    const GsVec2D<int> rotPt(Event.motion.x, Event.motion.y);
                    transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);

                    if(!mpVirtPad->mouseDown(Pos))
                    {
                        m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                        gPointDevice.mPointingState.mActionButton = 1;
                        gPointDevice.mPointingState.mPos = Pos;
                    }
                }
                else
#endif
                {
                    const GsVec2D<int> rotPt(Event.motion.x, Event.motion.y);
                    transMouseRelCoord(Pos, Event.motion, activeArea, tiltedScreen);
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                    gPointDevice.mPointingState.mActionButton = 1;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }

            break;

        case SDL_MOUSEBUTTONUP:

#ifdef USE_VIRTUALPAD
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
                    mpVirtPad->active())
            {
                const GsVec2D<int> rotPt(Event.motion.x, Event.motion.y);
                transMouseRelCoord(Pos, rotPt, activeArea, tiltedScreen);
                if(!mpVirtPad->mouseUp(Pos))
                {
                    passSDLEventVec = true;
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONUP ) );
                    gPointDevice.mPointingState.mActionButton = 0;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
#endif
            {
                passSDLEventVec = true;
                const GsVec2D<int> rotPt(Event.motion.x, Event.motion.y);
                transMouseRelCoord(Pos, Event.motion, activeArea, tiltedScreen);
                m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONUP ) );
                gPointDevice.mPointingState.mActionButton = 0;
                gPointDevice.mPointingState.mPos = Pos;
            }

            break;

#if SDL_VERSION_ATLEAST(2, 0, 0)
        case SDL_MOUSEWHEEL:
            gEventManager.add( new MouseWheelEvent( GsVec2D<float>(float(Event.wheel.x),
                                                                    float(Event.wheel.y)) ) );
            break;
#endif

        case SDL_MOUSEMOTION:

            const GsVec2D<int> rotPt(Event.motion.x, Event.motion.y);
            transMouseRelCoord(Pos, Event.motion, activeArea, tiltedScreen);

#ifdef USE_VIRTUALPAD
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
                    mpVirtPad->active())
            {
                bool ok;

                if(Event.motion.state & SDL_BUTTON_LMASK)
                    ok = mpVirtPad->mouseDown(Pos);
                else
                    ok = mpVirtPad->mouseUp(Pos);

                if(!ok)
                {
                    passSDLEventVec = true;
                    m_EventList.add( new PointingDevEvent( Pos, PDE_MOVED ) );
                    gPointDevice.mPointingState.mActionButton = 0;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
#endif
            {
                passSDLEventVec = true;
                m_EventList.add( new PointingDevEvent( Pos, PDE_MOVED ) );
                gPointDevice.mPointingState.mActionButton = Event.motion.state & SDL_BUTTON_LMASK;
                gPointDevice.mPointingState.mPos = Pos;
                break;
            }

#endif
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
    {
        for(int j=0 ; j<NUM_INPUTS ; j++)
        {
            auto &input = mInputCommands[j];

            input[i].firsttimeactive =
                      !input[i].lastactive &&
                             input[i].active;
        }
    }

#ifndef MOUSEWRAPPER

    // TODO: I'm not sure, if that should go here...
    // Check, if LALT+ENTER was pressed
    if((getHoldedKey(KALT)) && getPressedKey(KENTER))
    {
        bool value;
        value = gVideoDriver.getFullscreen();
        value = !value;
        gLogging.textOut(FONTCOLORS::GREEN,"Fullscreen mode triggered by user!<br>");
        gVideoDriver.isFullscreen(value);

        // initialize/activate all drivers
        gLogging.ftextOut("Restarting graphics driver...<br>");
        if ( gVideoDriver.applyMode() && gVideoDriver.start() )
        {
            gLogging.ftextOut(FONTCOLORS::PURPLE, "Toggled Fullscreen quick shortcut...<br>");
        }
        else
        {
            value = !value;
            gLogging.ftextOut(FONTCOLORS::PURPLE, "Couldn't change the resolution, Rolling back...<br>");
            gVideoDriver.applyMode();
            gVideoDriver.start();
        }

        gInput.flushAll();
    }

    // Check, if LALT+Q or LALT+F4 was pressed
    if(getHoldedKey(KALT) && (getPressedKey(KF4) || getPressedKey(KQ)) )
    {
        gLogging << "User exit request!";
        m_exit = true;
    }
#endif

#if defined(WIZ) || defined(GP2X)
    WIZ_AdjustVolume( volume_direction );
#endif

    SDL_SemPost( mpPollSem );
}

/**
 * \brief	This will tell if any joystick axes haven been moved and if they triggered a command by doing so...
 */
void CInput::processJoystickAxis(void)
{
    const auto evWhich = mJoyIdToInputIdx[Event.jaxis.which];

    // Input for player commands
    for(auto &input : mInputCommands)
    {
        for(auto &curInput : input)
        {
            // Only axis types
            if(curInput.joyeventtype != (unsigned int)EType::JOYAXIS)
                continue;

            // Only the right device id
            if( evWhich != curInput.which )
                continue;

            // It must match to mapped axis movement
            if( Event.jaxis.axis != curInput.joyaxis )
                continue;


            // Also respect the Deadzone
            if((Event.jaxis.value > m_joydeadzone &&
                curInput.joyvalue > 0) ||
               (Event.jaxis.value < -m_joydeadzone &&
                curInput.joyvalue < 0))
            {
                curInput.active = true;
                curInput.joymotion = Event.jaxis.value;
            }
            else
            {
                curInput.active = false;
            }
        }
    }
}

void CInput::processJoystickHat()
{
    const auto evWhich = mJoyIdToInputIdx[Event.jhat.which];

    for(int j=0 ; j<NUM_INPUTS ; j++)
    {
        auto &input = mInputCommands[j];
        for(int i=0 ; i<MAX_COMMANDS ; i++)
        {
            stInputCommand &command = input[i];

            if( command.joyeventtype == (unsigned int)EType::JOYHAT &&
                command.which ==  evWhich)
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
    const auto evWhich = mJoyIdToInputIdx[Event.jbutton.which];

    for( auto &inputCommand : mInputCommands)
    {
        auto &inputs = inputCommand;
        for( auto &input : inputs)
        {
            if(input.joyeventtype == (unsigned int)EType::JOYBUTTON)
            {
                // Joystick buttons are configured for this event
                if(evWhich == input.which &&
                   Event.jbutton.button == input.joybutton )
                {
                    input.active = value;
                }
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
    for( auto &input : mInputCommands )
    {
        for(decltype(input.size()) i=0 ;
            i<input.size() ; i++)
        {
            if(input[i].keysym == Event.key.keysym.sym &&
                    input[i].joyeventtype == (unsigned int)EType::KEYBOARD)
            {
                input[i].active = (keydown) ? true : false;

                if(i == IC_BACK)
                {
                    passSDLEventVec = false;
                }
            }
        }
    }

    // Detect Key modifier
    switch(Event.key.keysym.scancode)
    {
    case SDL_SCANCODE_LSHIFT: immediate_keytable[KSHIFT] = keydown;
    default: break;
    }

    // ... and for general keys
    if(Event.key.keysym.sym < 160)
    {
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
    }
    else if(Event.key.keysym.sym < immediate_keytable.size())
    {
        immediate_keytable[Event.key.keysym.sym] = keydown;
    }
    else // For some special keys
    {
        if(Event.key.keysym.scancode == SDL_SCANCODE_F10)
        {
            immediate_keytable[KF10] = keydown;
        }
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

    for(i=160 ; i<256 ; i++)
    {
        if(getPressedKey(i))
        {
            buf = i;
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
        for(i=160 ; i<256 ; i++)
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
    auto &input = mInputCommands[player];
    return (input[command].joyeventtype == (unsigned int)EType::JOYAXIS);
}

bool CInput::getHoldedCommand(int player, int command)
{
    auto &input = mInputCommands[player];
    return input[command].active;
}

int CInput::getJoyValue(const int player,
                        const int command,
                        const bool negative)
{
    auto &input = mInputCommands[player];
    int newval = input[command].joymotion;
    newval = (newval*101)>>15;

    newval = newval<0 ? -newval : newval;

    if( newval > 100 )
        newval = 100;

    newval = negative ? -newval : newval;
    return newval;
}


bool CInput::getPressedCommand(int command)
{
    bool retval = false;
    for(int player=0; player<NUM_INPUTS ; player++ )
    {
        retval |= getPressedCommand(player, command);
    }
    return retval;
}

bool CInput::getPressedCommand(int player, int command)
{
    auto &inputFromPlayer = mInputCommands[player];

    if(inputFromPlayer[command].firsttimeactive)
    {
        inputFromPlayer[command].firsttimeactive = false;
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
    auto &input = mInputCommands[player];

    if(input[command].active)
    {
        bool value = true;

        if(m_cmdpulse % msec != 0)
        {
            value = false;
        }
        m_cmdpulse++;
        return value;
    }
    if(!input[command].active &&
            input[command].lastactive )
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

bool CInput::getPressedAnyButtonCommand()
{
    bool retval = false;
    for(int player=0 ; player<NUM_INPUTS ; player++)
        retval |= getPressedAnyButtonCommand(player);
    return retval;
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
    auto &input = mInputCommands[player];

    input[command].active =
    input[command].lastactive =
    input[command].firsttimeactive = false;
}



/**
 * \brief	this will forget every key that was typed before
 */
void CInput::flushKeys(void)
{
    immediate_keytable.fill(false);
    last_immediate_keytable.fill(false);
    firsttime_immediate_keytable.fill(false);
}

/**
  * @brief flushEvent This will clear the whole event list
  */
void CInput::flushEvents()
{
    m_EventList.clear();
}

#define KSHOWHIDECTRLS	(-10)

#if SDL_VERSION_ATLEAST(2, 0, 0)

#if defined(MOUSEWRAPPER)
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
static const int w = 480, h = 320;
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
        { &InputCommand[0][IC_RUN],         KSHIFT,	w*5/6,  h*2/3,  w / 6,  h / 3},

        { &InputCommand[0][IC_BACK],        KQUIT,	0,      0,      w / 6,  h / 6},
        { &InputCommand[0][IC_STATUS],      KENTER, 5*w/6,  h / 6,  w / 6,  h / 6},
        { &InputCommand[0][IC_HELP],        KF1,	0,      h / 6,  w / 6,  h / 6},
        { NULL,                     KSHOWHIDECTRLS,	5*w/6,  0,      w / 6,  h / 6},
    };

    return phoneButtons;
}
#endif
#endif


typedef std::set<int> MouseIndexSet;

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
static const int phoneButtonN = 15;
static Uint32 phoneButtonLasttime[phoneButtonN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static MouseIndexSet phoneButton_MouseIndex[phoneButtonN];


static TouchButton* getPhoneButton(int x, int y, TouchButton phoneButtons[]) {
    for(int i = 0; i < phoneButtonN; ++i) {
        TouchButton& b = phoneButtons[i];
        if(b.isInside(x, y)) return &b;
    }
    return NULL;
}
#endif

#ifdef MOUSEWRAPPER
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
#endif

#ifdef MOUSEWRAPPER
void CInput::processMouse()
{
    /*
    TouchButton* phoneButtons = getPhoneButtons(mInputCommands);

    for(int i = 0; i < phoneButtonN; ++i) {
        bool down = phoneButton_MouseIndex[i].size() > 0;

        TouchButton& b = phoneButtons[i];

        if(b.cmd)
            b.cmd->active = down;

        // handle immediate keys
        if(b.immediateIndex >= 0)
            immediate_keytable[b.immediateIndex] = down;
    }*/
}
#endif

void CInput::processMouse(SDL_Event& ev) {

    SDL_Rect screenRect;
    //SDL_Touch* touch = SDL_GetTouch(ev.tfinger.touchId);
    SDL_Finger* touch = SDL_GetTouchFinger(ev.tfinger.touchId, 0);
    int x, y, dx, dy, w, h;

    if(SDL_GetDisplayBounds(0, &screenRect) == 0) {
        w = screenRect.w;
        h = screenRect.h;
    }

    if(touch == nullptr) return; //The touch has been removed

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
            float fdx = (float(ev.tfinger.dx))/touch->x;
            float fdy = (float(ev.tfinger.dy))/touch->y;
            dx = int(fdx*w); dy = int(fdy*h);
            processMouse(x - dx, y - dy, false, ev.tfinger.fingerId);
            processMouse(x, y, true, ev.tfinger.fingerId);
            break;
    }
}

void CInput::processMouse(int, int, bool, int)
{
    /*
    const GsRect<int> pt(x,y);

    if(down)
        {
            // If Virtual gamepad takes control...
            if(gVideoDriver.VGamePadEnabled() && mpVirtPad &&
               mpVirtPad->active() )
            {
                transMouseRelCoord(Pos, pt, activeArea, tiltedScreen);

                if(!mpVirtPad->mouseDown(Pos))
                {
                    m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                    gPointDevice.mPointingState.mActionButton = 1;
                    gPointDevice.mPointingState.mPos = Pos;
                }
            }
            else
            {
                transMouseRelCoord(Pos, pt, activeArea, tiltedScreen);
                m_EventList.add( new PointingDevEvent( Pos, PDE_BUTTONDOWN ) );
                gPointDevice.mPointingState.mActionButton = 1;
                gPointDevice.mPointingState.mPos = Pos;
            }
        }
    else
    {
    if (gVideoDriver.VGamePadEnabled() && mpVirtPad &&
            mpVirtPad->active()) {
            transMouseRelCoord(Pos, pt, activeArea, tiltedScreen);
            if (!mpVirtPad->mouseUp(Pos)) {
                passSDLEventVec = true;
                m_EventList.add(new PointingDevEvent(Pos, PDE_BUTTONUP));
                gPointDevice.mPointingState.mActionButton = 0;
                gPointDevice.mPointingState.mPos = Pos;
            }
        } else {
            passSDLEventVec = true;
            transMouseRelCoord(Pos, pt, activeArea, tiltedScreen);
            m_EventList.add(new PointingDevEvent(Pos, PDE_BUTTONUP));
            gPointDevice.mPointingState.mActionButton = 0;
            gPointDevice.mPointingState.mPos = Pos;
        }

    }*/

#ifdef MOUSEWRAPPER
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
#endif
}


#ifdef USE_OPENGL
/*
static void drawButton(TouchButton& button, bool down) {
    // similar mysterious constant as in renderTexture/initGL
    //glViewport(0,255,w,h);

    float w = 480.0f, h = 320.0f;

    int crop = 2;
    float x1 = float(button.x + crop) / w;
    float x2 = float(button.x+button.dim.x - crop) / w;
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
*/
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
    SDL_SemWait( mpPollSem );

    if(mSDLEventVec.empty())
    {
        SDL_SemPost( mpPollSem );
        return false;
    }

    evVec = mSDLEventVec;

    mSDLEventVec.clear();

    SDL_SemPost( mpPollSem );

    return true;
}

void CInput::pushBackButtonEventExtEng()
{
    SDL_SemWait( mpPollSem );

    if(mBackEventBuffer.empty())
    {
        SDL_SemPost( mpPollSem );
        return;
    }

    // Take one event and send a down and up event
    if(!mBackEventBuffer.empty())
    {
        SDL_Event ev = mBackEventBuffer.front();
        ev.type = SDL_KEYDOWN;
        mSDLEventVec.push_back(ev);
        ev.type = SDL_KEYUP;
        mSDLEventVec.push_back(ev);
    }

    mBackEventBuffer.clear();

    SDL_SemPost( mpPollSem );
}

