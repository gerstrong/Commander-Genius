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

#define PLAYERHEIGHT (mp_Player[i].h)
#define PLAYERWIDTH  (mp_Player[i].w)

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
    	  killplayer(); // Whines
      }*/

      // set psliding if we're on ice
      if (p_tile[mp_Map->at(tx>>CSF,(ty+PLAYERHEIGHT)>>CSF)].bup == 3 && !mp_Player[cp].ppogostick)
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
    	  mp_Player[cp].psemisliding = 0;

       // we use checkissolidl to check for blockedr, and vice versa
       // for blockedl. this is because here we are checking for the
       // right of the player--which will be the left of a potential tile.
       // Also check floor and ceiling tiles
      // TODO: Try to optimize this!
      unsigned int aux1;

      for( i=5 ; i < PLAYERWIDTH ; i++ )
      {
    	  aux1 = mp_Map->at((tx+i)>>CSF,ty>>CSF);
    	  if( p_tile[aux1].bdown
    			  || checkobjsolid((tx+i)<<5,(ty)<<5,cp))
		  {
    		  mp_Player[i].blockedu = true;
			  break;
		  }
      }

      /*for( i=2 ; i < PLAYERHEIGHT ; i++ )
      {
    	  if (checkissolidl(( (mp_Player[cp].x+PLAYERWIDTH+1)>>5, (mp_Player[cp].y>>5)+i, cp))
    	  {
    		  mp_Player[cp].blockedr = true;
    		  mp_Player[cp].widejump = false;
    		  break;
    	  }
      }

      for( i=2 ; i < PLAYERHEIGHT ; i++ )
      {
    	  if (checkissolidr((mp_Player[cp].x>>5)+2, (mp_Player[cp].y>>5)+i, cp))
    	  {
    		  mp_Player[cp].blockedl = true;
    		  mp_Player[cp].widejump = false;
    		  break;
    	  }
      }*/

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
char CPlayGame::checkissolidl(int x, int y, int cp)
{
int t = getmaptileat(x, y);

  if(g_pGfxEngine->Tilemap->mp_tiles[t].bleft || x < 0)
  {
    mp_Player[cp].blockedby = t;
    return 1;
  }
  if (checkobjsolid(x<<5,y<<5,cp))
  {
	  mp_Player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
    {
      return 1;
    }
  }
  return 0;
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
char CPlayGame::checkissolidr(int x, int y, int cp)
{
int t = mp_Map->at(x, y);
  if(g_pGfxEngine->Tilemap->mp_tiles[t].bright)
  {
	  mp_Player[cp].blockedby = t;
    return 1;
  }
  else if (checkobjsolid(x<<5,y<<5,cp))
  {
	  mp_Player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp, g_pGfxEngine->Tilemap->mp_tiles[t].behaviour))
    {
      return 1;
    }
  }
  return 0;
}

// returns 1 if player cp has the card to door t, which -> door
char CPlayGame::CheckDoorBlock(int t, int cp, int which)
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

