/*
 * CPlayGameNew.h
 *
 *  Created on: 26.05.2010
 *      Author: gerstrong
 *
 *  This is not the old class that before was called that way and served for the vorticon engine.
 *  This one serves for both engines and is a base class
 */

#ifndef CPLAYGAME_H_
#define CPLAYGAME_H_

const int WORLD_MAP_LEVEL=80;

class CPlayGame {
public:
	CPlayGame();

	// TODO: These are virtual functions which later will be forced to be used by its child classes
	virtual bool loadGameState(){return false;}
	virtual void process(){}
	virtual bool init(){return false;}
	virtual void cleanup(){}

	bool isFinished();
	bool getEndGame();
	bool getStartGame();
	bool getExitEvent();
	char getEpisode();
	char getNumPlayers();
	char getDifficulty();

	virtual ~CPlayGame();

private:
	bool m_finished;
	bool m_endgame;
	bool m_startgame;
	bool m_exitgame;
	char m_Episode;
	char m_NumPlayers;
	char m_Difficulty;

};

#endif /* CPLAYGAME_H_ */
