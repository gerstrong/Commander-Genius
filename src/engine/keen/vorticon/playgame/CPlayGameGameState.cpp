/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include <graphics/CColorMerge.h>
#include "../CVorticonMapLoader.h"
#include <base/interface/Base64.h>

#include <base/interface/property_tree/property_tree.h>

///////////////////////////
// Game State Management //
///////////////////////////

bool CPlayGameVorticon::loadXMLGameState()
{
    // Cleanups
    // Prepare for loading the new level map and the players.
    cleanup();

    /// Create tree
    using GsKit::ptree;
    ptree pt;

    CSaveGameController &savedGame = gSaveGameController;
    if(!savedGame.loadXMLTree(pt))
        return false;

    /// Load the nodes and retrieve the data as needed
    ptree &stateNode = pt.get_child("GameState");

    /// Load the Game in the CSavedGame object
    // get the episode, level and difficulty
    m_Episode = stateNode.get<int>("episode", 1); // Default value = 1. Bit strange not?
    int newLevel = stateNode.get<int>("level", 1);
    gBehaviorEngine.mDifficulty =
            static_cast<Difficulty>(stateNode.get<int>("difficulty", int(NORMAL) ));

    for( auto &stateTree : pt.get_child("GameState") )
    {
        const std::string tag = stateTree.first;
        if(tag == "checkpoint")
        {
            // Also the last checkpoint is stored. This is the level entered from map
            // in Commander Keen games
            auto &chkpnt = stateTree.second;
            m_checkpointset = chkpnt.get<bool>("<xmlattr>.set", false);
            m_checkpoint_x = chkpnt.get<int>("<xmlattr>.x", 0);
            m_checkpoint_y = chkpnt.get<int>("<xmlattr>.y", 0);
        }
    }


    // Create the special merge effect (Fadeout)
    CColorMerge *pColorMergeFX = new CColorMerge(8);


    CVorticonMapLoaderWithPlayer Maploader(mMap, m_Player, mSpriteObjectContainer);

    const bool isMap = (m_Level == 80);

    const bool loadmusic = ( m_Level != newLevel || isMap );
    m_Level = newLevel;

    if(!Maploader.load(m_Episode, m_Level, m_Gamepath, loadmusic, false))
      return false;

    mCurMusicTrack = gMusicPlayer.getCurTrack();

    m_level_command = START_LEVEL;


    // Empty container of the levels so we loaded the ones from our states
    if(!m_Player.empty())
        m_Player.clear();


    Uint32 spriteId = 0;

    for( auto &stateTree : pt.get_child("GameState") )
    {        
        const std::string tag = stateTree.first;
        if(tag == "Player")
        {
            CPlayer loadedPlayer(mpLevelCompleted, mMap, 0 );

            m_Player.push_back(loadedPlayer);

            auto &player = m_Player.back();
            auto &playerTree = stateTree.second;
            player.setupforLevelPlay();

            Uint32 x, y;
            x = playerTree.get<int>("x");
            y = playerTree.get<int>("y");

            player.moveToForce( x, y );

            player.blockedd = playerTree.get<bool>("blockedd", true);
            player.blockedu = playerTree.get<bool>("blockedu", true);
            player.blockedl = playerTree.get<bool>("blockedl", true);
            player.blockedr = playerTree.get<bool>("blockedr", true);

            player.inventory.deserialize( playerTree.get_child("Inventory") );

            player.m_index = m_Player.size()-1;
            player.setDatatoZero();
            player.pdie = 0;
        }
        else if(tag == "SpriteObj")
        {
            // save all the objects states
            auto &spriteTree = stateTree.second;
            object_t type;
            Uint32 x, y;

            int sprVarID = spriteTree.get<int>("<xmlattr>.variant", 0);

            if(spriteId >= mSpriteObjectContainer.size())
            {
              std::unique_ptr<CVorticonSpriteObject> object( new CVorticonSpriteObject( mMap, 0, 0, OBJ_NONE, sprVarID) );
              object->exists = false;
              mSpriteObjectContainer.push_back(move(object));
            }

            CVorticonSpriteObject &spriteObj = *(mSpriteObjectContainer.at(spriteId));

            type = (object_t)(spriteTree.get<int>("type", 0));
            x = spriteTree.get<int>("x", 0);
            y = spriteTree.get<int>("y", 0);

            spriteObj.m_type = type;
            spriteObj.moveToForce(GsVec2D<int>(x,y));
            spriteObj.mIsDead = spriteTree.get<bool>("dead", false);
            spriteObj.onscreen = spriteTree.get<bool>("onscreen", false);
            spriteObj.hasbeenonscreen = spriteTree.get<bool>("hasbeenonscreen", false);
            spriteObj.exists = spriteTree.get<bool>("exists", false);
            spriteObj.blockedd = spriteTree.get<bool>("blockedd", false);
            spriteObj.blockedu = spriteTree.get<bool>("blockedu",false);
            spriteObj.blockedl = spriteTree.get<bool>("blockedl", false);
            spriteObj.blockedr = spriteTree.get<bool>("blockedr", false);
            spriteObj.mHealthPoints = spriteTree.get<int>("HealthPoints", 0);
            spriteObj.canbezapped = spriteTree.get<bool>("canbezapped", true);
            spriteObj.cansupportplayer = spriteTree.get<bool>("cansupportplayer", true);
            spriteObj.inhibitfall = spriteTree.get<bool>("inhibitfall", true);
            spriteObj.honorPriority = spriteTree.get<bool>("honorPriority", true);
            spriteObj.mSpriteIdx = spriteTree.get<int>("sprite", 0);


            spriteObj.performCollisions();

            if(spriteObj.m_type == OBJ_DOOR or
              spriteObj.m_type == OBJ_RAY or
              spriteObj.m_type == OBJ_SNDWAVE or
              spriteObj.m_type == OBJ_FIREBALL or
              spriteObj.m_type == OBJ_ICECHUNK or
              spriteObj.m_type == OBJ_ICEBIT or
              spriteObj.m_type == OBJ_GOTPOINTS or
              spriteObj.m_type == OBJ_ANKHSHIELD) // Some objects are really not needed. So don't load them
            spriteObj.exists = false;

            spriteId++;
        }
        else if(tag == "Map")
        {
            // Save the map_data as it is left
            auto &mapNode = stateTree.second;
            mMap->m_width = mapNode.get<int>("width");
            mMap->m_height = mapNode.get<int>("height");

            const std::string b64text = mapNode.get<std::string>("fgdata");
            base64Decode( reinterpret_cast<gs_byte*>(mMap->getForegroundData()), b64text);
        }
    }

    const std::string b64text = stateNode.get<std::string>("complete");
    base64Decode( reinterpret_cast<gs_byte*>(mpLevelCompleted), b64text);


    // adjust camera settings
    int p_idx = 0;
    for(auto &player : m_Player)
    {
        player.setupCameraObject();
        player.setSpriteVariantId(p_idx);
        player.setSpecialIdx(p_idx);
        player.mpCamera->attachObject(&player);
        player.mpCamera->allowLead(p_idx);
        p_idx++;
    }

    while(m_Player[0].mpCamera->mMoving)
    {
      m_Player[0].mpCamera->process();
      m_Player[0].mpCamera->processEvents();
    }

    mMap->drawAll();

    // Create the special merge effect (Fadeout)
    gEffectController.setupEffect(pColorMergeFX);

    gBehaviorEngine.setNumPlayers(m_Player.size());


    mpObjectAI.reset( new CVorticonSpriteObjectAI(mMap, mSpriteObjectContainer, m_Player,
                              gBehaviorEngine.numPlayers(), m_Episode, m_Level,mMap->m_Dark) );
    setupPlayers();

    mMap->m_Dark = stateNode.get<bool>("dark", false);
    gGraphics.Palette.setdark(mMap->m_Dark);

    m_Player[0].mpCamera->reAdjust();

    return true;
}

bool CPlayGameVorticon::saveXMLGameState()
{
    /// Create tree
    using GsKit::ptree;
    ptree pt;

    /// Create the nodes and store the data as needed
    ptree &stateNode = pt.add("GameState", "");

    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty
    stateNode.put("episode", int(m_Episode));
    stateNode.put("level", m_Level);
    stateNode.put("difficulty", gBehaviorEngine.mDifficulty);    

    // Also the last checkpoint is stored. This is the level entered from map
    // in Commander Keen games
    {
        ptree &chkpnt = stateNode.add("checkpoint", "");        
        chkpnt.put("<xmlattr>.set", m_checkpointset);
        chkpnt.put("<xmlattr>.x", m_checkpoint_x);
        chkpnt.put("<xmlattr>.y", m_checkpoint_y);
    }

    stateNode.put("dark", mMap->m_Dark);

    const unsigned int numPlayers = gBehaviorEngine.numPlayers();

    // Now save every player, especially their inventories
    for( size_t i=0 ; i<numPlayers ; i++ )
    {
        auto &player = m_Player[i];
        ptree &playerNode = stateNode.add("Player", "");
        playerNode.put("<xmlattr>.id", i);
        playerNode.put("<xmlattr>.variant", player.getSpriteVariantIdx());

        playerNode.put("x", player.getXPosition());
        playerNode.put("y", player.getYPosition());
        playerNode.put("blockedd", player.blockedd);
        playerNode.put("blockedu", player.blockedu);
        playerNode.put("blockedl", player.blockedl);
        playerNode.put("blockedr", player.blockedr);

        player.inventory.serialize( playerNode.add("Inventory", "") );
    }

    const size_t size = mSpriteObjectContainer.size();

    // save the number of objects on screen
    for( size_t i=0 ; i<size ; i++ )
    {
        // save all the objects states
        auto &spriteObj = mSpriteObjectContainer[i];
        ptree &spriteNode = stateNode.add("SpriteObj", "");
        spriteNode.put("<xmlattr>.variant", spriteObj->getSpriteVariantIdx());
        spriteNode.put("type", spriteObj->m_type);
        spriteNode.put("x", spriteObj->getXPosition());
        spriteNode.put("y", spriteObj->getYPosition());
        spriteNode.put("dead", spriteObj->mIsDead);
        spriteNode.put("onscreen", spriteObj->onscreen);
        spriteNode.put("hasbeenonscreen", spriteObj->hasbeenonscreen);
        spriteNode.put("exists", spriteObj->exists);
        spriteNode.put("blockedd", spriteObj->blockedd);
        spriteNode.put("blockedu", spriteObj->blockedu);
        spriteNode.put("blockedl", spriteObj->blockedl);
        spriteNode.put("blockedr", spriteObj->blockedr);
        spriteNode.put("HealthPoints", int(spriteObj->mHealthPoints));
        spriteNode.put("canbezapped", spriteObj->canbezapped);
        spriteNode.put("cansupportplayer", spriteObj->cansupportplayer);
        spriteNode.put("inhibitfall", spriteObj->inhibitfall);
        spriteNode.put("honorPriority", spriteObj->honorPriority);
        spriteNode.put("sprite", spriteObj->mSpriteIdx);
    }

    // Save the map_data as it is left       
    {
        ptree &mapNode = stateNode.add("Map", "");
        mapNode.put("width", mMap->m_width);
        mapNode.put("height", mMap->m_height);

        const std::string b64text = base64Encode( reinterpret_cast<gs_byte*>(mMap->getForegroundData()),
                                                    2*mMap->m_width*mMap->m_height);

        mapNode.put("fgdata", b64text);
    }

    stateNode.put("complete", base64Encode( (gs_byte*)(mpLevelCompleted), MAX_LEVELS_VORTICON) );    

    CSaveGameController &savedGame = gSaveGameController;
    savedGame.saveXMLTree(pt);

    return true;
}


bool CPlayGameVorticon::saveGameState()
{	
    // For testing purposes only... for now...
    saveXMLGameState();

    size_t size;

    CSaveGameController &savedGame = gSaveGameController;

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	savedGame.encodeData(m_Episode);
	savedGame.encodeData(m_Level);
	savedGame.encodeData(gBehaviorEngine.mDifficulty);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	savedGame.encodeData(m_checkpointset);
	savedGame.encodeData(m_checkpoint_x);
	savedGame.encodeData(m_checkpoint_y);
	savedGame.encodeData(mMap->m_Dark);

	// Save number of Players
    const unsigned int numPlayers = gBehaviorEngine.numPlayers();
    savedGame.encodeData(numPlayers);

	// Now save the inventory of every player    
    for( size_t i=0 ; i<numPlayers ; i++ )
	{
		savedGame.encodeData(m_Player[i].getXPosition());
		savedGame.encodeData(m_Player[i].getYPosition());
		savedGame.encodeData(m_Player[i].blockedd);
		savedGame.encodeData(m_Player[i].blockedu);
		savedGame.encodeData(m_Player[i].blockedl);
		savedGame.encodeData(m_Player[i].blockedr);
		savedGame.encodeData(m_Player[i].inventory);
	}

	size = mSpriteObjectContainer.size();
	// save the number of objects on screen
	savedGame.encodeData(size);
	for( size_t  i=0 ; i<size ; i++)
	{
		// save all the objects states
		savedGame.encodeData(mSpriteObjectContainer[i]->m_type);
		savedGame.encodeData(mSpriteObjectContainer[i]->getXPosition());
		savedGame.encodeData(mSpriteObjectContainer[i]->getYPosition());
		savedGame.encodeData(mSpriteObjectContainer[i]->mIsDead);
		savedGame.encodeData(mSpriteObjectContainer[i]->onscreen);
		savedGame.encodeData(mSpriteObjectContainer[i]->hasbeenonscreen);
		savedGame.encodeData(mSpriteObjectContainer[i]->exists);
		savedGame.encodeData(mSpriteObjectContainer[i]->blockedd);
		savedGame.encodeData(mSpriteObjectContainer[i]->blockedu);
		savedGame.encodeData(mSpriteObjectContainer[i]->blockedl);
		savedGame.encodeData(mSpriteObjectContainer[i]->blockedr);
		savedGame.encodeData(mSpriteObjectContainer[i]->mHealthPoints);
		savedGame.encodeData(mSpriteObjectContainer[i]->canbezapped);
		savedGame.encodeData(mSpriteObjectContainer[i]->cansupportplayer);
		savedGame.encodeData(mSpriteObjectContainer[i]->inhibitfall);
		savedGame.encodeData(mSpriteObjectContainer[i]->honorPriority);
		savedGame.encodeData(mSpriteObjectContainer[i]->mSpriteIdx);
	}

	// Save the map_data as it is left
	savedGame.encodeData(mMap->m_width);
	savedGame.encodeData(mMap->m_height);
	savedGame.addData( reinterpret_cast<gs_byte*>(mMap->getForegroundData()),
													2*mMap->m_width*mMap->m_height );

	// store completed levels
	savedGame.addData( (gs_byte*)(mpLevelCompleted), MAX_LEVELS_VORTICON );

    return savedGame.save();
}
