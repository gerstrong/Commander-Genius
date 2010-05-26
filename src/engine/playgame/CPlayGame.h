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
	virtual bool getStartGame(){return false;}
	virtual bool getEndGame(){return false;}
	virtual char getNumPlayers(){return 0;}
	virtual char getDifficulty(){return -1;}
	virtual bool getExitEvent(){return false;}
	virtual void process(){}
	virtual bool init(){return false;}
	virtual void cleanup(){}

	virtual ~CPlayGame();
};

#endif /* CPLAYGAME_H_ */
