/*
 * CMapLoaderGalaxyEp5.cpp
 *
 *  Created on: 29.07.2012
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxyEp5.h"

// Episode 5
#include "engine/galaxy/common/ai/CPlayerWM.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"

// TODO: I'm not sure yet if those are really common platforms
#include "engine/galaxy/common/ai/platforms.h"
#include "engine/galaxy/common/ai/CFlag.h"
#include "engine/galaxy/common/ai/CSpriteItem.h"

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
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 127);
	}	
	
	// This is the keycard. Keen 5 only!
	if( foe == 0x46 )
	{
		p_newfoe = new galaxy::CSpriteItem(&Map, foe, x, y, 105);
	}	
	
	// If a foe was found, just return.
	if( p_newfoe )
		return p_newfoe;


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


	case 0x1B:
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B7C );
			break;
			
	//case 0x20:	
			//p_newfoe = new galaxy::CPlatformDrop( &Map, foe, x, y, 0x1B5E); break;

        // Var Plats other color
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
	
	// Var Plats
	case 0x50: 
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, UP, 0x1B7C); break;
	case 0x51: 
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, RIGHT, CENTER, 0x1B7C); break;
	case 0x52: 
	  		p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, CENTER, DOWN, 0x1B7C); break;
	case 0x53:
			p_newfoe = new galaxy::CVarPlatform( &Map, foe, x, y, LEFT, CENTER, 0x1B7C); break;

	case 0x54:			
			p_newfoe = new galaxy::CPlatformVertical( &Map, foe, x, y, UP, 0x1B7C ); break;
	case 0x55:			
			p_newfoe = new galaxy::CPlatformHorizontal( &Map, foe, RIGHT, x, y ); break;

			break;



	/*case PLATFORM_VERT: case PLATFORM_VERT_ALT:
		p_newfoe = new galaxy::CPlatformVertical(&Map, foe, x, y); break;
	case PLATFORM_HORIZ_ALT:
	case PLATFORM_HORIZ:
		p_newfoe = new galaxy::CPlatformHorizontal(&Map, foe, x, y); break;
	case PLATFORM_DROP:
		p_newfoe = new galaxy::CPlatformDrop(&Map, foe, x, y); break;*/


	default:
		break;
	}


	return p_newfoe;
}

};
