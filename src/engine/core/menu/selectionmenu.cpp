#include <base/CInput.h>

#include "SelectionMenu.h"

#include "graphics/GsGraphics.h"


SelectionEvent::~SelectionEvent()
{}

NewGamePlayersEvent::~NewGamePlayersEvent()
{}

struct ChangeEvent : InvokeFunctorEvent
{
    virtual ~ChangeEvent();

    ChangeEvent(CPlayerSpriteVarSelection &selObj):
        mSelObj(selObj) {}

    CPlayerSpriteVarSelection &mSelObj;

    void operator()() const
    {
        mSelObj.changeEvent();
    }
};

ChangeEvent::~ChangeEvent()
{}


CPlayerSpriteVarSelection::
CPlayerSpriteVarSelection(const Style style) :
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
        mpBmpBox =
        mpMenuDialog->add( new GsBitmapBox(mpPlayerBmpVec[0]));

        mpBmpBox->setRect( GsRect<float>(0.17f, 0.21f,
                                               0.24f, 0.32f) );
    }


    mpChangeButton =
    mpMenuDialog->add(
                new GameButton( "Change",
                         new ChangeEvent(*this), style));

    mpChangeButton->setRect( GsRect<float>(0.17f, 0.66f,
                                         0.8f, 0.1f) );


    mpGameButton =
    mpMenuDialog->add(
                new GameButton( "Start",
                      new SelectPlayerSpriteVarEvent(mCurIdx),style));


    mpGameButton->setRect( GsRect<float>(0.17f, 0.81f,
                                         0.8f, 0.1f) );
    select(1);
}


void
CPlayerSpriteVarSelection::
changeEvent()
{
    mCurIdx++;

    if( mCurIdx >= int(mpPlayerBmpVec.size()) )
    {
        mCurIdx = 0;
    }

    mpBmpBox->setBitmapPtr(mpPlayerBmpVec[mCurIdx]);

    // Change data in the event of the game button
    auto event = mpGameButton->event();
    auto spsv = std::dynamic_pointer_cast<SelectPlayerSpriteVarEvent>(event);
    spsv->mSpecialIdx = mCurIdx;
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
        mpBmpBox->setBitmapPtr(mpPlayerBmpVec[mCurIdx]);
    }

    // Change data in the event of the game button
    auto event = mpGameButton->event();
    auto spsv = std::dynamic_pointer_cast<SelectPlayerSpriteVarEvent>(event);
    spsv->mSpecialIdx = mCurIdx;
}
