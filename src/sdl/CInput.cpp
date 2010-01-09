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
#include "../CLogFile.h"
#include "../FindFile.h"

#if defined(WIZ) || defined(GP2X)
#include "sys/wizgp2x.h"
#endif

CInput::CInput() {
#if defined(WIZ) || defined(GP2X)
    volume_direction = VOLUME_NOCHG;
	volume = 60-VOLUME_CHANGE_RATE;
	WIZ_AdjustVolume(VOLUME_UP);
#endif
	g_pLogFile->ftextOut("Starting the input driver...<br>");
	memset(InputCommand, 0, NUM_INPUTS*NUMBER_OF_COMMANDS*sizeof(stInputCommand));

	for(size_t c=1 ; c<= NUM_INPUTS ; c++)
		resetControls(c);
	memset(&Event,0,sizeof(Event));
	loadControlconfig();
	startJoyDriver();
}

void CInput::resetControls(int player) {
	int i;

	if(player == 0)
	{
		player = 1;
		g_pLogFile->textOut("Error when resetting controls. The function has been used incorrectly");
	}
	// not a good idea, beause it would write twice in one array, and forget about the last one. (for example 4)
	// At least this warning will tell the people, that something is not right here!

	m_exit = false;
	m_cmdpulse = 0;
	m_joydeadzone = 16384;

	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);

	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		InputCommand[player][i].active = false;
	}

	// These are the default keyboard commands
	i=player-1;
	InputCommand[i][IC_LEFT].keysym = SDLK_LEFT;
	InputCommand[i][IC_UP].keysym = SDLK_UP;
	InputCommand[i][IC_RIGHT].keysym = SDLK_RIGHT;
	InputCommand[i][IC_DOWN].keysym = SDLK_DOWN;

	InputCommand[i][IC_JUMP].keysym = SDLK_LCTRL;
	InputCommand[i][IC_POGO].keysym = SDLK_LALT;
	InputCommand[i][IC_FIRE].keysym = SDLK_SPACE;
	InputCommand[i][IC_STATUS].keysym = SDLK_RETURN;

	InputCommand[i][IC_HELP].keysym = SDLK_F1;
	InputCommand[i][IC_QUIT].keysym = SDLK_ESCAPE;

	// And those are the default joystick handlings
	InputCommand[i][IC_LEFT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_LEFT].joyaxis = 0;
	InputCommand[i][IC_LEFT].joyvalue = -32767;
	InputCommand[i][IC_LEFT].which = 0;
	InputCommand[i][IC_UP].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_UP].joyaxis = 1;
	InputCommand[i][IC_UP].joyvalue = -32767;
	InputCommand[i][IC_UP].which = 0;
	InputCommand[i][IC_RIGHT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_RIGHT].joyaxis = 0;
	InputCommand[i][IC_RIGHT].joyvalue = 32767;
	InputCommand[i][IC_RIGHT].which = 0;
	InputCommand[i][IC_DOWN].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[i][IC_DOWN].joyaxis = 1;
	InputCommand[i][IC_DOWN].joyvalue = 32767;
	InputCommand[i][IC_DOWN].which = 0;

	InputCommand[i][IC_JUMP].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_JUMP].joybutton = 0;
	InputCommand[i][IC_JUMP].which = 0;
	InputCommand[i][IC_POGO].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_POGO].joybutton = 1;
	InputCommand[i][IC_POGO].which = 0;
	InputCommand[i][IC_FIRE].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_FIRE].joybutton = 2;
	InputCommand[i][IC_FIRE].which = 0;
	InputCommand[i][IC_STATUS].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_STATUS].joybutton = 3;
	InputCommand[i][IC_STATUS].which = 0;
	InputCommand[i][IC_HELP].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_HELP].joybutton = 4;
	InputCommand[i][IC_HELP].which = 0;
	InputCommand[i][IC_QUIT].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[i][IC_QUIT].joybutton = 5;
	InputCommand[i][IC_QUIT].which = 0;
	setTwoButtonFiring(i, false);
}

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
		int joynum;
		joynum = SDL_NumJoysticks();
		if(joynum)
		{
			if(joynum == 1)
				g_pLogFile->ftextOut("1 joystick was found.<br>\n", joynum );
			else
				g_pLogFile->ftextOut("%i joysticks were found.<br>\n", joynum );
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

short CInput::loadControlconfig(void)
{
	FILE *fp;
	if((fp=OpenGameFile("controls.dat","rb")) != NULL)
	{
		bool ok=true;
		ok &= (fread(InputCommand, sizeof(stInputCommand),NUMBER_OF_COMMANDS*NUM_INPUTS, fp) != 0);
		fread(TwoButtonFiring, sizeof(bool),NUM_INPUTS, fp); // This one won't be checked, in order to conserve compatibility
		if( !ok )
		{
			fclose(fp);
			return 1;
		}
		fclose(fp);
		return 0;
	}
	return 1;
}

short CInput::saveControlconfig(void)
{
	FILE *fp;
	if((fp=OpenGameFile("controls.dat","wb")) != NULL)
	{
		fwrite(InputCommand, sizeof(stInputCommand),NUMBER_OF_COMMANDS*NUM_INPUTS, fp);
		fwrite(TwoButtonFiring, sizeof(bool),NUM_INPUTS, fp);
		fclose(fp);
		return 0;
	}

	return 1;
}
void CInput::getEventName(int position, unsigned char input, std::string &buf)
{
	buf = SDL_GetKeyName(InputCommand[input][position].keysym);
	if(InputCommand[input][position].joyeventtype == ETYPE_JOYAXIS)
	{
		buf += ", ";
		buf += "Joy" + itoa(InputCommand[input][position].which) + "-A" + itoa(InputCommand[input][position].joyaxis);
		if(InputCommand[input][position].joyvalue < 0)
			buf += "-";
		else
			buf += "+";
	}
	if(InputCommand[input][position].joyeventtype == ETYPE_JOYBUTTON)
	{
		buf += ", ";
		buf += "Joy" + itoa(InputCommand[input][position].which) + "-B" + itoa(InputCommand[input][position].joybutton);
	}
}

bool CInput::readNewEvent(Uint8 device, int position)
{
	while( SDL_PollEvent( &Event ) )
	{
		switch ( Event.type )
		{
			case SDL_KEYDOWN:
				InputCommand[device][position].keysym = Event.key.keysym.sym;
				return true;
				break;
			case SDL_JOYBUTTONDOWN:
#if defined(WIZ) || defined(GP2X)
				WIZ_EmuKeyboard( Event.jbutton.button, 1 );
				return false;
#else
				InputCommand[device][position].joyeventtype = ETYPE_JOYBUTTON;
				InputCommand[device][position].joybutton = Event.jbutton.button;
				InputCommand[device][position].which = Event.jbutton.which;
				return true;
#endif
				break;
			case SDL_JOYAXISMOTION:
				InputCommand[device][position].joyeventtype = ETYPE_JOYAXIS;
				InputCommand[device][position].joyaxis = Event.jaxis.axis;
				InputCommand[device][position].which = Event.jaxis.which;
				InputCommand[device][position].joyvalue = (Event.jaxis.value>0) ? 32767 : -32767;
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

void CInput::pollEvents()
{
	// copy all the input of the last poll to a space for checking pressing or holding a button
	memcpy(last_immediate_keytable, immediate_keytable, KEYTABLE_SIZE*sizeof(char));

	for(int i=0 ; i<NUMBER_OF_COMMANDS ; i++)
		for(int j=0 ; j<NUM_INPUTS ; j++)
			InputCommand[j][i].lastactive = InputCommand[j][i].active;

	//While there's an event to handle
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
		}
	}

	// Check, if LALT+ENTER was pressed
	if((getHoldedKey(KALT)) && getPressedKey(KENTER))
	{
		bool value;
		value = g_pVideoDriver->getFullscreen();
		value = !value;
		g_pLogFile->textOut(GREEN,"Fullscreen mode triggered by user!<br>");
		g_pVideoDriver->isFullscreen(value);

		// initialize/activate all drivers
		g_pLogFile->ftextOut("Restarting graphics driver...<br>");
		if (!g_pVideoDriver->applyMode())
		{
			value = !value;
			g_pLogFile->ftextOut(PURPLE, "Couldn't change the resolution, Rolling back...<br>");
			if(g_pVideoDriver->applyMode()) g_pVideoDriver->initOpenGL();
		}
		else
			g_pVideoDriver->initOpenGL();


		if(value) g_pVideoDriver->AddConsoleMsg("Fullscreen enabled");
		else g_pVideoDriver->AddConsoleMsg("Fullscreen disabled");
	}

	// Check, if LALT+Q or LALT+F4 was pressed
	if(getHoldedKey(KALT) && (getPressedKey(KF4) || getPressedKey(KQ)) )
	{
		g_pLogFile->textOut("User exit request!");
		m_exit = true;
	}


#if defined(WIZ) || defined(GP2X)
	WIZ_AdjustVolume( volume_direction );
#endif
}

void CInput::processJoystickAxis(void)
{
	for(int j=0 ; j<NUM_INPUTS ; j++)
	{
		for(int i=0 ; i<NUMBER_OF_COMMANDS ; i++)
		{
			if(InputCommand[j][i].joyeventtype == ETYPE_JOYAXIS)
			{
				// Axis are configured for this commmand
				if(Event.jaxis.axis == InputCommand[j][i].joyaxis && Event.jaxis.which == InputCommand[j][i].which )
				{
					// Deadzone
					if((Event.jaxis.value > m_joydeadzone && InputCommand[0][i].joyvalue > 0) ||
					   (Event.jaxis.value < -m_joydeadzone && InputCommand[0][i].joyvalue < 0))
						InputCommand[j][i].active = true;
					else
						InputCommand[j][i].active = false;
				}
			}
		}
	}
}
void CInput::processJoystickButton(int value)
{
#if defined(WIZ) || defined(GP2X)
	WIZ_EmuKeyboard( Event.jbutton.button, value );
#else
	for(int j=0 ; j<NUM_INPUTS ; j++)
	{
		for(int i=0 ; i<NUMBER_OF_COMMANDS ; i++)
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

void CInput::sendKey(int key){	immediate_keytable[key] = true;	}

void CInput::processKeys(int value)
{
	for(int i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		for(int j=0 ; j<NUM_INPUTS ; j++)
		{
			if(InputCommand[j][i].keysym == Event.key.keysym.sym)
				InputCommand[j][i].active = (value) ? true : false;
		}
	}

    switch(Event.key.keysym.sym)
	{
			// These are only used for ingame stuff or menu, but not for controlling the player anymore
		case SDLK_LEFT: 	immediate_keytable[KLEFT]	= value;  break;
		case SDLK_UP:	immediate_keytable[KUP]		= value;  break;
		case SDLK_RIGHT:	immediate_keytable[KRIGHT]	= value;  break;
		case SDLK_DOWN:	immediate_keytable[KDOWN]	= value;  break;

			// Page Keys
		case SDLK_PAGEUP:	immediate_keytable[KPGUP]	= value;  break;
		case SDLK_PAGEDOWN:	immediate_keytable[KPGDN]		= value;  break;

		case SDLK_RETURN:immediate_keytable[KENTER]	= value;  break;
		case SDLK_RCTRL:immediate_keytable[KCTRL]	= value;  break;
		case SDLK_LCTRL:immediate_keytable[KCTRL]	= value;  break;
		case SDLK_SPACE:immediate_keytable[KSPACE]	= value;  break;
		case SDLK_RALT:immediate_keytable[KALT]		= value;  break;
		case SDLK_LALT:immediate_keytable[KALT]		= value;  break;
		case SDLK_TAB:immediate_keytable[KTAB]		= value;  break;
		case SDLK_LSHIFT:immediate_keytable[KSHIFT]	= value;  break;
		case SDLK_RSHIFT:immediate_keytable[KSHIFT]	= value;  break;
		case SDLK_ESCAPE:immediate_keytable[KQUIT]	= value;  break;

		case SDLK_BACKSPACE:immediate_keytable[KBCKSPCE] = value; break;

		case SDLK_QUOTE:immediate_keytable[KQUOTE]	= value;  break;
		case SDLK_COMMA:immediate_keytable[KCOMMA]	= value;  break;
		case SDLK_PERIOD:immediate_keytable[KPERIOD]	= value;  break;
		case SDLK_SLASH:immediate_keytable[KSLASH]	= value;  break;
		case SDLK_SEMICOLON:immediate_keytable[KSEMICOLON]	= value;  break;
		case SDLK_EQUALS:immediate_keytable[KEQUAL]	= value;  break;
		case SDLK_LEFTBRACKET:immediate_keytable[KLEFTBRACKET]	= value;  break;
		case SDLK_BACKSLASH:immediate_keytable[KBACKSLASH]	= value;  break;
		case SDLK_RIGHTBRACKET:immediate_keytable[KRIGHTBRACKET]	= value;  break;
		case SDLK_BACKQUOTE:immediate_keytable[KBACKQUOTE]	= value;  break;

		case SDLK_a:immediate_keytable[KA]	= value;  break;
		case SDLK_b:immediate_keytable[KB]	= value;  break;
		case SDLK_c:immediate_keytable[KC]	= value;  break;
		case SDLK_d:immediate_keytable[KD]	= value;  break;
		case SDLK_e:immediate_keytable[KE]	= value;  break;
		case SDLK_f:immediate_keytable[KF]	= value;  break;
		case SDLK_g:immediate_keytable[KG]	= value;  break;
		case SDLK_h:immediate_keytable[KH]	= value;  break;
		case SDLK_i:immediate_keytable[KI]	= value;  break;
		case SDLK_j:immediate_keytable[KJ]	= value;  break;
		case SDLK_k:immediate_keytable[KK]	= value;  break;
		case SDLK_l:immediate_keytable[KL]	= value;  break;
		case SDLK_m:immediate_keytable[KM]	= value;  break;
		case SDLK_n:immediate_keytable[KN]	= value;  break;
		case SDLK_o:immediate_keytable[KO]	= value;  break;
		case SDLK_p:immediate_keytable[KP]	= value;  break;
		case SDLK_q:immediate_keytable[KQ]	= value;  break;
		case SDLK_r:immediate_keytable[KR]	= value;  break;
		case SDLK_s:immediate_keytable[KS]	= value;  break;
		case SDLK_t:immediate_keytable[KT]	= value;  break;
		case SDLK_u:immediate_keytable[KU]	= value;  break;
		case SDLK_v:immediate_keytable[KV]	= value;  break;
		case SDLK_w:immediate_keytable[KW]	= value;  break;
		case SDLK_x:immediate_keytable[KX]	= value;  break;
		case SDLK_y:immediate_keytable[KY]	= value;  break;
		case SDLK_z:immediate_keytable[KZ]	= value;  break;

		case SDLK_F1:immediate_keytable[KF1]	= value;  break;
		case SDLK_F2:immediate_keytable[KF2]	= value;  break;
		case SDLK_F3:immediate_keytable[KF3]	= value;  break;
		case SDLK_F4:immediate_keytable[KF4]	= value;  break;
		case SDLK_F5:immediate_keytable[KF5]	= value;  break;
		case SDLK_F6:immediate_keytable[KF6]	= value;  break;
		case SDLK_F7:immediate_keytable[KF7]	= value;  break;
		case SDLK_F8:immediate_keytable[KF8]	= value;  break;
		case SDLK_F9:immediate_keytable[KF9]	= value;  break;
		case SDLK_F10:immediate_keytable[KF10]	= value;  break;

		case SDLK_0:immediate_keytable[KNUM0] = value;  break;
		case SDLK_1:immediate_keytable[KNUM1] = value;  break;
		case SDLK_2:immediate_keytable[KNUM2] = value;  break;
		case SDLK_3:immediate_keytable[KNUM3] = value;  break;
		case SDLK_4:immediate_keytable[KNUM4] = value;  break;
		case SDLK_5:immediate_keytable[KNUM5] = value;  break;
		case SDLK_6:immediate_keytable[KNUM6] = value;  break;
		case SDLK_7:immediate_keytable[KNUM7] = value;  break;
		case SDLK_8:immediate_keytable[KNUM8] = value;  break;
		case SDLK_9:immediate_keytable[KNUM9] = value;  break;

		case SDLK_EXCLAIM:immediate_keytable[KEXCLAIM]	= value;  break;
		case SDLK_QUOTEDBL:immediate_keytable[KDBLQUOTE]	= value;  break;
		case SDLK_HASH:immediate_keytable[KHASH]	= value;  break;
		case SDLK_DOLLAR:immediate_keytable[KDOLLAR]	= value;  break;
		case SDLK_AMPERSAND:immediate_keytable[KAMPERSAND]	= value;  break;
		case SDLK_ASTERISK:immediate_keytable[KAST]	= value;  break;
		case SDLK_LEFTPAREN:immediate_keytable[KLEFTPAREN]	= value;  break;
		case SDLK_RIGHTPAREN:immediate_keytable[KRIGHTPAREN]	= value;  break;
		case SDLK_COLON:immediate_keytable[KCOLON]	= value;  break;
		case SDLK_LESS:immediate_keytable[KLESS]	= value;  break;
		case SDLK_GREATER:immediate_keytable[KGREATER]	= value;  break;
		case SDLK_QUESTION:immediate_keytable[KQUESTION]	= value;  break;
		case SDLK_AT:immediate_keytable[KAT]	= value;  break;
		case SDLK_CARET:immediate_keytable[KCARET]	= value;  break;
		case SDLK_UNDERSCORE:immediate_keytable[KUNDERSCORE]	= value;  break;
		case SDLK_MINUS:immediate_keytable[KMINUS]	= value;  break;
		case SDLK_PLUS:immediate_keytable[KPLUS]	= value;  break;

		default: break;
	}

	if(getHoldedKey(KSHIFT))
	   {
		   if(getPressedKey(KBACKQUOTE)) immediate_keytable[KTILDE] = value;
		    if(getPressedKey(KNUM1)) immediate_keytable[KEXCLAIM] = value;
		    if(getPressedKey(KNUM2)) immediate_keytable[KAT] = value;
		    if(getPressedKey(KNUM3)) immediate_keytable[KHASH] = value;
		    if(getPressedKey(KNUM4)) immediate_keytable[KDOLLAR] = value;
		   if(getPressedKey(KNUM5)) immediate_keytable[KPERCENT] = value;
		    if(getPressedKey(KNUM6)) immediate_keytable[KCARET] = value;
		    if(getPressedKey(KNUM7)) immediate_keytable[KAMPERSAND] = value;
		    if(getPressedKey(KNUM8)) immediate_keytable[KAST] = value;
		    if(getPressedKey(KNUM9)) immediate_keytable[KLEFTPAREN] = value;
		    if(getPressedKey(KNUM0)) immediate_keytable[KRIGHTPAREN] = value;
		    if(getPressedKey(KMINUS)) immediate_keytable[KUNDERSCORE] = value;
		    if(getPressedKey(KEQUAL)) immediate_keytable[KPLUS] = value;
		   if(getPressedKey(KBACKSLASH)) immediate_keytable[KLINE] = value;
		    if(getPressedKey(KLEFTBRACKET)) immediate_keytable[KLEFTBRACE] = value;
		    if(getPressedKey(KRIGHTBRACKET)) immediate_keytable[KRIGHTBRACE] = value;
		    if(getPressedKey(KSEMICOLON)) immediate_keytable[KCOLON] = value;
		    if(getPressedKey(KQUOTE)) immediate_keytable[KDBLQUOTE] = value;
		    if(getPressedKey(KCOMMA)) immediate_keytable[KLESS] = value;
		    if(getPressedKey(KPERIOD)) immediate_keytable[KGREATER] = value;
		    if(getPressedKey(KSLASH)) immediate_keytable[KQUESTION] = value;
	   }
}

bool CInput::getHoldedKey(int key)
{
	if(immediate_keytable[key])
		return true;

	return false;
}

bool CInput::getPressedKey(int key)
{
	if(immediate_keytable[key] && !last_immediate_keytable[key])
	{
		immediate_keytable[key] = false;
		g_pLogFile->textOut(RED, itoa(key)+" is true.");
		return true;
	}

	return false;
}

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

std::string CInput::getPressedTypingKey(void)
{
	int i;
	std::string buf;

	for(i=KA ; i<=KZ ; i++)
	{
		if (getHoldedKey(KSHIFT) && getPressedKey(i))
		{
			buf = 'A' + i - KA;
			g_pLogFile->textOut(RED, buf);
			return buf;
		}
		else if(getPressedKey(i))
		{
			buf = 'a' + i - KA;
			g_pLogFile->textOut(RED, buf);
			return buf;
		}
	}
	for(i=KSPACE ; i<=KAT ; i++)
	{
		if(getPressedKey(i))
		{
			buf = 32 + i - KSPACE;
			g_pLogFile->textOut(RED, buf);
			return buf;
		}
	}
		for(i=KLEFTBRACKET ; i<=KBACKQUOTE ; i++)
	{
		if(getPressedKey(i))
		{
			buf = '[' + i - KLEFTBRACKET;
			g_pLogFile->textOut(RED, buf);
			return buf;
		}
	}
		for(i=KLEFTBRACE ; i<=KTILDE ; i++)
	{
		if(getPressedKey(i))
		{
			buf = '{' + i - KLEFTBRACE;
			g_pLogFile->textOut(RED, buf);
			return buf;
		}
	}
	return buf;
}

bool CInput::getPressedIsTypingKey(void)
{
	int i;

	if(getHoldedKey(KSHIFT))
	   {
		   return true;
	   }
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

bool CInput::getPressedAnyKey(void)
{
	int i;

	for(i=0 ; i<KEYTABLE_SIZE ; i++)
	{
		if(getPressedKey(i))
			return true;
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

bool CInput::getHoldedCommand(int player, int command)
{
	if(InputCommand[player][command].active)
		return true;
	else
		return false;
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
	if(InputCommand[player][command].active && !InputCommand[player][command].lastactive)
	{
		InputCommand[player][command].active = false;
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
	int i;

	for(i=0 ; i<10 ; i++)
		if(getPressedCommand(player,i))
			return true;

	return false;
}

void CInput::flushCommands(void)
{
	for(int i=0 ; i<NUM_INPUTS ; i++)
		for(int j=0 ; j<NUMBER_OF_COMMANDS ; j++)
			InputCommand[i][j].active = InputCommand[i][j].lastactive = false;
}

void CInput::flushKeys(void)
{
	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);
}
void CInput::flushAll(void){ flushKeys(); flushCommands(); }

CInput::~CInput() {
	// Shutdown Joysticks
	while(!mp_Joysticks.empty())
	{
		SDL_JoystickClose(mp_Joysticks.back());
		mp_Joysticks.pop_back();
	}
}


