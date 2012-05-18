/*
 * CBehaviorEngine.h
 *
 *  Created on: 11.06.2010
 *      Author: gerstrong
 *
 *  This is all a new Engine which will store
 *  the all the behaviors that are preloaded from the
 *  gamedata files.
 *
 *  An example are the TileProperties. At some places
 *  of AI they are needed and especially for the collision
 */


#ifndef CBEHAVIORENGINE_H_
#define CBEHAVIORENGINE_H_

#include <vector>
#include <string>
#include <map>
#include "fileio/CExeFile.h"
#include "CTileProperties.h"
#include "CPhysicsSettings.h"
#include "fileio/TypeDefinitions.h"
#include "engine/CEventContainer.h"
#include "common/options.h"

#include "CSingleton.h"

#define MAX_PLAYERS            4

#define g_pBehaviorEngine CBehaviorEngine::Get()

/*
 * This enumerator will hold and tell what engine we are using.
 * As some Vorticon and Galaxy classes are shared together, that variable is used to get them created correctly.
 */
enum EngineType
{
	ENGINE_LAUNCHER,
	ENGINE_VORTICON,
	ENGINE_GALAXY
};


enum Difficulty
{
	EASY = 1,
	NORMAL,
	HARD
};


typedef struct{
	Uint8 objectnumber1; // Andy (Adurdin) calls that sprite, not all his numbers seem to be right
	Uint8 objectnumber2; // Alternate number
	Uint16 x, y;
}stTeleporterTable;

class CBehaviorEngine : public CSingleton<CBehaviorEngine>
{
public:
	CBehaviorEngine() : 	mPlayers(0),
							mDifficulty(EASY),
							mPausedGamePlay(false) {}

	void addMessage(const std::string &name,
					const std::string &message);

	bool readTeleporterTable(byte *p_exedata);

	std::vector<CTileProperties> &getTileProperties(size_t tmnum = 1);
	CPhysicsSettings &getPhysicsSettings();
	std::string getString(const std::string& name);

	// This function evaluates if the used engine is galaxy or vorticon
	EngineType getEngine();
	size_t getEpisode();
	stTeleporterTable& getTeleporterTableAt(size_t num)
	{ return m_TeleporterTable[num]; }
	std::vector<stTeleporterTable>& getTeleporterTable()
	{ return m_TeleporterTable; }

	CEventContainer &EventList()
	{	return m_EventList;	}

	void setEpisode(size_t Episode);

	void setPause(const bool value)
	{	mPausedGamePlay = value;	}

	bool paused()
	{	return mPausedGamePlay;	}

	// TODO: EventList must an own singleton Container, as it very often used.
	CEventContainer m_EventList;
	CExeFile m_ExeFile;
	bool m_is_a_mod;
	stOption m_option[NUM_OPTIONS];

	unsigned int mPlayers;
	Difficulty mDifficulty;


private:
	std::vector<CTileProperties> m_TileProperties[2];
	CPhysicsSettings m_PhysicsSettings;

	std::map<std::string,std::string> stringmap;
	std::vector<stTeleporterTable> m_TeleporterTable; // Teleporter table used for the destinations
													  // used by Episode 1 especially
	int numStrings;
	size_t m_Episode;

	bool mPausedGamePlay;
};

#endif /* CBEHAVIORENGINE_H_ */
