#include "../keen.h"

#include "../include/enemyai.h"

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    10

void door_ai(int o)
{
   if (objects[o].needinit)
   {
     objects[o].ai.door.timer = 0;
     objects[o].ai.door.distance_traveled = 0;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
   }

   if (objects[o].ai.door.timer > DOOR_OPEN_SPEED)
   {
      objects[o].y += (1<<CSF);
      sprites[objects[o].sprite].ysize--;
      if (objects[o].ai.door.distance_traveled>31)
      {
        objects[o].exists = 0;
      }
      else objects[o].ai.door.distance_traveled++;
      objects[o].ai.door.timer = 0;
   } else objects[o].ai.door.timer++;
}
