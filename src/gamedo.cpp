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
#include "CLogFile.h"
#include "CGraphics.h"
#include "externals.h"
#include "StringUtils.h"
#include <string>

#include "include/enemyai.h"

extern unsigned long gotPlayX;

extern unsigned long CurrentTickCount;

extern unsigned int unknownKey;

int animtiletimer, curanimtileframe;

// gathers data from input controllers: keyboard, joystick, network,
// whatever to populate each player's keytable
unsigned char oldleftkey = 5;
unsigned char oldrightkey = 5;
unsigned char oldupkey = 5;
unsigned char olddownkey = 5;
unsigned char oldctrlkey = 5;
unsigned char oldaltkey = 5;
void gamedo_getInput(stLevelControl *p_levelcontrol)
{
int i=0;
int byt;
unsigned int msb, lsb;


     if (p_levelcontrol->demomode==DEMO_PLAYBACK)
     {
        // time to get a new key block?
        if (!demo_RLERunLen)
        {
          /* get next RLE run length */
          lsb = demo_data[demo_data_index++];
          msb = demo_data[demo_data_index++];
          demo_RLERunLen = (msb<<8) | lsb;
          byt = demo_data[demo_data_index++];         // get keys down

          player[0].playcontrol[PA_X] = 0;
          player[0].playcontrol[PA_POGO] = 0;
          player[0].playcontrol[PA_JUMP] = 0;
          player[0].playcontrol[PA_FIRE] = 0;
          player[0].playcontrol[PA_STATUS] = 0;

          if (byt & 1) player[0].playcontrol[PA_X] -= 100;
          if (byt & 2) player[0].playcontrol[PA_X] += 100;
          if (byt & 4) player[0].playcontrol[PA_POGO] = 1;
          if (byt & 8) player[0].playcontrol[PA_JUMP] = 1;
          if (byt & 16)player[0].playcontrol[PA_FIRE] = 1;
          if (byt & 32)player[0].playcontrol[PA_STATUS] = 1;
          if (byt & 64)
          {  // demo STOP command
            if (fade.mode!=FADE_GO) endlevel(1, p_levelcontrol );
          }
        }
        else
        {
          // we're still in the last RLE run, don't change any keys
          demo_RLERunLen--;
        }

        // user trying to cancel the demo?
        for(i=0;i<KEYTABLE_SIZE;i++)
        {
          if (g_pInput->getPressedKey(i))
          {
            if (fade.mode!=FADE_GO) endlevel( 0, p_levelcontrol );
          }
        }
        if (g_pInput->getPressedCommand(IC_STATUS))
        {
          if (fade.mode!=FADE_GO) endlevel( 0, p_levelcontrol );
        }

        return;
     }

     for(Uint8 p=0 ; p<numplayers ; p++)
     {
    	 memcpy(player[p].lastplaycontrol,player[p].playcontrol,PA_MAX_ACTIONS);

    	 // Entry for every player
   		 player[p].playcontrol[0] = 0;
   		 player[p].playcontrol[1] = 0;

    	 if(g_pInput->getHoldedCommand(p, IC_LEFT))
    		 player[p].playcontrol[PA_X] -= 100;
    	 if(g_pInput->getHoldedCommand(p, IC_RIGHT))
    		 player[p].playcontrol[PA_X] += 100;

    	 if(g_pInput->getHoldedCommand(p, IC_UP))
    		 player[p].playcontrol[PA_Y] -= 100;
    	 if(g_pInput->getHoldedCommand(p, IC_DOWN))
    		 player[p].playcontrol[PA_Y] += 100;

    	 if(g_pInput->getHoldedCommand(p, IC_JUMP))
    		 player[p].playcontrol[PA_JUMP]++;
    	 else
    		 player[p].playcontrol[PA_JUMP] = 0;

   		 player[p].playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(p, IC_POGO)   ? 1 : 0;
   		 player[p].playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(p, IC_FIRE)   ? 1 : 0;
   		 player[p].playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(p, IC_STATUS) ? 1 : 0;

   		 // The possibility to charge jumps. This is mainly used for the pogo.
   		 if( player[p].playcontrol[PA_JUMP] > 50) player[p].playcontrol[PA_JUMP] = 50;

    	 if (p_levelcontrol->demomode==DEMO_RECORD)
    	 {
    	   if(i) player[p].playcontrol[PA_X] += 100;
    	   fputc(i, demofile);
    	   if(i) player[p].playcontrol[PA_X] -= 100;
    	   fputc(i, demofile);
    	   if(i) player[p].playcontrol[PA_POGO] = 1;
    	   fputc(i, demofile);
    	   if(i) player[p].playcontrol[PA_JUMP] = 1;
    	   fputc(i, demofile);
    	   if(i) player[p].playcontrol[PA_FIRE] = 1;
    	   fputc(i, demofile);
    	   if(i) player[p].playcontrol[PA_STATUS] = 1;
    	   fputc(i, demofile);
    	 }
     }
}

// handles scrolling, for player cp
// returns nonzero if the scroll was changed
int gamedo_ScrollTriggers(int theplayer)
{
signed int px, py;
int scrollchanged;

   if (player[theplayer].pdie) return 0;

   px = (player[theplayer].x>>CSF)-scroll_x;
   py = (player[theplayer].y>>CSF)-scroll_y;

   scrollchanged = 0;

   /* left-right scrolling */
   if(px > SCROLLTRIGGERRIGHT && scroll_x < max_scroll_x)
   {
      map_scroll_right();
      scrollchanged = 1;
   }
   else if(px < SCROLLTRIGGERLEFT && scroll_x > 32)
   {
      map_scroll_left();
      scrollchanged = 1;
   }

   /* up-down scrolling */
   if (py > SCROLLTRIGGERDOWN && scroll_y < max_scroll_y)
   {
      map_scroll_down();
      scrollchanged = 1;
   }
   else if (py < SCROLLTRIGGERUP && scroll_y > 32)
   {
      map_scroll_up();
      scrollchanged = 1;
   }

   return scrollchanged;
}

// animates animated tiles
void gamedo_AnimatedTiles(bool animate_hinttiles)
{
int i;
   /* animate animated tiles */
   if (animtiletimer>ANIM_TILE_TIME)
   {
      /* advance to next frame */
	  curanimtileframe = (curanimtileframe+1)&7;

	  /* re-draw all animated tiles */
      for(i=1;i<MAX_ANIMTILES-1;i++)
      {
          if (animtiles[i].slotinuse)
          {
        	  if( TileProperty[animtiles[i].baseframe][BEHAVIOR] != 22 || animate_hinttiles ) // If the tile is a hint mb, then, only animate if it hasn't been triggered yet!
        	  {
        		  g_pGraphics->drawTile(animtiles[i].x, animtiles[i].y, animtiles[i].baseframe+((animtiles[i].offset+curanimtileframe)%TileProperty[animtiles[i].baseframe][ANIMATION]));
        	  }
          }
      }
      animtiletimer = 0;
   }
   else animtiletimer++;
}

// do object and enemy AI
void gamedo_enemyai(stLevelControl *p_levelcontrol)
{
int i, topobj;
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

      objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
      objects[i].scry = (objects[i].y>>CSF)-scroll_y;

      // Bitmaps are also part of the object, but only print them directly
      if ( objects[i].type==OBJ_EGA_BITMAP ) continue;

      if (objects[i].scrx < -(sprites[objects[i].sprite].xsize) || objects[i].scrx > 320 \
          || objects[i].scry < -(sprites[objects[i].sprite].ysize) || objects[i].scry > 200)
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
			objects[i].type==OBJ_FOOB || objects[i].type==OBJ_WALKER)

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
			  case OBJ_RAY: ray_ai(i, p_levelcontrol->episode,
							  options[OPT_FULLYAUTOMATIC].value, p_levelcontrol->cepvars.pShotSpeed); break;
			  case OBJ_DOOR: door_ai(i, p_levelcontrol->cepvars.DoorOpenDir); break;
			  case OBJ_ICECANNON: icecannon_ai(i); break;
			  case OBJ_ICECHUNK: icechunk_ai(i); break;
			  case OBJ_ICEBIT: icebit_ai(i); break;
			  case OBJ_TELEPORTER: teleporter_ai(i, *p_levelcontrol); break;
			  case OBJ_ROPE: rope_ai(i); break;

			  //KEEN2
			  case OBJ_WALKER: walker_ai(i, *p_levelcontrol); break;
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
			  case OBJ_NINJA: ninja_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_MEEP: meep_ai(i, *p_levelcontrol); break;
			  case OBJ_SNDWAVE: sndwave_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_MOTHER: mother_ai(i, *p_levelcontrol); break;
			  case OBJ_FIREBALL: fireball_ai(i, p_levelcontrol->hardmode); break;
			  case OBJ_BALL: ballandjack_ai(i); break;
			  case OBJ_JACK: ballandjack_ai(i); break;
			  case OBJ_PLATVERT: platvert_ai(i); break;
			  case OBJ_NESSIE: nessie_ai(i); break;

			  //Specials
			  //case OBJ_AUTORAY: case OBJ_AUTORAY_V: autoray_ai(i); break;
			  //case OBJ_GOTPOINTS: gotpoints_ai(i); break;

			  case OBJ_DEMOMSG: break;

			  default:
				  g_pLogFile->ftextOut("gamedo_enemy_ai: Object %d is of invalid type %d\n", i, objects[i].type);
        	  break;
			 }
         }

       	 objects[i].scrx = (objects[i].x>>CSF)-scroll_x;
       	 objects[i].scry = (objects[i].y>>CSF)-scroll_y;
      }
   }
}

// common enemy/object ai, such as falling, setting blocked variables,
// detecting player contact, etc.
void common_enemy_ai(int o)
{
int x,y,xa,ya,xsize,ysize;
int temp;
int cplayer;

	//if (objects[o].type==OBJ_GOTPOINTS) return;

	xsize = sprites[objects[o].sprite].xsize;
	ysize = sprites[objects[o].sprite].ysize;

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
  }
}

int savew, saveh;

void gamedo_render_drawobjects()
{
unsigned int i;
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
     objects[o].scry = (player[i].y>>CSF)-scroll_y;

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
    		  g_pGraphics->drawBitmap(objects[i].x, objects[i].y, objects[i].ai.bitmap.BitmapID);
    		  continue;
    	  }
    	  else
    	  {
    		  objects[i].scrx = ((objects[i].x>>CSF)-scroll_x);
    		  objects[i].scry = ((objects[i].y>>CSF)-scroll_y);
    	  }
    	  g_pGraphics->drawSprite(objects[i].scrx, objects[i].scry, objects[i].sprite, i);

        if (objects[i].honorPriority)
        {
            // handle priority tiles and tiles with masks
            // get the upper-left coordinates to start checking for tiles
            x = (((objects[i].x>>CSF)-1)>>4)<<4;
            y = (((objects[i].y>>CSF)-1)>>4)<<4;

            // get the xsize/ysize of this sprite--round up to the nearest 16
            xsize = ((sprites[objects[i].sprite].xsize)>>4<<4);
            if (xsize != sprites[objects[i].sprite].xsize) xsize+=16;

            ysize = ((sprites[objects[i].sprite].ysize)>>4<<4);
            if (ysize != sprites[objects[i].sprite].ysize) ysize+=16;

            // now redraw any priority/masked tiles that we covered up
            // with the sprite
            for(ya=0;ya<=ysize;ya+=16)
            {
              for(xa=0;xa<=xsize;xa+=16)
              {
                tl = getmaptileat(x+xa,y+ya);
                if(TileProperty[tl][BEHAVIOR] == 65534)
                {
                	g_pGraphics->drawTilewithmask(x+xa-scroll_x,y+ya-scroll_y,tl,tl+1);
                }
                else if (TileProperty[tl][BEHAVIOR] == 65535)
                {
                   if ( TileProperty[tl][ANIMATION] > 1 )
                   {
                	  tl = (tl-tiles[tl].animOffset)+((tiles[tl].animOffset+curanimtileframe)%TileProperty[tl][ANIMATION]);
                   }
                   g_pGraphics->drawPrioritytile(x+xa-scroll_x,y+ya-scroll_y,tl);
                }
              }
            }
        }

      }
    if(i==0) break;
   }
}

void gamedo_render_drawdebug(void)
{
int tl=0,y;
/*int h;*/
	std::string debugmsg;

   if (debugmode)
   {
      if (debugmode==1)
      {
        savew = 190;
        saveh = 80;
        g_pGraphics->saveArea(4,4,savew,saveh);
        y = 5-8;
		  debugmsg = "p1x/y: " + itoa(player[0].x) + "/" + itoa(player[0].y);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
		  debugmsg = "p2x/y: " + itoa(player[1].x) + "/" + itoa(player[1].y);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
		  debugmsg = "scroll_x/y = " + itoa(scroll_x) + "/" + itoa(scroll_y);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
        debugmsg = "scrollbuf_x/y: " + itoa(scrollx_buf) + "/" + itoa(scrolly_buf);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
        debugmsg = "iw,pw: " + itoa(player[0].inhibitwalking) + "/" + itoa(player[0].pwalking);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
        debugmsg = "pinertia_x: " + itoa(player[0].pinertia_x);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);
        debugmsg = "psupt: (" + itoa(player[0].psupportingtile) + "," + itoa(player[0].psupportingobject) + ")";
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);

        debugmsg = "lvl,tile = " + itoa(getlevelat((player[0].x>>CSF)+4, (player[0].y>>CSF)+9)) + "," + itoa(tl);
        g_pGraphics->sb_font_draw( debugmsg, 5, y+=8);

/*
        sprintf(debugmsg, "NOH=%d", NessieObjectHandle);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "x,y=(%d,%d)", objects[NessieObjectHandle].x,objects[NessieObjectHandle].y);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, " >>CSF=(%d,%d)", objects[NessieObjectHandle].x>>CSF,objects[NessieObjectHandle].y>>CSF);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, " >>CSF>>4=(%d,%d)", objects[NessieObjectHandle].x>>CSF>>4,objects[NessieObjectHandle].y>>CSF>>4);
        sb_font_draw(debugmsg, 5, y+=8);

        sprintf(debugmsg, "nessiestate = %d", objects[NessieObjectHandle].ai.nessie.state);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "pausetimer = %d", objects[NessieObjectHandle].ai.nessie.pausetimer);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "pausex/y = (%d,%d)", objects[NessieObjectHandle].ai.nessie.pausex,objects[NessieObjectHandle].ai.nessie.pausey);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "destx/y = %d/%d", objects[NessieObjectHandle].ai.nessie.destx,objects[NessieObjectHandle].ai.nessie.desty);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, " >>CSF = %d/%d", objects[NessieObjectHandle].ai.nessie.destx>>CSF,objects[NessieObjectHandle].ai.nessie.desty>>CSF);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, " >>CSF>>4 = %d/%d", objects[NessieObjectHandle].ai.nessie.destx>>CSF>>4,objects[NessieObjectHandle].ai.nessie.desty>>CSF>>4);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "mort_swim_amt = %d", objects[NessieObjectHandle].ai.nessie.mortimer_swim_amt);
        sb_font_draw(debugmsg, 5, y+=8);

        h = objects[NessieObjectHandle].ai.nessie.tiletrailhead;

        sprintf(debugmsg, "tthead=%d", h);
        sb_font_draw(debugmsg, 5, y+=8);

        sprintf(debugmsg, "ttX=%d,%d,%d,%d,%d", objects[NessieObjectHandle].ai.nessie.tiletrailX[0],objects[NessieObjectHandle].ai.nessie.tiletrailX[1],objects[NessieObjectHandle].ai.nessie.tiletrailX[2],objects[NessieObjectHandle].ai.nessie.tiletrailX[3],objects[NessieObjectHandle].ai.nessie.tiletrailX[4]);
        sb_font_draw(debugmsg, 5, y+=8);
        sprintf(debugmsg, "ttY=%d,%d,%d,%d,%d", objects[NessieObjectHandle].ai.nessie.tiletrailY[0],objects[NessieObjectHandle].ai.nessie.tiletrailY[1],objects[NessieObjectHandle].ai.nessie.tiletrailY[2],objects[NessieObjectHandle].ai.nessie.tiletrailY[3],objects[NessieObjectHandle].ai.nessie.tiletrailY[4]);
        sb_font_draw(debugmsg, 5, y+=8);
*/
      }
      else if (debugmode==2)
      {
        savew = map.xsize+4;
        saveh = map.ysize+4;
        g_pGraphics->saveArea(4,4,savew,saveh);
        radar();
      }
   }
}

void gamedo_render_erasedebug(void)
{
   if (debugmode) g_pGraphics->restoreArea(4,4,savew,saveh);
}

void gamedo_render_eraseobjects(void)
{
int i;

   // erase all objects.
   // note that this is done in the reverse order they are drawn.
   // this is necessary or you will see corrupted pixels when
   // two objects are occupying the same space.
   for(i=0;i<MAX_OBJECTS;i++)
   {
      if (objects[i].exists && objects[i].onscreen)
      {
    	  g_pGraphics->eraseSprite(objects[i].scrx, objects[i].scry, objects[i].sprite, i);
      }
   }
}

extern int NumConsoleMessages;

// draws sprites, players, and debug messages (if debug mode is on),
// performs frameskipping and blits the display as needed,
// at end of functions erases all drawn objects from the scrollbuf.

void gamedo_RenderScreen()
{
   g_pGraphics->renderHQBitmap();

   gamedo_render_drawobjects(); // (Sprites)

   g_pVideoDriver->sb_blit();	// blit scrollbuffer to display (Tiles)

   gamedo_render_erasedebug();
   gamedo_render_eraseobjects();

   curfps++;
}

int ctspace=0, lastctspace=0;
void gamedo_HandleFKeys(stCloneKeenPlus *pCKP)
{
int i;

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

            // F6 - onscreen debug--toggle through debug/radar/off
            if(g_pInput->getPressedKey(KF6))
            {
               debugmode++;
               if (debugmode>2) debugmode=0;
            }
    }

    // F10 - change primary player
    if(g_pInput->getPressedKey(KF10))
    {
        primaryplayer++;
        if (primaryplayer>=numplayers) primaryplayer=0;
    }
    // F3 - save game
    if (g_pInput->getPressedKey(KF3))
       game_save_interface(pCKP);

}

void gamedo_fades(void)
{
    if (fade.mode != FADE_GO) return;

    if (fade.fadetimer > fade.rate)
    {
      if (fade.dir==FADE_IN)
      {
        if (fade.curamt < PAL_FADE_SHADES)
        {
          fade.curamt++;                // coming in from black
        }
        else
        {
          fade.curamt--;                // coming in from white-out
        }
        if (fade.curamt==PAL_FADE_SHADES)
        {
           fade.mode = FADE_COMPLETE;
        }
        g_pGraphics->fadePalette(fade.curamt);
      }
      else if (fade.dir==FADE_OUT)
      {
        fade.curamt--;
        if (fade.curamt==0) fade.mode = FADE_COMPLETE;
        g_pGraphics->fadePalette(fade.curamt);
      }
      fade.fadetimer = 0;
    }
    else
    {
      fade.fadetimer++;
    }
}

// same as above but only does a sb_blit, not the full RenderScreen.
// used for intros etc.
void gamedo_frameskipping_blitonly()
{
    if (g_pTimer->TimeToRender())
    	g_pVideoDriver->sb_blit();
}

