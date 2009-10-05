#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"


// Tank Robot (Ep2)
#define TANK_LOOK       0
#define TANK_WALK       1

#define TANK2_SAME_LEVEL_TIME   150
#define TANK_REPEAT_FIRE_TIME  800

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_LOOKFIRE_PROB    500
#define TANK_MINTRAVELDIST    200

#define TANK_WALK_SPEED         4
#define TANK_WALK_ANIM_TIME     60
#define TANK_LOOK_ANIM_TIME     70
#define TANK_LOOK_TOTALTIME     180
#define TANK2_PREPAREFIRE_TIME  80

// frames
#define TANK2_WALK_LEFT_FRAME       116
#define TANK2_WALK_RIGHT_FRAME      112
#define TANK2_LOOK_FRAME            120

#define TANKPUSHAMOUNT        16

#define TANK_FIRE_PAUSE_TIME		100

#define TANK2_SHOTS_PER_VOLLEY    4
#define TANK2_MIN_TIME_TILL_CAN_FIRE  500
#define TANK2_MAX_TIME_TILL_CAN_FIRE  800
#define TANK2_TIME_BETWEEN_SHOTS  50
#define TANK2_TIME_BEFORE_FIRE_WHEN_SEE      100
#define TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL  400

#define Sprite g_pGfxEngine->Sprite

// reference to ../misc.cpp
unsigned int rnd(void);

void static tank_searchplayers(int o);
void static tank2_fire(int o);

void tankep2_ai(int o, bool hardmode)
{
/*int newobject;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.tank.state = TANK_WALK;
     objects[o].ai.tank.movedir = RIGHT;
     objects[o].ai.tank.fireafterlook = 0;
     objects[o].ai.tank.animtimer = 0;
     objects[o].ai.tank.timer = 0;
     objects[o].ai.tank.dist_traveled = 0;
     objects[o].ai.tank.pausetime = 0;
     objects[o].ai.tank.timetillcanfire = TANK2_MAX_TIME_TILL_CAN_FIRE;
     objects[o].ai.tank.firetimes = 0;
     objects[o].ai.tank.detectedPlayer = 0;
     objects[o].ai.tank.detectedPlayerIndex = primaryplayer;
     objects[o].ai.tank.turnaroundtimer = 0;

     objects[o].canbezapped = 1;   // will stop bullets but is not harmed
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }
   // touched player?
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
        killplayer(objects[o].touchedBy);

   switch(objects[o].ai.tank.state)
   {
    case TANK_LOOK:
      // animation
      if (objects[o].ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
      {
    	  objects[o].ai.tank.frame ^= 1;
    	  objects[o].ai.tank.animtimer = 0;
      }
      else
    	  objects[o].ai.tank.animtimer++;

	  objects[o].sprite = TANK2_LOOK_FRAME + objects[o].ai.tank.frame;


      // when time is up go back to moving
      if (objects[o].ai.tank.timer > TANK_LOOK_TOTALTIME)
      {
         // decide what direction to go
             if (objects[o].blockedr)
               { objects[o].ai.tank.movedir = LEFT; }
             else if (objects[o].blockedl)
               { objects[o].ai.tank.movedir = RIGHT; }
             else if (objects[o].x > player[0].x)
               { objects[o].ai.tank.movedir = LEFT; }
             else
               { objects[o].ai.tank.movedir = RIGHT; }

             objects[o].ai.tank.alreadyfiredcauseonsamelevel = 0;
             objects[o].ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
             objects[o].ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BEFORE_FIRE_WHEN_SEE;
             objects[o].ai.tank.firetimes = 0;
             objects[o].ai.tank.state = TANK_WALK;
             objects[o].ai.tank.frame = 0;
             objects[o].ai.tank.animtimer = 0;
             objects[o].ai.tank.timer = 0;
             objects[o].ai.tank.dist_traveled = 0;
      }
      else
    	  objects[o].ai.tank.timer++;

      break;

    case TANK_WALK:
        // hover animation
        if (objects[o].ai.tank.animtimer > TANK_WALK_ANIM_TIME)
        {
          if (objects[o].ai.tank.frame>=3) objects[o].ai.tank.frame=0;
                                      else objects[o].ai.tank.frame++;
          objects[o].ai.tank.animtimer = 0;
        } else objects[o].ai.tank.animtimer++;

		if (objects[o].ai.tank.movedir==LEFT)
			objects[o].sprite = TANK2_WALK_LEFT_FRAME + objects[o].ai.tank.frame;
		else
			objects[o].sprite = TANK2_WALK_RIGHT_FRAME + objects[o].ai.tank.frame;

	 // if we're about to, or just did, fire a volley, don't move
		if (!hardmode)
		{
				if (objects[o].ai.tank.pausetime)
				{
					objects[o].ai.tank.pausetime--;
					return;
				}
		}
		else
			objects[o].ai.tank.pausetime = 0;

       // are we firing a volley?
       if (objects[o].ai.tank.firetimes)
       {
         // is it time to fire the next shot in the volley?
         if (!objects[o].ai.tank.timetillnextshot)
         {
             if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].scrx);
             if (objects[o].ai.tank.movedir==RIGHT)
             {
                 newobject = spawn_object(objects[o].x+(Sprite[TANK2_WALK_RIGHT_FRAME]->getWidth()<<CSF), objects[o].y+(6<<CSF), OBJ_RAY);
                 objects[newobject].ai.ray.direction = RIGHT;
             }
             else
             {
                 newobject = spawn_object(objects[o].x-(Sprite[ENEMYRAYEP2]->getWidth()<<CSF), objects[o].y+(6<<CSF), OBJ_RAY);
                 objects[newobject].ai.ray.direction = LEFT;
             }
             objects[newobject].sprite = ENEMYRAYEP2;
             objects[newobject].ai.ray.dontHitEnable = 0;

             objects[o].ai.tank.timetillnextshot = TANK2_TIME_BETWEEN_SHOTS;
             if (!--objects[o].ai.tank.firetimes)
             {
            	 objects[o].ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
             }
         }
         else
         {
             objects[o].ai.tank.timetillnextshot--;
         }

         // don't move when firing except on hard mode
         if (hardmode)
        	 return;

       }
       else
       {  // not firing a volley
          if (!objects[o].ai.tank.timetillcanfire)
          {
        	  tank2_fire(o);
          }
          else
          {
        	  objects[o].ai.tank.timetillcanfire--;
          }

       }

		// is keen on same level?
		tank_searchplayers(o);

		if (objects[o].ai.tank.detectedPlayer)
		{
			// facing keen?
			objects[o].ai.tank.alreadyfiredcauseonsamelevel = 1;
			// are we facing him?
             if (((player[objects[o].ai.tank.detectedPlayerIndex].x < objects[o].x) && objects[o].ai.tank.movedir==LEFT) || \
                 ((player[objects[o].ai.tank.detectedPlayerIndex].x > objects[o].x) && objects[o].ai.tank.movedir==RIGHT))
				{
					// yes, we're facing him! FIRE!!!
					if (!objects[o].ai.tank.firetimes)
					{
						if (!objects[o].ai.tank.timetillcanfirecauseonsamelevel)
						{
							tank2_fire(o);
							objects[o].ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL;
						}
						else objects[o].ai.tank.timetillcanfirecauseonsamelevel--;
					}
				}
				else
				{
					// no, we're not facing him, on hard difficulty turn around
					if (hardmode)
					{
						if (!objects[o].ai.tank.turnaroundtimer)
						{
		                    objects[o].ai.tank.frame = 0;
		                    objects[o].ai.tank.timer = 0;
		                    objects[o].ai.tank.animtimer = 0;
		                    objects[o].ai.tank.state = TANK_LOOK;
							objects[o].ai.tank.turnaroundtimer = 100;
						}
						else objects[o].ai.tank.turnaroundtimer--;
					}
				}
		}
		else
		{  // no, not on same level
			objects[o].ai.tank.alreadyfiredcauseonsamelevel = 0;
			objects[o].ai.tank.turnaroundtimer = 0;
		}


		if (objects[o].ai.tank.movedir==LEFT)
		{  // move left
			if (!objects[o].blockedl)
			{
				objects[o].x -= TANK_WALK_SPEED;
				objects[o].ai.tank.dist_traveled++;
			}
			else
			{
				objects[o].ai.tank.frame = 0;
				objects[o].ai.tank.timer = 0;
				objects[o].ai.tank.animtimer = 0;
				objects[o].ai.tank.state = TANK_LOOK;
			}
		}
		else
		{  // move right
			objects[o].sprite = TANK2_WALK_RIGHT_FRAME + objects[o].ai.tank.frame;
			if (!objects[o].blockedr)
			{
				objects[o].x += TANK_WALK_SPEED;
				objects[o].ai.tank.dist_traveled++;
			}
			else
			{
				objects[o].ai.tank.frame = 0;
				objects[o].ai.tank.timer = 0;
				objects[o].ai.tank.animtimer = 0;
				objects[o].ai.tank.state = TANK_LOOK;
			}
		}
		break;
	  }*/
}

// makes the tank start firing
void static tank2_fire(int o)
{
	objects[o].ai.tank.firetimes = TANK2_SHOTS_PER_VOLLEY;
	objects[o].ai.tank.timetillnextshot = 0;
	objects[o].ai.tank.timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
	objects[o].ai.tank.pausetime = TANK_FIRE_PAUSE_TIME;
}


// searches for any players on the same level as the tank
void static tank_searchplayers(int o)
{
/*	objects[o].ai.tank.detectedPlayer = 0;
	for( unsigned int i=0 ; i<numplayers ; i++ )
	{
		if (player[i].y >= objects[o].y-(12<<CSF))
		{
			if ((player[i].y>>CSF)+Sprite[0]->getHeight() <= (objects[o].y>>CSF)+Sprite[objects[o].sprite]->getHeight()+12)
			{
				objects[o].ai.tank.detectedPlayer = 1;
				objects[o].ai.tank.detectedPlayerIndex = i;
				break;
			}
		}
	}*/
}
