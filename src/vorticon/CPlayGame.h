/*
 * CPlayGame.h
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 */


#ifndef CPlayGame_H_
#define CPlayGame_H_

#include "../graphics/CGfxEngine.h"
#include "../common/CPlayer.h"
#include "../common/CObject.h"
#include "../common/CMenu.h"
#include "../common/CMap.h"
#include <SDL/SDL.h>
#include <string>
#include <vector>

#define WORLD_MAP_LEVEL 80

class CPlayGame {
public:

enum e_levelcommands
{
	NONE,
	GOTO_WORLD_MAP,
	START_LEVEL,
};

	CPlayGame( char episode, char level, 
			char numplayers, char difficulty,
			std::string &gamepath );

	bool init();
	bool loadGameState( std::string &statefile );

	void process();
	void drawObjects();

	bool isFinished() 
		{ return m_finished; }

	bool getEndGame() { return m_endgame; }
	bool getExitEvent() { return m_exitgame; }

	void cleanup();
	virtual ~CPlayGame();

private:
	bool m_finished;
	bool m_endgame;
	bool m_exitgame;
	char m_Episode;
	char m_Level;
	char m_NumPlayers;
	char m_Difficulty;
	char m_level_command;
	std::string m_Gamepath;

	CMap *mp_Map;
	CMenu *mp_Menu;
	CPlayer *mp_Player;
	std::vector<CObject> m_Object;
};
#endif /* CPlayGame_H_ */
