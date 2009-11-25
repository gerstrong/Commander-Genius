/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../../StringUtils.h"

#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }

///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGame::loadGameState()
{
	// This fills the datablock from CSavedGame object
	if(m_SavedGame.load())
	{
		// get the episode, level and difficulty
		m_SavedGame.decodeVariable(m_Episode);
		m_SavedGame.decodeVariable(m_Level);
		m_SavedGame.decodeVariable(m_Difficulty);

		m_SavedGame.decodeVariable(m_checkpointset);
		m_SavedGame.decodeVariable(m_checkpoint_x);
		m_SavedGame.decodeVariable(m_checkpoint_y);

		// Load number of Players
		m_SavedGame.decodeVariable(m_NumPlayers);

		// Now load the inventory of every player
		SAFE_DELETE_ARRAY(mp_Player);
		mp_Player = new CPlayer[m_NumPlayers];
		for( int i=0 ; i<m_NumPlayers ; i++ ) {
			m_SavedGame.decodeStruct(mp_Player[i].inventory);
			m_SavedGame.decodeStruct(mp_Player[i].x);
			m_SavedGame.decodeStruct(mp_Player[i].y);
			mp_Player[i].goto_x = mp_Player[i].x;
			mp_Player[i].goto_y = mp_Player[i].y;
		}

		// load the number of objects on screen
		Uint32 size;
		m_SavedGame.decodeVariable(size);
		m_Object.clear();
		for( Uint32 i=0 ; i<size ; i++) {
			// save all the objects states
			m_Object.pop_back();
			m_SavedGame.decodeVariable(m_Object[i].m_type);
			m_SavedGame.decodeVariable(m_Object[i].x);
			m_SavedGame.decodeVariable(m_Object[i].y);
			m_Object[i].new_x = m_Object[i].x;
			m_Object[i].new_y = m_Object[i].y;
			m_SavedGame.decodeVariable(m_Object[i].dead);
			m_SavedGame.decodeVariable(m_Object[i].exists);
			m_SavedGame.decodeStruct(m_Object[i].ai);
		}

		// An algorithm for comparing the number of players saved and we actually have need to be in sync

		// Load the map_data as it was left last
		m_SavedGame.decodeVariable(mp_Map->m_width);
		m_SavedGame.decodeVariable(mp_Map->m_height);
		SAFE_DELETE_ARRAY(mp_Map->mp_data);
		mp_Map->mp_data = new Uint16[mp_Map->m_height*mp_Map->m_width];
		m_SavedGame.readDataBlock( (uchar*)(mp_Map->mp_data));

		// Load completed levels
		m_SavedGame.readDataBlock( (uchar*)(mp_level_completed));

		return true;
	}

	return false;
}

bool CPlayGame::saveGameState()
{
	int i;
	int size;

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	m_SavedGame.encodeVariable(m_Episode);
	m_SavedGame.encodeVariable(m_Level);
	m_SavedGame.encodeVariable(m_Difficulty);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	m_SavedGame.encodeVariable(m_checkpointset);
	m_SavedGame.encodeVariable(m_checkpoint_x);
	m_SavedGame.encodeVariable(m_checkpoint_y);

	// Save number of Players
	m_SavedGame.encodeVariable(m_NumPlayers);

	// Now save the inventory of every player
	for( i=0 ; i<m_NumPlayers ; i++ ) {
		m_SavedGame.encodeStruct(mp_Player[i].inventory);
	}

	size = m_Object.size();
	// save the number of objects on screen
	m_SavedGame.encodeVariable(size);
	for( i=0 ; i<size ; i++) {
		// save all the objects states
		m_SavedGame.encodeVariable(m_Object[i].m_type);
		m_SavedGame.encodeVariable(m_Object[i].x);
		m_SavedGame.encodeVariable(m_Object[i].y);
		m_SavedGame.encodeVariable(m_Object[i].dead);
		m_SavedGame.encodeVariable(m_Object[i].exists);
		m_SavedGame.encodeStruct(m_Object[i].ai);
	}

	// Save the map_data as it is left
	m_SavedGame.encodeVariable(mp_Map->m_width);
	m_SavedGame.encodeVariable(mp_Map->m_height);
	m_SavedGame.addData( (uchar*)(mp_Map->mp_data), 2*mp_Map->m_width*mp_Map->m_height );

	// store completed levels
	m_SavedGame.addData( (uchar*)(mp_level_completed), MAX_LEVELS );

	return m_SavedGame.save();
}
