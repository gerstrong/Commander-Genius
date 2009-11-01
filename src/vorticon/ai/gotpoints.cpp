/*
 * gotpoints.cpp
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */

#include "../../keen.h"

// GotPoints object (rising numbers when you get a bonus item)
// (this wasn't in original Keen)

#define GOTPOINTS_SPEED         4
#define GOTPOINTS_LIFETIME      100
#define YORPSHIELD_LIFETIME     20

// Reference to ../game.cpp
void delete_object(int o);

void gotpoints_ai(int o)
{
	
	/*if (objects[o].needinit)
	 {
	 objects[o].ai.ray.offscreentime = GOTPOINTS_LIFETIME;
	 objects[o].inhibitfall = 1;
	 objects[o].honorPriority = 0;
	 objects[o].needinit = 0;
	 }
	 
	 // delete it after it's existed for a certain amount of time
	 if (!objects[o].ai.ray.offscreentime)
	 {
	 delete_object(o);
	 return;
	 }
	 else objects[o].ai.ray.offscreentime--;
	 */
}

