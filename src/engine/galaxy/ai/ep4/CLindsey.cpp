/*
 * CLindsey.cpp
 *
 *  Created on: 14.08.2011
 *      Author: gerstrong
 */

#include "CLindsey.h"

namespace galaxy {

const int LINDSEY_MOVE_SPEED = 20;
const int LINDSEY_MOVE_TIME = 60;

CLindsey::CLindsey(CMap *pmap, Uint32 x, Uint32 y) :
CSpriteObject(pmap, x, y, OBJ_NONE),
m_timer(0)
{
	setupGalaxyObjectOnMap(0x38EA, 0);

	answermap[0] = "LINDSEY_START_TEXT";

	answermap[1] = "LINDSEY_TEXT1";
	answermap[2] = "LINDSEY_TEXT2";

	answermap[3] = "LINDSEY_END_TEXT1";
	answermap[4] = "LINDSEY_END_TEXT2";
}

void CLindsey::process()
{
	if(m_timer <= 0)
	{
		m_timer = LINDSEY_MOVE_SPEED;
		m_vDir = (m_vDir==UP) ? DOWN : UP;
	}


	if(m_vDir == UP)
		moveUp(LINDSEY_MOVE_SPEED);
	else
		moveDown(LINDSEY_MOVE_SPEED);

	m_timer--;

	processActionRoutine();
}

void CLindsey::getTouchedBy(CSpriteObject &theObject)
{
	// When Keen touches the pincess give that hint
	if(theObject.m_type == OBJ_PLAYER)
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		g_pSound->playSound(SOUND_GET_WETSUIT, PLAY_PAUSEALL);

		std::string lindsey_text[3];

		lindsey_text[0] = g_pBehaviorEngine->getString(answermap[0]);


		Uint16 cur_level = mp_Map->getLevel();
		if(cur_level > 5)
		{
			lindsey_text[1] = g_pBehaviorEngine->getString(answermap[1]);
			lindsey_text[2] = g_pBehaviorEngine->getString(answermap[3]);
		}
		else
		{
			lindsey_text[1] = g_pBehaviorEngine->getString(answermap[2]);
			lindsey_text[2] = g_pBehaviorEngine->getString(answermap[4]);
		}


		EventContainer.add( new EventSendBitmapDialogMsg(108, lindsey_text[0], LEFT) );
		EventContainer.add( new EventSendBitmapDialogMsg(108, lindsey_text[1], LEFT) );
		EventContainer.add( new EventSendBitmapDialogMsg(106, lindsey_text[2], RIGHT) );

		exists = false;
	}
}


} /* namespace galaxy */
