/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../../StringUtils.h"
///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGame::loadGameState()
{
	// TODO: Add Code here!
	return m_SavedGame.load();
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

	// Load the map_data as it was left last
	m_SavedGame.encodeVariable(mp_Map->m_width);
	m_SavedGame.encodeVariable(mp_Map->m_height);
	m_SavedGame.addData( (uchar*)(mp_Map->mp_data), 2*mp_Map->m_width*mp_Map->m_height );

	// store completed levels
	m_SavedGame.addData( (uchar*)(mp_level_completed), MAX_LEVELS );

	return m_SavedGame.save();
}
