/*
 * CPlayGameCollision.cpp
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../../keen.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"
#include "../../sdl/sound/CSound.h"
#include "../../common/CMapLoader.h"
#include "../../graphics/CGfxEngine.h"

// returns 1 if player cp has the card to door t, which -> door
char CPlayGame::checkDoorBlock(int t, CPlayer *p_player, int which)
{
	if (which==DOOR_YELLOW)
	{
		if (!p_player->inventory.HasCardYellow)
		{
			p_player->blockedby = t;
            return 1;
		}
	}
	else if (which==DOOR_RED)
	{
		if (!p_player->inventory.HasCardRed)
		{
			p_player->blockedby = t;
            return 1;
		}
	}
	else if (which==DOOR_GREEN)
	{
		if (!p_player->inventory.HasCardGreen)
		{
			p_player->blockedby = t;
            return 1;
		}
	}
	else if (which==DOOR_BLUE)
	{
		if (!p_player->inventory.HasCardBlue)
		{
			p_player->blockedby = t;
            return 1;
		}
	}
	
	return 0;
}

