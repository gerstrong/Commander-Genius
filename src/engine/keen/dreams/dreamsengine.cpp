#include "dreamsengine.h"

#include "../../refkeen/be_cross.h"

// TODO: Ugly wrapper for the refkeen variables used. It serves as interface to C. Might be inmproved in future.
extern "C"
{

void kdreams_exe_main(void);

char *dreamsengine_datapath = nullptr;

extern void RefKeen_Patch_id_ca(void);
extern void RefKeen_Patch_id_us(void);
extern void RefKeen_Patch_id_rf(void);
extern void RefKeen_Patch_kd_play(void);

}



namespace dreams
{

void DreamsEngine::start()
{
    dreamsengine_datapath = const_cast<char*>(mDataPath.c_str());

    BE_Cross_PrepareGameInstallations();

    RefKeen_Patch_id_ca();
    RefKeen_Patch_id_us();
    RefKeen_Patch_id_rf();
    RefKeen_Patch_kd_play();

    kdreams_exe_main();
}

}
