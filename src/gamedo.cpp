/* GAMEDO.C
  Contains all of the gamedo_xxx functions...which are called from the
  main game loop. These functions perform some task that is done each
  time around the game loop, not directly related to the player.
*/

#include "keen.h"

#include "include/game.h"
#include "include/gamedo.h"
#include "include/gamepdo.h"
#include "include/misc.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "graphics/CGfxEngine.h"
#include "CLogFile.h"
#include "externals.h"
#include "StringUtils.h"
#include <string>

//#include "include/enemyai.h"

// do object and enemy AI
void gamedo_enemyai(stLevelControl *p_levelcontrol)
{
/*int i, topobj;
	// handle objects and do enemy AI
	topobj = highest_objslot;
   for(i=1;i<topobj;i++)
   {
      if ( !objects[i].exists || objects[i].type==OBJ_PLAYER ) continue;

      //gamedo_calcenemyvisibility(i);

      // This will do the function gamedo_calcenemyvisibility(i);
      // check if object is really in the map!!!
      if (objects[i].x < 0 || objects[i].y < 0)
    	  continue;

      if (objects[i].x > (map.xsize << CSF << 4) || objects[i].y > (map.ysize << CSF << 4))
    	  continue;

      //objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
      //objects[i].scry = (objects[i].y>>CSF)-scroll_y;

      // Bitmaps are also part of the object, but only print them directly
      if ( objects[i].type==OBJ_EGA_BITMAP ) continue;

      if (objects[i].scrx < -(g_pGfxEngine->Sprite[objects[i].sprite]->getWidth()) || objects[i].scrx > g_pVideoDriver->getGameResRect().w \
          || objects[i].scry < -(g_pGfxEngine->Sprite[objects[i].sprite]->getHeight()) || objects[i].scry > g_pVideoDriver->getGameResRect().h)
          {
			 objects[i].onscreen = 0;
             objects[i].wasoffscreen = 1;
             if (objects[i].type==OBJ_ICEBIT) objects[i].exists = 0;
          }
          else
          {
             objects[i].onscreen = 1;
             objects[i].hasbeenonscreen = 1;
          }

		if (objects[i].hasbeenonscreen || objects[i].zapped ||
			objects[i].type==OBJ_RAY || \
			objects[i].type==OBJ_ICECHUNK || objects[i].type==OBJ_PLATFORM ||
			objects[i].type==OBJ_PLATVERT || objects[i].type==OBJ_YORP ||
			objects[i].type==OBJ_FOOB || objects[i].type==OBJ_SCRUB)

      {
         common_enemy_ai(i);
         if(!p_levelcontrol->level_done)
         {
        	 switch(objects[i].type)
        	 {
        	 //KEEN1
			  case OBJ_YORP: yorp_ai(i, *p_levelcontrol); break;
			  case OBJ_GARG: garg_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_VORT: vort_ai(i, p_levelcontrol ); break;
			  case OBJ_BUTLER: butler_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_TANK: tank_ai(i, p_levelcontrol->hardmode); break;
//			  case OBJ_RAY: ray_ai(i, p_levelcontrol->episode,
//							  options[OPT_FULLYAUTOMATIC].value, p_levelcontrol->cepvars.pShotSpeed); break;
			  case OBJ_DOOR: door_ai(i, p_levelcontrol->cepvars.DoorOpenDir); break;
			  case OBJ_ICECANNON: icecannon_ai(i); break;
			  case OBJ_ICECHUNK: icechunk_ai(i); break;
			  case OBJ_ICEBIT: icebit_ai(i); break;
			  case OBJ_TELEPORTER: teleporter_ai(i, *p_levelcontrol); break;
			  case OBJ_ROPE: rope_ai(i); break;

			  //KEEN2
			  case OBJ_SCRUB: scrub_ai(i, *p_levelcontrol); break;
			  case OBJ_TANKEP2: tankep2_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_PLATFORM: platform_ai(i, *p_levelcontrol); break;
			  case OBJ_VORTELITE: vortelite_ai(i, p_levelcontrol->dark); break;
			  case OBJ_SECTOREFFECTOR: se_ai(i, p_levelcontrol ); break;
			  case OBJ_BABY: baby_ai(i, p_levelcontrol->episode,
								  p_levelcontrol->hardmode); break;
			  case OBJ_EXPLOSION: explosion_ai(i); break;
			  case OBJ_EARTHCHUNK: earthchunk_ai(i); break;
			  case OBJ_SPARK: spark_ai(i, &(p_levelcontrol->sparks_left) ); break;

			  //KEEN3
			  case OBJ_FOOB: foob_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_NINJA: ninja_ai( i, p_levelcontrol->hardmode); break;
			  case OBJ_MEEP: meep_ai( i, *p_levelcontrol ); break;
			  case OBJ_SNDWAVE: sndwave_ai( i, p_levelcontrol->hardmode); break;
			  case OBJ_MOTHER: mother_ai( i, *p_levelcontrol ); break;
			  case OBJ_FIREBALL: fireball_ai( i, p_levelcontrol->hardmode ); break;
			  case OBJ_BALL: ballandjack_ai(i); break;
			  case OBJ_JACK: ballandjack_ai(i); break;
			  case OBJ_PLATVERT: platvert_ai(i); break;
			  case OBJ_NESSIE: nessie_ai(i); break;

			  //Specials
			  case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
			  case OBJ_GOTPOINTS: gotpoints_ai(i); break;

			  case OBJ_DEMOMSG: break;

			  default:
				  g_pLogFile->ftextOut("gamedo_enemy_ai: Object %d is of invalid type %d\n", i, objects[i].type);
        	  break;
			 }
         }

       	 objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
       	 //objects[i].scry = (objects[i].y>>CSF)-scroll_y;
      }
   }*/
}

// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void common_enemy_ai(int o)
{
/*int x,y,xa,ya,xsize,ysize;
int temp;
int cplayer;

	if (objects[o].type==OBJ_GOTPOINTS) return;


	ysize = g_pGfxEngine->Sprite[objects[o].sprite]->getHeight();
	xsize = g_pGfxEngine->Sprite[objects[o].sprite]->getWidth();

 // set value of blockedd--should object fall?
	temp = (objects[o].y>>CSF)+ysize;
	if ((temp>>TILE_S)<<TILE_S != temp)
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
			if (TileProperty[getmaptileat(x+xa,y)][BUP] || IsStopPoint(x+xa,y,o))
			{
				objects[o].blockedd = 1;
				break;
			}
		}

		if (!objects[o].blockedd)	// check final point
		{
			if (TileProperty[getmaptileat(x+xsize-2, y)][BUP] || IsStopPoint(x+xsize-2,y,o))
			{
				objects[o].blockedd = 1;
			}
		}

		// If the object is out of map
		if(y >= (int)((map.ysize-2)<<4))	objects[o].blockedd = 1;
	}

// set blockedu
	objects[o].blockedu = 0;
	x = (objects[o].x>>CSF);
	y = (objects[o].y>>CSF)-1;
	for(xa=1;xa<xsize;xa+=16)		// change start pixel to xa=1 for icecannon in ep1l8
	{
		if (TileProperty[getmaptileat(x+xa,y)][BDOWN] || IsStopPoint(x+xa,y,o))
		{
			objects[o].blockedu = 1;
			break;
		}
    }

	if (!objects[o].blockedu)		// check final point
	{
		if (TileProperty[getmaptileat(x+xsize-2, y)][BDOWN] || IsStopPoint(x+xsize-2,y,o))
		{
			objects[o].blockedu = 1;
		}
	}

	// If the object is out of map
	if(y <= (2<<4)) objects[o].blockedu = 1;

 // set blockedl
    objects[o].blockedl = 0;
    x = (objects[o].x>>CSF)-1;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {

        if (TileProperty[getmaptileat(x,y+ya)][BRIGHT] || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedl = 1;
          goto blockedl_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BRIGHT] ||
    		IsStopPoint(x, (objects[o].y>>CSF)+ysize-1, o))
      objects[o].blockedl = 1;

    blockedl_set: ;

	// If the object is out of map
	if(x <= (2<<4)) objects[o].blockedl = 1;

 // set blockedr
    objects[o].blockedr = 0;
    x = (objects[o].x>>CSF)+xsize;
    y = (objects[o].y>>CSF)+1;
    for(ya=0;ya<ysize;ya+=16)
    {
        if (TileProperty[getmaptileat(x,y+ya)][BLEFT] || IsStopPoint(x,y+ya,o))
        {
          objects[o].blockedr = 1;
          goto blockedr_set;
        }
    }
    if (TileProperty[getmaptileat(x, ((objects[o].y>>CSF)+ysize-1))][BLEFT] ||
		IsStopPoint(x, (objects[o].y>>CSF)+ysize-1, o))
    {
      objects[o].blockedr = 1;
    }
    blockedr_set: ;

    if(x >= (int)((map.xsize-2)<<4)) objects[o].blockedr = 1;

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
			if (!player[cplayer].godmode)
			{
	            objects[o].touchPlayer = 1;
	            objects[o].touchedBy = cplayer;
			}
			else
			{
				if (objects[o].type==OBJ_MOTHER || objects[o].type==OBJ_BABY ||\
					objects[o].type==OBJ_MEEP || objects[o].type==OBJ_YORP)
				{
					if (objects[o].canbezapped)
						objects[o].zapped += 100;
				}
			}
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
  }*/
}

int savew, saveh;

void gamedo_render_drawobjects()
{
/*unsigned int i;
int x,y,o,tl,xsize,ysize;
int xa,ya;

   // copy player data to their associated objects show they can get drawn
   // in the object-drawing loop with the rest of the objects
   for( i=0 ;i < numplayers ; i++)
   {
     o = player[i].useObject;

     if (!player[i].hideplayer)
       objects[o].sprite = player[i].playframe + playerbaseframes[i];

     else
       objects[o].sprite = BlankSprite;

     objects[o].x = player[i].x;
     objects[o].y = player[i].y;
     objects[o].scrx = (player[i].x>>CSF)-scroll_x;
     //objects[o].scry = (player[i].y>>CSF)-scroll_y;

   }

   // draw all objects. drawn in reverse order because the player sprites
   // are in the first few indexes and we want them to come out on top.
   for( i=highest_objslot ;; i-- )
   {
      if (objects[i].exists && objects[i].onscreen)
      {
    	  // Draw the Demo-Bar if it must be shown
    	  if( objects[i].type == OBJ_DEMOMSG )
    	  {
   				#define DEMO_X_POS         137
				#define DEMO_Y_POS         6
   			   objects[i].scrx = DEMO_X_POS;
   			   objects[i].scry = DEMO_Y_POS;
    	  }
    	  else if( objects[i].type == OBJ_EGA_BITMAP )
    	  {
    		  g_pGfxEngine->Bitmap[objects[i].ai.bitmap.BitmapID]->
    		  draw(g_pVideoDriver->getScrollSurface(), objects[i].x, objects[i].y );
    		  continue;
    	  }
    	  else
    	  {
    		  objects[i].scrx = ((objects[i].x>>CSF)-scroll_x);
    		  //objects[i].scry = ((objects[i].y>>CSF)-scroll_y);
    	  }
    	  //g_pGraphics->drawSprite(objects[i].scrx, objects[i].scry, objects[i].sprite, i);
    	  g_pGfxEngine->Sprite[objects[i].sprite]->drawSprite( g_pVideoDriver->BlitSurface,
															  objects[i].scrx, objects[i].scry );


        if (objects[i].honorPriority)
        {
        	CSprite *sprite = g_pGfxEngine->Sprite[objects[i].sprite];
            // handle priority tiles and tiles with masks
            // get the upper-left coordinates to start checking for tiles
            x = (((objects[i].x>>CSF)-1)>>4)<<4;
            y = (((objects[i].y>>CSF)-1)>>4)<<4;

            // get the xsize/ysize of this sprite--round up to the nearest 16
            xsize = ((sprite->getWidth())>>4<<4);
            if (xsize != sprite->getWidth()) xsize+=16;

            ysize = ((g_pGfxEngine->Sprite[objects[i].sprite]->getHeight())>>4<<4);
            if (ysize != sprite->getHeight()) ysize+=16;

            tl = getmaptileat(x,y);

            // now redraw any priority/masked tiles that we covered up
            // with the sprite
            //SDL_Surface *sfc = g_pVideoDriver->BlitSurface;
            SDL_Rect sfc_rect;
            sfc_rect.w = sfc_rect.h = 16;

            for(ya=0;ya<=ysize;ya+=16)
            {
              for(xa=0;xa<=xsize;xa+=16)
              {
                tl = getmaptileat(x+xa,y+ya);
                if(TileProperty[tl][BEHAVIOR] == 65534)
                	g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-scroll_x, y+ya-scroll_y, tl+1);
                else if (TileProperty[tl][BEHAVIOR] == 65535)
                   g_pGfxEngine->Tilemap->drawTile(sfc, x+xa-scroll_x, y+ya-scroll_y, tl);
              }
            }
        }

      }
    if(i==0) break;
   }*/
}

extern int NumConsoleMessages;

int ctspace=0, lastctspace=0;
void gamedo_HandleFKeys(stCloneKeenPlus *pCKP)
{
	/*int i;

    if (g_pInput->getHoldedKey(KC) &&
    		g_pInput->getHoldedKey(KT) &&
    		g_pInput->getHoldedKey(KSPACE))
       {
          ctspace = 1;
       }
       else ctspace = 0;

       if (ctspace && !lastctspace)
       {
              for(i=0;i<MAX_PLAYERS;i++)
              {
                 if (player[i].isPlaying)
                 {
                    give_keycard(DOOR_YELLOW, i);
                    give_keycard(DOOR_RED, i);
                    give_keycard(DOOR_GREEN, i);
                    give_keycard(DOOR_BLUE, i);

                    player[i].inventory.charges = 999;
                    player[i].inventory.HasPogo = 1;
                    player[i].inventory.lives = 10;

                    // Show a message like in the original game
          		    char **text;
          		    int i;

          		    text = (char**) malloc(4*sizeof(char*));

          		    for(i=0;i<4;i++)
          		    {
						static const int MAX_STRING_LENGTH = 256;
          		    	text[i]= (char*) malloc(MAX_STRING_LENGTH*sizeof(char));
          		    }

          		    std::string Text;


          		    Text = 	"You are now cheating!\n";
          		    Text +=	"You got more lives\n";
          		    Text +=	"all the key cards, and\n";
          		    Text +=	"lots of ray gun charges!\n";

          		    showTextMB(Text);
                 }
              }
         g_pVideoDriver->AddConsoleMsg("All items cheat");
       }

       lastctspace = ctspace;

       // GOD cheat -- toggle god mode
       if (g_pInput->getHoldedKey(KG) && g_pInput->getHoldedKey(KO) && g_pInput->getHoldedKey(KD))
       {
           for(i=0;i<MAX_PLAYERS;i++)
              player[i].godmode ^= 1;

           g_pVideoDriver->DeleteConsoleMsgs();
           if (player[0].godmode)
        	   g_pVideoDriver->AddConsoleMsg("God mode ON");
           else
        	   g_pVideoDriver->AddConsoleMsg("God mode OFF");

           g_pSound->playSound(SOUND_GUN_CLICK, PLAY_FORCE);

           // Show a message like in the original game
           showTextMB(player[0].godmode ? "Godmode enabled" : "Godmode disabled");
       }

    if (pCKP->Option[OPT_CHEATS].value)
    {
            if (g_pInput->getHoldedKey(KTAB)) // noclip/revive
            {
              // resurrect any dead players. the rest of the KTAB magic is
              // scattered throughout the various functions.
              for(i=0;i<MAX_PLAYERS;i++)
              {
                 if (player[i].pdie)
                 {
                   player[i].pdie = PDIE_NODIE;
                   player[i].y -= (8<<CSF);
                 }
                 player[i].pfrozentime = 0;
              }
            }

            // F9 - exit level immediately
            if(g_pInput->getPressedKey(KF9))
            {
               endlevel(1, &(pCKP->Control.levelcontrol) );
            }
    }*/

    // F10 - change primary player
    /*if(g_pInput->getPressedKey(KF10))
    {
        primaryplayer++;
        if (primaryplayer>=numplayers) primaryplayer=0;
    }
    // F3 - save game
    if (g_pInput->getPressedKey(KF3))
       game_save_interface(pCKP);*/

}
