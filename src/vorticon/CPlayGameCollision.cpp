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
void CPlayGame::checkPlayerCollisions(int cp)
{
   // Caution: This function still has a lot of bugs!
	int tx,ty;
	int i=0;

#define PLAYERHEIGHT (int)(mp_Player[cp].h)
#define PLAYERWIDTH  (int)(mp_Player[cp].w)

	mp_Player[cp].blockedl = mp_Player[cp].blockedr = false;
	mp_Player[cp].blockedu = false;

   if ((mp_Player[cp].x>>CSF) < 2) mp_Player[cp].blockedl = true;

   //if ( !p_option[OPT_CHEATS].value || g_pInput->getHoldedKey(KTAB)==0)   // holding down TAB will turn off clipping
   //{
      tx = (mp_Player[cp].x);
      ty = (mp_Player[cp].y);

      stTile *p_tile = g_pGfxEngine->Tilemap->mp_tiles;

      /*if((p_tile[mp_map->at(tx,ty)].bup == true) && (p_tile[mp_map->at(tx,ty)].behaviour == true))
      {
    	  killplayer(); // Whines in Episode 1
      }*/

      // set psliding if we're on ice
      /*if (p_tile[mp_Map->at(tx>>CSF,(ty+PLAYERHEIGHT)>>CSF)].bup == 3 && !mp_Player[cp].ppogostick)
      {
        mp_Player[cp].psliding = 1;
        mp_Player[cp].pshowdir = mp_Player[cp].pdir;
      }
      else
      {
    	  mp_Player[cp].psliding = 0;
      }
      // set psemisliding if we're on an ice block
      if (p_tile[mp_Map->at(tx>>CSF,(ty+PLAYERHEIGHT)>>CSF)].bup == 2) // 2*pheighth because the ice down off the player must be detected
    	  mp_Player[cp].psemisliding = 1;
      else
    	  mp_Player[cp].psemisliding = 0;*/

       // we use checkissolidl to check for blockedr, and vice versa
       // for blockedl. this is because here we are checking for the
       // right of the player--which will be the left of a potential tile.
       // Also check floor and ceiling tiles
      // TODO: Try to optimize this!
      //unsigned int aux1;

      /*for( i=5 ; i < PLAYERWIDTH ; i+=1<<5 )
      {
    	  aux1 = mp_Map->at((tx+i)>>CSF,ty>>CSF);
    	  if( p_tile[aux1].bdown
    			  || checkobjsolid((tx+i)>>CSF,ty>>CSF,cp))
		  {
    		  mp_Player[i].blockedu = true;
			  break;
		  }
      }*/

      for( i=0 ; i < PLAYERHEIGHT ; i++ )
      {
    	  if (checkisSolidl( tx+PLAYERWIDTH, ty+i, cp ))
    	  {
    		  mp_Player[cp].blockedr = true;
    		  mp_Player[cp].widejump = false;
    		  break;
    	  }
      }

      for( i=0 ; i < PLAYERHEIGHT ; i++ )
      {
    	  if (checkisSolidr( tx, ty+i, cp ))
    	  {
    		  mp_Player[cp].blockedl = true;
    		  mp_Player[cp].widejump = false;
    		  break;
    	  }
      }

      // for one-way force fields in ep3, so you can back out if you're
      // not all the way through yet
/*
      if (mp_Player[cp].blockedr &&
    		  p_tile[mp_Player[cp].blockedby].bleft &&
    		  !p_tile[mp_Player[cp].blockedby].bright)
      {
        if (((mp_Player[cp].x>>5)+13)>>4<<4 != ((mp_Player[cp].x>>5)+11))
        {  // not on a tile boundary.
        	mp_Player[i].blockedr = false;
        }
      }


      if (mp_Player[cp].blockedl &&
    		  p_tile[mp_Player[cp].blockedby].bright &&
    		  !p_tile[mp_Player[cp].blockedby].bleft)
      {
        if ((((mp_Player[cp].x>>5)+2)>>4<<4)+12 != ((mp_Player[cp].x>>5)+2))
        {  // not on a tile boundary.
        	mp_Player[i].blockedl = false;
        }
      }
*/
      // Check if the player is going out of the level map
      if( mp_Player[cp].y <= (2<<CSF) ) // Upper edge or ceiling
    	  mp_Player[cp].blockedu = true;
      else if( mp_Player[cp].y >= ( mp_Map->m_height<<CSF ) )
      {
    	  mp_Player[cp].blockedd = true;
    	  mp_Player[cp].pdie = true;

      }

      if( (mp_Player[cp].x) >= ( (mp_Map->m_width-3)<<CSF ) )
    	  mp_Player[cp].blockedr = true;

      else if( (mp_Player[cp].x) <= ( 2<<CSF ) )
    	  mp_Player[cp].blockedl = true;
   //}
   //else playpushed_x = false;
}

// checks if tile at (x,y) is solid to the player walking left into it.
// returns 1 and sets blockedby if so.
bool CPlayGame::checkisSolidl(int x, int y, int cp)
{
int t = mp_Map->at(x>>CSF, y>>CSF);

  if(g_pGfxEngine->Tilemap->mp_tiles[t].bleft)
  {
    mp_Player[cp].blockedby = t;
    return true;
  }
  if (checkObjSolid(x>>5,y>>5,cp))
  {
	mp_Player[cp].blockedby = 0;
    return true;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (checkDoorBlock(t, cp, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
    {
      return true;
    }
  }
  return false;
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
char CPlayGame::checkisSolidr(int x, int y, int cp)
{
int t = mp_Map->at(x>>CSF, y>>CSF);
  if(g_pGfxEngine->Tilemap->mp_tiles[t].bright)
  {
	  mp_Player[cp].blockedby = t;
    return true;
  }
  else if (checkObjSolid(x>>5,y>>5,cp))
  {
	  mp_Player[cp].blockedby = 0;
    return true;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (checkDoorBlock(t, cp, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
      return true;
  }
  return false;
}

// this is so objects can block the player,
// player can stand on them, etc.
// x and y are the CSFed coordinates to check (e.g. playx and playy)
// returns nonzero if there is a solid object
// at that point
int CPlayGame::checkObjSolid(unsigned int x, unsigned int y, unsigned int cp)
{
  CSprite *sprite;
  int o=0;

  std::vector<CObject>::iterator p_object;
   for( p_object=m_Object.begin() ; p_object!=m_Object.end() ; p_object++ )
   {
	   sprite = g_pGfxEngine->Sprite.at(p_object->sprite);
	   if (p_object->exists && p_object->cansupportplayer[cp])
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
char CPlayGame::checkDoorBlock(int t, int cp, int which)
{
        if (which==DOOR_YELLOW)
        {
          if (!mp_Player[cp].inventory.HasCardYellow)
          {
        	  mp_Player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_RED)
        {
          if (!mp_Player[cp].inventory.HasCardRed)
          {
        	  mp_Player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_GREEN)
        {
          if (!mp_Player[cp].inventory.HasCardGreen)
          {
        	  mp_Player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_BLUE)
        {
          if (!mp_Player[cp].inventory.HasCardBlue)
          {
        	  mp_Player[cp].blockedby = t;
            return 1;
          }
        }

   return 0;
}

