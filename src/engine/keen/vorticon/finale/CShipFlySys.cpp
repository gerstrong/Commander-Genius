/*
 * CShipFlySys.cpp
 *
 *  Created on: 09.11.2009
 *      Author: gerstrong
 *
 *  This is used for the scene you see in EP1 and EP2
 *  when the player flies with his ship from point A to B.
 *  TODO: This class might be extended to the Tantalus shoot to earth
 *  scene
 */

#include "CShipFlySys.h"
#include "../ai/CYorp.h"

CShipFlySys::CShipFlySys(CPlayer &Player, std::shared_ptr<CMap> &pMap, int ship_rightsprite, int ship_leftsprite) :
m_player(Player), // in this case the player will be the ship flying home
mpMap(pMap)
{
	m_playsprite_right = ship_rightsprite;
	m_playsprite_left = ship_leftsprite;

	m_finished = false;
	m_scrollingon = true;
	m_ShipQueuePtr = 0;
	m_player.sprite = m_playsprite_right;

	// Now, that everything is initialized, create a mark that will be used
	// as ! or ? Sprite
	// before it was yorp // TODO: This doesn't work anyway! Check!
	/*CVorticonSpriteObject *mark = new CVorticonSpriteObject(mpMap.get(), 0,0, OBJ_YORP);
	mark->m_type = OBJ_YORP;                // doesn't matter
	mark->solid = false;                // doesn't matter
	mark->sprite = SPR_QUESTION;

	// keep the question or exclamation mark sprite next to the player
	mark->onscreen = true;

	m_player.mp_object->push_back(mark);*/

	//mp_mark = mark; // We still need to manipulate it!*/
}

void CShipFlySys::addShipQueue(int cmd, int time, int flag1)
{
	m_shipqueue[m_ShipQueuePtr].cmd = cmd;
	m_shipqueue[m_ShipQueuePtr].time = time;
	m_shipqueue[m_ShipQueuePtr].flag1 = flag1;
	m_ShipQueuePtr++;
}

void CShipFlySys::process()
{
	//int x,y;
	 // execute the current command in the queue
	 switch(m_shipqueue[m_ShipQueuePtr].cmd)
	 {
	 case CMD_MOVE:
		 switch(m_shipqueue[m_ShipQueuePtr].flag1)
		 {
		 case DUP:
			 m_player.moveUp(SHIPSPD);
			 m_player.sprite = m_playsprite_right;
			 break;
		 case DDOWN:
			 m_player.moveDown(SHIPSPD/2);
			 m_player.sprite = m_playsprite_right;
			 break;
		 case DLEFT:
			 m_player.moveLeft(SHIPSPD);
			 m_player.sprite = m_playsprite_left;
			 break;
		 case DRIGHT:
			 m_player.moveRight(SHIPSPD);
			 m_player.sprite = m_playsprite_right;
			 break;
		 case DDOWNRIGHT:
			 m_player.moveRight(SHIPSPD*2);
			 m_player.moveDown(SHIPSPD*0.8);
			 m_player.sprite = m_playsprite_right;
			 break;
		 case DUPLEFT:
			 m_player.moveLeft(SHIPSPD*2);
			 m_player.moveUp(SHIPSPD*0.8);
			 m_player.sprite = m_playsprite_left;
			 break;
		 }

		 //x = m_player.getXPosition();
		 //y = m_player.getYPosition();

		 //mp_mark->moveTo(Vector2D<int>(x + (1<<CSF), y - (1<<CSF)));
	 break;
	 case CMD_SPAWNSPR:
		 //mp_mark->sprite = m_shipqueue[m_ShipQueuePtr].flag1;
		 //mp_mark->exists = true;
		 break;
	 case CMD_REMOVESPR:
		 //mp_mark->sprite = m_shipqueue[m_ShipQueuePtr].flag1;
		 //mp_mark->exists = false;
		 break;
	 case CMD_ENABLESCROLLING:
		 m_scrollingon = true;
		 break;
	 case CMD_DISABLESCROLLING:
		 m_scrollingon = false;
		 break;
	 case CMD_WAIT:
		 break;
	 case CMD_ENDOFQUEUE:
		 m_finished = true;
		 break;
	 default: break;
	 }

	 if (m_shipqueue[m_ShipQueuePtr].time > 0) // decrease the time remaining
		 m_shipqueue[m_ShipQueuePtr].time--;
	 else // no time left on this command, go to next cmd
	 {
		 m_ShipQueuePtr++;
	 }

	 m_player.processCamera();
}


CShipFlySys::~CShipFlySys()
{
	//delete mp_mark;
	//m_player.mp_object->pop_back();
}
