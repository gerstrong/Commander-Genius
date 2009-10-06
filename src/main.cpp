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
  Commander Keen game in order to be able to interpret that
  episode.

  Enjoy the Code
         - The Commander Genius Team

  CloneKeen 		2003-2005 Caitlin Shaw
  CloneKeenPlus 	2008-2009 Gerstrong
  Commander Genius 	2009	  Tulip, Pickle, DaVince and Albert
*/

#include "keen.h"
#include "sdl/CInput.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "hqp/CMusic.h"
#include "include/misc.h"
#include "include/CStartScreen.h"
#include "include/game.h"
#include "include/fileio.h"
#include "include/fileio/story.h"
#include "include/main.h"
#include "fileio/CParser.h"
#include "vorticon/CHighScores.h"
#include "CLogFile.h"
#include "CGame.h"
#include "sdl/CSettings.h"
#include "FindFile.h"
#include "fileio/CSavedGame.h"

int IntroCanceled;
int NessieObjectHandle;
int DemoObjectHandle;
int BlankSprite;
int DemoSprite;
int fps=0, curfps=0;

//stOption *options = NULL;

unsigned int demo_RLERunLen;
unsigned char demo_data[DEMO_MAX_SIZE+1];
unsigned int demo_data_index;

char QuitState = NO_QUIT;

stString strings[MAX_STRINGS+1];
int numStrings = 0;

FILE *demofile = NULL;

char ScreenIsScrolling;
int gunfiretimer, gunfirefreq;

char loadinggame, loadslot;

stMap map;
unsigned int AnimTileInUse[ATILEINUSE_SIZEX][ATILEINUSE_SIZEY];
stTile tiles[MAX_TILES+1];

unsigned int objdefsprites[NUM_OBJ_TYPES+1];

int thisplayer;
unsigned char primaryplayer;
unsigned char numplayers;

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

	/*		g_pLogFile->textOut(RED,"Parsing Command line flags.<br>");
		if(readCommandLine(argc, argv, &CKP) != 0)
		{
			g_pLogFile->textOut(RED,"Sorry, but CKP needs correct command line parameters.<br>");
			printf("Sorry, but CKP needs correct command line parameters.\n");
		}

	 * */

	CGame Game;
	////////////////////////////
	// Initialize Game Engine //
	////////////////////////////
	if(Game.init())
	{
		///////////////////////
		// Start Game Engine //
		///////////////////////
		Game.run();
	}

	/////////////////////////
	// Cleanup Game Engine //
	/////////////////////////
	Game.cleanup();

	/*g_pLogFile->textOut(RED,"Loading Games Menu.<br>");

	CStartMenu StartMenu = new CStartMenu;
	if(StartMenu->loadResources(&CKP, &Game))
	{

		delete StartMenu;
	}
	else
	{
		g_pLogFile->textOut(RED,"Error! Resources for start menu cannot be loaded! Maybe you need to copy the data files!<br>");
		delete StartMenu;
	}



	// Must clean up.

	if(!CKP.Control.skipstarting)
	{

		while(!loadStartMenu(&CKP))
			g_pLogFile->textOut(PURPLE,"Error! You have chosen a Game that doesn't exist. Please correct the \"games.cfg\" File under \"games\" and choose another game.<br>");
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
					g_pLogFile->textOut(PURPLE,"Error! You have chosen a Game that doesn't exist. Please correct the \"games.cfg\" File under \"games\" and choose another game.<br>");

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
	Settings.saveDrvCfg();*/
	
	printf("Thank you very much for playing this wonderful game!");

	return 0;
}

void cleanupResources(stCloneKeenPlus *pCKP)
{
	cleanup(pCKP);
	return;
}

// TODO: Those mmust go to a new class. Let's call it CDevices
bool loadCKPDrivers()
{
	// initialize/activate all drivers
	g_pLogFile->ftextOut("Starting graphics driver...<br>");

	// The graphics are very important, if the other subsystems fail, warn but continue
	if (!g_pVideoDriver->start()) return false;

	g_pLogFile->ftextOut("Starting sound driver...<br>");
	g_pSound->init();

	g_pLogFile->ftextOut("Starting the input driver...<br>");
	g_pInput->loadControlconfig();

	return true;
}

short abortCKP(stCloneKeenPlus *pCKP)
{
	  g_pLogFile->ftextOut("Fatal error, cleaning up.<br>");
	  cleanup(pCKP);
	  g_pLogFile->ftextOut("A fatal error has occurred; game is shutting down.<br>");
	  return 1;
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
//char SaveOptions[NUM_OPTIONS];
	stLevelControl *p_levelcontrol;
	//stOption *p_option;

	p_levelcontrol = &(pCKP->Control.levelcontrol);
	//p_option = pCKP->Option;

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
   pCKP->Control.levelcontrol.demomode = DEMO_PLAYBACK;
   loadinggame = 0;
   p_levelcontrol->curlevel = lvl;
   p_levelcontrol->command = LVLC_NOCOMMAND;

   initgamefirsttime(pCKP, s);
   initgame( &(pCKP->Control.levelcontrol) );

   // now load the map and play the level
   sprintf(filename, "level%02d.ck%d", p_levelcontrol->curlevel, p_levelcontrol->episode);
   if ( loadmap(filename, pCKP->Resources.GameDataDirectory,
		   p_levelcontrol->curlevel, &(pCKP->Control.levelcontrol)) ) return DEMO_RESULT_FILE_BAD;

   //for(i=0;i<NUM_OPTIONS;i++) SaveOptions[i] = p_option[i].value;
   // SetDefaultOptions();
   gameloop(pCKP);
   //for(i=0;i<NUM_OPTIONS;i++) p_option[i].value = SaveOptions[i];

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
	    	  //pCKP->Option[OPT_ALLOWPKING].value = 0;
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
	    	  //pCKP->Option[OPT_CHEATS].value = 1;
	      }
	      else if (strcmp(tempbuf, "-rec")==0)      // record a demo
	      {
	    	  pCKP->Control.levelcontrol.demomode = DEMO_RECORD;
	      }
	      else if (strcmp(tempbuf, "-eseq")==0)     // play end sequence
	      {
	        pCKP->Control.eseq = 1;
	      }

	      else if (strcmp(tempbuf, "-story")==0)     // play end sequence
	      {
	        //pCKP->Control.storyboard = 1;
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
	    	  g_pSound->setSoundmode(0,true, AUDIO_U8);
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
