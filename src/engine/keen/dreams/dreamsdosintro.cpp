#include "dreamsdosintro.h"


#include <base/CInput.h>

#include "../../refkeen/be_cross.h"
#include "../galaxy/res/CAudioGalaxy.h"

#include "dreamsintro.h"


#define REFKEEN_VER_KDREAMS_ANYEGA_ALL

extern "C"
{

#include "../../refkeen/kdreams/kd_def.h"


extern void RefKeen_FillObjStatesWithDOSPointers(void);


}






namespace dreams
{





// Mapping the strings of the filenames to the pointers where we store the embedded data
extern std::map< std::string, unsigned int > gOffsetMap;


bool setupAudio()
{
    CAudioGalaxy *audio = new CAudioGalaxy();

    const auto audioOffset = gOffsetMap["AUDIODCT.KDR"];

    if(audio->loadSoundData(audioOffset))
    {
        gSound.setupSoundData(audio->sndSlotMapGalaxy[7], audio);
        return true;
    }

    return false;
}

void InitGame()
{
    MM_Startup();

    id0_int_t i;

/*#if 0
    // Handle piracy screen...
    //
    movedata(FP_SEG(PIRACY),(id0_unsigned_t)PIRACY,0xb800,displayofs,4000);
    while (BE_ST_BiosScanCode(0) != sc_Return);
    //while ((bioskey(0)>>8) != sc_Return);
#endif*/

#if GRMODE == EGAGR
    if (mminfo.mainmem < 335l*1024)
    {
//#pragma warn    -pro
//#pragma warn    -nod
#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
        BE_ST_textcolor(7);
#endif
#ifndef REFKEEN_VER_KDREAMS_CGA_ALL
        if (refkeen_current_gamever == BE_GAMEVER_KDREAMSE113)
#endif
        {
            BE_ST_textbackground(0);
        }
//#pragma warn    +nod
//#pragma warn    +pro
        BE_ST_clrscr();                       // we can't include CONIO because of a name conflict
//#pragma warn    +nod
//#pragma warn    +pro
        BE_ST_puts ("There is not enough memory available to play the game reliably.  You can");
        BE_ST_puts ("play anyway, but an out of memory condition will eventually pop up.  The");
        BE_ST_puts ("correct solution is to unload some TSRs or rename your CONFIG.SYS and");
        BE_ST_puts ("AUTOEXEC.BAT to free up more memory.\n");
        BE_ST_puts ("Do you want to (Q)uit, or (C)ontinue?");
        //i = bioskey (0);
        //if ( (i>>8) != sc_C)
        i = BE_ST_BiosScanCode (0);
        if (i != sc_C)
            Quit ("");
    }
#endif

    US_TextScreen();

    VW_Startup ();
    RF_Startup ();
    IN_Startup ();
    SD_Startup ();
    US_Startup ();

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
    US_UpdateTextScreen();
#endif

    CA_Startup ();
    US_Setup ();

//
// load in and lock down some basic chunks
//

    CA_ClearMarks ();

    CA_MarkGrChunk(STARTFONT);
    CA_MarkGrChunk(STARTFONTM);
    CA_MarkGrChunk(STARTTILE8);
    CA_MarkGrChunk(STARTTILE8M);
    for ( id0_int_t j=KEEN_LUMP_START ; j<=KEEN_LUMP_END ; j++)
    {
        CA_MarkGrChunk(j);
    }

#ifdef REFKEEN_VER_KDREAMS_CGA_ALL
    CA_CacheMarks (NULL);
#elif defined REFKEEN_VER_KDREAMS_ANYEGA_ALL
    CA_CacheMarks (NULL, 0);
#endif

    MM_SetLock (&grsegs[STARTFONT],true);
    MM_SetLock (&grsegs[STARTFONTM],true);
    MM_SetLock (&grsegs[STARTTILE8],true);
    MM_SetLock (&grsegs[STARTTILE8M],true);
    for ( id0_int_t j=KEEN_LUMP_START ; j<=KEEN_LUMP_END ; j++)
    {
        MM_SetLock (&grsegs[j],true);
    }

    setupAudio();

    fontcolor = WHITE;

    RefKeen_FillObjStatesWithDOSPointers(); // Saved games compatibility

    US_FinishTextScreen();
}


void DreamsDosIntro::start()
{
    InitGame();
}

void DreamsDosIntro::pumpEvent(const CEvent *evPtr)
{

}

void DreamsDosIntro::ponder(const float deltaT)
{
    if( gInput.getPressedAnyCommand() )
    {
        gEventManager.add( new SwitchToIntro );
        gInput.flushAll();
        // If we press any switch to the next section -> where Dreams is really loaded into CGA/EGA mode and show the intro screen
    }
}

void DreamsDosIntro::render()
{

}

}
