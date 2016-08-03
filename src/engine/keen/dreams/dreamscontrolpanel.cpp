#include "dreamscontrolpanel.h"


#define REFKEEN_VER_KDREAMS_ANYEGA_ALL


// Some forward declarations
extern "C"
{
#include "../../refkeen/kdreams/kd_def.h"
}


namespace dreams
{


DreamsControlPanel::DreamsControlPanel()
{
    const id0_char_t *FileName1;
    struct Shape FileShape1;

    id0_int_t bufsave	= bufferofs;
    id0_int_t dissave	= displayofs;

//
// demo loop
//
    US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
    restartgame = gd_Continue;
}


void DreamsControlPanel::start()
{
    US_ControlPanel_Init();
}


void DreamsControlPanel::ponder(const float deltaT)
{
    //bufferofs = bufsave;
    //displayofs = dissave;

    VW_FixRefreshBuffer();
    //US_ControlPanel ();
    US_ControlPanel_Ponder();

    //if(gDreamsForceClose)
//        return;
}


void DreamsControlPanel::render()
{
    VW_UpdateScreen();
}

}
