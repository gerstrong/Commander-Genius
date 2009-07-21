/* GAME.C
  Main and miscellaneous functions for in-game, contains the main
  game loop, etc.
*/

#include "keen.h"
#include "demobox.h"
#include "include/game.h"
#include "sdl/CVideoDriver.h"
#include "include/menu.h"
#include "include/misc.h"
#include "include/gamedo.h"
#include "include/gamepdo.h"
#include "include/gm_pdowm.h"
#include "include/eseq_ep3.h"
#include "sdl/CTimer.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "include/enemyai.h"
#include "hqp/CMusic.h"
#include "vorticon/CPlayer.h"
#include "vorticon/CHighScores.h"
#include "hqp/CHQBitmap.h"
char otherplayer;

// TODO: seperate status boxes for the different players
// TODO: Your Ship Needs These Parts in multiplayer

int playerbaseframes[MAX_PLAYERS] = {0,0,0,0,0,0,0,0};

unsigned int max_scroll_x, max_scroll_y;
char debugmode=0,acceleratemode=0;

// and this is where the magic happens
void gameloop(stCloneKeenPlus *pCKP)
{
	unsigned int i;

  int enter,lastquit;

  // Enable the yorp/garg statues elders switches animations
  for(int i=0 ; i<numtiles ; i++)
	  if(TileProperty[i][BEHAVIOR] == 22)
		  TileProperty[i][ANIMATION] = 4;

  if (player[0].x==0 || player[0].y==0)

  {
    crashflag=1;
    crashflag2 = pCKP->Control.levelcontrol.curlevel;
    crashflag3 = pCKP->Control.levelcontrol.episode;
    why_term_ptr = "No player start position! (flag2=pCKP->Control.levelcontrol.curlevel, flag3=pCKP->Control.levelcontrol.episode)";
  }

  if (!loadinggame)
  {
     gameloop_initialize(pCKP);
  }
  else
  {
     loadinggame = 0;
     fade.mode = FADE_GO;
     fade.dir = FADE_IN;
     fade.curamt = 0;
     fade.fadetimer = 0;
     fade.rate = FADE_NORM;
  }

  // fire all guns immediately first time around
  gunfiretimer = (gunfirefreq+1);

  // if this is Mortimer's Castle, fade in and do the conversation
  // with Mortimer.
  if (pCKP->Control.levelcontrol.episode==3 && pCKP->Control.levelcontrol.curlevel==16)
  {
     for(i=0;i<MAX_PLAYERS;i++)
     {
       if (player[i].isPlaying) gamepdo_SelectFrame(i, pCKP);
     }

     do
     {
        gamedo_fades();
        gamedo_AnimatedTiles();
        g_pInput->pollEvents();
        g_pTimer->SpeedThrottle();
        gamedo_RenderScreen(pCKP);
     } while(fade.mode!=FADE_COMPLETE /*&& !immediate_keytable[KQUIT]*/);

     eseq3_Mortimer(pCKP);
  }

  lastquit = 1;
  g_pInput->flushKeys(); // The Windows need that. I don't know why!

  // Now, we are ready to loop the game scenes (map and level)
  // Let's create the player objects
  do
  {
     if (primaryplayer==1) otherplayer = 0; else otherplayer = 1;

     #ifdef NETWORK_PLAY
//       if (numplayers>1) net_getdata();
         if (is_server)
         {
           Net_Server_Run();
         }
         else if (is_client)
         {
           Net_Client_Run();
         }
     #endif

     gamedo_fades();

     // periodically make all enemy gun fixtures fire (in ep3)
     // (also ice cannons in ep1) we do this in a global variable
     // so they're all in sync. when gunfiretimer==0 all gun SE
     // objects will fire.
     if (gunfiretimer > gunfirefreq)
     {
       gunfiretimer = 0;
     }
     else gunfiretimer++;

     // gather input and copy to player[].keytable[] structures
     gamedo_getInput(pCKP);

     // run the player behaviour for each player in the game
   	 if (!map.isworldmap)
     {
   		for(i=0;i<MAX_PLAYERS;i++)
   		{
    		if (player[i].isPlaying) gamepdo_HandlePlayer(i, pCKP);
    	}
     }
     else
     {
    	for(i=0;i<MAX_PLAYERS;i++)
    	{
    	 if (player[i].isPlaying) gamepdo_wm_HandlePlayer(i, pCKP);
    	}
   	 }

     gamedo_AnimatedTiles();
     gamedo_enemyai(pCKP);

     gamedo_HandleFKeys(pCKP);

     /* scroll triggers */
     if (!pCKP->Control.levelcontrol.gameovermode && pCKP->Control.levelcontrol.level_done==LEVEL_NOT_DONE)
     {
        ScreenIsScrolling = 0;
        if (gamedo_ScrollTriggers(primaryplayer)) ScreenIsScrolling = 1;
     }



     // do frameskipping, and render/blit the screen if it's time
     gamedo_frameskipping(pCKP);

     // when we complete a fade out flag to exit the game loop
     if (fade.mode==FADE_COMPLETE)
     {
         if (fade.dir==FADE_OUT)
         {
            demomode = DEMO_NODEMO;
            pCKP->Control.levelcontrol.level_done = LEVEL_COMPLETE;
            pCKP->Control.levelcontrol.command = LVLC_CHANGE_LEVEL;
            if (pCKP->Control.levelcontrol.curlevel != WM_MAP_NUM)
            { // exiting a level, going back to world map
                for(i=0;i<numplayers;i++)
                {
               		player[i].inventory.HasCardYellow = 0;
               		player[i].inventory.HasCardBlue = 0;
               		player[i].inventory.HasCardGreen = 0;
               		player[i].inventory.HasCardRed = 0;
                }

              if (pCKP->Control.levelcontrol.success==1)
              { // mark level as completed on world map
                pCKP->Control.levelcontrol.levels_completed[pCKP->Control.levelcontrol.curlevel] = 1;
              }
              pCKP->Control.levelcontrol.chglevelto = WM_MAP_NUM;
            }
         }
         else
         {
           fade.mode = NO_FADE;
         }
     }

     // when walking through the exit door don't show keen's sprite past
     // the door frame (so it looks like he walks "through" the door)
     if (pCKP->Control.levelcontrol.level_done==LEVEL_DONE_WALK)
     {
        gamepdo_walkbehindexitdoor(pCKP->Control.levelcontrol.level_finished_by, pCKP);
     }

     // allow enter to return to main menu
     // if we're in game over mode

     enter = (g_pInput->getPressedCommand(IC_STATUS));
     if (pCKP->Control.levelcontrol.gameovermode)
     {
       if (enter)
       {
    	 int cities=0;
      	 CHighScores *HighScoreTable;

     	 if (pCKP->Control.levelcontrol.levels_completed[4]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[6]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[7]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[13]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[11]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[9]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[15]) cities++;
      	 if (pCKP->Control.levelcontrol.levels_completed[16]) cities++;

      	 HighScoreTable = new CHighScores(pCKP);

      	 bool extras[4];

      	 // check inventory or saved cities
      	 memset(extras,false,4*sizeof(bool));
      	 if(pCKP->Control.levelcontrol.episode == 1)
      	 {
      		 if(player[0].inventory.HasJoystick)
      			 extras[0] = true;
      		 if(player[0].inventory.HasBattery)
      			 extras[1] = true;
      		 if(player[0].inventory.HasVacuum)
      			 extras[2] = true;
      		 if(player[0].inventory.HasFuel)
      			 extras[3] = true;
      	 }

      	 HighScoreTable->writeHighScore((int)player[0].inventory.score,extras,cities);

      	 HighScoreTable->showHighScore();

      	 delete HighScoreTable;


         if (fade.mode!=FADE_GO && fade.dir!=FADE_OUT)
         {
           fade.dir = FADE_OUT;
           fade.curamt = PAL_FADE_SHADES;
           fade.fadetimer = 0;
           fade.rate = FADE_NORM;
           fade.mode = FADE_GO;
         }

         pCKP->Control.levelcontrol.command = LVLC_GAME_OVER;

       }

       if (fade.mode==FADE_COMPLETE && fade.dir==FADE_OUT)
       {
         pCKP->Control.levelcontrol.command = LVLC_GAME_OVER;
       }
     }

     #ifdef NETWORK_PLAY
//       if (numplayers>1) net_senddata();
     #endif

     if (g_pInput->getPressedKey(KQUIT))
     {
       VerifyQuit(pCKP);
     }
     if (QuitState != NO_QUIT) return;

     // limit frame rate
     if (!acceleratemode)
     {
    	 g_pInput->pollEvents();
    	 g_pTimer->SpeedThrottle();
        #ifdef NETWORK_PLAY
//          if (numplayers>1) net_sync();
        #endif
     }

     if(g_pInput->getExitEvent())
     {
    	 g_pInput->sendKey(KQUIT);
    	 g_pInput->cancelExitEvent();
     }

  } while(!crashflag && pCKP->Control.levelcontrol.command==LVLC_NOCOMMAND);

  // Cleanup the player structure!

}

// gives keycard for door doortile to player p
void give_keycard(int doortile, int p)
{
	bool keystack = options[OPT_KEYCARDSTACK].value; // True if set
	g_pSound->playSound(SOUND_GET_CARD, PLAY_NOW);

	if (doortile==DOOR_YELLOW) player[p].inventory.HasCardYellow = keystack ? player[p].inventory.HasCardYellow + 1 : 1;
	else if (doortile==DOOR_RED) player[p].inventory.HasCardRed = keystack ? player[p].inventory.HasCardRed + 1 : 1;
	else if (doortile==DOOR_GREEN) player[p].inventory.HasCardGreen = keystack ? player[p].inventory.HasCardGreen + 1 : 1;
	else if (doortile==DOOR_BLUE) player[p].inventory.HasCardBlue = keystack ? player[p].inventory.HasCardBlue + 1 : 1;
	else
	{
		crashflag = 1;
		crashflag2 = doortile;
		why_term_ptr = "give_keycard(): invalid value for doortile parameter.";
	}
}

// take away the specified keycard from player p
void take_keycard(int doortile, int p)
{
	if (doortile==DOOR_YELLOW) player[p].inventory.HasCardYellow--;
	else if (doortile==DOOR_RED) player[p].inventory.HasCardRed--;
	else if (doortile==DOOR_GREEN) player[p].inventory.HasCardGreen--;
	else if (doortile==DOOR_BLUE) player[p].inventory.HasCardBlue--;

	if(player[p].inventory.HasCardYellow > 9) player[p].inventory.HasCardYellow = 0;
	if(player[p].inventory.HasCardRed > 9) player[p].inventory.HasCardRed = 0;
	if(player[p].inventory.HasCardGreen > 9) player[p].inventory.HasCardGreen = 0;
	if(player[p].inventory.HasCardBlue > 9) player[p].inventory.HasCardBlue = 0;
}

// unregisters all animated tiles with baseframe tile
void unregister_animtiles(int tile)
{
int i;
     for(i=0;i<MAX_ANIMTILES-1;i++)
     {
        if (animtiles[i].baseframe == tile)
        {
           animtiles[i].slotinuse = 0;
        }
     }
}

void open_door(int doortile, int doorsprite, int mpx, int mpy, int cp, stCloneKeenPlus *pCKP)
{
int o,chgtotile;
short tilefix=0;

	g_pSound->playSound(SOUND_DOOR_OPEN, PLAY_NOW);

	take_keycard(doortile, cp);

   /* erase door from map */
   if (pCKP->Control.levelcontrol.episode==3)
   {
     chgtotile = map.mapdata[mpx-1][mpy];
   }
   else
   {
     chgtotile = tiles[map.mapdata[mpx][mpy]].chgtile;
   }

   if(TileProperty[map.mapdata[mpx][mpy-1]][BEHAVIOR] > 1 &&
		   TileProperty[map.mapdata[mpx][mpy-1]][BEHAVIOR] < 6) // This happens because, sometimes the player opens the door
															  // from a lower part.
   {
	   map_chgtile(mpx, mpy-1, chgtotile);
	   tilefix=1;

   }
   if(TileProperty[map.mapdata[mpx][mpy]][BEHAVIOR] > 1 &&
		   TileProperty[map.mapdata[mpx][mpy]][BEHAVIOR] < 6) // This happens because, sometimes the player opens the door
															  // from a lower part.
   {
	   map_chgtile(mpx, mpy, chgtotile); // upper?

   }
   if(TileProperty[map.mapdata[mpx][mpy+1]][BEHAVIOR] > 1 &&
		   TileProperty[map.mapdata[mpx][mpy+1]][BEHAVIOR] < 6) // This happens because, sometimes the player opens the door
															  // from a lower part.
   {
	   map_chgtile(mpx, mpy+1, chgtotile); // When he stands in front of the door!
   }

   // replace the door tiles with a door object, which will do the animation
   o = spawn_object(mpx<<4<<CSF,(mpy-tilefix)<<4<<CSF,OBJ_DOOR);
   objects[o].sprite = doorsprite;
}

// checks if score is > than "extra life at" and award 1-UPs when appropriate
void extralifeat(int cp)
{
  if (player[cp].inventory.score > player[cp].inventory.extralifeat)
  {
	  g_pSound->playSound(SOUND_EXTRA_LIFE, PLAY_NOW);
	  player[cp].inventory.lives++;
	  player[cp].inventory.extralifeat += 20000;
  }
}

// have keen pick up the goodie at screen pixel position (px, py)
void keen_get_goodie(int px, int py, int theplayer, stCloneKeenPlus *pCKP)
{
int mpx,mpy,t;
/*int i;*/
   mpx = px>>4;
   mpy = py>>4;
   t = map.mapdata[mpx][mpy];

   if ((TileProperty[t][BEHAVIOR] < 17 && TileProperty[t][BEHAVIOR] > 5) ||
	   (TileProperty[t][BEHAVIOR] > 17 && TileProperty[t][BEHAVIOR] < 22) ||
	   (TileProperty[t][BEHAVIOR] == 27 || TileProperty[t][BEHAVIOR] == 28)   ) // All pickupable items
   //if (tiles[t].pickupable)
   {  // pick up the goodie, i.e. erase it from the map
      map_chgtile(mpx, mpy, tiles[t].chgtile);
      //if (tiles[t].isAnimated) map_deanimate(mpx, mpy);
      if (TileProperty[t][ANIMATION] != 1) map_deanimate(mpx, mpy);
   }
   else if (TileProperty[t][BEHAVIOR] == 1)
   //else if (tiles[t].lethal)
   {  // whoah, this "goodie" isn't so good...
      killplayer(theplayer, pCKP);
      return;
   }

   // do whatever the goodie is supposed to do...
   procgoodie(t, mpx, mpy, theplayer, pCKP);
}

void initgame(stCloneKeenPlus *pCKP)
{
int x,y;
unsigned int i;

  animtiletimer = curanimtileframe = 0;
  //PlatExtending = 0;

  // reset player walk frame widths
  for(i=0;i<numplayers;i++)
  {
    sprites[playerbaseframes[i]+0].xsize = PLAYERSPRITE_WIDTH;
    sprites[playerbaseframes[i]+1].xsize = PLAYERSPRITE_WIDTH;
    sprites[playerbaseframes[i]+2].xsize = PLAYERSPRITE_WIDTH;
    sprites[playerbaseframes[i]+3].xsize = PLAYERSPRITE_WIDTH;
  }

  // set gun/ice cannon fire freq
  if (pCKP->Control.levelcontrol.episode==1)
  {
    gunfirefreq = ICECANNON_FIRE_FREQ;
  }
  else
  {
    gunfirefreq = GUN_FIRE_FREQ;
  }

  // reset the ysize attribute of all doors
  sprites[DOOR_YELLOW_SPRITE].ysize = 32;
  sprites[DOOR_RED_SPRITE].ysize = 32;
  sprites[DOOR_GREEN_SPRITE].ysize = 32;
  sprites[DOOR_BLUE_SPRITE].ysize = 32;

  pCKP->Control.levelcontrol.level_done_timer = 0;
  pCKP->Control.levelcontrol.gameovermode = 0;

  // all objects -> not exist
 for(i=1;i<MAX_OBJECTS-1;i++) objects[i].exists = 0;
 // clear out AnimTileInUse array
 for(y=0;y<ATILEINUSE_SIZEX-1;y++)
 {
   for(x=0;x<ATILEINUSE_SIZEY-1;x++)
   {
      AnimTileInUse[x][y] = 0;
   }
 }
 // set all animated tile slots to "not in use"
 map_unregister_all_animtiles();

// initilize default sprites for objects
  memset(objdefsprites, 0, sizeof(objdefsprites));
  if (pCKP->Control.levelcontrol.episode==1)
  {
    objdefsprites[OBJ_YORP] = OBJ_YORP_DEFSPRITE;
    objdefsprites[OBJ_GARG] = OBJ_GARG_DEFSPRITE;
    objdefsprites[OBJ_BUTLER] = OBJ_BUTLER_DEFSPRITE;
    objdefsprites[OBJ_TANK] = OBJ_TANK_DEFSPRITE;
    objdefsprites[OBJ_ICECHUNK] = OBJ_ICECHUNK_DEFSPRITE;
    objdefsprites[OBJ_ICEBIT] = OBJ_ICEBIT_DEFSPRITE;
    objdefsprites[OBJ_ROPE] = OBJ_ROPE_DEFSPRITE;

    objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP1;
    objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP1;
  }
  else if (pCKP->Control.levelcontrol.episode==2)
  {
    objdefsprites[OBJ_WALKER] = OBJ_WALKER_DEFSPRITE;
    objdefsprites[OBJ_TANKEP2] = OBJ_TANKEP2_DEFSPRITE;
    objdefsprites[OBJ_BEAR] = OBJ_BEAR_DEFSPRITE;

    objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP2;
    objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP2;
    objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP2;
    objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP2;
  }
  else if (pCKP->Control.levelcontrol.episode==3)
  {
    objdefsprites[OBJ_FOOB] = OBJ_FOOB_DEFSPRITE;
    objdefsprites[OBJ_NINJA] = OBJ_NINJA_DEFSPRITE;
    objdefsprites[OBJ_MOTHER] = OBJ_MOTHER_DEFSPRITE;
    objdefsprites[OBJ_MEEP] = OBJ_MEEP_DEFSPRITE;
    objdefsprites[OBJ_BALL] = OBJ_BJ_DEFSPRITE;
    objdefsprites[OBJ_JACK] = OBJ_BJ_DEFSPRITE;

    objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP3;
    objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP3;
    objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP3;
    objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP3;
  }

  objdefsprites[OBJ_DOOR] = DOOR_YELLOW_SPRITE;
  objdefsprites[OBJ_TELEPORTER] = OBJ_TELEPORTER_DEFSPRITE;

  objdefsprites[OBJ_SECTOREFFECTOR] = BlankSprite;

// initilize game variables
  pCKP->Control.levelcontrol.level_done = LEVEL_NOT_DONE;
  animtiletimer = curanimtileframe = 0;
  DemoObjectHandle = 0;

  for(i=0;i<MAX_PLAYERS;i++)
  {
    player[i].isPlaying = 0;
  }

  for(i=0;i<numplayers;i++)
  {
    player[i].isPlaying = 1;
    player[i].useObject = (i+1); // player objects start at 1 cause DemoObject is index 0
    player[i].dpadcount = player[i].dpadlastcount = 0;
    player[i].hideplayer = 0;
    player[i].mounted = 0;
    player[i].ppogostick = false;
    player[i].pjumping = 0;
    player[i].pfalling = 0;
    player[i].pwalking = player[i].playspeed = 0;
    player[i].pinertia_x = player[i].pinertia_y = 0;
    player[i].pboost_x = 0;
    player[i].playpushed_x = 0;
    player[i].pfiring = 0;
    player[i].psliding = player[i].psemisliding = 0;
    player[i].pdie = 0;

    player[i].pfrozentime = 0;
    player[i].ankhtime = 0;
    player[i].keyprocstate = 0;         // KPROC_IDLE
    player[i].pjustjumped = player[i].pjustfell = 0;
  }
  // each player is tied to a "puppet" object.
  // initialize objects used by players.
  int o;
  for(i=0;i<numplayers;i++)
  {
	  o = player[i].useObject;
	  if (player[i].isPlaying)
	  {
    	objects[o].exists = 1;
    	objects[o].onscreen = 1;
    	objects[o].type = OBJ_PLAYER;
    	objects[o].sprite = 0;
    	objects[o].onscreen = 1;
    	objects[o].AssociatedWithPlayer = i;
    	objects[o].honorPriority = 1;
	  }
  }


  scroll_x = 0;
  scrollx_buf = 0;
  scrollpix = 0;
  mapx = 0;
  mapxstripepos = 0;

  scroll_y = 0;
  scrolly_buf = 0;
  scrollpixy = 0;
  mapy = 0;
  mapystripepos = 0;

  thisplayer = 0;
}

int initgamefirsttime(stCloneKeenPlus *pCKP, int s)
{
	int i;

	map.firsttime = 1;

	memset(player, 0, sizeof(player));

	for(i=0;i<MAX_LEVELS;i++)
	   pCKP->Control.levelcontrol.levels_completed[i] = 0;

	for(i=0;i<MAX_PLAYERS;i++)
	{
       player[i].inventory.extralifeat = 20000;
       player[i].inventory.lives = 4;
       player[i].godmode = 0;

       if (pCKP->Control.levelcontrol.episode==1)
       {
         player[i].inventory.charges = 0;
       }
       else if (pCKP->Control.levelcontrol.episode==2)
       {
         player[i].inventory.charges = 3;
       }
       else
       {
         player[i].inventory.charges = 5;
       }
       if (demomode) player[i].inventory.charges = 100;

       // start with pogo stick in all episodes but 1
       if (pCKP->Control.levelcontrol.episode!=1 || demomode)
         { player[i].inventory.HasPogo = 1; }
       else
         { player[i].inventory.HasPogo = 0; }
   }

   initsprites(pCKP, s);

   if (demomode) srand(375);

   primaryplayer = 0;

   return 0;
}

char spawn_object(int x, int y, int otype)
{
int i;
 // find an unused object slot
 for(i=1;i<MAX_OBJECTS-1;i++)
 {
   if (!objects[i].exists && objects[i].type != OBJ_PLAYER)
   {
     objects[i].x = x;
     objects[i].y = y;
     objects[i].type = otype;
     objects[i].sprite = objdefsprites[otype];
     objects[i].exists = 1;
     objects[i].needinit = 1;
     objects[i].onscreen = 0;
     objects[i].hasbeenonscreen = 0;
     objects[i].canbezapped = 0;
     objects[i].zapped = 0;
     objects[i].inhibitfall = 0;
     objects[i].honorPriority = 1;
     SetAllCanSupportPlayer(i, 0);
     return i;
   }
 }
 // object could not be created
 crashflag = 1;
 crashflag2 = otype;
 crashflag3 = 0;
 why_term_ptr = "Object could not be created.";
 return 0;
}

// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void common_enemy_ai(int o)
{
int x,y,xa,ya,xsize,ysize;
int temp;
int cplayer;

       if (objects[o].type==OBJ_DEMOMSG) return;

       xsize = sprites[objects[o].sprite].xsize;
       ysize = sprites[objects[o].sprite].ysize;

 // set value of blockedd--should object fall?
       temp = (objects[o].y>>CSF)+ysize;
       if ((temp>>4)<<4 != temp)
       {
          objects[o].blockedd = 0;
       }
       else
       { // on a tile boundary, test if tile under object is solid
          objects[o].blockedd = 0;
          x = (objects[o].x>>CSF);
          y = (objects[o].y>>CSF)+ysize+1;
          for(xa=0;xa<xsize-2;xa+=16)
          {
        	if(TileProperty[getmaptileat(x+xa,y)][BUP] || (TileProperty[getmaptileat(x+xa,y)][BEHAVIOR] > 1 &&
        	TileProperty[getmaptileat(x+xa,y)][BEHAVIOR] < 6) )
            {
              objects[o].blockedd = 1;
              goto setblockedd;
            }
          }
          if(TileProperty[getmaptileat(x+xsize-2, y)][BUP] || (TileProperty[getmaptileat(x+xa,y)][BEHAVIOR] > 1 &&
        		  TileProperty[getmaptileat(x+xa,y)][BEHAVIOR] < 6)  )
          {
             objects[o].blockedd = 1;
          }
          setblockedd: ;
       }

  // set blockedu
    objects[o].blockedu = 0;
    x = (objects[o].x>>CSF);
    y = (objects[o].y>>CSF)-1;
    for(xa=0;xa<xsize;xa+=16)
    {
    	if(TileProperty[getmaptileat(x+xa,y)][BDOWN])
        //if (tiles[getmaptileat(x+xa,y)].solidceil)
        {
          objects[o].blockedu = 1;
          goto setblockedu;
        }
    }
    if(TileProperty[getmaptileat(x+xsize-1, y)][BDOWN])
    //if (tiles[getmaptileat(x+xsize-1, y)].solidceil)
    {
      objects[o].blockedu = 1;
    }
    setblockedu: ;

 // set blockedl
    objects[o].blockedl = 0;
    x = (objects[o].x>>CSF)-1;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
    	if(TileProperty[getmaptileat(x,y+ya)][BRIGHT])
        //if (tiles[getmaptileat(x,y+ya)].solidr)
        {
          objects[o].blockedl = 1;
          goto setblockedl;
        }
    }
    if(TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BRIGHT])
    //if (tiles[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))].solidr)
    {
      objects[o].blockedl = 1;
    }
    setblockedl: ;

 // set blockedr
    objects[o].blockedr = 0;
    x = (objects[o].x>>CSF)+xsize;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
    	if(TileProperty[getmaptileat(x,y+ya)][BRIGHT])
        //if (tiles[getmaptileat(x,y+ya)].solidl)
        {
          objects[o].blockedr = 1;
          goto setblockedr;
        }
    }
    if(TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BLEFT])
    //if (tiles[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))].solidl)
    {
      objects[o].blockedr = 1;
    }
    setblockedr: ;

    // hit detection with players
    objects[o].touchPlayer = 0;
    for(cplayer=0;cplayer<MAX_PLAYERS;cplayer++)
    {
      if (player[cplayer].isPlaying)
      {
        objects[player[cplayer].useObject].x = player[cplayer].x;
        objects[player[cplayer].useObject].y = player[cplayer].y;
        objects[player[cplayer].useObject].sprite = 0;
        if (!player[cplayer].pdie)
        {
          if (hitdetect(o, player[cplayer].useObject))
          {
            objects[o].touchPlayer = 1;
            objects[o].touchedBy = cplayer;
            break;
          }
        }
      }
    }

// have object fall if it should
  if (!objects[o].inhibitfall)
  {
       #define OBJFALLSPEED   20
       if (objects[o].blockedd)
       {
         objects[o].yinertia = 0;
       }
       else
       {
#define OBJ_YINERTIA_RATE  5
         if (objects[o].yinertiatimer>OBJ_YINERTIA_RATE)
         {
           if (objects[o].yinertia < OBJFALLSPEED) objects[o].yinertia++;
           objects[o].yinertiatimer = 0;
         } else objects[o].yinertiatimer++;
       }
       objects[o].y += objects[o].yinertia;
  }
}

// returns nonzero if object1 overlaps object2
char hitdetect(int object1, int object2)
{
int s1, s2;
unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
unsigned int rect2x1, rect2y1, rect2x2, rect2y2;

  // get the sprites used by the two objects
  s1 = objects[object1].sprite;
  s2 = objects[object2].sprite;

  // get the bounding rectangle of the first object
  rect1x1 = objects[object1].x + sprites[s1].bboxX1;
  rect1y1 = objects[object1].y + sprites[s1].bboxY1;
  rect1x2 = objects[object1].x + sprites[s1].bboxX2;
  rect1y2 = objects[object1].y + sprites[s1].bboxY2;

  // get the bounding rectangle of the second object
  rect2x1 = objects[object2].x + sprites[s2].bboxX1;
  rect2y1 = objects[object2].y + sprites[s2].bboxY1;
  rect2x2 = objects[object2].x + sprites[s2].bboxX2;
  rect2y2 = objects[object2].y + sprites[s2].bboxY2;

  // find out if the rectangles overlap
  if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return 0;
  if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return 0;
  if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return 0;
  if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return 0;

  return 1;
}

void killplayer(int theplayer, stCloneKeenPlus *pCKP)
{
   if (player[theplayer].godmode || g_pInput->getHoldedKey(KTAB)) return;
   if (player[theplayer].ankhtime) return;
   if (pCKP->Control.levelcontrol.level_done) return;
   if (!player[theplayer].pdie)
   {
      player[theplayer].pdie = PDIE_DYING;
      player[theplayer].pdieframe = 0;
      player[theplayer].pdietimer = 0;
      player[theplayer].pdietillfly = DIE_TILL_FLY_TIME;
      player[theplayer].pdie_xvect = rand()%(DIE_MAX_XVECT*2);
      player[theplayer].pdie_xvect -= DIE_MAX_XVECT;
      player[theplayer].inventory.lives--;
      player[theplayer].y += (8<<CSF);
      gamepdo_SelectFrame(theplayer, pCKP);
      g_pMusicPlayer->stop();
      g_pSound->playSound(SOUND_KEEN_DIE, PLAY_NOW);
   }
}

void freezeplayer(int theplayer)
{
   if (player[theplayer].godmode /*|| immediate_keytable[KTAB]*/) return;
   if (player[theplayer].ankhtime) return;
   // give the player a little "kick"
   player[theplayer].pjumptime = PJUMP_NORMALTIME_1;
   player[theplayer].pjumpupdecreaserate = PJUMP_UPDECREASERATE_1;
   player[theplayer].pjumpupspeed = 15;
   player[theplayer].pjumping = PJUMPUP;
   player[theplayer].pjumpupspeed_decreasetimer = 0;
   player[theplayer].pjustjumped = 1;

   // and freeze him (stun him on ep2/3)
   player[theplayer].pfrozentime = PFROZEN_TIME;
   player[theplayer].pfrozenframe = 0;
   player[theplayer].pfrozenanimtimer = 0;
   player[theplayer].ppogostick = false;
}


void PlayerTouchedExit(int theplayer, stCloneKeenPlus *pCKP)
{
/*int i;*/
       if (!player[theplayer].pjumping && !player[theplayer].pfalling\
           && !player[theplayer].ppogostick && \
           pCKP->Control.levelcontrol.level_done==LEVEL_NOT_DONE)
       {
            // don't allow player to walk through a door if he's standing
            // on an object such as a platform or an enemy
            if (player[theplayer].psupportingobject)
            {
              return;
            }

            // if player has ankh shut it off
            if (player[theplayer].ankhtime)
            {
              player[theplayer].ankhtime = 0;
              objects[player[theplayer].ankhshieldobject].exists = 0;
            }

            player[theplayer].ppogostick = false;

            g_pMusicPlayer->stop();
            g_pSound->playSound(SOUND_LEVEL_DONE, PLAY_NOW);
            pCKP->Control.levelcontrol.level_done = LEVEL_DONE_WALK;
            pCKP->Control.levelcontrol.level_finished_by = theplayer;
       }
}

void endlevel(int success, stCloneKeenPlus *pCKP)
{
  if (fade.mode == NO_FADE)
  {
    fade.dir = FADE_OUT;
    fade.curamt = PAL_FADE_SHADES;
    fade.fadetimer = 0;
    fade.rate = FADE_NORM;
    fade.mode = FADE_GO;
    pCKP->Control.levelcontrol.success = success;
    pCKP->Control.levelcontrol.tobonuslevel = 0;
  }
}

void SetGameOver(stCloneKeenPlus *pCKP)
{
/*int x,y,bmnum;*/
   if (!pCKP->Control.levelcontrol.gameovermode)
   {
     pCKP->Control.levelcontrol.gameovermode = 1;
     g_pSound->playSound(SOUND_GAME_OVER, PLAY_NOW);
   }
}


// this is so objects can block the player,
// player can stand on them, etc.
// x and y are the CSFed coordinates to check (e.g. playx and playy)
// returns nonzero if there is a solid object
// at that point
char checkobjsolid(unsigned int x, unsigned int y, unsigned int cp)
{
  int o;

   for(o=1;o<MAX_OBJECTS;o++)
   {
      if (objects[o].exists && objects[o].cansupportplayer[cp])
      {
        if (x >= objects[o].x+sprites[objects[o].sprite].bboxX1)
          if (x <= objects[o].x+sprites[objects[o].sprite].bboxX2)
            if (y >= objects[o].y+sprites[objects[o].sprite].bboxY1)
              if (y <= objects[o].y+sprites[objects[o].sprite].bboxY2)
                return o;
      }
   }
 return 0;
}

// returns 1 if player cp has the card to door t, which -> door
char CheckDoorBlock(int t, int cp, int which,stCloneKeenPlus *pCKP)
{
        if (which==DOOR_YELLOW)
        {
          if (!player[cp].inventory.HasCardYellow)
          {
            player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_RED)
        {
          if (!player[cp].inventory.HasCardRed)
          {
            player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_GREEN)
        {
          if (!player[cp].inventory.HasCardGreen)
          {
            player[cp].blockedby = t;
            return 1;
          }
        }
        else if (which==DOOR_BLUE)
        {
          if (!player[cp].inventory.HasCardBlue)
          {
            player[cp].blockedby = t;
            return 1;
          }
        }

   return 0;
}

// checks if tile at (x,y) is solid to the player walking left into it.
// returns 1 and sets blockedby if so.
char checkissolidl(int x, int y, int cp, stCloneKeenPlus *pCKP)
{
int t;
  t = getmaptileat(x, y);

  if(TileProperty[t][BLEFT] || x < 0)
  //if (tiles[t].solidl)
  {
    player[cp].blockedby = t;
    return 1;
  }
  if (checkobjsolid(x<<CSF,y<<CSF,cp))
  {
    player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp, TileProperty[t][BEHAVIOR], pCKP))
    {
      return 1;
    }
  }
  return 0;
}

// checks if tile at (x,y) is solid to the player walking right into it.
// returns 1 and sets blockedby if so.
char checkissolidr(int x, int y, int cp, stCloneKeenPlus *pCKP)
{
int t;
  t = getmaptileat(x, y);
  if(TileProperty[t][BRIGHT])
  //if (tiles[t].solidr)
  {
    player[cp].blockedby = t;
    return 1;
  }
  else if (checkobjsolid(x<<CSF,y<<CSF,cp))
  {
    player[cp].blockedby = 0;
    return 1;
  }
  else
  {
    // don't let player walk through doors he doesn't have the key to
    if (CheckDoorBlock(t, cp, TileProperty[t][BEHAVIOR], pCKP))
    {
      return 1;
    }
  }
  return 0;
}

// copies tile data into a sprite. multiple tiles can be copied into
// a single sprite--they will be stacked up vertically.
void CopyTileToSprite(int t, int s, int ntilestocopy, int transparentcol)
{
int x,y1,y2,tboundary;
    sprites[s].xsize = 16;
    sprites[s].ysize = 16 * ntilestocopy;
    sprites[s].bboxX1 = sprites[s].bboxY1 = 0;
    sprites[s].bboxX2 = (sprites[s].xsize << CSF);
    sprites[s].bboxY2 = (sprites[s].ysize << CSF);

    tboundary = 0;
    y2 = 0;
    for(y1=0;y1<(16*ntilestocopy);y1++)
    {
      for(x=0;x<16;x++)
      {
        sprites[s].imgdata[y1][x] = tiledata[t][y2][x];
        if (sprites[s].imgdata[y1][x] != transparentcol)
        {
          sprites[s].maskdata[y1][x] = 15;
        }
        else
        {
          sprites[s].maskdata[y1][x] = 0;
        }
      }
      y2++;
      if (y2>=16)
      {
        y2 = 0;
        t++;
      }
    }
}

// creates a mask from a sourcetile, places it in desttile
void MakeMask(int sourcetile, int desttile, int transparentcol)
{
int x,y,c;
   for(y=0;y<16;y++)
   {
     for(x=0;x<16;x++)
     {
        c = tiledata[sourcetile][y][x];
        if (c != transparentcol) c = 0; else c = 15;
        tiledata[desttile][y][x] = c;
     }
   }
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
void ReplaceSpriteColor(int s, int find, int replace, int miny)
{
int x,y;

 for(y=miny;y<sprites[s].ysize;y++)
 {
   for(x=0;x<sprites[s].xsize;x++)
   {
     if (sprites[s].imgdata[y][x]==find)
     {
       sprites[s].imgdata[y][x] = replace;
     }
   }
 }
}

// duplicates sprite source to dest
void CopySprite(int source, int dest)
{
int x,y;

  sprites[dest].xsize = sprites[source].xsize;
  sprites[dest].ysize = sprites[source].ysize;

  for(y=0;y<sprites[source].ysize;y++)
  {
    for(x=0;x<sprites[source].xsize;x++)
    {
      sprites[dest].imgdata[y][x] = sprites[source].imgdata[y][x];
      sprites[dest].maskdata[y][x] = sprites[source].maskdata[y][x];
    }
  }
}

// initilize sprites that come from tiles, such as the doors
void initsprites(stCloneKeenPlus *pCKP, int s)
{
int x,y,i,indx;


    	for(i=0 ; i < numtiles ; i++ )
    	{
    		if(TileProperty[i][BEHAVIOR] == DOOR_YELLOW)
    			CopyTileToSprite(i-1, DOOR_YELLOW_SPRITE, 2, 7);

    		if(TileProperty[i][BEHAVIOR] == DOOR_RED)
    			CopyTileToSprite(i-1, DOOR_RED_SPRITE, 2, 7);

    		if(TileProperty[i][BEHAVIOR] == DOOR_GREEN)
    			CopyTileToSprite(i-1, DOOR_GREEN_SPRITE, 2, 7);

    		if(TileProperty[i][BEHAVIOR] == DOOR_BLUE)
    			CopyTileToSprite(i-1, DOOR_BLUE_SPRITE, 2, 7);
    	}


    // create BLANKSPRITE
    BlankSprite = s;
    sprites[BlankSprite].xsize = sprites[BlankSprite].ysize = 0;

    // create DEMOSPRITE (the "demo" message the appears on the screen
    // during a demo--the graphics for it are stored in demobox.h)
    s++;
    DemoSprite = s;
    sprites[DemoSprite].xsize = 48;
    sprites[DemoSprite].ysize = 16;
    indx = 0;
    for(y=0;y<DEMOBOX_HEIGHT;y++)
    {
      for(x=0;x<DEMOBOX_WIDTH;x++)
      {
        sprites[DemoSprite].imgdata[y][x] = demobox_image[indx];
        sprites[DemoSprite].maskdata[y][x] = demobox_mask[indx];
        indx++;
      }
    }

    // create the sprites for player 2
    s++;
    playerbaseframes[1] = s;
    for(i=0;i<48;i++)
    {
      CopySprite(i, s);
      ReplaceSpriteColor(s, 13, 10, 0);
      ReplaceSpriteColor(s, 5, 2, 0);
      ReplaceSpriteColor(s, 9, 14, 8);
      ReplaceSpriteColor(s, 1, 6, 8);
      ReplaceSpriteColor(s, 12, 11, 0);
      ReplaceSpriteColor(s, 4, 3, 0);
      s++;
    }

    // create the sprites for player 3
    playerbaseframes[2] = s;
    for(i=0;i<48;i++)
    {
      CopySprite(i, s);
      ReplaceSpriteColor(s, 12, 6, 0);
      ReplaceSpriteColor(s, 4, 6, 0);
      ReplaceSpriteColor(s, 13, 12, 0);
      ReplaceSpriteColor(s, 5, 4, 0);
      ReplaceSpriteColor(s, 9, 12, 8);
      ReplaceSpriteColor(s, 1, 4, 8);
      s++;
    }

    // set up mask for ice cannons (ep1)
    if (pCKP->Control.levelcontrol.episode==1)
    {

//       MakeMask(443, 155, 7);
  //     tiles[443].masktile = 155;
//       tiles[428].masktile = 155;
    }
}

void procgoodie(int t, int mpx, int mpy, int theplayer, stCloneKeenPlus *pCKP)
{
   if ((TileProperty[t][BEHAVIOR] > 5 && TileProperty[t][BEHAVIOR] < 11) ||
		   (TileProperty[t][BEHAVIOR] > 17 && TileProperty[t][BEHAVIOR] < 22)   )
   {
	   if((player[theplayer].x*player[theplayer].y) % 2 == 1)
		   g_pSound->playStereofromCoord(SOUND_GET_BONUS, PLAY_NOW, 0);
	   else
		   g_pSound->playStereofromCoord(SOUND_GET_BONUS, PLAY_NOW, 320);
   }
   else if (TileProperty[t][BEHAVIOR] > 10 && TileProperty[t][BEHAVIOR] < 16) g_pSound->playSound(SOUND_GET_ITEM, PLAY_NOW);
   switch(TileProperty[t][BEHAVIOR])
   {
    // keycards
    case 18: give_keycard(DOOR_YELLOW, theplayer); break;
    case 19: give_keycard(DOOR_RED, theplayer); break;
    case 20: give_keycard(DOOR_GREEN, theplayer); break;
    case 21: give_keycard(DOOR_BLUE, theplayer); break;

    case DOOR_YELLOW:
           if (player[theplayer].inventory.HasCardYellow)
             open_door(DOOR_YELLOW, DOOR_YELLOW_SPRITE, mpx, mpy, theplayer, pCKP);
         break;
    case DOOR_RED:
           if (player[theplayer].inventory.HasCardRed)
             open_door(DOOR_RED, DOOR_RED_SPRITE, mpx, mpy, theplayer, pCKP);
         break;
    case DOOR_GREEN:
           if (player[theplayer].inventory.HasCardGreen)
             open_door(DOOR_GREEN, DOOR_GREEN_SPRITE, mpx, mpy, theplayer, pCKP);
         break;
    case DOOR_BLUE:
           if (player[theplayer].inventory.HasCardBlue)
             open_door(DOOR_BLUE, DOOR_BLUE_SPRITE, mpx, mpy, theplayer, pCKP);
         break;

    case 7:    // What gives you 100 Points
         player[theplayer].inventory.score += 100; extralifeat(theplayer);
         break;
    case 8:    // What gives you 200 Points
         player[theplayer].inventory.score += 200; extralifeat(theplayer);
         break;
    case 6:    // What gives you 500 Points
         player[theplayer].inventory.score += 500; extralifeat(theplayer);
         break;
    case 9:    // What gives you 1000 Points
         player[theplayer].inventory.score += 1000; extralifeat(theplayer);
         break;
    case 10:    // What gives you 5000 Points
         player[theplayer].inventory.score += 5000; extralifeat(theplayer);
         break;

    case 15:           // raygun
         player[theplayer].inventory.charges += 5;
    break;
    case 16:           // the Holy Pogo Stick
         player[theplayer].inventory.HasPogo = 1;
         g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
    break;

    case 11:
     player[theplayer].inventory.HasJoystick = 1;
     g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
    break;

    case 12:
     player[theplayer].inventory.HasBattery = 1;
     g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
    break;
    case 13:

     player[theplayer].inventory.HasVacuum = 1;
     g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
    break;
    case 14:
     player[theplayer].inventory.HasFuel = 1;
     g_pSound->playSound(SOUND_GET_PART, PLAY_NOW);
    break;

    // in-level teleporter
    // (in level13.ck1 that takes you to the bonus level)
    case 24:
        endlevel(0, pCKP);
        pCKP->Control.levelcontrol.tobonuslevel = 1;
    break;

    case YORPSTATUEHEAD:
      if(pCKP->Control.levelcontrol.episode == 1)
      {
    	  youseeinyourmind(mpx, mpy, pCKP);
      }
      else if(pCKP->Control.levelcontrol.episode == 2)
    	  if(!player[theplayer].blockedr && !player[theplayer].blockedl)
    		  VorticonElder(mpx, mpy, pCKP);
      break;

    case 27:
      if(pCKP->Control.levelcontrol.episode == 3)
         GiveAnkh(theplayer);
      break;
    case 28:
      player[theplayer].inventory.charges++;
      g_pSound->playSound(SOUND_GET_ITEM, PLAY_NOW);
      break;

    case 17:
      if (pCKP->Control.levelcontrol.canexit && (!pCKP->Control.levelcontrol.isfinallevel || player[theplayer].inventory.HasFuel))
      {
        pCKP->Control.levelcontrol.exitXpos = (mpx+2)<<4;
        PlayerTouchedExit(theplayer, pCKP);
      }
    break;

    case 23:break;	// these are switches. They cannot not be picked up!
    case 25:break;  // Refer to JumpandPogo to check the activation code
    case 26:break;

    // we fell off the bottom of the map
    case TILE_FELLOFFMAP_EP1:
      if (!player[theplayer].pdie)
      {
    	  g_pSound->playSound(SOUND_KEEN_FALL, PLAY_FORCE);
    	  player[theplayer].ankhtime = 0;
    	  player[theplayer].godmode = 0;
    	  player[theplayer].pdie = PDIE_FELLOFFMAP;
      }
      break;

    default:
    	why_term_ptr = "procgoodie_ep1: Unknown goodie-- value given in flag2.";
        break;
   }


}

void GiveAnkh(int cp)
{
int o;
  if (!player[cp].ankhtime)
  {
    o = spawn_object(player[cp].x, player[cp].y, OBJ_SECTOREFFECTOR);
    objects[o].ai.se.type = SE_ANKHSHIELD;
    player[cp].ankhshieldobject = o;
  }

  g_pSound->playSound(SOUND_ANKH, PLAY_NOW);
  player[cp].ankhtime = PLAY_ANKH_TIME;
  gamepdo_ankh(cp);
}

void gameloop_initialize(stCloneKeenPlus *pCKP)
{
unsigned int x,y,i/*,tl*/;
int timeout;

   if (pCKP->Control.levelcontrol.episode == 3)
   {
     // coat the top of the map ("oh no!" border) with a non-solid tile
     // so keen can jump partially off the top of the screen
     for(x=1;x<map.xsize-2;x++)
     {
         map.mapdata[x][1] = 143;
     }

     // make it lethal to fall off the bottom of the map
     // the "oh no" border is set solidceil=0, and here we're
     // going to coat the bottom of the map, below the "oh no"
     // border, with a special tile that has standgoodie set
     // and will trigger the "fell off the map" kill
     y = map.ysize;
     for(x=2;x<map.xsize-2;x++)
     {
        map.mapdata[x][y] = TILE_FELLOFFMAP_EP3;
     }
   }

  if (pCKP->Control.levelcontrol.episode==1)
  {
    // coat the bottom of the map below the border.
    // since the border has solidceil=1 this provides
    // a platform to catch yorps that fall off the map
    y = map.ysize;
    for(x=2;x<map.xsize-2;x++)
    {
       map.mapdata[x][y] = TILE_FELLOFFMAP_EP1;
    }
  }

   // draw map first time
   drawmap();

   // generate other player's start positions
   // don't generate player positions for the world
   // map, except the first time
   if (!map.isworldmap || map.firsttime)
   {
     x = player[0].x;
     for(i=1;i<numplayers;i++)
     {
      if (player[0].x>>CSF>>4 < (map.xsize/2) || pCKP->Control.levelcontrol.episode==1)
      {
        x += (18<<CSF);
      }
      else
      {
        x -= (18<<CSF);
      }
      player[i].x = x;
      player[i].y = player[0].y;
     }
   }
   map.firsttime = 0;
   // set the maximum amount the map is allowed to scroll
   max_scroll_x = (map.xsize-2-(320/16))<<4;
   max_scroll_y = (map.ysize<<4)-200-32;

   // players start facing left if the start position
   // is on the left half of the map, else right
   // for ep1 it's always facing right (because of level08)
   for(i=0;i<MAX_PLAYERS;i++)
   {
    if (map.isworldmap)
      player[i].pdir = player[i].pshowdir = DOWN;
    else
    {
      if (player[i].x>>CSF>>4 < (map.xsize/2) || pCKP->Control.levelcontrol.episode==1)
        player[i].pdir = player[i].pshowdir = RIGHT;
      else
        player[i].pdir = player[i].pshowdir = LEFT;
    }
   }

// scroll past the first two tiles (the level border), they'll
// now never be visible because you're not allowed to scroll
// left past X=32.
   for(i=0;i<2*16;i++)
   {
      map_scroll_right();
      map_scroll_down();
   }

// scroll the screen until the primary player is onscreen
// enough to where he doesn't set off the scroll triggers
  for(timeout=0;timeout<10000;timeout++)
  {
    if (!gamedo_ScrollTriggers(primaryplayer)) break;
  }

  // initiate fade-in
  fade.mode = FADE_GO;
  fade.dir = FADE_IN;
  fade.rate = FADE_NORM;
  fade.curamt = 0;
  fade.fadetimer = 0;

  // "keens left" when returning to world map after dying
  if (pCKP->Control.levelcontrol.dokeensleft)
  {
    keensleft(pCKP);
    pCKP->Control.levelcontrol.dokeensleft = 0;
  }

}
