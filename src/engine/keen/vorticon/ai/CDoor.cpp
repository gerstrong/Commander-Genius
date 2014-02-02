#include "CDoor.h"

#include "graphics/GsGraphics.h"

CDoor::CDoor(CMap *pmap, Uint32 x, Uint32 y, Uint32 doorspriteID):
CVorticonSpriteObject(pmap, x, y, OBJ_DOOR, 0)
{
	sprite=doorspriteID;
    GsSprite &Doorsprite = gGraphics.getSprite(mSprVar,sprite);
	timer = 0;
	Doorsprite.setHeight(32);
	inhibitfall = true;
	x = getXPosition()>>CSF;
	y = getYPosition()>>CSF;
	mp_Map->redrawAt(x, y);
	mp_Map->redrawAt(x, y+1);
}

void CDoor::process()
{
    GsSprite &Doorsprite = gGraphics.getSprite(mSprVar,sprite);

	if (timer > DOOR_OPEN_SPEED)
	{
		// TODO: Create a flag for mods in which the door can be opened in another direction
		//if (DoorOpenDir==DOWN)
		moveDown(1<<STC);
		Doorsprite.setHeight(Doorsprite.getHeight()-1);
		timer = 0;
		if (Doorsprite.getHeight() == 0)
		{
			exists = false;
			return;
		}
	}
	else timer++;
}
