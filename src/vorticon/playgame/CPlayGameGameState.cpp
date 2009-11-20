/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGame::loadGameState(std::string statefile)
{
	// TODO: Add Code here!
	return false;
}

bool CPlayGame::saveGameState(std::string statefile)
{
	int i;
	int size;
	// Create the CSaved Game object
	CSavedGame SavedGame(m_Gamepath);

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	SavedGame.encodeVariable(m_Episode);
	SavedGame.encodeVariable(m_Level);
	SavedGame.encodeVariable(m_Difficulty);

	// Get number of players
	SavedGame.encodeVariable(m_NumPlayers);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	SavedGame.encodeVariable(m_checkpointset);
	SavedGame.encodeVariable(m_checkpoint_x);
	SavedGame.encodeVariable(m_checkpoint_y);

	// save the inventory of every player
	SavedGame.encodeVariable(m_NumPlayers);
	for( i=0 ; i<m_NumPlayers ; i++) {
		SavedGame.encodeStruct(mp_Player[i].inventory);
	}

	size = m_Object.size();
	// save the number of objects on screen
	SavedGame.encodeVariable(size);
	for( i=0 ; i<size ; i++) {
		// save all the objects states
		SavedGame.encodeVariable(m_Object[i].m_type);
		SavedGame.encodeVariable(m_Object[i].x);
		SavedGame.encodeVariable(m_Object[i].y);
		SavedGame.encodeVariable(m_Object[i].dead);
		SavedGame.encodeVariable(m_Object[i].exists);
		SavedGame.encodeStruct(m_Object[i].ai);
	}

	// Load the map_data as it was left lastly
	SavedGame.encodeVariable(mp_Map->m_width);
	SavedGame.encodeVariable(mp_Map->m_height);
	SavedGame.addData( (uchar*)(mp_Map->mp_data), 2*mp_Map->m_width*mp_Map->m_height );

	// store completed levels
	SavedGame.addData( (uchar*)(mp_level_completed), MAX_LEVELS );

	//SavedGame.save();

	return true;
}
