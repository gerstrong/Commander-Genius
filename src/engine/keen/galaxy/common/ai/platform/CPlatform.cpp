/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: Gerstrong and FCTW
 */

#include "CPlatform.h"
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	m_ActionBaseOffset = 0x316A;
}

void CPlatform::movePlatX(const int amnt)
{
	if(amnt > 0)
		movePlatRight(amnt);
	else if(amnt < 0)
		movePlatLeft(-amnt);
}

void CPlatform::movePlatY(const int amnt)
{
	if(amnt > 0)
		movePlatDown(amnt);
	else if(amnt < 0)
		movePlatUp(-amnt);
}

std::vector<CSpriteObject*> CPlatform::fetchCarryingPlayer()
{
    std::vector<CSpriteObject*> carriedObjVec;

    for(auto &player : mCarriedPlayerVec)
    {
        if(player == nullptr)
            continue;

        if(!player->dying || !player->dead)
            carriedObjVec.push_back(player);
    }

    return carriedObjVec;
}

void CPlatform::movePlatLeft(const int amnt)
{
    // If the Player is standing on the plat move him with it!
	if(amnt <= 0)
	    return;

    if(!mCarriedPlayerVec.empty())
	{
        std::vector<CSpriteObject*> carriedObjVec = fetchCarryingPlayer();        
        mMoveTasks.push_back(new ObjMoveCouples(-amnt, 0, carriedObjVec));
        return;
	}		    
	moveLeft(amnt);
}

void CPlatform::movePlatRight(const int amnt)
{
	// If the Player is standing on the plat move him with it!
	if(amnt <= 0)
	    return;

    if(!mCarriedPlayerVec.empty())
    {
        std::vector<CSpriteObject*> carriedObjVec = fetchCarryingPlayer();
        mMoveTasks.push_back(new ObjMoveCouples(amnt,0, carriedObjVec));
        return;
    }


	// Now move the platform itself.
	moveRight(amnt);
}

void CPlatform::movePlatUp(const int amnt)
{
	// First move the object on platform if any
    if(!mCarriedPlayerVec.empty())
    {
        std::vector<CSpriteObject*> carriedObjVec = fetchCarryingPlayer();
        mMoveTasks.push_back(new ObjMoveCouples(0,-amnt, carriedObjVec));
        return;
    }

	// Now move the platform itself.
	moveUp(amnt);
}

void CPlatform::movePlatDown(const int amnt)
{
	// First move the object on platform if any
    if(!mCarriedPlayerVec.empty())
    {
        std::vector<CSpriteObject*> carriedObjVec = fetchCarryingPlayer();
        mMoveTasks.push_back(new ObjMoveCouples(0, amnt, carriedObjVec));
        return;
    }

	// Now move the platform itself.
	moveDown(amnt);
}

void CPlatform::movePlat(const Vector2D<int> &speed)
{
    movePlatX(speed.x);
    movePlatY(speed.y);    
}

void CPlatform::process()
{
	// check if someone is still standing on the platform

    for(auto &carriedObj : mCarriedPlayerVec)
	{
        if(carriedObj == nullptr)
            continue;

        CPlayerLevel* carriedPlayer = dynamic_cast<CPlayerLevel*>(carriedObj);

        if(!hitdetect(*carriedPlayer) || carriedPlayer->blockedu )
		{
            carriedPlayer->pSupportedbyobject = nullptr;
            carriedPlayer->m_jumpdownfromobject = false;
            carriedPlayer->dontdraw = false;
            carriedObj = nullptr;
		}
        else if(!carriedPlayer->m_jumpdownfromobject)
		{
            if(carriedPlayer->getActionNumber(A_KEEN_STAND) || carriedPlayer->getActionNumber(A_KEEN_ON_PLAT))
		    {
			// Check that he correctly stands on the platform
			const unsigned int standY = getYUpPos()+1;
			
            if( standY > carriedPlayer->getYDownPos() )
                carriedPlayer->moveDown((standY-carriedPlayer->getYDownPos())/2 + 1);
            else if( standY < carriedPlayer->getYDownPos() )
                carriedPlayer->moveUp((carriedPlayer->getYDownPos()-standY)/2 + 1);
		    }    
		}
	}

    if(!mCarriedPlayerVec.empty())
    {
        if(mCarriedPlayerVec.back() == nullptr)
            mCarriedPlayerVec.pop_back();
    }

	processActionRoutine();
}

void CPlatform::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		const int m_py2 = player->getYDownPos();
		const int m_y2 = getYUpPos()+(4<<STC);
		if( m_py2 <= m_y2 && !player->pSupportedbyobject && !player->m_jumpdownfromobject )
		{
            bool playerInList = false;

            for(auto &carriedObj : mCarriedPlayerVec)
            {
                if(player == carriedObj)
                {
                    playerInList = true;
                    break;
                }
            }

            if(!playerInList)
            {
                mCarriedPlayerVec.push_back(player);
                player->pSupportedbyobject = this;
                player->dontdraw = true;
            }

		}
	}
}


void CPlatform::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( sprite == BLANKSPRITE || dontdraw )
        return;
    
    GsSprite &Sprite = gGraphics.getSprite(mSprVar,sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        int showX = scrx+Sprite.getXOffset();
        int showY = scry+Sprite.getYOffset();

        auto visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

        if( showX+Sprite.getWidth() < visGA.x || showX > visGA.x+visGA.w )
            return;

        if( showY+Sprite.getHeight() < visGA.y || showY > visGA.y+visGA.h )
            return;

        int w = Sprite.getWidth();
        int h = Sprite.getHeight();

        Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );

        for(auto &carriedObj : mCarriedPlayerVec)
        {
            if(carriedObj)
            {
                GsSprite &playSprite = gGraphics.getSprite(carriedObj->getSpriteVariantId(),carriedObj->sprite);
                int distx = carriedObj->getXPosition()-getXPosition();
                int disty = carriedObj->getYPosition()-getYPosition();

                distx = (distx>>STC);
                distx += (playSprite.getXOffset()-Sprite.getXOffset());
                disty = (disty>>STC);
                disty += (playSprite.getYOffset()-Sprite.getYOffset());

                int playW = playSprite.getWidth();
                int playH = playSprite.getHeight();

                playSprite.drawSprite( showX+distx, showY+disty, playW, playH );
            }
        }

        hasbeenonscreen = true;
    }
}

bool CPlatform::calcVisibility()
{
    return true;
}


}
