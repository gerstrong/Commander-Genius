/* ESEQ_EP1.C
 Ending sequence for Episode 1.
 */

#include "../keen.h"
#include "../game.h"
#include "../include/gamedo.h"
#include "../include/gamepdo.h"
#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../StringUtils.h"
#include "../fileio/CExeFile.h"

#include "EndingSequenceEp1.h"
#include "CommonEnding.h"


#define LETTER_SHOW_SPD          30
#define RETURNTOSHIP_WAIT_TIME   600

#define BACKHOME_SHORT_WAIT_TIME   250

int eseq1_BackAtHome(stCloneKeenPlus *pCKP)
{
	/*int i;
	 std::string text[10];
	 std::string strname;
	 std::string tempbuf;
	 short textline, showtimer;
	 unsigned short amountshown;
	 signed int waittimer;
	 int state;
	 int enter, lastenterstate;
	 int dlgX, dlgY, dlgW, dlgH;
	 CBitmap *bm_window;
	 
	 SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;
	 
	 #define STATE_TEXTAPPEARING   0
	 #define STATE_WAITASEC        1
	 #define STATE_FADING          2
	 
	 // get pointers to all the strings we're going to be using
	 for(i=0;i<8;i++)
	 {
	 strname = "EP1_ESEQ_PART2_PAGE" + itoa(i+1);
	 text[i] = getstring(strname);
	 }
	 
	 textline = 0;
	 amountshown = 0;
	 showtimer = 0;
	 state = STATE_TEXTAPPEARING;
	 lastenterstate = 1;
	 waittimer = 0;
	 
	 SDL_Surface *finale_sfc = SDL_CreateRGBSurface( g_pVideoDriver->BlitSurface->flags, 320, 200, 8, 0, 0, 0, 0);
	 SDL_SetColors( finale_sfc, g_pGfxEngine->Palette.m_Palette, 0, 255);
	 finale_draw( finale_sfc, "finale.ck1", pCKP->Resources.GameDataDirectory);
	 
	 scrollx_buf = scroll_x = 0;
	 scrolly_buf = scroll_y = 0;
	 
	 numplayers = 1;
	 player[0].x = 16;
	 player[0].y = 16;
	 player[0].playframe = BlankSprite;
	 
	 dlgX = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "LEFT");
	 dlgY = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "TOP");
	 dlgW = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "WIDTH");
	 dlgH = GetStringAttribute("EP1_ESEQ_PART2_PAGE1", "HEIGHT");
	 bm_window = g_pGfxEngine->getBitmap("WINDOFF");	// window lights off
	 
	 do
	 {
	 // Draw uncompressed finale Plot
	 SDL_BlitSurface( finale_sfc, NULL, g_pVideoDriver->BlitSurface, NULL );
	 
	 enter = ( g_pInput->getPressedKey(KENTER) || g_pInput->getPressedKey(KCTRL) || g_pInput->getPressedKey(KALT) );
	 
	 // Show the window (lights on or off)
	 bm_window->draw(g_pVideoDriver->BlitSurface, 80, 0);
	 
	 g_pGfxEngine->drawDialogBox(sfc ,dlgX, dlgY, dlgW, dlgH);
	 
	 // draw the current text line up to the amount currently shown
	 tempbuf = text[textline];
	 if(amountshown < tempbuf.size())
	 tempbuf.erase(amountshown);
	 g_pGfxEngine->Font->drawFont(sfc, tempbuf, (dlgX+1)*8, (dlgY+1)*8);
	 
	 if (state==STATE_TEXTAPPEARING)
	 {
	 if (enter) goto fullshow;
	 if (showtimer > LETTER_SHOW_SPD)
	 {  // it's time to show a new letter
	 amountshown++;
	 if (amountshown > text[textline].size())
	 {  // reached end of line
	 state = STATE_WAITASEC;
	 waittimer = -BACKHOME_SHORT_WAIT_TIME*2;
	 }
	 // if the last letter shown is a dash/cr ('Billy...are you a-'),
	 // show the rest of the text immediately
	 // (for when the mom shouts "WHAT IS THIS ONE-EYED GREEN THING..."
	 if (text[textline][amountshown]==13 && \
	 text[textline][amountshown-1]=='-')
	 {
	 fullshow: ;
	 amountshown = text[textline].size();
	 state = STATE_WAITASEC;
	 waittimer = -BACKHOME_SHORT_WAIT_TIME*3;
	 }
	 showtimer = 0;
	 } else showtimer++;
	 
	 // user pressed enter
	 if (enter)
	 {  // show all text immediately
	 
	 }
	 }
	 else if (state==STATE_WAITASEC)
	 {
	 if (enter) goto nextline;
	 if (waittimer<BACKHOME_SHORT_WAIT_TIME)
	 {
	 waittimer++;
	 if (waittimer==BACKHOME_SHORT_WAIT_TIME)
	 {
	 nextline: ;
	 
	 if( (textline > 0 && textline < 5)  || textline == 6)
	 bm_window = g_pGfxEngine->getBitmap("WINDON"); // lights on
	 else
	 bm_window = g_pGfxEngine->getBitmap("WINDOFF"); // lights off
	 
	 textline++;
	 state = STATE_TEXTAPPEARING;
	 amountshown = 0;
	 if (textline>7)
	 {  // end of text
	 break;
	 }
	 }
	 }
	 }
	 
	 lastenterstate = enter;
	 g_pInput->pollEvents();
	 if (g_pInput->getPressedKey(KQUIT)) return 1;
	 gamedo_frameskipping_blitonly();
	 } while(1);
	 
	 SDL_FreeSurface(finale_sfc);
	 */
	return 0;
}

int readStoryText(char **ptext, int episode, const std::string& path);

void eseq1_showEndingText(std::string &Path)
{
	std::string text;
	unsigned char *filebuf;
	unsigned long startflag=0x1652A-512, endflag=0x1679A-512; // where story begins and ends!
	
	CExeFile *ExeFile = new CExeFile(1, Path);
	if(!ExeFile) return;
	if(!ExeFile->readData()) return;
	filebuf = ExeFile->getData() + startflag;
	
	for( unsigned long i=0 ; i<endflag-startflag ; i++ )
	{
		if(filebuf[i])
			text.push_back(filebuf[i]);
	}
	
	delete ExeFile;
	
	text.clear();
}
