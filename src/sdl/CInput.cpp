/*
 * CInput.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>
#include <stdio.h>

#include "CInput.h"
#include "../CLogFile.h"

#ifdef WIZ
#include "gp2x.h"
#endif

CInput::CInput() {

	Uint8 i;

	m_exit = false;

	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);

	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		InputCommand[i].active = false;
	}

	// These are the default keyboard commands
	InputCommand[IC_LEFT].keysym = SDLK_LEFT;
	InputCommand[IC_UP].keysym = SDLK_UP;
	InputCommand[IC_RIGHT].keysym = SDLK_RIGHT;
	InputCommand[IC_DOWN].keysym = SDLK_DOWN;

	InputCommand[IC_JUMP].keysym = SDLK_RCTRL;
	InputCommand[IC_POGO].keysym = SDLK_MODE;
	InputCommand[IC_FIRE].keysym = SDLK_SPACE;
	InputCommand[IC_STATUS].keysym = SDLK_RETURN;

	// And those are the default joystick handlings
	InputCommand[IC_LEFT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[IC_LEFT].joyaxis = 0;
	InputCommand[IC_LEFT].joyvalue = -32767;
	InputCommand[IC_UP].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[IC_UP].joyaxis = 1;
	InputCommand[IC_UP].joyvalue = -32767;
	InputCommand[IC_RIGHT].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[IC_RIGHT].joyaxis = 0;
	InputCommand[IC_RIGHT].joyvalue = 32767;
	InputCommand[IC_DOWN].joyeventtype = ETYPE_JOYAXIS;
	InputCommand[IC_DOWN].joyaxis = 1;
	InputCommand[IC_DOWN].joyvalue = 32767;

	InputCommand[IC_JUMP].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[IC_JUMP].joybutton = 0;
	InputCommand[IC_POGO].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[IC_POGO].joybutton = 1;
	InputCommand[IC_FIRE].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[IC_FIRE].joybutton = 2;
	InputCommand[IC_STATUS].joyeventtype = ETYPE_JOYBUTTON;
	InputCommand[IC_STATUS].joybutton = 3;
}

CInput::~CInput() {
}

short CInput::loadControlconfig(void)
{
	FILE *fp;
	if((fp=fopen("controls.dat","rb")) != NULL)
	{
		if(fread(InputCommand, sizeof(stInputCommand),NUMBER_OF_COMMANDS, fp) == 0 )
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
	if((fp=fopen("controls.dat","wb")) != NULL)
	{
		fwrite(InputCommand, sizeof(stInputCommand),NUMBER_OF_COMMANDS, fp);
		fclose(fp);
		return 0;
	}

	return 1;
}
void CInput::getEventName(int position, char *buf)
{
	char buf2[256];
	memset(buf,0,256);

	strcat(buf,SDL_GetKeyName(InputCommand[position].keysym));
	if(InputCommand[position].joyeventtype == ETYPE_JOYAXIS)
	{
		strcat(buf,", ");
		sprintf(buf2,"Joy-A%d",InputCommand[position].joyaxis);
		strcat(buf,buf2);
		if(InputCommand[position].joyvalue < 0)
			strcat(buf,"-");
		else
			strcat(buf,"+");
	}
	if(InputCommand[position].joyeventtype == ETYPE_JOYBUTTON)
	{
		strcat(buf,", ");
		sprintf(buf2,"Joy-B%d",InputCommand[position].joybutton);
		strcat(buf,buf2);
	}
}

bool CInput::readNewEvent(int position)
{
	while( SDL_PollEvent( &Event ) )
	{
		switch ( Event.type )
		{
		 case SDL_KEYDOWN:
			 InputCommand[position].keysym = Event.key.keysym.sym;
			 return true;
			 break;
		 case SDL_JOYBUTTONDOWN:
#ifdef WIZ
			 WIZ_EmuKeyboard( Event.jbutton.button, 1 );
			 return false;
#else
			 InputCommand[position].joyeventtype = ETYPE_JOYBUTTON;
			 InputCommand[position].joybutton = Event.jbutton.button;
			 return true;
#endif
			 break;
		 case SDL_JOYAXISMOTION:
			 InputCommand[position].joyeventtype = ETYPE_JOYAXIS;
			 InputCommand[position].joyaxis = Event.jaxis.axis;
			 InputCommand[position].joyvalue = Event.jaxis.value;
			 return true;
			 break;
		}
	}
	return false;
}

bool CInput::getExitEvent(void) {	return m_exit;	}
void CInput::cancelExitEvent(void) { m_exit=false; }

void CInput::pollEvents()
{
	Uint8 i;

	// copy all the input of the last poll to a space for checking pressing or holding a button
	memcpy(last_immediate_keytable, immediate_keytable, KEYTABLE_SIZE*sizeof(char));

	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
		InputCommand[i].lastactive = InputCommand[i].active;

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

#ifdef WIZ
	WIZ_AdjustVolume( volume_direction ); 
#endif
}

void CInput::processJoystickAxis(void)
{
	unsigned int i;
	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		if(InputCommand[i].joyeventtype == ETYPE_JOYAXIS)
		{
			// Axis are configured for this commmand
			if(Event.jaxis.axis == InputCommand[i].joyaxis)
			{
				// Deadzone
				if((Event.jaxis.value > 3200 && InputCommand[i].joyvalue > 0) ||
					(Event.jaxis.value < -3200 && InputCommand[i].joyvalue < 0))
					InputCommand[i].active = true;
				else
					InputCommand[i].active = false;
			}
		}
	}
}
void CInput::processJoystickButton(int value)
{
#ifdef WIZ
	WIZ_EmuKeyboard( Event.jbutton.button, value );
#else
	unsigned int i;
	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		if(InputCommand[i].joyeventtype == ETYPE_JOYBUTTON)
		{
			// Joystick buttons are configured for this event !!
			if(Event.jbutton.button == InputCommand[i].joybutton)
				InputCommand[i].active = value;
		}
	}
#endif
}

void CInput::sendKey(int key){	immediate_keytable[key] = true;	}

void CInput::processKeys(int value)
{

	unsigned short i;

	for(i=0 ; i<NUMBER_OF_COMMANDS ; i++)
	{
		if(InputCommand[i].keysym == Event.key.keysym.sym)
		{
			if(value)
				InputCommand[i].active = true;
			else
				InputCommand[i].active = false;
		}
	}

    switch(Event.key.keysym.sym)
     {
       // These are only used for ingame stuff or menu, but not for controlling the player anymore
       case SDLK_LEFT: 	immediate_keytable[KLEFT]	= value;  break;
       case SDLK_UP:	immediate_keytable[KUP]		= value;  break;
       case SDLK_RIGHT:	immediate_keytable[KRIGHT]	= value;  break;
       case SDLK_DOWN:	immediate_keytable[KDOWN]	= value;  break;

       case SDLK_RETURN:immediate_keytable[KENTER]	= value;  break;
       case SDLK_RCTRL:immediate_keytable[KCTRL]	= value;  break;
       case SDLK_SPACE:immediate_keytable[KSPACE]	= value;  break;
       case SDLK_MODE:immediate_keytable[KALT]		= value;  break;
       case SDLK_TAB:immediate_keytable[KTAB]		= value;  break;
       case SDLK_LSHIFT:immediate_keytable[KLSHIFT]	= value;  break;
       case SDLK_ESCAPE:immediate_keytable[KQUIT]	= value;  break;

       case SDLK_BACKSPACE:immediate_keytable[KBCKSPCE] = value; break;

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

       case SDLK_1:immediate_keytable[KNUM1] = value;  break;
       case SDLK_2:immediate_keytable[KNUM2] = value;  break;
       case SDLK_3:immediate_keytable[KNUM3] = value;  break;
       case SDLK_4:immediate_keytable[KNUM4] = value;  break;
       case SDLK_5:immediate_keytable[KNUM5] = value;  break;
       case SDLK_6:immediate_keytable[KNUM6] = value;  break;
       case SDLK_7:immediate_keytable[KNUM7] = value;  break;
       case SDLK_8:immediate_keytable[KNUM8] = value;  break;
       case SDLK_9:immediate_keytable[KNUM9] = value;  break;


       default: break;
     }
}

bool CInput::getHoldedKey(int key)
{
	if(immediate_keytable[key])
	{
		return true;
	}

	return false;
}

bool CInput::getPressedKey(int key)
{
	if(immediate_keytable[key] && !last_immediate_keytable[key])
	{
		immediate_keytable[key] = false;
		return true;
	}

	return false;
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
	if(InputCommand[command].active)
	{
		return true;
	}

	return false;
}

bool CInput::getPressedCommand(int command)
{
	if(InputCommand[command].active && !InputCommand[command].lastactive)
	{
		InputCommand[command].active = false;
		return true;
	}

	return false;
}
bool CInput::getPressedAnyCommand(void)
{
	int i;

	for(i=0 ; i<4 ; i++)
	{
		if(getPressedCommand(i))
			return true;
	}
	return false;
}

void CInput::flushKeys(void)
{
	memset(immediate_keytable,false,KEYTABLE_SIZE);
	memset(last_immediate_keytable,false,KEYTABLE_SIZE);
}

#ifdef WIZ
void CInput::WIZ_EmuKeyboard( int button, int value )
{
	SDL_Event fakeevent1, fakeevent2;

	//printf( "Button %d Value %d\n", button, value );

	if( value == 1 ) {
	      fakeevent1.type			= SDL_KEYDOWN;
	      fakeevent1.key.state		= SDL_PRESSED;
	      fakeevent1.key.type		= SDL_KEYDOWN;
	      fakeevent1.key.keysym.mod		= KMOD_NONE;

	      fakeevent2.type			= SDL_KEYDOWN;
	      fakeevent2.key.state		= SDL_PRESSED;
	      fakeevent2.key.type		= SDL_KEYDOWN;
	      fakeevent2.key.keysym.mod		= KMOD_NONE;
	}
	else {
	      fakeevent1.type			= SDL_KEYUP;
	      fakeevent1.key.state		= SDL_RELEASED;
	      fakeevent1.key.type		= SDL_KEYUP;
	      fakeevent1.key.keysym.mod		= KMOD_NONE;

	      fakeevent2.type			= SDL_KEYUP;
	      fakeevent2.key.state		= SDL_RELEASED;
	      fakeevent2.key.type		= SDL_KEYUP;
	      fakeevent2.key.keysym.mod		= KMOD_NONE;
	}

	//printf( "Button %d %d\n", button, value );
	fakeevent1.key.keysym.sym = SDLK_UNKNOWN;
	fakeevent2.key.keysym.sym = SDLK_UNKNOWN;
	switch(button)
	{
		case GP2X_BUTTON_LEFT:
			fakeevent1.key.keysym.sym = SDLK_LEFT;
			break;
		case GP2X_BUTTON_RIGHT:
			fakeevent1.key.keysym.sym = SDLK_RIGHT;
			break;
		case GP2X_BUTTON_UP:
			fakeevent1.key.keysym.sym = SDLK_UP;
			break;
		case GP2X_BUTTON_DOWN:
			fakeevent1.key.keysym.sym = SDLK_DOWN;
			break;
		case GP2X_BUTTON_SELECT:
			fakeevent1.key.keysym.sym = SDLK_RETURN;
			break;
		case GP2X_BUTTON_START:
			fakeevent1.key.keysym.sym = SDLK_ESCAPE;
			break;
		case GP2X_BUTTON_L:
			fakeevent1.key.keysym.sym = SDLK_q;
			fakeevent2.key.keysym.sym = SDLK_1;
			break;
		case GP2X_BUTTON_R:
			fakeevent1.key.keysym.sym = SDLK_t;
			fakeevent2.key.keysym.sym = SDLK_2;
			break;
		case GP2X_BUTTON_A:
			fakeevent1.key.keysym.sym = SDLK_a;
			break;
		case GP2X_BUTTON_B:
			fakeevent1.key.keysym.sym = SDLK_b;
			break;
		case GP2X_BUTTON_X:
			fakeevent1.key.keysym.sym = SDLK_n;
			break;
		case GP2X_BUTTON_Y:
			fakeevent1.key.keysym.sym = SDLK_y;
			fakeevent2.key.keysym.sym = SDLK_F3;
			break;
		case GP2X_BUTTON_VOLUP:
			if( value == 1)
				volume_direction = VOLUME_UP;
			else
				volume_direction = VOLUME_NOCHG;
			break;
		case GP2X_BUTTON_VOLDOWN:
			if( value == 1)
				volume_direction = VOLUME_DOWN;
			else
				volume_direction = VOLUME_NOCHG;
			break;
	}

	if( fakeevent1.key.keysym.sym != SDLK_UNKNOWN )
	{
		SDL_PushEvent (&fakeevent1);	
	}

	if( fakeevent2.key.keysym.sym != SDLK_UNKNOWN )
	{
		SDL_PushEvent (&fakeevent2);	
	}
}

void CInput::WIZ_AdjustVolume( int direction )
{
	if( direction != VOLUME_NOCHG )
	{
		if( volume <= 10 )
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE/2;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE/2;
		}
		else
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE;
		}

		if( volume < VOLUME_MIN ) volume = VOLUME_MIN;
		if( volume > VOLUME_MAX ) volume = VOLUME_MAX;

		printf( "Volume Change: %i\n", volume );

		unsigned long soundDev = open("/dev/mixer", O_RDWR);
		if(soundDev)
		{
			int vol = ((volume << 8) | volume);
			ioctl(soundDev, SOUND_MIXER_WRITE_PCM, &vol);
			close(soundDev);
		}
	}
}
#endif
