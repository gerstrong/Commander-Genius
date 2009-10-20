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
			if( checkisSolidl( p_player->x+p_player->w, p_player->y+1, p_player)
				or checkisSolidl( p_player->x+p_player->w, p_player->y + p_player->h/2, p_player)
				or checkisSolidl( p_player->x+p_player->w, p_player->y + p_player->h-1, p_player) )
			{
				p_player->pinertia_x = 0;
				break;
			}
			p_player->x++;
		}
	}
	else if( p_player->goto_x < p_player->x )
	{
		// The player walked left
		while(p_player->goto_x < p_player->x)
		{
			if( checkisSolidr( p_player->x, p_player->y+1, p_player)
				or checkisSolidr( p_player->x, p_player->y + p_player->h/2, p_player)
				or checkisSolidl( p_player->x, p_player->y + p_player->h-1, p_player) )
			{
				p_player->pinertia_x = 0;
				break;
			}
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
			if( checkisSolidu( p_player->x+1, p_player->y+p_player->h, p_player) )
			{
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
			if( checkisSolidd( p_player->x+1, p_player->y, p_player) )
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
bool CPlayGame::checkisSolidl(int x, int y, CPlayer *p_player)
{
int t = mp_Map->at(x>>CSF, y>>CSF);

  if(g_pGfxEngine->Tilemap->mp_tiles[t].bleft)
  {
	  p_player->blockedby = t;
    return true;
  }
  if (checkObjSolid(x>>5,y>>5, p_player))
  {
	  p_player->blockedby = 0;
    return true;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (checkDoorBlock(t, p_player, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
    {
      return true;
    }
  }
  return false;
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
bool CPlayGame::checkisSolidr(int x, int y, CPlayer *p_player)
{
int t = mp_Map->at(x>>CSF, y>>CSF);
  if(g_pGfxEngine->Tilemap->mp_tiles[t].bright)
  {
	  p_player->blockedby = t;
    return true;
  }
  else if (checkObjSolid(x>>5,y>>5, p_player))
  {
	  p_player->blockedby = 0;
    return true;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (checkDoorBlock(t, p_player, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
      return true;
  }
  return false;
}

bool CPlayGame::checkisSolidd(int x, int y, CPlayer *p_player)
{
int t = mp_Map->at(x>>CSF, y>>CSF);
  if(g_pGfxEngine->Tilemap->mp_tiles[t].bdown)
  {
    return true;
  }
  else if (checkObjSolid(x>>5,y>>5, p_player))
  {
    return true;
  }
  return false;
}

bool CPlayGame::checkisSolidu(int x, int y, CPlayer *p_player)
{
int t = mp_Map->at(x>>CSF, y>>CSF);
  if(g_pGfxEngine->Tilemap->mp_tiles[t].bup)
  {
    return true;
  }
  else if (checkObjSolid(x>>(CSF-4),y>>(CSF-4), p_player))
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
int CPlayGame::checkObjSolid(unsigned int x, unsigned int y, CPlayer *p_player)
{
  CSprite *sprite;
  int o=0;

  std::vector<CObject>::iterator p_object;
   for( p_object=m_Object.begin() ; p_object!=m_Object.end() ; p_object++ )
   {
	   sprite = g_pGfxEngine->Sprite.at(p_object->sprite);
	   if (p_object->exists && p_object->cansupportplayer[p_player->m_player_number])
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

