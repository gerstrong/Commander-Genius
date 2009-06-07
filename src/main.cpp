
/* MAIN.C
  This is CloneKeen's main source file.

  The CloneKeen source may be freely distributed and
  modified as per the GPL but please give credit to
  the original author, Caitlin Shaw and to the new author
  Gerstrong.

  Please go ahead and port this game to other platforms.
  I would love to see it on Linux, Mac, or CE but I do
  not have the platforms/knowledge to port to these platforms.

  If you make any changes or improvements to the code that
  you feel merit inclusion in the source tree email them
  to me at gerstrong@gmail.com or get my latest email
  from the SourceForge site.

  Thanks to ID Software for the "Commander Keen: Invasion of
  the Vorticons" games. "Commander Keen" and it's associated
  graphics, level, and sound files are the property of ID
  Software. CloneKeen requires the original version of a
  Commander Keen game in order to be able to emulate that
  episode.

  Enjoy the Code
         -Caitlin and Gerstrong

  CloneKeen 	2003-2005 Caitlin Shaw
  CloneKeenPlus 2008-2009 Gerstrong
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


int IntroCanceled;
int NessieObjectHandle;
int DemoObjectHandle;
int BlankSprite;
int DemoSprite;
int current_demo;
int framebyframe;
int fps=0, curfps=0;

stOption *options;


unsigned int demo_RLERunLen;
unsigned char demo_data[DEMO_MAX_SIZE+1];
unsigned int demo_data_index;

char QuitState = NO_QUIT;

stString strings[MAX_STRINGS+1];
int numStrings;

int demomode;
FILE *demofile;

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
unsigned char tiledata[MAX_TILES+1][16][16];
stSprite sprites[MAX_SPRITES+1];
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
char is_client;
char is_server;
char localmp;

int crashflag,crashflag2,crashflag3;
const char *why_term_ptr = "No reason given.";

void setoption(stOption *options, int opt, const char *name, char value)
{
  if (name != NULL)
    options[opt].name = (char*) name;

  options[opt].value = value;
}

short loadGamesConfig(stOption *Option)
{
	short retval = 0;
	int i;
	CParser Parser;

	if(!Parser.loadParseFile())
		return 1;

	  for (i = 0; i < NUM_OPTIONS; i++)
	  {
		  Option[i].value = Parser.getIntValue(Option[i].name,"Game");
		  if(Option[i].value == -1)
		  {
			  createDefaultSettings(Option);
			  break;
		  }
	  }

	return retval;

  g_pLogFile->ftextOut("<br>Your personal settings were loaded successfully...<br>");
  return 0;
}

void SaveConfig(stOption *Option)
{
	int i;
	CParser Parser;

	if(Parser.loadParseFile())
	{
		for (i = 0; i < NUM_OPTIONS; i++)
			Parser.saveIntValue(Option[i].name,"Game",Option[i].value);

		Parser.saveParseFile();
	}

	return;
}

int main(int argc, char *argv[])
{

	stCloneKeenPlus CKP; // This is the future main structure of CloneKeen. It will be one variable which controls all
						 // the program instead of having global variables around somewhere.

	g_pLogFile->CreateLogfile("CGLog.html");

	banner(); // Intro on the text-console.

	preallocateCKP(&CKP);

	if(loadDriverConfiguration(&CKP) != 0) // Always return 0 if no ERROR
	{
		g_pLogFile->textOut(RED,"First time message: CKP didn't find the driver config file. However, it is going to generate one basing on default configurations.<br>");
		saveDriverConfiguration(&CKP);
	}
	// TODO: Implement Driver Configuration as class

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

	createDefaultSettings(CKP.Option);
	if(loadGamesConfig(CKP.Option) != 0)
	{
		g_pLogFile->textOut(PURPLE,"There are no settings! CKP is going to use the default options. You can change them later in the game.<br>");
		SaveConfig(CKP.Option);
	}

	CGame* Game;
	Game = new CGame();

	if(loadResourcesforStartMenu(&CKP, Game) != 0)
	{
		g_pLogFile->textOut(RED,"Error! Resources for start menu cannot be loaded! Maybe you need to copy the data files!<br>");
		return 1;
	}

	if(!CKP.Control.skipstarting)
	{
		if(loadStartMenu(&CKP) == 1)
		{
			cleanupResources(&CKP);
			return 0;
		}
	}

	if(!g_pInput->getExitEvent())
	{

		CKP.Control.levelcontrol.episode = CKP.GameData[CKP.Resources.GameSelected-1].Episode; // Assign the correct Episode

		options = CKP.Option;
		Game->loadResources(CKP.Control.levelcontrol.episode, CKP.GameData[CKP.Resources.GameSelected-1].DataDirectory);

		//Game->runCycle();

		CKP.shutdown = SHUTDOWN_BOOTUP; // Prepare game for starting

		while( CKP.shutdown == SHUTDOWN_RESTART || CKP.shutdown == SHUTDOWN_BOOTUP )
		{
			CKP.shutdown = SHUTDOWN_NONE; // Game is runnning
			runGameCycle(&CKP, Game);
		}
	}

	if(Game){ delete Game; Game = NULL; }

	cleanupResources(&CKP);

	printf("Thank you very much for playing this wonderful game!");

	return 0;
}

short runGameCycle(stCloneKeenPlus *pCKP, CGame *Game)
{
  int opt = MAINMNU_1PLAYER;
  int retval;
  int eseq = 0;
  int defaultopt = 0;

  initgamefirsttime(pCKP, Game->getLatch()->getLatchHeader()->NumSprites);
  initgame(pCKP);

  g_pLogFile->ftextOut("Game starting...<br>");

  if (eseq)
  {
	  endsequence(pCKP);
	  closeCKP(pCKP);
  }

  if(!pCKP->Control.skipstarting)
  {
	  if (intro(pCKP)){ pCKP->shutdown=SHUTDOWN_EXIT; return 0;	}
	  pCKP->Control.skipstarting=0;
  }

  do
  {
    if (QuitState==QUIT_TO_TITLE) QuitState = NO_QUIT;

    if(pCKP->Control.storyboard == 1) // Show the story of the game
    {
    	char *text;
    	int textsize;

    	textsize = readStoryText(&text,
				pCKP->GameData[pCKP->Resources.GameSelected-1].Episode,
				pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory); // Read text from
																			   // and store it at the text pointer

    	if(textsize > 0)
    	{
				showPage(text,pCKP,textsize);
				free(text);
		}
    	else if(textsize == 0)
    	{
    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. Are you sure that there is any story text?");
    	}
    	else if(textsize == 0)
    	{
    		g_pLogFile->ftextOut("readStoryText(): Error reading story text. The version appears to be incompatible");
    	}
    	pCKP->Control.storyboard = 0;
    }

    if(pCKP->Control.levelcontrol.command != LVLC_START_LEVEL)
    {
    	g_pLogFile->ftextOut("calling mainmenu()<br>");

		opt = mainmenu(pCKP, defaultopt); // Read option from the main menu
										  // of the game.

		pCKP->Control.skipstarting=0;

		g_pLogFile->ftextOut("gcl: opt = %d<br>", opt);
    }

    defaultopt = 0;
    IntroCanceled = 0;
    switch(opt)
      {
      case MAINMNU_1PLAYER:
        numplayers = 1;
        defaultopt = 0;
        current_demo = 1;
        initgamefirsttime(pCKP, Game->getLatch()->getLatchHeader()->NumSprites);
        loadinggame = 0;
        playgame_levelmanager(pCKP);
        break;
      case MAINMNU_2PLAYER:
        defaultopt = 0;
        current_demo = 1;
        numplayers = 2;
        initgamefirsttime(pCKP, Game->getLatch()->getLatchHeader()->NumSprites);
        loadinggame = 0;
        playgame_levelmanager(pCKP);
        break;
      case MAINMNU_LOADGAME:
        if (loadslot)
        {
           loadinggame = 1;
           defaultopt = 0;
           current_demo = 1;
           numplayers = 1; // here was 2. Why was that? I don't understand
           initgamefirsttime(pCKP, Game->getLatch()->getLatchHeader()->NumSprites);
           playgame_levelmanager(pCKP);
        }
        break;

      case MAINMNU_STORY:
          pCKP->Control.storyboard=1;
          break;

      case MAINMNU_HIGHSCORES:
    	  CHighScores *pHighscores;
    	  pHighscores = new CHighScores(pCKP);
    	  if(pHighscores->showHighScore())
    	  {
    		  g_pLogFile->ftextOut("Error processing Highscore!!<br>");
    	  }
    	  delete pHighscores;
    	  break;

      case MAINMNU_NEW_GAME:
    	  if(loadStartMenu(pCKP) == 1)
    	  {
    			pCKP->shutdown = SHUTDOWN_EXIT;
    			break;
    	  }
    	  //loadResourcesforGame(pCKP);
    	  Game->loadResources(pCKP->Control.levelcontrol.episode, pCKP->GameData[pCKP->Resources.GameSelected-1].DataDirectory);


    	  pCKP->shutdown = SHUTDOWN_RESTART;
    	break;

      case MAINMNU_TIMEOUT:
      case MAINMNU_DEMO:
        retval = play_demo(current_demo, pCKP, Game->getLatch()->getLatchHeader()->NumSprites);

        if (retval==DEMO_RESULT_FILE_BAD)
        {
           // we tried to play a demo that did not exist--assume we
           // reached the last demo and go back to the intro
           intro(pCKP);
           current_demo = 0;
        }
        else if (retval==DEMO_RESULT_CANCELED)
        { // user hit a key to cancel demo
           IntroCanceled = 1;            // pop up menu
        }

        if (IntroCanceled)
        { // user canceled out of demo (or intro if at end of demos)
           // if user selected "demo" have it selected when he comes back
           if (opt==MAINMNU_DEMO)
           {
             defaultopt = MAINMNU_DEMO;
           }
        }

        current_demo++;
      break;
      case RESTART_GAME:
        g_pLogFile->ftextOut("********************<br>");
        g_pLogFile->ftextOut(" Restarting game...<br>");
        g_pLogFile->ftextOut("********************<br>\n");
        cleanup(pCKP);
        pCKP->shutdown = SHUTDOWN_RESTART;
        return 0;
      break;
      case BACK2MAINMENU:

      default: break;
      }
      g_pLogFile->ftextOut("bottom of game control loop opt=%d crashflag=%d<br>", opt, crashflag);
      if(pCKP->shutdown == SHUTDOWN_EXIT) break;
    } while(opt != MAINMNU_QUIT && opt != MAINMNU_NEW_GAME && !crashflag);

	return 0;
}

void cleanupResources(stCloneKeenPlus *pCKP)
{
	cleanup(pCKP);
	return;
}

void createDefaultSettings(stOption *Option)
{
	setoption(Option,OPT_FULLYAUTOMATIC, "autogun", 0);
	setoption(Option,OPT_SUPERPOGO, "superpogo", 0);
	setoption(Option,OPT_ALLOWPKING, "pking", 1);
	setoption(Option,OPT_CHEATS, "allcheats", 0);
	setoption(Option,OPT_TWOBUTTON, "two-button-firing", 0);
	setoption(Option,OPT_KEYCARDSTACK, "keycard-stacking", 0);
	setoption(Option,OPT_ANALOGJOYSTICK, "analog-joystick", 1);
}

short loadCKPDrivers(stCloneKeenPlus *pCKP)
{
	g_pGraphics->allocScrollBufmem();

	// initialize/activate all drivers
	g_pLogFile->ftextOut("Starting graphics driver...<br>");
	if (!g_pVideoDriver->start())
	{
		return abortCKP(pCKP);
	}

	g_pLogFile->ftextOut("Starting sound driver...<br>");

	g_pSound->init();

	g_pLogFile->ftextOut("Starting input driver...<br>");
	JoyDrv_Start(&(pCKP->Device.Joystick));
	g_pInput->loadControlconfig();

	return 0;
}

short saveDriverConfiguration(stCloneKeenPlus *pCKP)
{
	short retval = 0;

	CParser Parser;

	Parser.saveIntValue("bpp","Video",g_pVideoDriver->getDepth());
	Parser.saveIntValue("frameskip","Video",g_pVideoDriver->getFrameskip());

	if(g_pVideoDriver->getFullscreen())
		Parser.saveIntValue("fullscreen","Video",1);
	else
		Parser.saveIntValue("fullscreen","Video",0);

	if(g_pVideoDriver->isOpenGL())
		Parser.saveIntValue("OpenGL","Video",1);
	else
		Parser.saveIntValue("OpenGL","Video",0);


	Parser.saveIntValue("width","Video",g_pVideoDriver->getWidth());
	Parser.saveIntValue("height","Video",g_pVideoDriver->getHeight());
	Parser.saveIntValue("scale","Video",g_pVideoDriver->getZoomValue());
	Parser.saveIntValue("OGLfilter","Video",g_pVideoDriver->getOGLFilter());
	Parser.saveIntValue("filter","Video",g_pVideoDriver->getFiltermode());
	Parser.saveIntValue("autoframeskip","Video",g_pVideoDriver->getTargetFPS());

	Parser.saveIntValue("channels","Audio",(g_pSound->getAudioSpec()).channels);
	Parser.saveIntValue("format","Audio",(g_pSound->getAudioSpec()).format);
	Parser.saveIntValue("rate","Audio",(g_pSound->getAudioSpec()).freq);
	Parser.saveIntValue("mixerch","Audio",(g_pSound->getMixingchannels()));

	Parser.saveParseFile();

	return retval;
}

short loadDriverConfiguration(stCloneKeenPlus *pCKP)
{
	short retval = 0;
	CParser Parser;

	if(!Parser.loadParseFile())
	{
		retval = 1;
	}
	else
	{
		int width, height, depth;

		depth  = Parser.getIntValue("bpp","Video");
		width  = Parser.getIntValue("width","Video");
		height = Parser.getIntValue("height","Video");

		if(depth*width*height < 0)
			g_pLogFile->ftextOut(RED,"Error reading the configuration file. It appears to be damaged!");

		g_pVideoDriver->setMode(width, height, depth);

		g_pVideoDriver->setFrameskip(Parser.getIntValue("frameskip","Video"));

		if((Parser.getIntValue("fullscreen","Video")) == 1)
			g_pVideoDriver->isFullscreen(true);

		g_pVideoDriver->setOGLFilter(Parser.getIntValue("OGLfilter","Video"));
		g_pVideoDriver->setZoom(Parser.getIntValue("scale","Video"));
		g_pVideoDriver->setTargetFPS(Parser.getIntValue("autoframeskip","Video"));

		g_pVideoDriver->setFilter(Parser.getIntValue("filter","Video"));

		if(Parser.getIntValue("OpenGL","Video") == 1)
			g_pVideoDriver->enableOpenGL(true);
		else
			g_pVideoDriver->enableOpenGL(false);

		if(Parser.getIntValue("channels","Audio") == 2)
			g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"), true);
		else
			g_pSound->setSoundmode(Parser.getIntValue("rate","Audio"), false);
	}

	return retval;
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
	  banner();
	  SaveConfig(pCKP->Option);
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

  usedinfobox = 0;

  do
  {
    initgame(pCKP);

    newlevel = p_levelcontrol->chglevelto;
    if (p_levelcontrol->episode==1 && p_option[OPT_MEAN].value)
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
    if (p_levelcontrol->curlevel == FINAL_MAP)
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
   demofile = fopen(filename, "rb");
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
	    	  pCKP->Option[OPT_MEAN].value = 1;
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
	      else if (strcmp(tempbuf, "-acc")==0)       // Hardware Acceleration
	      {
	        //pCKP->Device.Display.Mode = VIDEO_MODE_SOFTWARE;
	      }
	      else if (strcmp(tempbuf, "-ogl")==0)       // Early OpenGL Support
	      {
	        //pCKP->Device.Display.Mode = VIDEO_MODE_OPENGL;
	      }
	      else if (strcmp(tempbuf, "-stereo")==0)       // Enable Stereo Sound
	      {
	    	  g_pSound->setSoundmode(0,true);
	      }
	      else if (strcmp(tempbuf, "-showfps")==0)  // show fps
	      {
	        g_pVideoDriver->showFPS(true);
	      }
	      else if (strcmp(tempbuf, "-host")==0)     // start network server
	      {
	        is_server = 1;
	        localmp = 0;
	      }
	      else if (strcmp(tempbuf, "-join")==0)     // connect to a server
	      {
	        is_client = 1;
	        localmp = 0;
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
void preallocateCKP(stCloneKeenPlus *pCKP)
{
	// This function prepares the CKP Structure so that the it is allocated in the memory.
	pCKP->numGames = 0;
	pCKP->Resources.GameSelected = 0;

	TileProperty = NULL;

	pCKP->GameData = NULL;
	pCKP->GameData = new stGameData[1];

	memset(pCKP->GameData, 0, sizeof(stGameData));

	framebyframe = 0;

	demomode = DEMO_NODEMO;
	current_demo = 1;

	memset(&pCKP->Control, 0, sizeof(stControl));

	pCKP->Joystick = NULL;

	acceleratemode = 0;
	is_client = 0;
	is_server = 0;
	localmp = 1;
	g_pVideoDriver->showFPS(false);

	player[0].x = player[0].y = 0;
}


