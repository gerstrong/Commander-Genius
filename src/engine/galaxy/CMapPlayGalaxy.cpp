/*
 * CMapPlayGalaxy.cpp
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 */

#include "CMapPlayGalaxy.h"
#include "engine/galaxy/ai/CPlayerBase.h"
#include "common/CBehaviorEngine.h"
#include "CMapLoaderGalaxy.h"

CMapPlayGalaxy::CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode) :
mActive(false),
mExeFile(ExeFile),
mInventory(Inventory),
mpOption(g_pBehaviorEngine->m_option),
mCheatmode(Cheatmode)
{}




bool CMapPlayGalaxy::isActive()
{	return mActive;	}




void CMapPlayGalaxy::setActive(bool value)
{
	mActive = value;

	if(mActive)
	{
		mMap.drawAll();
	    // Set Scrollbuffer
	    g_pVideoDriver->updateScrollBuffer(mMap);
	}
}




/**
 * return the name of the opened level
 */
std::string CMapPlayGalaxy::getLevelName()
{
	return mMap.getLevelName();
}







void CMapPlayGalaxy::process(const bool msgboxactive)
{
	// Check if the engine need to be paused
	const bool pause = mInventory.showStatus() || msgboxactive;

	// Animate the tiles of the map
	mMap.m_animation_enabled = !pause;
	mMap.animateAllTiles();

	if(!pause)
	{
		for(size_t i=0 ; i<mObjectPtr.size() ; i++)
		{
			CSpriteObject* p_Object = mObjectPtr[i].get();

			// If the Player is not only dying, but also lost it's existence, meaning he got out of the screen
			// how the death-message or go gameover.
			if( galaxy::CPlayerBase *player = dynamic_cast<galaxy::CPlayerBase*>(p_Object) )
			{
				// Is he really dead?
				if( !player->exists )
					player->processDead();
			}


			if( p_Object->exists && p_Object->calcVisibility() )
			{
				// Process the AI of the object as it's given
				p_Object->process();

				// Check the post-collision stuff, where some
				// sprites are pushed out in case they get stuck
				//p_Object->processPushOutCollision();

				// process all the objects' events
				p_Object->processEvents();

				// Check collision between objects
				for(size_t j=0 ; j<mObjectPtr.size() ; j++)
				{
					CSpriteObject *theOtherObj = mObjectPtr[j].get();

					if( !theOtherObj->exists )
						continue;

					if( theOtherObj != p_Object )
					{
						p_Object->isNearby(*theOtherObj);
						if( p_Object->hitdetect(*theOtherObj) )
							p_Object->getTouchedBy(*theOtherObj);
					}
				}
			}
		}
	}

	g_pVideoDriver->mDrawTasks.add( new BlitScrollSurfaceTask() );

	std::vector< SmartPointer <CGalaxySpriteObject> >::reverse_iterator obj;

	// Draw all the sprites without player
	// The player sprites are drawn as last
	galaxy::CPlayerBase *player = NULL;
	for( obj=mObjectPtr.rbegin() ;
			obj!=mObjectPtr.rend() ; obj++ )
	{

		if( galaxy::CPlayerBase* newplayer = dynamic_cast<galaxy::CPlayerBase*>(obj->get()) )
		{
			player = newplayer;
			continue;
		}

		if((*obj)->honorPriority )
			(*obj)->draw();
	}

	if(player)
	{
		player->draw();
	}

	// Draw masked tiles here!
	mMap.drawForegroundTiles();

	for( obj=mObjectPtr.rbegin() ;
			obj!=mObjectPtr.rend() ; obj++ )
	{
		if(!(*obj)->honorPriority)
			(*obj)->draw();
	}

	if(mpOption[OPT_HUD].value )
		mInventory.drawHUD();


	CEventContainer &EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventSpawnObject *ev =  EventContainer.occurredEvent<EventSpawnObject>() )
	{
		mObjectPtr.push_back( const_cast<CGalaxySpriteObject*>(ev->pObject) );
		EventContainer.pop_Event();
	}

}

// Saves the inventory using the Savegamecontroller.
void CMapPlayGalaxy::operator>>(CSaveGameController &savedGame)
{
	const Uint16 level = mMap.getLevel();
	savedGame.encodeData( level );

	size_t size = mObjectPtr.size();
	// save the number of objects on screen
	savedGame.encodeData(size);

	std::vector< SmartPointer<CGalaxySpriteObject> >::iterator it = mObjectPtr.begin();
	for( ; it != mObjectPtr.end() ; it++ )
	{
		// save all the objects states
		savedGame.encodeData( (*it)->mFoeID );
		savedGame.encodeData( (*it)->getXPosition() );
		savedGame.encodeData( (*it)->getYPosition() );
		savedGame.encodeData( (*it)->dead );
		savedGame.encodeData( (*it)->onscreen );
		savedGame.encodeData( (*it)->hasbeenonscreen );
		savedGame.encodeData( (*it)->exists );
		savedGame.encodeData( (*it)->blockedd );
		savedGame.encodeData( (*it)->blockedu );
		savedGame.encodeData( (*it)->blockedl );
		savedGame.encodeData( (*it)->blockedr );
		savedGame.encodeData( (*it)->mHealthPoints );
		savedGame.encodeData( (*it)->canbezapped );
		savedGame.encodeData( (*it)->cansupportplayer );
		savedGame.encodeData( (*it)->inhibitfall );
		savedGame.encodeData( (*it)->honorPriority );
		savedGame.encodeData( (*it)->sprite );
		savedGame.encodeData( (*it)->m_ActionNumber );
	}

	// Save the map_data as it is left
	savedGame.encodeData(mMap.m_width);
	savedGame.encodeData(mMap.m_height);

	const Uint32 mapSize = mMap.m_width*mMap.m_height*sizeof(word);

	savedGame.addData( reinterpret_cast<byte*>(mMap.getBackgroundData()), mapSize );
	savedGame.addData( reinterpret_cast<byte*>(mMap.getForegroundData()), mapSize );
}



// This is for loading the game
bool CMapPlayGalaxy::operator<<(CSaveGameController &savedGame)
{
	Uint16 level;
	savedGame.decodeData( level );

	// Load the World map level.
	galaxy::CMapLoaderGalaxy MapLoader(mExeFile, mObjectPtr, mInventory, mCheatmode);

	MapLoader.loadMap( mMap, level );

    // Load the Background Music
	g_pMusicPlayer->stop();

    if( !g_pMusicPlayer->load(mExeFile, level) )
    	g_pLogFile->textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    else
    	g_pMusicPlayer->play();


	// load the number of objects on screen
	Uint32 size;
	Uint32 x, y;
	Uint16 foeID;
	uint16_t actionNumber;
	savedGame.decodeData(size);

	// Now load the previously created objects

	if(!mObjectPtr.empty())
		mObjectPtr.clear();

	for( Uint32 i=0 ; i<size ; i++ )
	{
		savedGame.decodeData(foeID);
		savedGame.decodeData(x);
		savedGame.decodeData(y);

		CGalaxySpriteObject *pNewfoe = MapLoader.addFoe(mMap, foeID, x, y);


		// TODO: Be careful here is a bad Null Pointer inside that structure
		if(pNewfoe == NULL)
			pNewfoe = new CGalaxySpriteObject(&mMap, foeID, x, y);

		savedGame.decodeData( pNewfoe->dead );
		savedGame.decodeData( pNewfoe->onscreen );
		savedGame.decodeData( pNewfoe->hasbeenonscreen );
		savedGame.decodeData( pNewfoe->exists );
		savedGame.decodeData( pNewfoe->blockedd );
		savedGame.decodeData( pNewfoe->blockedu );
		savedGame.decodeData( pNewfoe->blockedl );
		savedGame.decodeData( pNewfoe->blockedr );
		savedGame.decodeData( pNewfoe->mHealthPoints );
		savedGame.decodeData( pNewfoe->canbezapped );
		savedGame.decodeData( pNewfoe->cansupportplayer );
		savedGame.decodeData( pNewfoe->inhibitfall );
		savedGame.decodeData( pNewfoe->honorPriority );
		savedGame.decodeData( pNewfoe->sprite );
		savedGame.decodeData( actionNumber );
		pNewfoe->setActionForce(actionNumber);

		if(pNewfoe->exists)
			mObjectPtr.push_back(pNewfoe);
	}

	// Save the map_data as it is left
	savedGame.decodeData(mMap.m_width);
	savedGame.decodeData(mMap.m_height);
	//mMap.resetScrolls();

	savedGame.readDataBlock( reinterpret_cast<byte*>(mMap.getBackgroundData()) );
	savedGame.readDataBlock( reinterpret_cast<byte*>(mMap.getForegroundData()) );

	if( mMap.m_width * mMap.m_height > 0 )
	{
		mMap.drawAll();
	}

	return true;
}
