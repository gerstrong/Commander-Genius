/*
 * CMapLoaderGalaxyEp4.cpp
 *
 *  Created on: 28.07.2012
 *      Author: gerstrong
 */

#include "CMapLoaderGalaxyEp4.h"

// Episode 4
#include "engine/galaxy/ai/CPlayerWM.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/ep4/CPlayerDive.h"
#include "engine/galaxy/ai/ep4/CPoisonSlug.h"
#include "engine/galaxy/ai/ep4/CMadMushroom.h"
#include "engine/galaxy/ai/ep4/CCouncilMember.h"
#include "engine/galaxy/ai/ep4/CMiragia.h"
#include "engine/galaxy/ai/ep4/CDiveSuit.h"
#include "engine/galaxy/ai/ep4/CArachnut.h"
#include "engine/galaxy/ai/ep4/CBounder.h"
#include "engine/galaxy/ai/ep4/CDopeFish.h"
#include "engine/galaxy/ai/ep4/CWaterMine.h"
#include "engine/galaxy/ai/ep4/CDevilSprite.h"
#include "engine/galaxy/ai/ep4/CSchoolFish.h"
#include "engine/galaxy/ai/ep4/CLick.h"
#include "engine/galaxy/ai/ep4/CLindsey.h"
#include "engine/galaxy/ai/ep4/CWormmouth.h"
#include "engine/galaxy/ai/ep4/CSkypest.h"
#include "engine/galaxy/ai/ep4/CMimrock.h"
#include "engine/galaxy/ai/ep4/CEgg.h"
#include "engine/galaxy/ai/ep4/CBerkeloid.h"
#include "engine/galaxy/ai/ep4/CBlueBird.h"
#include "engine/galaxy/ai/ep4/CThunderCloud.h"
#include "engine/galaxy/ai/ep4/CTreasureEater.h"
#include "engine/galaxy/ai/ep4/CInchWorm.h"
#include "engine/galaxy/ai/ep4/CFoot.h"

// TODO: I'm note sure yet, if those are really common platforms
#include "engine/galaxy/ai/platforms.h"
#include "engine/galaxy/ai/CFlag.h"

namespace galaxy
{


CMapLoaderGalaxyEp4::CMapLoaderGalaxyEp4(CExeFile &ExeFile,
		std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtr,
		CInventory &Inventory, stCheat &Cheatmode) :
CMapLoaderGalaxy( ExeFile, ObjectPtr, Inventory, Cheatmode)
{}


// Platforms:
// 27-30 & 32 seem to be the actual platforms.  There may be more of numbers lower than 27.
// 31 seem to be the stoppers, those are not created because the platform
// get this from the object map directly

const unsigned int PLATFORM_VERT_ALT = 27;
const unsigned int PLATFORM_HORIZ_ALT = 28;
const unsigned int PLATFORM_VERT = 29;
const unsigned int PLATFORM_HORIZ = 30;
// 31 does not count, because it's a blocker.
const unsigned int PLATFORM_DROP = 32;

const unsigned int DIVE_SUIT = 35;
/**
 * @brief	Loads a foe given by the coordiantes
 */
CGalaxySpriteObject* CMapLoaderGalaxyEp4::addFoe(CMap &Map, word foe, size_t x, size_t y)
{

	CGalaxySpriteObject* commonfoe = CMapLoaderGalaxy::addFoe(Map, foe, x, y);


	// If a foe was found, that is common in all the galaxy games, just return.
	if( commonfoe )
		return commonfoe;

	// otherwise look for special foe.
	VectorD2<Uint32> loc(x,y);

	CGalaxySpriteObject* p_newfoe = NULL;

	switch(foe)
	{
	case 1:
	case 2:
		// This is the player on the map in one level
		p_newfoe = new galaxy::CPlayerLevel(&Map, foe, x, y, m_ObjectPtr,
				(foe==1) ? RIGHT : LEFT, m_Inventory, m_Cheatmode, 0x98C);
		break;

	case 3:
		// This is the player on the world map
		// Add the Camera into the game scene and attach it to this player
		p_newfoe = new galaxy::CPlayerWM(&Map, foe, x, y,
										m_ObjectPtr, m_Inventory, m_Cheatmode, 0x15C2);

		break;

	case 4:
		//This is a council member.
		p_newfoe = new galaxy::CCouncilMember(&Map, foe, x, y-750);
		break;

	case 6:
		//This is pincess Lindsey.
		p_newfoe = new galaxy::CLindsey(&Map, foe, x, y-750);
		break;

	case 7:
		// This is a wormmouth.
		p_newfoe = new galaxy::CWormmouth(&Map, foe, x, y);
		break;

	case 8:
		// This is Skypest.
		p_newfoe = new galaxy::CSkypest(&Map, foe, x, y);
		break;


	case 9:
		//This is the Thunder Cloud
		p_newfoe = new galaxy::CThunderCloud(&Map, foe, x, y);
		break;

	case 11:
		// This is an inchworm.
		p_newfoe = new galaxy::CInchWorm(&Map, foe, x, y);
		break;

	case 12:
		// This is a Bounder.
		// TODO: Those relative coordinates are not a good sign. Try to remove them and make the Sprite substract them
		p_newfoe = new galaxy::CBounder(&Map, foe, x, y-250);
		break;

	case 13:
		// This is an egg
		// TODO: Those relative coordinates are not a good sign. Try to remove them and make the Sprite substract them
		if( g_pBehaviorEngine->mDifficulty > 1 )
			p_newfoe = new galaxy::CBlueBird(&Map, foe, x, y-(2<<CSF));
		else
			p_newfoe = new galaxy::CEgg(&Map, foe, x, y-(2<<CSF));
		break;

	case 18:
		// This might be the treasure eater.
		p_newfoe = new galaxy::CTreasureEater(&Map, foe, x, y);
		break;

	case 19:
		//This is a Mimrock.
		// TODO: Those relative coordinates are not a good sign. Try to remove them and make the Sprite substract them
		p_newfoe = new galaxy::CMimrock(&Map, foe, x, y);
		break;

	case 20:
		// This is an Arachnut.
		p_newfoe = new galaxy::CArachnut(&Map, foe, x, y);
		break;


	case 21:
		// This is a Mad Mushroom.
		p_newfoe = new galaxy::CMadMushroom(&Map, foe, x, y);
		break;


	case 22:
		// This is a Poison Slug.
		p_newfoe = new galaxy::CPoisonSlug(&Map, foe, x, y-250, m_ObjectPtr);
		break;

	case 23:
		// This is a Sprite from the well of wishes.
		p_newfoe = new galaxy::CDevilSprite(&Map, foe, x, y);
		break;

	case 24:
		// This is a Sprite from the well of wishes.
		p_newfoe = new galaxy::CSchoolFish(&Map, foe, x, y);
		break;

	/*case 25:
		// This is Skypest.
		p_newfoe = new galaxy::CSkypest(&Map, foe, x, y);
		break;*/


	case PLATFORM_VERT: case PLATFORM_VERT_ALT:
		p_newfoe = new galaxy::CPlatformVertical(&Map, foe, x, y, UP, 0x316A); break;
	case PLATFORM_HORIZ_ALT:
	case PLATFORM_HORIZ:
		p_newfoe = new galaxy::CPlatformHorizontal(&Map, foe, x, y); break;
	case PLATFORM_DROP:
		p_newfoe = new galaxy::CPlatformDrop(&Map, foe, x, y); break;

	case 33:
		// Place Miragia in Episode 4 on the Map
		p_newfoe = new galaxy::CMiragia(&Map, foe, loc);
		break;

	case DIVE_SUIT:
		// Place Miragia in Episode 4 on the Map
		p_newfoe = new galaxy::CDiveSuit(&Map, foe, x, y);
		break;

	case 42:
		// This is Keen in the swimming suit
		p_newfoe = new galaxy::CPlayerDive(&Map, foe, x, y, m_ObjectPtr,
						RIGHT, m_Inventory, m_Cheatmode);
		break;

	case 46:
		// This is the Lick
		if( g_pBehaviorEngine->mDifficulty >= HARD )
			p_newfoe = new galaxy::CSkypest(&Map, foe, x, y);
		break;

	case 47:
		// This is the Lick
		if( g_pBehaviorEngine->mDifficulty >= NORMAL )
			p_newfoe = new galaxy::CLick(&Map, foe, x, y);
		break;

	case 48:
		// This is the Lick
		if( g_pBehaviorEngine->mDifficulty >= HARD )
			p_newfoe = new galaxy::CLick(&Map, foe, x, y);
		break;


	case 50:
		// This is the Berkeloid
		p_newfoe = new galaxy::CBerkeloid(&Map, foe, x, y);
		break;

	case 67:
		// This is the Foot
		Map.isSecret = true;
		p_newfoe = new galaxy::CFoot(&Map, foe, 0x2EF4, x, y);
		break;

	case 71:
		// Watermine vertical
		p_newfoe = new galaxy::CWaterMine(&Map, foe, x, y, true);
		break;
	case 72:
		// Watermine horizontal
		p_newfoe = new galaxy::CWaterMine(&Map, foe, x, y, false);
		break;

	case 87:
		// Dope Fish
		p_newfoe = new galaxy::CDopeFish(&Map, foe, x, y);
		break;

	case CFlag::FOE_ID:
		p_newfoe = new galaxy::CFlag(&Map, loc, loc);
		break;


	default:
		break;
	}


	return p_newfoe;
}

};
