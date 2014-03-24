#include "CCarrier.h"
#include <base/video/CVideoDriver.h>

CCarrier::CCarrier(CMap* pmap, Uint32 x, Uint32 y, object_t type) : 
CVorticonSpriteObject(pmap, x, y, type, 0)
{

}

void CCarrier::getTouchedBy(CVorticonSpriteObject& theObject)
{                
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
        //const int m_py2 = player->getYMidPos();
        const int playerFeetY = player->getYDownPos()-(4<<STC); // works better with scrubs
        const int headY = getYUpPos()+(4<<STC);

        if(playerFeetY > headY)
        {
            if( (player->getXMidPos() < getXMidPos() && yDirection >= 0) || yDirection <= 0 )
            {
                player->push(*this);
                player->yinertia = 0;
            }
        }
        else
        {
            bool playerFound = false;

            for(auto &carriedPlayer : mCarriedPlayerVec)
            {
                if(carriedPlayer == player)
                    playerFound = true;
            }

            if(!playerFound)
            {
                mCarriedPlayerVec.push_back(player);
                player->pSupportedbyobject = this;
                player->dontdraw = true;
            }
        }
    }
}

void CCarrier::process()
{
    // check if someone is still standing on the platform
    for(auto &carriedPlayer : mCarriedPlayerVec)
    {
        if(!carriedPlayer)
            continue;

        if(!hitdetect(*carriedPlayer) || carriedPlayer->blockedu)
        {
            carriedPlayer->pSupportedbyobject = nullptr;
            carriedPlayer->dontdraw = false;
            carriedPlayer = nullptr;
        }
    }

}


void CCarrier::draw()
{
    // This draw routine also is able to draw a second object in case it is holding one.
    if( dontdraw )
        return;
    
    GsSprite &Sprite = gGraphics.getSprite(mSprVar,sprite);
    
    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;
    
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    
    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        Uint16 showX = scrx+Sprite.getXOffset();
        Uint16 showY = scry+Sprite.getYOffset();
        if(m_blinktime > 0)
        {
            Sprite.drawBlinkingSprite( showX, showY );
            m_blinktime--;
        }
        else
        {
            const int w = Sprite.getWidth();
            const int h = Sprite.getHeight();

            Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );

            for(auto &carriedPlayer : mCarriedPlayerVec)
            {
                if(!carriedPlayer)
                    continue;

                GsSprite &playSprite = gGraphics.getSprite(mSprVar, carriedPlayer->sprite);
                int distx = carriedPlayer->getXPosition()-getXPosition();
                int disty = carriedPlayer->getYPosition()-getYPosition();

                distx = (distx>>STC);
                distx += (playSprite.getXOffset()-Sprite.getXOffset());
                disty = (disty>>STC);
                disty += (playSprite.getYOffset()-Sprite.getYOffset());

                const int playW = playSprite.getWidth();
                const int playH = playSprite.getHeight();

                playSprite.drawSprite( showX+distx, showY+disty, playW, playH );
            }
        }
        hasbeenonscreen = true;
    }
}

std::vector<CSpriteObject*> CCarrier::fetchCarriedPlayer()
{
    std::vector<CSpriteObject*> carriedSO;

    for( auto &player : mCarriedPlayerVec)
    {
        if(player == nullptr)
            continue;

        if( !player->dead || !player->dying )
            carriedSO.push_back( (CSpriteObject*) player);
    }

    return carriedSO;
}


void CCarrier::moveCarrierLeft(const int amnt)
{
    // If the Player is standing on the plat move him with it!
    if(amnt <= 0)
        return;

    std::vector<CSpriteObject*> carriedSO = fetchCarriedPlayer();

    if(!mCarriedPlayerVec.empty())
    {
        mMoveTasks.push_back(new ObjMoveCouples(-amnt,0, carriedSO));
        return;
    }
    moveLeft(amnt);
}


void CCarrier::moveCarrierRight(const int amnt)
{
    // If the Player is standing on the plat move him with it!
    if(amnt <= 0)
        return;

    std::vector<CSpriteObject*> carriedSO = fetchCarriedPlayer();

    if(!mCarriedPlayerVec.empty())
    {
        mMoveTasks.push_back(new ObjMoveCouples(amnt,0, carriedSO));
        return;
    }
    
    // Now move the platform itself.
    moveRight(amnt);
}

void CCarrier::moveCarrierUp(const int amnt)
{
    std::vector<CSpriteObject*> carriedSO = fetchCarriedPlayer();

    // First move the object on platform if any
    if(!mCarriedPlayerVec.empty())
    {
        mMoveTasks.push_back(new ObjMoveCouples(0,-amnt, carriedSO));
        return;
    }
    
    // Now move the platform itself.
    moveUp(amnt);
}

void CCarrier::moveCarrierDown(const int amnt)
{
    std::vector<CSpriteObject*> carriedSO = fetchCarriedPlayer();

    // First move the object on platform if any
    if(!mCarriedPlayerVec.empty())
    {
        mMoveTasks.push_back(new ObjMoveCouples(0,amnt, carriedSO));
        return;
    }

    // Now move the platform itself.
    moveDown(amnt);
}
