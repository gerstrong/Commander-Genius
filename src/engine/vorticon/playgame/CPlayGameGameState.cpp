/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include "StringUtils.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/CVideoDriver.h"
#include "common/CVorticonMapLoader.h"
#include "SmartPointer.h"


///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGameVorticon::loadGameState()
{
	CSaveGameController &savedGame = *(gpSaveGameController);

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
	}

	return false;
}

bool CPlayGameVorticon::saveGameState()
{
	size_t i;
	size_t size;

	CSaveGameController &savedGame = *(gpSaveGameController);

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	savedGame.encodeData(m_Episode);
	savedGame.encodeData(m_Level);
	savedGame.encodeData(g_pBehaviorEngine->mDifficulty);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	savedGame.encodeData(m_checkpointset);
	savedGame.encodeData(m_checkpoint_x);
	savedGame.encodeData(m_checkpoint_y);
	savedGame.encodeData(mMap->m_Dark);

	// Save number of Players
	savedGame.encodeData(m_NumPlayers);

	// Now save the inventory of every player
	for( i=0 ; i<m_NumPlayers ; i++ )
	{
		savedGame.encodeData(m_Player[i].getXPosition());
		savedGame.encodeData(m_Player[i].getYPosition());
		savedGame.encodeData(m_Player[i].blockedd);
		savedGame.encodeData(m_Player[i].blockedu);
		savedGame.encodeData(m_Player[i].blockedl);
		savedGame.encodeData(m_Player[i].blockedr);
		savedGame.encodeData(m_Player[i].inventory);
	}

	size = m_Object.size();
	// save the number of objects on screen
	savedGame.encodeData(size);
	for( i=0 ; i<size ; i++) {
		// save all the objects states
		savedGame.encodeData(m_Object[i]->m_type);
		savedGame.encodeData(m_Object[i]->getXPosition());
		savedGame.encodeData(m_Object[i]->getYPosition());
		savedGame.encodeData(m_Object[i]->dead);
		savedGame.encodeData(m_Object[i]->onscreen);
		savedGame.encodeData(m_Object[i]->hasbeenonscreen);
		savedGame.encodeData(m_Object[i]->exists);
		savedGame.encodeData(m_Object[i]->blockedd);
		savedGame.encodeData(m_Object[i]->blockedu);
		savedGame.encodeData(m_Object[i]->blockedl);
		savedGame.encodeData(m_Object[i]->blockedr);
		savedGame.encodeData(m_Object[i]->mHealthPoints);
		savedGame.encodeData(m_Object[i]->canbezapped);
		savedGame.encodeData(m_Object[i]->cansupportplayer);
		savedGame.encodeData(m_Object[i]->inhibitfall);
		savedGame.encodeData(m_Object[i]->honorPriority);
		savedGame.encodeData(m_Object[i]->sprite);
	}

	// Save the map_data as it is left
	savedGame.encodeData(mMap->m_width);
	savedGame.encodeData(mMap->m_height);
	savedGame.addData( reinterpret_cast<byte*>(mMap->getForegroundData()),
													2*mMap->m_width*mMap->m_height );

	// store completed levels
	savedGame.addData( (byte*)(mp_level_completed), MAX_LEVELS_VORTICON );

	return savedGame.save();
}
