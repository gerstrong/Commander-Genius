/*
 * CPlayGame.h
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 *
 *  This is not the old class that before was called that way and served for the vorticon engine.
 *  This one serves for both engines and is a base class
 */

#ifndef CPLAYGAME_H_
#define CPLAYGAME_H_

#include <string>
#include "../../fileio/CExeFile.h"
#include <SDL.h>

const int WORLD_MAP_LEVEL=80;

class CPlayGame{
public:
	CPlayGame(CExeFile &ExeFile, char level,
			 char numplayers, char difficulty);

	// TODO: These are virtual functions which later will be forced to be used by its child classes
	virtual bool loadGameState() = 0;
	virtual void process() = 0;
	virtual bool init() = 0;
	virtual void cleanup() = 0;

	bool isFinished();
	bool getEndGame();
	bool getStartGame();
	bool getExitEvent();
	char getEpisode();
	char getNumPlayers();
	char getDifficulty();

	virtual ~CPlayGame();

protected:
	char m_NumPlayers;
	bool m_endgame;
	CExeFile &m_ExeFile;
	char m_Episode;
	short m_Level;
	bool m_startgame;
	bool m_exitgame;
	char m_Difficulty;

	Uint32  m_NumSprites;

	enum
	{
		NONE, GOTO_WORLD_MAP,
		START_LEVEL, GOTO_FINALE
	} m_level_command;

	bool m_paused;
	bool m_gameover;
	bool m_alldead;
	bool m_showPauseDialog;
	bool m_hideobjects;
	std::string m_Gamepath;
	int m_checkpoint_x, m_checkpoint_y;
	bool m_checkpointset;
};

#endif /* CPLAYGAME_H_ */
