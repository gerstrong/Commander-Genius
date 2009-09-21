/* ESEQ_EP2.C
  Ending sequence for Episode 2.
*/

#include "../keen.h"
#include "../sdl/CInput.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../include/CStartScreen.h"
#include "../include/enemyai.h"
#include "../include/gamedo.h"
#include "EndingSequenceEp2.h"
#include "CommonEnding.h"
#include "../CGraphics.h"
#include "../StringUtils.h"


enum cmd_actions{
CMD_MOVE,
CMD_WAIT,
CMD_SPAWNSPR,
CMD_REMOVESPR,
CMD_FADEOUT,
CMD_ENDOFQUEUE,
CMD_ENABLESCROLLING,
CMD_DISABLESCROLLING
};

stShipQueue shipqueue[32];
int ShipQueuePtr;

// start x,y map scroll position for eseq2_TantalusRay()
#define TANTALUS_X       0
#define TANTALUS_Y       0

#define TANTALUS_SPD_X   21
#define TANTALUS_SPD_Y   9

#define TANTALUS_SPRITE  58

#define TAN_DELAY_BEFORE_FIRE   500

#define TAN_STATE_WAITBEFOREFIRE        0
#define TAN_STATE_FIRING                1
#define TAN_STATE_EARTH_EXPLODING       2
#define TAN_STATE_CHUNKS_FLYING         3
#define TAN_STATE_GAMEOVER              4

#define EARTHCHUNK_BIG_UP       64
#define EARTHCHUNK_BIG_DN       66
#define EARTHCHUNK_SMALL_UP     68
#define EARTHCHUNK_SMALL_DN     70

int eseq2_TantalusRay(stCloneKeenPlus *pCKP)
{
char enter;
int x, y, t, o=0, i;
int tantalus_animframe, tantalus_animtimer=0;
int state, timer, spawnedcount=0;
CSprite **sprites = &g_pGfxEngine->Sprite[0];

Uint16 tantalus_sprite_width = sprites[TANTALUS_SPRITE]->getWidth();
Uint16 tantalus_sprite_height = sprites[TANTALUS_SPRITE]->getHeight();
CBitmap *bm_gameover =NULL;

  pCKP->Control.levelcontrol.dark = 0;

  initgame( &(pCKP->Control.levelcontrol) );
  state = TAN_STATE_WAITBEFOREFIRE;

  showmapatpos(81,TANTALUS_X, TANTALUS_Y, pCKP);

  AllPlayersInvisible();
  numplayers = 1;
  player[0].hideplayer = 0;
  objects[player[0].useObject].onscreen = 1;

  // place the player (which is actually the tantalus ray) at the mouth
  // of the vorticon mothership
  if (map_findtile(593, &x, &y))
  { // found the tile
    player[0].x = ((x<<4)-1)<<CSF;
    player[0].y = ((y<<4)-0)<<CSF;
  }
  else
  {
      crashflag = 1;
      why_term_ptr = "eseq2_TantalusRay(): unable to find tile 593.";
      return 0;
  }

  player[0].playframe = BlankSprite;

  tantalus_animframe = 0;
  timer = 0;
  do
  {
	switch(state)
	{
	case TAN_STATE_WAITBEFOREFIRE:
	    if (timer > TAN_DELAY_BEFORE_FIRE)
	    {
		  g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, objects[o].scrx);
		  state = TAN_STATE_FIRING;
		  timer = 0;
	    }
	    else timer++;
	break;
	case TAN_STATE_FIRING:

	    if (tantalus_animtimer>5)
	    {
		  tantalus_animframe ^= 1;
		  player[0].playframe = TANTALUS_SPRITE + tantalus_animframe;
	    }
	    else tantalus_animtimer++;

	    player[0].x += TANTALUS_SPD_X;
	    player[0].y += TANTALUS_SPD_Y;

	    t = getmaptileat((player[0].x>>CSF)+(tantalus_sprite_width/2), (player[0].y>>CSF)+(tantalus_sprite_height/2));
	    if (t==586)
	    {  // hit center of earth
		  state = TAN_STATE_EARTH_EXPLODING;
		  player[0].playframe = BlankSprite;
		  timer = 0;
		  spawnedcount = 0;
		  srand(300);
		  o = spawn_object(player[0].x+(24<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION);
		  g_pSound->playSound(SOUND_KEEN_BLOK, PLAY_NOW);
	    }
	break;
	case TAN_STATE_EARTH_EXPLODING:
	  if (!timer)
	  {
		  if (spawnedcount<16) o = spawn_object(player[0].x+((rand()%32)<<CSF), player[0].y+((rand()%32)<<CSF)-(8<<CSF), OBJ_EXPLOSION);
		  switch(spawnedcount)
		  {
		  case 0: o = spawn_object(player[0].x-(8<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
		  case 1: o = spawn_object(player[0].x+(24<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
		  case 2: o = spawn_object(player[0].x+(16<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
		  case 3: o = spawn_object(player[0].x+(24<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION); break;
		  case 4: o = spawn_object(player[0].x-(8<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
		  case 5:
		    o = spawn_object(player[0].x-(8<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION);
		  // spawn a bunch of small fragments of the earth to go flying off

		  // spawn small earth chunks in all possible directions
		  // (upleft/upright/dnleft/dnright)
		  // up/left/down/right
		  for(i=0;i<=9;i++)
		  {
			o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
			objects[o].ai.ray.direction = i;
			if (i > 4)
			{
			  objects[o].sprite = EARTHCHUNK_SMALL_DN;
			}
			else
			{
			  objects[o].sprite = EARTHCHUNK_SMALL_UP;
			}
		  }

		  break;
		  case 6:
			o = spawn_object(player[0].x+(16<<CSF), player[0].y+(16<<CSF), OBJ_EXPLOSION);
		  break;
		  case 7: o = spawn_object(player[0].x+(24<<CSF), player[0].y-(8<<CSF), OBJ_EXPLOSION); break;
		  case 8: o = spawn_object(player[0].x+(16<<CSF), player[0].y+(4<<CSF), OBJ_EXPLOSION); break;
		  case 10:
		  // spawn four big fragments of the earth to go flying off
			o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_UP;
			objects[o].ai.ray.direction = EC_UPLEFT;
			o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_UP;
			objects[o].ai.ray.direction = EC_UPRIGHT;
			o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_DN;
			objects[o].ai.ray.direction = EC_DOWNRIGHT;
			o = spawn_object(player[0].x+(16<<CSF), player[0].y, OBJ_EARTHCHUNK);
			objects[o].sprite = EARTHCHUNK_BIG_DN;
			objects[o].ai.ray.direction = EC_DOWNLEFT;
		  break;
		  case 32:
			state = TAN_STATE_GAMEOVER;
			g_pSound->playSound(SOUND_GAME_OVER, PLAY_NOW);
			pCKP->Control.levelcontrol.gameovermode = true;

			bm_gameover = g_pGfxEngine->getBitmap("GAMEOVER");
			// figure out where to center the gameover bitmap and draw it
		    x = (320/2)-(bm_gameover->getWidth()/2);
			y = (200/2)-(bm_gameover->getHeight()/2);

			break;
		  }
		  spawnedcount++;
		  timer = 60;
	  }
	  else timer--;
	break;
	case TAN_STATE_GAMEOVER:
	break;
	}

	enter = (g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_POGO));

	if (state!=TAN_STATE_GAMEOVER) gamedo_AnimatedTiles();

	gamedo_enemyai( &(pCKP->Control.levelcontrol) );

	if(((player[0].x>>CSF)-scroll_x) > 160-16) map_scroll_right();
	if (((player[0].y>>CSF)-scroll_y) > 100)
	  map_scroll_down();

	g_pInput->pollEvents();

	if(bm_gameover) bm_gameover->draw(g_pVideoDriver->FGLayerSurface, x, y );

	gamedo_RenderScreen();
  } while(!g_pInput->getPressedKey(KQUIT));
  delete_object(find_next_object(OBJ_EGA_BITMAP));
  return 1;
}

void eseq2_vibrate()
{
int xamt, yamt;
int xdir, ydir;
int vibratetimes;
int i;
int x,y,w,h;

  vibratetimes = 0;

  #define VIBRATE_AMT             16
  #define VIBRATE_NUM_FRAMES      500

  xamt = yamt = 0;
  xdir = 0; ydir = 0;
  do
  {
	// undo the scroll from last time
	if (!xdir)
	{
	  for(i=0;i<xamt;i++) map_scroll_right();
	}
	else
	{
	  for(i=0;i<xamt;i++) map_scroll_left();
	}

	if (!ydir)
	{
	  for(i=0;i<yamt;i++) map_scroll_down();
	}
	else
	{
	  for(i=0;i<yamt;i++) map_scroll_up();
	}

	// randomize a new amount to scroll this frame
	xamt = (rand()%VIBRATE_AMT);
	yamt = (rand()%VIBRATE_AMT);
	xdir = rand() & 1;
	ydir = rand() & 1;

	// scroll the map
	if (xdir)
	{
	  for(i=0;i<xamt;i++) map_scroll_right();
	}
	else
	{
	  for(i=0;i<xamt;i++) map_scroll_left();
	}

	if (ydir)
	{
	  for(i=0;i<yamt;i++) map_scroll_down();
	}
	else
	{
	  for(i=0;i<yamt;i++) map_scroll_up();
	}

	// align sprites with new scroll position
	for(i=0;i<MAX_OBJECTS;i++)
	{
	  if (objects[i].exists && objects[i].type!=OBJ_PLAYER)
	  {
		objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
		objects[i].scry = (objects[i].y>>CSF)-scroll_y;
	  }
	}
	vibratetimes++;
	g_pInput->pollEvents();

    	 gamedo_RenderScreen();

  } while(!g_pInput->getPressedKey(KQUIT) && vibratetimes < VIBRATE_NUM_FRAMES);

  // display the "uh-oh."
  x = GetStringAttribute("EP2_AfterVibrateString", "LEFT");
  y = GetStringAttribute("EP2_AfterVibrateString", "TOP");
  w = GetStringAttribute("EP2_AfterVibrateString", "WIDTH");
  h = GetStringAttribute("EP2_AfterVibrateString", "HEIGHT");

  eseq_showmsg(getstring("EP2_AfterVibrateString"),x,y,w,h,1);
}


#define HEADFOREARTH_X          0
#define HEADFOREARTH_Y          0

#define SPR_SHIP_RIGHT_EP2      132
#define SPR_SHIP_LEFT_EP2       133
#define SPR_VORTICON_MOTHERSHIP 72

#define HEADSFOREARTH_X        3
#define HEADSFOREARTH_Y        15
#define HEADSFOREARTH_W        33
#define HEADSFOREARTH_H        8

int eseq2_HeadsForEarth(stCloneKeenPlus *pCKP)
{
bool enter;
int x, y;
int downtimer;
int afterfadewaittimer;

  g_pInput->flushAll();

  initgame( &(pCKP->Control.levelcontrol) );

  // set up the ship's route
  ShipQueuePtr = 0;
  addshipqueue(CMD_WAIT, 10, 0);
  addshipqueue(CMD_MOVE, 2360, DDOWNRIGHT);
  addshipqueue(CMD_FADEOUT, 0, 0);

  showmapatpos(81, HEADFOREARTH_X, HEADFOREARTH_Y, pCKP);

  numplayers = 1;
  // place the player near the vorticon mothership
  if (map_findtile(593, &x, &y))
  {
    player[0].x = ((x<<4)+2)<<CSF;
    player[0].y = ((y<<4)-24)<<CSF;
  }
  else
  {
      crashflag = 1;
      why_term_ptr = "eseq2_HeadForEarth(): unable to find Vorticon Mothership.";
      return 1;
  }

  player[0].playframe = SPR_SHIP_RIGHT_EP2;

  ShipQueuePtr = 0;
  max_scroll_x = max_scroll_y = 20000;

  eseq_showmsg(getstring("EP2_ESEQ_PART1"),HEADSFOREARTH_X,HEADSFOREARTH_Y,HEADSFOREARTH_W,HEADSFOREARTH_H, true);

  // erase the message dialog
  map_redraw();

  downtimer = 0;
  afterfadewaittimer = 0;
  do
  {
	  switch(shipqueue[ShipQueuePtr].cmd)
	  {
		case CMD_MOVE:
		// down-right only, here
		  player[0].x+=9;
		  player[0].y+=4;
		  // we need a little bit more resolution that we can get--
		  // the Y speed needs to be somewhere between 4 and 5 for
		  // him to end up at the center of the earth
		  if (downtimer > 6)
		  {
			player[0].y++;
			downtimer = 0;
		  }
		  else downtimer++;
		break;
		case CMD_WAIT:
		break;

		break;
		default: break;
	  }
	  // decrease the time remaining
	  if (shipqueue[ShipQueuePtr].time)
		shipqueue[ShipQueuePtr].time--;
	  else // no time left on this command, go to next cmd
		ShipQueuePtr++;

	  if ( afterfadewaittimer > 80 || enter ) return 0;
	  else afterfadewaittimer++;

	  enter = (g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_POGO));

	  gamedo_AnimatedTiles();
	  gamedo_ScrollTriggers(0);
	  gamedo_render_drawobjects();
	  gamedo_RenderScreen();
	  g_pInput->pollEvents();
  } while(!g_pInput->getPressedKey(KQUIT));
  return 1;
}

#define LIMPSHOME_X          0
#define LIMPSHOME_Y          344

int eseq2_LimpsHome(stCloneKeenPlus *pCKP)
{
bool enter;
int downtimer;
int afterfadewaittimer = 0;

  initgame( &(pCKP->Control.levelcontrol) );
  g_pInput->flushAll();

  // set up the ship's route
  ShipQueuePtr = 0;
  addshipqueue(CMD_WAIT, 10, 0);
  addshipqueue(CMD_MOVE, 1600, DUPLEFT);
  addshipqueue(CMD_FADEOUT, 0, 0);

  showmapatpos(81, LIMPSHOME_X, LIMPSHOME_Y, pCKP);

  numplayers = 1;
  player[0].x = (10 <<4<<CSF);
  player[0].y = (26 <<4<<CSF);

  player[0].playframe = SPR_VORTICON_MOTHERSHIP;

  ShipQueuePtr = 0;

  eseq_showmsg(getstring("EP2_ESEQ_PART2"),HEADSFOREARTH_X,HEADSFOREARTH_Y,HEADSFOREARTH_W,HEADSFOREARTH_H-1,1);

  // erase the message dialog
  map_redraw();

  downtimer = 0;
  do
  {
	  switch(shipqueue[ShipQueuePtr].cmd)
	  {
		case CMD_MOVE:
		// up-left only, here
		  player[0].x-=1;
		  player[0].y-=2;
		break;
		case CMD_WAIT:
		break;
		case CMD_FADEOUT:
		break;
		default: break;
	  }
	  // decrease the time remaining
	  if ( shipqueue[ShipQueuePtr].time )	shipqueue[ShipQueuePtr].time--;
	  else	ShipQueuePtr++;

	  if ( afterfadewaittimer > 80 || enter )	return 0;
	  else afterfadewaittimer++;

	  enter = (g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_POGO));

	  gamedo_AnimatedTiles();
	  gamedo_render_drawobjects();
	  gamedo_RenderScreen();
	  g_pInput->pollEvents();

  } while(!g_pInput->getPressedKey(KQUIT));
  return 1;
}

int eseq2_SnowedOutside(stCloneKeenPlus *pCKP)
{
int curpage;
int lastpage;
std::string tempstr;
std::string text;
int dlgX, dlgY, dlgW, dlgH;

  g_pInput->flushAll();
  scrollx_buf = scroll_x = 0;
  scrolly_buf = scroll_y = 0;

  SDL_Surface *finale_sfc = SDL_CreateRGBSurface( g_pVideoDriver->BlitSurface->flags, 320, 200, 8, 0, 0, 0, 0);
  SDL_SetColors( finale_sfc, g_pGfxEngine->Palette.m_Palette, 0, 255);
  finale_draw( finale_sfc, "finale.ck2", pCKP->Resources.GameDataDirectory);

  curpage = 1;

  numplayers = 1;
  player[0].x = player[0].y = 0;
  player[0].playframe = BlankSprite;

  do
  {
		// Draw uncompressed finale Plot
		SDL_BlitSurface( finale_sfc, NULL, g_pVideoDriver->BlitSurface, NULL );

		tempstr = "EP2_ESEQ_PART3_PAGE" + itoa(curpage);
		text = getstring(tempstr);
		dlgX = GetStringAttribute(tempstr, "LEFT");
		dlgY = GetStringAttribute(tempstr, "TOP");
		dlgW = GetStringAttribute(tempstr, "WIDTH");
		dlgH = GetStringAttribute(tempstr, "HEIGHT");
		lastpage = GetStringAttribute(tempstr, "LASTPAGE");

		eseq_showmsg( text, dlgX, dlgY, dlgW, dlgH, 1, finale_sfc );
		if (lastpage==1) break;

		curpage++;
  } while(!g_pInput->getPressedKey(KQUIT));

  finale_draw( finale_sfc, "finale.ck2", pCKP->Resources.GameDataDirectory);
  // Draw uncompressed finale Plot
  SDL_BlitSurface( finale_sfc, NULL, g_pVideoDriver->BlitSurface, NULL );
  eseq_ToBeContinued( finale_sfc );
  SDL_FreeSurface( finale_sfc );

  return 0;
}
