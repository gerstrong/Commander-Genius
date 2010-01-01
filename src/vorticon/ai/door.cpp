#include "CObjectAI.h"

#include "../../keen.h"
#include "../../graphics/CGfxEngine.h"

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    10

void CObjectAI::door_ai( CObject &object, char DoorOpenDir )
{
	CSprite &doorsprite = g_pGfxEngine->getSprite(object.sprite);
	if (object.needinit)
	{
		object.ai.door.timer = 0;
		doorsprite.setHeight(32);
		object.inhibitfall = true;
		object.needinit = false;
		mp_Map->redrawAt(object.x>>CSF, object.y>>CSF);
		mp_Map->redrawAt(object.x>>CSF, (object.y>>CSF)+1);
	}
	 
	if (object.ai.door.timer > DOOR_OPEN_SPEED)
	{
		// TODO: Create a flag for mods in which the door can be opened in another direction
		if (DoorOpenDir==DOWN) object.y += (1<<STC);
		doorsprite.setHeight(doorsprite.getHeight()-1);
		if (doorsprite.getHeight() == 0)
		{
			object.ai.door.timer = 0;
			deleteObj(object);
			return;
		}
	}
	else object.ai.door.timer++;
}


