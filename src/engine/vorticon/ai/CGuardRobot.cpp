#include "../../../sdl/sound/CSound.h"
#include "../../spritedefines.h"

#include "CTank.h"
#include "CRay.h"

CGuardRobot::CGuardRobot(CMap *p_map, Uint32 x, Uint32 y) :
CTank(p_map, x, y)
{
	// first time initilization
//	state = TANK_WALK;
//	movedir = RIGHT;
//	fireafterlook = 0;
//	animtimer = 0;
//	timer = 0;
//	dist_to_travel = TANK_MINTRAVELDIST;
//	pausetime = 0;
//	timetillcanfire = TANK2_MAX_TIME_TILL_CAN_FIRE;
//	firetimes = 0;
//	detectedPlayer = 0;
//	detectedPlayerIndex = m_Player[0].m_index;
//	turnaroundtimer = 0;
//
//	canbezapped = 1;   // will stop bullets but is not harmed
//	inhibitfall = 1;
//	needinit = 0;
}

void CGuardRobot::process()
{
//	// touched player?
//	if (touchPlayer && !m_Player[touchedBy].pdie)
//		killplayer(touchedBy);
//
//	switch(state)
//	{
//	case TANK_LOOK:
//		// animation
//		if (animtimer > TANK_LOOK_ANIM_TIME)
//		{
//			frame ^= 1;
//			animtimer = 0;
//		}
//		else
//			animtimer++;
//
//		sprite = TANK2_LOOK_FRAME + frame;
//
//		// when time is up go back to moving
//		if (timer > TANK_LOOK_TOTALTIME)
//		{
//			// decide what direction to go
//			if (blockedr)
//			{ movedir = LEFT; }
//			else if (blockedl)
//			{ movedir = RIGHT; }
//			else if (getXPosition() > m_Player[0].getXPosition())
//			{ movedir = LEFT; }
//			else
//			{ movedir = RIGHT; }
//
//			alreadyfiredcauseonsamelevel = 0;
//			timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
//			timetillcanfirecauseonsamelevel = TANK2_TIME_BEFORE_FIRE_WHEN_SEE;
//			firetimes = 0;
//			state = TANK_WALK;
//			frame = 0;
//			animtimer = 0;
//			timer = 0;
//			dist_to_travel = TANK_MINTRAVELDIST;
//		}
//		else
//			timer++;
//
//		break;
//
//	case stTankData::TANK_WALK:
//		// hover animation
//		if (animtimer > TANK_WALK_ANIM_TIME)
//		{
//			if (frame>=3) frame=0;
//			else frame++;
//			animtimer = 0;
//		} else animtimer++;
//
//		if (movedir==LEFT)
//			sprite = TANK2_WALK_LEFT_FRAME + frame;
//		else
//			sprite = TANK2_WALK_RIGHT_FRAME + frame;
//
//		// if we're about to, or just did, fire a volley, don't move
//		if (!hardmode)
//		{
//			if (pausetime)
//			{
//				pausetime--;
//				return;
//			}
//		}
//		else
//			pausetime = 0;
//
//		// are we firing a volley?
//		if (firetimes)
//		{
//			// is it time to fire the next shot in the volley?
//			if (!timetillnextshot)
//			{
//				CRay *newobject = new CRay(mp_Map);
//				if (onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, scrx);
//				if (movedir==RIGHT)
//				{
//					newobject->spawn(getXRightPos()+(8<<STC), getYUpPos()+(5<<STC), OBJ_RAY, m_Episode);
//					newobject->m_Direction = RIGHT;
//				}
//				else
//				{
//					newobject->spawn(getXPosition(), getYUpPos()+(5<<STC), OBJ_RAY, m_Episode, LEFT);
//					newobject->m_Direction = LEFT;
//				}
//				newobject->owner.ID = m_index;
//				newobject->owner.obj_type = OBJ_TANKEP2;
//				newobject->sprite = ENEMYRAYEP2;
//				newobject->dontHitEnable = 0;
//
//				m_Objvect.push_back(newobject);
//
//				timetillnextshot = TANK2_TIME_BETWEEN_SHOTS;
//				if (!--firetimes)
//				{
//					pausetime = TANK_FIRE_PAUSE_TIME;
//				}
//			}
//			else
//			{
//				timetillnextshot--;
//			}
//
//			// don't move when firing except on hard mode
//			if (hardmode)
//				return;
//
//		}
//		else
//		{  // not firing a volley
//			if (!timetillcanfire)
//			{
//				tank2_fire(object);
//			}
//			else
//			{
//				timetillcanfire--;
//			}
//
//		}
//
//		// is keen on same level?
//		tank_searchplayers(object);
//
//		if (detectedPlayer)
//		{
//			unsigned int x = getXPosition();
//			// facing keen?
//			alreadyfiredcauseonsamelevel = 1;
//			// are we facing him?
//			if (((m_Player[detectedPlayerIndex].getXPosition() < x) && movedir==LEFT) || \
//					((m_Player[detectedPlayerIndex].getXPosition() > x) && movedir==RIGHT))
//			{
//				// yes, we're facing him! FIRE!!!
//				if (!firetimes)
//				{
//					if (!timetillcanfirecauseonsamelevel)
//					{
//						tank2_fire(object);
//						timetillcanfirecauseonsamelevel = TANK2_TIME_BETWEEN_FIRE_CAUSE_LEVEL;
//					}
//					else timetillcanfirecauseonsamelevel--;
//				}
//			}
//			else
//			{
//				// no, we're not facing him, on hard difficulty turn around
//				if (hardmode)
//				{
//					if (!turnaroundtimer)
//					{
//						frame = 0;
//						timer = 0;
//						animtimer = 0;
//						state = stTankData::TANK_LOOK;
//						turnaroundtimer = 100;
//					}
//					else turnaroundtimer--;
//				}
//			}
//		}
//		else
//		{  // no, not on same level
//			alreadyfiredcauseonsamelevel = 0;
//			turnaroundtimer = 0;
//		}
//
//
//		if (movedir==LEFT)
//		{  // move left
//			if (!blockedl)
//			{
//				moveLeft(TANK_WALK_SPEED);
//				dist_to_travel--;
//			}
//			else
//			{
//				frame = 0;
//				timer = 0;
//				animtimer = 0;
//				state = stTankData::TANK_LOOK;
//			}
//		}
//		else
//		{  // move right
//			sprite = TANK2_WALK_RIGHT_FRAME + frame;
//			if (!blockedr)
//			{
//				moveRight(TANK_WALK_SPEED);
//				dist_to_travel--;
//			}
//			else
//			{
//				frame = 0;
//				timer = 0;
//				animtimer = 0;
//				state = stTankData::TANK_LOOK;
//			}
//		}
//		break;
//	default : break;
//	}
}

//// makes the tank start firing
//void CGuardRobot::tank2_fire(CObject &object)
//{
//	firetimes = TANK2_SHOTS_PER_VOLLEY;
//	timetillnextshot = 0;
//	timetillcanfire = (rnd()%(TANK2_MAX_TIME_TILL_CAN_FIRE-TANK2_MIN_TIME_TILL_CAN_FIRE))+TANK2_MIN_TIME_TILL_CAN_FIRE;
//	pausetime = TANK_FIRE_PAUSE_TIME;
//}
//
//
//// searches for any players on the same level as the tank
//void CGuardRobot::tank_searchplayers(CObject &object)
//{
//	detectedPlayer = 0;
//	for( size_t i=0 ; i<m_Player.size() ; i++ )
//	{
//		if (m_Player[i].getYPosition() >= getXLeftPos())
//		{
//			if ( (m_Player[i].getYDownPos()) <= (getYDownPos()) )
//			{
//				detectedPlayer = 1;
//				detectedPlayerIndex = i;
//				break;
//			}
//		}
//	}
//}
