/*
 * CMapPlayGalaxy.cpp
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 */

#include "CMapPlayGalaxy.h"
#include "engine/galaxy/ai/CPlayerBase.h"
#include "common/CBehaviorEngine.h"

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
	size_t size = mObjectPtr.size();
	// save the number of objects on screen
	savedGame.encodeData(size);

	std::vector< SmartPointer<CGalaxySpriteObject> >::iterator it = mObjectPtr.begin();
	for( ; it != mObjectPtr.end() ; it++ )
	{
		// save all the objects states
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

	savedGame.encodeData( mActive );

	// Save the map_data as it is left
	savedGame.encodeData(mMap.m_width);
	savedGame.encodeData(mMap.m_height);
	savedGame.addData( reinterpret_cast<byte*>(mMap.getBackgroundData()),
													mMap.m_width*mMap.m_height*sizeof(word) );
	savedGame.addData( reinterpret_cast<byte*>(mMap.getForegroundData()),
													mMap.m_width*mMap.m_height*sizeof(word) );
}

// This is for loading the game
bool CMapPlayGalaxy::operator<<(CSaveGameController &savedGame)
{
	/*CSaveGameController &savedGame = *(gpSaveGameController);

	// This fills the datablock from CSavedGame object
	if(savedGame.load())
	{
		// Create the special merge effect (Fadeout)
		CColorMerge *pColorMergeFX = new CColorMerge(8);

		// Prepare for loading the new level map and the players.
		cleanup();

		// get the episode, level and difficulty
		char newLevel;
		savedGame.decodeData(m_Episode);
		savedGame.decodeData(newLevel);

		bool loadmusic = ( m_Level != newLevel || m_Level == 80 );
		m_Level = newLevel;

		savedGame.decodeData(g_pBehaviorEngine->mDifficulty);

		bool dark, checkpointset;
		int checkx, checky;
		savedGame.decodeData(checkpointset);
		savedGame.decodeData(checkx);
		savedGame.decodeData(checky);
		savedGame.decodeData(dark);

		// Load number of Players
		savedGame.decodeData(m_NumPlayers);

		if(!m_Player.empty())
			m_Player.clear();

		m_Player.assign(m_NumPlayers, CPlayer(m_Episode, m_Level,
				 mp_level_completed, m_Object, *mMap.get() ) );
		for( size_t i=0 ; i < m_Player.size() ; i++ )
		{
			m_Player.at(i).m_index = i;
			m_Player.at(i).setDatatoZero();
		}

		CVorticonMapLoader Maploader(mMap, &m_Player);
		Maploader.mp_objvect = &m_Object;
		m_checkpointset = checkpointset;
		Maploader.m_checkpointset = m_checkpointset;
		if(!Maploader.load(m_Episode, m_Level, m_Gamepath, loadmusic, false))
			return false;

		m_checkpoint_x = checkx;
		m_checkpoint_y = checky;

		m_level_command = START_LEVEL;

		std::vector<CPlayer> :: iterator player;
		for( player=m_Player.begin() ; player != m_Player.end() ; player++ ) {
			int x, y;
			player->setupforLevelPlay();
			savedGame.decodeData(x);
			savedGame.decodeData(y);
			player->moveToForce(VectorD2<int>(x,y));
			savedGame.decodeData(player->blockedd);
			savedGame.decodeData(player->blockedu);
			savedGame.decodeData(player->blockedl);
			savedGame.decodeData(player->blockedr);
			savedGame.decodeData(player->inventory);
			player->pdie = 0;
		}

		// load the number of objects on screen
		Uint32 size;
		savedGame.decodeData(size);
		for( Uint32 i=0 ; i<size  ; i++ )
		{
			unsigned int x,y;

			if(i >= m_Object.size())
			{
				CVorticonSpriteObject *object = new CVorticonSpriteObject( mMap.get(), 0, 0, OBJ_NONE);
				object->exists = false;
				m_Object.push_back(object);
			}

			CVorticonSpriteObject* object = dynamic_cast<CVorticonSpriteObject*>(m_Object.at(i));

			savedGame.decodeData(object->m_type);
			savedGame.decodeData(x);
			savedGame.decodeData(y);
			object->moveToForce(VectorD2<int>(x,y));
			savedGame.decodeData(object->dead);
			savedGame.decodeData(object->onscreen);
			savedGame.decodeData(object->hasbeenonscreen);
			savedGame.decodeData(object->exists);
			savedGame.decodeData(object->blockedd);
			savedGame.decodeData(object->blockedu);
			savedGame.decodeData(object->blockedl);
			savedGame.decodeData(object->blockedr);
			savedGame.decodeData(object->mHealthPoints);
			savedGame.decodeData(object->canbezapped);
			savedGame.decodeData(object->cansupportplayer);
			savedGame.decodeData(object->inhibitfall);
			savedGame.decodeData(object->honorPriority);
			savedGame.decodeData(object->sprite);
			object->performCollisions();

			if(object->m_type == OBJ_DOOR or
				object->m_type == OBJ_RAY or
				object->m_type == OBJ_SNDWAVE or
				object->m_type == OBJ_FIREBALL or
				object->m_type == OBJ_ICECHUNK or
				object->m_type == OBJ_ICEBIT or
				object->m_type == OBJ_GOTPOINTS or
				object->m_type == OBJ_ANKHSHIELD) // Some objects are really not needed. So don't load them
				object->exists = false;
		}

		// TODO: An algorithm for comparing the number of players saved and we actually have need to be in sync

		// Load the map_data as it was left last
		savedGame.decodeData(mMap->m_width);
		savedGame.decodeData(mMap->m_height);
		savedGame.readDataBlock( reinterpret_cast<byte*>(mMap->getForegroundData()) );

		// Load completed levels
		savedGame.readDataBlock( (byte*)(mp_level_completed) );

		m_Player[0].setMapData(mMap.get());
		m_Player[0].setupCameraObject();
		m_Player[0].mp_camera->attachObject(&m_Player[0]);

		while(m_Player[0].mp_camera->m_moving)
		{
			m_Player[0].mp_camera->process();
			m_Player[0].mp_camera->processEvents();
		}

		mMap->drawAll();

		// Create the special merge effect (Fadeout)
		g_pGfxEngine->setupEffect(pColorMergeFX);


		mpObjectAI = new CVorticonSpriteObjectAI(mMap.get(), m_Object, m_Player,
									m_NumPlayers, m_Episode, m_Level,
									mMap->m_Dark);
		setupPlayers();

		mMap->m_Dark = dark;
		g_pGfxEngine->Palette.setdark(mMap->m_Dark);

		return true;
	}*/

	return false;
}
