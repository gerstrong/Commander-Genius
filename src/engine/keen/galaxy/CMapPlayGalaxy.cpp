/*
 * CMapPlayGalaxy.cpp
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 */

#include "CMapPlayGalaxy.h"
#include "common/ai/CPlayerBase.h"
#include "engine/core/CBehaviorEngine.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "ep5/CMapLoaderGalaxyEp5.h"
#include "ep6/CMapLoaderGalaxyEp6.h"
#include "ep4/ai/CInchWorm.h"
#include "ep4/ai/CFoot.h"
#include "ep4/ai/CSmokePuff.h"
#include "engine/core/VGamepads/vgamepadsimple.h"
#include "fileio/CExeFile.h"
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>

#include <base/interface/property_tree/property_tree.h>
#include <base/interface/Base64.h>

#include "GalaxyEngine.h"

CMapPlayGalaxy::CMapPlayGalaxy(std::vector<CInventory> &inventoryVec) :
mInventoryVec(inventoryVec)
{}

bool CMapPlayGalaxy::init()
{
    mpMap.reset(new CMap());
    return true;
}


void CMapPlayGalaxy::refreshGameplayMap()
{
    const auto scroll = mpMap->getMainScrollCoords();
    gVideoDriver.updateScrollBuffer( scroll.x, scroll.y );
    gVideoDriver.setRefreshSignal(true);
}

void CMapPlayGalaxy::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( const auto ev = std::dynamic_pointer_cast<const EventSpawnObject>(evPtr) )
    {
        std::shared_ptr<CGalaxySpriteObject> obj( static_cast<CGalaxySpriteObject*>(
                            const_cast<CSpriteObject*>(ev->pObject) ) );
        mObjectPtr.push_back( move(obj) );
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventSpawnFoot>(evPtr) ) // Special Case where the Foot is created
    {                                                                                // Episode 4 Secret level
        // kill all the InchWorms in that case, so they can't do any spawning
        for( auto obj=mObjectPtr.rbegin() ; obj!=mObjectPtr.rend() ; obj++ )
        {
            galaxy::CInchWorm *inchworm = dynamic_cast<galaxy::CInchWorm*>(obj->get());
            if( inchworm != nullptr )
            {
                inchworm->exists = false;
            }
        }


        // Create the foot with Smoke Puff
        int posX = ev->x;
        int posY = ev->y-(6<<CSF);
        for( int x=-1 ; x<2 ; x++ )
        {
            for( int y=-1 ; y<2 ; y++ )
            {
                std::shared_ptr<CGalaxySpriteObject> smoke(new galaxy::CSmokePuff( mpMap, posX+(x<<CSF), posY+(y<<CSF), 0 ));
                mObjectPtr.push_back( smoke );
            }
        }

        std::shared_ptr<CGalaxySpriteObject> foot(new galaxy::CFoot( mpMap, ev->foeID, 0x2EF4, posX, posY));
        mObjectPtr.push_back( foot );
    }
    else if( const auto moveBut = std::dynamic_pointer_cast<const EventMoveAllPlayersBut>(evPtr) )
    {
        const auto excp = moveBut->mException;
        const auto target = moveBut->mTarget;

        for( auto obj = mObjectPtr.begin(); obj != mObjectPtr.end() ; obj++)
        {
            if(auto *player = dynamic_cast<galaxy::CPlayerBase*>(obj->get()) )
            {
                if(player->getPlayerIdx() != excp)
                {
                    player->moveToForce(target);
                    player->m_camera.setPosition(target);
                    player->setAction(0); // Reset what they are doing, so no collision fails and strange stuff would happen
                }
            }
        }
    }
    else if( std::dynamic_pointer_cast<const EventReloadMusic>(evPtr) )
    {
        reloadBgMusic();
        gMusicPlayer.play();
    }


    for( auto obj = mObjectPtr.begin(); obj != mObjectPtr.end() ; obj++)
    {
        auto &objRef = *(obj->get());
        objRef.pumpEvent(evPtr);
    }
}

CSpriteObject *CMapPlayGalaxy::getNextStandingStillPlayer()
{
    CSpriteObject *playerPtr = nullptr;

    for( auto obj = mObjectPtr.begin(); obj != mObjectPtr.end() ; obj++)
    {
        if( galaxy::CPlayerBase *player = dynamic_cast<galaxy::CPlayerBase*>(obj->get()) )
        {
            // Player must be alive
            if(!player->exists || player->mIsDead)
                continue;

            // player must be standing still and do nothing
            if(player->isOneOfActionNumbers(0, 5))
            {
                playerPtr = player;
                break;
            }
        }
    }
    return playerPtr;
}



void CMapPlayGalaxy::revivePlayerAt(const int playerIdx, const GsVec2D<int> pos)
{
    galaxy::CPlayerBase *player = nullptr;

    for( auto obj = mObjectPtr.begin(); obj != mObjectPtr.end() ; obj++)
    {
        // If the Player is not only dying, but also lost it's existence, meaning he got out of the screen
        // show the death-message or go gameover.
        player = dynamic_cast<galaxy::CPlayerBase*>(obj->get());
        if( player )
        {
            if(player->getSpriteVariantIdx() == playerIdx)
            {
                break;
            }
        }
    }

    if(player)
    {
        player->moveToForce(pos);
        player->mIsDead = false;
        player->resetMainVars();
        player->setAction(0);
        player->m_camera.allowLead(playerIdx);
        player->blockedd = 1;
    }
}


void CMapPlayGalaxy::stopMusic()
{
    gMusicPlayer.stop();
    mCurMusicTrack = "-1";
}

void CMapPlayGalaxy::playMusic(const int track)
{
    gMusicPlayer.setIMFLoadTrackCallback(imfMusicTrackloader);
    gMusicPlayer.stop();
    if( gMusicPlayer.loadTrack(track) )
    {
        mCurMusicTrack = gMusicPlayer.getCurTrack();
        gMusicPlayer.play();
    }
    else
    {
        gLogging.textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    }
}


void CMapPlayGalaxy::reloadBgMusic()
{
    try{
        gMusicPlayer.setIMFLoadTrackCallback(imfMusicTrackloader);
        gMusicPlayer.load(mCurMusicTrack);
    }
    catch(...) {
        throw "reloadBgMusic: unknown exception.";
    }
}

void CMapPlayGalaxy::ponderBase(const float deltaT)
{
    bool oneInvOpen = false;

    auto &mplayer = gMusicPlayer;
    const auto curTrack = mplayer.getCurTrack();

    if(mCurMusicTrack != curTrack){
        reloadBgMusic();
    }

    // Check if the engine need to be paused
    for( auto &inv : mInventoryVec) {
        oneInvOpen |= inv.showStatus();
    }

    // Animate the tiles of the map
    mpMap->m_animation_enabled = !oneInvOpen;

    if(mMsgBoxOpen) {
        mpMap->m_animation_enabled = true;
    }

    mpMap->animateAllTiles();

    if(!oneInvOpen && !mMsgBoxOpen)
    {
        for( auto obj = mObjectPtr.begin(); obj != mObjectPtr.end() ; obj++)
        {
            auto &objRef = *(obj->get());
            bool visibility = false;

            if( objRef.exists )
            {
                visibility = objRef.calcVisibility();

                if( visibility )
                {
                    // Process the AI of the object as it's given
                    objRef.process();

                    // Check collision between objects using NlogN order
                    auto theOther = obj; theOther++;
                    for( ; theOther != mObjectPtr.end() ; theOther++)
                    {
                        auto &theOtherRef = *(theOther->get());
                        if( !theOtherRef.exists )
                            continue;

                        objRef.isNearby(theOtherRef);
                        theOtherRef.isNearby(objRef);

                        if( objRef.hitdetect(theOtherRef) )
                        {
                            objRef.getTouchedBy(theOtherRef);
                            theOtherRef.getTouchedBy(objRef);
                        }
                    }
                }
            }

            // If the Player is not only dying, but also lost it's existence, meaning he got out of the screen
            // show the death-message or go gameover.
            if( galaxy::CPlayerBase *player = dynamic_cast<galaxy::CPlayerBase*>(obj->get()) )
            {
                if(player->exists)
                {
                    // Special cases, when Keen is god, but still has to die,
                    // kill him anyways.
                    if( player->touchedBottomOfMap() )
                    {
                        player->kill(true);
                    }

                    // Is he really dead?
                    if( player->mIsDead || (!visibility && player->mDying) )
                    {
                        player->processDead();
                    }
                }
                else
                {
                    const auto curPlayerIdx = player->getSpriteVariantIdx();
                    if(gInput.getPressedCommand(curPlayerIdx, IC_CAMLEAD))
                    {
                        gEventManager.add(new RevivePlayer(curPlayerIdx));
                    }
                }
            }

            objRef.processEvents();
        }
    }


}

void CMapPlayGalaxy::render()
{
    auto &vidDrv = gVideoDriver;
    vidDrv.blitScrollSurfaces();

    // Draw all the sprites without player
    // The player sprites are drawn at the end (Reverse order of drawing)
    galaxy::CPlayerBase* player[] = {nullptr, nullptr, nullptr, nullptr};

    int pIt = 0;
    auto objVecSize = mObjectPtr.size();
    for( int ctr = 0 ; ctr < int(objVecSize)  ; ctr++)
    {
        auto &obj = mObjectPtr[objVecSize-ctr-1];

        if( galaxy::CPlayerBase* curplayer = dynamic_cast<galaxy::CPlayerBase*>(obj.get()) )
        {
            player[pIt] = curplayer;
            pIt++;
            continue;
        }

        if(obj->honorPriority )
            obj->draw();
    }

    for(int i=0 ; i<pIt ; i++)
    {
        player[i]->draw();
    }

    // RenderShake Effect
    mpMap->renderShaking();

    // Draw foregroundtiles here!
    mpMap->_drawForegroundTiles();

    for( auto obj=mObjectPtr.rbegin() ;
         obj!=mObjectPtr.rend() ; obj++ )
    {
        if(!(*obj)->honorPriority)
            (*obj)->draw();
    }

    const auto &optHUD = gBehaviorEngine.mOptions[GameOption::HUD];

    if( optHUD.value )
    {
        for(auto &inv : mInventoryVec)
        {
            inv.drawHUD();
        }
    }

}


// Saved GameState stuff for that individual map

void CMapPlayGalaxy::operator>>(CSaveGameController &savedGame)
{
    const Uint16 level = mpMap->getLevel();
    savedGame.encodeData( level );

    std::vector< std::shared_ptr<CGalaxySpriteObject> > filteredObjects;

    // let's filter the Foe out that won't do any good!
    for( auto &it : mObjectPtr )
    {
        if( it->mFoeID != 0 )
        {
            filteredObjects.push_back( it );
        }
    }

    const size_t size = filteredObjects.size();

    // save the number of objects on screen
    savedGame.encodeData(size);

    for( auto &it : filteredObjects )
    {
        // save all the objects states
        int newYpos = it->getYPosition();
        int newXpos = it->getXPosition();

        savedGame.encodeData( it->mFoeID );
        savedGame.encodeData( newXpos );
        savedGame.encodeData( newYpos );
        savedGame.encodeData( it->mIsDead );
        savedGame.encodeData( it->onscreen );
        savedGame.encodeData( it->hasbeenonscreen );
        savedGame.encodeData( it->exists );
        savedGame.encodeData( it->blockedd );
        savedGame.encodeData( it->blockedu );
        savedGame.encodeData( it->blockedl );
        savedGame.encodeData( it->blockedr );
        savedGame.encodeData( it->xDirection );
        savedGame.encodeData( it->yDirection );
        savedGame.encodeData( it->mHealthPoints );
        savedGame.encodeData( it->canbezapped );
        savedGame.encodeData( it->cansupportplayer );
        savedGame.encodeData( it->inhibitfall );
        savedGame.encodeData( it->honorPriority );
        savedGame.encodeData( it->mSpriteIdx );
        savedGame.encodeData( it->m_ActionNumber );
        it->serialize(savedGame);
    }

    // Save the map_data as it is left
    savedGame.encodeData(mpMap->m_width);
    savedGame.encodeData(mpMap->m_height);

    const Uint32 mapSize = mpMap->m_width*mpMap->m_height*sizeof(word);

    savedGame.addData( reinterpret_cast<gs_byte*>(mpMap->getBackgroundData()), mapSize );
    savedGame.addData( reinterpret_cast<gs_byte*>(mpMap->getForegroundData()), mapSize );
    savedGame.addData( reinterpret_cast<gs_byte*>(mpMap->getInfoData()), mapSize );
}



// This is for loading the game
bool CMapPlayGalaxy::operator<<(CSaveGameController &savedGame)
{
    Uint16 level;
    savedGame.decodeData( level );

    std::unique_ptr<galaxy::CMapLoaderGalaxy> mapLoader;
    const auto episode = gBehaviorEngine.getEpisode();

    mpMap.reset(new CMap());

    if(episode == 4)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp4( mObjectPtr, mInventoryVec) );
    }
    else if(episode == 5)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp5( mObjectPtr, mInventoryVec) );
    }
    else if(episode == 6)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp6( mObjectPtr, mInventoryVec, gBehaviorEngine.isDemo()) );
    }
    else
    {
        gLogging.textOut("Error loading the file. This game is not supported!");
        return false;
    }

    // Load the World map level.
    mapLoader->loadMap( mpMap, level );

    // Load the Background Music
    gMusicPlayer.stop();

    if( !galaxy::loadLevelMusic(level) )
    {
        gLogging.textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    }
    else
    {
        mCurMusicTrack = gMusicPlayer.getCurTrack();
        gMusicPlayer.play();
    }


    // load the number of objects on screen
    Uint32 size;
    Uint32 x, y;
    Uint16 foeID;
    uint16_t actionNumber;
    savedGame.decodeData(size);

    // Now load the previously created objects

    if(!mObjectPtr.empty())
        mObjectPtr.clear();

    mpMap->mNumFuses = 0;
    mpMap->mFuseInLevel = false;

    for( Uint32 i=0 ; i<size ; i++ )
    {
        savedGame.decodeData(foeID);
        savedGame.decodeData(x);
        savedGame.decodeData(y);

        auto pNewfoe = mapLoader->addFoe(mpMap, foeID, x, y);

        if(!pNewfoe)
        {
            pNewfoe.reset(new CGalaxySpriteObject(mpMap, foeID, x, y, 0));
        }

        savedGame.decodeData( pNewfoe->mIsDead );
        savedGame.decodeData( pNewfoe->onscreen );
        savedGame.decodeData( pNewfoe->hasbeenonscreen );
        savedGame.decodeData( pNewfoe->exists );
        savedGame.decodeData( pNewfoe->blockedd );
        savedGame.decodeData( pNewfoe->blockedu );
        savedGame.decodeData( pNewfoe->blockedl );
        savedGame.decodeData( pNewfoe->blockedr );
        savedGame.decodeData( pNewfoe->xDirection );
        savedGame.decodeData( pNewfoe->yDirection );
        savedGame.decodeData( pNewfoe->mHealthPoints );
        savedGame.decodeData( pNewfoe->canbezapped );
        savedGame.decodeData( pNewfoe->cansupportplayer );
        savedGame.decodeData( pNewfoe->inhibitfall );
        savedGame.decodeData( pNewfoe->honorPriority );
        savedGame.decodeData( pNewfoe->mSpriteIdx );
        savedGame.decodeData( actionNumber );
        pNewfoe->deserialize(savedGame);

        if(pNewfoe->exists)
        {
            pNewfoe->setActionForce(actionNumber);
            std::shared_ptr<CGalaxySpriteObject> newFoe(pNewfoe);
            mObjectPtr.push_back(newFoe);
        }
    }

    // Save the map_data as it is left
    savedGame.decodeData(mpMap->m_width);
    savedGame.decodeData(mpMap->m_height);

    savedGame.readDataBlock( reinterpret_cast<gs_byte*>(mpMap->getBackgroundData()) );
    savedGame.readDataBlock( reinterpret_cast<gs_byte*>(mpMap->getForegroundData()) );
    savedGame.readDataBlock( reinterpret_cast<gs_byte*>(mpMap->getInfoData()) );

    if( mpMap->m_width * mpMap->m_height > 0 )
    {
        mpMap->drawAll();
    }

    return true;
}


// Saves the level using the Savegamecontroller in XML.
void CMapPlayGalaxy::operator>>(GsKit::ptree &levelNode)
{
    // Coding here
    const Uint16 level = mpMap->getLevel();
    levelNode.put("level", level);

    std::vector< std::shared_ptr<CGalaxySpriteObject> > filteredObjects;

    // let's filter the Foe out that won't do any good, by having an invalid foe id
    for( auto &it : mObjectPtr )
    {
        if( it->mFoeID != 0 )
        {
            filteredObjects.push_back( it );
        }
    }

    const size_t size = filteredObjects.size();

    // save the number of objects on screen
    levelNode.put("NumSprites", size);

    for( auto &it : filteredObjects )
    {
        auto &spriteNode = levelNode.add("Sprite", "");

        // save all the objects states
        const auto newYpos = it->getYPosition();
        const auto newXpos = it->getXPosition();

        spriteNode.put("<xmlattr>.id", it->mFoeID);
        spriteNode.put("<xmlattr>.variant", it->getSpriteVariantIdx());
        spriteNode.put("<xmlattr>.x", newXpos);
        spriteNode.put("<xmlattr>.y", newYpos);

        spriteNode.put("dead", it->mIsDead);
        spriteNode.put("onscreen", it->onscreen);
        spriteNode.put("hasbeenonscreen", it->hasbeenonscreen);
        spriteNode.put("exists", it->exists);
        spriteNode.put("blockedd", it->blockedd);
        spriteNode.put("blockedu", it->blockedu);
        spriteNode.put("blockedl", it->blockedl);
        spriteNode.put("blockedr", it->blockedr);
        spriteNode.put("xDirection", it->xDirection);
        spriteNode.put("yDirection", it->yDirection);
        spriteNode.put("health", (int)it->mHealthPoints);
        spriteNode.put("canbezapped", it->canbezapped);
        spriteNode.put("cansupportplayer", it->cansupportplayer);
        spriteNode.put("inhibitfall", it->inhibitfall);
        spriteNode.put("honorPriority", it->honorPriority);
        spriteNode.put("spritePic", it->mSpriteIdx);
        spriteNode.put("Actionumber", it->m_ActionNumber);
        it->serialize(spriteNode);
    }

    // Save the map_data as it is left
    {
        auto &mapNode = levelNode.put("Map", "");
        mapNode.put("width", mpMap->m_width);
        mapNode.put("height", mpMap->m_height);

        const Uint32 mapSize = mpMap->m_width*mpMap->m_height*sizeof(word);

        const std::string b64textBG   = base64Encode( reinterpret_cast<gs_byte*>(mpMap->getBackgroundData()), mapSize);
        const std::string b64textFG   = base64Encode( reinterpret_cast<gs_byte*>(mpMap->getForegroundData()), mapSize);
        const std::string b64textInfo = base64Encode( reinterpret_cast<gs_byte*>(mpMap->getInfoData()), mapSize);

        mapNode.put("bgdata", b64textBG);
        mapNode.put("fgdata", b64textFG);
        mapNode.put("infodata", b64textInfo);
    }
}

// This is for loading the game
void CMapPlayGalaxy::operator<<(GsKit::ptree &levelNode)
{
    int level = levelNode.get<int>("level", 0);

    init();

    std::unique_ptr<galaxy::CMapLoaderGalaxy> mapLoader;
    const unsigned int episode = gBehaviorEngine.getEpisode();

    if(episode == 4)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp4( mObjectPtr, mInventoryVec) );
    }
    else if(episode == 5)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp5( mObjectPtr, mInventoryVec) );
    }
    else if(episode == 6)
    {
        mapLoader.reset( new galaxy::CMapLoaderGalaxyEp6( mObjectPtr, mInventoryVec, gBehaviorEngine.isDemo() ) );
    }
    else
    {
        gLogging.textOut("Error loading the file. This game is not supported!");
        return;
    }

    // Load the World map level.
    mapLoader->loadMap( mpMap, level );

    gMusicPlayer.stop();

    // Prepare to load the Background Music
    if( !galaxy::loadLevelMusic(level) )
    {
        gLogging.textOut("Warning: The music cannot be played. "
                         "Check that all the files have been correctly copied!");
    }
    else
    {
        mCurMusicTrack = gMusicPlayer.getCurTrack();
        gMusicPlayer.play();
    }

    // load the number of objects on screen
    Uint16 foeID;

    // Now load the previously created objects
    if(!mObjectPtr.empty())
        mObjectPtr.clear();

    mapLoader->resetNumLoadedPlayers();

    mpMap->mNumFuses = 0;
    mpMap->mFuseInLevel = false;

    gLogging << "Restoring sprite states." << CLogFile::endl;

    for( auto &levelItem : levelNode )
    {
        if(levelItem.first != "Sprite")
            continue;

        auto &spriteNode = levelItem.second;

        foeID = spriteNode.get<int>("<xmlattr>.id");
        int sprVarID = spriteNode.get<int>("<xmlattr>.variant", 0);
        const auto x = spriteNode.get<int>("<xmlattr>.x", 0);
        const auto y = spriteNode.get<int>("<xmlattr>.y", 0);

        if(x < 0 || y < 0)
        {
            gLogging << "Warning: Ignoring Position (" << x << ","
                     << y << ") of foe " << foeID << "." << CLogFile::endl;

            continue;
        }

        auto pNewfoe = mapLoader->addFoe(mpMap, foeID, x, y);

        if(pNewfoe == nullptr)
        {
            gLogging << "Warning: Creating Dummy object of unknown type(" << x << ","
                     << y << ") of foe " << foeID << ". For some reason your savestate has it." << CLogFile::endl;

            pNewfoe.reset(new CGalaxySpriteObject(mpMap, foeID, x, y, sprVarID));
        }

        pNewfoe->mIsDead = spriteNode.get<bool>("dead", false);
        pNewfoe->onscreen = spriteNode.get<bool>("onscreen", false);
        pNewfoe->hasbeenonscreen = spriteNode.get<bool>("hasbeenonscreen", false);
        pNewfoe->exists = spriteNode.get<bool>("exists", false);
        pNewfoe->blockedd = spriteNode.get<bool>("blockedd", false);
        pNewfoe->blockedu = spriteNode.get<bool>("blockedu", false);
        pNewfoe->blockedl = spriteNode.get<bool>("blockedl", false);
        pNewfoe->blockedr = spriteNode.get<bool>("blockedr", false);
        pNewfoe->xDirection = spriteNode.get<int>("xDirection", 0);
        pNewfoe->yDirection = spriteNode.get<int>("yDirection", 0);
        pNewfoe->mHealthPoints = spriteNode.get<int>("health", 1);
        pNewfoe->canbezapped = spriteNode.get<bool>("canbezapped", false);
        pNewfoe->cansupportplayer = spriteNode.get<bool>("cansupportplayer", false);
        pNewfoe->inhibitfall = spriteNode.get<bool>("inhibitfall", false);
        pNewfoe->honorPriority = spriteNode.get<bool>("honorPriority", false);
        pNewfoe->mSpriteIdx = spriteNode.get<int>("spritePic", 1);
        const Uint16 actionNumber = spriteNode.get<int>("Actionumber", 1);
        pNewfoe->deserialize(spriteNode);

        if(pNewfoe->exists)
        {
            pNewfoe->setActionForce(actionNumber);
            mObjectPtr.push_back(pNewfoe);
        }
    }

    gLogging << "Restoring World Map status." << CLogFile::endl;

    // Save the map_data as it is left
    {
        auto &mapNode = levelNode.get_child("Map");
        mpMap->m_width = mapNode.get<int>("width");
        mpMap->m_height = mapNode.get<int>("height");

        const std::string b64textBG   = mapNode.get<std::string>("bgdata");
        const std::string b64textFG   = mapNode.get<std::string>("fgdata");
        const std::string b64textInfo = mapNode.get<std::string>("infodata");

        base64Decode(reinterpret_cast<gs_byte*>
                     (mpMap->getBackgroundData()), b64textBG);
        base64Decode(reinterpret_cast<gs_byte*>
                     (mpMap->getForegroundData()), b64textFG);
        base64Decode(reinterpret_cast<gs_byte*>
                     (mpMap->getInfoData()), b64textInfo);
    }

    if( mpMap->m_width * mpMap->m_height > 0 )
    {
        mpMap->drawAll();
    }
}


