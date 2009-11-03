/*
 * CPlayGameCollision.cpp
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../keen.h"
#include "../sdl/CTimer.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../common/CMapLoader.h"
#include "../graphics/CGfxEngine.h"

// set blockedl/r/u...is Keen up against a solid object or a the edge of the level?
void CPlayGame::checkPlayerCollisions(CPlayer *p_player)
{
	// Start with X-Coordinates
	p_player->blockedl = p_player->blockedr = false;
	if( p_player->goto_x > p_player->x )
	{
		// The player walked right
		while(p_player->goto_x > p_player->x)
		{
			if( checkisSolidl(p_player) )	break;
			p_player->x++;
		}
	}
	else if( p_player->goto_x < p_player->x )
	{
		// The player walked left
		while(p_player->goto_x < p_player->x)
		{
			if( checkisSolidr(p_player) )	break;
			p_player->x--;
		}
	}
	p_player->goto_x = p_player->x;
	
	// Continue with Y-Coordinates
	p_player->blockedu = p_player->blockedd = false;
	if( p_player->goto_y > p_player->y )
	{
		// The player is falling
		while(p_player->goto_y > p_player->y)
		{
			if( checkisSolidu(p_player) )
			{
				p_player->pfalling = false;
				p_player->blockedd = true;
				break;
			}
			p_player->y++;
		}
	}
	else if( p_player->goto_y < p_player->y )
	{
		// The player jumped or flew up!
		while(p_player->goto_y < p_player->y)
		{
			if( checkisSolidd(p_player) )
			{
				p_player->blockedu = true;
				break;
			}
			p_player->y--;
		}
	}
	
	p_player->goto_y = p_player->y;
	
    // Check if the player is going out of the level map
    if( p_player->y <= 1<<CSF ) // Upper edge or ceiling
    	p_player->blockedu = true;
    else if( p_player->y >= ( mp_Map->m_height<<CSF ) ) // lower edge or floor
    {
    	p_player->blockedd = true;
    	p_player->pdie = true;
    }
	
    if( (p_player->x) >= ( mp_Map->m_width<<CSF ) ) // right edge
    	p_player->blockedr = true;
    else if( p_player->x <= 0 ) // left edge
    	p_player->blockedl = true;
}

// checks if tile at (x,y) is solid to the player walking left into it.
// returns 1 and sets blockedby if so.
bool CPlayGame::checkisSolidl(CPlayer *p_player)
{
CSprite *sprite = g_pGfxEngine->Sprite[p_player->playframe];

	int x=p_player->x+sprite->m_bboxX2;
	int y1=p_player->y+sprite->m_bboxY1+1;
	int y2=p_player->y+sprite->m_bboxY2/2;
	int y3=p_player->y+sprite->m_bboxY2-1;
	
	int t1 = mp_Map->at(x>>CSF, y1>>CSF);
	int t2 = mp_Map->at(x>>CSF, y2>>CSF);
	int t3 = mp_Map->at(x>>CSF, y3>>CSF);
	
	if(p_player->pdie) return false;
	
	if(g_pGfxEngine->Tilemap->mp_tiles[t1].bleft)
	{
		p_player->blockedby = t1;
		return true;
	}
	else if(g_pGfxEngine->Tilemap->mp_tiles[t2].bleft)
	{
		p_player->blockedby = t2;
		return true;
	}
	else if(g_pGfxEngine->Tilemap->mp_tiles[t3].bleft)
	{
		p_player->blockedby = t3;
		return true;
	}
	
	if (checkObjSolid(x>>(CSF-4),y1>>(CSF-4), p_player->m_player_number)
		or checkObjSolid(x>>(CSF-4),y2>>(CSF-4), p_player->m_player_number)
		or checkObjSolid(x>>(CSF-4),y3>>(CSF-4), p_player->m_player_number) )
	{
		p_player->blockedby = 0;
		return true;
	}
	else
	{
		// don't let player walk through doors he doesn't have the key to
		if (checkDoorBlock(t2, p_player, g_pGfxEngine->Tilemap->mp_tiles[t2].behaviour))
		{
			return true;
		}
	}
	return false;
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
bool CPlayGame::checkisSolidr(CPlayer *p_player)
{
CSprite *sprite = g_pGfxEngine->Sprite[p_player->playframe];

	int x=p_player->x+sprite->m_bboxX1;
	int y1=p_player->y+sprite->m_bboxY1+1;
	int y2=p_player->y+sprite->m_bboxY2/2;
	int y3=p_player->y+sprite->m_bboxY2-1;
	
	int t1 = mp_Map->at(x>>CSF, y1>>CSF);
	int t2 = mp_Map->at(x>>CSF, y2>>CSF);
	int t3 = mp_Map->at(x>>CSF, y3>>CSF);
	
	if(p_player->pdie) return false;
	
	if(g_pGfxEngine->Tilemap->mp_tiles[t1].bleft)
	{
		p_player->blockedby = t1;
		return true;
	}
	else if(g_pGfxEngine->Tilemap->mp_tiles[t2].bleft)
	{
		p_player->blockedby = t2;
		return true;
	}
	else if(g_pGfxEngine->Tilemap->mp_tiles[t3].bleft)
	{
		p_player->blockedby = t3;
		return true;
	}
	
	else if (checkObjSolid(x>>(CSF-4),y1>>(CSF-4), p_player->m_player_number)
			 or checkObjSolid(x>>(CSF-4),y2>>(CSF-4), p_player->m_player_number)
			 or checkObjSolid(x>>(CSF-4),y3>>(CSF-4), p_player->m_player_number) )
	{
		p_player->blockedby = 0;
		return true;
	}
	else
	{
		// don't let player walk through doors he doesn't have the key to
		if (checkDoorBlock(t2, p_player, g_pGfxEngine->Tilemap->mp_tiles[t2].behaviour))
			return true;
	}
	return false;
}

bool CPlayGame::checkisSolidd(CPlayer *p_player)
{
CSprite *sprite = g_pGfxEngine->Sprite[p_player->playframe];

	int x1 = p_player->x+sprite->m_bboxX1+1;
	int x2 = p_player->x+sprite->m_bboxX2-1;
	int y = p_player->y+sprite->m_bboxY1-1;
	int t1 = mp_Map->at(x1>>CSF, y>>CSF);
	int t2 = mp_Map->at(x2>>CSF, y>>CSF);
	
	if(p_player->pdie) return false;
	
	if(g_pGfxEngine->Tilemap->mp_tiles[t1].bdown || g_pGfxEngine->Tilemap->mp_tiles[t2].bdown )
	{
		return true;
	}
	else if (checkObjSolid(x1>>(CSF-4),y>>(CSF-4), p_player->m_player_number)
			 || checkObjSolid(x2>>(CSF-4),y>>(CSF-4), p_player->m_player_number) )
	{
		return true;
	}
	return false;
}

bool CPlayGame::checkisSolidu(CPlayer *p_player)
{
CSprite *sprite = g_pGfxEngine->Sprite[p_player->playframe];

	int x1 = p_player->x+sprite->m_bboxX1+1;
	int x2 = p_player->x+sprite->m_bboxX2-1;
	int y = p_player->y+sprite->m_bboxY2+(1<<STC);
	int t1 = mp_Map->at(x1>>CSF, y>>CSF);
	int t2 = mp_Map->at(x2>>CSF, y>>CSF);
	
	if(p_player->pdie) return false;
	
	if(y != (y>>CSF)<<CSF) return false;
	
	if(g_pGfxEngine->Tilemap->mp_tiles[t1].bup || g_pGfxEngine->Tilemap->mp_tiles[t2].bup)
	{
		return true;
	}
	else if (checkObjSolid(x1>>(CSF-4),y>>(CSF-4), p_player->m_player_number)
			 || checkObjSolid(x2>>(CSF-4),y>>(CSF-4), p_player->m_player_number) )
	{
		return true;
	}
	return false;
}

// this is so objects can block the player,
// player can stand on them, etc.
// x and y are the CSFed coordinates to check (e.g. playx and playy)
// returns nonzero if there is a solid object
// at that point
int CPlayGame::checkObjSolid(unsigned int x, unsigned int y, int cp)
{
	CSprite *sprite;
	int o=0;
	
	std::vector<CObject>::iterator p_object;
	for( p_object=m_Object.begin() ; p_object!=m_Object.end() ; p_object++ )
	{
		sprite = g_pGfxEngine->Sprite.at(p_object->sprite);
		if (p_object->exists && p_object->cansupportplayers)
		{
			if (x >= p_object->x+sprite->m_bboxX1)
				if (x <= p_object->x+sprite->m_bboxX2)
					if (y >= p_object->y+sprite->m_bboxY1)
						if (y <= p_object->y+sprite->m_bboxY2)
							return o;
			o++;
		}
	}
	return 0;
}


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

