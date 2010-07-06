#include "CDoor.h"

#include "../../../keen.h"
#include "../../../graphics/CGfxEngine.h"

CDoor::CDoor(CMap *pmap, Uint32 x, Uint32 y):
CObject(pmap, x, y)
{
	CSprite &doorsprite = g_pGfxEngine->getSprite(sprite);
	timer = 0;
	doorsprite.setHeight(32);
	inhibitfall = true;
}

void CDoor::process()
{
	CSprite &doorsprite = g_pGfxEngine->getSprite(sprite);

	if (timer > DOOR_OPEN_SPEED)
	{
		// TODO: Create a flag for mods in which the door can be opened in another direction
		//if (DoorOpenDir==DOWN) moveDown(1<<STC);
		doorsprite.setHeight(doorsprite.getHeight()-1);
		timer = 0;
		if (doorsprite.getHeight() == 0)
		{
			return;
		}
	}
	else timer++;
}
