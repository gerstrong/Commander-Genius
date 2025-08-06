/*
 * CFlag.cpp
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 */

#include "CFlag.h"
#include <base/audio/Audio.h>

#include <base/interface/property_tree/property_tree.h>
#include <fileio/CSaveGameController.h>
#include <engine/core/CBehaviorEngine.h>

namespace galaxy {    

enum FLAGACTIONS
{
    A_FLAG_WAVE = 0,
    A_FLAG_FLIP = 4
};
    

//const Uint16 FLYING_BASEFRAME_EP4 = 174;
//const Uint16 FLYING_BASEFRAME_EP5 = 173;

//const Uint16 ANIMATION_TIME = 8;


CFlag::CFlag(std::shared_ptr<CMap> pMap, const GsVec2D<Uint32> &Location,
            const GsVec2D<Uint32> &dest,
            const int sprVar, bool newAction, const bool canLock ) :
CGalaxySpriteObject(pMap, FOE_ID, Location.x, Location.y, sprVar),
m_destination(dest)
{
	solid = false;
	honorPriority = false;
		
	mActionMap[A_FLAG_WAVE] = &CFlag::processWaving;
	mActionMap[A_FLAG_FLIP] = &CFlag::processFlipping;
	
	const auto episode = gBehaviorEngine.getEpisode();    

    if(canLock)
    {
        mpMap->lock();
    }

	if(episode == 6)
	{
        setupGalaxyObjectOnMap(gBehaviorEngine.isDemo() ? 0x145A : 0x13F4, A_FLAG_FLIP);
	}
	else if(episode == 5)
	{
        // In Episode 5 the sign is not thrown! It just appears in the holder. Put it there!
	    moveTo(m_destination);
	    setupGalaxyObjectOnMap(0x148A, A_FLAG_WAVE);
        mpMap->unlock();
        mPlayMapSound = false; // There is no waving Flag so no sound fro that.
	}
    else // Episode 4
	{
	    setupGalaxyObjectOnMap(0x15EE, A_FLAG_FLIP);                
	}

}

void CFlag::serialize(GsKit::ptree &node)
{
  auto &posNode = node.put("position", "");
  posNode.put("<xmlattr>.x", m_Pos.x);
  posNode.put("<xmlattr>.y", m_Pos.y);
}

void CFlag::deserialize(GsKit::ptree &node)
{
    auto &posNode = node.put("position", "");
    m_Pos.x = posNode.get<int>("<xmlattr>.x", m_Pos.x);
    m_Pos.y = posNode.get<int>("<xmlattr>.y", m_Pos.y);
}

void CFlag::deserialize(CSaveGameController &savedGame)
{
  savedGame.decodeData(m_Pos.x);
  savedGame.decodeData(m_Pos.y);
}

void CFlag::serialize(CSaveGameController &savedGame)
{
  savedGame.encodeData(m_Pos.x);
  savedGame.encodeData(m_Pos.y);
}

void CFlag::getTouchedBy(CSpriteObject &theObject)
{    
    if( getActionNumber(A_FLAG_FLIP) )
        return;
    
    // In case another flag is sitting in the pole, make that one non existent
    if( CFlag *flag = dynamic_cast<CFlag*>(&theObject) )
    {
        if( flag->getActionNumber(A_FLAG_WAVE) )
        {
            exists = false;
        }
    }
}

void CFlag::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		processState = mActionMap[ActionNumber];
	else
		CGalaxySpriteObject::setActionForce(0); // This might happen, when the action-map is incomplete
}

/**
 * Main process cycle
 */
void CFlag::process()
{
    if(mPlayMapSound)
    {
        playSound( GameSound::FLAG_APPEAR );
        mPlayMapSound = false;
    }

    processActionRoutine();

    (this->*processState)();
}

/**
 * Called when Flag is flying to the pole
 */
void CFlag::processFlipping()
{
	if(m_Pos != m_destination)
	{        
        GsVec2D<int> dir = m_destination - m_Pos;
        const float length = dir.GetLength();
        GsVec2D<float> base_dir( dir.x/length, dir.y/length );

        if( fabs(length) < mSpeed )
		{
			moveTo(m_destination);
		}
		else
		{
            const auto moveAmt = base_dir*float(mSpeed);
            moveDir(moveAmt);
		}
	}
	else
	{
	    setAction(A_FLAG_WAVE);        
	    setActionSprite();
        playSound( GameSound::FLAG_LAND );
	    
	    const auto episode = gBehaviorEngine.getEpisode();
        if(episode == 6)
        {
            GsVec2D<int> tilePos = m_Pos;

            tilePos.y = getYDownPos();

            Uint32 new_tile_no = mpMap->getPlaneDataAt(1, tilePos)+1;
            tilePos = tilePos>>CSF;
            mpMap->setTile(tilePos.x, tilePos.y, new_tile_no, true);
        }
        mpMap->unlock();
	}
}

/*
 * Called when flag is in the pole. Keen 4 or 6
 */
void CFlag::processWaving()
{}

bool CFlag::calcVisibility()
{
    return true;
}

}
