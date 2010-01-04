/*
 * CPlayGame.h
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 */


#ifndef CPlayGame_H_
#define CPlayGame_H_

#include "../../graphics/CGfxEngine.h"
#include "../../fileio/CSavedGame.h"
#include "../../dialog/CMessageBox.h"
#include "../../common/CPlayer.h"
#include "../../common/CObject.h"
#include "../../common/CMenu.h"
#include "../../common/CMap.h"
#include "../../common/options.h"
#include "../../common/CPhysicsSettings.h"
#include "../../common/CTeleporter.h"
#include "../ai/CObjectAI.h"
#include "../ai/CEGABitmap.h"
#include "../finale/CEndingEp1.h"
#include "../finale/CEndingEp2.h"
#include "../finale/CEndingEp3.h"
#include "../../hqp/CMusic.h"
#include "../infoscenes/CHighScores.h"
#include <SDL.h>
#include <string>
#include <vector>
#include <list>

enum door_colours{
DOOR_YELLOW = 2,
DOOR_RED = 3,
DOOR_GREEN = 4,
DOOR_BLUE =  5
};

const int WORLD_MAP_LEVEL=80;

class CPlayGame {
public:

	enum e_levelcommands
	{
		NONE,
		GOTO_WORLD_MAP,
		START_LEVEL,
		GOTO_FINALE
	};

	CPlayGame( char episode, char level,
			  char numplayers, char difficulty,
			  std::string &gamepath, stOption *p_option,
			  bool finale, CSavedGame &SavedGame,
			  std::vector<stTeleporterTable> &TeleporterTable);

	void setupPlayers();
	bool init();
	void createPlayerObjects();

	// Game states
	bool loadGameState();
	bool saveGameState();

	void process();
	void processOnWorldMap();
	void processInLevel();
	void showKeensLeft();
	void goBacktoMap();
	void YourShipNeedsTheseParts();
	void ShipEp3();
	void processObjectsAI();
	void drawObjects();
	void handleFKeys();
	void verifyCutscenes();
	void collectHighScoreInfo();
	void createFinale();

	// Collision stuff stored in CPlayGameCollision.cpp
	void checkPlayerCollisions(CPlayer *p_player);
	bool checkisSolidr(CPlayer *p_player);
	bool checkisSolidl(CPlayer *p_player);
	bool checkisSolidd(CPlayer *p_player);
	bool checkisSolidu(CPlayer *p_player);
	void checkStandingOnIce(CPlayer &player);
	char checkDoorBlock(int t, CPlayer *p_player, int which);
	int checkObjSolid(unsigned int x, unsigned int y, int cp);
	void processPlayerfallings(CPlayer *p_player);
	void processLevelTrigger(int trigger);
	void losePlayer(CPlayer *p_player);

	// Dialog processes stored in CPlayGameDialogs.cpp
	void processPauseDialogs();
	void showPausedGameDlg();

	bool isFinished()
	{ return m_finished; }

	bool getEndGame();
	bool getStartGame();
	bool getExitEvent();
	char getEpisode();
	char getNumPlayers();
	char getDifficulty();

	void cleanup();
	virtual ~CPlayGame();

private:
	bool m_finished;
	bool m_endgame;
	bool m_startgame;
	bool m_exitgame;
	int  m_NumSprites;
	char m_Episode;
	short m_Level;
	char m_NumPlayers;
	char m_Difficulty;
	char m_level_command;
	bool m_paused;
	bool m_gameover;
	bool m_alldead;
	bool m_showPauseDialog;
	bool m_hideobjects;
	std::string m_Gamepath;
	bool mp_level_completed[MAX_LEVELS];
	int m_checkpoint_x, m_checkpoint_y;
	bool m_checkpointset;
	bool m_dark;

	CMap m_Map;
	CMenu *mp_Menu;
	std::vector<CPlayer> m_Player;
	CObjectAI *mp_ObjectAI;
	CFinale *mp_Finale;
	CEGABitmap *mp_gameoverbmp;
	stOption *mp_option;
	CSavedGame &m_SavedGame;
	CPhysicsSettings m_PhysicsSettings;
	std::list<CMessageBox*> m_MessageBoxes;
	std::vector<stTeleporterTable> &m_TeleporterTable;
	bool m_showKeensLeft;
	std::vector<CObject> m_Object;
	CHighScores *mp_HighScores;
};
#endif /* CPlayGame_H_ */
