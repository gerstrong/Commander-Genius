#include "dreamscontrolpanel.h"

#include <base/CInput.h>

#define REFKEEN_VER_KDREAMS_ANYEGA_ALL


// Some forward declarations
extern "C"
{
#include "../../refkeen/kdreams/kd_def.h"

extern void
USL_XORICursor(id0_int_t x,id0_int_t y,const id0_char_t *s,id0_word_t cursor);

extern void (*USL_MeasureString)(const id0_char_t *,const id0_char_t *,id0_word_t *,id0_word_t *);
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
        mpLineInput->start(openLineInput->x, openLineInput->y,
                           openLineInput->buf, openLineInput->def,
                           openLineInput->escok,
                           openLineInput->maxchars, openLineInput->maxwidth);
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

void LineInput::start(int x, int y,
                      char *buf, const char *def,
                      bool escok,
                      int maxchars, int maxwidth)
{

    id0_boolean_t		redraw,
                cursorvis,cursormoved,
                done,result;
    id0_char_t		c,olds[MaxString];
    id0_word_t		i,
                cursor,
                w,h,
                len;
    id0_longword_t	lasttime;


    mMaxchars = maxchars;
    mMaxwidth = maxwidth;

    mx = x;
    my = y;

    VW_HideCursor();

    mStr = def;
    mBuf = buf;

    *olds = '\0';
    mCursor = mStr.size();

    mEscok = escok;

    //mLasttime = SD_GetTimeCount();
    //mLastASCII = key_None;
    //mLastScan = sc_None;

}

void LineInput::ponder()
{

    if(mDone)
    {
        if (mCursorvis)
        {
            USL_XORICursor(mx,my,mStr.c_str(),mCursor);
        }

//	asm	pushf
//	asm	cli
        BE_ST_ShortSleep();

        mSc = LastScan;
        LastScan = sc_None;
        mC = LastASCII;
        LastASCII = key_None;

//	asm	popf

        switch (mSc)
        {
        case sc_LeftArrow:
            if (mCursor)
                mCursor--;
            mC = key_None;
            mCursorMoved = true;
            break;
        case sc_RightArrow:
            if (mStr[mCursor])
                mCursor++;
            mC = key_None;
            mCursorMoved = true;
            break;
        case sc_Home:
            mCursor = 0;
            mC = key_None;
            mCursorMoved = true;
            break;
        case sc_End:
            mCursor = mStr.size();
            mC = key_None;
            mCursorMoved = true;
            break;

        case sc_Return:
            strcpy(mBuf,mStr.c_str());
            mDone = true;
            mResult = true;
            mC = key_None;
            break;
        case sc_Escape:
            if (mEscok)
            {
                mDone = true;
                mResult = false;
            }
            mC = key_None;
            break;

        case sc_BackSpace:
            if (mCursor)
            {
                auto sCur = mStr.substr(mCursor);
                mStr.replace(mCursor - 1,  sCur.size(),  sCur);
                mCursor--;
                mRedraw = true;
            }
            mC = key_None;
            mCursorMoved = true;
            break;
        case sc_Delete:
            if (mStr[mCursor])
            {                
                auto sCur = mStr.substr(mCursor+1);
                mStr.replace (mCursor,  sCur.size(),  sCur);
                mRedraw = true;
            }
            mC = key_None;
            mCursorMoved = true;
            break;

        case 0x4c:	// Keypad 5
        case sc_UpArrow:
        case sc_DownArrow:
        case sc_PgUp:
        case sc_PgDn:
        case sc_Insert:
            mC = key_None;
            break;
        }

        if (mC)
        {
            uint16_t w,h;
            const auto len = mStr.size();
            USL_MeasureString(mStr.c_str(),NULL,&w,&h);

            if
            (
                isprint(mC)
            &&	(len < MaxString - 1)
            &&	((!mMaxchars) || (len < mMaxchars))
            &&	((!mMaxwidth) || (w < mMaxwidth))
            )
            {
                for (int i = len + 1 ; i > mCursor ; i--)
                {
                    mStr[i] = mStr[i - 1];
                }
                mStr[mCursor++] = mC;
                mRedraw = true;
            }
        }


        if(gInput.getPressedCommand(IC_JUMP))
        {
            // Let the system set a name;
            mDone = true;
            mResult = true;

            time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );

            std::string slotStr = asctime(timeinfo);
            mStr = slotStr;

            mRedraw = true;
            strcpy(mBuf,mStr.c_str());
        }
        if(gInput.getPressedCommand(IC_BACK))
        {
            // Let the system set a name;
            mDone = true;
            mResult = false;
            return;
        }

        /*if (mRedraw)
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

        VW_UpdateScreen();*/

    }
    else
    {
        /*gInput.flushAll();

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

        IN_ClearKeysDown();*/
    }
    //return(result);
}

void LineInput::render()
{

}

}
