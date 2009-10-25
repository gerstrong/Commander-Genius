/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */


#include "../../keen.h"
#include "../../game.h"
#include "../../sdl/sound/CSound.h"

// Reference to ../game.cpp
unsigned char spawn_object(int x, int y, int otype);

void autoray_ai(int o)
{
/*int newobject;
	if (objects[o].needinit)
	{
		objects[o].ai.se.timer = 0;
		objects[o].sprite = BLANKSPRITE;
		objects[o].inhibitfall = 1;
		objects[o].needinit = 0;
	}

	if (!gunfiretimer)
	{
          if (objects[o].type==OBJ_AUTORAY_V)
          {
            newobject = spawn_object(objects[o].x, objects[o].y, OBJ_RAY);
            objects[newobject].sprite = RAY_VERT_EP3;
            objects[newobject].ai.ray.direction = DOWN;
          }
          else
          {
            newobject = spawn_object(objects[o].x, objects[o].y, OBJ_RAY);
            objects[newobject].sprite = ENEMYRAYEP3;
            objects[newobject].ai.ray.direction = RIGHT;
          }
          if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].x);
	}*/
}



