/*
 * CVarPlatform.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: gerstrong
 */

#include "CVarPlatform.h"
#include <base/video/CVideoDriver.h>
#include <base/interface/property_tree/property_tree.h>
#include <graphics/GsGraphics.h>
#include <engine/core/CBehaviorEngine.h>
#include <fileio/KeenFiles.h>

namespace galaxy {


const int MOVE_SPEED = 20;

const int GIK_SPRITE = 379;

CVarPlatform::CVarPlatform(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y,
                           const direction_t horidir,
                           const direction_t vertdir,
                           const int actionOffset, const int sprVar) :
    CGalaxySpriteObject(pMap, foeID, x, y, sprVar),
    CPlatform(pMap, foeID, x, y),    
    CMoveTarget(m_Pos, xDirection, yDirection),
    m_GikSprite(GIK_SPRITE)
{
    m_ActionBaseOffset = actionOffset;

    setActionForce(A_PLATFORM_MOVE);
    setActionSprite();

    xDirection = horidir;
    yDirection = vertdir;

    const int episode = gBehaviorEngine.getEpisode();

    if(episode == 6 && !gKeenFiles.exeFile.isDemo() )
        drawGik = true;
    else
        drawGik = false;

    solid = false; // I think they must be false, because some plats like those in Keen6
    // should only obey to the set markers

    calcBoundingBoxes();
}


void
CVarPlatform::
deserialize(CSaveGameController &savedGame)
{
    savedGame.decodeData(target.x);
    savedGame.decodeData(target.y);
}

void
CVarPlatform::
serialize(CSaveGameController &savedGame)
{
    savedGame.encodeData(target.x);
    savedGame.encodeData(target.y);
}

void
CVarPlatform::
serialize(GsKit::ptree &node)
{
    auto &posNode = node.put("target", "");
    posNode.put("<xmlattr>.x", target.x);
    posNode.put("<xmlattr>.y", target.y);
}

void
CVarPlatform::
deserialize(GsKit::ptree &node)
{
    auto &posNode = node.get_child("target");
    target.x = posNode.get<int>("<xmlattr>.x");
    target.y = posNode.get<int>("<xmlattr>.y");
}



void CVarPlatform::process()
{
    int xBlockPos = target.x - getXPosition();
    int yBlockPos = target.y - getYPosition();
    
    const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
    const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;
    
    if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
    {
        const Uint16 object = mpMap->getPlaneDataAt(2, target.x, target.y);

        GsVec2D<int> speed(xBlockPos, yBlockPos);
        movePlat(speed);

        readDirection(object, xDirection, yDirection );
        detectNextTarget(target, xDirection, yDirection);
    }
    
    if(yDirection == UP && blockedu)
        yDirection = DOWN;
    else if(yDirection == DOWN && blockedd)
        yDirection = UP;
    
    if(xDirection == RIGHT && blockedr)
        xDirection = LEFT;
    else if(xDirection == LEFT && blockedl)
        xDirection = RIGHT;
    
    GsVec2D<int> speed;
    
    if(yDirection == UP)
    {
        speed.y = -MOVE_SPEED;
    }
    else if(yDirection == DOWN)
    {
        speed.y = MOVE_SPEED;
    }
    
    
    if(xDirection == RIGHT)
    {
        speed.x = MOVE_SPEED;
    }
    else if(xDirection == LEFT)
    {
        speed.x = -MOVE_SPEED;
    }
    
    movePlat(speed);
    
    CPlatform::process();
}


void CVarPlatform::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( dontdraw )
        return;

    GsSprite &Sprite = gGraphics.getSprite(mSprVar,mSpriteIdx);

    const int sprW = Sprite.getWidth();
    const int sprH = Sprite.getHeight();

    const auto scroll = mpMap->getMainScrollCoords();

    scrx = (m_Pos.x>>STC)-scroll.x;
    scry = (m_Pos.y>>STC)-scroll.y;

    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        int showX = scrx+Sprite.getXOffset();
        int showY = scry+Sprite.getYOffset();

        // Only in Episode 6
        if(drawGik)
        {
            auto gikSpr = m_GikSprite;

            if(yDirection == CENTER && xDirection == LEFT)
                gikSpr = m_GikSprite;
            else if(yDirection == UP && xDirection == LEFT)
                gikSpr = m_GikSprite+1;
            else if(yDirection == UP && xDirection == CENTER)
                gikSpr = m_GikSprite+2;
            else if(yDirection == UP && xDirection == RIGHT)
                gikSpr = m_GikSprite+3;
            else if(yDirection == CENTER && xDirection == RIGHT)
                gikSpr = m_GikSprite+4;
            else if(yDirection == DOWN && xDirection == RIGHT)
                gikSpr = m_GikSprite+5;
            else if(yDirection == DOWN && xDirection == CENTER)
                gikSpr = m_GikSprite+6;
            else if(yDirection == DOWN && xDirection == LEFT)
                gikSpr = m_GikSprite+7;


            GsSprite &gikSprite = gGraphics.getSprite(mSprVar, gikSpr);

            const int w = gikSprite.getWidth();
            const int h = gikSprite.getHeight();

            gikSprite.drawSprite(showX+16, showY+16, w, h);

            Sprite.drawSprite( showX, showY, sprW, sprH, (255-transluceny) );
        }
        else
        {
            Sprite.drawSprite( showX, showY, sprW, sprH, (255-transluceny) );
        }



        for( auto &player : mCarriedPlayerVec)
        {
            if(!player)
                continue;

            GsSprite &playSprite = gGraphics.getSprite(player->getSpriteVariantIdx(),player->mSpriteIdx);
            int distx = player->getXPosition()-getXPosition();
            int disty = player->getYPosition()-getYPosition();

            distx = (distx>>STC);
            distx += (playSprite.getXOffset()-Sprite.getXOffset());
            disty = (disty>>STC);
            disty += (playSprite.getYOffset()-Sprite.getYOffset());

            const int playW = playSprite.getWidth();
            const int playH = playSprite.getHeight();

            playSprite.drawSprite( showX+distx, showY+disty, playW, playH );
        }

        hasbeenonscreen = true;
    }
}

}

