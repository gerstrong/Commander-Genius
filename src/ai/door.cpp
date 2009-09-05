#include "../keen.h"

#include "door.h"
#include "../graphics/CGfxEngine.h"

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    10

void door_ai(int o, char DoorOpenDir)
{
	if (objects[o].needinit)
	{
		objects[o].ai.door.timer = 0;
		g_pGfxEngine->Sprite[objects[o].sprite]->setHeight(32);
		objects[o].inhibitfall = 1;
		objects[o].needinit = 0;
	}

   if (objects[o].ai.door.timer > DOOR_OPEN_SPEED)
   {
	   // TODO: Create a flag for mods in which the door can be opened in another direction
		/*if (DoorOpenDir==DOWN)*/ objects[o].y += (1<<CSF);
		g_pGfxEngine->Sprite[objects[o].sprite]->setHeight(g_pGfxEngine->Sprite[objects[o].sprite]->getHeight()-1);
		if (g_pGfxEngine->Sprite[objects[o].sprite]->getHeight() == 0)
		{
			delete_object(o);
		}
		objects[o].ai.door.timer = 0;
   }
   else objects[o].ai.door.timer++;
}


