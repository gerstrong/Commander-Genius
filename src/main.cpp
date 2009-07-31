
/* MAIN.C
  This is CloneKeen's main source file.

  The CloneKeen source may be freely distributed and
  modified as per the GPL but please give credit to
  the original author, Caitlin Shaw and to the new author
  Gerstrong.

  If you make any changes or improvements to the code that
  you feel merit inclusion in the source tree email them
  to me at gerstrong@gmail.com or get my latest email
  from the SourceForge site.

  Thanks to ID Software for the "Commander Keen: Invasion of
  the Vorticons" games. "Commander Keen" and it's associated
  graphics, level, and sound files are the property of ID
  Software. Commander Genius requires the original version of a
  Commander Keen game in order to be able to emulate that
  episode.

  Enjoy the Code
         - The Commander Genius Team

  CloneKeen 		2003-2005 Caitlin Shaw
  CloneKeenPlus 	2008-2009 Gerstrong
  Commander Genius 	2009	  Tulip, Pickle, DaVince and Albert
*/

#include "keen.h"
#include "sdl/joydrv.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "include/misc.h"
#include "include/menu.h"
#include "sdl/CVideoDriver.h"
#include "include/game.h"
#include "include/eseq_ep2.h"
#include "include/fileio.h"
#include "include/fileio/story.h"
#include "include/main.h"
#include "fileio/CParser.h"
#include "vorticon/CPlayer.h"
#include "vorticon/CHighScores.h"
#include "CLogFile.h"
#include "CGame.h"
#include "CGraphics.h"
#include "sdl/CSettings.h"
#include "FindFile.h"


int IntroCanceled;
int NessieObjectHandle;
int DemoObjectHandle;
int BlankSprite;
int DemoSprite;
int framebyframe;
int fps=0, curfps=0;

stOption *options = NULL;


unsigned int demo_RLERunLen;
unsigned char demo_data[DEMO_MAX_SIZE+1];
unsigned int demo_data_index;

char QuitState = NO_QUIT;

stString strings[MAX_STRINGS+1];
int numStrings = 0;

int demomode;
FILE *demofile = NULL;

char ScreenIsScrolling;
int gunfiretimer, gunfirefreq;

extern unsigned long scroll_x;
extern unsigned int scrollx_buf;
extern unsigned char scrollpix;
extern unsigned int mapx;
extern unsigned int mapxstripepos;
extern unsigned int scroll_y;
extern unsigned int scrolly_buf;
extern unsigned char scrollpixy;
extern unsigned int mapy;
extern unsigned int mapystripepos;

extern CPlayer *Player;

char loadinggame, loadslot;

stFade fade;
stMap map;
unsigned int AnimTileInUse[ATILEINUSE_SIZEX][ATILEINUSE_SIZEY];
stTile tiles[MAX_TILES+1];
int numtiles;
int **TileProperty = NULL; // This version will replace the old stTile Structure and save memory
unsigned char tiledata[MAX_TILES+1][16][16];
stSprite *sprites = NULL;
stBitmap bitmaps[MAX_BITMAPS+1];
stObject objects[MAX_OBJECTS+1];
char font[MAX_FONT+1][8][8];
stAnimTile animtiles[MAX_ANIMTILES+1];
stPlayer player[MAX_PLAYERS];
stPlayer net_lastplayer[MAX_PLAYERS];

unsigned int scroll_y = 0;
unsigned int objdefsprites[NUM_OBJ_TYPES+1];
char frameskiptimer=0;

int thisplayer;
unsigned int primaryplayer;
unsigned int numplayers;

int crashflag,crashflag2,crashflag3;
const char *why_term_ptr = "No reason given.";

int main(int argc, char *argv[])
{
	banner(); // Intro on the text-console.

	if(argc >= 1) {
		binary_dir = argv[0];
		size_t slashpos = findLastPathSep(binary_dir);
		if(slashpos != std::string::npos)  {
			binary_dir.erase(slashpos);
			binary_dir = SystemNativeToUtf8(binary_dir);
		} else
			binary_dir = ".";
	} else {
		warnings << "Binary-argument not given, assuming current dir" << endl;
		binary_dir = ".";
	}
	binary_dir = GetAbsolutePath(binary_dir);
	
	InitSearchPaths();
	
	stCloneKeenPlus CKP; // This is the future main structure of CloneKeen. It will be one variable which controls all
						 // the program instead of having global variables around somewhere.

	g_pLogFile->CreateLogfile("CGLog.html");

	CGame Game;
	Game.preallocateCKP(&CKP);

	CSettings Settings;
	if(Settings.loadDrvCfg() != 0) // Always return 0 if no ERROR
	{
		g_pLogFile->textOut(RED,"First time message: CKP didn't find the driver config file. However, it is going to generate one basing on default configurations.<br>");
		Settings.saveDrvCfg();
	}

	if(readCommandLine(argc, argv, &CKP) != 0)
	{
		g_pLogFile->textOut(RED,"Sorry, but CKP needs correct command line parameters.<br>");
		printf("Sorry, but CKP needs correct command line parameters.\n");
	}

	if(loadCKPDrivers(&CKP) != 0)
	{
		g_pLogFile->textOut(RED,"The game cannot start, because you do not meet the hardware requirements.<br>");
		return 1;
	}

	Settings.loadDefaultGameCfg(CKP.Option);
	if(Settings.loadGameCfg(CKP.Option) != 0)
	{
		g_pLogFile->textOut(PURPLE,"There are no settings! CKP is going to use the default options. You can change them later in the game.<br>");
		Settings.saveGameCfg(CKP.Option);
	}

	if(loadResourcesforStartMenu(&CKP, &Game) != 0)
	{
		g_pLogFile->textOut(RED,"Error! Resources for start menu cannot be loaded! Maybe you need to copy the data files!<br>");
		return 1;
	}

	if(!CKP.Control.skipstarting)
	{
		while(!loadStartMenu(&CKP))
		{
			g_pLogFile->textOut(PURPLE,"Error! You have chosen a Game that doesn't exist. Please correct the \"games.cfg\" File under \"data\" and choose another game.<br>");
			//cleanupResources(&CKP);
			//return 0;
		}
	}

	if(!g_pInput->getExitEvent())
	{

		CKP.Control.levelcontrol.episode = CKP.GameData[CKP.Resources.GameSelected-1].Episode; // Assign the correct Episode

		options = CKP.Option;
		if(Game.loadResources(CKP.Control.levelcontrol.episode, CKP.GameData[CKP.Resources.GameSelected-1].DataDirectory))
			CKP.shutdown = SHUTDOWN_BOOTUP; // Prepare game for starting
		else
			CKP.shutdown = SHUTDOWN_NEW_GAME;

		while( CKP.shutdown == SHUTDOWN_RESTART || CKP.shutdown == SHUTDOWN_BOOTUP || CKP.shutdown == SHUTDOWN_NEW_GAME )
		{
			if(CKP.shutdown != SHUTDOWN_NEW_GAME) {
				CKP.shutdown = SHUTDOWN_NONE; // Game is runnning
				Game.runCycle(&CKP);
			}
			
			if(CKP.shutdown == SHUTDOWN_NEW_GAME)
			{
				while(!loadStartMenu(&CKP))
				{
					g_pLogFile->textOut(PURPLE,"Error! You have chosen a Game that doesn't exist. Please correct the \"games.cfg\" File under \"data\" and choose another game.<br>");
					//cleanupResources(&CKP);
					//return 0;
				}

				//loadResourcesforGame(pCKP);
				if(Game.loadResources(CKP.Control.levelcontrol.episode, CKP.GameData[CKP.Resources.GameSelected-1].DataDirectory))
				  CKP.shutdown = SHUTDOWN_RESTART;
				else
				  CKP.shutdown = SHUTDOWN_NEW_GAME;
			}
		}
	}

	Settings.saveGameCfg(CKP.Option);

	cleanupResources(&CKP);

	Settings.saveDrvCfg();
	
	printf("Thank you very much for playing this wonderful game!");

	return 0;
}

void cleanupResources(stCloneKeenPlus *pCKP)
{
	cleanup(pCKP);
	return;
}

short loadCKPDrivers(stCloneKeenPlus *pCKP)
{
	g_pGraphics->allocScrollBufmem();

	// initialize/activate all drivers
	g_pLogFile->ftextOut("Starting graphics driver...<br>");

	if (!g_pVideoDriver->start())
		return abortCKP(pCKP);

	g_pLogFile->ftextOut("Starting sound driver...<br>");

	g_pSound->init();

	g_pLogFile->ftextOut("Starting input driver...<br>");
	JoyDrv_Start(&(pCKP->Device.Joystick));
	g_pInput->loadControlconfig();

	return 0;
}

short abortCKP(stCloneKeenPlus *pCKP)
{
	  g_pLogFile->ftextOut("Fatal error, cleaning up.<br>");
	  cleanup(pCKP);
	  g_pLogFile->ftextOut("A fatal error has occurred; game is shutting down.<br>");
	  return 1;
}

short closeCKP(stCloneKeenPlus *pCKP)
{
	int count;
	int i;
	CSettings Settings;

	banner();

	Settings.saveGameCfg(pCKP->Option);

	g_pLogFile->ftextOut("<br>Thanks for playing!<br><br>");
	cleanup(pCKP);
	if (crashflag)
	{
		if (crashflag != QUIT_NONFATAL) g_pLogFile->ftextOut("\a");
		g_pLogFile->ftextOut("abnormal program termination, error code %d/%d/%d<br>explanation: %s<br>", crashflag,crashflag2,crashflag3, why_term_ptr);

		g_pLogFile->ftextOut("numplayers: %d<br>", numplayers);

		for(count=0,i=0;i<MAX_PLAYERS;i++) if (player[i].isPlaying) count++;

		g_pLogFile->ftextOut("# of player instances with isPlaying set: %d<br>", count);
	}
	return 0;
}

void playgame_levelmanager(stCloneKeenPlus *pCKP)
{
  int i, o, wm, firsttime = 1;
  char levelname[80];
  char SaveGameFileName[40];
  int newlevel;

  stLevelControl *p_levelcontrol;
  stOption	*p_option;

  p_option = pCKP->Option;
  p_levelcontrol = &(pCKP->Control.levelcontrol);

  if( p_levelcontrol->command != LVLC_START_LEVEL )
  {
	  p_levelcontrol->command = LVLC_CHANGE_LEVEL;
	  p_levelcontrol->chglevelto = WM_MAP_NUM;
  }
  p_levelcontrol->tobonuslevel = 0;
  p_levelcontrol->success = 0;
  map.firsttime = 1;

  do
  {
    initgame(pCKP);

    newlevel = p_levelcontrol->chglevelto;
    if (p_levelcontrol->episode==1 && p_levelcontrol->hardmode)
    {
        // in high-difficulity mode switch levels 5 & 9 so
        // you can't get the pogo stick until you make it
        // to the dark side of mars.
        if (newlevel==5)
           newlevel = 9;
        else if (newlevel==9)
           newlevel = 5;
    }
    sprintf(levelname, "level%02d.ck%d", newlevel, p_levelcontrol->episode);

    if (p_levelcontrol->chglevelto==WORLD_MAP)
    {
      wm = 1;
    }
    else
    {
      wm = 0;
    }
    p_levelcontrol->canexit = 1;   // assume can exit before loading map
    if (loadmap(levelname, pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory, p_levelcontrol->chglevelto, wm, pCKP))
    {
      crashflag = 1;
      crashflag2 = p_levelcontrol->chglevelto;
      why_term_ptr = "Unable to load the map (# shown in crashflag2).";
    }

    p_levelcontrol->curlevel = p_levelcontrol->chglevelto;
    if (p_levelcontrol->curlevel == FINAL_MAP && p_levelcontrol->episode == 1)
    {
      p_levelcontrol->isfinallevel = 1;
      p_levelcontrol->canexit = 0;
    }
    else
    {
      p_levelcontrol->isfinallevel = 0;
    }

    if (firsttime)
    {
    	int op;
    	for(i=0;i<MAX_PLAYERS;i++)
    	{
    		op = player[i].useObject;
      		player[i].mapplayx = player[i].x;
    		player[i].mapplayy = player[i].y;
    		player[i].w = sprites[objects[op].sprite].xsize-4;
    		player[i].h = sprites[objects[op].sprite].ysize;
    	}
    }
    firsttime = 0;

    p_levelcontrol->command = LVLC_NOCOMMAND;

    p_levelcontrol->dark = 0;
    p_levelcontrol->usedhintmb = false;
    if (loadinggame)
    {
      sprintf(SaveGameFileName, "ep%csave%c.dat", p_levelcontrol->episode+'0', loadslot+'0');
      wm = savegameiswm(SaveGameFileName);
      if (game_load(SaveGameFileName, pCKP))
      {
        crashflag = 1;
        crashflag2 = loadslot;
        why_term_ptr = "Error loading game (slot # in flag2)! The save file may be corrupt or created by a different version of CloneKeen.";
        return;
      }
    }
	g_pGraphics->initPalette(p_levelcontrol->dark);

    // Now load HQ Stuff, because the game could have been loaded too.
	g_pGraphics->loadHQGraphics(p_levelcontrol->episode,p_levelcontrol->chglevelto,pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);

	g_pInput->flushAll();

    if (wm)
    {  // entering map from normal level, or first time around
      if (!p_levelcontrol->tobonuslevel)
      {
        if (!loadinggame)
        {
          for(i=0;i<MAX_PLAYERS;i++)
          {
            player[i].x = player[i].mapplayx;
            player[i].y = player[i].mapplayy;
          }
        }
      }
      else
      {  // respawn at the bonus level
        for(i=0;i<MAX_PLAYERS;i++)
        {
          player[i].x = BONUSLEVEL_RESPAWN_X;
          player[i].y = BONUSLEVEL_RESPAWN_Y;
          if (player[i].isPlaying && player[i].inventory.lives)
          {
             player[i].hideplayer = 1;
             o = spawn_object((player[i].x>>CSF>>4)<<CSF<<4,((player[i].y>>CSF>>4)+1)<<CSF<<4,OBJ_TELEPORTER);
             objects[o].ai.teleport.direction = TELEPORTING_IN;
             objects[o].ai.teleport.whichplayer = i;
             objects[o].ai.teleport.baseframe = TELEPORT_RED_BASEFRAME_EP1;
             objects[o].ai.teleport.idleframe = TELEPORT_RED_IDLEFRAME_EP1;
             g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, objects[player[i].useObject].scrx);
          }
        }
      }

      if (!p_levelcontrol->success || firsttime)
      {
        if (!p_levelcontrol->tobonuslevel) p_levelcontrol->dokeensleft = 1;
        // when you die you lose all keycards
        for(i=0;i<MAX_PLAYERS;i++)
        {
          if (player[i].isPlaying)
          {
              take_keycard(DOOR_YELLOW, i);
              take_keycard(DOOR_RED, i);
              take_keycard(DOOR_GREEN, i);
              take_keycard(DOOR_BLUE, i);
           }
        }
      }
      else p_levelcontrol->dokeensleft = 0;

      gameloop(pCKP);

      for(i=0;i<MAX_PLAYERS;i++)
      {
        player[i].mapplayx = player[i].x;
        player[i].mapplayy = player[i].y;
      }

    }
    else
    {
       // entering a normal level from map
       p_levelcontrol->dokeensleft = 0;
       gameloop(pCKP);

       // after completion of a normal level check if the game is won
       if (gameiswon(pCKP))
       {
          p_levelcontrol->command = LVLC_END_SEQUENCE;
       }
    }

    if(QuitState==QUIT_PROGRAM)
    {
    	pCKP->shutdown = SHUTDOWN_EXIT;
    	break;
    }
    g_pGraphics->unloadHQGraphics();

    for(unsigned int i=0;i<numplayers;i++)
    {
    	player[i].x = 0;
    	player[i].y = 0;
    }

  } while(p_levelcontrol->command==LVLC_CHANGE_LEVEL && !crashflag);

  g_pGraphics->unloadHQGraphics();

  if (p_levelcontrol->command==LVLC_END_SEQUENCE)
  {
    endsequence(pCKP);
    g_pLogFile->ftextOut("eseq complete<br>");
  }
  else if (p_levelcontrol->command==LVLC_TANTALUS_RAY)
  {
    eseq2_vibrate(pCKP);
    eseq2_TantalusRay(pCKP);
    IntroCanceled = 1;               // popup main menu immediately
  }
}

// plays the demo file specified in fname
// returns:
//  DEMO_RESULT_FILE_BAD               demo does not exist or file corrupt
//  DEMO_RESULT_COMPLETED              demo played all the way through
//  DEMO_RESULT_CANCELED               user canceled the demo
char play_demo(int demonum, stCloneKeenPlus *pCKP, int s)
{
int i;
int byt;
int lvl;
char filename[40];
char SaveOptions[NUM_OPTIONS];
	stLevelControl *p_levelcontrol;
	stOption *p_option;

	p_levelcontrol = &(pCKP->Control.levelcontrol);
	p_option = pCKP->Option;

   // open the demo file
   sprintf(filename, "ep%ddemo%d.dat", p_levelcontrol->episode, demonum);
   demofile = OpenGameFile(filename, "rb");
   if (!demofile)
   {
     return DEMO_RESULT_FILE_BAD;
   }

   // read in the header
   if (fgetc(demofile) != 'D') goto demoHeaderCorrupt;
   if (fgetc(demofile) != 'M') goto demoHeaderCorrupt;
   if (fgetc(demofile) != 'O') goto demoHeaderCorrupt;
   if (fgetc(demofile) != p_levelcontrol->episode) goto demoHeaderCorrupt;
   lvl = fgetc(demofile);

   // load the compressed demo into the demo_data[] array
   for(i=0;i<DEMO_MAX_SIZE;i++)
   {
     byt = fgetc(demofile);     // get byte from file
     if (byt<0) goto gotEOF;    // check for EOF
     demo_data[i] = byt;        // copy to demo_data[]
   }
   why_term_ptr = "Demo file is too big! (Increase DEMO_MAX_SIZE)";
   crashflag = 1;
gotEOF: ;
   fclose(demofile);
   demofile = NULL;

   // initilize some variables
   demo_RLERunLen = 0;
   demo_data_index = 0;
   demomode = DEMO_PLAYBACK;
   loadinggame = 0;
   p_levelcontrol->curlevel = lvl;
   p_levelcontrol->command = LVLC_NOCOMMAND;

   initgamefirsttime(pCKP, s);
   initgame(pCKP);

   // now load the map and play the level
   sprintf(filename, "level%02d.ck%d", p_levelcontrol->curlevel, p_levelcontrol->episode);
   if (loadmap(filename, pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory,
		   p_levelcontrol->curlevel, 0, pCKP)) return DEMO_RESULT_FILE_BAD;

   for(i=0;i<NUM_OPTIONS;i++) SaveOptions[i] = p_option[i].value;
   // SetDefaultOptions();
   gameloop(pCKP);
   for(i=0;i<NUM_OPTIONS;i++) p_option[i].value = SaveOptions[i];

   // based on success/failure returned from gameloop let the
   // calling procedure know whether the user canceled or not

   if (p_levelcontrol->success)
   {
     return DEMO_RESULT_COMPLETED;
   }
   else
   {
     return DEMO_RESULT_CANCELED;
   }

// this label is jumped to when there's an error reading the header.
// it closes the demo file and aborts.
demoHeaderCorrupt: ;
   fclose(demofile);
   demofile = NULL;
   return DEMO_RESULT_FILE_BAD;
}

short readCommandLine(int argc, char *argv[], stCloneKeenPlus *pCKP)
{
	int i;
	static const int MAX_STRING_LENGTH = 256;
	char tempbuf[MAX_STRING_LENGTH];


	unsigned short *p_gamesel;
	  // process command line options
	  g_pLogFile->ftextOut("Processing command-line options.<br>");
	  numplayers = 1;
	  p_gamesel = &(pCKP->Resources.GameSelected);

	  if (argc>1)
	  {
	    for(i=1;i<argc;i++)
	    {
	      strcpy(tempbuf, argv[i]);
	      if (strcmp(tempbuf, "-1player")==0)
	      {
	        numplayers = 1;
	      }
	      else if (strcmp(tempbuf, "-2player")==0)
	      {
	        numplayers = 2;
	      }
	      else if (strcmp(tempbuf, "-3player")==0)
	      {
	        numplayers = 3;
	      }
	      else if (strcmp(tempbuf, "-4player")==0)
	      {
	        numplayers = 4;
	      }
	      else if (strcmp(tempbuf, "-single")==0)
	      {
	        numplayers = 1;
	      }
	      else if (strcmp(tempbuf, "-nopk")==0)     // do not allow players to kill each other
	      {
	    	  pCKP->Option[OPT_ALLOWPKING].value = 0;
	      }
	      else if (strncmp(tempbuf, "-game",strlen("-game"))==0)      // select the game
	      {
	    	int g;

	    	sscanf(tempbuf+strlen("-game"),"%d",&g);

	        *p_gamesel = g;
	      }
	      else if (strcmp(tempbuf, "-dtm")==0)      // direct to map
	      {
	        pCKP->Control.dtm = 1;
	      }
	      else if (strcmp(tempbuf, "-mean")==0)     // increase difficulty
	      {
			  pCKP->Control.levelcontrol.hardmode = true;
	      }
	      else if (strcmp(tempbuf, "-cheat")==0)    // enable cheat codes
	      {
	    	  pCKP->Option[OPT_CHEATS].value = 1;
	      }
	      else if (strcmp(tempbuf, "-rec")==0)      // record a demo
	      {
	        demomode = DEMO_RECORD;
	      }
	      else if (strcmp(tempbuf, "-eseq")==0)     // play end sequence
	      {
	        pCKP->Control.eseq = 1;
	      }

	      else if (strcmp(tempbuf, "-story")==0)     // play end sequence
	      {
	        pCKP->Control.storyboard = 1;
	        pCKP->Control.skipstarting = 1;
	      }
	      else if (strcmp(tempbuf, "-fs")==0)       // full-screen
	      {
	        g_pVideoDriver->isFullscreen(true);
	      }
	      else if (strcmp(tempbuf, "-dbl")==0)       // 2x
	      {
	        g_pVideoDriver->setZoom(2);
	      }
	      else if (strcmp(tempbuf, "-stereo")==0)       // Enable Stereo Sound
	      {
	    	  g_pSound->setSoundmode(0,true);
	      }
	      else if (strcmp(tempbuf, "-showfps")==0)  // show fps
	      {
	        g_pVideoDriver->showFPS(true);
	      }
	      else if (strncmp(tempbuf, "-level",strlen("-level"))==0)      // select the game
	      {
	    	int g;
	    	sscanf(tempbuf+strlen("-level"),"%d",&g);

			  pCKP->Control.levelcontrol.command = LVLC_START_LEVEL;
			  pCKP->Control.levelcontrol.chglevelto = g;
			  pCKP->Control.skipstarting = 1;
	      }
	      else if (i!=1 || atoi(argv[i])==0)
	      {
	        g_pLogFile->ftextOut("Wait a minute...what the hell does '%s' mean? I'm going to ignore this!<br>",tempbuf);
	        usage();
	        return 1;
	      }
	    }
	  }
	  return 0;
}
