#include "CObjectAI.h"

#include "../../keen.h"
#include "../../graphics/CGfxEngine.h"

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    10

void CObjectAI::door_ai( CObject *p_object, char DoorOpenDir )
{
	if (p_object->needinit)
	{
		p_object->ai.door.timer = 0;
		g_pGfxEngine->Sprite[p_object->sprite]->setHeight(32);
		p_object->inhibitfall = true;
		p_object->needinit = false;
		mp_Map->redrawAt(p_object->x>>CSF, p_object->y>>CSF);
		mp_Map->redrawAt(p_object->x>>CSF, (p_object->y>>CSF)+1);
	}
	 
	if (p_object->ai.door.timer > DOOR_OPEN_SPEED)
	{
		// TODO: Create a flag for mods in which the door can be opened in another direction
		if (DoorOpenDir==DOWN) p_object->y += (1<<STC);
		g_pGfxEngine->Sprite[p_object->sprite]->setHeight(g_pGfxEngine->Sprite[p_object->sprite]->getHeight()-1);
		if (g_pGfxEngine->Sprite[p_object->sprite]->getHeight() == 0)
		{
			p_object->ai.door.timer = 0;
			deleteObj(p_object);
			return;
		}
	}
	else p_object->ai.door.timer++;
}


