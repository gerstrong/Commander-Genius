/*
 * CMapLoaderGalaxyEp6.cpp
 *
 *  Created on: 24.12.2012
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxyEp6.h"
#include "ai/CBloog.h"
#include "ai/CBlooguard.h"
#include "ai/CBlooglet.h"
#include "ai/CFlect.h"
#include "ai/CBipShip.h"
#include "ai/CBlorb.h"
#include "ai/CGrabbiter.h"
#include "ai/CSpecialItem.h"
#include "ai/CBabobba.h"
#include "ai/CBip.h"
#include "ai/CBobba.h"
#include "ai/CCeilick.h"
#include "ai/CGik.h"
#include "ai/CNospike.h"
#include "ai/COrbatrix.h"
#include "ai/CFleex.h"
#include "ai/CRope.h"
#include "ai/CRocket.h"
#include "ai/CSatelite.h"
#include "ai/CMolly.h"

#include "../common/ai/CPlayerWM.h"
#include "../common/ai/CPlayerLevel.h"

#include "../common/ai/platforms.h"
#include "../common/ai/CFlag.h"
#include "../common/ai/CSpriteItem.h"
#include "../common/ai/platform/CPlatformSit.h"
#include "../common/ai/Autogun.h"

#include <engine/core/CBehaviorEngine.h>

namespace galaxy
{


CMapLoaderGalaxyEp6::CMapLoaderGalaxyEp6(std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
                                         std::vector<CInventory> &inventoryVec,
                                         bool demo = false) :
CMapLoaderGalaxy(ObjectPtr, inventoryVec)
{
    m_demo = demo;
}

bool CMapLoaderGalaxyEp6::isKeenPlayer(const int foeID)
{
    return (foeID > 0 && foeID <= 3);
}


/**
 * @brief	Loads a foe given by the coordiantes
 */
CGalaxySpriteObject* CMapLoaderGalaxyEp6::addFoe(std::shared_ptr<CMap> map, word foe, size_t x, size_t y)
{
	CGalaxySpriteObject* p_newfoe = nullptr;

    const int totalNumPlayer = int(mInventoryVec.size());
	
	// Gems
	for( Uint32 i=0x39 ; i<=0x3C ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 118+2*(i-0x39);
            p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, newsprite, 0);
		}
	}
	
	// Point Item Sprites (Candies, etc...)
	for( Uint32 i=0x3D ; i<=0x43 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 104+2*(i-0x3D);
            p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, newsprite, 0);
		}
	}

	// Neuronal-stunner
	if( foe == 0x44 )
	{
        p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, 127, 0);
	}
	
	// Neuronal-stunner which appears if you are low on bullets
	if( foe == 0x45 )
	{
        for( auto &inventory : mInventoryVec)
        {
            if(inventory.Item.m_bullets < 5)
            {
                p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, 127, 0);
                break;
            }
        }
	}	
	
	// If a foe was found, just return.
	if( p_newfoe )
		return p_newfoe;

	const Difficulty difficulty = gBehaviorEngine.mDifficulty;

	// otherwise look for special foe.
    GsVec2D<Uint32> loc(x,y);


    auto addPlayerInLevel = [&]()
    {
        if(totalNumPlayer <= mNumLoadedPlayers)
            return;

        assert(mNumLoadedPlayers < int(mInventoryVec.size()));
        auto &inventory = mInventoryVec[mNumLoadedPlayers];

        if(inventory.Item.m_lifes >= 0)
        {

            // This is the player on the map in one level
            inventory.Item.mLevelName = map->getLevelName();
            p_newfoe = new galaxy::CPlayerLevel(map, foe,
                                                x, y, m_ObjectPtr,
                                                (foe==1) ? RIGHT : LEFT,
                                                inventory,
                                                m_demo ? 0x91E : 0x89A,
                                                mNumLoadedPlayers,
                                                inventory.mSpriteVar);
        }
        mNumLoadedPlayers++;
    };

    auto addPlayerOnWM = [&]()
    {
        if(totalNumPlayer <= mNumLoadedPlayers)
            return;

        assert(mNumLoadedPlayers < int(mInventoryVec.size()));
        auto &inventory = mInventoryVec[mNumLoadedPlayers];

        if(inventory.Item.m_lifes >= 0)
        {

            // This is the player on the world map
            // Add the Camera into the game scene and attach it to this player
            inventory.Item.mLevelName = map->getLevelName();
            p_newfoe =
                new galaxy::CPlayerWM(map,foe, x, y, inventory,
                                      m_demo ? 0x1446 : 0x13E0,
                                      mNumLoadedPlayers,
                                      inventory.mSpriteVar );
        }

        mNumLoadedPlayers++;
    };

	switch(foe)
	{
	case 0x01:
    case 0x02:
        addPlayerInLevel();
        break;

	case 0x03:
        addPlayerOnWM();
        break;

	case 0x06: if( difficulty < HARD ) break;
	case 0x05: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x04: 
		// Bloog
        p_newfoe = new galaxy::CBloog(map, foe, x, y);
		break;
		

	// Blooglets        
    case 0x07:	p_newfoe = new galaxy::CBlooglet(map, foe, m_demo ? 0x1D4A : 0x208A, false, x, y);	break;
    case 0x08:	p_newfoe = new galaxy::CBlooglet(map, foe, m_demo ? 0x1DE0 : 0x2120, false, x, y);	break;
    case 0x09:	p_newfoe = new galaxy::CBlooglet(map, foe, m_demo ? 0x1E76 : 0x21B6, false, x, y);	break;
    case 0x0A:	p_newfoe = new galaxy::CBlooglet(map, foe, m_demo ? 0x1F0C : 0x224C, false, x, y);	break;
	
	// Blooglets carrying gems
    case 0x0B:	p_newfoe = new galaxy::CBlooglet(map, foe, 0x208A, true, x, y);	break;
    case 0x0C:	p_newfoe = new galaxy::CBlooglet(map, foe, 0x2120, true, x, y);	break;
    case 0x0D:	p_newfoe = new galaxy::CBlooglet(map, foe, 0x21B6, true, x, y);	break;
    case 0x0E:	p_newfoe = new galaxy::CBlooglet(map, foe, 0x224C, true, x, y);	break;
	
	case 0x13: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x12: 
		// Fleex
        p_newfoe = new galaxy::CFleex(map, foe, x, y);
		break;	
	
	// Molly the Babysitter
    case 0x18:	p_newfoe = new galaxy::CMolly(map, foe, x, y);	break;

		
    case 0x1B:
        p_newfoe = new galaxy::CPlatformVertical( map, foe, x, y, UP, m_demo ? 0x1A98 : 0x1DD8, 0 );
			break;
    case 0x1C:
        p_newfoe = new galaxy::CPlatformHorizontal( map, foe, RIGHT, x, y, m_demo ? 0x1A98 : 0x1DD8, 0 );
			break;
    case 0x1D:
        p_newfoe = new galaxy::CPlatformVertical( map, foe, x, y, DOWN, m_demo ? 0x1A98 : 0x1DD8, 0 );
			break;
    case 0x1E:
        p_newfoe = new galaxy::CPlatformHorizontal( map, foe, LEFT, x, y, m_demo ? 0x1A98 : 0x1DD8, 0 );
			break;		
			
    case 0x20:
        p_newfoe = new galaxy::CPlatformDrop( map, foe, x, y, m_demo ? 0x1A98 : 0x1E14, 0); break;
		

	case 0x21: if (difficulty >= NORMAL) break;
	case 0x22: if (difficulty >= HARD) break;
    case 0x23:
        p_newfoe = new galaxy::CPlatformSit( map, foe, x, y, m_demo ? 0x1A98 : 0x1E14, 0); break;

			
        // Var Plats red color
	case 0x24:
        p_newfoe = new galaxy::CVarPlatform( map, foe, x, y, CENTER, UP, m_demo ? 0x1A98 : 0x1E6E, 0); break;
	case 0x25:
        p_newfoe = new galaxy::CVarPlatform( map, foe, x, y, RIGHT, CENTER, m_demo ? 0x1A98 : 0x1E6E, 0);	break;
	case 0x26:
        p_newfoe = new galaxy::CVarPlatform( map, foe, x, y, CENTER, DOWN, m_demo ? 0x1A98 : 0x1E6E, 0);	break;
	case 0x27:
        p_newfoe = new galaxy::CVarPlatform( map, foe, x, y, LEFT, CENTER, m_demo ? 0x1A98 : 0x1E6E, 0);	break;
			
			
	case 0x28:
        p_newfoe = new galaxy::CPlatformMoveAway( map, foe, x, y, CENTER, LEFT, m_demo ? 0x1A98 : 0x1EC8, 0);
			break;
			
	case 0x2B: if( difficulty < HARD ) break;
	case 0x2A: if( difficulty < NORMAL ) break;   
		// This is a Bobba
        p_newfoe = new galaxy::CBobba(map, foe, x, y);
		break;

	case 0x31: if( difficulty < HARD ) break;
	case 0x30: if( difficulty < NORMAL ) break;
	case 0x2F: // This happens normally only in Mods like Keen 8
		// This is a Nospike
        p_newfoe = new galaxy::CNospike(map, foe, x, y);
		break;
			

	case 0x34: if (difficulty < HARD) break;
	case 0x33: if (difficulty < NORMAL) break;
	case 0x32:
        p_newfoe = new galaxy::CGik( map, foe, x, y);
		break;

	case 0x35: if ( difficulty < NORMAL ) break;
        p_newfoe = new galaxy::AutoShot(map, foe, x, y, CENTER, UP, 130, 0);
		break;

	case 0x36: if ( difficulty < NORMAL ) break;
        p_newfoe = new galaxy::AutoShot(map, foe, x, y, RIGHT, CENTER, 130, 0);
		break;


	case 0x37: if ( difficulty < NORMAL ) break;
        p_newfoe = new galaxy::AutoShot(map, foe, x, y, CENTER, DOWN, 130, 0);
		break;

	case 0x38: if ( difficulty < NORMAL ) break;
        p_newfoe = new galaxy::AutoShot(map, foe, x, y, LEFT, CENTER, 130, 0);
		break;

	case 0x48: if (difficulty < HARD) break;
	case 0x47: if (difficulty < NORMAL) break;
	case 0x46:
        p_newfoe = new galaxy::COrbatrix( map, foe, x, y, 0);
		break;


	
	case 0x4B: if( difficulty < HARD ) break;
	case 0x4A: if( difficulty < NORMAL ) break;
		// Bipship
        p_newfoe = new galaxy::CBipShip(map, foe, x, y, 0);
		break;
	
	case 0x4E: if( difficulty < HARD ) break;
	case 0x4D: if( difficulty < NORMAL ) break;
	case 0x4C: 
		// Flect
        p_newfoe = new galaxy::CFlect(map, foe, x, y);
		break;
		
	case 0x51: if( difficulty < HARD ) break;
	case 0x50: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x4F: 
		// Blorb
        p_newfoe = new galaxy::CBlorb(map, foe, x, y);
		break;

	case 0x54: if( difficulty < HARD ) break;
	case 0x53: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x52: 
		// Ceilick
        p_newfoe = new galaxy::CCeilick(map, foe, x, y);
		break;
			
	
	case 0x57: if( difficulty < HARD ) break;
	case 0x56: if( difficulty < NORMAL ) break;   
	case 0x55: 
		// This is a Blooguard
        p_newfoe = new galaxy::CBlooguard(map, foe, x, y);
		break;


	case 0x58: 
		// This is a Grabbiter on the World map
        p_newfoe = new galaxy::CGrabbiter(map, foe, x, y);
		break;

	// Satelite
    case 0x59: p_newfoe = new galaxy::CSatelite(map, foe, x, y); break;
		

	// Special Items which end the level
    case 0x63: p_newfoe = new galaxy::CSpecialItem(map, foe, x, y, 0x1D24); break;
    case 0x64: p_newfoe = new galaxy::CSpecialItem(map, foe, x, y, 0x1D06); break;
    case 0x65: p_newfoe = new galaxy::CSpecialItem(map, foe, x, y, 0x1D42); break;

	
	case 0x68: if( difficulty < HARD ) break;
	case 0x67: if( difficulty < NORMAL ) break;   
	case 0x66: 
		// This is a Babobba
        p_newfoe = new galaxy::CBabobba(map, foe, x, y);
		break;
	
	// Rocket
    case 0x69: p_newfoe = new galaxy::CRocket(map, foe, x, y); break;


	case 0x6C: 
		// This is a Bip (In normal level it is never loaded but from saved games it could be restored)
        p_newfoe = new galaxy::CBip(map, foe, x, y);
		break;		

	// Not sure if after that there are more sprites to load		
	// This special sprite is only loaded by saved game. Normal gamemaps do not have that flag
	case CFlag::FOE_ID:
        p_newfoe = new galaxy::CFlag(map, loc, loc, 0, false, false);
		break;

	case CRope::FOE_ID:
        p_newfoe = new galaxy::CRope(map, x, y); break;

	default:
		break;
	}


	return p_newfoe;
}

};
