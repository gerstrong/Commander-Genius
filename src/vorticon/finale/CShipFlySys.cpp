/*
 * CShipFlySys.cpp
 *
 *  Created on: 09.11.2009
 *      Author: gerstrong
 */

#include "CShipFlySys.h"

CShipFlySys::CShipFlySys(CPlayer *p_Player, CMap *p_Map) :
	mp_Map(p_Map), mp_player(p_Player)
{
	// TODO Auto-generated constructor stub
	m_finished = false;
}

void CShipFlySys::setInitialPostion(int x, int y)
{

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
	/*std::vector<CObject> &objvect = *(mp_player[0].mp_object);
	objvect[MARK_SPR_NUM].type = OBJ_YORP;                // doesn't matter
	objvect[MARK_SPR_NUM].exists = 0;
	objvect[MARK_SPR_NUM].sprite = SPR_QUESTION;

	// place the player at the center of mars or earth
	if (mp_Map->findTile()map_findtile( flyback ? 586 : 593, &x, &y))
	{ // found the tile at the center of mars or earth
		mp_player[0].x = ((x<<4)+1)<<CSF;
		mp_player[0].y = ((y<<4)-3)<<CSF;
	}
	else
	{
		crashflag = 1;
		why_term_ptr = "eseq1_shipflys(): unable to find center of Mars.";
		return 1;
	}

	 player[0].playframe = SPR_SHIP_RIGHT;

	 ShipQueuePtr = 0;
	 max_scroll_x = max_scroll_y = 20000;

	 // keep the question or exclamation mark sprite next to the player
	 objects[MARK_SPR_NUM].x = player[0].x + (20<<CSF);
	 objects[MARK_SPR_NUM].y = player[0].y - (10<<CSF);
	 objects[MARK_SPR_NUM].onscreen = 1;
	 objects[MARK_SPR_NUM].scrx = (objects[MARK_SPR_NUM].x>>CSF)-scroll_x;
	 objects[MARK_SPR_NUM].scry = (objects[MARK_SPR_NUM].y>>CSF)-scroll_y;

	 // execute the current command in the queue
	 switch(shipqueue[ShipQueuePtr].cmd)
	 {
	 case CMD_MOVE:
		 switch(shipqueue[ShipQueuePtr].flag1)
		 {
		 case DUP:
			 player[0].y-=SHIPSPD;
			 player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DDOWN:
			 player[0].y+=SHIPSPD/2;
			 player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DLEFT:
			 player[0].x-=SHIPSPD;
			 player[0].playframe = SPR_SHIP_LEFT;
			 break;
		 case DRIGHT:
			 player[0].x+=SHIPSPD;
			 player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DDOWNRIGHT:
			 player[0].x+=SHIPSPD*2;
			 player[0].y+=SHIPSPD*0.8;
			 player[0].playframe = SPR_SHIP_RIGHT;
			 break;
	 }
	 break;
	 case CMD_SPAWNSPR:
		 objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
		 objects[MARK_SPR_NUM].exists = 1;
		 break;
	 case CMD_REMOVESPR:
		 objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
		 objects[MARK_SPR_NUM].exists = 0;
		 break;
	 case CMD_ENABLESCROLLING:
		 scrollingon = 1;
		 break;
	 case CMD_DISABLESCROLLING:
		 scrollingon = 0;
		 break;
	 case CMD_WAIT:
		 break;
	 case CMD_FADEOUT:
		 m_finished = true;
		 break;
	 default: break;
	 }

	 if (shipqueue[ShipQueuePtr].time != 0) // decrease the time remaining
		 shipqueue[ShipQueuePtr].time--;
	 else // no time left on this command, go to next cmd
		 ShipQueuePtr++;
	*/
}


CShipFlySys::~CShipFlySys() {
	// TODO Auto-generated destructor stub
}
