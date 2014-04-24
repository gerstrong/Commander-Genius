/*
 * CMapLoaderGalaxyEp5.cpp
 *
 *  Created on: 29.07.2012
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxyEp5.h"
#include "ai/CSparky.h"
#include "ai/CAmpton.h"
#include "ai/CRoboRed.h"
#include "ai/CVolteface.h"
#include "ai/CKorath.h"
#include "ai/CSlicestar.h"
#include "ai/CSphereful.h"
#include "ai/CShikadi.h"
#include "ai/CSpirogrip.h"
#include "ai/CFuse.h"
#include "ai/CShockshound.h"
#include "ai/CShikadiMine.h"
#include "ai/CSpindred.h"
#include "ai/CShelly.h"
#include "ai/CShikadiMaster.h"

// Episode 5
#include "../common/ai/CPlayerWM.h"
#include "../common/ai/CPlayerLevel.h"

// TODO: I'm not sure yet if those are really common platforms
#include "../common/ai/platforms.h"
#include "../common/ai/CFlag.h"
#include "../common/ai/CSpriteItem.h"
#include "../common/ai/platform/CPlatformSit.h"
#include "../common/ai/Autogun.h"

namespace galaxy
{


CMapLoaderGalaxyEp5::CMapLoaderGalaxyEp5(CExeFile &ExeFile,
		std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
        std::vector<CInventory> &inventoryVec, stCheat &Cheatmode) :
CMapLoaderGalaxy( ExeFile, ObjectPtr, inventoryVec, Cheatmode)
{}


// Platforms:
// 27-30 & 32 seem to be the actual platforms.  There may be more of numbers lower than 27.
// 31 seem to be the stoppers, those are not created because the platform
// get this from the object map directly

const unsigned int PLATFORM_HORIZ_ALT = 28;
const unsigned int PLATFORM_VERT = 29;
const unsigned int PLATFORM_HORIZ = 30;
// 31 does not count, because it's a blocker.
const unsigned int PLATFORM_DROP = 32;

const unsigned int DIVE_SUIT = 35;

bool CMapLoaderGalaxyEp5::isKeenPlayer(const int foeID)
{
    return (foeID > 0 && foeID <= 3);
}


/**
 * @brief	Loads a foe given by the coordiantes
 */
CGalaxySpriteObject* CMapLoaderGalaxyEp5::addFoe(CMap &Map, word foe, size_t x, size_t y)
{
	CGalaxySpriteObject* p_newfoe = nullptr;
	
	// Gems
	for( Uint32 i=0x39 ; i<=0x3C ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 122+2*(i-0x39);
            p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, newsprite,0);
		}
	}
	
	// Point Item Sprites (Candies, etc...)
	for( Uint32 i=0x3D ; i<=0x43 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 108+2*(i-0x3D);
            p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, newsprite,0);
		}
	}

	// Neuronal-stunner
	if( foe == 0x44 )
	{
        p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 131,0);
	}
	
	if( foe == 0x45 )
	{
        for( auto &inventory : mInventoryVec)
        {
            if(inventory.Item.m_bullets < 5)
            {
                p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 131,0);
                break;
            }
        }
	}
	
	
	// This is the keycard. Keen 5 only!
	if( foe == 0x46 )
	{
        p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 105,0);
	}	
	
	// If a foe was found, just return.
	if( p_newfoe )
		return p_newfoe;
	
	const Difficulty difficulty = g_pBehaviorEngine->mDifficulty;

	// otherwise look for special foe.
	Vector2D<Uint32> loc(x,y);	
    auto &inventory = mInventoryVec[mNumLoadedPlayers];

	switch(foe)
	{
	case 0x01:
	case 0x02:

        if(inventory.Item.m_lifes >= 0)
        {

			// This is the player on the map in one level
            inventory.Item.mLevelName = Map.getLevelName();

            p_newfoe = new galaxy::CPlayerLevel(&Map, foe, x, y, m_ObjectPtr,
               (foe==1) ? RIGHT : LEFT, inventory, m_Cheatmode, 0x0888, mNumLoadedPlayers);
        }
        mNumLoadedPlayers++;
        break;

	case 0x03:

        if(inventory.Item.m_lifes >= 0)
        {
			// This is the player on the world map
			// Add the Camera into the game scene and attach it to this player
            inventory.Item.mLevelName = Map.getLevelName();
            p_newfoe = new galaxy::CPlayerWM(&Map,foe, x, y, inventory, m_Cheatmode, 0x146E, mNumLoadedPlayers );
			// 0x137A
        }
        mNumLoadedPlayers++;
        break;

			
	case 0x06: if( difficulty < HARD ) break;
	case 0x05: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x04: 
		// This is Sparky
		p_newfoe = new galaxy::CSparky(&Map, foe, x, y);
		break;

	case 0x09: if( difficulty < HARD ) break;
	case 0x08: if( difficulty < NORMAL ) break;
	case 0x07: 	
		// This is the Mine
		p_newfoe = new galaxy::CShikadiMine(&Map, foe, x, y);
		break;


	
	case 0x0C: if ( difficulty < HARD ) break;
	case 0x0B: if ( difficulty < NORMAL ) break;
	case 0x0A: 
	      p_newfoe = new galaxy::CSlicestar(&Map, foe, x, y, true, CENTER, UP);
	      break;
			
	case 0x0F: if ( difficulty < HARD ) break;
	case 0X0E: if ( difficulty < NORMAL ) break;
	case 0x0D: 
  		// This is Robored
		p_newfoe = new galaxy::CRoboRed(&Map, foe, x, y);
		break;

	case 0x12: if ( difficulty < HARD ) break;
	case 0x11: if ( difficulty < NORMAL ) break;
	case 0x10: 
		p_newfoe = new galaxy::CSpirogrip(&Map, foe, x, y);
		break;
	

	case 0x15: if ( difficulty < HARD ) break;
	case 0x14: if ( difficulty < NORMAL ) break;
	case 0x13: 
		p_newfoe = new galaxy::CSlicestar(&Map, foe, x, y, false, RIGHT, UP);
		break;

	case 0x18: if ( difficulty < HARD ) break;
	case 0x17: if ( difficulty < NORMAL ) break;
	case 0x16: 
		p_newfoe = new galaxy::CSlicestar(&Map, foe, x, y, true, RIGHT, CENTER);
		break;
		
				
	case 0x1B: 
            p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B5E, 0 );
			break;
	case 0x1C: 
            p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, RIGHT, x, y, 0x1B5E,0 );
			break;
	case 0x1D: 
            p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, DOWN, 0x1B5E,0 );
			break;
	case 0x1E: 
            p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, LEFT, x, y, 0x1B5E,0 );
			break;
									
	case 0x20:	
            p_newfoe = new galaxy::CPlatformDrop( &Map, foe, x, y, 0x1BD6, 0); break;
		

	case 0x21: if (difficulty >= NORMAL) break;
	case 0x22: if (difficulty >= HARD) break;
	case 0x23: 
            p_newfoe = new galaxy::CPlatformSit( &Map, foe, x, y, 0x1BD6,0 ); break;

			
        // Var Plats red color
	case 0x24:
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, UP, 0x1B5E,0 ); break;
	case 0x25:
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, RIGHT, CENTER, 0x1B5E,0 );	break;
	case 0x26:
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, DOWN, 0x1B5E,0 );	break;
	case 0x27:
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, LEFT, CENTER,0x1B5E, 0);	break;
			
			
	case 0x28:
            p_newfoe = new galaxy::CPlatformMoveAway( &Map, foe, x, y, CENTER, LEFT, 0x1B7C, 0);
			break;
						
	case 0x29:
	  /*if (current_level == 12) {
		Fuse1 = 4;
		QEDSpawn(tx, ty);
	  } else {
		Fuse1++;
	  }*/
            p_newfoe = new galaxy::CFuse( &Map, foe, x, y, 0);
			break;


	case 0x35: if ( difficulty < HARD ) break;
	case 0x31: if ( difficulty < NORMAL ) break;
	case 0x2D: 
        p_newfoe = new galaxy::AutoShot(&Map, foe, x, y, CENTER, UP, 134, 0);
		break;


	case 0x36: if ( difficulty < HARD ) break;
	case 0x32: if ( difficulty < NORMAL ) break;
	case 0x2E: 
        p_newfoe = new galaxy::AutoShot(&Map, foe, x, y, RIGHT, CENTER, 134, 0);
		break;


	case 0x37: if ( difficulty < HARD ) break;
	case 0x33: if ( difficulty < NORMAL ) break;
	case 0x2F: 
        p_newfoe = new galaxy::AutoShot(&Map, foe, x, y, CENTER, DOWN, 134, 0);
		break;


	case 0x38: if ( difficulty < HARD ) break;
	case 0x34: if ( difficulty < NORMAL ) break;
	case 0x30: 
        p_newfoe = new galaxy::AutoShot(&Map, foe, x, y, LEFT, CENTER, 134, 0);
		break;

	
	case 0x49: if ( difficulty < HARD ) break;
	case 0x48: if ( difficulty < NORMAL ) break;
	case 0x47: 
  		// This is Voltface
		p_newfoe = new galaxy::CVolteface(&Map, foe, x, y);
		break;
	

	case 0x4C: if ( difficulty < HARD ) break;
	case 0x4B: if ( difficulty < NORMAL ) break;
	case 0x4A: 
  		// This is Shelly
		p_newfoe = new galaxy::CShelly(&Map, foe, x, y);
		break;
	
	
	case 0x4F: if ( difficulty < HARD ) break;
	case 0x4E: if ( difficulty < NORMAL ) break;
	case 0x4D: 
	  	// This is Spindred
		p_newfoe = new galaxy::CSpindred(&Map, foe, x, y);
		break;	
	

	case 0x5A: if ( difficulty < HARD ) break;
	case 0x59: if ( difficulty < NORMAL ) break;
	case 0x58: 
		// This is the Shikadi Master
		p_newfoe = new galaxy::CShikadiMaster( &Map, foe, x, y );
		break;	

	case 0x65: if ( difficulty < HARD ) break;
	case 0x64: if ( difficulty < NORMAL ) break;
	case 0x63: 	
	  	// This is Shikadi
		p_newfoe = new galaxy::CShikadi(&Map, foe, x, y);
		break;

	case 0x68: if ( difficulty < HARD ) break;
	case 0x67: if ( difficulty < NORMAL ) break;
	case 0x66: 
		// This is Shockshound
		p_newfoe = new galaxy::CShockshound(&Map, foe, x, y);
		break;
	
	case 0x6B: if ( difficulty < HARD ) break;
	case 0x6A: if ( difficulty < NORMAL ) break;
	case 0x69: 
			p_newfoe = new galaxy::CSphereful( &Map, foe, x, y );
			break;		

	case 0x7C: 
			p_newfoe = new galaxy::CKorath( &Map, foe, x, y );
			break;
	
	/*

	case 125: TeleInLevel = 1; break; 
 */		
			
	case 0x2C: if ( difficulty < HARD ) break;
	case 0x2B: if ( difficulty < NORMAL ) break;
	case 0x2A: 
			p_newfoe = new galaxy::CAmpton( &Map, foe, x, y );
			break;
			
			
	// All the Purple Plats
	// Plat with trails
	case 0x50: 
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, UP, 0x1B7C, 0); break;
	case 0x51: 
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, RIGHT, CENTER, 0x1B7C, 0); break;
	case 0x52: 
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, DOWN, 0x1B7C, 0); break;
	case 0x53:
            p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, LEFT, CENTER, 0x1B7C, 0); break;

	// Without trails
	case 0x54:			
            p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B7C, 0 ); break;
	case 0x55:			
            p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, RIGHT, x, y, 0x1B7C, 0 ); break;
	case 0x56:			
            p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, DOWN, 0x1B7C, 0 ); break;
	case 0x57:			
            p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, LEFT, x, y, 0x1B7C, 0 ); break;

			break;

	// Not sure if after that there are more sprites to load		
	// This special sprite is only loaded by saved game. Normal gamemaps do not have that flag
	case CFlag::FOE_ID:
        p_newfoe = new galaxy::CFlag(&Map, loc, loc, false, 0, false);
		break;
			
	default:
		break;
	}


	return p_newfoe;
}

}
