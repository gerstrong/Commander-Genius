#include "../sdl/sound/CSound.h"

#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// the chunks of ice shot out by an ice cannon (ep1)

#define ICECHUNK_FLY       0

#define ICECHUNK_SPEED        15
#define ICECHUNK_WAIT_TIME    150

void icechunk_ai(int o)
{
int newobject,blockedlr;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.icechunk.state = ICECHUNK_FLY;
     objects[o].ai.icechunk.timer = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
     objects[o].canbezapped = 0;
   }

   if (objects[o].touchPlayer && objects[o].ai.icechunk.state==ICECHUNK_FLY &&\
       !player[objects[o].touchedBy].pfrozentime)
   {
     switch(objects[o].ai.icechunk.movedir)
     {
       case DUPRIGHT:
       case DDOWNRIGHT:
         player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
         player[objects[o].touchedBy].pinertia_x = PMAXSPEED;
         break;
       case DUPLEFT:
       case DDOWNLEFT:
         player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
         player[objects[o].touchedBy].pinertia_x = -PMAXSPEED;
         break;
     }
     freezeplayer(objects[o].touchedBy);
     goto smash;
   }

   switch(objects[o].ai.icechunk.state)
   {
   case ICECHUNK_FLY:
     blockedlr = 0;
     if (objects[o].ai.icechunk.movedir==DUPRIGHT || objects[o].ai.icechunk.movedir==DDOWNRIGHT)
     {
       if (objects[o].blockedr) blockedlr = 1;
     }
     if (objects[o].ai.icechunk.movedir==DUPLEFT || objects[o].ai.icechunk.movedir==DDOWNLEFT)
     {
       if (objects[o].blockedl) blockedlr = 1;
     }

     if (blockedlr)
     {
     smash: ;
        if (objects[o].onscreen)
        {
        	g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, objects[o].scrx);
        	newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
        	objects[newobject].ai.icechunk.movedir = DUPLEFT;
        	newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
        	objects[newobject].ai.icechunk.movedir = DUPRIGHT;
        	newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
        	objects[newobject].ai.icechunk.movedir = DDOWNLEFT;
        	newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
        	objects[newobject].ai.icechunk.movedir = DDOWNRIGHT;
        }

        objects[o].exists = 0;
        return;
     }

     switch(objects[o].ai.icechunk.movedir)
     {
     case DUPRIGHT:
       objects[o].x += ICECHUNK_SPEED;
       objects[o].y -= ICECHUNK_SPEED;
     break;
     case DUPLEFT:
       objects[o].x -= ICECHUNK_SPEED;
       objects[o].y -= ICECHUNK_SPEED;
     break;
     case DDOWNLEFT:
       objects[o].x -= ICECHUNK_SPEED;
       objects[o].y += ICECHUNK_SPEED;
     break;
     case DDOWNRIGHT:
       objects[o].x += ICECHUNK_SPEED;
       objects[o].y += ICECHUNK_SPEED;
     break;
     break;
     }
   break;
   }
}
