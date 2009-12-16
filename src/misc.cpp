/* MISC.C
 All KINDS of assorted crap :) Has most of the in-game dialog boxes
 such as the status box etc.

 Also like I said there's all kinds of assorted crap in here.
 That's why it's called "misc.c" (get it? :))
 */

#include "keen.h"
#include <SDL.h>
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CSound.h"

#include "include/misc.h"
#include "game.h"
#include "fileio/CSavedGame.h"
#include "CLogFile.h"
#include "graphics/CGfxEngine.h"
#include "StringUtils.h"
#include "FindFile.h"

#include "fileio/CExeFile.h"

#include "dialog/CTextViewer.h"

#include <fstream>

void banner(void)
{
	char buf[80];

	sprintf(buf, "%s  *Unknown* build check banner()", REVISION);
#ifdef TARGET_WIN32
    sprintf(buf, "%s  Windows build", REVISION);
#endif
#ifdef TARGET_LNX
    sprintf(buf, "%s  Linux build", REVISION);
#endif
	printf("%s", buf);
	printf(" (%d bit)", static_cast<int> (sizeof(int*)*8));

	printf("\nby The CloneKeenPlus Team 2009\n");
	printf("\n");
	printf("BY A FAN, FOR FANS. ALL \"COMMANDER KEEN\" GRAPHICS,\n");
	printf("SOUND, AND LEVEL FILES ARE THE PROPERTY OF ID SOFTWARE.\n");
	printf("\n");
}

void inventory_draw_ep2(int p, bool *levels_completed)
{
	/*int x,i,j;
	 std::string tempbuf;
	 int dlgX,dlgY,dlgW,dlgH;

	 SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

	 dlgX = GetStringAttribute("EP2_StatusBox", "LEFT");
	 dlgY = GetStringAttribute("EP2_StatusBox", "TOP");
	 dlgW = GetStringAttribute("EP2_StatusBox", "WIDTH");
	 dlgH = GetStringAttribute("EP2_StatusBox", "HEIGHT");

	 g_pGfxEngine->drawDialogBox( boxsurface, dlgX,dlgY,dlgW,dlgH);
	 g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);

	 // cards
	 if (player[p].inventory.HasCardYellow)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+21)<<3)-4, ((dlgY+8)<<3)+3, 424);

	 if(player[p].inventory.HasCardYellow > 1)
	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardYellow),(dlgX+20)<<3,((dlgY+8)<<3)+3);

	 }
	 if (player[p].inventory.HasCardRed)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+25)<<3)-4, ((dlgY+8)<<3)+3, 425);

	 if(player[p].inventory.HasCardRed > 1)
	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardRed),(dlgX+24)<<3,((dlgY+8)<<3)+3,0);

	 }
	 if (player[p].inventory.HasCardGreen)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+21)<<3)-4, ((dlgY+10)<<3)+4, 426);

	 if(player[p].inventory.HasCardGreen > 1)
	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardGreen),(dlgX+20)<<3,((dlgY+10)<<3)+3,0);
	 }
	 if (player[p].inventory.HasCardBlue)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+25)<<3)-4, ((dlgY+10)<<3)+4, 427);

	 if(player[p].inventory.HasCardBlue > 1)
	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardBlue),(dlgX+24)<<3,((dlgY+10)<<3)+3,0);

	 }
	 // cities saved
	 if (levels_completed[4]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL4_TargetName"), (dlgX+1)<<3, (dlgY+8)<<3, 0);
	 if (levels_completed[6]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL6_TargetName"), (dlgX+8)<<3, (dlgY+8)<<3, 0);
	 if (levels_completed[7]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL7_TargetName"), (dlgX+1)<<3, (dlgY+9)<<3, 0);
	 if (levels_completed[13]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL13_TargetName"), (dlgX+8)<<3, (dlgY+9)<<3, 0);
	 if (levels_completed[11]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL11_TargetName"), (dlgX+1)<<3, (dlgY+10)<<3, 0);
	 if (levels_completed[9]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL9_TargetName"), (dlgX+8)<<3, (dlgY+10)<<3, 0);
	 if (levels_completed[15]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL15_TargetName"), (dlgX+1)<<3, (dlgY+11)<<3, 0);
	 if (levels_completed[16]) g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP2_LVL16_TargetName"), (dlgX+8)<<3, (dlgY+11)<<3, 0);

	 // raygun icon
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+20)<<3, ((dlgY+5)<<3)-5, 414);

	 // ray gun charges text
	 i = player[p].inventory.charges;
	 if (i>999) i=999;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+27-tempbuf.size())<<3, ((dlgY+5)<<3)-1, 0);

	 // score
	 i = player[p].inventory.score;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3, 0);

	 // extra life at
	 i = player[p].inventory.extralifeat;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3, 0);

	 // lives
	 i = player[p].inventory.lives;
	 x = ((dlgX + 1)<<3)+4;
	 if (i>7) i=7;
	 for(j=0;j<i;j++)
	 {
	 g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
	 x += g_pGfxEngine->Sprite[0]->getWidth();
	 }
	 */
}

void inventory_draw_ep3(int p)
{
	//int x,y,t,i,j;
	/*int i,j,x;
	 int ankhtimepercent;
	 std::string tempbuf;
	 int dlgX,dlgY,dlgW,dlgH;

	 SDL_Surface *boxsurface = g_pVideoDriver->FGLayerSurface;

	 dlgX = GetStringAttribute("EP3_StatusBox", "LEFT");
	 dlgY = GetStringAttribute("EP3_StatusBox", "TOP");
	 dlgW = GetStringAttribute("EP3_StatusBox", "WIDTH");
	 dlgH = GetStringAttribute("EP3_StatusBox", "HEIGHT");

	 g_pGfxEngine->drawDialogBox( boxsurface, dlgX,dlgY,dlgW,dlgH);
	 g_pGfxEngine->Font->drawFont( boxsurface, getstring("EP3_StatusBox"), (dlgX+1)<<3, (dlgY+1)<<3, 0);

	 // calculate % ankh time left
	 ankhtimepercent = (int)((float)player[p].ankhtime / (PLAY_ANKH_TIME/100));
	 // ankh time
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+4)<<3, ((dlgY+8)<<3)+3, 214);

	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(ankhtimepercent), (dlgX+8)<<3, ((dlgY+8)<<3)+7, 0);

	 // raygun icon
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, (dlgX+23)<<3, ((dlgY+5)<<3)-5, 216);

	 // ray gun charges text
	 i = player[p].inventory.charges;
	 if (i>999) i=999;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont( boxsurface, tempbuf, (dlgX+26)<<3, ((dlgY+5)<<3)-1, 0);

	 // cards
	 if (player[p].inventory.HasCardYellow)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+14)<<3)+4, ((dlgY+8)<<3)+4, 217);

	 if(player[p].inventory.HasCardYellow > 1)
	 g_pGfxEngine->Font->drawFont( boxsurface, itoa(player[p].inventory.HasCardYellow),(dlgX+13)<<3,((dlgY+8)<<3)+3,0);

	 }
	 if (player[p].inventory.HasCardRed)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+18)<<3)+4, ((dlgY+8)<<3)+4, 218);

	 if(player[p].inventory.HasCardRed > 1)
	 g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardRed),(dlgX+17)<<3,((dlgY+8)<<3)+3,0);

	 }
	 if (player[p].inventory.HasCardGreen)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+22)<<3)+4, ((dlgY+8)<<3)+4, 219);

	 if(player[p].inventory.HasCardGreen > 1)
	 g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardGreen),(dlgX+21)<<3,((dlgY+8)<<3)+3,0);

	 }
	 if (player[p].inventory.HasCardBlue)
	 {
	 g_pGfxEngine->Tilemap->drawTile(boxsurface, ((dlgX+26)<<3)+4, ((dlgY+8)<<3)+4, 220);

	 if(player[p].inventory.HasCardBlue > 1)
	 g_pGfxEngine->Font->drawFont(boxsurface, itoa(player[p].inventory.HasCardBlue),(dlgX+25)<<3,((dlgY+8)<<3)+3,0);

	 }

	 // score
	 i = player[p].inventory.score;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont(boxsurface, tempbuf, (dlgX+12-tempbuf.size())<<3, (dlgY+2)<<3);
	 // extra life at
	 i = player[p].inventory.extralifeat;
	 tempbuf = itoa(i);
	 g_pGfxEngine->Font->drawFont(boxsurface, tempbuf, (dlgX+28-tempbuf.size())<<3, (dlgY+2)<<3);
	 // lives
	 i = player[p].inventory.lives;
	 x = ((dlgX+1)<<3)+4;
	 if (i>9) i=9;
	 for(j=0;j<i;j++)
	 {
	 g_pGfxEngine->Sprite[playerbaseframes[p]]->drawSprite( g_pVideoDriver->FGLayerSurface, x, (dlgY+4)<<3);
	 x += g_pGfxEngine->Sprite[0]->getWidth();
	 }*/
}

void AllPlayersInvisible(void)
{
	/*int i;

	 for(i=0;i<MAX_PLAYERS;i++)
	 {
	 if (player[i].isPlaying)
	 {
	 objects[player[i].useObject].onscreen = 0;
	 player[i].hideplayer = 1;
	 }
	 }*/
}

char gameiswon(stCloneKeenPlus *pCKP)
{
	/*int partcount;
	 int i;
	 stLevelControl *p_levelcontrol;
	 p_levelcontrol = &(pCKP->Control.levelcontrol);

	 if (p_levelcontrol->episode==1)
	 {
	 // episode 1: game is won when all parts are collected

	 // count the number of parts the players have acquired
	 partcount = 0;
	 for(i=0;i<MAX_PLAYERS;i++)
	 {
	 if (player[i].isPlaying)
	 {
	 if (player[i].inventory.HasJoystick) partcount++;
	 if (player[i].inventory.HasBattery) partcount++;
	 if (player[i].inventory.HasWiskey) partcount++;
	 if (player[i].inventory.HasVacuum) partcount++;
	 }
	 }

	 // go to end sequence if all the parts have been got
	 if (partcount >= 4)
	 {
	 return 1;
	 }
	 else return 0;
	 }
	 else if (p_levelcontrol->episode==2)
	 {
	 // episode 2: game is won when all cities are saved
	 if (!p_levelcontrol->levels_completed[4]) return 0;
	 if (!p_levelcontrol->levels_completed[6]) return 0;
	 if (!p_levelcontrol->levels_completed[7]) return 0;
	 if (!p_levelcontrol->levels_completed[13]) return 0;
	 if (!p_levelcontrol->levels_completed[11]) return 0;
	 if (!p_levelcontrol->levels_completed[9]) return 0;
	 if (!p_levelcontrol->levels_completed[15]) return 0;
	 if (!p_levelcontrol->levels_completed[16]) return 0;
	 return 1;
	 }
	 else if (p_levelcontrol->episode==3)
	 {
	 // episode 3: game is won when mortimer is defeated
	 if (p_levelcontrol->levels_completed[16])
	 {
	 return 1;
	 }
	 else
	 {
	 return 0;
	 }
	 }*/

	return 0;
}

void usage(void)
{
	// TODO: Many options has to be changed. For now it is disabled
	/*
	 printf("Usage: keen [lvlnum] [-*player] [-nopk] [-ep*] [-dtm] [-nocheat] [-rec] -[eseq]<br>\n");
	 printf("lvlnum          specify a level number (such as 2) to go directly to that level<br>");
	 printf("-*player        select number of players (1-4); defaults to 1<br>");
	 printf("-nopk           do not allow players to kill each other in multiplayer games<br>");
	 printf("-game*            select game of data base; if not given, start menu is opened<br>");
	 printf("-dtm            go directly to the world map, bypassing intro and title screen<br>");
	 printf("-mean           increase game difficulty<br>");
	 printf("-cheat          enable function key cheat/debug codes<br>");
	 printf("-rec            record player actions to demo.dat for making a demo<br>");
	 printf("-eseq           for the impatient--cut directly to the ending sequence<br>");
	 #ifdef BUILD_SDL
	 printf("-fs             use fullscreen mode<br>");
	 printf("-dbl            zoom image 2x<br>");
	 printf("-ogl            hardware acceleration<br>");
	 printf("-showfps        show FPS in upper-right of screen<br>");
	 #endif

	 printf("<br>");
	 printf("Examples:<br>");
	 printf("  keen 3 -ep2                play ep 2, level 3 in 1-player mode<br>");
	 printf("  keen -ep3 -dtm -2player    play ep3, skip title&intro, 2-player mode<br>");
	 printf("  keen -ep3                  play a normal game of ep3<br>");
	 */
}

unsigned int rnd(void)
{
	//random_seed = random_seed * 1103515245 + 12345;
	//return (uint)(random_seed / 65536) % 32768;
	return rand();
}

// This function shows up the helping text when F1 is pushed
void showF1HelpText(int episode, std::string DataDirectory)
{
/*	std::string helptext;

	if(episode==1)
	{
		// We suppose that we are using version 131. Maybe it must be extended
		std::string filename = DataDirectory;
		if(DataDirectory != "")
			filename += "/";

		filename += "helptext.ck1";

		std::ifstream File; OpenGameFileR(File, filename, std::ios::binary);

		if(!File) return;

		while(!File.eof())
			helptext.push_back(File.get());

		File.close();
	}
	else
	{
		// Here the Text file is within the EXE-File
		unsigned long startflag=0, endflag=0;
		unsigned char *text_data = NULL;

		CExeFile *ExeFile = new CExeFile(episode, DataDirectory);
		ExeFile->readData();

		if(!ExeFile->getData()) return;

		if(episode == 2)
		{
			startflag = 0x15DC0-512;
			endflag = 0x1659F-512;
		}
		else // Episode 3
		{
			startflag = 0x17BD0-512;
			endflag = 0x1839F-512;
		}

		text_data = ExeFile->getData();

		for(unsigned long i=startflag ; i<endflag ; i++ )
			helptext.push_back(text_data[i]);

		delete ExeFile;
	}

	helptext += "\n\nNOTE: The controls in CKP may be different than in the help described.\nCheck out the Readme.txt!\n";

	CTextViewer *Textviewer = new CTextViewer(g_pVideoDriver->FGLayerSurface, 0, 0, 320, 140);

	Textviewer->loadText(helptext);

	Textviewer->process();

	delete Textviewer;*/
}

