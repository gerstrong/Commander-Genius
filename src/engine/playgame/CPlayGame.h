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

#include "fileio/CExeFile.h"
#include "core/LogicDrawSplitter.h"
#include "common/options.h"
#include "common/CBehaviorEngine.h"
#include "core/mode/CGameMode.h"
#include "fileio/CSaveGameController.h"
#include <string>
#include <SDL.h>

const int WM_MAP_NUM = 80;


class CPlayGame : public CGameMode
{
public:
    CPlayGame(CExeFile &ExeFile, char level);

    void loadGame();

    virtual bool loadGameState() = 0;
    //virtual bool saveGameState() = 0;
    virtual bool loadXMLGameState() = 0;
    virtual bool saveXMLGameState() = 0;
	virtual void cleanup() = 0;

    virtual void pumpEvent(const CEvent *evPtr);

	bool isFinished();
	bool getEndGame();
	bool getStartGame();
	bool getExitEvent();
	char getEpisode();
	char getNumPlayers();
	CSaveGameController getSavedGameBlock();

protected:
	bool m_endgame;
	CExeFile &m_ExeFile;
	char m_Episode;
	short m_Level;
	int mCamLead;
	bool m_startgame;
	bool m_exitgame;

	Uint32  m_NumSprites;

	enum
	{
		NONE, GOTO_WORLD_MAP,
		START_LEVEL, GOTO_FINALE
	} m_level_command;

	bool m_gameover;
	bool m_alldead;
	bool m_showPauseDialog;
	bool m_hideobjects;
	std::string m_Gamepath;
	int m_checkpoint_x, m_checkpoint_y;
	bool m_checkpointset;
	bool m_restartVideo;

	stOption *mp_option;
};

#endif /* CPLAYGAME_H_ */
