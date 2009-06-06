#include "../keen.h"

#include "../include/enemyai.h"

// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)

#define ICEBIT_SPEED        20

void icebit_ai(int o)
{
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].inhibitfall = 1;
     objects[o].canbezapped = 0;
     objects[o].needinit = 0;
   }

     switch(objects[o].ai.icechunk.movedir)
     {
     case DUPRIGHT:
       objects[o].x += ICEBIT_SPEED;
       objects[o].y -= ICEBIT_SPEED;
       break;
     case DUPLEFT:
       objects[o].x -= ICEBIT_SPEED;
       objects[o].y -= ICEBIT_SPEED;
       break;
     case DDOWNRIGHT:
       objects[o].x += ICEBIT_SPEED;
       objects[o].y += ICEBIT_SPEED;
       break;
     case DDOWNLEFT:
       objects[o].x -= ICEBIT_SPEED;
       objects[o].y += ICEBIT_SPEED;
       break;
     break;
     }
}
