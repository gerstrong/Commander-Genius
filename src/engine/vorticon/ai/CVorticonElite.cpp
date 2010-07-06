#include "../../../misc.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../graphics/CGfxEngine.h"
#include "../../spritedefines.h"

#include "CVorticonElite.h"

// the "Vorticon Elite" enemies in ep2
enum vortelite_actions{
	VORTELITE_WALK, VORTELITE_JUMP,
	VORTELITE_ABOUTTOFIRE, VORTELITE_FIRED,
	VORTELITE_DYING, VORTELITE_DEAD,
};

#define VORTELITE_JUMP_PROB          15
#define VORTELITE_FIRE_PROB          8

#define VORTELITE_MIN_TIME_BETWEEN_FIRE    	25
#define VORTELITE_HOLD_GUN_OUT_TIME         22
#define VORTELITE_HOLD_GUN_AFTER_FIRE_TIME  20

#define VORTELITE_MIN_JUMP_HEIGHT    90
#define VORTELITE_MAX_JUMP_HEIGHT    150
#define VORTELITE_MAX_FALL_SPEED     240
#define VORTELITE_JUMP_FRICTION      2

#define VORTELITE_WALK_SPEED         45
#define VORTELITE_WALK_ANIM_TIME     12

// number of shots to kill
#define VORTELITE_HP                 4

#define VORTELITE_DIE_ANIM_TIME     45

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

CVorticonElite::CVorticonElite(CMap *p_map) :
CObject(p_map)
{}

//void CObjectAI::vortelite_ai(CObject &object, bool darkness)
//{
//	int bonk;
//	int x, y;
//
//	if (object.needinit)
//	{  // first time initialization
//		object.ai.vortelite.state = VORTELITE_WALK;
//		object.ai.vortelite.movedir = LEFT;
//		object.sprite = VORTELITE_WALK_LEFT_FRAME;
//		object.ai.vortelite.frame = 0;
//		object.ai.vortelite.animtimer = 0;
//		object.ai.vortelite.timer = 0;
//		object.ai.vortelite.timesincefire = 0;
//		object.ai.vortelite.dist_traveled = VORTELITE_TRAPPED_DIST+1;
//		object.canbezapped = 1;
//		object.needinit = 0;
//	}
//	if (object.ai.vortelite.state==VORTELITE_DEAD)
//	{
//		object.hasbeenonscreen = 0;
//		return;
//	}
//
//	if (object.canbezapped)
//	{
//		x = object.getXPosition();
//		y = object.getYPosition();
//		// if we touch a glowcell, we die!
//		if ( mp_Map->at( x>>CSF, y>>CSF) == TILE_GLOWCELL )
//		{
//			object.zapped += VORTELITE_HP;
//		}
//
//		if (object.zapped >= VORTELITE_HP)
//		{
//			object.inhibitfall = 0;
//			object.canbezapped = 0;
//			object.ai.vortelite.animtimer = 0;
//			object.ai.vortelite.frame = 0;
//			object.ai.vortelite.state = VORTELITE_DYING;
//			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, object.scrx);
//		}
//	}
//	// deadly to the touch
//	if (object.touchPlayer && object.canbezapped)
//		killplayer(object.touchedBy);
//
//	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
//
//	reprocess: ;
//	switch(object.ai.vortelite.state)
//	{
//	case VORTELITE_WALK:
//		object.ai.vortelite.dist_traveled++;
//
//		if (getProbability(VORTELITE_JUMP_PROB) && !darkness && !object.blockedu)
//		{  // let's jump.
//			vortelite_initiatejump(object);
//			goto reprocess;
//		}
//		else
//		{
//			if (object.ai.vortelite.timesincefire > VORTELITE_MIN_TIME_BETWEEN_FIRE)
//			{
//				if (getProbability(VORTELITE_FIRE_PROB))
//				{  // let's fire
//					// usually shoot toward keen
//					if (rand()%5 != 0)
//					{
//						if (object.getXPosition() < m_Player[0].getXPosition())
//						{
//							object.ai.vortelite.movedir = RIGHT;
//						}
//						else
//						{
//							object.ai.vortelite.movedir = LEFT;
//						}
//					}
//					object.ai.vortelite.timer = 0;
//					object.ai.vortelite.state = VORTELITE_ABOUTTOFIRE;
//				}
//			}
//			else object.ai.vortelite.timesincefire++;
//		}
//
//		if (object.ai.vortelite.movedir==LEFT)
//		{  // move left
//			object.sprite = VORTELITE_WALK_LEFT_FRAME + object.ai.vortelite.frame;
//			if (!object.blockedl)
//			{
//				object.moveLeft(VORTELITE_WALK_SPEED);
//			}
//			else
//			{
//				object.ai.vortelite.movedir = RIGHT;
//
//				// if we only traveled a tiny amount before hitting a wall, we've
//				// probably fallen into a small narrow area, and we need to try
//				// to jump out of it
//				if (object.ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && object.blockedd)
//				{
//					vortelite_initiatejump(object);
//					goto reprocess;
//				}
//				else object.ai.vortelite.dist_traveled = 0;
//			}
//		}
//		else
//		{  // move right
//			object.sprite = VORTELITE_WALK_RIGHT_FRAME + object.ai.vortelite.frame;
//			if (!object.blockedr)
//			{
//				object.moveRight(VORTELITE_WALK_SPEED);
//			}
//			else
//			{
//				object.ai.vortelite.movedir = LEFT;
//
//				// if we only traveled a tiny amount before hitting a wall, we've
//				// probably fallen into a small narrow area, and we need to try
//				// to jump out of it
//				if (object.ai.vortelite.dist_traveled < VORTELITE_TRAPPED_DIST && !darkness && object.blockedd)
//				{
//					vortelite_initiatejump(object);
//					goto reprocess;
//				}
//				else object.ai.vortelite.dist_traveled = 0;
//			}
//		}
//
//		// walk animation
//		if (object.ai.vortelite.animtimer > VORTELITE_WALK_ANIM_TIME)
//		{
//			if (object.ai.vortelite.frame>=3) object.ai.vortelite.frame=0;
//			else object.ai.vortelite.frame++;
//			object.ai.vortelite.animtimer = 0;
//		} else object.ai.vortelite.animtimer++;
//		break;
//	case VORTELITE_JUMP:
//		if (object.ai.vortelite.movedir == RIGHT)
//		{ if (!object.blockedr) object.moveRight(VORTELITE_WALK_SPEED); }
//		else
//		{ if (!object.blockedl) object.moveLeft(VORTELITE_WALK_SPEED); }
//
//		if (object.ai.vortelite.inertiay>0 && object.blockedd)
//		{  // the bear has landed
//			object.inhibitfall = 0;
//			object.ai.vortelite.state = VORTELITE_WALK;
//			goto reprocess;
//		}
//		// check if the bear has bonked into a ceiling, if so,
//		// immediately terminate the jump
//		x = object.getXPosition();
//		y = object.getYPosition();
//		bonk = 0;
//		if (TileProperties[mp_Map->at((x>>CSF), (y>>CSF))].bdown) bonk = true;
//		else if (TileProperties[mp_Map->at((x>>CSF)+1, (y>>CSF))].bdown) bonk = true;
//		else if (TileProperties[mp_Map->at(((x+200)>>CSF), (y>>CSF))].bdown) bonk = true;
//		if (bonk && object.ai.vortelite.inertiay < 0)
//		{
//			object.ai.vortelite.inertiay = 0;
//		}
//
//		// apply Y inertia
//		object.moveYDir(object.ai.vortelite.inertiay);
//
//		if (object.ai.vortelite.timer > VORTELITE_JUMP_FRICTION)
//		{ // slowly decrease upgoing rate
//			if (object.ai.vortelite.inertiay<VORTELITE_MAX_FALL_SPEED)
//			{
//				object.ai.vortelite.inertiay+=16;
//			}
//			object.ai.vortelite.timer = 0;
//		} else object.ai.vortelite.timer++;
//		break;
//	case VORTELITE_ABOUTTOFIRE:
//		if (object.ai.vortelite.movedir==RIGHT)
//		{ object.sprite = VORTELITE_FIRE_RIGHT_FRAME; }
//		else
//		{ object.sprite = VORTELITE_FIRE_LEFT_FRAME; }
//		if (object.ai.vortelite.timer > VORTELITE_HOLD_GUN_OUT_TIME)
//		{
//			object.ai.vortelite.timer = 0;
//			object.ai.vortelite.state = VORTELITE_FIRED;
//
//			CObject *newobject = new CObject(mp_Map);
//			if (object.ai.vortelite.movedir==RIGHT)
//			{
//				newobject->spawn(object.getXRightPos()+1, object.getYPosition()+(9<<STC), OBJ_RAY, m_Episode);
//				newobject->ai.ray.direction = RIGHT;
//			}
//			else
//			{
//				newobject->spawn(object.getXLeftPos()-1, object.getYPosition()+(9<<STC), OBJ_RAY, m_Episode, LEFT);
//				newobject->ai.ray.direction = LEFT;
//			}
//			newobject->ai.ray.owner = object.m_index;
//			newobject->sprite = ENEMYRAYEP2;
//			// don't shoot other vorticon elite
//			newobject->ai.ray.dontHitEnable = 1;
//			newobject->ai.ray.dontHit = OBJ_VORTELITE;
//			m_Objvect.push_back(newobject);
//
//			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, object.scrx);
//		}
//		else object.ai.vortelite.timer++;
//		break;
//	case VORTELITE_FIRED:
//		if (object.ai.vortelite.movedir==RIGHT)
//		{ object.sprite = VORTELITE_FIRE_RIGHT_FRAME; }
//		else
//		{ object.sprite = VORTELITE_FIRE_LEFT_FRAME; }
//
//		if (object.ai.vortelite.timer > VORTELITE_HOLD_GUN_AFTER_FIRE_TIME)
//		{
//			object.ai.vortelite.timer = 0;
//			object.ai.vortelite.frame = 0;
//			object.ai.vortelite.timesincefire = 0;
//			object.ai.vortelite.state = VORTELITE_WALK;
//			// head toward keen
//			if (object.getXPosition() < m_Player[0].getXPosition())
//			{
//				object.ai.vortelite.movedir = RIGHT;
//			}
//			else
//			{
//				object.ai.vortelite.movedir = LEFT;
//			}
//		}
//		else object.ai.vortelite.timer++;
//		break;
//	case VORTELITE_DYING:
//		object.sprite = VORTELITE_DYING_FRAME;
//		if (object.ai.vortelite.animtimer > VORTELITE_DIE_ANIM_TIME)
//		{
//			object.sprite = VORTELITE_DEAD_FRAME;
//			object.ai.vortelite.state = VORTELITE_DEAD;
//		}
//		else
//		{
//			object.ai.vortelite.animtimer++;
//		}
//		break;
//	default: break;
//	}
//}
//
//void CObjectAI::vortelite_initiatejump(CObject &object)
//{
//	if (object.ai.vortelite.state==VORTELITE_JUMP) return;
//
//	object.ai.vortelite.frame = 0;
//	object.ai.vortelite.animtimer = 0;
//	object.ai.vortelite.inertiay = \
//			-((rand()%(VORTELITE_MAX_JUMP_HEIGHT-VORTELITE_MIN_JUMP_HEIGHT))+VORTELITE_MIN_JUMP_HEIGHT);
//
//	if (object.ai.vortelite.movedir==RIGHT)
//		object.sprite = VORTELITE_JUMP_RIGHT_FRAME;
//	else
//		object.sprite = VORTELITE_JUMP_LEFT_FRAME;
//
//	object.inhibitfall = 1;
//	object.ai.vortelite.state = VORTELITE_JUMP;
//}

