/* FILEIO.C
  Functions responsible for loading data from files, such as the one that
  decodes the level map files (loadmap()) and the one that loads in the
  tile attribute data contained in ep?attr.dat (loadtileattributes()).
  The functions for loading the graphics (EGALATCH&EGASPRIT) are in latch.c.
*/

#include "keen.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CSound.h"
#include "hqp/CMusic.h"
#include "include/fileio.h"
#include "include/fileio/rle.h"
#include "vorticon/CPlayer.h"
#include "CLogFile.h"
#include "CGraphics.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

extern CPlayer *Player;

unsigned int curmapx, curmapy;
unsigned char mapdone;
void addmaptile(unsigned int t)
{
  map.mapdata[curmapx][curmapy] = t;
  curmapx++;
  if (curmapx >= map.xsize)
  {
    curmapx = 0;
    curmapy++;

    if (curmapy >= map.ysize) mapdone = 1;
  }
}

bool renameFilenamesLowerCase(const char *dir_name)
{
#ifdef TARGET_WIN32
	return true;
#else
	DIR *p_Dir;
	char newname[256];
	char buf[256];
	struct dirent *dp;

	memset(newname,0,256);
	strcpy(buf,dir_name);

	if(buf[0] == '\0')
		strcpy(buf,"./");

	chdir("data");

	if((p_Dir = opendir(buf))==NULL)
		return false;

	bool retval = true;
	// This function checks if all the files in the directory are lower case.
	// If they aren't rename them
	while((dp = readdir(p_Dir)) != NULL)
	{
        if(dp->d_type == DT_REG)
        {
        	strcpy(newname,dp->d_name);

        	int len = strlen(newname);
        	for(int pos=0 ; pos < len ; pos++ )
        		if(newname[pos] >= 'A' && newname[pos] <= 'Z')
        			newname[pos] += ('a'-'A');

        	if(strncmp(newname,dp->d_name,strlen(dp->d_name)) != 0)
        		if(rename(dp->d_name,newname) != -1)
        			retval &= true;
        }
    }

	closedir(p_Dir);

	chdir("../");

	return retval;

#endif
}

short checkConsistencyofGameData(stGameData *p_GameData)
{
	short ok = 0;

	FILE *fp;

	// Let's define which files need to be read!
	memset(p_GameData->FileList,0,MAX_NUMBER_OF_FILES*MAX_STRING_LENGTH);
	if(p_GameData->Episode >= 1 && p_GameData->Episode <= 3)
	{
		sprintf(p_GameData->FileList[1],"keen%d.exe",p_GameData->Episode);
		sprintf(p_GameData->FileList[2],"egahead.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[3],"egasprit.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[4],"egalatch.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[5],"finale.ck%d",p_GameData->Episode);

		// Levels!
		sprintf(p_GameData->FileList[6],"level01.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[7],"level02.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[8],"level03.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[9],"level04.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[10],"level05.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[11],"level06.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[12],"level07.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[13],"level08.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[14],"level09.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[15],"level10.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[16],"level11.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[17],"level12.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[18],"level13.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[19],"level14.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[20],"level15.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[21],"level16.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[22],"level80.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[23],"level81.ck%d",p_GameData->Episode);
		sprintf(p_GameData->FileList[24],"level90.ck%d",p_GameData->Episode);

		// Other
		if(p_GameData->Episode == 1)
			sprintf(p_GameData->FileList[24],"sounds.ck%d",p_GameData->Episode);
	}

	// Rename all the the files in the directory to lower case
	if(!renameFilenamesLowerCase(p_GameData->DataDirectory))
	{
		g_pLogFile->ftextOut(PURPLE,"WARNING: There was an error while trying to format correctly the game data.");
		g_pLogFile->ftextOut(PURPLE,"Please check, if you have write permissions on the game data directory and it is accessible<br>");
	}

	// Finally check if they really exist!
	char buf[MAX_STRING_LENGTH];
	int c=0;
	for(c = 0 ; c < MAX_NUMBER_OF_FILES ; c++)
	{
		if(p_GameData->FileList[c][0] == 0) // If there are no more files!
			break;

		memset(buf,0,MAX_STRING_LENGTH*sizeof(char));
		strcat(buf,"data/");
		strcat(buf,p_GameData->DataDirectory);
		if(*(p_GameData->DataDirectory) != 0)
			strcat(buf,"/");
		strcat(buf,p_GameData->FileList[c]);

		if((fp = fopen(buf,"r")) != NULL)
		{
			fclose(fp);
		}
		else
		{
			g_pLogFile->ftextOut("Error! File \"%s\" was not found! Please add it to the configured directory.<br>", p_GameData->FileList[c]);
			p_GameData->Episode = -1;
			ok++;
		}
	}

	if(ok==0)
		g_pLogFile->ftextOut("Game data of Episode %d is complete.<br>", p_GameData->Episode);

	return ok;
}

char NessieAlreadySpawned;
void addobjectlayertile(unsigned int t, stCloneKeenPlus *pCKP)
{
int o;
  switch(t)
  {
   case 0: break;       // blank
   case 255:            // player start

	 //Player[0].setCoord(curmapx << 4 << CSF, curmapy << 4 << CSF);

	 player[0].x = curmapx << 4 << CSF;
     player[0].y = curmapy << 4 << CSF;
     map.objectlayer[curmapx][curmapy] = 0;
     break;
   case NESSIE_PATH:          // spawn nessie at first occurance of her path
     if (pCKP->Control.levelcontrol.episode==3)
     {
       if (!NessieAlreadySpawned)
       {
         o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_NESSIE);
         objects[o].hasbeenonscreen = 1;
         NessieAlreadySpawned = 1;
         NessieObjectHandle = o;
       }
     }
     goto levelmarker;
   break;
   default:             // level marker
levelmarker: ;
     if ((t&0x7fff) < 256 && pCKP->Control.levelcontrol.levels_completed[t&0x00ff])
     {
    	 if(!options[OPT_LVLREPLAYABILITY].value)
    		map.objectlayer[curmapx][curmapy] = 0;
    	 else
    		 map.objectlayer[curmapx][curmapy] = t;

    	 int newtile = tiles[map.mapdata[curmapx][curmapy]].chgtile;

    	 // Consistency check! Some Mods have issues with that.
    	 if(pCKP->Control.levelcontrol.episode == 1 || pCKP->Control.levelcontrol.episode == 2)
    	 {
    		 if(newtile<77 || newtile>81)
    			 // something went wrong. Use default tile
    			 newtile = 77;

    		 // try to guess, if it is a 32x32 (4 16x16) Tile
    		 if(map.mapdata[curmapx-1][curmapy-1] == (unsigned int) newtile &&
    				 map.mapdata[curmapx][curmapy-1] == (unsigned int) newtile  &&
    				 map.mapdata[curmapx-1][curmapy] == (unsigned int) newtile)
    		 {
    			 map.mapdata[curmapx-1][curmapy-1] = 78; //ul
    			 map.mapdata[curmapx][curmapy-1] = 79; //ur
    			 map.mapdata[curmapx-1][curmapy] = 80; //bl
    			 newtile = 81; // br. this one
    		 }
    	 }
    	 else if(pCKP->Control.levelcontrol.episode == 3)
    	 {
    		 if(newtile<52 || newtile>56)
    			 // something went wrong. Use default tile
    			 newtile = 56;
    		 // try to guess, if it is a 32x32 (4 16x16) Tile
    		 if(map.mapdata[curmapx-1][curmapy-1] == (unsigned int) newtile &&
    				 map.mapdata[curmapx][curmapy-1] == (unsigned int) newtile  &&
    				 map.mapdata[curmapx-1][curmapy] == (unsigned int) newtile)
    		 {
    			 map.mapdata[curmapx-1][curmapy-1] = 52; //bl
    			 map.mapdata[curmapx][curmapy-1] = 53; //ur
    			 map.mapdata[curmapx-1][curmapy] = 54; //ul
    			 newtile = 55; // br. this one
    		 }
    	 }

 		map.mapdata[curmapx][curmapy] = newtile;

     }
     else
     {
       map.objectlayer[curmapx][curmapy] = t;
     }
     break;
  }

  curmapx++;
  if (curmapx >= map.xsize)
  {
    curmapx = 0;
    curmapy++;
    if (curmapy >= map.ysize) mapdone = 1;
  }
}

void addenemytile(unsigned int t, stCloneKeenPlus *pCKP)
{
int o,x;
  map.objectlayer[curmapx][curmapy] = t;

  if (t)
  {
    if (t==255)
    {
        if(curmapx >= map.xsize-2) // Edge bug. Keen would fall in some levels without this.
      	   curmapx = 4;

        if(curmapy >= map.ysize-2) // Edge bug. Keen would fall in some levels without this.
      	   curmapx = 4;

       //Player[0].setCoord(curmapx << 4 << CSF, ((curmapy << 4) + 8) << CSF);

       player[0].x = curmapx << 4 << CSF;
       player[0].y = ((curmapy << 4) + 8) << CSF;
    }
    else
    {
      switch(t)
      {
      case 0: break;
      case -1: break;
      case 1:  // yorp (ep1) vort (ep2&3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
              x = curmapx;

              if (TileProperty[map.mapdata[x][curmapy+1]][BLEFT]) x--;
              spawn_object(x<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_YORP);
           }
           else
           {
              // in ep2 level 16 there a vorticon embedded in the floor for
              // some reason! that's what the if() is for--to fix it.
        	  if (TileProperty[map.mapdata[curmapx][curmapy+1]][BLEFT])
              {
                spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-16)<<CSF, OBJ_VORT);
              }
              else
              {
                spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORT);
              }
           }
           break;
      case 2:    // garg (ep1) baby vorticon (ep2&3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
             // those bastards. sometimes embedding garg's in the floor in
             // the original maps.
        	 if(TileProperty[map.mapdata[curmapx+1][curmapy+1]][BLEFT])
             {
               if (pCKP->Control.levelcontrol.chglevelto==7)
               {
                 spawn_object(curmapx<<4<<CSF, (curmapy-1)<<4<<CSF, OBJ_GARG);
               }
               else
               {
                 spawn_object((curmapx-1)<<4<<CSF, (curmapy)<<4<<CSF, OBJ_GARG);
               }
             }
             else
             {
               spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_GARG);
             }
           }
           else
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BABY);
           }
           break;
      case 3:    // vorticon (ep1) bear (ep2)
           if (pCKP->Control.levelcontrol.episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORT);
           }
           else if (pCKP->Control.levelcontrol.episode==2)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BEAR);
           }
           else if (pCKP->Control.levelcontrol.episode==3)
           {
              spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_MOTHER);
           }
           break;
      case 4:    // butler (ep1) OR walker (ep2) OR meep (ep3)
           if (pCKP->Control.levelcontrol.episode==1)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BUTLER);
           else if (pCKP->Control.levelcontrol.episode==2)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_WALKER);
           else if (pCKP->Control.levelcontrol.episode==3)
             spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_MEEP);
           break;
      case 5:    // tank robot (ep1&2) karate bear (ep3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_TANK);
              // set tank robot guarding bonus level to be active at startup
              if (pCKP->Control.levelcontrol.chglevelto==13)
              {
                objects[o].hasbeenonscreen = 1;
              }
           }
           else if (pCKP->Control.levelcontrol.episode==2)
              spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+0)<<CSF, OBJ_TANKEP2);
           else if (pCKP->Control.levelcontrol.episode==3)
           {
        	  if(TileProperty[map.mapdata[curmapx][curmapy+1]][BLEFT])
              {
                spawn_object(curmapx<<4<<CSF, (curmapy-1)<<4<<CSF, OBJ_NINJA);
              }
              else
              {
                spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_NINJA);
              }
           }
           break;
      case 6:    // up-right-flying ice chunk (ep1) horiz platform (ep2)
                 // foob (ep3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
              o = spawn_object((((curmapx+1)<<4)+4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_SECTOREFFECTOR);
              objects[o].ai.se.type = SE_ICECANNON;
              objects[o].ai.se.dir = DUPRIGHT;
              objects[o].inhibitfall = 1;
              objects[o].hasbeenonscreen = 1;
           }
           else if (pCKP->Control.levelcontrol.episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-3)<<CSF, OBJ_PLATFORM);
           }
           else if (pCKP->Control.levelcontrol.episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_FOOB);
           }
           break;
      case 7:   // spark (ep2) ball (ep3)
           if (pCKP->Control.levelcontrol.episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_SECTOREFFECTOR);
              objects[o].ai.se.type = SE_SPARK;
              pCKP->Control.levelcontrol.canexit = 0;    // can't exit till spark is shot
           }
           else
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_BALL);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 8:    // jack (ep3)
           if (pCKP->Control.levelcontrol.episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_JACK);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 9:    // up-left-flying ice chunk (ep1) horiz platform (ep3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
             o = spawn_object(((curmapx<<4)-4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_SECTOREFFECTOR);
//             objects[o].ai.icechunk.movedir = DUPLEFT;
              objects[o].ai.se.type = SE_ICECANNON;
              objects[o].ai.se.dir = DUPLEFT;
             objects[o].inhibitfall = 1;
             objects[o].hasbeenonscreen = 1;
           }
           else if (pCKP->Control.levelcontrol.episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, (((curmapy)<<4)-4)<<CSF, OBJ_PLATFORM);
           }
           break;
      case 10:   // rope holding the stone above the final vorticon (ep1)
                 // vert platform (ep3)
           if (pCKP->Control.levelcontrol.episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_ROPE);
           }
           else if (pCKP->Control.levelcontrol.episode==3)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_PLATVERT);
           }
           break;
      case 11:   // jumping vorticon (ep3)
           if (pCKP->Control.levelcontrol.episode==3)
           {
             spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-8)<<CSF, OBJ_VORT);
           }
           break;
      case 12:   // sparks in mortimer's machine
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_SPARK;
          objects[o].hasbeenonscreen = 1;
          break;
      case 13:   // mortimer's heart
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_HEART;
          objects[o].hasbeenonscreen = 1;
          break;
      case 14:   // right-pointing raygun (ep3)
           if (pCKP->Control.levelcontrol.episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
             objects[o].ai.se.type = SE_GUN_RIGHT;
             objects[o].hasbeenonscreen = 1;
           }
           break;
      case 15:   // vertical raygun (ep3)
           if (pCKP->Control.levelcontrol.episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
             objects[o].ai.se.type = SE_GUN_VERT;
             objects[o].hasbeenonscreen = 1;
           }
           break;
      case 16:  // mortimer's arms
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_ARM;
          objects[o].hasbeenonscreen = 1;
      break;
      case 17:  // mortimer's left leg
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_LEG_LEFT;
          objects[o].hasbeenonscreen = 1;
      break;
      case 18:  // mortimer's right leg
          o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SECTOREFFECTOR);
          objects[o].ai.se.type = SE_MORTIMER_LEG_RIGHT;
          objects[o].hasbeenonscreen = 1;
      break;
      default:
    	   g_pGraphics->fadePalette(20);
           g_pLogFile->ftextOut(PURPLE,"unknown enemy type %d at (%d,%d)<br>", t, curmapx, curmapy);
           //while(g_pInput->getPressedCommand(KENTER));

           break;
      }
    }
  }
  curmapx++;
  if (curmapx >= map.xsize)
  {
    curmapx = 0;
    curmapy++;
    if (curmapy >= map.ysize) mapdone = 1;
  }
}

unsigned int fgeti(FILE *fp) {
unsigned int temp1, temp2;
  temp1 = fgetc(fp);
  temp2 = fgetc(fp);
  return (temp2<<8) | temp1;
}

unsigned long fgetl(FILE *fp) {
unsigned int temp1, temp2, temp3, temp4;
  temp1 = fgetc(fp);
  temp2 = fgetc(fp);
  temp3 = fgetc(fp);
  temp4 = fgetc(fp);
  return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

unsigned int loadmap(char filename[MAX_STRING_LENGTH], char *path, int lvlnum, int isworldmap, stCloneKeenPlus *pCKP)
{
FILE *fp;
char fname[256];
int t;
unsigned int c;
int numruns = 0;
int gottenazero;
int resetcnt, resetpt;

  NessieAlreadySpawned = 0;
  map.isworldmap = isworldmap;

  char buffer[256];

  formatPathString(buffer, path);

  sprintf(fname, "%s%s", buffer, filename);
  fp = fopen(fname, "rb");
  if (!fp)
  {
    // only record this error message on build platforms that log errors
    // to a file and not to the screen.
	  g_pLogFile->ftextOut("loadmap(): unable to open file %s<br>", fname);
    return 1;
  }
  g_pLogFile->ftextOut("loadmap(): file %s opened. Loading...<br>", fname);

    // decompress map RLEW data
  curmapx = curmapy = mapdone = 0;

 unsigned int *filebuf; // big File Buffer for the uncompression
 filebuf = (unsigned int*) malloc(500000*sizeof(int));

 if(filebuf == NULL)
 {
	 g_pLogFile->ftextOut("loadmap(): unable to allocate memory for unpacking the level file<br>", fname);
	    return 1;
 }

  int finsize; 		   // Final size

  finsize = unRLEW(fp, filebuf);

  c=0;

  if(finsize == -1)
  {
	  rewind(fp);
	  while(!feof(fp))
	  {
		  filebuf[c] = fgeti(fp);
		  c++;
	  }
  }


  map.xsize = filebuf[2];
  map.ysize = filebuf[3];

  if (map.xsize > 260 || map.ysize > 260)
  {
    g_pLogFile->textOut(RED,"loadmap(): level is too big<br>");
    return 1;
  }

  c=18;

  while(c < ((filebuf[9] / 2)+18)) // Check against Tilesize
  {
	  t = filebuf[c];
	  if(!mapdone)
		  addmaptile(t);

	  if(t > 255)
	  {
		  t=0; // If there are some invalid values in the file
	  }

      c++;
  }


  // now do the enemies
    gottenazero = 0;

  // get enemy/objectlayer data
    curmapx = curmapy = mapdone = numruns = 0;
    resetcnt = resetpt = 0;

    while(!mapdone)
        {
          t = filebuf[c];

          if (t==0 && !gottenazero)
          {
             curmapx = curmapy = 0;
             gottenazero = 1;
          }
          if (map.isworldmap) addobjectlayertile(t, pCKP); else addenemytile(t, pCKP);
          if (++resetcnt==resetpt) curmapx=curmapy=0;

          c++;
        }

    free(filebuf);

    fclose(fp);

    // HQ Sounds. Load Music for a level if you have HQP
    g_pMusicPlayer->stop();

    if((fp=fopen("data/hqp/music/table.cfg","rt")) != NULL)
    {
    	char buf1[MAX_STRING_LENGTH];
    	char buf2[MAX_STRING_LENGTH];

    	memset(buf1,0,sizeof(char)*MAX_STRING_LENGTH);
    	memset(buf2,0,sizeof(char)*MAX_STRING_LENGTH);

    	while(!feof(fp))
    	{
    		fscanf(fp,"%s",buf1);

    		if(strcmp(buf1,filename) == 0)
    		{
    			fscanf(fp,"%s",buf2);
    			break;
    		}
    		else
    			fgets(buf1,MAX_STRING_LENGTH,fp);
    	}


    	fclose(fp);

    	if(*buf2 != 0)
    	{
    		strcpy(buf1,"data/hqp/music/");
    		strcat(buf1,buf2);
    		g_pMusicPlayer->load(g_pSound->getAudioSpec(),buf1);
    		g_pMusicPlayer->play();
     	}
    }

    // Didn't it work? Don't matter. HQP is optional, so continue

 // install enemy stoppoints as needed
 if (pCKP->Control.levelcontrol.episode==1 && lvlnum==13)
 {
    map.objectlayer[94][13] = GARG_STOPPOINT;
    map.objectlayer[113][13] = GARG_STOPPOINT;
    map.objectlayer[48][6] = GARG_STOPPOINT;
    map.objectlayer[80][5] = GARG_STOPPOINT;
    map.objectlayer[87][5] = GARG_STOPPOINT;
    map.objectlayer[39][18] = GARG_STOPPOINT;
 }
 else if (pCKP->Control.levelcontrol.episode==3 && lvlnum==6)
 {
    map.objectlayer[40][7] = BALL_NOPASSPOINT;
    map.objectlayer[50][7] = BALL_NOPASSPOINT;
 }
 else if (pCKP->Control.levelcontrol.episode==3 && lvlnum==9)
 {
    map.objectlayer[45][106] = BALL_NOPASSPOINT;

 }
 else if (pCKP->Control.levelcontrol.episode==3 && lvlnum==4)
 {
    map.objectlayer[94][17] = BALL_NOPASSPOINT;
 }

  return 0;
}

char loadstrings_AddAttr(char *attr, int stringIndex)
{
char stAttrName[80];
char stAttrValue[80];
int attrvalue;
int RAMAllocSize;
char *copyPtr;
unsigned int i;

  // if the attribute does not have an equals sign bail
  if (!strstr(attr, "="))
  {
	  g_pLogFile->ftextOut("loadstrings_AddAttr(): '%s' is not a valid attribute definition.<br>", attr);
    return 1;
  }

  // split the attribute up into it's name and it's value
  copyPtr = stAttrName;
  for(i=0;i<strlen(attr);i++)
  {
    if (attr[i] != ' ' && attr[i]!=9)      // strip out spaces and tabs
    {
      if (attr[i] != '=')
      {
        *copyPtr = attr[i];
        copyPtr++;
      }
      else
      { // hit the equals sign
        *copyPtr = 0;
        copyPtr = stAttrValue;
      }
    }
  }
  *copyPtr = 0;

  attrvalue = atoi(stAttrValue);

  // malloc space for the attribute name
  RAMAllocSize = strlen(stAttrName) + 1;
  strings[stringIndex].attrnames[strings[stringIndex].numAttributes] = (unsigned char*) malloc(RAMAllocSize+1);
  if (!strings[stringIndex].attrnames[strings[stringIndex].numAttributes])
  {
    g_pLogFile->textOut(RED,"loadstrings_AddAttr(): Unable to allocate space for attribute name ('%s').<br>", stAttrName);
    return 1;
  }

  // copy the data into the strings structure
  memcpy(strings[stringIndex].attrnames[strings[stringIndex].numAttributes], stAttrName, RAMAllocSize);
  strings[stringIndex].attrvalues[strings[stringIndex].numAttributes] = attrvalue;

  strings[stringIndex].numAttributes++;
  return 0;
}

// load strings from file *fname ("strings.dat")
char loadstrings(const char *fname)
{
FILE *fp;
char state;
unsigned char stName[80];
unsigned char stString[1024];
unsigned char stAttr[80];
int i,c;
int nameIndex, stringIndex;
int attrIndex=0;
int waitChar, gotoState;
char highlight;
int RAMSize;
char *RAMPtr;

  #define STSTATE_WAITCHAR      0
  #define STSTATE_READNAME      1
  #define STSTATE_READSTRING    2
  #define STSTATE_READATTR      3

g_pLogFile->ftextOut("loadstrings(): Opening string file '%s'.<br>", fname);
  fp = fopen(fname, "rb");
  if (!fp)
  {
	  g_pLogFile->ftextOut("loadstrings(): String file unable to open.<br>");
    return 1;
  }

  // go through all the strings and NULL out the entries...this will
  // let us know which ones are in use (and need to be free()d at shutdown)
  for(i=0;i<MAX_STRINGS;i++)
  {
    strings[i].name = NULL;
    strings[i].stringptr = NULL;
    strings[i].numAttributes = 0;
  }

  nameIndex = 0;
  stringIndex = 0;
  numStrings = 0;
  highlight = 0;

  // read until we get to the first string name
  state = STSTATE_WAITCHAR;
  waitChar = '[';
  gotoState = STSTATE_READNAME;
  do
  {
    c = fgetc(fp);              // read byte from file

    if (c<0)
    {   // EOF
      break;
    }
    // ignore LF's
    if (c==10) continue;

    switch(state)
    {
     case STSTATE_WAITCHAR:
      // ignore chars until we read a waitChar, then go to state gotoState
      if (c==waitChar)
      {
        state = gotoState;
      }
      break;
     case STSTATE_READATTR:
      if (c==13)
      { // reached CR, start reading string
        if (attrIndex)
        {
          stAttr[attrIndex] = 0;
          if (loadstrings_AddAttr( (char*) stAttr, numStrings)) return 1;
        }
        state = STSTATE_READSTRING;
      }
      else if (c==' ')
      { // end of an attribute definition
        if (attrIndex)
        {
          stAttr[attrIndex] = 0;  // null-terminate
          if (loadstrings_AddAttr( (char*) stAttr, numStrings)) return 1;
        }
        attrIndex = 0;
      }
      else
      { // save char to attribute buffer
        stAttr[attrIndex] = c;
        attrIndex++;
      }
      break;
     case STSTATE_READNAME:
      // read in the string name until we get to ']'
      if (c != ']')
      {
        stName[nameIndex] = c;
        nameIndex++;
      }
      else
      {
        stName[nameIndex] = 0;  //null-terminate
	highlight = 0;
        // read any attributes until the CR
        state = STSTATE_READATTR;
        attrIndex = 0;
      }
      break;
     case STSTATE_READSTRING:
      // read in string data until we see another '['
      if (c != '[')
      {
        // allow delimiters:
        // you can put [ and ] in the string by using \( and \).
        // set a highlight (change font color to the +128 font) with \H
        // stop highlighting with \h
        if (stringIndex>0 && stString[stringIndex-1]=='\\'+(highlight*128))
        {  // delimiter detected
          if (c=='(')
          {
            stString[stringIndex - 1] = '[' + (highlight*128);
          }
          else if (c==')')
          {
            stString[stringIndex - 1] = ']' + (highlight*128);
          }
          else if (c=='H')
          {
            highlight = 1;
            stringIndex--;
          }
          else if (c=='h')
          {
            highlight = 0;
            stringIndex--;
          }
          else if (c=='\\')
          {
            stString[stringIndex - 1] = '\\' + (highlight*128);
          }
        }
        else
        { // normal non-delimited char
          stString[stringIndex] = c;
          if (highlight && c!=0 && c!=13)
          {
            stString[stringIndex] += 128;
          }
          stringIndex++;
        }
      }
      else
      {
        stString[stringIndex-1] = 0;  //null-terminate (cutting off final CR)

        /* save the string to the strings[] structure */

        // we're going to malloc() an area and copy the name, then the string,
        // into it. We'll need room for both the name and the string, plus
        // null-terminators for each.
        RAMSize = strlen( (char*) stName) + strlen((char*)stString) + 2;
        RAMPtr = (char*) malloc(RAMSize);
        if (!RAMPtr)
        {
          g_pLogFile->ftextOut(RED,"loadstrings(): Could not allocate memory for string '%s'<br>", stName);
          return 1;
        }

        // assign our pointers
        strings[numStrings].name = (unsigned char*) &RAMPtr[0];
        strings[numStrings].stringptr = (unsigned char*) &RAMPtr[strlen((char*)stName)+1];

        // copy the string info to the newly malloc()'d memory area
        memcpy(strings[numStrings].name, stName, strlen((char*)stName)+1);
        memcpy(strings[numStrings].stringptr, stString, strlen((char*)stString)+1);

        numStrings++;
        // read the name of the next string
        state = STSTATE_READNAME;
        nameIndex = 0;
        stringIndex = 0;
      }
      break;
    }

  } while(1);

  g_pLogFile->ftextOut("loadstrings(): loaded %d strings from '%s'.<br>", numStrings, fname);
  fclose(fp);
  return 0;
}

int freestrings(void)
{
int i,j;
int NumStringsFreed;

  NumStringsFreed = 0;
  for(i=0;i<MAX_STRINGS;i++)
  {
    if (strings[i].name)
    {
      // free the string name
      free(strings[i].name);
      strings[i].name = strings[i].stringptr = NULL;
      // free all attribute names
      for(j=0;j<strings[i].numAttributes;j++)
      {
        free(strings[i].attrnames[j]);
      }
      strings[i].numAttributes = 0;

      NumStringsFreed++;
    }
  }

  return NumStringsFreed;
}

const char *MissingString = "MISSING STRING!";

// returns a pointer to the string with name 'name'
char* getstring(const char *name)
{
int i;
  for(i=0;i<numStrings;i++)
  {
    if (!strcmp((char*) name, (char*) strings[i].name))
    {
      return (char*) strings[i].stringptr;
    }
  }

  return (char*) MissingString;
}

// because windows and linux read files differently, these is to function to get them correctly
void formatPathString(char *output, const char *path)
{
	char *buf;

	strcpy(output,"data");

	strcat(output,"/");

	buf = strchr(path,'\r');

	if(buf)
		*buf=0; // Trim return caret

	if(path[0] != 0 )
	{
		strcat(output,path);
		strcat(output,"/");
	}

}

// returns attribute attrname of string stringname, or -1 if it doesn't exist.
int GetStringAttribute(const char *stringName, const char *attrName)
{
int i,j;
  for(i=0;i<numStrings;i++)
  {
    if (!strcmp((char*) stringName, (char*) strings[i].name))
    {
      // we found the string, now find the requested attribute
      for(j=0;j<strings[i].numAttributes;j++)
      {
        if (!strcmp((char*) attrName, (char*) strings[i].attrnames[j]))
        {
          return strings[i].attrvalues[j];
        }
      }
      // failed to find attribute
      return -1;
    }
  }
  // failed to find string
  return -1;
}

