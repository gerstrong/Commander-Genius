/*
 * CAmpton.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CFuse.h"
#include "CShikadiMine.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include <base/utils/misc.h>
#include <audio/music/CMusic.h>
#include "../../common/ai/CEnemyShot.h"
#include "../../menu/ComputerWrist.h"
#include "engine/core/mode/CGameMode.h"
#include <base/GsTimer.h>



/*
$3186W #QED?
 */

const int secretLevel = 13;

namespace galaxy {  
  
  
CFuse::CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar)
{  
	// Adapt this AI
	m_ActionBaseOffset = 0x3186;
	setActionForce(0);
	//setupGalaxyObjectOnMap(0x3186, 0);
	
	xDirection = LEFT;
	
	mpMap->mNumFuses++;
	mpMap->mFuseInLevel = true;

    loadLuaScript("qed");
}

bool CFuse::loadLuaScript(const std::string &scriptBaseName)
{
    bool ok = true;

    auto fullFName = JoinPaths(gKeenFiles.gameDir ,"ai");
    fullFName = JoinPaths(fullFName, scriptBaseName + ".lua");

    ok &= mLua.loadFile( fullFName );

    if(!ok)
        return false;

    const int level = mpMap->getLevel();
    std::string text;
    ok &= mLua.runFunctionRetOneStr("getLevelText", level, text);

    if(!ok)
        return false;

    std::string levelText = "LEVEL_TEXT";
    levelText += itoa(level);
    gBehaviorEngine.setMessage(levelText, text);

    int bmpIdx = mBmpIdx;
    mLua.runFunctionRetOneInt("getLevelTextBmp", level, bmpIdx);
    mBmpIdx = bmpIdx;

    return true;
}


static const int CSF_DISTANCE_TO_EXPLODE = (3<<CSF);

bool CFuse::isNearby(CSpriteObject &theObject)
{
    if( auto *thePlayer = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        mSecretFuseBroken = thePlayer->m_Inventory.Item.fuse_level_secret_completed;
    }


    if( CMineShards *mineShard = dynamic_cast<CMineShards*>(&theObject) )
    {
        const auto shardPos = mineShard->getMidPos();
        const auto minePos = getMidPos();

        // Chance to explode?
        if( shardPos.x < minePos.x - CSF_DISTANCE_TO_EXPLODE ||
            shardPos.x > minePos.x + CSF_DISTANCE_TO_EXPLODE )
            return false;

        if( shardPos.y < minePos.y - CSF_DISTANCE_TO_EXPLODE ||
            shardPos.y > minePos.y + CSF_DISTANCE_TO_EXPLODE )
            return false;

        // Quickly decorate the rotten QED!
        const int dx = (getXMidPos()>>CSF)-1;
        const int dy = (getYMidPos()>>CSF)-1;

        playSound( SOUND_FUSE_BREAK, SoundPlayMode::PLAY_FORCE );

        for(int i=0 ; i<4 ; i++)
        {
            const int t1 = mpMap->at(i,0);
            const int t2 = mpMap->at(i,1);
            const int t3 = mpMap->at(i+4,0);
            const int t4 = mpMap->at(i+4,1);
            mpMap->setTile(dx+i, dy,   t1, true, 1);
            mpMap->setTile(dx+i, dy+1, t2, true, 1);
            mpMap->setTile(dx+i, dy+2, t3, true, 1);
            mpMap->setTile(dx+i, dy+3, t4, true, 1);
        }

        winGame(5000);
        mineShard->exists = false;
        mIsDead = true;

        return true;
    }

    return false;
}


void CFuse::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;


    if( auto *thePlayer = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        const auto level = mpMap->getLevel();
        std::string levelText = "LEVEL_TEXT";
        levelText += itoa(level);

        const auto msg = gBehaviorEngine.getString(levelText);

        if(!msg.empty())
        {
            if(level == secretLevel)
            {
                thePlayer->m_Inventory.Item.fuse_level_secret_completed = true;
            }

            thePlayer->m_Inventory.Item.fuse_levels_completed++;
            mpMap->mFuseInLevel = false;

            std::vector<CMessageBoxGalaxy*> msgs;

            int sprVar = thePlayer->getSpriteVariantIdx();

            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar,
                                msg,
                                gGraphics.getBitmapFromId(sprVar, mBmpIdx),
                                RIGHT,
                                false, nullptr) );

            showMsgVec( msgs );
            mIsDead = true;
        }
    }
    else if( auto *theEnemyShot = dynamic_cast<CEnemyShot*>(&theObject) ) // happens when Keen 9 - Fight against Mortimer
    {
        // Now replace those tiles
        for(int x=m_BBox.x1 ; x<m_BBox.x2 ; x+=(1<<CSF))
        {
            for(int y=m_BBox.y1 ; y<m_BBox.y2 ; y+=(1<<CSF))
            {
                const Uint16 where_x = (getXPosition()+x)>>CSF;
                const Uint16 where_y = (getYPosition()+y)>>CSF;

                mpMap->setTile(where_x, where_y, 0, true, 1);
            }
        }

        mIsDead = true;
        theEnemyShot->mIsDead = true;
    }        

}

void CFuse::winGame(const int msecs)
{
    mGameEndsTime = msecs + timerTicks();
}



void CFuse::process()
{
    // TODO: We might need a scattermLevelTestBmp effect here
	//if(!processActionRoutine())
	  //  exists = false;

    // This is set to true when game can end
    if(mGameEndsTime)
    {
        if(timerTicks() > mGameEndsTime)
        {
            gEventManager.add(new EventEndGamePlay());

            if(mSecretFuseBroken)
            {
                gEventManager.add(new OpenComputerWrist(5, false, false, 12, true));
            }
            else
            {
                gEventManager.add(new OpenComputerWrist(4, false, false, 12, true));
            }
        }
    }
}

}
