#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "ray.h"
#include "../include/game.h"

#include "../include/enemyai.h"

char PlatExtending=0;

// "Sector Effector" object (The name comes from D3D)...it's basically
// an object which can do a number of different things depending on it's
// .ai.se.type attribute, usually it affects the map or the enviorment
// around it. Used where it wasn't worth it to create a whole new object
// (or where I was too lazy to do it).

void se_extend_plat(int o);             // extends a platform (ep2)
void se_spark(int o, stLevelControl *p_levelcontrol);                   // spark in Tantalus Ray (ep2)
void se_gun(int o);                     // periodically-firing guns (ep3)
void se_ankhshield(int o);              // Ankh Shield (ep3)
void se_icecannon(int o);               // ice cannon (ep1)
void se_mortimer_arm(int o);            // Mortimer's arms (ep3)
void se_mortimer_leg_left(int o);       // Mortimer's left leg (ep3)
void se_mortimer_leg_right(int o, stCloneKeenPlus *pCKP);      // Mortimer's right leg (ep3)
void se_mortimer_spark(int o, stCloneKeenPlus *pCKP);          // spark in Mortimer's machine (ep3)
void se_mortimer_heart(int o);          // Mortimer's "heart" (ep3)
void se_mortimer_zapsup(int o, stCloneKeenPlus *pCKP);         // causes zaps to roll up Mortimer's machine (ep3)
void se_mortimer_randomzaps(int o);     // spawns random zaps on Mortimer's machine (ep3)
void se_retract_plat(int o);

void set_mortimer_surprised(int yes);
int mortimer_surprisedcount=0;

void se_ai(int o, stCloneKeenPlus *pCKP)
{

   switch(objects[o].ai.se.type)
   {
    case SE_EXTEND_PLATFORM: se_extend_plat(o); break;
    case SE_RETRACT_PLATFORM: se_retract_plat(o); break;
    case SE_SPARK: se_spark(o, (&pCKP->Control.levelcontrol)); break;
    case SE_GUN_VERT: se_gun(o); break;
    case SE_GUN_RIGHT: se_gun(o); break;
    case SE_ANKHSHIELD: se_ankhshield(o); break;
    case SE_ICECANNON: se_icecannon(o); break;
    case SE_MORTIMER_ARM: se_mortimer_arm(o); break;
    case SE_MORTIMER_LEG_LEFT: se_mortimer_leg_left(o); break;
    case SE_MORTIMER_LEG_RIGHT: se_mortimer_leg_right(o, pCKP); break;
    case SE_MORTIMER_SPARK: se_mortimer_spark(o, pCKP); break;
    case SE_MORTIMER_HEART: se_mortimer_heart(o); break;
    case SE_MORTIMER_ZAPSUP: se_mortimer_zapsup(o, pCKP); break;
    case SE_MORTIMER_RANDOMZAPS: se_mortimer_randomzaps(o); break;

    default: crashflag = 1;
             crashflag2 = objects[o].ai.se.type;
             why_term_ptr = "Invalid sector effector type (shown in flag2)";
             break;
   }

}


void se_extend_plat(int o)
{
//char buf[80];
    #define PLAT_EXTEND_RATE        30

      if (objects[o].needinit)
      {
        objects[o].ai.se.timer = 0;
        objects[o].inhibitfall = 1;
        objects[o].canbezapped = 0;
        objects[o].sprite = BlankSprite;
        objects[o].needinit = 0;

        // figure out which direction the bridge is supposed to go
        if (!TileProperty[map.mapdata[objects[o].ai.se.platx+1][objects[o].ai.se.platy]][BLEFT])
        //if (!tiles[map.mapdata[objects[o].ai.se.platx+1][objects[o].ai.se.platy]].solidl)
          objects[o].ai.se.dir = RIGHT;
        else
          objects[o].ai.se.dir = LEFT;
      }

      if (!objects[o].ai.se.timer)
      {
    	if (!TileProperty[map.mapdata[objects[o].ai.se.platx][objects[o].ai.se.platy]][BUP])
        //if (!tiles[map.mapdata[objects[o].ai.se.platx][objects[o].ai.se.platy]].solidfall)
        {
          map_chgtile(objects[o].ai.se.platx, objects[o].ai.se.platy, TILE_EXTENDING_PLATFORM);

          if (objects[o].ai.se.dir==RIGHT)
            objects[o].ai.se.platx++;
          else
            objects[o].ai.se.platx--;

          objects[o].ai.se.timer = PLAT_EXTEND_RATE;
        }
        else
        {
          objects[o].exists = 0;
          PlatExtending = 0;
        }
      }
      else objects[o].ai.se.timer--;

}

void se_retract_plat(int o)
{
      if (objects[o].needinit)
      {
        objects[o].ai.se.timer = PLAT_EXTEND_RATE;
        objects[o].inhibitfall = 1;
        objects[o].canbezapped = 0;
        objects[o].sprite = BlankSprite;
        objects[o].needinit = 0;

        // get the background tile from the tile above the starting point
        objects[o].ai.se.bgtile = map.mapdata[objects[o].ai.se.platx][objects[o].ai.se.platy-1];
        // figure out which direction the bridge is supposed to go
        if (map.mapdata[objects[o].ai.se.platx-1][objects[o].ai.se.platy] != TILE_EXTENDING_PLATFORM)
          objects[o].ai.se.dir = LEFT;
        else
          objects[o].ai.se.dir = RIGHT;

        // scan across until we find the end of the platform--that will
        // be where we will start (remove platform in oppisote direction
        // it was extended)
        do
        {
          if (map.mapdata[objects[o].ai.se.platx][objects[o].ai.se.platy] != TILE_EXTENDING_PLATFORM)
          { // we've found the end of the platform
            break;
          }
          if (objects[o].ai.se.dir==LEFT)
          {
            if (objects[o].ai.se.platx==map.xsize)
            {
              crashflag = 1;
              why_term_ptr = "SE_RETRACT_PLATFORM: Failed to find end of platform when scanning right.";
              return;
            }
            objects[o].ai.se.platx++;
          }
          else
          { // platform will be removed in a right-going direction
            if (objects[o].ai.se.platx==0)
            {
              crashflag = 1;
              why_term_ptr = "SE_RETRACT_PLATFORM: Failed to find end of platform when scanning left.";
              return;
            }
            objects[o].ai.se.platx--;
          }
        } while(1);

        // when we were scanning we went one tile too far, go back one
        if (objects[o].ai.se.dir==LEFT) objects[o].ai.se.platx--;
                                   else objects[o].ai.se.platx++;
      }

      if (objects[o].ai.se.timer >= PLAT_EXTEND_RATE)
      {
        if (map.mapdata[objects[o].ai.se.platx][objects[o].ai.se.platy]==TILE_EXTENDING_PLATFORM)
        {
          map_chgtile(objects[o].ai.se.platx, objects[o].ai.se.platy, objects[o].ai.se.bgtile);

          if (objects[o].ai.se.dir==RIGHT)
            objects[o].ai.se.platx++;
          else
            objects[o].ai.se.platx--;

          objects[o].ai.se.timer = 0;
        }
        else
        {
          objects[o].exists = 0;
          PlatExtending = 0;
        }
      }
      else objects[o].ai.se.timer++;

}


void se_spark(int o, stLevelControl *p_levelcontrol)
{
int newobject;
int mx,my,x,y;

    #define SPARK_BASEFRAME         128
    #define SPARK_ANIMRATE          20

    #define SPARK_ANIMATE           0
    #define SPARK_BLOWUP1           1
    #define SPARK_BLOWUP2           2
    #define SPARK_BLOWUP3           3

    #define SPARK_BLOW_DELAY        100

      if (objects[o].needinit)
      {
        objects[o].ai.se.state = SPARK_ANIMATE;
        objects[o].ai.se.timer = 0;
        objects[o].canbezapped = 1;
        objects[o].inhibitfall = 1;
        objects[o].needinit = 0;

        objects[o].ai.se.mx = objects[o].x >> CSF >> 4;
        objects[o].ai.se.my = objects[o].y >> CSF >> 4;
      }

      if (objects[o].ai.se.state==SPARK_ANIMATE)
      {
         objects[o].sprite = SPARK_BASEFRAME + objects[o].ai.se.frame;
      }
      else
      {
         objects[o].sprite = BlankSprite;
      }

      switch(objects[o].ai.se.state)
      {
      case SPARK_ANIMATE:
         if (objects[o].ai.se.timer > SPARK_ANIMRATE)
         {
           objects[o].ai.se.frame++;
           if (objects[o].ai.se.frame > 3) objects[o].ai.se.frame = 0;
           objects[o].ai.se.timer = 0;
         } else objects[o].ai.se.timer++;

         if (objects[o].zapped)
         {
           g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
           // break the glass and blow out the electric arcs
           map_chgtile(objects[o].ai.se.mx - 2, objects[o].ai.se.my, 492);
           map_chgtile(objects[o].ai.se.mx - 1, objects[o].ai.se.my, 546);
           map_chgtile(objects[o].ai.se.mx, objects[o].ai.se.my, 547);
           map_chgtile(objects[o].ai.se.mx + 1, objects[o].ai.se.my, 548);
           map_chgtile(objects[o].ai.se.mx + 2, objects[o].ai.se.my, 492);
           map_deanimate(objects[o].ai.se.mx - 2, objects[o].ai.se.my);
           map_deanimate(objects[o].ai.se.mx + 2, objects[o].ai.se.my);
           // remove the unneeded dome tiles
           map_chgtile(objects[o].ai.se.mx - 1, objects[o].ai.se.my-1, BG_GRAY);
           map_chgtile(objects[o].ai.se.mx, objects[o].ai.se.my-1, BG_GRAY);
           map_chgtile(objects[o].ai.se.mx + 1, objects[o].ai.se.my-1, BG_GRAY);
           // break the switch
           map_chgtile(objects[o].ai.se.mx - 3, objects[o].ai.se.my + 4, 506);

           // meltdown!
           objects[o].ai.se.state = SPARK_BLOWUP1;
           objects[o].ai.se.timer = 0;
           objects[o].ai.se.blowy = 0;

           fade.mode = FADE_GO;
           fade.dir = FADE_IN;
           fade.curamt = PAL_FADE_WHITEOUT;
           fade.fadetimer = 0;
           fade.rate = FADE_NORM;
         }
      break;
      case SPARK_BLOWUP1:
         // one by one blow out the purple thingies below the device
         if (objects[o].ai.se.timer > SPARK_BLOW_DELAY)
         {
            objects[o].ai.se.timer = 0;
            mx = objects[o].ai.se.mx;
            my = objects[o].ai.se.my+3+objects[o].ai.se.blowy;
            map_chgtile(mx, my, 505);
            map_deanimate(mx, my);
            // spawn a ZAP! or a ZOT!
            newobject = spawn_object(mx<<4<<CSF, my<<4<<CSF, OBJ_RAY);
            objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
            objects[newobject].inhibitfall = 1;
            objects[newobject].needinit = 0;
            objects[newobject].ai.ray.dontHitEnable = 0;
            g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);

            objects[o].ai.se.blowy++;
            if (objects[o].ai.se.blowy >= 3)
            {
              objects[o].ai.se.state = SPARK_BLOWUP2;
              objects[o].ai.se.blowx = 0;
            }
         }
         else objects[o].ai.se.timer++;
      break;
      case SPARK_BLOWUP2:
         // blow out the glowing cells
         if (objects[o].ai.se.timer > SPARK_BLOW_DELAY)
         {
            if (objects[o].ai.se.blowx >= 4)
            {
                // done blowing up the glowcells
                // static the targeting display
                mx = objects[o].ai.se.mx - 7;
                my = objects[o].ai.se.my + 2;
                for(y=0;y<3;y++)
                {
                  for(x=0;x<3;x++)
                  {
                    map_chgtile(mx+x,my+y,533);
                    map_animate(mx+x,my+y);
                  }
                }
                objects[o].exists = 0;
                p_levelcontrol->canexit = 1;       // ok, you can exit now
                return;
            }

            objects[o].ai.se.timer = 0;
            mx = objects[o].ai.se.mx + objects[o].ai.se.blowx + 3;
            for(y=0;y<3;y++)
            {
              my = objects[o].ai.se.my+3+y;
              map_chgtile(mx, my, 549);
              map_deanimate(mx, my);
              // spawn a ZAP! or a ZOT!
              newobject = spawn_object(mx<<4<<CSF, my<<4<<CSF, OBJ_RAY);
              objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
              objects[newobject].inhibitfall = 1;
              objects[newobject].needinit = 0;
            }

            g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
            objects[o].ai.se.blowx++;
         }
         else objects[o].ai.se.timer++;
      break;
     }  // end of state switch for SE_SPARK

}

void se_gun(int o)
{
int newobject;
        if (objects[o].needinit)
        {
          objects[o].ai.se.timer = 0;
          objects[o].sprite = BlankSprite;
          objects[o].inhibitfall = 1;
          objects[o].needinit = 0;
        }

        if (!gunfiretimer)
        {
          if (objects[o].ai.se.type==SE_GUN_VERT)
          {
            newobject = spawn_object(objects[o].x+(4<<CSF), objects[o].y+(1<<CSF), OBJ_RAY);
            objects[newobject].sprite = RAY_VERT_EP3;
            objects[newobject].ai.ray.direction = DOWN;
          }
          else if (objects[o].ai.se.type==SE_GUN_RIGHT)
          {
            newobject = spawn_object(objects[o].x+(1<<CSF), objects[o].y+(2<<CSF), OBJ_RAY);
            objects[newobject].sprite = ENEMYRAYEP3;
            objects[newobject].ai.ray.direction = RIGHT;
          }
          if((objects[o].scrx > 0) && (objects[o].scrx < 255))
			  g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].scrx);
		}
	}

void se_ankhshield(int o)
{
        #define ANKH_FLICKER_FREQ       12

        if (objects[o].needinit)
        {
          objects[o].ai.se.frame = 0;
          objects[o].ai.se.timer = 0;
          objects[o].inhibitfall = 1;
          objects[o].canbezapped = 0;
          objects[o].needinit = 0;

          objects[o].ai.se.state = ANKH_STATE_NOFLICKER;
        }

        switch(objects[o].ai.se.state)
        {
         case ANKH_STATE_NOFLICKER:
           objects[o].sprite = ANKH_SHIELD_FRAME + (objects[o].ai.se.frame&1);
           break;
         case ANKH_STATE_FLICKERFAST:
           if (objects[o].ai.se.frame&1)
             objects[o].sprite = BlankSprite;
           else
           {
             if (objects[o].ai.se.frame&2)
               objects[o].sprite = ANKH_SHIELD_FRAME+1;
             else
               objects[o].sprite = ANKH_SHIELD_FRAME;
           }
           break;
          case ANKH_STATE_FLICKERSLOW:
           if (objects[o].ai.se.frame>4)
             objects[o].sprite = BlankSprite;
           else
             objects[o].sprite = ANKH_SHIELD_FRAME;
           break;
        }

        if (objects[o].ai.se.timer > ANKH_FLICKER_FREQ)
        {
          objects[o].ai.se.frame++;
          if (objects[o].ai.se.frame>8) objects[o].ai.se.frame = 0;
          objects[o].ai.se.timer = 0;
        }
        else objects[o].ai.se.timer++;


}


void se_icecannon(int o)
{
int newobject;

   if (!gunfiretimer)
   {
       newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICECHUNK);
       objects[newobject].ai.icechunk.movedir = objects[o].ai.se.dir;
   }
}

#define ARM_GO          0
#define ARM_WAIT        1

#define ARM_MOVE_SPEED   50
#define ARM_WAIT_TIME    70
void se_mortimer_arm(int o)
{
int mx,my;
   if (objects[o].needinit)
   {
     objects[o].ai.se.dir = DOWN;
     objects[o].ai.se.state = ARM_GO;
     objects[o].ai.se.timer = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }

   switch(objects[o].ai.se.state)
   {
    case ARM_GO:
    // vertical arm 618 620 619
    // pole 597
    // polka dot background 169
      if (objects[o].ai.se.timer > ARM_MOVE_SPEED)
      {
        mx = objects[o].x >> CSF >> 4;
        my = objects[o].y >> CSF >> 4;

        if (objects[o].ai.se.dir==DOWN)
        {
           // reached bottom?
           if (map.mapdata[mx][my+3]==471)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = ARM_WAIT;
           }
           else
           {
              // add to the pole
              map_chgtile(mx, my+1, 597);
              // create left side of pincher
              map_chgtile(mx-1, my+1, 618);
              map_chgtile(mx-1, my+2, 620);
              map_chgtile(mx-1, my+3, 619);
              // create right side of pincher
              map_chgtile(mx+1, my+1, 618);
              map_chgtile(mx+1, my+2, 620);
              map_chgtile(mx+1, my+3, 619);
              // erase the top of the pincher we don't need anymore
              map_chgtile(mx-1, my, 169);
              map_chgtile(mx+1, my, 169);
              objects[o].y += (16<<CSF);
           }
        }
        else
        {  // arm going up

           // reached top?
           if (map.mapdata[mx][my+1]==619)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = ARM_WAIT;
           }
           else
           {
              // create left side of pincher
              map_chgtile(mx-1, my+1, 618);
              map_chgtile(mx-1, my+2, 620);
              map_chgtile(mx-1, my+3, 619);
              // create right side of pincher
              map_chgtile(mx+1, my+1, 618);
              map_chgtile(mx+1, my+2, 620);
              map_chgtile(mx+1, my+3, 619);
              // erase the bottom of the pincher we don't need anymore
              map_chgtile(mx-1, my+4, 169);
              map_chgtile(mx+1, my+4, 169);
              // erase the pole
              map_chgtile(mx, my+2, 169);

              objects[o].y -= (16<<CSF);
           }
        }
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
    case ARM_WAIT:
      if (objects[o].ai.se.timer > ARM_WAIT_TIME)
      {
        if (objects[o].ai.se.dir==DOWN)
        {
          objects[o].ai.se.dir = UP;
          objects[o].y -= (32<<CSF);
        }
        else
        {
          objects[o].ai.se.dir = DOWN;
          objects[o].y += (16<<CSF);
        }

        objects[o].ai.se.state = ARM_GO;
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
   }
}

#define MORTIMER_SPARK_BASEFRAME        114

#define MORTIMER_LEFT_ARM_X             5
#define MORTIMER_RIGHT_ARM_X            17
#define MORTIMER_ARMS_YSTART            7
#define MORTIMER_ARMS_YEND              18

#define ARMS_DESTROY_RATE        50

#define MSPARK_IDLE              0
#define MSPARK_DESTROYARMS       1
void se_mortimer_spark(int o, stCloneKeenPlus *pCKP)
{
int x,mx,i;
int newobject;
    if (objects[o].needinit)
    {
      objects[o].ai.se.state = MSPARK_IDLE;
      objects[o].ai.se.timer = 0;
      objects[o].ai.se.frame = 0;
      objects[o].inhibitfall = 1;
      objects[o].canbezapped = 1;
      objects[o].needinit = 0;
    }

    switch(objects[o].ai.se.state)
    {
    case MSPARK_IDLE:
        objects[o].sprite = MORTIMER_SPARK_BASEFRAME + objects[o].ai.se.frame;

        // deadly to the touch
        if (objects[o].touchPlayer)
        {
           killplayer(objects[o].touchedBy, pCKP);
        }

        if (objects[o].ai.se.timer > SPARK_ANIMRATE)
        {
           objects[o].ai.se.frame++;
           if (objects[o].ai.se.frame > 3) objects[o].ai.se.frame = 0;
           objects[o].ai.se.timer = 0;
        }
        else objects[o].ai.se.timer++;

        if (objects[o].zapped)
        {
           set_mortimer_surprised(1);

           // if there are any sparks left, destroy the spark,
           // else destroy mortimer's arms
           for(i=0;i<MAX_OBJECTS;i++)
           {
             if (objects[i].type==OBJ_SECTOREFFECTOR &&\
                 objects[i].ai.se.type==SE_MORTIMER_SPARK &&\
                 objects[i].exists)
             {
               if (i!=o)
               {
                  objects[o].ai.se.type = SE_MORTIMER_RANDOMZAPS;
                  objects[o].needinit = 1;
                  return;
               }
             }
           }

           /* destroy mortimer's arms */
           objects[o].sprite = BlankSprite;

           // destroy the sector effectors controlling his arms
           for(i=0;i<MAX_OBJECTS;i++)
           {
             if (objects[i].type==OBJ_SECTOREFFECTOR && \
                 objects[i].ai.se.type==SE_MORTIMER_ARM)
             {
                objects[i].exists = 0;
             }
           }
           // go into a state to destroy mortimer's arms
           objects[o].ai.se.state = MSPARK_DESTROYARMS;
           objects[o].ai.se.my = MORTIMER_ARMS_YSTART;
           objects[o].ai.se.timer = 0;
        }
    break;
    case MSPARK_DESTROYARMS:
      if (!objects[o].ai.se.timer)
      {
        for(x=0;x<3;x++)
        {
           mx = MORTIMER_LEFT_ARM_X+x;
           if (map.mapdata[mx][objects[o].ai.se.my]!=169)
           {
             //if (tiles[map.mapdata[mx][objects[o].ai.se.my]].isAnimated)
             if (TileProperty[map.mapdata[mx][objects[o].ai.se.my]][ANIMATION] > 1)
             {
                map_deanimate(mx, objects[o].ai.se.my);
             }
             map_chgtile(mx, objects[o].ai.se.my, 169);
             // spawn a ZAP! or a ZOT!
             newobject = spawn_object(((mx<<4)+4)<<CSF, objects[o].ai.se.my<<4<<CSF, OBJ_RAY);
             objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
             objects[newobject].ai.ray.direction = DOWN;
             objects[newobject].inhibitfall = 1;
             objects[newobject].needinit = 0;
           }

           mx = MORTIMER_RIGHT_ARM_X+x;
           if (map.mapdata[mx][objects[o].ai.se.my]!=169)
           {
             if (TileProperty[map.mapdata[mx][objects[o].ai.se.my]][ANIMATION] > 1)
             {
                map_deanimate(mx, objects[o].ai.se.my);
             }
             map_chgtile(mx, objects[o].ai.se.my, 169);
             // spawn a ZAP! or a ZOT!
             newobject = spawn_object(((mx<<4)+4)<<CSF, objects[o].ai.se.my<<4<<CSF, OBJ_RAY);
             objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
             objects[newobject].ai.ray.direction = DOWN;
             objects[newobject].inhibitfall = 1;
             objects[newobject].needinit = 0;
           }

        }
        objects[o].ai.se.timer = ARMS_DESTROY_RATE;
        objects[o].ai.se.my++;
        if (objects[o].ai.se.my > MORTIMER_ARMS_YEND)
        {
          objects[o].exists = 0;
          set_mortimer_surprised(0);
        }
      }
      else objects[o].ai.se.timer--;
    break;
   }
}

#define MORTIMER_HEART_BASEFRAME        146
#define HEART_ANIMRATE                  60

#define HEART_IDLE              0
#define HEART_ZAPSRUNUP         1
#define HEART_ZAPSRUNDOWN       2

#define MORTIMER_MACHINE_YSTART         3
#define MORTIMER_MACHINE_YEND           18
#define MORTIMER_MACHINE_YENDNOLEGS     14

#define MORTIMER_MACHINE_XSTART         8
#define MORTIMER_MACHINE_XEND           17

#define MACHINE_DESTROY_RATE            50
#define MORTIMER_ZAPWAVESPACING        200
#define MORTIMER_NUMZAPWAVES             5

#define ZAPSUP_NORMAL           0
#define ZAPSUP_ABOUTTOFADEOUT   1
void se_mortimer_heart(int o)
{
int i,x;
int newobject;

    if (objects[o].needinit)
    {
      objects[o].ai.se.timer = 0;
      objects[o].ai.se.frame = 0;
      objects[o].ai.se.state = HEART_IDLE;
      objects[o].inhibitfall = 1;
      objects[o].canbezapped = 1;
      objects[o].needinit = 0;
      mortimer_surprisedcount = 0;
    }

    switch(objects[o].ai.se.state)
    {
    case HEART_IDLE:
        objects[o].sprite = MORTIMER_HEART_BASEFRAME + objects[o].ai.se.frame;

        if (objects[o].ai.se.timer > HEART_ANIMRATE)
        {
           objects[o].ai.se.frame ^= 1;
           objects[o].ai.se.timer = 0;
        }
        else objects[o].ai.se.timer++;

        if (objects[o].zapped)
        {
           objects[o].sprite = BlankSprite;
           set_mortimer_surprised(1);

           /* destroy Mortimer's machine */

           // kill all enemies
           for(i=0;i<MAX_OBJECTS;i++)
           {
              if (objects[i].type!=OBJ_PLAYER && i!=o)
              {
                 objects[i].exists = 0;
              }
           }

           set_mortimer_surprised(1);
           // have waves of zaps run up mortimer's machine
           objects[o].ai.se.timer = 0;
           objects[o].ai.se.state = HEART_ZAPSRUNUP;
           objects[o].ai.se.counter = 0;
        }
    break;
    case HEART_ZAPSRUNUP:
        if (!objects[o].ai.se.timer)
        {
           // spawn another wave of zaps
           newobject = spawn_object(objects[o].x, objects[o].y, OBJ_SECTOREFFECTOR);
           objects[newobject].ai.se.type = SE_MORTIMER_ZAPSUP;
           objects[newobject].ai.se.my = MORTIMER_MACHINE_YEND;
           objects[newobject].ai.se.timer = 0;
           objects[newobject].ai.se.destroytiles = 0;
           objects[newobject].ai.se.state = ZAPSUP_NORMAL;
           objects[newobject].hasbeenonscreen = 1;

           objects[o].ai.se.timer = MORTIMER_ZAPWAVESPACING;
           if (objects[o].ai.se.counter > MORTIMER_NUMZAPWAVES)
           {
              objects[newobject].ai.se.destroytiles = 1;
              objects[o].exists = 0;
           }
           else objects[o].ai.se.counter++;
        }
        else objects[o].ai.se.timer--;
    break;
    case HEART_ZAPSRUNDOWN:
      if (!objects[o].ai.se.timer)
      {
        for(x=MORTIMER_MACHINE_XSTART;x<MORTIMER_MACHINE_XEND;x++)
        {
             // delete the tile
             //if (tiles[map.mapdata[x][objects[o].ai.se.my]].isAnimated)
             if (TileProperty[map.mapdata[x][objects[o].ai.se.my]][ANIMATION])
             {
                map_deanimate(x,objects[o].ai.se.my);
             }
             map_chgtile(x,objects[o].ai.se.my,169);
             // spawn a ZAP! or a ZOT!
             newobject = spawn_object(((x<<4)+4)<<CSF, objects[o].ai.se.my<<4<<CSF, OBJ_RAY);
             objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
             objects[newobject].ai.ray.direction = DOWN;
             objects[newobject].inhibitfall = 1;
             objects[newobject].needinit = 0;
        }

        objects[o].ai.se.timer = MACHINE_DESTROY_RATE;
        if (objects[o].ai.se.my > MORTIMER_MACHINE_YEND)
        {
          objects[o].exists = 0;
        }
        else objects[o].ai.se.my++;
      }
      else objects[o].ai.se.timer--;
    break;
    }
}

#define TIME_AFTER_DESTROY_BEFORE_FADEOUT       500
void se_mortimer_zapsup(int o, stCloneKeenPlus *pCKP)
{
int x, newobject;

      if (!objects[o].ai.se.timer)
      {
        if (objects[o].ai.se.state==ZAPSUP_ABOUTTOFADEOUT)
        {
        	pCKP->Control.levelcontrol.level_done = LEVEL_DONE_FADEOUT;
            endlevel(1,pCKP);
            objects[o].exists = 0;
            return;
        }

        for(x=MORTIMER_MACHINE_XSTART;x<MORTIMER_MACHINE_XEND;x++)
        {
             // spawn a ZAP! or a ZOT!
             newobject = spawn_object(((x<<4)+4)<<CSF, objects[o].ai.se.my<<4<<CSF, OBJ_RAY);
             objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
             objects[newobject].ai.ray.direction = DOWN;
             objects[newobject].inhibitfall = 1;
             objects[newobject].needinit = 0;

             if (objects[o].ai.se.destroytiles)
             {
                // delete the tile
                //if (tiles[map.mapdata[x][objects[o].ai.se.my]].isAnimated)
            	if (TileProperty[map.mapdata[x][objects[o].ai.se.my]][ANIMATION])
                {
                   map_deanimate(x,objects[o].ai.se.my);
                }
                map_chgtile(x,objects[o].ai.se.my,169);
             }
        }

        objects[o].ai.se.timer = MACHINE_DESTROY_RATE;
        if (objects[o].ai.se.my <= MORTIMER_MACHINE_YSTART)
        {
          if (objects[o].ai.se.destroytiles)
          {
            // last wave, prepare to initiate level fadeout
            objects[o].ai.se.timer = TIME_AFTER_DESTROY_BEFORE_FADEOUT;
            objects[o].ai.se.state = ZAPSUP_ABOUTTOFADEOUT;
            return;
          }
          else
          {
             objects[o].exists = 0;
             objects[o].ai.se.timer = 0;
          }
        }
        else objects[o].ai.se.my--;
      }
      else objects[o].ai.se.timer--;
}

#define LEG_GO          0
#define LEG_WAIT        1

#define LEFTLEG_MOVE_SPEED   60
#define LEFTLEG_WAIT_TIME    150

#define RIGHTLEG_MOVE_SPEED   80
#define RIGHTLEG_WAIT_TIME    120
void se_mortimer_leg_left(int o)
{
int mx,my;
   if (objects[o].needinit)
   {
     objects[o].ai.se.dir = UP;
     objects[o].ai.se.state = LEG_GO;
     objects[o].ai.se.timer = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }

   switch(objects[o].ai.se.state)
   {
    case LEG_GO:
    // leg tiles 621 623 622
    // pole 597
    // polka dot background 169
    // bottom marker for leg 430
    // top marker for leg 420
      if (objects[o].ai.se.timer > LEFTLEG_MOVE_SPEED)
      {
        mx = objects[o].x >> CSF >> 4;
        my = objects[o].y >> CSF >> 4;

        if (objects[o].ai.se.dir==DOWN)
        {
           // reached bottom?
           if (map.mapdata[mx][my+1]==430)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = LEG_WAIT;
              g_pSound->playStereofromCoord(SOUND_FOOTSLAM, PLAY_NOW, objects[o].scrx);
           }
           else
           {
              // create the leg
              map_chgtile(mx-3,my+1,621);
              map_chgtile(mx-2,my+1,623);
              map_chgtile(mx-1,my+1,623);
              map_chgtile(mx-0,my+1,622);
              // erase the tiles above us that used to be the leg
              map_chgtile(mx-3,my,169);
              map_chgtile(mx-2,my,169);
              map_chgtile(mx-1,my,169);
              map_chgtile(mx-0,my,597);         // add to pole

              objects[o].y += (16<<CSF);
           }
        }
        else
        {  // leg going up

           // reached top?
           if (map.mapdata[mx][my-1]==420)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = LEG_WAIT;
           }
           else
           {
              // create the leg
              map_chgtile(mx-3,my-1,621);
              map_chgtile(mx-2,my-1,623);
              map_chgtile(mx-1,my-1,623);
              map_chgtile(mx-0,my-1,622);
              // erase the tiles beneath us that used to be the leg
              map_chgtile(mx-3,my,169);
              map_chgtile(mx-2,my,169);
              map_chgtile(mx-1,my,169);
              map_chgtile(mx-0,my,169);

              objects[o].y -= (16<<CSF);
           }
        }
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
    case LEG_WAIT:
      if (objects[o].ai.se.timer > LEFTLEG_WAIT_TIME)
      {
        if (objects[o].ai.se.dir==DOWN)
        {
          objects[o].ai.se.dir = UP;
        }
        else
        {
          objects[o].ai.se.dir = DOWN;
        }

        objects[o].ai.se.state = LEG_GO;
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
   }
}

void se_mortimer_leg_right(int o, stCloneKeenPlus *pCKP)
{
int mx,my;
  if (objects[o].touchPlayer)
  {
     killplayer(objects[o].touchedBy, pCKP);
  }
   if (objects[o].needinit)
   {
     objects[o].ai.se.dir = UP;
     objects[o].ai.se.state = LEG_GO;
     objects[o].ai.se.timer = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }

   switch(objects[o].ai.se.state)
   {
    case LEG_GO:
    // leg tiles 621 623 622
    // pole 597
    // polka dot background 169
    // bottom marker for leg 430
    // top marker for leg 420
      if (objects[o].ai.se.timer > RIGHTLEG_MOVE_SPEED)
      {
        mx = objects[o].x >> CSF >> 4;
        my = objects[o].y >> CSF >> 4;

        if (objects[o].ai.se.dir==DOWN)
        {
           // reached bottom?
           if (map.mapdata[mx][my+1]==430)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = LEG_WAIT;
              g_pSound->playStereofromCoord(SOUND_FOOTSLAM, PLAY_NOW, objects[o].scrx);
           }
           else
           {
              // create the leg
              map_chgtile(mx+3,my+1,622);
              map_chgtile(mx+2,my+1,623);
              map_chgtile(mx+1,my+1,623);
              map_chgtile(mx+0,my+1,621);
              // erase the tiles above us that used to be the leg
              map_chgtile(mx+3,my,169);
              map_chgtile(mx+2,my,169);
              map_chgtile(mx+1,my,169);
              map_chgtile(mx+0,my,597);         // add to pole

              objects[o].y += (16<<CSF);
           }
        }
        else
        {  // leg going up

           // reached top?
           if (map.mapdata[mx][my-1]==420)
           {
              objects[o].ai.se.timer = 0;
              objects[o].ai.se.state = LEG_WAIT;
           }
           else
           {
              // create the leg
              map_chgtile(mx+3,my-1,622);
              map_chgtile(mx+2,my-1,623);
              map_chgtile(mx+1,my-1,623);
              map_chgtile(mx+0,my-1,621);
              // erase the tiles beneath us that used to be the leg
              map_chgtile(mx+3,my,169);
              map_chgtile(mx+2,my,169);
              map_chgtile(mx+1,my,169);
              map_chgtile(mx+0,my,169);

              objects[o].y -= (16<<CSF);
           }
        }
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
    case LEG_WAIT:
      if (objects[o].ai.se.timer > RIGHTLEG_WAIT_TIME)
      {
        if (objects[o].ai.se.dir==DOWN)
        {
          objects[o].ai.se.dir = UP;
        }
        else
        {
          objects[o].ai.se.dir = DOWN;
        }

        objects[o].ai.se.state = LEG_GO;
        objects[o].ai.se.timer = 0;
      }
      else objects[o].ai.se.timer++;
    break;
   }
}

#define NUM_RANDOM_ZAPS         30
#define TIME_BETWEEN_ZAPS       10
void se_mortimer_randomzaps(int o)
{
int x,y;
int newobject;
  if (objects[o].needinit)
  {
    objects[o].sprite = BlankSprite;
    objects[o].ai.se.counter = 0;
    objects[o].ai.se.timer = 0;
    objects[o].needinit = 0;
  }

  if (!objects[o].ai.se.timer)
  {
    x = rand()%((MORTIMER_MACHINE_XEND*16)-(MORTIMER_MACHINE_XSTART*16))+(MORTIMER_MACHINE_XSTART*16);
    y = rand()%((MORTIMER_MACHINE_YENDNOLEGS*16)-(MORTIMER_MACHINE_YSTART*16))+(MORTIMER_MACHINE_YSTART*16);

    // spawn a ZAP! or a ZOT!
    newobject = spawn_object(x<<CSF, y<<CSF, OBJ_RAY);
    objects[newobject].ai.ray.state = RAY_STATE_SETZAPZOT;
    objects[newobject].inhibitfall = 1;
    objects[newobject].needinit = 0;

    objects[o].ai.se.timer = TIME_BETWEEN_ZAPS;
    if (objects[o].ai.se.counter > NUM_RANDOM_ZAPS)
    {
      set_mortimer_surprised(0);
      objects[o].exists = 0;
    }
    else objects[o].ai.se.counter++;
  }
  else objects[o].ai.se.timer--;
}

void set_mortimer_surprised(int yes)
{
    if (yes)
    {
      mortimer_surprisedcount++;
    }
    else
    {
      if (mortimer_surprisedcount>0) mortimer_surprisedcount--;
    }

    if (mortimer_surprisedcount)
    {
      //12,6 -> 610 -- give mortimer his "surprised" face
      map_chgtile(12,6,610);
      map_deanimate(12,6);
      // deanimate mortimer's hands
      map_chgtile(11,6,613);
      map_deanimate(11,6);
      map_chgtile(13,6,615);
      map_deanimate(13,6);
    }
    else
    {
      // give mortimer his normal face
      map_chgtile(12,6,607);
      map_animate(12,6);
      // reanimate things
      map_animate(11,6);
      map_chgtile(13,6,616);
      map_animate(13,6);
    }
}
