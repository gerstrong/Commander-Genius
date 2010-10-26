/*
 * CPlayGameVorticon.h
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 */


#ifndef CPlayGameVorticon_H_
#define CPlayGameVorticon_H_

#include "graphics/CGfxEngine.h"
#include "fileio/CSavedGame.h"
#include "dialog/CMessageBox.h"
#include "common/CPlayer.h"
#include "common/CTileProperties.h"
#include "common/CObject.h"
#include "common/CMap.h"
#include "common/options.h"
#include "common/CHUD.h"
#include "engine/vorticon/ai/CObjectAI.h"
#include "engine/vorticon/ai/CEGABitmap.h"
#include "engine/vorticon/CMenuVorticon.h"
#include "engine/vorticon/finale/CEndingEp1.h"
#include "engine/vorticon/finale/CEndingEp2.h"
#include "engine/vorticon/finale/CEndingEp3.h"
#include "hqp/CMusic.h"
#include "engine/infoscenes/CHighScores.h"
#include "engine/playgame/CPlayGame.h"
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

class CPlayGameVorticon : public CPlayGame {
public:

	CPlayGameVorticon(CExeFile &ExeFile, char level,
			  char numplayers, Uint8& difficulty,
			  stOption *p_option,
			  bool finale, CSavedGame &SavedGame);

	void setupPlayers();
	bool init();

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
	void drawAllElements();
	void handleFKeys();
	void verifyFinales();
	void collectHighScoreInfo();
	void createFinale();

	// Collision stuff stored in CPlayGameVorticonCollision.cpp
	void checkPlayerCollisions(CPlayer *p_player);
	bool checkisSolidr(CPlayer *p_player);
	bool checkisSolidl(CPlayer *p_player);
	bool checkisSolidd(CPlayer *p_player);
	bool checkisSolidu(CPlayer *p_player);
	void checkStandingOnIce(CPlayer &player);
	char checkDoorBlock(int t, CPlayer *p_player, int which);
	void processPlayerfallings(CPlayer *p_player);
	void processLevelTrigger(int trigger);
	void losePlayer(CPlayer *p_player);

	// Dialog processes stored in CPlayGameVorticonDialogs.cpp
	void processPauseDialogs();
	void showPausedGameDlg();

	// Teleportation Stuff
	int getTeleporterInfo(int objectID);
	void teleportPlayer(int objectID, CPlayer &player);
	void teleportPlayerFromLevel( CPlayer &player, int origx, int origy);
	void readTeleportDestCoordinatesEP1(int objectID, int &destx, int &desty);
	void readTeleportDestCoordinatesEP3(int objectID, int &destx, int &desty);

	void cleanup();
	virtual ~CPlayGameVorticon();

private:
	bool mp_level_completed[MAX_LEVELS_VORTICON];

	CMap m_Map;
	CMenuVorticon *mp_Menu;
	std::vector<CPlayer> m_Player;
	CObjectAI *mp_ObjectAI;
	CFinale *mp_Finale;
	CEGABitmap *mp_gameoverbmp;
	CSavedGame &m_SavedGame;
	std::list<CMessageBox*> m_MessageBoxes;
	bool m_showKeensLeft;
	std::vector<CObject*> m_Object;
	CHighScores *mp_HighScores;
	CHUD *mp_HUD;
	SDL_Surface *mp_KeenLeftSfc;
};
#endif /* CPlayGameVorticon_H_ */
