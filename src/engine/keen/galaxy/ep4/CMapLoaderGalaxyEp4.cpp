/*
 * CMapLoaderGalaxyEp4.cpp
 *
 *  Created on: 28.07.2012
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxyEp4.h"

// Episode 4
#include "../common/ai/CPlayerWM.h"
#include "../common/ai/CPlayerLevel.h"
#include "ai/CPlayerDive.h"
#include "ai/CPoisonSlug.h"
#include "ai/CMadMushroom.h"
#include "ai/CCouncilMember.h"
#include "ai/CMiragia.h"
#include "ai/CDiveSuit.h"
#include "ai/CArachnut.h"
#include "ai/CBounder.h"
#include "ai/CDopeFish.h"
#include "ai/CWaterMine.h"
#include "ai/CDevilSprite.h"
#include "ai/CSchoolFish.h"
#include "ai/CLick.h"
#include "ai/CLindsey.h"
#include "ai/CWormmouth.h"
#include "ai/CSkypest.h"
#include "ai/CMimrock.h"
#include "ai/CEgg.h"
#include "ai/CBerkeloid.h"
#include "ai/CBlueBird.h"
#include "ai/CThunderCloud.h"
#include "ai/CTreasureEater.h"
#include "ai/CInchWorm.h"
#include "ai/CFoot.h"

#include "engine/keen/galaxy/common/ai/platform/CPlatformHorizontal.h"
#include "engine/keen/galaxy/common/ai/platform/CPlatformVertical.h"
#include "engine/keen/galaxy/common/ai/platform/CPlatformDrop.h"

#include "../common/ai/CFlag.h"
#include "../common/ai/Autogun.h"
#include "../common/ai/CSpriteItem.h"

#include <engine/core/CBehaviorEngine.h>
#include <graphics/GsGraphics.h>

namespace galaxy
{

CMapLoaderGalaxyEp4::CMapLoaderGalaxyEp4(std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
        std::vector<CInventory> &inventoryVec) :
CMapLoaderGalaxy( ObjectPtr, inventoryVec)
{}


// Platforms:
// 27-30 & 32 seem to be the actual platforms.  There may be more of numbers lower than 27.
// 31 seem to be the stoppers, those are not created because the platform
// get this from the object map directly

//const unsigned int PLATFORM_VERT_ALT = 27;
//const unsigned int PLATFORM_HORIZ_ALT = 28;
//const unsigned int PLATFORM_VERT = 29;
//const unsigned int PLATFORM_HORIZ = 30;
// 31 does not count, because it's a blocker.
//const unsigned int PLATFORM_DROP = 32;

//const unsigned int DIVE_SUIT = 35;


bool CMapLoaderGalaxyEp4::isKeenPlayer(const int foeID)
{
    return ((foeID > 0 && foeID <= 3) || (foeID == 0x2A));
}

/**
 * @brief	Loads a foe given by the coordiantes
 */
std::shared_ptr<CGalaxySpriteObject> CMapLoaderGalaxyEp4::addFoe(std::shared_ptr<CMap> map, word foe, const size_t x, const size_t y)
{
	CGalaxySpriteObject* p_newfoe = nullptr;

    const int totalNumPlayer = mInventoryVec.size();

    const int variations = gGraphics.spriteVecVec().size();

			
	// Point Item Sprites (Candies, etc...)
	for( Uint32 i=61 ; i<=67 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 103+2*(i-61);
            p_newfoe = new galaxy::CSpriteItem(map, foe, x, y,
                                               newsprite, x%variations);
		}
	}

	// Neuronal-stunner
	if( foe == 68 )
	{
        p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, 127, 0);
	}

	// Gems
	for( Uint32 i=57 ; i<=60 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 118+2*(i-57);
            p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, newsprite, 0);
		}
	}	
	
	
	const Difficulty difficulty = gBehaviorEngine.mDifficulty;

	// If a foe was found just return.
	if( p_newfoe )
        return std::shared_ptr<CGalaxySpriteObject>(p_newfoe);


	// otherwise look for special foe.
	GsVec2D<Uint32> loc(x,y);


	switch(foe)
	{
	case 0x01:
    case 0x02:

        if(totalNumPlayer > mNumLoadedPlayers)
        {
            assert(mNumLoadedPlayers < int(mInventoryVec.size()));
            auto &inventory = mInventoryVec[mNumLoadedPlayers];
            if(inventory.Item.m_lifes >= 0)
            {
                // This is the player on the map in one level
                inventory.Item.mLevelName = map->getLevelName();
                p_newfoe = new galaxy::CPlayerLevel(map, foe, x, y,
                                                    m_ObjectPtr,
                                                    (foe==0x01) ? RIGHT : LEFT,
                                                    inventory, 0x98C,
                                                    mNumLoadedPlayers,
                                                    inventory.mSpriteVar);
            }
            mNumLoadedPlayers++;
        }
        break;

	case 0x03:

        if(totalNumPlayer > mNumLoadedPlayers)
        {
            assert(mNumLoadedPlayers < int(mInventoryVec.size()));
            auto &inventory = mInventoryVec[mNumLoadedPlayers];

            if(inventory.Item.m_lifes >= 0)
            {
                // This is the player on the world map
                // Add the Camera into the game scene and attach it to this player
                inventory.Item.mLevelName = map->getLevelName();
                p_newfoe = new galaxy::CPlayerWM(map, foe, x, y,
                                                 inventory, 0x15C2,
                                                 mNumLoadedPlayers,
                                                 inventory.mSpriteVar);
            }
            mNumLoadedPlayers++;
        }
        break;

	case 0x04:
		//This is a council member.
		p_newfoe = new galaxy::CCouncilMember(map, foe, x, y);
		break;
		
	case 0x05:
		p_newfoe = new galaxy::CBerkeloid(map, foe, x, y);
		break;

	case 0x06:
		//This is pincess Lindsey.
		p_newfoe = new galaxy::CLindsey(map, foe, x, y);
		break;

	case 0x07:
		// This is a wormmouth.
		p_newfoe = new galaxy::CWormmouth(map, foe, x, y);
		break;

	case 0x08:
		// This is Skypest.
		p_newfoe = new galaxy::CSkypest(map, foe, x, y);
		break;


	case 0x09:
		//This is the Thunder Cloud
        p_newfoe = new galaxy::CThunderCloud(map, foe, x, y, 0);
		break;
		
	case 0x0A:
		// This is the Foot
        map->isSecret = true;
		p_newfoe = new galaxy::CFoot(map, foe, 0x2EF4, x, y);
		break;
		
	case 0x0B:
        // This is one inchworm.
        p_newfoe = new galaxy::CInchWorm(map, foe, x, y);
		break;

	case 0x0C:
		// This is a Bounder.
		p_newfoe = new galaxy::CBounder(map, foe, x, y);
		break;

	case 0x0D:
		// This is an egg
		if( difficulty > EASY )
			p_newfoe = new galaxy::CBlueBird(map, foe, x, y);
		else
			p_newfoe = new galaxy::CEgg(map, foe, x, y);
		break;
		
	case 0x0E:
		p_newfoe = new galaxy::CLick(map, foe, x, y);
		break;	    
	
	case 0x0F:
		// Dope Fish
        p_newfoe = new galaxy::CDopeFish(map, foe, x, y, 0);
		break;	    

	case 0x10: 
        p_newfoe = new galaxy::CSchoolFish(map, foe, x, y);
		break;
		
	case 0x11:
        p_newfoe = new galaxy::CDevilSprite(map, foe, x, y );
		break;
	    
	case 0x12:
		// This might be the treasure eater.
		p_newfoe = new galaxy::CTreasureEater(map, foe, x, y);
		break;

	case 0x13:
		//This is a Mimrock.
		p_newfoe = new galaxy::CMimrock(map, foe, x, y);
		break;

	case 0x14:
		// This is an Arachnut.
		p_newfoe = new galaxy::CArachnut(map, foe, x, y);
		break;

	case 0x15:
		// This is a Mad Mushroom.
		p_newfoe = new galaxy::CMadMushroom(map, foe, x, y);
		break;


	case 0x16:
		// This is a Poison Slug.
        p_newfoe = new galaxy::CPoisonSlug(map, foe, x, y);
		break;

	case 0x18: if( difficulty < HARD ) break;
	case 0x17: if( difficulty < NORMAL ) break;	// not sure here    
		// This is a Sprite from the well of wishes.
		p_newfoe = new galaxy::CDevilSprite(map, foe, x, y);
		break;

	// case 0x19: This is horizontal blocker, not a sprite to load!!
	// case 0x1A: This is vertical blocker, not a sprite to load!!

	case 0x1B: // Plat starts moving up
        p_newfoe = new galaxy::CPlatformVertical(map, foe, x, y, UP, 0x316A, 0); break;
		
	case 0x1D: // Plat starts moving down
        p_newfoe = new galaxy::CPlatformVertical(map, foe, x, y, DOWN, 0x316A, 0); break;
		
	case 0x1C: // plat moves to the RIGHT
        p_newfoe = new galaxy::CPlatformHorizontal(map, foe, RIGHT, x, y, 0x316A, 0); break;
		
	case 0x1E: // plat moves to the LEFT
        p_newfoe = new galaxy::CPlatformHorizontal(map, foe, LEFT, x, y, 0x316A, 0); break;
		
	//case 0x1F: break; // Platform-Blocker
	case 0x20:
        p_newfoe = new galaxy::CPlatformDrop(map, foe, x, y, 0x316A, 0); break;

	case 0x21:
		// Place Miragia in Episode 4 on the Map
		p_newfoe = new galaxy::CMiragia(map, foe, loc);
		break;

	case 0x22:

        if(totalNumPlayer > mNumLoadedPlayers )
        {
            // Place a gun in case Keen is missing bullets
            for( auto &inventory : mInventoryVec)
            {
                if(inventory.Item.m_bullets < 5)
                {
                    p_newfoe = new galaxy::CSpriteItem(map, foe, x, y, 127, 0);
                    break;
                }
            }
        }
		break;

	case 0x23:
		// Place Miragia in Episode 4 on the Map
        p_newfoe = new galaxy::CDiveSuit(map, foe, x, y, 0);
		break;
		
	//case 0x24: break; // Seem to be an unused keen Sprite. Not sure though
	//case 0x25: break; // Molly?
	//case 0x26: break; // The Father?
	//case 0x27: break; // Unused
	//case 0x28: break; // Unused
	//case 0x29: break; // Unused

	case 0x2A:

        if(totalNumPlayer > mNumLoadedPlayers )
        {
            assert(mNumLoadedPlayers < int(mInventoryVec.size()));
            auto &inventory = mInventoryVec[mNumLoadedPlayers];

            if(inventory.Item.m_lifes >= 0)
            {
                // This is Keen in the diving suit
                inventory.Item.mLevelName = map->getLevelName();
                p_newfoe = new galaxy::CPlayerDive(map, foe, x, y,
                                                   RIGHT, inventory,
                                                   mNumLoadedPlayers,
                                                   inventory.mSpriteVar);
            }
            mNumLoadedPlayers++;
        }
        break;
	
	case 0x2C: if( difficulty < HARD ) break;
	case 0x2B: if( difficulty < NORMAL ) break;
		// This is a Poison Slug.
        p_newfoe = new galaxy::CPoisonSlug(map, foe, x, y);
		break;
	

	case 0x2E:  if( difficulty < HARD ) break;	
	case 0x2D:  if( difficulty < NORMAL ) break;
		// This is Skypest		
		p_newfoe = new galaxy::CSkypest(map, foe, x, y);
		break;
		
		
	// This is the Lick
	case 0x30:  if( difficulty < HARD ) break;	
	case 0x2F:  if( difficulty < NORMAL ) break;
		p_newfoe = new galaxy::CLick(map, foe, x, y);
		break;		
	
	case 0x32: if( difficulty < HARD ) break;
	case 0x31: if( difficulty < NORMAL ) break;
		// This is the Berkeloid
		p_newfoe = new galaxy::CBerkeloid(map, foe, x, y);
		break;
	
	case 0x34: if( difficulty < HARD ) break;
	case 0x33: if( difficulty < NORMAL ) break;	
		// This is a wormmouth difficult mode
        p_newfoe = new galaxy::CWormmouth(map, foe, x, y);
        break;
	
	case 0x46:
		// Watermine horizontal
		p_newfoe = new galaxy::CWaterMine(map, foe, x, y, RIGHT, false);
		break;
		
	case 0x47:
		// Watermine vertical
		p_newfoe = new galaxy::CWaterMine(map, foe, x, y, CENTER, true);
		break;
	
	case 0x48:
		// Watermine horizontal
		p_newfoe = new galaxy::CWaterMine(map, foe, x, y, LEFT, false);
		break;
	
	//case 0x49: break; // Empty
		
	case 0x4A: if( difficulty < HARD ) break;
		// This is an Arachnut.
		p_newfoe = new galaxy::CArachnut(map, foe, x, y);
		break;
		
	//case 0x4B // Shows an ass in the editor, Dunno
	//case 0x4C // Empty		
	
	case 0x4E: if( difficulty < HARD ) break;
	case 0x4D: if( difficulty < NORMAL ) break;	
		p_newfoe = new galaxy::CBlueBird(map, foe, x, y);
		break;
	
	case 0x53 ... 0x56: if( difficulty < HARD ) break; foe -= 0x04; // 0x53 is empty, but could be used maybe...
	case 0x4F ... 0x52: if( difficulty < NORMAL ) break; foe -= 0x1A;
	case 0x35 ... 0x38: 
		// Dart guns
		if(foe == 0x35)
            p_newfoe = new galaxy::AutoShot(map, foe, x, y, CENTER, UP, 256, 0);
		else if(foe == 0x36)
            p_newfoe = new galaxy::AutoShot(map, foe, x, y, RIGHT, CENTER, 260, 0);
		else if(foe == 0x37)
            p_newfoe = new galaxy::AutoShot(map, foe, x, y, CENTER, DOWN, 258, 0);
		else
            p_newfoe = new galaxy::AutoShot(map, foe, x, y, LEFT, CENTER, 262, 0);
		break;

	
	case 0x58: if( difficulty < HARD ) break;
	case 0x57: if( difficulty < NORMAL ) break;	    
		// Dope Fish
        p_newfoe = new galaxy::CDopeFish(map, foe, x, y, 0);
		break;
		
		
	// Not sure if after that there are more sprites to load		
	// This special sprite is only loaded by saved game. Normal gamemaps do not have that flag
	case CFlag::FOE_ID:
        p_newfoe = new galaxy::CFlag(map, loc, loc, 0, false, false);
		break;


	default:
		break;
	}


    return std::shared_ptr<CGalaxySpriteObject>(p_newfoe);
}

}
