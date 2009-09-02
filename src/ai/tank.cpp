#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// Tank Robot (ep1)

#define TANK_LOOK       0
#define TANK_WALK       1
#define TANK_FIRE       2

#define TANK_SAME_LEVEL_TIME   400
#define TANK_REPEAT_FIRE_TIME  800

#define TANK_SAME_LEVEL_TIME_FAST    50
#define TANK_REPEAT_FIRE_TIME_FAST   300

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_LOOKFIRE_PROB    500
#define TANK_MINTRAVELDIST    200

#define TANK_WALK_SPEED         4
#define TANK_WALK_ANIM_TIME     60
#define TANK_LOOK_ANIM_TIME     70
#define TANK_LOOK_TOTALTIME     180
#define TANK_PREPAREFIRE_TIME   180
#define TANK_PREPAREFIRE_TIME_FAST   80

// frames
#define TANK_WALK_LEFT_FRAME        102
#define TANK_WALK_RIGHT_FRAME       98
#define TANK_LOOK_FRAME             106

#define TANKPUSHAMOUNT        16

char tank_CanMoveLeft(int o, int h);
char tank_CanMoveRight(int o, int w, int h);

//reference to ../misc.cpp
unsigned int rnd(void);

void tank_ai(int o, CSprite *sprites,bool hardmode)
{
int newobject;
unsigned int i;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.tank.state = TANK_WALK;
     objects[o].ai.tank.movedir = RIGHT;
     objects[o].ai.tank.fireafterlook = 0;
     objects[o].ai.tank.animtimer = 0;
     objects[o].ai.tank.frame = 0;
     objects[o].ai.tank.timer = 0;
     objects[o].ai.tank.ponsameleveltime = 0;
     objects[o].ai.tank.alreadyfiredcauseonsamelevel = 0;
     objects[o].ai.tank.dist_traveled = 0;
     objects[o].ai.tank.detectedPlayer = 0;
     objects[o].ai.tank.detectedPlayerIndex = primaryplayer;
     objects[o].canbezapped = 1;  // will stop bullets but are not harmed
     objects[o].needinit = 0;
   }
   // stop keen from walking through our sprite
     if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
     {
        g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, objects[o].scrx);

        if (!((player[objects[o].touchedBy].y) < (objects[o].y - 300))) // give the player a little jump-over room
        {
          if (player[objects[o].touchedBy].x < objects[o].x)
          {
             player[objects[o].touchedBy].playpushed_x = -TANKPUSHAMOUNT;
             player[objects[o].touchedBy].playpushed_decreasetimer = 0;
             player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
          }
          else
          {
             player[objects[o].touchedBy].playpushed_x = TANKPUSHAMOUNT;
             player[objects[o].touchedBy].playpushed_decreasetimer = 0;
             player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
          }
        }
     }

   switch(objects[o].ai.tank.state)
   {
    case TANK_LOOK:
      objects[o].sprite = TANK_LOOK_FRAME + objects[o].ai.tank.frame;
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
         // did we go into this state for the purpose of turning and firing?
         if (!objects[o].ai.tank.fireafterlook)
         { // no
             if (!tank_CanMoveRight(o, sprites[TANK_WALK_RIGHT_FRAME].getWidth(), sprites[TANK_WALK_RIGHT_FRAME].getHeight()))
               { objects[o].ai.tank.movedir = LEFT; }
             else if (!tank_CanMoveLeft(o, sprites[TANK_WALK_RIGHT_FRAME].getHeight()))
               { objects[o].ai.tank.movedir = RIGHT; }
             else if (rnd()&1) // turn towards player
               { objects[o].ai.tank.movedir = LEFT; }
             else
               { objects[o].ai.tank.movedir = RIGHT; }

             objects[o].ai.tank.state = TANK_WALK;
             objects[o].ai.tank.frame = 0;
             objects[o].ai.tank.animtimer = 0;
             objects[o].ai.tank.timer = 0;
             objects[o].ai.tank.dist_traveled = 0;
         }
         else
         { // yes, face towards player and fire.
             if (player[objects[o].ai.tank.detectedPlayerIndex].x < objects[o].x)
             {
               objects[o].ai.tank.movedir = LEFT;
               objects[o].sprite = TANK_WALK_LEFT_FRAME;
             }
             else
             {
               objects[o].ai.tank.movedir = RIGHT;
               objects[o].sprite = TANK_WALK_RIGHT_FRAME;
             }
             objects[o].ai.tank.timer = 0;
             objects[o].ai.tank.fireafterlook = 0;
             objects[o].ai.tank.state = TANK_FIRE;
         }
      } else objects[o].ai.tank.timer++;
      break;
    case TANK_WALK:
       // is keen on same level?
       objects[o].ai.tank.detectedPlayer = 0;
       for(i=0;i<numplayers;i++)
       {
         if (player[i].y >= objects[o].y-(48<<CSF))
         {
           if ((player[i].y>>CSF)+sprites[0].getHeight() <= (objects[o].y>>CSF)+sprites[objects[o].sprite].getHeight()+48)
           {
             objects[o].ai.tank.detectedPlayer = 1;
             objects[o].ai.tank.detectedPlayerIndex = i;
             break;
           }
         }
       }

         if (objects[o].ai.tank.detectedPlayer)
         {
           objects[o].ai.tank.ponsameleveltime++;
           if ((objects[o].ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME) ||
                 (objects[o].ai.tank.ponsameleveltime > TANK_SAME_LEVEL_TIME_FAST && hardmode))
           {   // keen would be a good target now (hard mode).

              if (!objects[o].ai.tank.alreadyfiredcauseonsamelevel ||\
                  objects[o].ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME || \
                  (objects[o].ai.tank.ponsameleveltime > TANK_REPEAT_FIRE_TIME_FAST && hardmode))
              {
                // facing keen?
                objects[o].ai.tank.alreadyfiredcauseonsamelevel = 1;
                if (((player[objects[o].ai.tank.detectedPlayerIndex].x < objects[o].x) && objects[o].ai.tank.movedir==LEFT) || \
                    ((player[objects[o].ai.tank.detectedPlayerIndex].x > objects[o].x) && objects[o].ai.tank.movedir==RIGHT))
                { // yes, we're facing him, we see him, we fire!
                   objects[o].ai.tank.timer = 0;
                   objects[o].ai.tank.state = TANK_FIRE;
                   objects[o].ai.tank.ponsameleveltime = 0;
                }
                else
                { // no, we're not facing him. turn and fire!
                   objects[o].ai.tank.timer = 0;
                   objects[o].ai.tank.frame = 0;
                   objects[o].ai.tank.fireafterlook = 1;
                   objects[o].ai.tank.state = TANK_LOOK;
                }
              }
           }
         }
         else
         {  // no, not on same level
           objects[o].ai.tank.ponsameleveltime = 0;
           objects[o].ai.tank.alreadyfiredcauseonsamelevel = 0;
         }

       if (objects[o].ai.tank.dist_traveled > TANK_MINTRAVELDIST && objects[o].onscreen)
       {
          if (rnd()%TANK_LOOKFIRE_PROB==(TANK_LOOKFIRE_PROB/2))
          {  // we're either going to look or fire
              if (rnd()&1)
              { // look
                 objects[o].ai.tank.timer = 0;
                 objects[o].ai.tank.frame = 0;
                 objects[o].ai.tank.state = TANK_LOOK;
              }
              else
              { // FIRE!
            	  objects[o].ai.tank.timer = 0;
            	  objects[o].ai.tank.state = TANK_FIRE;
              }
              break;
          }
       }

       if (objects[o].ai.tank.movedir==LEFT)
       {  // move left
         objects[o].sprite = TANK_WALK_LEFT_FRAME + objects[o].ai.tank.frame;
         if (tank_CanMoveLeft(o, sprites[TANK_WALK_RIGHT_FRAME].getHeight()))
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
         objects[o].sprite = TANK_WALK_RIGHT_FRAME + objects[o].ai.tank.frame;
         if (tank_CanMoveRight(o, sprites[TANK_WALK_RIGHT_FRAME].getWidth(),
								 sprites[TANK_WALK_RIGHT_FRAME].getHeight()))
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
       // walk animation
       if (objects[o].ai.tank.animtimer > TANK_WALK_ANIM_TIME)
       {
         if (objects[o].ai.tank.frame>=3) objects[o].ai.tank.frame=0;
                                     else objects[o].ai.tank.frame++;
         objects[o].ai.tank.animtimer = 0;
       } else objects[o].ai.tank.animtimer++;
    break;
     case TANK_FIRE:
      if (objects[o].ai.tank.timer > TANK_PREPAREFIRE_TIME || \
          (objects[o].ai.tank.timer > TANK_PREPAREFIRE_TIME_FAST && hardmode))
      {
         if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].scrx);
         if (objects[o].ai.tank.movedir==RIGHT)
         {
             newobject = spawn_object(objects[o].x+(sprites[TANK_WALK_RIGHT_FRAME].getWidth()<<CSF), objects[o].y+(5<<CSF), OBJ_RAY);
             objects[newobject].ai.ray.direction = RIGHT;
         }
         else
         {
             newobject = spawn_object(objects[o].x-(sprites[OBJ_RAY_DEFSPRITE_EP1].getWidth()<<CSF), objects[o].y+(5<<CSF), OBJ_RAY);
             objects[newobject].ai.ray.direction = LEFT;
         }
         objects[newobject].sprite = ENEMYRAY;
         objects[newobject].ai.ray.dontHitEnable = 0;

         objects[o].ai.tank.state = TANK_WALK;

      } else objects[o].ai.tank.timer++;
     break;

   }
}

char tank_CanMoveLeft(int o, int h)
{
	int not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+h)][BUP];
	if (!objects[o].blockedl && not_about_to_fall) return 1;
	return 0;
}

char tank_CanMoveRight(int o, int w, int h)
{
	int not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)+w+1, (objects[o].y>>CSF)+h)][BUP];
	if (!objects[o].blockedr && not_about_to_fall) return 1;
	return 0;
}
