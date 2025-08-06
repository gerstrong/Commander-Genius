/*
 * CLindsey.cpp
 *
 *  Created on: 14.08.2011
 *      Author: gerstrong
 */

#include "CLindsey.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"

#include <engine/core/CBehaviorEngine.h>
#include <fileio/KeenFiles.h>

namespace galaxy {

const int LINDSEY_MOVE_SPEED = 20;
//const int LINDSEY_MOVE_TIME = 60;

CLindsey::CLindsey(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pMap, foeID, x, y, 0),
m_timer(0),
prepareToVanish(false)
{
	setupGalaxyObjectOnMap(0x38EA, 0);

	answermap[0] = "LINDSEY_START_TEXT";

	answermap[1] = "LINDSEY_TEXT1";
	answermap[2] = "LINDSEY_TEXT2";

	answermap[3] = "LINDSEY_END_TEXT1";
	answermap[4] = "LINDSEY_END_TEXT2";

    const auto fullFName = JoinPaths(gKeenFiles.gameDir, "lindsey.lua");
    mLua.loadFile(fullFName);
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

    int curLevel = mpMap->getLevel();
    
    // When Keen touches the pincess give that hint
    if( auto *player =  dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        playSound(GameSound::GET_WETSUIT, SoundPlayMode::PLAY_PAUSEALL);

        std::array< std::string, 3> lindsey_text;

        lindsey_text[0] = gBehaviorEngine.getString(answermap[0]);
        player->m_Inventory.addAchievementTask("A word of advice", 1);

        if(curLevel > 5)
        {
            lindsey_text[1] = gBehaviorEngine.getString(answermap[1]);
            lindsey_text[2] = gBehaviorEngine.getString(answermap[3]);
        }
        else
        {
            lindsey_text[1] = gBehaviorEngine.getString(answermap[2]);
            lindsey_text[2] = gBehaviorEngine.getString(answermap[4]);
        }


        std::vector<CMessageBoxGalaxy*> msgs;

        // Lua custom dialogs
        bool customDlgs = false;

        if(mLua)
        {
            customDlgs = true;

            bool ok = true;
            ok &= mLua.runFunctionRetOneStr("getLindseyDialog", curLevel, lindsey_text[0]);
            ok &= mLua.runFunctionRetOneStr("getLindseyAnswer", curLevel, lindsey_text[1]);
        }

        const int sprVar = player->getSpriteVariantIdx();

        if(!customDlgs)
        {
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lindsey_text[0],
                            gGraphics.getBitmapFromId(sprVar, 108), LEFT, false, nullptr) );
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lindsey_text[1],
                            gGraphics.getBitmapFromId(sprVar, 108), LEFT, false, nullptr) );
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lindsey_text[2],
                            *gGraphics.getBitmapFromStr(sprVar, "KEENTHUMBSUP"), RIGHT, false, nullptr) );
        }
        else
        {
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lindsey_text[0],
                            gGraphics.getBitmapFromId(sprVar, 108), LEFT, false, nullptr) );
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lindsey_text[1],
                            *gGraphics.getBitmapFromStr(sprVar, "KEENTHUMBSUP"), RIGHT, false, nullptr) );
        }

        showMsgVec( msgs );

        prepareToVanish = true;
    }
}


} /* namespace galaxy */
