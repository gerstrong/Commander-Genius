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
void CObject::performAI(int episode, stOption *p_option)
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
		 performCommonAI();
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
void CObject::performCommonAI()
{
/*int x,y,xa,ya,xsize,ysize;
int temp;
int cplayer;

	if (objects[o].type==OBJ_GOTPOINTS) return;


	ysize = g_pGfxEngine->Sprite[objects[o].sprite]->getHeight();
	xsize = g_pGfxEngine->Sprite[objects[o].sprite]->getWidth();

 // set value of blockedd--should object fall?
	temp = (objects[o].y>>CSF)+ysize;
	if ((temp>>TILE_S)<<TILE_S != temp)
	{
		objects[o].blockedd = 0;
	}
	else
	{ // on a tile boundary, test if tile under object is solid
		objects[o].blockedd = 0;
		x = (objects[o].x>>CSF);
		y = (objects[o].y>>CSF)+ysize+1;
		for(xa=0;xa<xsize-2;xa+=16)
		{
			if (TileProperty[getmaptileat(x+xa,y)][BUP] || IsStopPoint(x+xa,y,o))
			{
				objects[o].blockedd = 1;
				break;
			}
		}

		if (!objects[o].blockedd)	// check final point
		{
			if (TileProperty[getmaptileat(x+xsize-2, y)][BUP] || IsStopPoint(x+xsize-2,y,o))
			{
				objects[o].blockedd = 1;
			}
		}

		// If the object is out of map
		if(y >= (int)((map.ysize-2)<<4))	objects[o].blockedd = 1;
	}

// set blockedu
	objects[o].blockedu = 0;
	x = (objects[o].x>>CSF);
	y = (objects[o].y>>CSF)-1;
	for(xa=1;xa<xsize;xa+=16)		// change start pixel to xa=1 for icecannon in ep1l8
	{
		if (TileProperty[getmaptileat(x+xa,y)][BDOWN] || IsStopPoint(x+xa,y,o))
		{
			objects[o].blockedu = 1;
			break;
		}
    }

	if (!objects[o].blockedu)		// check final point
	{
		if (TileProperty[getmaptileat(x+xsize-2, y)][BDOWN] || IsStopPoint(x+xsize-2,y,o))
		{
			objects[o].blockedu = 1;
		}
	}

	// If the object is out of map
	if(y <= (2<<4)) objects[o].blockedu = 1;

 // set blockedl
    objects[o].blockedl = 0;
    x = (objects[o].x>>CSF)-1;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {

        if (TileProperty[getmaptileat(x,y+ya)][BRIGHT] || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedl = 1;
          goto blockedl_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BRIGHT] ||
    		IsStopPoint(x, (objects[o].y>>CSF)+ysize-1, o))
      objects[o].blockedl = 1;

    blockedl_set: ;

	// If the object is out of map
	if(x <= (2<<4)) objects[o].blockedl = 1;

 // set blockedr
    objects[o].blockedr = 0;
    x = (objects[o].x>>CSF)+xsize;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (TileProperty[getmaptileat(x,y+ya)][BLEFT] || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedr = 1;
          goto blockedr_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BLEFT] ||
		IsStopPoint(x, (objects[o].y>>CSF)+ysize-1, o))
    {
      objects[o].blockedr = 1;
    }
    blockedr_set: ;

    if(x >= (int)((map.xsize-2)<<4)) objects[o].blockedr = 1;

    // hit detection with players
    objects[o].touchPlayer = 0;
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
	            objects[o].touchPlayer = 1;
	            objects[o].touchedBy = cplayer;
			}
			else
			{
				if (objects[o].type==OBJ_MOTHER || objects[o].type==OBJ_BABY ||\
					objects[o].type==OBJ_MEEP || objects[o].type==OBJ_YORP)
				{
					if (objects[o].canbezapped)
						objects[o].zapped += 100;
				}
			}
            break;
          }
        }
      }
    }

// have object fall if it should
  if (!objects[o].inhibitfall)
  {
       #define OBJFALLSPEED   20
       if (objects[o].blockedd)
       {
         objects[o].yinertia = 0;
       }
       else
       {
#define OBJ_YINERTIA_RATE  5
         if (objects[o].yinertiatimer>OBJ_YINERTIA_RATE)
         {
           if (objects[o].yinertia < OBJFALLSPEED) objects[o].yinertia++;
           objects[o].yinertiatimer = 0;
         } else objects[o].yinertiatimer++;
       }
       objects[o].y += objects[o].yinertia;
  }*/
}


///
// Cleanup Routine
///
CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

