#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../include/game.h"

#include "enemyai.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "ray.h"

#define Sprite g_pGfxEngine->Sprite

void ray_ai(int o, int episode, bool automatic_raygun, char pShotSpeed)
{
/*int i;
int hitlethal;
int rayspeed;
  if (objects[o].needinit)
  {
    objects[o].ai.ray.state = RAY_STATE_FLY;
    objects[o].inhibitfall = 1;
    objects[o].needinit = 0;

    // if we shoot directly up against a wall have
    // the ZAP appear next to the wall, not in it
    if (objects[o].ai.ray.direction==RIGHT && objects[o].blockedr)
    {
       objects[o].x = (objects[o].x >> 4 >> CSF) << 4 << CSF;
       if (TileProperty[getmaptileat(objects[o].x>>CSF,objects[o].y>>CSF)][BLEFT])
    	   objects[o].x -= (16<<CSF);

       objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;

       if (objects[o].onscreen)
    	   g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
    }
    else if (objects[o].ai.ray.direction==LEFT && objects[o].blockedl)
    {
       objects[o].x = (objects[o].x >> 4 >> CSF) << 4 << CSF;
       if (TileProperty[getmaptileat(objects[o].x>>CSF,objects[o].y>>CSF)][BRIGHT]) objects[o].x += (16<<CSF);
       //if (tiles[getmaptileat(objects[o].x>>CSF,objects[o].y>>CSF)].solidl) objects[o].x -= (16<<CSF);
       objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
       if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
    }
  }

  // shots from "fully automatic" raygun go faster
  if (objects[o].sprite!=objdefsprites[OBJ_RAY] || !automatic_raygun)
  {
	if (!pShotSpeed)
		rayspeed = RAY_SPEED;
	else
		rayspeed = pShotSpeed;
  }
  else
	rayspeed = RAY_AUTO_SPEED;


  switch(objects[o].ai.ray.state)
  {
  case RAY_STATE_FLY:
       // test if it hit a baddie
       for(i=1 ; i<highest_objslot ; i++)
       {
         if (!objects[i].exists || i==o) continue;
         if (objects[o].ai.ray.dontHitEnable)
         {
           if (objects[i].type==objects[o].ai.ray.dontHit) continue;
         }

		 if (objects[i].type==OBJ_RAY) continue;

		 if (objects[i].canbezapped && objects[i].onscreen)
         {
            if (hitdetect(i, o))
            {
              objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
              objects[i].zapped++;
              objects[i].zapx = objects[o].x;
              objects[i].zapy = objects[o].y;
              objects[i].zapd = objects[o].ai.ray.direction;
              if (objects[o].sprite==ENEMYRAY || objects[o].sprite==ENEMYRAYEP2 || objects[o].sprite==ENEMYRAYEP3)
					objects[i].zappedbyenemy = 1;
              else
					objects[i].zappedbyenemy = 0;

            }
         }
       }
       // check if ray hit keen. if canpk=0, only enemy rays can hurt keen
       if (objects[o].touchPlayer)
       {
         if (player[objects[o].touchedBy].pfrozentime > PFROZEN_THAW && episode==1)
         {
            // shot a frozen player--melt the ice
            player[objects[o].touchedBy].pfrozentime = PFROZEN_THAW;
            objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
         }
         else
         {
            if (objects[o].ai.ray.dontHitEnable==0 || objects[o].ai.ray.dontHit!=OBJ_PLAYER)
            {
              killplayer(objects[o].touchedBy);
              objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
            }
         }
       }

       if (objects[o].ai.ray.direction == RIGHT)
       {
           // don't go through bonklethal tiles, even if they're not solid
           // (for the arms on mortimer's machine)
       	   if (TileProperty[getmaptileat((objects[o].x>>CSF)+Sprite[objects[o].sprite]->getWidth(), (objects[o].y>>CSF)+1)][BEHAVIOR] == 1)
              hitlethal = 1;
           else if (TileProperty[getmaptileat((objects[o].x>>CSF)+Sprite[objects[o].sprite]->getWidth(), (objects[o].y>>CSF)+(Sprite[objects[o].sprite]->getHeight()-1))][BEHAVIOR] == 1)
              hitlethal = 1;
           else
        	  hitlethal = 0;

           if (objects[o].blockedr)
           {
              objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
              if (objects[o].onscreen)
            	  g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
           }

           objects[o].x += rayspeed;
       }
       else if (objects[o].ai.ray.direction == LEFT)
       {
           if (TileProperty[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+1)][BEHAVIOR] == 1)
              hitlethal = 1;
           else if (TileProperty[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+(Sprite[objects[o].sprite]->getHeight()-1))][BEHAVIOR] == 1)
              hitlethal = 1;
           else
        	  hitlethal = 0;

           if (objects[o].blockedl)
           {
              objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
              if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
           }

           objects[o].x -= rayspeed;
       }
       else if (objects[o].ai.ray.direction == DOWN)
       {
           if (objects[o].blockedd || objects[o].blockedu)
           {
              objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
              if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
           }

           objects[o].y += rayspeed;
       }
  break;
  case RAY_STATE_SETZAPZOT:
       objects[o].ai.ray.state = RAY_STATE_ZAPZOT;
       objects[o].ai.ray.zapzottimer = RAY_ZAPZOT_TIME;

       if (episode==1)
       {
         if (rnd()&1)
           { objects[o].sprite = RAY_FRAME_ZAP_EP1; }
         else
           { objects[o].sprite = RAY_FRAME_ZOT_EP1; }
       }
       else if (episode==2)
       {
         if (rnd()&1)
           { objects[o].sprite = RAY_FRAME_ZAP_EP2; }
         else
           { objects[o].sprite = RAY_FRAME_ZOT_EP2; }
       }
       else
       {
         if (rnd()&1)
           { objects[o].sprite = RAY_FRAME_ZAP_EP3; }
         else
           { objects[o].sprite = RAY_FRAME_ZOT_EP3; }
       }

       if (objects[o].ai.ray.direction==LEFT || objects[o].ai.ray.direction==RIGHT)
       {
         objects[o].y -= (2<<CSF);
       }
       else
       {
         objects[o].x -= (4<<CSF);
       }
       // ... and fall through
  case RAY_STATE_ZAPZOT:
       if (!objects[o].ai.ray.zapzottimer)
         { delete_object(o); }
       else objects[o].ai.ray.zapzottimer--;
  break;
  }*/
}

