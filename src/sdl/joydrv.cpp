/* JOYDRV.C

*/

#include "../keen.h"
#include "../keenext.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"

#include <SDL.h>
#include "joydrv.h"

char JoyDrv_Start(SDL_Joystick **joystick)
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
		  {
			  g_pLogFile->ftextOut("    %s<br>", SDL_JoystickName(i));
		  }
	  }
	  else
	  {
		  g_pLogFile->ftextOut("No joysticks were found.<br>\n");
	  }
	}

  SDL_JoystickEventState(SDL_ENABLE);
  *joystick = SDL_JoystickOpen(0);

  return 0;
}

void JoyDrv_Stop(SDL_Joystick **joystick)
{
	if(*joystick)
		SDL_JoystickClose(*joystick);
}
