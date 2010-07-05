#include "CDoor.h"

#include "../../../keen.h"
#include "../../../graphics/CGfxEngine.h"

CDoor::CDoor()
{
	int x, y;
	timer = 0;
	doorsprite.setHeight(32);
	inhibitfall = true;
	needinit = false;
	x = getXPosition()>>CSF;
	y = getYPosition()>>CSF;
	mp_Map->redrawAt(x, y);
	mp_Map->redrawAt(x, y+1);
}

void CDoor::process()
{
	CSprite &doorsprite = g_pGfxEngine->getSprite(sprite);

	if (timer > DOOR_OPEN_SPEED)
	{
		// TODO: Create a flag for mods in which the door can be opened in another direction
		if (DoorOpenDir==DOWN) moveDown(1<<STC);
		doorsprite.setHeight(doorsprite.getHeight()-1);
		timer = 0;
		if (doorsprite.getHeight() == 0)
		{
			return;
		}
	}
	else timer++;
}
