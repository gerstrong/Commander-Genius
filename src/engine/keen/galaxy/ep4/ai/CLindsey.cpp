/*
 * CLindsey.cpp
 *
 *  Created on: 14.08.2011
 *      Author: gerstrong
 */

#include "CLindsey.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy {

const int LINDSEY_MOVE_SPEED = 20;
const int LINDSEY_MOVE_TIME = 60;

CLindsey::CLindsey(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
m_timer(0),
prepareToVanish(false)
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
    if(prepareToVanish)
	exists = false;
    
    if(m_timer <= 0)
    {
	m_timer = LINDSEY_MOVE_SPEED;
	yDirection = (yDirection==UP) ? DOWN : UP;
    }
    
    
    if(yDirection == UP)
	moveUp(LINDSEY_MOVE_SPEED);
    else
	moveDown(LINDSEY_MOVE_SPEED);
    
    m_timer--;
    
    if(!processActionRoutine())
	exists = false;
}

void CLindsey::getTouchedBy(CSpriteObject &theObject)
{
    if(prepareToVanish)
	return;
    
    // When Keen touches the pincess give that hint
    if( dynamic_cast<CPlayerLevel*>(&theObject) )
    {
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


        std::vector<CMessageBoxGalaxy*> msgs;

        msgs.push_back( new CMessageBoxBitmapGalaxy(lindsey_text[0], gGraphics.getBitmapFromId(108), LEFT) );
        msgs.push_back( new CMessageBoxBitmapGalaxy(lindsey_text[1], gGraphics.getBitmapFromId(108), LEFT) );
        msgs.push_back( new CMessageBoxBitmapGalaxy(lindsey_text[2], *gGraphics.getBitmapFromStr("KEENTHUMBSUP"), RIGHT) );

        showMsgVec( msgs );

        prepareToVanish = true;
    }
}


} /* namespace galaxy */
