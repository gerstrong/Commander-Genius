/**
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
 *  of AI they are needed and of course the collision
 */


#ifndef CBEHAVIORENGINE_H_
#define CBEHAVIORENGINE_H_

#include <vector>
#include <string>
#include <map>
#include <SDL.h>
#include "engine/keen/galaxy/res/EGAStructs.h"
#include <base/Configurator.h>
#include "fileio/CTileProperties.h"
#include "CPhysicsSettings.h"
#include <base/TypeDefinitions.h>
#include <base/GsEvent.h>
#include "engine/core/options.h"
#include "Cheat.h"

#include <base/GsEvent.h>

#include <base/Singleton.h>

#if defined (SINGLEPLAYER)
const unsigned int MAX_PLAYERS = 1;
#else
const unsigned int MAX_PLAYERS = 4;
#endif

#define gBehaviorEngine CBehaviorEngine::get()

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
	UNKNOWN = 0,
	EASY = 1,
	NORMAL = 2,
    HARD = 3,
    EXPERT = 4,
    NINJA = 5,
    ELITE = 6
};


typedef struct
{
	Uint8 objectnumber1; // Andy (Adurdin) calls that sprite, not all his numbers seem to be right
	Uint8 objectnumber2; // Alternate number
	Uint16 x, y;
} stTeleporterTable;

class CBehaviorEngine : public GsSingleton<CBehaviorEngine>
{
public:

	void setMessage(const std::string &name,
					const std::string &message);

	bool readTeleporterTable(byte *p_exedata);

	std::vector<CTileProperties> &getTileProperties(size_t tmnum = 1);
    CPhysicsSettings &getPhysicsSettings()
    {
        return m_PhysicsSettings;
    }

	std::string getString(const std::string& name);

	// This function evaluates if the used engine is galaxy or vorticon
	EngineType getEngine();
	size_t getEpisode();
	bool isDemo();
	stTeleporterTable& getTeleporterTableAt(size_t num)
	{ return m_TeleporterTable[num]; }
	std::vector<stTeleporterTable>& getTeleporterTable()
	{ return m_TeleporterTable; }

    //CEventContainer &EventList()
    //{	return gEventManager;	}

    void setEpisode(const size_t Episode)
    {	mEpisode = Episode;	}

    void setDemo(bool demo)
    {	mDemo = demo;	}

	void setPause(const bool value)
	{	mPausedGamePlay = value;	}

	bool paused()
	{	return mPausedGamePlay;	}

    std::string mPatchFname;
    std::map<GameOption, stOption> mOptions;

    Difficulty mDifficulty = EASY;

    EpisodeInfoStruct* getEpisodeInfoStructRef()
    {	return &pEpisodeInfo[mDemo ? 4 : mEpisode - 4];	}

	void setEpisodeInfoStructPtr(const EpisodeInfoStruct* epStruct)
    {	pEpisodeInfo = const_cast<EpisodeInfoStruct*>(epStruct);	}
	
	
    auto numPlayers() const {
        return mPlayers;
    }

    void setNumPlayers(const int numPlayers){
        mPlayers = numPlayers;
    }
/*

    auto numPlayerVariants() const {
        return mPlayers;
    }

    void setNumPlayerVariants(const int numPlayerVar){
        mPlayerVariants = numPlayerVar;
    }
*/

	std::string mapLevelName;

    stCheat mCheatmode;

private:

    /**
     * @brief mPlayers  Number of players
     */
    int mPlayers = 1;

    /**
     * @brief mPlayerVarianst  Number of players that totally are possible.
     *                         Means not what is currently being played.
     */
    //int mPlayerVariants = 1;


	std::vector<CTileProperties> m_TileProperties[2];
    CPhysicsSettings m_PhysicsSettings;

	std::map<std::string,std::string> stringmap;
	std::vector<stTeleporterTable> m_TeleporterTable; // Teleporter table used for the destinations
													  // used by Episode 1 especially
	int numStrings;
    size_t mEpisode;
    bool mDemo;

    bool mPausedGamePlay = false;

    EpisodeInfoStruct *pEpisodeInfo;

    int mBwbLevel = 99;
};

#endif /* CBEHAVIORENGINE_H_ */
