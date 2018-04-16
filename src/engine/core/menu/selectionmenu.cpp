#include <base/CInput.h>

#include "SelectionMenu.h"

#include "graphics/GsGraphics.h"

CPlayerSpriteVarSelection::
CPlayerSpriteVarSelection(const GsControl::Style style) :
    GameMenu( GsRect<float>(0.25f, 0.2f, 0.5f, 0.5f), style )
{

    for( auto &SpriteVec : gGraphics.spriteVecVec() )
    {
        std::shared_ptr<GsBitmap> playerBmp(
                    new GsBitmap(SpriteVec[6].exportBitmap()));

        mpPlayerBmpVec.push_back(playerBmp);
    }

    if(!mpPlayerBmpVec.empty())
    {
        mBmpBox = new CGUIBitmap(mpPlayerBmpVec[0]);

        mpMenuDialog->addControl( mBmpBox,
                                  GsRect<float>(0.07f, 0.41f,
                                                0.15f, 0.2f));
    }


    mpGameButton = new GameButton( "Start",
                                   new SelectPlayerSpriteVarEvent(mCurIdx),
                                   style);

    mpMenuDialog->addControl( mpGameButton,
                              GsRect<float>(0.07f, 0.71f,
                                            0.1f, 0.8f));

}

void
CPlayerSpriteVarSelection::
ponder(const float deltaT)
{
    // If IC_BACK is invoked, make the menu controller close the controller
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseMenuEvent() );
        return;
    }

    bool update = false;

    if( gInput.getPressedCommand(IC_LEFT) )
    {
        update = true;
        mCurIdx--;
    }
    else if( gInput.getPressedCommand(IC_RIGHT) )
    {
        update = true;
        mCurIdx++;
    }
    else if( gInput.getPressedCommand(IC_JUMP) ||
             gInput.getPressedCommand(IC_STATUS))
    {
        gEventManager.add( new SelectPlayerSpriteVarEvent(mCurIdx) );
        return;
    }

    mpMenuDialog->processLogic();

    if( mCurIdx < 0 )
    {
        mCurIdx = 0;
        return;
    }
    if( mCurIdx >= int(mpPlayerBmpVec.size()) )
    {
        mCurIdx = mpPlayerBmpVec.size()-1;
        return;
    }

    if(update)
    {
        mBmpBox->setBitmapPtr(mpPlayerBmpVec[mCurIdx]);
    }

    // Change data in the event of the game button
    auto event = mpGameButton->event();
    auto spsv = std::dynamic_pointer_cast<SelectPlayerSpriteVarEvent>(event);
    spsv->mSpecialIdx = mCurIdx;
}
