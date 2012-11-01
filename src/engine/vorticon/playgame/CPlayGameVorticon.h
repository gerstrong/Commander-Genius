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
#include "fileio/CSaveGameController.h"
#include "common/CPlayer.h"
#include "common/CTileProperties.h"
#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CMap.h"
#include "common/options.h"
#include "common/CHUD.h"
#include "engine/vorticon/ai/CVorticonSpriteObjectAI.h"
#include "engine/vorticon/ai/CEGABitmap.h"
#include "engine/vorticon/finale/CEndingEp1.h"
#include "engine/vorticon/finale/CEndingEp2.h"
#include "engine/vorticon/finale/CEndingEp3.h"
#include "engine/vorticon/dialog/CMessageBoxVort.h"
#include "sdl/music/CMusic.h"
#include "engine/infoscenes/CHighScores.h"
#include "engine/playgame/CPlayGame.h"
#include "SmartPointer.h"
#include <SDL.h>
#include <string>
#include <vector>
#include <list>
#include <memory>

const int WORLD_MAP_LEVEL_VORTICON = 80;

enum door_colours{
DOOR_YELLOW = 2,
DOOR_RED = 3,
DOOR_GREEN = 4,
DOOR_BLUE =  5
};

class CPlayGameVorticon : public CPlayGame
{
public:

	CPlayGameVorticon(CExeFile &ExeFile, char level,
			  char numplayers,  CSaveGameController &SavedGame);

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
	void cycleCamLead();
	void handleFKeys();
	void verifyFinales();
	void collectHighScoreInfo(CHighScores &highScores);
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

	// Tell whether any of the Players' status screen is open
	bool StatusScreenOpen();

	bool mp_level_completed[MAX_LEVELS_VORTICON];

	std::shared_ptr<CMap> mMap;
	std::vector<CPlayer> m_Player;
	SmartPointer<CVorticonSpriteObjectAI> mpObjectAI;
	SmartPointer<CFinale> mpFinale;
	SmartPointer<CEGABitmap> mpGameoverBmp;
	std::list< SmartPointer<CMessageBoxVort> > mMessageBoxes;
	bool m_showKeensLeft;
	std::vector<CVorticonSpriteObject*> m_Object;
	SmartPointer<CHUD> mp_HUD;
	std::shared_ptr<SDL_Surface> mpKeenLeftSfc;
	SDL_Rect keenleft_rect;

	std::shared_ptr<SDL_Surface> mpFPSSurface;
};
#endif /* CPlayGameVorticon_H_ */
