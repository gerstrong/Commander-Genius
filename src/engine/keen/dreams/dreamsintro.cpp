#include "dreamsintro.h"

#include <base/CInput.h>
#include "dreamscontrolpanel.h"

#define REFKEEN_VER_KDREAMS_ANYEGA_ALL

// Some forward declarations
extern "C"
{
#include "../../refkeen/kdreams/kd_def.h"
}

#define	TickBase	70		// 70Hz per tick - used as a base for timer 0


std::string mFileName1;
struct Shape mFileShape1;


namespace dreams
{


DreamsIntro::DreamsIntro()
{
    mStep = 1;
    mTimer = TickBase * 4;

    VW_SetScreenMode (GRMODE);
    VW_ClearVideo (BLACK);

    US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
    restartgame = gd_Continue;

    //while (true)
    //{

        loadedgame = false;

        mFileName1 = "TITLESCR.LBM";
        if (LoadLIBShape("KDREAMS.CMP", mFileName1.c_str(), &mFileShape1))
        {
            //Quit("Can't load TITLE SCREEN");
        }

        //while (!restartgame && !loadedgame)


    VW_InitDoubleBuffer();
    IN_ClearKeysDown();

    VW_SetScreen(0, 0);
    MoveGfxDst(0, 200);
    UnpackEGAShapeToScreen(&mFileShape1, 0, 0);
    mTimer = TickBase * 4;

}

void DreamsIntro::start()
{

}

void DreamsIntro::pumpEvent(const CEvent *evPtr)
{

}

void DreamsIntro::ponder(const float deltaT)
{
    // Ticking until input happens or user presses something
    if(mTimer > 0)
    {
        mTimer--;

        if(gInput.getPressedAnyCommand())
        {
            gEventManager.add( new LaunchControlPanel );
            mTimer = 0;
        }

        return;
    }

    if(mStep == 1)
    {
        VW_SetScreen(0, 0);
        MoveGfxDst(0, 200);
        UnpackEGAShapeToScreen(&mFileShape1, 0, 0);
        mTimer = TickBase * 4;
        mStep++;
    }
    else if(mStep == 2)
    {
        MoveGfxDst(0, 200);
        UnpackEGAShapeToScreen(&mFileShape1, 0, 0);
        mTimer = TickBase * 3;
        mStep++;
    }
    else
    {
        displayofs = 0;
        mTimer = TickBase * 6;
        mStep = 1;
    }
}

void DreamsIntro::render()
{
    if(mStep == 1)
    {
        VW_ScreenToScreen (64*200,0,40,200);
    }
    else if(mStep == 2)
    {
        VW_ScreenToScreen (64*200,0,40,200);
    }
    else
    {
        displayofs = 0;
        VWB_Bar(0,0,320,200,FIRSTCOLOR);
        US_DisplayHighScores(-1);

    }
}

}
