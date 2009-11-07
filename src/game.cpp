/* GAME.C
 Main and miscellaneous functions for in-game, contains the main
 game loop, etc.
 */

#include "keen.h"
#include "demobox.h"
#include "CGame.h"
#include "common/CMenu.h"
#include "game.h"
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CSound.h"
#include "include/misc.h"
#include "include/gamedo.h"
#include "include/gamepdo.h"
#include "include/gm_pdowm.h"
#include "hqp/CMusic.h"
#include "graphics/CGfxEngine.h"
#include "vorticon/CHighScores.h"
#include "CLogFile.h"

void start_gameover(stCloneKeenPlus *pCKP)
{
	/*int cities=0;
   	 CHighScores *HighScoreTable = new CHighScores(g_pVideoDriver->FGLayerSurface, pCKP);
	 
	 bool extras[4] = {false,false,false,false};
	 
	 stLevelControl *p_levelcontrol = &(pCKP->Control.levelcontrol);
	 
   	 // check inventory or saved cities
   	 if(p_levelcontrol->episode == 1)
   	 {
	 if(player[0].inventory.HasJoystick)
	 extras[0] = true;
	 if(player[0].inventory.HasBattery)
	 extras[1] = true;
	 if(player[0].inventory.HasVacuum)
	 extras[2] = true;
	 if(player[0].inventory.HasWiskey)
	 extras[3] = true;
   	 }
   	 else if(p_levelcontrol->episode == 2)
   	 {
	 if (p_levelcontrol->levels_completed[4]) cities++;
	 if (p_levelcontrol->levels_completed[6]) cities++;
	 if (p_levelcontrol->levels_completed[7]) cities++;
	 if (p_levelcontrol->levels_completed[13]) cities++;
	 if (p_levelcontrol->levels_completed[11]) cities++;
	 if (p_levelcontrol->levels_completed[9]) cities++;
	 if (p_levelcontrol->levels_completed[15]) cities++;
	 if (p_levelcontrol->levels_completed[16]) cities++;
   	 }
	 
   	 HighScoreTable->writeHighScore((int)player[0].inventory.score,extras,cities);
	 
   	 HighScoreTable->showHighScore();
	 
   	 delete HighScoreTable;
	 
     p_levelcontrol->command = LVLC_GAME_OVER;*/
}

void initgame(stLevelControl *p_levelcontrol)
{
	/*int x,y;
	 unsigned int i;
	 
	 animtiletimer = curanimtileframe = 0;
	 
	 // reset player walk frame widths. Lame!
	 for(i=0;i<numplayers;i++)
	 {
	 g_pGfxEngine->Sprite[playerbaseframes[i]+0]->setWidth(PLAYERSPRITE_WIDTH);
	 g_pGfxEngine->Sprite[playerbaseframes[i]+1]->setWidth(PLAYERSPRITE_WIDTH);
	 g_pGfxEngine->Sprite[playerbaseframes[i]+2]->setWidth(PLAYERSPRITE_WIDTH);
	 g_pGfxEngine->Sprite[playerbaseframes[i]+3]->setWidth(PLAYERSPRITE_WIDTH);
	 }
	 
	 // set gun/ice cannon fire freq
	 if (p_levelcontrol->episode==1)
	 {
	 gunfirefreq = ICECANNON_FIRE_FREQ;
	 }
	 else
	 {
	 gunfirefreq = GUN_FIRE_FREQ;
	 }
	 
	 // reset the ysize attribute of all doors
	 CSprite **sprites = &g_pGfxEngine->Sprite[0];
	 sprites[DOOR_YELLOW_SPRITE]->setHeight(32);
	 sprites[DOOR_RED_SPRITE]->setHeight(32);
	 sprites[DOOR_GREEN_SPRITE]->setHeight(32);
	 sprites[DOOR_BLUE_SPRITE]->setHeight(32);
	 
	 p_levelcontrol->level_done_timer = 0;
	 p_levelcontrol->gameovermode = false;
	 
	 // all objects -> not exist
	 for(i=1;i<MAX_OBJECTS-1;i++) objects[i].exists = 0;
	 // clear out AnimTileInUse array
	 for(y=0;y<ATILEINUSE_SIZEX-1;y++)
	 for(x=0;x<ATILEINUSE_SIZEY-1;x++)
	 AnimTileInUse[x][y] = 0;
	 
	 // set all animated tile slots to "not in use"
	 //map_unregister_all_animtiles();
	 
	 // initilize default sprites for objects
	 memset(objdefsprites, 0, sizeof(objdefsprites));
	 
	 objdefsprites[OBJ_AUTORAY] = ENEMYRAYEP3;
	 objdefsprites[OBJ_AUTORAY_V] = RAY_VERT_EP3;
	 
	 objdefsprites[OBJ_DOOR] = DOOR_YELLOW_SPRITE;
	 objdefsprites[OBJ_TELEPORTER] = OBJ_TELEPORTER_DEFSPRITE;
	 
	 objdefsprites[OBJ_SECTOREFFECTOR] = BLANKSPRITE;
	 objdefsprites[OBJ_GOTPOINTS] = PT500_SPRITE;
	 
	 if (p_levelcontrol->episode==1)
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
	 // make ice cannon markers show up in level editor
	 objdefsprites[OBJ_ICECANNON] = OBJ_ICECHUNK_DEFSPRITE;
	 }
	 else if (p_levelcontrol->episode==2)
	 {
	 objdefsprites[OBJ_SCRUB] = OBJ_SCRUB_DEFSPRITE;
	 objdefsprites[OBJ_TANKEP2] = OBJ_TANKEP2_DEFSPRITE;
	 objdefsprites[OBJ_VORTELITE] = OBJ_VORTELITE_DEFSPRITE;
	 
	 objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP2;
	 objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP2;
	 objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP2;
	 objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP2;
	 objdefsprites[OBJ_SPARK] = OBJ_SPARK_DEFSPRITE_EP2;
	 }
	 else if (p_levelcontrol->episode==3)
	 {
	 objdefsprites[OBJ_FOOB] = OBJ_FOOB_DEFSPRITE;
	 objdefsprites[OBJ_NINJA] = OBJ_NINJA_DEFSPRITE;
	 objdefsprites[OBJ_MOTHER] = OBJ_MOTHER_DEFSPRITE;
	 objdefsprites[OBJ_MEEP] = OBJ_MEEP_DEFSPRITE;
	 objdefsprites[OBJ_BALL] = OBJ_BALL_DEFSPRITE;
	 objdefsprites[OBJ_JACK] = OBJ_JACK_DEFSPRITE;
	 objdefsprites[OBJ_NESSIE] = OBJ_NESSIE_DEFSPRITE;
	 
	 objdefsprites[OBJ_RAY] = OBJ_RAY_DEFSPRITE_EP3;
	 objdefsprites[OBJ_VORT] = OBJ_VORT_DEFSPRITE_EP3;
	 objdefsprites[OBJ_PLATFORM] = OBJ_PLATFORM_DEFSPRITE_EP3;
	 objdefsprites[OBJ_PLATVERT] = OBJ_PLATFORM_DEFSPRITE_EP3;
	 objdefsprites[OBJ_BABY] = OBJ_BABY_DEFSPRITE_EP3;
	 }
	 
	 // initilize game variables
	 p_levelcontrol->level_done = LEVEL_NOT_DONE;
	 animtiletimer = curanimtileframe = 0;
	 DemoObjectHandle = 0;
	 
	 for(i=0;i<MAX_PLAYERS;i++)
	 {
	 player[i].isPlaying = 0;
	 }
	 
	 for(i=0;i<numplayers;i++)
	 {
	 player[i].isPlaying = 1;
	 player[i].useObject = (i+1); // player objects start at min 1 cause DemoObject is index 0
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
	 for(i=0;i<numplayers;i++)
	 {
	 if (player[i].isPlaying)
	 {
	 Uint8 obj = player[i].useObject;
	 objects[obj].exists = 1;
	 objects[obj].onscreen = 1;
	 objects[obj].type = OBJ_PLAYER;
	 objects[obj].sprite = 0;
	 objects[obj].onscreen = 1;
	 objects[obj].AssociatedWithPlayer = i;
	 objects[obj].honorPriority = 1;
	 objects[obj].canbezapped = false;
	 highest_objslot = obj + 1;
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
	 
	 thisplayer = 0;*/
}

int initgamefirsttime(stCloneKeenPlus *pCKP, int s)
{
	/*int i;
	 
	 map.firsttime = 1;
	 
	 for(i=0;i<MAX_LEVELS;i++)
	 pCKP->Control.levelcontrol.levels_completed[i] = 0;
	 
	 for(i=0;i<MAX_PLAYERS;i++)
	 {
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
	 
	 // start with pogo stick in all episodes but 1
	 if (pCKP->Control.levelcontrol.episode!=1 || pCKP->Control.levelcontrol.demomode)
	 { player[i].inventory.HasPogo = 1; }
	 else
	 { player[i].inventory.HasPogo = 0; }
	 }
	 
	 initsprites(pCKP, s);
	 
	 if (pCKP->Control.levelcontrol.demomode) srand(375);
	 
	 primaryplayer = 0;
	 */
	return 0;
}


int find_next_object(unsigned int type)
{
	/*for(int o=0 ; o<highest_objslot ; o++)
	 if(objects[o].type == type) return o;*/
	return -1;
}

void delete_all_objects(void)
{
	/*int i;
	 for(i=0;i<MAX_OBJECTS;i++)
	 {
	 if (objects[i].exists && objects[i].type != OBJ_PLAYER)
	 delete_object(i);
	 }
	 recalc_highest_objslot();*/
}

void recalc_highest_objslot(void)
{
	/*int i;
	 highest_objslot = 0;
	 for(i=MAX_OBJECTS-1;i>=0;i--)
	 {
	 if (objects[i].exists)
	 {
	 highest_objslot = i+1;
	 break;
	 }
	 }*/
}

void killobject(int o)
{
	/*if (objects[o].exists)
	 {
	 if (objects[o].type==OBJ_PLAYER)
	 {
	 killplayer(o);
	 }
	 else
	 {
	 if (objects[o].zapped < 500 && objects[o].canbezapped)
	 objects[o].zapped += 500;
	 }
	 }*/
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void kill_all_intersecting_tile(int mpx, int mpy)
{
	/*unsigned int xpix,ypix;
	 xpix = mpx<<TILE_S<<CSF;
	 ypix = mpy<<TILE_S<<CSF;
	 for(int i=0 ; i<highest_objslot ; i++)
	 if (objects[i].exists)
	 if (xpix <= objects[i].x && xpix+(16<<CSF) >= objects[i].x)
	 if (ypix <= objects[i].y && ypix+(16<<CSF) >= objects[i].y)
	 killobject(i);*/
}

void freezeplayer(int theplayer)
{
	/*if ( player[theplayer].godmode ) return;
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
	 player[theplayer].ppogostick = false;*/
}

// initialize sprites that come from tiles, such as the doors
void initsprites(stCloneKeenPlus *pCKP, int s)
{
    // create BLANKSPRITE
    //sprite[s]->setSize(0, 0);
	
    // TODO: Demo-Sprite must be added. This time loaded from one TGA File! The TGA is already there!
	
    // create the sprites for player 2
    /*s++;
	 playerbaseframes[1] = s;
	 for(i=0;i<48;i++)
	 {
	 sprite[i]->copy( sprite[s], g_pGfxEngine->Palette.m_Palette );
	 sprite[s]->replaceSpriteColor( 13, 10 ,0 );
	 sprite[s]->replaceSpriteColor( 5, 2 ,0 );
	 sprite[s]->replaceSpriteColor( 9, 14 ,8 );
	 sprite[s]->replaceSpriteColor( 1, 6 ,8 );
	 sprite[s]->replaceSpriteColor( 12, 11 ,0 );
	 sprite[s]->replaceSpriteColor( 4, 3 ,0 );
	 s++;
	 }*/
	
    // create the sprites for player 3
    // Unsupported for now...
}
