//
// cosmo_engine created on 14/Nov/2017.
//
#ifndef COSMO_ENGINE_ACTOR_COLLISION_H
#define COSMO_ENGINE_ACTOR_COLLISION_H

#include "actor.h"

int actor_update_impl(ActorData *actor, int actorInfoIndex, int frame_num, int x_pos, int y_pos);

void actor_explode_container(ActorData *actor);

#endif //COSMO_ENGINE_ACTOR_COLLISION_H
