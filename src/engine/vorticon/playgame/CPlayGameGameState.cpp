/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include "../../../StringUtils.h"
#include "../../../graphics/effects/CColorMerge.h"
#include "../../../sdl/CVideoDriver.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }

///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGameVorticon::loadGameState()
{
	// This fills the datablock from CSavedGame object
	if(m_SavedGame.load())
	{
		// Create the special merge effect (Fadeout)
		CColorMerge *pColorMergeFX = new CColorMerge(8);

		// get the episode, level and difficulty
		m_SavedGame.decodeData(m_Episode);
		m_SavedGame.decodeData(m_Level);
		m_SavedGame.decodeData(m_Difficulty);

		m_SavedGame.decodeData(m_checkpointset);
		m_SavedGame.decodeData(m_checkpoint_x);
		m_SavedGame.decodeData(m_checkpoint_y);
		m_SavedGame.decodeData(m_Map.m_Dark);

		// Load number of Players
		m_SavedGame.decodeData(m_NumPlayers);

		// Now load the inventory for every player
		m_Player.clear();
		m_Object.clear();

		m_level_command = START_LEVEL;
		g_pMusicPlayer->stop();

		// Prepare for loading the new level map and the players.
		cleanup();

		m_Player.assign(m_NumPlayers, CPlayer(m_Episode, m_Level, m_Difficulty,
				 mp_level_completed, mp_option, m_Object, m_Map));

		init();
		std::vector<CPlayer> :: iterator player;
		for( player=m_Player.begin() ; player != m_Player.end() ; player++ ) {
			int x, y;
			player->setupforLevelPlay();
			m_SavedGame.decodeData(x);
			m_SavedGame.decodeData(y);
			player->moveToForce(x,y-(4<<STC));
			m_SavedGame.decodeData(player->blockedd);
			m_SavedGame.decodeData(player->blockedu);
			m_SavedGame.decodeData(player->blockedl);
			m_SavedGame.decodeData(player->blockedr);
			m_SavedGame.decodeData(player->inventory);
		}

		// load the number of objects on screen
		Uint32 size;
		int x, y;
		m_Object.clear();
		m_SavedGame.decodeData(size);
		for( Uint32 i=0 ; i<size ; i++) {
			// save all the objects states

			// TODO: This code won't work the way it is.
			CObject *object=NULL;

			/*m_SavedGame.decodeData(object.m_type);
			m_SavedGame.decodeData(x);
			m_SavedGame.decodeData(y);
			//object.moveToForce(x,y-1);
			//object = new CObject(&m_Map);
			m_SavedGame.decodeData(object.dead);
			m_SavedGame.decodeData(object.needinit);
			m_SavedGame.decodeData(object.onscreen);
			m_SavedGame.decodeData(object.hasbeenonscreen);
			m_SavedGame.decodeData(object.exists);
			m_SavedGame.decodeData(object.blockedd);
			m_SavedGame.decodeData(object.blockedu);
			m_SavedGame.decodeData(object.blockedl);
			m_SavedGame.decodeData(object.blockedr);
			m_SavedGame.decodeData(object.HealthPoints);
			m_SavedGame.decodeData(object.canbezapped);
			m_SavedGame.decodeData(object.cansupportplayer);
			m_SavedGame.decodeData(object.inhibitfall);
			m_SavedGame.decodeData(object.honorPriority);
			m_SavedGame.decodeData(object.sprite);
			m_SavedGame.decodeData(object.ai);*/
			//if(object.m_type != OBJ_DOOR) // small workaround for doors which might be opening
				//m_Object.push_back(&object);
		}

		// TODO: An algorithm for comparing the number of players saved and we actually have need to be in sync

		// Load the map_data as it was left last
		m_SavedGame.decodeData(m_Map.m_width);
		m_SavedGame.decodeData(m_Map.m_height);
		m_SavedGame.readDataBlock( reinterpret_cast<byte*>(m_Map.getForegroundData()) );

		// Load completed levels
		m_SavedGame.readDataBlock( (byte*)(mp_level_completed));

		m_Player[0].mp_map = &m_Map;
		while(m_Player[0].scrollTriggers()); // Scroll to the right position on the map

		m_Map.drawAll();

		// Create the special merge effect (Fadeout)
		g_pGfxEngine->pushEffectPtr(pColorMergeFX);

		g_pGfxEngine->Palette.setdark(m_Map.m_Dark);

		//Set surface alpha
		SDL_SetAlpha( g_pVideoDriver->FGLayerSurface, SDL_SRCALPHA, 225 );

		return true;
	}

	return false;
}

bool CPlayGameVorticon::saveGameState()
{
	int i;
	int size;

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	m_SavedGame.encodeData(m_Episode);
	m_SavedGame.encodeData(m_Level);
	m_SavedGame.encodeData(m_Difficulty);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	m_SavedGame.encodeData(m_checkpointset);
	m_SavedGame.encodeData(m_checkpoint_x);
	m_SavedGame.encodeData(m_checkpoint_y);
	m_SavedGame.encodeData(m_Map.m_Dark);

	// Save number of Players
	m_SavedGame.encodeData(m_NumPlayers);

	// Now save the inventory of every player
	for( i=0 ; i<m_NumPlayers ; i++ ) {
		m_SavedGame.encodeData(m_Player[i].getXPosition());
		m_SavedGame.encodeData(m_Player[i].getYPosition());
		m_SavedGame.encodeData(m_Player[i].blockedd);
		m_SavedGame.encodeData(m_Player[i].blockedu);
		m_SavedGame.encodeData(m_Player[i].blockedl);
		m_SavedGame.encodeData(m_Player[i].blockedr);
		m_SavedGame.encodeData(m_Player[i].inventory);
	}

	size = m_Object.size();
	// save the number of objects on screen
	m_SavedGame.encodeData(size);
	for( i=0 ; i<size ; i++) {
		// save all the objects states
		m_SavedGame.encodeData(m_Object[i]->m_type);
		m_SavedGame.encodeData(m_Object[i]->getXPosition());
		m_SavedGame.encodeData(m_Object[i]->getYPosition());
		m_SavedGame.encodeData(m_Object[i]->dead);
		//m_SavedGame.encodeData(m_Object[i]->needinit);
		m_SavedGame.encodeData(m_Object[i]->onscreen);
		m_SavedGame.encodeData(m_Object[i]->hasbeenonscreen);
		m_SavedGame.encodeData(m_Object[i]->exists);
		m_SavedGame.encodeData(m_Object[i]->blockedd);
		m_SavedGame.encodeData(m_Object[i]->blockedu);
		m_SavedGame.encodeData(m_Object[i]->blockedl);
		m_SavedGame.encodeData(m_Object[i]->blockedr);
		m_SavedGame.encodeData(m_Object[i]->HealthPoints);
		m_SavedGame.encodeData(m_Object[i]->canbezapped);
		m_SavedGame.encodeData(m_Object[i]->cansupportplayer);
		m_SavedGame.encodeData(m_Object[i]->inhibitfall);
		m_SavedGame.encodeData(m_Object[i]->honorPriority);
		m_SavedGame.encodeData(m_Object[i]->sprite);
		m_SavedGame.encodeData(m_Object[i]->ai);
	}

	// Save the map_data as it is left
	m_SavedGame.encodeData(m_Map.m_width);
	m_SavedGame.encodeData(m_Map.m_height);
	m_SavedGame.addData( reinterpret_cast<byte*>(m_Map.getForegroundData()), 2*m_Map.m_width*m_Map.m_height );

	// store completed levels
	m_SavedGame.addData( (byte*)(mp_level_completed), MAX_LEVELS_VORTICON );

	return m_SavedGame.save();
}
