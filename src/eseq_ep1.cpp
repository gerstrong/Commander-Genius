/* ESEQ_EP1.C
  Ending sequence for Episode 1.
*/

#include "keen.h"
#include "include/game.h"
#include "include/gamedo.h"
#include "include/gamepdo.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "include/eseq_ep1.h"
#include "include/eseq_ep2.h"
#include "include/menu.h"
#include "CGraphics.h"
#include "StringUtils.h"

#define CMD_MOVE                0
#define CMD_WAIT                1
#define CMD_SPAWNSPR            2
#define CMD_REMOVESPR           3
#define CMD_FADEOUT             4
#define CMD_ENDOFQUEUE          5
#define CMD_ENABLESCROLLING     6
#define CMD_DISABLESCROLLING    7


// start x,y map scroll position for eseq1_ShipFlys()
#define SHIPFLY_X       32
#define SHIPFLY_Y       0

// worldmap scroll position for eseq1_ReturnsToShip()
#define WM_X            40
#define WM_Y            540

#define LETTER_SHOW_SPD          30
#define RETURNTOSHIP_WAIT_TIME   600

#define SPR_SHIP_RIGHT  115
#define SPR_SHIP_LEFT   116
#define SPR_EXCLAMATION 117
#define SPR_QUESTION    118
#define SHIPSPD         4

#define BACKHOME_SHORT_WAIT_TIME   250

int eseq1_ReturnsToShip(stCloneKeenPlus *pCKP)
{
int i;

  for(i=0;i<MAX_LEVELS;i++)
   pCKP->Control.levelcontrol.levels_completed[i] = 0;

  showmapatpos(80, WM_X, WM_Y, 0, pCKP);

  // draw keen next to his ship
  g_pGraphics->drawSprite(168, 85, PMAPLEFTFRAME, 0);
  // do not show player when we render the screen
  numplayers = 1;
  player[0].x = 0;
  player[0].y = 0;
  player[0].playframe = BlankSprite;
  // all objects -> nonexistant
  for(i=0;i<MAX_OBJECTS;i++) objects[i].exists = 0;

  fade.mode = FADE_GO;
  fade.dir = FADE_IN;
  fade.curamt = 0;
  fade.fadetimer = 0;
  fade.rate = FADE_NORM;

  eseq_showmsg(getstring("EP1_ESEQ_PART1"),1,18,37,6,1, pCKP);

  // fade out
  fade.mode = FADE_GO;
  fade.dir = FADE_OUT;
  fade.curamt = PAL_FADE_SHADES;
  fade.fadetimer = 0;
  fade.rate = FADE_NORM;
  do
  {
    gamedo_fades();
    gamedo_AnimatedTiles();

    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
  } while(fade.mode != FADE_COMPLETE && !g_pInput->getPressedKey(KQUIT));

  return 0;
}


void addshipqueue(int cmd, int time, int flag1)
{
  shipqueue[ShipQueuePtr].cmd = cmd;
  shipqueue[ShipQueuePtr].time = time;
  shipqueue[ShipQueuePtr].flag1 = flag1;
  ShipQueuePtr++;
}

int eseq1_ShipFlys(stCloneKeenPlus *pCKP)
{
char enter,lastenterstate;
int x, y;
int scrollingon;

  scrollingon = 1;

  #define MARK_SPR_NUM          2
  initgame(pCKP);

  // set up the ship's route
  ShipQueuePtr = 0;
  addshipqueue(CMD_MOVE, 230, DUP);
  addshipqueue(CMD_WAIT, 50, 0);
  addshipqueue(CMD_MOVE, 2690, DDOWNRIGHT);
  addshipqueue(CMD_WAIT, 100, 0);
  addshipqueue(CMD_MOVE, 480, DDOWN);
  addshipqueue(CMD_WAIT, 150, 0);
  addshipqueue(CMD_SPAWNSPR, 0, SPR_QUESTION);
  addshipqueue(CMD_DISABLESCROLLING, 0, 0);
  addshipqueue(CMD_WAIT, 350, 0);
  addshipqueue(CMD_REMOVESPR, 0, 0);
  addshipqueue(CMD_WAIT, 50, 0);
  addshipqueue(CMD_MOVE, 700, DLEFT);
  addshipqueue(CMD_WAIT, 150, 0);
  addshipqueue(CMD_SPAWNSPR, 0, SPR_EXCLAMATION);
  addshipqueue(CMD_WAIT, 500, 0);
  addshipqueue(CMD_REMOVESPR, 0, 0);
  addshipqueue(CMD_WAIT, 50, 0);
  addshipqueue(CMD_MOVE, 700, DRIGHT);
  addshipqueue(CMD_WAIT, 25, 0);
  addshipqueue(CMD_ENABLESCROLLING, 0, 0);
  addshipqueue(CMD_MOVE, 465, DDOWN);
  addshipqueue(CMD_FADEOUT, 0, 0);
  addshipqueue(CMD_MOVE, 100, DDOWN);
  addshipqueue(CMD_ENDOFQUEUE, 0, 0);

  showmapatpos(81, SHIPFLY_X, SHIPFLY_Y, 0, pCKP);

  objects[MARK_SPR_NUM].type = OBJ_YORP;                // doesn't matter
  objects[MARK_SPR_NUM].exists = 0;
  objects[MARK_SPR_NUM].sprite = SPR_QUESTION;

  numplayers = 1;
  // place the player at the center of mars
  if (map_findtile(593, &x, &y))
  { // found the tile at the center of mars
    player[0].x = ((x<<4)+1)<<CSF;
    player[0].y = ((y<<4)-3)<<CSF;
  }
  else
  {
      crashflag = 1;
      why_term_ptr = "eseq1_shipflys(): unable to find center of Mars.";
      return 1;
  }

  player[0].playframe = SPR_SHIP_RIGHT;
//  player[0].pbgprioritycorners = 0;

  fade.mode = FADE_GO;
  fade.dir = FADE_IN;
  fade.curamt = 0;
  fade.fadetimer = 0;
  fade.rate = FADE_NORM;
  ShipQueuePtr = 0;
  max_scroll_x = max_scroll_y = 20000;
  do
  {
    // keep the question or exclamation mark sprite next to the player
    objects[MARK_SPR_NUM].x = player[0].x + (20<<CSF);
    objects[MARK_SPR_NUM].y = player[0].y - (10<<CSF);
    objects[MARK_SPR_NUM].onscreen = 1;
//    objects[MARK_SPR_NUM].priorityptsfound = 0;
//    objects[MARK_SPR_NUM].priorityptschecked = 4;
    objects[MARK_SPR_NUM].scrx = (objects[MARK_SPR_NUM].x>>CSF)-scroll_x;
    objects[MARK_SPR_NUM].scry = (objects[MARK_SPR_NUM].y>>CSF)-scroll_y;

    // execute the current command in the queue
    if (fade.dir != FADE_OUT)
    {
          switch(shipqueue[ShipQueuePtr].cmd)
          {
            case CMD_MOVE:
              switch(shipqueue[ShipQueuePtr].flag1)
              {
                case DUP:
                  player[0].y-=SHIPSPD;
                  player[0].playframe = SPR_SHIP_RIGHT;
                  break;
                case DDOWN:
                  player[0].y+=SHIPSPD/2;
                  player[0].playframe = SPR_SHIP_RIGHT;
                  break;
                case DLEFT:
                  player[0].x-=SHIPSPD;
                  player[0].playframe = SPR_SHIP_LEFT;
                  break;
                case DRIGHT:
                  player[0].x+=SHIPSPD;
                  player[0].playframe = SPR_SHIP_RIGHT;
                  break;
                case DDOWNRIGHT:
                  player[0].x+=SHIPSPD*2;
                  player[0].y+=SHIPSPD*0.8;
                  player[0].playframe = SPR_SHIP_RIGHT;
                  break;
              }
            break;
            case CMD_SPAWNSPR:
              objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
              objects[MARK_SPR_NUM].exists = 1;
            break;
            case CMD_REMOVESPR:
              objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
              objects[MARK_SPR_NUM].exists = 0;
            break;
            case CMD_ENABLESCROLLING:
              scrollingon = 1;
            break;
            case CMD_DISABLESCROLLING:
              scrollingon = 0;
            break;
            case CMD_WAIT:
            break;
            case CMD_FADEOUT:
              if (fade.dir!=FADE_OUT)
              {
                fade.dir = FADE_OUT;
                fade.curamt = PAL_FADE_SHADES;
                fade.fadetimer = 0;
                fade.mode = FADE_GO;
                fade.rate = FADE_NORM;
              }
            break;
            default: break;
          }
          // decrease the time remaining
          if (shipqueue[ShipQueuePtr].time)
          {
            shipqueue[ShipQueuePtr].time--;
          }
          else
          {  // no time left on this command, go to next cmd
            ShipQueuePtr++;
          }
    }

    if (fade.dir==FADE_OUT && fade.mode==FADE_COMPLETE)
    {  // we're done
      return 0;
    }

    enter = ( g_pInput->getPressedKey(KENTER) || g_pInput->getPressedKey(KCTRL) || g_pInput->getPressedKey(KALT) );
    if (enter)
    {
      if (fade.dir!=FADE_OUT)
      {
        fade.dir = FADE_OUT;
        fade.curamt = PAL_FADE_SHADES;
        fade.fadetimer = 0;
        fade.mode = FADE_GO;
        fade.rate = FADE_NORM;
      }
    }
    lastenterstate = enter;

    gamedo_fades();
    gamedo_AnimatedTiles();

    gamedo_frameskipping(pCKP);
    if (scrollingon) gamedo_ScrollTriggers(0);

    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
  } while(!g_pInput->getPressedKey(KQUIT));
  return 1;
}

int eseq1_BackAtHome(stCloneKeenPlus *pCKP)
{
/*int draw;*/
int i;
	std::string text[10];
	std::string strname;
	std::string tempbuf;
short textline, showtimer;
unsigned short amountshown;
signed int waittimer;
int state;
int enter, lastenterstate;
int dlgX, dlgY, dlgW, dlgH;

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

  finale_draw("finale.ck1", pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);
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

  fade.mode = FADE_GO;
  fade.rate = FADE_NORM;
  fade.dir = FADE_IN;
  fade.curamt = 0;
  fade.fadetimer = 0;
  do
  {
	enter = ( g_pInput->getPressedKey(KENTER) || g_pInput->getPressedKey(KCTRL) || g_pInput->getPressedKey(KALT) );

	sb_dialogbox(dlgX, dlgY, dlgW, dlgH);

    // draw the current text line up to the amount currently shown
	  tempbuf = text[textline];
	  if(amountshown < tempbuf.size())
		  tempbuf.erase(amountshown);
    g_pGraphics->sb_font_draw( tempbuf, (dlgX+1)*8, (dlgY+1)*8);

    if (state==STATE_TEXTAPPEARING)
    {
      if (enter) goto fullshow;
      if (showtimer > LETTER_SHOW_SPD)
      {  // it's time to show a new letter
        amountshown++;
        if (amountshown > text[textline].size())
        {  // reached end of line
          state = STATE_WAITASEC;
//          if (textline==8)
  //          waittimer = -BACKHOME_SHORT_WAIT_TIME*3;
    //      else
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

    if (fade.dir==FADE_OUT && fade.mode==FADE_COMPLETE)
      return 0;

    gamedo_fades();
    gamedo_frameskipping_blitonly();

    lastenterstate = enter;

    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
    if (g_pInput->getPressedKey(KQUIT)) return 1;
  } while(1);

  finale_draw("finale.ck1", pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);
  eseq_ToBeContinued(pCKP);
  return 1;
}

void eseq_ToBeContinued(stCloneKeenPlus *pCKP)
{
int i;
	std::string text;
int dlgX, dlgY, dlgW, dlgH;

  // remove all objects because eseq_showmsg will call drawobjects
  for(i=0;i<MAX_OBJECTS;i++)
     objects[i].exists = 0;

  text = getstring("TO_BE_CONTINUED");
  dlgX = GetStringAttribute("TO_BE_CONTINUED", "LEFT");
  dlgY = GetStringAttribute("TO_BE_CONTINUED", "TOP");
  dlgW = GetStringAttribute("TO_BE_CONTINUED", "WIDTH");
  dlgH = GetStringAttribute("TO_BE_CONTINUED", "HEIGHT");
  eseq_showmsg(text, dlgX, dlgY, dlgW, dlgH, 0, pCKP);

  fade.dir = FADE_OUT;
  fade.curamt = PAL_FADE_SHADES;
  fade.fadetimer = 0;
  fade.rate = FADE_NORM;
  fade.mode = FADE_GO;
  do
  {
    gamedo_fades();
    if (g_pInput->getPressedKey(KQUIT)) return;
    g_pInput->pollEvents();
    g_pTimer->SpeedThrottle();
  } while(fade.mode == FADE_GO);
}
