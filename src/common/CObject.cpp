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

//////////////////
// AI Processes //
//////////////////
///
// do object and enemy AI
///
bool CObject::checkforAIObject( CMap *p_map )
{
	if ( !exists || m_type==OBJ_PLAYER ) return false;

    //gamedo_calcenemyvisibility(i);

    // This will do the function gamedo_calcenemyvisibility(i);
    // check if object is really in the map!!!
    if (x < 0 || y < 0) return false;

    if (x > (p_map->m_width<<CSF) || y > (p_map->m_height<<CSF) )
    	  return false;

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
		return true;
    }
	return false;
}

void CObject::performSpecialAI( stOption *p_option, char episode )
{
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

	// set blockedl
    blockedl = false;
    x0 = (x>>(CSF-TILE_S))-1;
    y0 = (y>>(CSF-TILE_S))+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (TileProperty[p_map->at(x0>>TILE_S,(y0+ya)>>TILE_S)].bright )
        {
          blockedl = true;
          goto blockedl_set;
        }
    }
    if ( TileProperty[p_map->at(x0>>TILE_S, ((y0>>(CSF-TILE_S))+ysize-1)>>TILE_S)].bright )
      blockedl = true;

    blockedl_set: ;

	// If the object is out of map
	if(x <= (2<<4)) blockedl = true;

 // set blockedr
    blockedr = false;
    x0 = (x>>(CSF-TILE_S))+xsize;
    y0 = (y>>(CSF-TILE_S))+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if ( TileProperty[p_map->at(x0>>TILE_S,(y0+ya)>>TILE_S)].bleft )
        {
          blockedr = true;
          goto blockedr_set;
        }
    }
    if ( TileProperty[p_map->at(x0>>TILE_S, ((y0+ysize-1)>>TILE_S))].bleft )
    {
      blockedr = true;
    }
    blockedr_set: ;

    if(x >= (p_map->m_width-2)<<4) blockedr = true;

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
      }
}

// returns nonzero if object1 overlaps object2
bool CObject::hitdetect(CObject *p_hitobject)
{
CSprite *spr1, *spr2;
unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
unsigned int rect2x1, rect2y1, rect2x2, rect2y2;

  // get the sprites used by the two objects
  spr1 = g_pGfxEngine->Sprite.at(sprite);
  spr2 = g_pGfxEngine->Sprite.at(p_hitobject->sprite);

  // get the bounding rectangle of the first object
  rect1x1 = x + spr1->m_bboxX1;
  rect1y1 = y + spr1->m_bboxY1;
  rect1x2 = x + spr1->m_bboxX2;
  rect1y2 = y + spr1->m_bboxY2;

  // get the bounding rectangle of the second object
  rect2x1 = p_hitobject->x + spr2->m_bboxX1;
  rect2y1 = p_hitobject->y + spr2->m_bboxY1;
  rect2x2 = p_hitobject->x + spr2->m_bboxX2;
  rect2y2 = p_hitobject->y + spr2->m_bboxY2;

  // find out if the rectangles overlap
  if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return false;
  if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return false;
  if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return false;
  if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return false;

  return true;
}

///
// Cleanup Routine
///
CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

