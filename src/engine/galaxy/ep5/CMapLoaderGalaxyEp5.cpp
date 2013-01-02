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

// Episode 5
#include "engine/galaxy/common/ai/CPlayerWM.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"

// TODO: I'm not sure yet if those are really common platforms
#include "engine/galaxy/common/ai/platforms.h"
#include "engine/galaxy/common/ai/CFlag.h"
#include "engine/galaxy/common/ai/CSpriteItem.h"
#include <engine/galaxy/common/ai/platform/CPlatformSit.h>

namespace galaxy
{


CMapLoaderGalaxyEp5::CMapLoaderGalaxyEp5(CExeFile &ExeFile,
		std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtr,
		CInventory &Inventory, stCheat &Cheatmode) :
CMapLoaderGalaxy( ExeFile, ObjectPtr, Inventory, Cheatmode)
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
			p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, newsprite);
		}
	}
	
	// Point Item Sprites (Candies, etc...)
	for( Uint32 i=0x3D ; i<=0x43 ; i++ )
	{
		if( foe == i )
		{
			const Uint32 newsprite = 108+2*(i-0x3D);
			p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, newsprite);
		}
	}

	// Neuronal-stunner
	if( foe == 0x44 )
	{
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 131);
	}
	
	if( foe == 0x45 )
	{
	  if(m_Inventory.Item.m_bullets < 5)
	  {
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 131);
	  }
	}
	
	
	// This is the keycard. Keen 5 only!
	if( foe == 0x46 )
	{
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 105);
	}	
	
	// If a foe was found, just return.
	if( p_newfoe )
		return p_newfoe;
	
	const Difficulty difficulty = g_pBehaviorEngine->mDifficulty;

	// otherwise look for special foe.
	VectorD2<Uint32> loc(x,y);	

	switch(foe)
	{
	case 0x01:
	case 0x02:
			// This is the player on the map in one level
			p_newfoe = new galaxy::CPlayerLevel(&Map, foe, x, y, m_ObjectPtr,
					(foe==1) ? RIGHT : LEFT, m_Inventory, m_Cheatmode, 0x0888);
			break;

	case 0x03:
			// This is the player on the world map
			// Add the Camera into the game scene and attach it to this player
			p_newfoe = new galaxy::CPlayerWM(&Map,foe, x, y, m_Inventory, m_Cheatmode, 0x146E );
			// 0x137A
			break;

			
	case 0x06: if( difficulty < HARD ) break;
	case 0x05: if( difficulty < NORMAL ) break;	// not sure here    
	case 0x04: 
		// This is Sparky
		p_newfoe = new galaxy::CSparky(&Map, foe, x, y);
		break;

	/*case 0x09: if (difficulty_level < 3) break;
	case 0x08: if (difficulty_level < 2) break;
	case 0x07: MineSpawn(tx, ty); MineInLevel = 1; break;*/

	
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

	/*case 0x12: if (difficulty_level < 3) break;
	case 0x11: if (difficulty_level < 2) break;
	case 0x10: SpiroSpawn(tx, ty); SpiroInLevel = 1; break;
*/
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
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B5E );
			break;
	case 0x1C: 
			p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, RIGHT, x, y, 0x1B5E );
			break;
	case 0x1D: 
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, DOWN, 0x1B5E );
			break;
	case 0x1E: 
			p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, LEFT, x, y, 0x1B5E );
			break;
									
	case 0x20:	
			p_newfoe = new galaxy::CPlatformDrop( &Map, foe, x, y, 0x1BD6); break;
		

	case 0x21: if (difficulty >= NORMAL) break;
	case 0x22: if (difficulty >= HARD) break;
	case 0x23: 
			p_newfoe = new galaxy::CPlatformSit( &Map, foe, x, y, 0x1BD6); break;		

			
        // Var Plats red color
	case 0x24:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, UP, 0x1B5E); break;
	case 0x25:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, RIGHT, CENTER, 0x1B5E);	break;
	case 0x26:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, DOWN, 0x1B5E);	break;
	case 0x27:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, LEFT, CENTER,0x1B5E);	break;
			
			
	case 0x28:
			p_newfoe = new galaxy::CPlatformMoveAway( &Map, foe, x, y, CENTER, LEFT, 0x1B7C);
			break;
						
	/*case 0x29:
	  if (current_level == 12) {
		Fuse1 = 4;
		QEDSpawn(tx, ty);
	  } else {
		Fuse1++;
	  }
	
	  FuseInLevel = 1;
	break;


	case 53: if (difficulty_level < 3) break;
	case 49: if (difficulty_level < 2) break;
	case 45: AutoGunSpawn(tx, ty, 0); AutoGunInLevel = 1; break;

	case 54: if (difficulty_level < 3) break;
	case 50: if (difficulty_level < 2) break;
	case 46: AutoGunSpawn(tx, ty, 1); AutoGunInLevel = 1; break;

	case 55: if (difficulty_level < 3) break;
	case 51: if (difficulty_level < 2) break;
	case 47: AutoGunSpawn(tx, ty, 2); AutoGunInLevel = 1; break;

	case 56: if (difficulty_level < 3) break;
	case 52: if (difficulty_level < 2) break;
	case 48: AutoGunSpawn(tx, ty, 3); AutoGunInLevel = 1; break;*/

	case 0x49: if ( difficulty < HARD ) break;
	case 0x48: if ( difficulty < NORMAL ) break;
	case 0x47: 
  		// This is Voltface
		p_newfoe = new galaxy::CVolteface(&Map, foe, x, y);
		break;
	

	/*case 76: if (difficulty_level < 3) break;
	case 75: if (difficulty_level < 2) break;
	case 74: ShellySpawn(tx, ty); ShellyInLevel = 1; break;

	case 79: if (difficulty_level < 3) break;
	case 78: if (difficulty_level < 2) break;
	case 77: SpindredSpawn(tx, ty); SpindredInLevel = 1; break;

	case 88: if (difficulty_level < 3) break;
	case 89: if (difficulty_level < 2) break;
	case 90: MasterSpawn(tx, ty); MasterInLevel = 1; break;

	case 101: if (difficulty_level < 3) break;
	case 100: if (difficulty_level < 2) break;
	case 99: ShikadiSpawn(tx, ty); ShikadiInLevel = 1; break;

	case 104: if (difficulty_level < 3) break;
	case 103: if (difficulty_level < 2) break;
	case 102: ShocksundSpawn(tx, ty); ShocksundInLevel = 1; break;
*/
	
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
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, UP, 0x1B7C); break;
	case 0x51: 
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, RIGHT, CENTER, 0x1B7C); break;
	case 0x52: 
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, DOWN, 0x1B7C); break;
	case 0x53:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, LEFT, CENTER, 0x1B7C); break;

	// Without trails
	case 0x54:			
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B7C ); break;
	case 0x55:			
			p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, RIGHT, x, y, 0x1B7C ); break;
	case 0x56:			
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, DOWN, 0x1B7C ); break;
	case 0x57:			
			p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, LEFT, x, y, 0x1B7C ); break;

			break;


	default:
		break;
	}


	return p_newfoe;
}

};
