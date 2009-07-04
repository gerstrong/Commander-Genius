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
#define TANK_LOOK_ANIM_TIME     110
#define TANK_LOOK_TOTALTIME     250
#define TANK2_PREPAREFIRE_TIME  80

// frames
#define TANK2_WALK_LEFT_FRAME       116
#define TANK2_WALK_RIGHT_FRAME      112
#define TANK2_LOOK_FRAME            120

#define TANKPUSHAMOUNT        16


#define TANK2_SHOTS_PER_VOLLEY    4
#define TANK2_MIN_TIME_TILL_CAN_FIRE  600
#define TANK2_MAX_TIME_TILL_CAN_FIRE  1000
#define TANK2_PAUSE_BEFORE_FIRST_SHOT 150
#define TANK2_TIME_BETWEEN_SHOTS  50
#define TANK2_TIME_BEFORE_FIRE_WHEN_SEE      100
#define TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL  400

void tankep2_ai(int o, stCloneKeenPlus *pCKP)
{
int newobject;
//int not_about_to_fall;
unsigned int i;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.tank.state = TANK_WALK;
     objects[o].ai.tank.movedir = RIGHT;
     objects[o].ai.tank.fireafterlook = 0;
     objects[o].ai.tank.animtimer = 0;
     objects[o].ai.tank.timer = 0;
     objects[o].ai.tank.dist_traveled = 0;
     objects[o].ai.tank.timetillcanfire = TANK2_MAX_TIME_TILL_CAN_FIRE;
     objects[o].ai.tank.firetimes = 0;
     objects[o].ai.tank.detectedPlayer = 0;
     objects[o].ai.tank.detectedPlayerIndex = primaryplayer;

     objects[o].canbezapped = 1;   // will stop bullets but is not harmed
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }
   // touched player?
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
   {
        killplayer(objects[o].touchedBy, pCKP);
   }

   switch(objects[o].ai.tank.state)
   {
    case TANK_LOOK:
      objects[o].sprite = TANK2_LOOK_FRAME + objects[o].ai.tank.frame;
      // animation
      if (objects[o].ai.tank.animtimer > TANK_LOOK_ANIM_TIME)
      {
        objects[o].ai.tank.frame ^= 1;
        objects[o].ai.tank.animtimer = 0;
      } else objects[o].ai.tank.animtimer++;

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
             objects[o].ai.tank.timetillcanfire = (rand()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
             objects[o].ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BEFORE_FIRE_WHEN_SEE;
             objects[o].ai.tank.firetimes = 0;
             objects[o].ai.tank.state = TANK_WALK;
             objects[o].ai.tank.frame = 0;
             objects[o].ai.tank.animtimer = 0;
             objects[o].ai.tank.timer = 0;
             objects[o].ai.tank.dist_traveled = 0;
      } else objects[o].ai.tank.timer++;
      break;
    case TANK_WALK:
       // is keen on same level?
       objects[o].ai.tank.detectedPlayer = 0;
       for(i=0;i<numplayers;i++)
       {
         if (player[i].y >= objects[o].y-(12<<CSF))
         {
           if ((player[i].y>>CSF)+sprites[0].ysize <= (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize+12)
           {
             objects[o].ai.tank.detectedPlayer = 1;
             objects[o].ai.tank.detectedPlayerIndex = i;
             break;
           }
         }
       }

         if (objects[o].ai.tank.detectedPlayer)
         {
              // facing keen?
              objects[o].ai.tank.alreadyfiredcauseonsamelevel = 1;
              // are we facing him?
              if (((player[objects[o].ai.tank.detectedPlayerIndex].x < objects[o].x) && objects[o].ai.tank.movedir==LEFT) || \
                  ((player[objects[o].ai.tank.detectedPlayerIndex].x > objects[o].x) && objects[o].ai.tank.movedir==RIGHT))
              { // yes, we're facing him! FIRE!!!
                 if (!objects[o].ai.tank.firetimes)
                 {
                   if (!objects[o].ai.tank.timetillcanfirecauseonsamelevel)
                   {
                     objects[o].ai.tank.firetimes = TANK2_SHOTS_PER_VOLLEY;
                     objects[o].ai.tank.timetillnextshot = TANK2_PAUSE_BEFORE_FIRST_SHOT;
                     objects[o].ai.tank.timetillcanfire = (rand()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
                     objects[o].ai.tank.timetillcanfirecauseonsamelevel = TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL;
                   }
                   else objects[o].ai.tank.timetillcanfirecauseonsamelevel--;
                 }
              }
              else
              {
                 // no we're not facing him, on hard difficulty turn around
                 if (pCKP->Control.levelcontrol.hardmode)
                 {
                    objects[o].ai.tank.frame = 0;
                    objects[o].ai.tank.timer = 0;
                    objects[o].ai.tank.animtimer = 0;
                    objects[o].ai.tank.state = TANK_LOOK;
                 }
              }
         }
         else
         {  // no, not on same level
           objects[o].ai.tank.alreadyfiredcauseonsamelevel = 0;
         }

       // are we firing a volley?
       if (objects[o].ai.tank.firetimes)
       {
         // is it time to fire the next shot in the volley?
         if (!objects[o].ai.tank.timetillnextshot)
         {
             if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].scrx);
             if (objects[o].ai.tank.movedir==RIGHT)
             {
                 newobject = spawn_object(objects[o].x+(sprites[TANK2_WALK_RIGHT_FRAME].xsize<<CSF), objects[o].y+(6<<CSF), OBJ_RAY);
                 objects[newobject].ai.ray.direction = RIGHT;
             }
             else
             {
                 newobject = spawn_object(objects[o].x-(sprites[ENEMYRAYEP2].xsize<<CSF), objects[o].y+(6<<CSF), OBJ_RAY);
                 objects[newobject].ai.ray.direction = LEFT;
             }
             objects[newobject].sprite = ENEMYRAYEP2;
             objects[newobject].ai.ray.dontHitEnable = 0;

             objects[o].ai.tank.timetillnextshot = TANK2_TIME_BETWEEN_SHOTS;
             objects[o].ai.tank.firetimes--;
         }
         else
         {
             objects[o].ai.tank.timetillnextshot--;
         }
       }
       else
       {  // not firing a volley
          if (!objects[o].ai.tank.timetillcanfire)
          {
            objects[o].ai.tank.firetimes = TANK2_SHOTS_PER_VOLLEY;
            objects[o].ai.tank.timetillnextshot = TANK2_PAUSE_BEFORE_FIRST_SHOT;
            objects[o].ai.tank.timetillcanfire = (rand()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
          }
          else
          {
            objects[o].ai.tank.timetillcanfire--;
          }
       }

	  if (!objects[o].ai.tank.firetimes) // can't walk while firing
	  {
		  if (objects[o].ai.tank.movedir==LEFT)
		  {  // move left
			  objects[o].sprite = TANK2_WALK_LEFT_FRAME + objects[o].ai.tank.frame;
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
	  }

      // hover animation
      if (objects[o].ai.tank.animtimer > TANK_WALK_ANIM_TIME)
      {
        if (objects[o].ai.tank.frame>=3) objects[o].ai.tank.frame=0;
                                    else objects[o].ai.tank.frame++;
        objects[o].ai.tank.animtimer = 0;
      } else objects[o].ai.tank.animtimer++;
    break;

   }
}
