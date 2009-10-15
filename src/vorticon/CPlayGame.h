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

#define DOOR_YELLOW        2
#define DOOR_RED           3
#define DOOR_GREEN         4
#define DOOR_BLUE          5

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
	bool scrollTriggers();

	// Collision stuff stored in CPlayGameCollision.cpp
	void checkPlayerCollisions(int cp);
	char checkisSolidr(int x, int y, int cp);
	bool checkisSolidl(int x, int y, int cp);
	char checkDoorBlock(int t, int cp, int which);
	int checkObjSolid(unsigned int x, unsigned int y, unsigned int cp);

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
	int  m_NumSprites;
	char m_Episode;
	char m_Level;
	char m_NumPlayers;
	char m_Difficulty;
	char m_level_command;
	std::string m_Gamepath;
	bool mp_level_completed[16];

	CMap *mp_Map;
	CMenu *mp_Menu;
	CPlayer *mp_Player;
	int m_theplayer;
	std::vector<CObject> m_Object;
};
#endif /* CPlayGame_H_ */
