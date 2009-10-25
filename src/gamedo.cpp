/* GAMEDO.C
  Contains all of the gamedo_xxx functions...which are called from the
  main game loop. These functions perform some task that is done each
  time around the game loop, not directly related to the player.
*/

#include "keen.h"

#include "game.h"
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
