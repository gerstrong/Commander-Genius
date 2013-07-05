/*
 * CPlayGameGameState.cpp
 *
 *  Created on: 20.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include "StringUtils.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/CVideoDriver.h"
#include "common/CVorticonMapLoader.h"
#include "Base64.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

///////////////////////////
// Game State Management //
///////////////////////////
bool CPlayGameVorticon::loadGameState()
{
	CSaveGameController &savedGame = *(gpSaveGameController);
	
	bool ok = true;

	// This fills the datablock from CSavedGame object
	if(!savedGame.load())
	{
	  return false;
	}
	
	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);
	
	// Prepare for loading the new level map and the players.
	cleanup();
	
	// get the episode, level and difficulty
	char newLevel;
	ok &= savedGame.decodeData(m_Episode);
	ok &= savedGame.decodeData(newLevel);
	
	bool loadmusic = ( m_Level != newLevel || m_Level == 80 );
	m_Level = newLevel;
	
	ok &= savedGame.decodeData(g_pBehaviorEngine->mDifficulty);
	
	bool dark, checkpointset;
	int checkx, checky;
	ok &= savedGame.decodeData(checkpointset);
	ok &= savedGame.decodeData(checkx);
	ok &= savedGame.decodeData(checky);
	ok &= savedGame.decodeData(dark);
	
	// Load number of Players
	ok &= savedGame.decodeData(m_NumPlayers);
	
	if(!m_Player.empty())
	  m_Player.clear();
	
	m_Player.assign(m_NumPlayers, CPlayer(m_Episode, m_Level,
					      mp_level_completed, *mMap.get() ) );
	for( size_t i=0 ; i < m_Player.size() ; i++ )
	{
	  m_Player.at(i).m_index = i;
	  m_Player.at(i).setDatatoZero();
	}
	
	CVorticonMapLoaderWithPlayer Maploader(mMap, m_Player, mSpriteObjectContainer);
	m_checkpointset = checkpointset;
	Maploader.m_checkpointset = m_checkpointset;
	if(!Maploader.load(m_Episode, m_Level, m_Gamepath, loadmusic, false))
	  return false;
	
	m_checkpoint_x = checkx;
	m_checkpoint_y = checky;
	
	m_level_command = START_LEVEL;
	
	std::vector<CPlayer> :: iterator player;
	for( player=m_Player.begin() ; player != m_Player.end() ; player++ ) 
	{
	  int x, y;
	  player->setupforLevelPlay();
	  ok &= savedGame.decodeData(x);
	  ok &= savedGame.decodeData(y);
	  player->moveToForce(VectorD2<int>(x,y));
	  ok &= savedGame.decodeData(player->blockedd);
	  ok &= savedGame.decodeData(player->blockedu);
	  ok &= savedGame.decodeData(player->blockedl);
	  ok &= savedGame.decodeData(player->blockedr);
	  ok &= savedGame.decodeData(player->inventory);
	  player->pdie = 0;
	}
	
	// load the number of objects on screen
	Uint32 size;
	ok &= savedGame.decodeData(size);
	for( Uint32 i=0 ; i<size  ; i++ )
	{
	  unsigned int x,y;
	  
	  if(i >= mSpriteObjectContainer.size())
	  {
	    std::unique_ptr<CVorticonSpriteObject> object( new CVorticonSpriteObject( mMap.get(), 0, 0, OBJ_NONE) );
	    object->exists = false;
	    mSpriteObjectContainer.push_back(move(object));
	  }
	  
	  CVorticonSpriteObject &object = *(mSpriteObjectContainer.at(i));
	  
	  ok &= savedGame.decodeData(object.m_type);
	  ok &= savedGame.decodeData(x);
	  ok &= savedGame.decodeData(y);
	  object.moveToForce(VectorD2<int>(x,y));
	  ok &= savedGame.decodeData(object.dead);
	  ok &= savedGame.decodeData(object.onscreen);
	  ok &= savedGame.decodeData(object.hasbeenonscreen);
	  ok &= savedGame.decodeData(object.exists);
	  ok &= savedGame.decodeData(object.blockedd);
	  ok &= savedGame.decodeData(object.blockedu);
	  ok &= savedGame.decodeData(object.blockedl);
	  ok &= savedGame.decodeData(object.blockedr);
	  ok &= savedGame.decodeData(object.mHealthPoints);
	  ok &= savedGame.decodeData(object.canbezapped);
	  ok &= savedGame.decodeData(object.cansupportplayer);
	  ok &= savedGame.decodeData(object.inhibitfall);
	  ok &= savedGame.decodeData(object.honorPriority);
	  ok &= savedGame.decodeData(object.sprite);
	  object.performCollisions();
	  
	  if(object.m_type == OBJ_DOOR or
	    object.m_type == OBJ_RAY or
	    object.m_type == OBJ_SNDWAVE or
	    object.m_type == OBJ_FIREBALL or
	    object.m_type == OBJ_ICECHUNK or
	    object.m_type == OBJ_ICEBIT or
	    object.m_type == OBJ_GOTPOINTS or
	    object.m_type == OBJ_ANKHSHIELD) // Some objects are really not needed. So don't load them
	  object.exists = false;
	}
	
	// TODO: An algorithm for comparing the number of players saved and we actually have need to be in sync
	
	// Load the map_data as it was left last
	ok &= savedGame.decodeData(mMap->m_width);
	ok &= savedGame.decodeData(mMap->m_height);
	ok &= savedGame.readDataBlock( reinterpret_cast<byte*>(mMap->getForegroundData()) );
	
	// Load completed levels
	ok &= savedGame.readDataBlock( (byte*)(mp_level_completed) );
	
	m_Player[0].setMapData(mMap.get());
	m_Player[0].setupCameraObject();
	m_Player[0].mpCamera->attachObject(&m_Player[0]);
	
	while(m_Player[0].mpCamera->m_moving)
	{
	  m_Player[0].mpCamera->process();
	  m_Player[0].mpCamera->processEvents();
	}
	
	mMap->drawAll();
	
	// Create the special merge effect (Fadeout)
	g_pGfxEngine->setupEffect(pColorMergeFX);
	
	
	mpObjectAI.reset( new CVorticonSpriteObjectAI(mMap.get(), mSpriteObjectContainer, m_Player,
						      m_NumPlayers, m_Episode, m_Level,
					       mMap->m_Dark) );
	setupPlayers();
	
	mMap->m_Dark = dark;
	g_pGfxEngine->Palette.setdark(mMap->m_Dark);
	
	
	
	// left-right scrolling
	/*for(int scrollPos = 0 ; scrollPos<320 ; scrollPos++ )
	{
	  mMap->scrollRight();	  
	  mMap->scrollDown();
	}	
	for(int scrollPos = 0 ; scrollPos<320 ; scrollPos++ )
	{
	  mMap->scrollLeft();	  
	  mMap->scrollUp();
	}	*/
	
	
	m_Player[0].mpCamera->reAdjust();
	
	
	return ok;
}

bool CPlayGameVorticon::saveXMLGameState()
{
    /// Create tree
    using boost::property_tree::ptree;
    ptree pt;

    /// Create the nodes and store the data as needed
    ptree &stateNode = pt.add("GameState", "");

    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty
    stateNode.put("episode", int(m_Episode));
    stateNode.put("level", m_Level);
    stateNode.put("difficulty", g_pBehaviorEngine->mDifficulty);

    // Also the last checkpoint is stored. This is the level entered from map
    // in Commander Keen games
    {
        ptree &chkpnt = stateNode.add("checkpoint", "");
        chkpnt.put("<xmlattr>.x", m_checkpoint_x);
        chkpnt.put("<xmlattr>.y", m_checkpoint_y);
    }

    stateNode.put("dark", mMap->m_Dark);

    // Now save the inventory of every player
    for( size_t i=0 ; i<m_NumPlayers ; i++ )
    {
        auto &player = m_Player[i];
        ptree &playerNode = stateNode.add("Player", "");
        playerNode.put("<xmlattr>.id", i);

        playerNode.put("x", player.getXPosition());
        playerNode.put("y", player.getYPosition());
        playerNode.put("blockedd", player.blockedd);
        playerNode.put("blockedu", player.blockedu);
        playerNode.put("blockedl", player.blockedl);
        playerNode.put("blockedr", player.blockedr);
        playerNode.put("blockedr", player.blockedr);

        player.inventory.serialize( playerNode.add("Player", "") );
    }

    const size_t size = mSpriteObjectContainer.size();

    // save the number of objects on screen
    for( size_t i=0 ; i<size ; i++ )
    {
        // save all the objects states
        auto &spriteObj = mSpriteObjectContainer[i];
        ptree &spriteNode = stateNode.add("SpriteObj", "");
        spriteNode.put("type", spriteObj->m_type);
        spriteNode.put("x", spriteObj->getXPosition());
        spriteNode.put("y", spriteObj->getYPosition());
        spriteNode.put("dead", spriteObj->dead);
        spriteNode.put("onscreen", spriteObj->onscreen);
        spriteNode.put("hasbeenonscreen", spriteObj->hasbeenonscreen);
        spriteNode.put("exists", spriteObj->exists);
        spriteNode.put("blockedd", spriteObj->blockedd);
        spriteNode.put("blockedu", spriteObj->blockedu);
        spriteNode.put("blockedl", spriteObj->blockedl);
        spriteNode.put("blockedr", spriteObj->blockedr);
        spriteNode.put("HealthPoints", spriteObj->mHealthPoints);
        spriteNode.put("canbezapped", spriteObj->canbezapped);
        spriteNode.put("cansupportplayer", spriteObj->cansupportplayer);
        spriteNode.put("inhibitfall", spriteObj->inhibitfall);
        spriteNode.put("honorPriority", spriteObj->honorPriority);
        spriteNode.put("sprite", spriteObj->sprite);
    }

    // Save the map_data as it is left       
    {
        ptree &mapNode = stateNode.add("Map", "");
        mapNode.put("width", mMap->m_width);
        mapNode.put("height", mMap->m_height);

        const std::string b64text = base64Encode( reinterpret_cast<byte*>(mMap->getForegroundData()),
                                                    2*mMap->m_width*mMap->m_height);

        mapNode.put("fgdata", b64text);
    }



    /*savedGame.addData( reinterpret_cast<byte*>(mMap->getForegroundData()),
                                                    2*mMap->m_width*mMap->m_height );

    // store completed levels
    savedGame.addData( (byte*)(mp_level_completed), MAX_LEVELS_VORTICON );
    */

    // Write the xml-file
    std::ofstream os("/home/gerstrong/keensavetest.xml");
    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
    //write_xml( "/home/gerstrong/keensavetest.xml", pt, std::locale(), settings);

    write_xml( os, pt, settings );

    return true;
}

bool CPlayGameVorticon::saveGameState()
{	
    // For testing purposes only... for now...
    saveXMLGameState();

    size_t size;

	CSaveGameController &savedGame = *(gpSaveGameController);

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	savedGame.encodeData(m_Episode);
	savedGame.encodeData(m_Level);
	savedGame.encodeData(g_pBehaviorEngine->mDifficulty);

	// Also the last checkpoint is stored. This is the level entered from map
	// in Commander Keen games
	savedGame.encodeData(m_checkpointset);
	savedGame.encodeData(m_checkpoint_x);
	savedGame.encodeData(m_checkpoint_y);
	savedGame.encodeData(mMap->m_Dark);

	// Save number of Players
	savedGame.encodeData(m_NumPlayers);

	// Now save the inventory of every player
	for( size_t i=0 ; i<m_NumPlayers ; i++ )
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
		savedGame.encodeData(mSpriteObjectContainer[i]->dead);
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
		savedGame.encodeData(mSpriteObjectContainer[i]->sprite);
	}

	// Save the map_data as it is left
	savedGame.encodeData(mMap->m_width);
	savedGame.encodeData(mMap->m_height);
	savedGame.addData( reinterpret_cast<byte*>(mMap->getForegroundData()),
													2*mMap->m_width*mMap->m_height );

	// store completed levels
	savedGame.addData( (byte*)(mp_level_completed), MAX_LEVELS_VORTICON );

	return savedGame.save();
}
