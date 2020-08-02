//
// Created by Eric Fry on 17/11/2017.
//

#include "actor_toss.h"
#include "actor.h"

#define MAX_TOSSED_ACTORS 6

typedef struct ActorToss
{
int actorInfoIndex;
int x;
int y;
int counter;
} ActorToss;

ActorToss actorToss[MAX_TOSSED_ACTORS];

void actor_toss_add_new(int actorInfoIndex, int x_pos, int y_pos) {
    for(int i=0;i<MAX_TOSSED_ACTORS;i++)
    {
        if(actorToss[i].actorInfoIndex == 0)
        {
            actorToss[i].actorInfoIndex = actorInfoIndex;
            actorToss[i].x = x_pos;
            actorToss[i].y = y_pos;
            actorToss[i].counter = 0;
            return;
        }
    }
}

void actor_toss_update() {
    for(int i=0;i<MAX_TOSSED_ACTORS;i++)
    {
        ActorToss *at = &actorToss[i];
        if (at->actorInfoIndex == 0)
            continue;
        
        at->counter = at->counter + 1;
        at->y--;
        if (sprite_blocking_check(0, at->actorInfoIndex, 0, at->x, at->y) == NOT_BLOCKED)
        {
            if (at->counter >= 9 || sprite_blocking_check(0, at->actorInfoIndex, 0, at->x, --at->y) == NOT_BLOCKED)
            {
                if(at->counter != 11)
                {
                    display_actor_sprite_maybe(at->actorInfoIndex, 0, at->x, at->y, 4);
                }
                else
                {
                    actor_add_new(at->actorInfoIndex, at->x, at->y);
                    display_actor_sprite_maybe(at->actorInfoIndex, 0, at->x, at->y, 4);
                    at->actorInfoIndex = 0;
                }
            }
            else
            {
                actor_add_new(at->actorInfoIndex, at->x, at->y + 1);
                display_actor_sprite_maybe(at->actorInfoIndex, 0, at->x, at->y + 1, 0);
                at->actorInfoIndex = 0;
            }
        }
        else
        {
            actor_add_new(at->actorInfoIndex, at->x, at->y + 1);
            display_actor_sprite_maybe(at->actorInfoIndex, 0, at->x, at->y + 1, 0);
            at->actorInfoIndex = 0;
        }
    }
}

void actor_toss_clear_all() {
    for(int i=0;i<MAX_TOSSED_ACTORS;i++)
    {
        actorToss[i].actorInfoIndex = 0;
    }
}
