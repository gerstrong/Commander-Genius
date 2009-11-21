#include "../../keen.h"
#include "vortelite.h"
#include "../../game.h"
#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"


// the "Vorticon Elite" enemies in ep2
#define VORTELITE_WALK          0
#define VORTELITE_JUMP          1
#define VORTELITE_ABOUTTOFIRE   2
#define VORTELITE_FIRED         3
#define VORTELITE_DYING         4
#define VORTELITE_DEAD          5

#define VORTELITE_JUMP_PROB          400
#define VORTELITE_FIRE_PROB          320

#define VORTELITE_MIN_TIME_BETWEEN_FIRE    100
#define VORTELITE_HOLD_GUN_OUT_TIME         90
#define VORTELITE_HOLD_GUN_AFTER_FIRE_TIME  75

#define VORTELITE_MIN_JUMP_HEIGHT    15
#define VORTELITE_MAX_JUMP_HEIGHT    25
#define VORTELITE_MAX_FALL_SPEED     20
#define VORTELITE_JUMP_FRICTION      6

#define VORTELITE_WALK_SPEED         7
#define VORTELITE_WALK_ANIM_TIME     50

// number of shots to kill
#define VORTELITE_HP                 4

#define VORTELITE_DIE_ANIM_TIME     180

#define VORTELITE_LOOK_ANIM_TIME     60

#define VORTELITE_WALK_LEFT_FRAME    88
#define VORTELITE_WALK_RIGHT_FRAME   92
#define VORTELITE_JUMP_RIGHT_FRAME   98
#define VORTELITE_JUMP_LEFT_FRAME    99
#define VORTELITE_DYING_FRAME        100
#define VORTELITE_DEAD_FRAME         101
#define VORTELITE_FIRE_LEFT_FRAME    96
#define VORTELITE_FIRE_RIGHT_FRAME   97

#define VORTELITE_PALETTE_FLASH_TIME  5

#define VORTELITE_TRAPPED_DIST        150

#define sprites g_pGfxEngine->Sprite
/*

void objectvortelite_initiatejump(CObject &object);

void CObjectAI::vortelite_ai(CObject &object, bool darkness)
{
	int bonk;
	int newobject;

	if (object.needinit)
	{  // first time initilization
		object.ai.vortelite.state = VORTELITE_WALK;
		object.ai.vortelite.movedir = LEFT;
		object.sprite = VORTELITE_WALK_LEFT_FRAME;
		object.ai.vortelite.frame = 0;
		object.ai.vortelite.animtimer = 0;
		object.ai.vortelite.timer = 0;
		object.ai.vortelite.timesincefire = 0;
		object.ai.vortelite.dist_traveled = VORTELITE_TRAPPED_DIST+1;
		object.canbezapped = 1;
		object.needinit = 0;
	}
	if (object.ai.vortelite.state==VORTELITE_DEAD)
	{
		object.hasbeenonscreen = 0;
		return;
	}

	if (object.canbezapped)
	{
		// if we touch a glowcell, we die!
		if (getmaptileat((object.x>>CSF)+12, (object.y>>CSF)+16)==TILE_GLOWCELL)
		{
			object.zapped += VORTELITE_HP;
		}

		if (object.zapped >= VORTELITE_HP)
		{
			object.inhibitfall = 0;
			object.canbezapped = 0;
			object.ai.vortelite.animtimer = 0;
			object.ai.vortelite.frame = 0;
			object.ai.vortelite.state = VORTELITE_DYING;
			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
		}
	}
	// deadly to the touch
	if (object.touchPlayer && object.canbezapped)
	{
		killplayer(object.touchedBy);
	}

	reprocess: ;
	switch(object.ai.vortelite.state)
	{
	case VORTELITE_WALK:
		object.ai.vortelite.dist_traveled++;

		if (rand()%VORTELITE_JUMP_PROB == (VORTELITE_JUMP_PROB/2) && !darkness && !object.blockedu)
		{  // let's jump.
			vortelite_initiatejump(o);
			goto reprocess;
		}
		else
		{
			if (object.ai.vortelite.timesincefire > VORTELITE_MIN_TIME_BETWEEN_FIRE)
			{
				if (rand()%VORTELITE_FIRE_PROB == (VORTELITE_FIRE_PROB/2))
				{  // let's fire
					// usually shoot toward keen
					if (rand()%5 != 0)
					{
						if (object.x < player[primaryplayer].x)
						{
							object.ai.vortelite.movedir = RIGHT;
						}
						else
						{
							object.ai.vortelite.movedir = LEFT;
						}
					}
					object.ai.vortelite.timer = 0;
					object.ai.vortelite.state = VORTELITE_ABOUTTOFIRE;
				}
			}
			else object.ai.vortelite.timesincefire++;
		}

		if (object.ai.vortelite.movedir==LEFT)
		{  // move left
			object.sprite = VORTELITE_WALK_LEFT_FRAME + object.ai.vortelite.frame;
			if (!object.blockedl)
			{
				object.x -= VORTELITE_WALK_SPEED;
			}
			else
			{
				object.ai.vortelite.movedir = RIGHT;

				// if we only traveled a tiny amount before hitting a wall, we've
				// probably fallen into a small narrow area, and we need to try
				// to jump out of it
				if (object.ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && object.blockedd)
				{
					vortelite_initiatejump(o);
					goto reprocess;
				}
				else object.ai.vortelite.dist_traveled = 0;
			}
		}
		else
		{  // move right
			object.sprite = VORTELITE_WALK_RIGHT_FRAME + object.ai.vortelite.frame;
			if (!object.blockedr)
			{
				object.x += VORTELITE_WALK_SPEED;
			}
			else
			{
				object.ai.vortelite.movedir = LEFT;

				// if we only traveled a tiny amount before hitting a wall, we've
				// probably fallen into a small narrow area, and we need to try
				// to jump out of it
				if (object.ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && object.blockedd)
				{
					vortelite_initiatejump(o);
					goto reprocess;
				}
				else object.ai.vortelite.dist_traveled = 0;
			}
		}

		// walk animation
		if (object.ai.vortelite.animtimer > VORTELITE_WALK_ANIM_TIME)
		{
			if (object.ai.vortelite.frame>=3) object.ai.vortelite.frame=0;
			else object.ai.vortelite.frame++;
			object.ai.vortelite.animtimer = 0;
		} else object.ai.vortelite.animtimer++;
		break;
	case VORTELITE_JUMP:
		if (object.ai.vortelite.movedir == RIGHT)
		{ if (!object.blockedr) object.x += VORTELITE_WALK_SPEED; }
		else
		{ if (!object.blockedl) object.x -= VORTELITE_WALK_SPEED; }

		if (object.ai.vortelite.inertiay>0 && object.blockedd)
		{  // the bear has landed
			object.inhibitfall = 0;
			object.ai.vortelite.state = VORTELITE_WALK;
			goto reprocess;
		}
		// check if the bear has bonked into a ceiling, if so,
		// immediately terminate the jump
		bonk = 0;
		if (TileProperty[getmaptileat((object.x>>CSF)+1, (object.y>>CSF))][BDOWN]) bonk = 1;
		else if (TileProperty[getmaptileat((object.x>>CSF)+16, (object.y>>CSF))][BDOWN]) bonk = 1;
		else if (TileProperty[getmaptileat((object.x>>CSF)+23, (object.y>>CSF))][BDOWN]) bonk = 1;
		if (bonk && object.ai.vortelite.inertiay < 0)
		{
			object.ai.vortelite.inertiay = 0;
		}

		// apply Y inertia
		object.y += object.ai.vortelite.inertiay;

		if (object.ai.vortelite.timer > VORTELITE_JUMP_FRICTION)
		{ // slowly decrease upgoing rate
			if (object.ai.vortelite.inertiay<VORTELITE_MAX_FALL_SPEED)
			{
				object.ai.vortelite.inertiay++;
			}
			object.ai.vortelite.timer = 0;
		} else object.ai.vortelite.timer++;
		break;
	case VORTELITE_ABOUTTOFIRE:
		if (object.ai.vortelite.movedir==RIGHT)
		{ object.sprite = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		{ object.sprite = VORTELITE_FIRE_LEFT_FRAME; }
		if (object.ai.vortelite.timer > VORTELITE_HOLD_GUN_OUT_TIME)
		{
			object.ai.vortelite.timer = 0;
			object.ai.vortelite.state = VORTELITE_FIRED;

			if (object.ai.vortelite.movedir==RIGHT)
			{
				newobject = spawn_object(object.x+(sprites[ENEMYRAYEP2]->getWidth()<<CSF), object.y+(7<<CSF), OBJ_RAY);
				objects[newobject].ai.ray.direction = RIGHT;
			}
			else
			{
				newobject = spawn_object(object.x-(sprites[ENEMYRAYEP2]->getWidth()<<CSF), object.y+(9<<CSF), OBJ_RAY);
				objects[newobject].ai.ray.direction = LEFT;
			}
			objects[newobject].sprite = ENEMYRAYEP2;
			// don't shoot other vorticon elite
			objects[newobject].ai.ray.dontHitEnable = 1;
			objects[newobject].ai.ray.dontHit = OBJ_VORTELITE;

			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, object.scrx);
		}
		else object.ai.vortelite.timer++;
		break;
	case VORTELITE_FIRED:
		if (object.ai.vortelite.movedir==RIGHT)
		{ object.sprite = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		{ object.sprite = VORTELITE_FIRE_LEFT_FRAME; }

		if (object.ai.vortelite.timer > VORTELITE_HOLD_GUN_AFTER_FIRE_TIME)
		{
			object.ai.vortelite.timer = 0;
			object.ai.vortelite.frame = 0;
			object.ai.vortelite.timesincefire = 0;
			object.ai.vortelite.state = VORTELITE_WALK;
			// head toward keen
			if (object.x < player[primaryplayer].x)
			{
				object.ai.vortelite.movedir = RIGHT;
			}
			else
			{
				object.ai.vortelite.movedir = LEFT;
			}
		}
		else object.ai.vortelite.timer++;
		break;
	case VORTELITE_DYING:
		object.sprite = VORTELITE_DYING_FRAME;
		if (object.ai.vortelite.animtimer > VORTELITE_DIE_ANIM_TIME)
		{
			object.sprite = VORTELITE_DEAD_FRAME;
			object.ai.vortelite.state = VORTELITE_DEAD;
		}
		else
		{
			object.ai.vortelite.animtimer++;
		}
		break;
	default: break;
	}
}

void CObjectAI::vortelite_initiatejump(CObject &object)
{
	if (object.ai.vortelite.state==VORTELITE_JUMP) return;

	object.ai.vortelite.frame = 0;
	object.ai.vortelite.animtimer = 0;
	object.ai.vortelite.inertiay = \
			-((rand()%(VORTELITE_MAX_JUMP_HEIGHT-VORTELITE_MIN_JUMP_HEIGHT))+VORTELITE_MIN_JUMP_HEIGHT);

	if (object.ai.vortelite.movedir==RIGHT)
		object.sprite = VORTELITE_JUMP_RIGHT_FRAME;
	else
		object.sprite = VORTELITE_JUMP_LEFT_FRAME;

	object.inhibitfall = 1;
	object.ai.vortelite.state = VORTELITE_JUMP;
}
*/
