/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include "../CLogFile.h"
#include "../vorticon/spritedefines.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../keen.h"
#include <string.h>

///
// Initialization Routine
///
CObject::CObject() {
	honorPriority = false;
	exists = false;
	cansupportplayers = false;
}

bool CObject::spawn(int x0, int y0, int otype)
{
   // find an unused object slot
   if (!exists && otype != OBJ_PLAYER)
   {
     x = x0;
     y = y0;
     m_type = otype;
     exists = 1;
     needinit = true;
     dead = false;
     onscreen = 0;
     hasbeenonscreen = 0;
     zapped = false;
     canbezapped = 0;
     inhibitfall = false;
     honorPriority = true;
  	 cansupportplayers = false;

  	setupObjectType();

     return true;
   }
   // object could not be created
   g_pLogFile->ftextOut("Object of type %d could not be created at %d,%d (out of object slots)<br>",otype,x,y);
   return false;
}

void CObject::setupObjectType()
{
	switch(m_type)
	{
	case OBJ_AUTORAY: sprite = ENEMYRAYEP3; break;
	case OBJ_AUTORAY_V: sprite = RAY_VERT_EP3; break;
	case OBJ_DOOR: sprite = DOOR_YELLOW_SPRITE; break;
	case OBJ_TELEPORTER: sprite = OBJ_TELEPORTER_DEFSPRITE; break;
	case OBJ_SECTOREFFECTOR: sprite = BLANKSPRITE; break;
	case OBJ_GOTPOINTS: sprite = PT500_SPRITE; break;
	case OBJ_YORP: sprite = OBJ_YORP_DEFSPRITE; break;
	case OBJ_GARG: sprite = OBJ_GARG_DEFSPRITE; break;
	case OBJ_BUTLER: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_TANK: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_ICECHUNK: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ICEBIT: sprite = OBJ_ICEBIT_DEFSPRITE; break;
	case OBJ_ICECANNON: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ROPE: sprite = OBJ_ROPE_DEFSPRITE; break;
	case OBJ_VORT: sprite = OBJ_RAY_DEFSPRITE_EP1; break;
	default: sprite = BLANKSPRITE; break;
	}
	  /*else if (p_levelcontrol->episode==2)
	  {
			objdefsprites[OBJ_SCRUB] = OBJ_SCRUB_DEFSPRITE;
			objdefsprites[OBJ_TANKEP2] = OBJ_TANKEP2_DEFSPRITE;
			objdefsprites[OBJ_VORTELITE] = OBJ_VORTELITE_DEFSPRITE;

			objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP2;
			objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP2;
			objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP2;
			objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP2;
			objdefsprites[OBJ_SPARK] = OBJ_SPARK_DEFSPRITE_EP2;
	  }
	  else if (p_levelcontrol->episode==3)
	  {
			objdefsprites[OBJ_FOOB] = OBJ_FOOB_DEFSPRITE;
			objdefsprites[OBJ_NINJA] = OBJ_NINJA_DEFSPRITE;
			objdefsprites[OBJ_MOTHER] = OBJ_MOTHER_DEFSPRITE;
			objdefsprites[OBJ_MEEP] = OBJ_MEEP_DEFSPRITE;
			objdefsprites[OBJ_BALL] = OBJ_BALL_DEFSPRITE;
			objdefsprites[OBJ_JACK] = OBJ_JACK_DEFSPRITE;
			objdefsprites[OBJ_NESSIE] = OBJ_NESSIE_DEFSPRITE;

			objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP3;
			objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP3;
			objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP3;
			objdefsprites[OBJ_PLATVERT] = OBJ_PLATFORM_DEFSPRITE_EP3;
			objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP3;
	  }*/
}

void CObject::setPos( int px, int py )
{
	scrx = px;
	scry = py;
}

///
// do object and enemy AI
///
void CObject::performAI( CMap *p_map, stOption *p_option, char episode )
{
	if ( !exists || m_type==OBJ_PLAYER ) return;

    //gamedo_calcenemyvisibility(i);

    // This will do the function gamedo_calcenemyvisibility(i);
    // check if object is really in the map!!!
    if (x < 0 || y < 0) return;

/*    if (x > (map.xsize << (CSF-4) || objects[i].y > (map.ysize << CSF << 4))
    	  return;*/
    // TODO: This check should work again

    // Bitmaps are also part of the object, but only print them directly
    //if ( m_type==OBJ_EGA_BITMAP ) return;

    if (scrx < -(g_pGfxEngine->Sprite[sprite]->getWidth()) || scrx > g_pVideoDriver->getGameResRect().w
          || scry < -(g_pGfxEngine->Sprite[sprite]->getHeight()) || scry > g_pVideoDriver->getGameResRect().h)
    {
    	onscreen = false;
        wasoffscreen = true;
        if (m_type==OBJ_ICEBIT) exists = false;
    }
    else
    {
    	onscreen = true;
    	hasbeenonscreen = true;
    }

	if (hasbeenonscreen || zapped ||
			m_type==OBJ_RAY || \
			m_type==OBJ_ICECHUNK || m_type==OBJ_PLATFORM ||
			m_type==OBJ_PLATVERT || m_type==OBJ_YORP ||
			m_type==OBJ_FOOB || m_type==OBJ_SCRUB)

    {
		 performCommonAI(p_map);
       	 switch(m_type)
       	 {
       	 /*//KEEN1
		  case OBJ_YORP: yorp_ai(i, *p_levelcontrol); break;
		  case OBJ_GARG: garg_ai(i, p_levelcontrol->hardmode); break;
		  case OBJ_VORT: vort_ai(i, p_levelcontrol ); break;
		  case OBJ_BUTLER: butler_ai(i, p_levelcontrol->hardmode); break;
		  case OBJ_TANK: tank_ai(i, p_levelcontrol->hardmode); break;
		  case OBJ_DOOR: door_ai(i, p_levelcontrol->cepvars.DoorOpenDir); break;
		  case OBJ_ICECANNON: icecannon_ai(i); break;
		  case OBJ_ICECHUNK: icechunk_ai(i); break;
		  case OBJ_ICEBIT: icebit_ai(i); break;
		  case OBJ_TELEPORTER: teleporter_ai(i, *p_levelcontrol); break;
		  case OBJ_ROPE: rope_ai(i); break;

		  //KEEN2
		  case OBJ_SCRUB: scrub_ai(i, *p_levelcontrol); break;
		  case OBJ_TANKEP2: tankep2_ai(i, p_levelcontrol->hardmode); break;
		  case OBJ_PLATFORM: platform_ai(i, *p_levelcontrol); break;
		  case OBJ_VORTELITE: vortelite_ai(i, p_levelcontrol->dark); break;
		  case OBJ_SECTOREFFECTOR: se_ai(i, p_levelcontrol ); break;
		  case OBJ_BABY: baby_ai(i, p_levelcontrol->episode,
							  p_levelcontrol->hardmode); break;
		  case OBJ_EXPLOSION: explosion_ai(i); break;
		  case OBJ_EARTHCHUNK: earthchunk_ai(i); break;
		  case OBJ_SPARK: spark_ai(i, &(p_levelcontrol->sparks_left) ); break;

		  //KEEN3
		  case OBJ_FOOB: foob_ai(i, p_levelcontrol->hardmode); break;
		  case OBJ_NINJA: ninja_ai( i, p_levelcontrol->hardmode); break;
		  case OBJ_MEEP: meep_ai( i, *p_levelcontrol ); break;
		  case OBJ_SNDWAVE: sndwave_ai( i, p_levelcontrol->hardmode); break;
		  case OBJ_MOTHER: mother_ai( i, *p_levelcontrol ); break;
		  case OBJ_FIREBALL: fireball_ai( i, p_levelcontrol->hardmode ); break;
		  case OBJ_BALL: ballandjack_ai(i); break;
		  case OBJ_JACK: ballandjack_ai(i); break;
		  case OBJ_PLATVERT: platvert_ai(i); break;
		  case OBJ_NESSIE: nessie_ai(i); break;

		  //Specials*/
		  case OBJ_RAY: ray_ai( episode, p_option[OPT_FULLYAUTOMATIC].value, 1); break;
		  //case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
		  //case OBJ_GOTPOINTS: gotpoints_ai(i); break;

		  //case OBJ_DEMOMSG: break;

		  default:
			  g_pLogFile->ftextOut("gamedo_enemy_ai: Object is of invalid type %d\n",  m_type);
       	  break;
         }
      }
}

///
//
///
// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void CObject::performCommonAI(CMap *p_map)
{
int x0,y0,xa,ya,xsize,ysize;
int temp;
stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	if (m_type==OBJ_GOTPOINTS) return;

	ysize = g_pGfxEngine->Sprite[sprite]->getHeight()<<(CSF-4);
	xsize = g_pGfxEngine->Sprite[sprite]->getWidth()<<(CSF-4);

	// set value of blockedd--should object fall?
	temp = (y>>(CSF-4))+ysize;
	if ((temp>>TILE_S)<<TILE_S != temp)	blockedd = false;
	else
	{ // on a tile boundary, test if tile under object is solid
		blockedd = 0;
		x0 = (x>>(CSF-TILE_S));
		y0 = (y>>(CSF-TILE_S))+ysize+1;
		for(xa=0;xa<xsize-2;xa+=16)
		{
			if ( TileProperty[p_map->at((x0+xa)>>TILE_S,y0>>TILE_S)].bup )
			{
				blockedd = true;
				return;
			}
		}

		if (!blockedd)	// check final point
		{
			if ( TileProperty[p_map->at((x0+xsize-2)>>TILE_S, y0>>TILE_S)].bup )
				blockedd = true;
		}

		// If the object is out of map
		if(y0 >= (int)((p_map->m_height-2)<<TILE_S))	blockedd = true;
	}

// set blockedu
	blockedu = false;
	x0 = (x>>(CSF-TILE_S));
	y0 = (y>>(CSF-TILE_S))-1;
	for(xa=1;xa<xsize;xa+=16)		// change start pixel to xa=1 for icecannon in ep1l8
	{
		if ( TileProperty[getmaptileat(x+xa,y)].bdown )
		{
			blockedu = 1;
			break;
		}
    }

	if (!blockedu)	// check final point
	{
		if ( TileProperty[p_map->at((x0+xsize-2)>>TILE_S, y0>>TILE_S)].bdown )
			blockedu = true;
	}

	// If the object is out of map
	if(y0 <= (2<<4)) blockedu = true;
/*
 // set blockedl
    blockedl = false;
    x0 = (x>>(CSF-TILE_S))-1;
    y0 = (y>>(CSF-TILE_S))+1;
    for(ya=0;ya<ysize;ya+=16)
    {

        if (TileProperty[getmaptileat(x,y+ya)][BRIGHT] || IsStopPoint(x,y+ya,o))
        {
          blockedl = 1;
          goto blockedl_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((y>>CSF)+ysize-1))][BRIGHT] ||
    		IsStopPoint(x, (y>>CSF)+ysize-1, o))
      blockedl = 1;

    blockedl_set: ;

	// If the object is out of map
	if(x <= (2<<4)) blockedl = 1;

 // set blockedr
    blockedr = 0;
    x = (x>>CSF)+xsize;
    y = (y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (TileProperty[getmaptileat(x,y+ya)][BLEFT] || IsStopPoint(x,y+ya,o))
        {
          blockedr = 1;
          goto blockedr_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((y>>CSF)+ysize-1))][BLEFT] ||
		IsStopPoint(x, (y>>CSF)+ysize-1, o))
    {
      blockedr = 1;
    }
    blockedr_set: ;

    if(x >= (int)((map.xsize-2)<<4)) blockedr = 1;

    // hit detection with players
    touchPlayer = 0;
    for(cplayer=0;cplayer<MAX_PLAYERS;cplayer++)
    {
      if (player[cplayer].isPlaying)
      {
        objects[player[cplayer].useObject].x = player[cplayer].x;
        objects[player[cplayer].useObject].y = player[cplayer].y;
        objects[player[cplayer].useObject].sprite = 0;
        if (!player[cplayer].pdie)
        {
          if (hitdetect(o, player[cplayer].useObject))
          {
			if (!player[cplayer].godmode)
			{
	            touchPlayer = 1;
	            touchedBy = cplayer;
			}
			else
			{
				if (type==OBJ_MOTHER || type==OBJ_BABY ||\
					type==OBJ_MEEP || type==OBJ_YORP)
				{
					if (canbezapped)
						zapped += 100;
				}
			}
            break;
          }
        }
      }
    }

// have object fall if it should
  if (!inhibitfall)
  {
       #define OBJFALLSPEED   20
       if (blockedd)
       {
         yinertia = 0;
       }
       else
       {
#define OBJ_YINERTIA_RATE  5
         if (yinertiatimer>OBJ_YINERTIA_RATE)
         {
           if (yinertia < OBJFALLSPEED) yinertia++;
           yinertiatimer = 0;
         } else yinertiatimer++;
       }
       y += yinertia;
  }*/
}


///
// Cleanup Routine
///
CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

