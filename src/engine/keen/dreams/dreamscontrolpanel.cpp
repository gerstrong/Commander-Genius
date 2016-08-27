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
    /*const id0_char_t *FileName1;
    struct Shape FileShape1;

    id0_int_t bufsave	= bufferofs;
    id0_int_t dissave	= displayofs;*/

//
// demo loop
//
    US_SetLoadSaveHooks(LoadGame,SaveGame,ResetGame);
    restartgame = gd_Continue;
}


DreamsControlPanel::~DreamsControlPanel()
{
    //USL_TearDownCtlPanel();
}



void DreamsControlPanel::start()
{
    US_ControlPanel_Init();    
}


void DreamsControlPanel::pumpEvent(const CEvent *evPtr)
{
    if( const OpenLineInput* openLineInput = dynamic_cast<const OpenLineInput*>(evPtr) )
    {
        mpLineInput.reset(new LineInput);
        mpLineInput->start();
    }
}

void DreamsControlPanel::ponder(const float deltaT)
{
    if(mpLineInput)
    {
        mpLineInput->ponder();
        return;
    }

    VW_FixRefreshBuffer();
    US_ControlPanel_Ponder();
}


void DreamsControlPanel::render()
{
    if(mpLineInput)
    {
        mpLineInput->render();
        return;
    }

    VW_UpdateScreen();
}


/// Line Input implementations

void LineInput::start()
{
    /*
    id0_boolean_t		redraw,
                cursorvis,cursormoved,
                done,result;
    ScanCode	sc;
    id0_char_t		c,
                s[MaxString],olds[MaxString];
    id0_word_t		i,
                cursor,
                w,h,
                len;
    id0_longword_t	lasttime;

    VW_HideCursor();

    if (def)
        strcpy(s,def);
    else
        *s = '\0';
    *olds = '\0';
    cursor = strlen(s);
    cursormoved = redraw = true;

    cursorvis = done = false;
    lasttime = SD_GetTimeCount();
    LastASCII = key_None;
    LastScan = sc_None;
*/
}

void LineInput::ponder()
{

/*

    while (!done)
    {
        if (cursorvis)
            USL_XORICursor(x,y,s,cursor);

//	asm	pushf
//	asm	cli
        BE_ST_ShortSleep();

        sc = LastScan;
        LastScan = sc_None;
        c = LastASCII;
        LastASCII = key_None;

//	asm	popf

        switch (sc)
        {
        case sc_LeftArrow:
            if (cursor)
                cursor--;
            c = key_None;
            cursormoved = true;
            break;
        case sc_RightArrow:
            if (s[cursor])
                cursor++;
            c = key_None;
            cursormoved = true;
            break;
        case sc_Home:
            cursor = 0;
            c = key_None;
            cursormoved = true;
            break;
        case sc_End:
            cursor = strlen(s);
            c = key_None;
            cursormoved = true;
            break;

        case sc_Return:
            strcpy(buf,s);
            done = true;
            result = true;
            c = key_None;
            break;
        case sc_Escape:
            if (escok)
            {
                done = true;
                result = false;
            }
            c = key_None;
            break;

        case sc_BackSpace:
            if (cursor)
            {
                strcpy(s + cursor - 1,s + cursor);
                cursor--;
                redraw = true;
            }
            c = key_None;
            cursormoved = true;
            break;
        case sc_Delete:
            if (s[cursor])
            {
                strcpy(s + cursor,s + cursor + 1);
                redraw = true;
            }
            c = key_None;
            cursormoved = true;
            break;

        case 0x4c:	// Keypad 5
        case sc_UpArrow:
        case sc_DownArrow:
        case sc_PgUp:
        case sc_PgDn:
        case sc_Insert:
            c = key_None;
            break;
        }

        if (c)
        {
            len = strlen(s);
            USL_MeasureString(s,NULL,&w,&h);

            if
            (
                isprint(c)
            &&	(len < MaxString - 1)
            &&	((!maxchars) || (len < maxchars))
            &&	((!maxwidth) || (w < maxwidth))
            )
            {
                for (i = len + 1;i > cursor;i--)
                    s[i] = s[i - 1];
                s[cursor++] = c;
                redraw = true;
            }
        }


        if(gInput.getPressedCommand(IC_JUMP))
        {
            // Let the system set a name;
            done = true;
            result = true;

            time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );

            std::string slotStr = asctime(timeinfo);
            strcpy(s,slotStr.c_str());

            redraw = true;
            strcpy(buf,s);
        }
        if(gInput.getPressedCommand(IC_BACK))
        {
            // Let the system set a name;
            done = true;
            result = false;
            break;
        }

        if (redraw)
        {
            px = x;
            py = y;
            USL_DrawString(olds,NULL);
            strcpy(olds,s);

            px = x;
            py = y;
            USL_DrawString(s,NULL);

            redraw = false;
        }

        if (cursormoved)
        {
            cursorvis = false;
            lasttime = SD_GetTimeCount() - TickBase;

            cursormoved = false;
        }
        if (SD_GetTimeCount() - lasttime > TickBase / 2)
        {
            lasttime = SD_GetTimeCount();

            cursorvis ^= true;
        }
        if (cursorvis)
            USL_XORICursor(x,y,s,cursor);

        VW_UpdateScreen();

    }

    gInput.flushAll();

    // REFKEEN - Alternative controllers support
    //BE_ST_AltControlScheme_Pop();

    if (cursorvis)
        USL_XORICursor(x,y,s,cursor);
    if (!result)
    {
        px = x;
        py = y;
        USL_DrawString(olds,NULL);
    }
    VW_ShowCursor();
    VW_UpdateScreen();

    IN_ClearKeysDown();
    return(result);


*/
}

void LineInput::render()
{

}

}
