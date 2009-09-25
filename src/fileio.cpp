/* FILEIO.C
  Functions responsible for loading data from files, such as the one that
  decodes the level map files (loadmap()) and the one that loads in the
  tile attribute data contained in ep?attr.dat (loadtileattributes()).
  The functions for loading the graphics (EGALATCH&EGASPRIT) are in latch.c.
*/

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <stdio.h>

#include "keen.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CSound.h"
#include "include/fileio.h"
#include "include/fileio/rle.h"
#include "CLogFile.h"
#include "StringUtils.h"
#include "Debug.h"
#include "FindFile.h"

#ifdef TARGET_WIN32
#define uint unsigned int
#define ulong unsigned long
#endif

short checkConsistencyofGameData(stGameData *p_GameData)
{
	short ok = 0;

	FILE *fp;

	// Let's define which files need to be read!
	if(p_GameData->Episode >= 1 && p_GameData->Episode <= 3)
	{
		p_GameData->FileList[1] = "keen" + itoa(p_GameData->Episode) + ".exe";
		p_GameData->FileList[2] = "egahead.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[3] = "egasprit.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[4] = "egalatch.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[5] = "finale.ck" + itoa(p_GameData->Episode);

		// Levels!
		p_GameData->FileList[6] = "level01.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[7] = "level02.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[8] = "level03.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[9] = "level04.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[10] = "level05.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[11] = "level06.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[12] = "level07.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[13] = "level08.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[14] = "level09.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[15] = "level10.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[16] = "level11.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[17] = "level12.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[18] = "level13.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[19] = "level14.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[20] = "level15.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[21] = "level16.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[22] = "level80.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[23] = "level81.ck" + itoa(p_GameData->Episode);
		p_GameData->FileList[24] = "level90.ck" + itoa(p_GameData->Episode);

		// Other
		if(p_GameData->Episode == 1)
			p_GameData->FileList[24] = "sounds.ck" + itoa(p_GameData->Episode);
	}


	// Finally check if they really exist!
	int c=0;
	for(c = 0 ; c < MAX_NUMBER_OF_FILES ; c++)
	{
		if(p_GameData->FileList[c][0] == 0) // If there are no more files!
			break;

		std::string buf = p_GameData->DataDirectory;
		if(p_GameData->DataDirectory != "")
			buf += "/";
		buf += p_GameData->FileList[c];

		if((fp = OpenGameFile(buf.c_str(),"r")) != NULL)
		{
			fclose(fp);
		}
		else
		{
			g_pLogFile->ftextOut("Error! File \"%s\" was not found! Please add it to the configured directory.<br>", p_GameData->FileList[c].c_str());
			p_GameData->Episode = -1;
			ok++;
		}
	}

	if(ok==0)
		g_pLogFile->ftextOut("Game data of Episode %d is complete.<br>", p_GameData->Episode);

	return ok;
}

char NessieAlreadySpawned;
void addobjectlayertile(unsigned int t, int episode, bool *levels_completed)
{
/*int o;
  switch(t)
  {
   case 0: break;       // blank
   case 255:            // player start

	 //player[0].x = curmapx << 4 << CSF;
     //player[0].y = curmapy << 4 << CSF;
     map.objectlayer[curmapx][curmapy] = 0;
     break;
   case NESSIE_PATH:          // spawn nessie at first occurance of her path
     if (episode==3)
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
     if ((t&0x7fff) < 256 && levels_completed[t&0x00ff])
     {
    	 if(!options[OPT_LVLREPLAYABILITY].value)
    		map.objectlayer[curmapx][curmapy] = 0;
    	 else
    		 map.objectlayer[curmapx][curmapy] = t;

    	 int newtile = tiles[map.mapdata[curmapx][curmapy]].chgtile;

    	 // Consistency check! Some Mods have issues with that.
    	 if(episode == 1 || episode == 2)
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
    	 else if(episode == 3)
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
  }*/

}

void addenemytile(unsigned int t, int episode,
					int chglevelto)
{
/*int o,x;
  map.objectlayer[curmapx][curmapy] = t;

  if (t)
  {
    if (t==255)
    {
        if(curmapx >= map.xsize-2) // Edge bug. Keen would fall in some levels without this.
      	   curmapx = 4;

        if(curmapy >= map.ysize-2) // Edge bug. Keen would fall in some levels without this.
      	   curmapx = 4;

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
           if (episode==1)
           {
              x = curmapx;

              if (TileProperty[map.mapdata[x][curmapy+1]][BLEFT]) x--;
              spawn_object(x<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_YORP);
           }
           else
           {
              // in ep2 level 16 there a vorticon embedded in the floor for
              // some reason! that's what the if() is for--to fix it.
        	  // TODO: Is this still needed?
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
           if (episode==1)
           {
             // those bastards. sometimes embedding garg's in the floor in
             // the original maps.
        	 if(TileProperty[map.mapdata[curmapx+1][curmapy+1]][BLEFT])
             {
               if (chglevelto==7)
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
           if (episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORT);
           }
           else if (episode==2)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_VORTELITE);
           }
           else if (episode==3)
           {
              spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_MOTHER);
           }
           break;
      case 4:    // butler (ep1) OR scrub (ep2) OR meep (ep3)
           if (episode==1)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_BUTLER);
           else if (episode==2)
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_SCRUB);
           else if (episode==3)
             spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_MEEP);
           break;
      case 5:    // tank robot (ep1&2) karate bear (ep3)
           if (episode==1)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+8)<<CSF, OBJ_TANK);
              // set tank robot guarding bonus level to be active at startup
              if (chglevelto==13)
              {
                objects[o].hasbeenonscreen = 1;
              }
           }
           else if (episode==2)
              spawn_object(curmapx<<4<<CSF, ((curmapy<<4)+0)<<CSF, OBJ_TANKEP2);
           else if (episode==3)
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
           if (episode==1)
           {
              o = spawn_object((((curmapx+1)<<4)+4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_ICECANNON);
              objects[o].ai.icechunk.vector_x = 1;
              objects[o].ai.icechunk.vector_y = -1;
           }
           else if (episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF, ((curmapy<<4)-3)<<CSF, OBJ_PLATFORM);
           }
           else if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_FOOB);
           }
           break;
      case 7:   // spark (ep2) ball (ep3)
           if (episode==2)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_SPARK);
           }
           else if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_BALL);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 8:    // jack (ep3)
           if (episode==3)
           {
              o = spawn_object(curmapx<<4<<CSF,curmapy<<4<<CSF,OBJ_JACK);
              objects[o].hasbeenonscreen = 1;
           }
           break;
      case 9:    // up-left-flying ice chunk (ep1) horiz platform (ep3)
           if (episode==1)
           {
             o = spawn_object(((curmapx<<4)-4)<<CSF, ((curmapy<<4)-4)<<CSF, OBJ_ICECANNON);
             objects[o].ai.icechunk.vector_x = -1;
             objects[o].ai.icechunk.vector_y = -1;
           }
           else if (episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, (((curmapy)<<4)-4)<<CSF, OBJ_PLATFORM);
           }
           break;
      case 10:   // rope holding the stone above the final vorticon (ep1)
                 // vert platform (ep3)
           if (episode==1)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_ROPE);
           }
           else if (episode==3)
           {
             spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_PLATVERT);
           }
           break;
      case 11:   // jumping vorticon (ep3)
           if (episode==3)
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
           if (episode==3)
           {
             o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_AUTORAY);
           }
           break;
      case 15:   // vertical raygun (ep3)
           if (episode==3)
           {
        	   o = spawn_object(curmapx<<4<<CSF, curmapy<<4<<CSF, OBJ_AUTORAY_V);
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
           g_pLogFile->ftextOut(PURPLE,"unknown enemy type %d at (%d,%d)<br>", t, curmapx, curmapy);
           break;
      }
    }
  }
*/
}

unsigned int fgeti(FILE *fp) {
unsigned int lsb, msb;
  lsb = fgetc(fp);
  msb = fgetc(fp);
  return (msb<<8) | lsb;
}

void fputi(uint word, FILE *fp)
{
	fputc(word&255, fp);
	fputc(word/256, fp);
}

unsigned long fgetl(FILE *fp) {
unsigned int temp1, temp2, temp3, temp4;
  temp1 = fgetc(fp);
  temp2 = fgetc(fp);
  temp3 = fgetc(fp);
  temp4 = fgetc(fp);
  return (temp4<<24) | (temp3<<16) | (temp2<<8) | temp1;
}

void fputl(unsigned long word, FILE *fp)
{
unsigned long a,b,c,d;
	a=b=c=d = word;
	a &= 0xFF000000; a >>= 24;
	b &= 0x00FF0000; b >>= 16;
	c &= 0x0000FF00; c >>= 8;
	d &= 0x000000FF;
	fputc(d, fp);
	fputc(c, fp);
	fputc(b, fp);
	fputc(a, fp);
}

unsigned int loadmap(const std::string& filename, const std::string& path,
									int lvlnum,stLevelControl* p_levelcontrol)
{
/*	FILE *fp;
	int t;
	unsigned int c;
	int numruns = 0;
	int resetcnt, resetpt;
	unsigned int planesize = 0;

	int episode = p_levelcontrol->episode;
	int chglevelto = p_levelcontrol->chglevelto;
	bool *levels_completed = p_levelcontrol->levels_completed;

	NessieAlreadySpawned = 0;
	map.isworldmap = (lvlnum == 80);

	std::string buffer = formatPathString(path);
	std::string fname = buffer + filename;
	fp = OpenGameFile(fname.c_str(), "rb");
  if (!fp)
  {
	  // only record this error message on build platforms that log errors
	  // to a file and not to the screen.
	  g_pLogFile->ftextOut("loadmap(): unable to open file %s<br>", fname.c_str());
	  return 1;
  }
  g_pLogFile->ftextOut("loadmap(): file %s opened. Loading...<br>", fname.c_str());

    // decompress map RLEW data
	std::vector<unsigned int> filebuf;
	unsigned long finsize; 		   // Final size

	finsize = unRLEW(fp, filebuf);

  c=0;

  if(!finsize)
  {
	  rewind(fp);
	  while(!feof(fp))
	  {
		  filebuf.push_back( fgeti(fp) );
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

  planesize = filebuf[9];
  planesize /= 2; // Size of two planes, but we only need one

  for( c=18 ; c<planesize+18 ; c++ ) // Check against Tilesize
  {
		t = filebuf[c];

		//addmaptile(t);

		curmapx++;
		if (curmapx >= map.xsize)
		{
			curmapx = 0;
			curmapy++;
			if (curmapy >= map.ysize) break;
		}

		if(t > 255)
			t=0; // If there are some invalid values in the file
  }

  // now do the sprites
  // get sprite data
    curmapx = curmapy = numruns = 0;
    resetcnt = resetpt = 0;

    for( c=planesize+18 ; c<2*planesize+18 ; c++ )
    {
    	t = filebuf[c];

        if (map.isworldmap) addobjectlayertile(t,  episode,  levels_completed);
        else addenemytile(t, episode, chglevelto);

        curmapx++;
        if (curmapx >= map.xsize)
        {
          curmapx = 0;
          curmapy++;
          if (curmapy >= map.ysize) break;
        }

        if (++resetcnt==resetpt) curmapx=curmapy=0;
    }

    filebuf.clear();
    fclose(fp);


 // install enemy stoppoints as needed
 if (episode==1 && lvlnum==13)
 {
    map.objectlayer[94][13] = GARG_STOPPOINT;
    map.objectlayer[113][13] = GARG_STOPPOINT;
    map.objectlayer[48][6] = GARG_STOPPOINT;
    map.objectlayer[80][5] = GARG_STOPPOINT;
    map.objectlayer[87][5] = GARG_STOPPOINT;
    map.objectlayer[39][18] = GARG_STOPPOINT;
 }
 else if (episode==3 && lvlnum==6)
 {
    map.objectlayer[40][7] = BALL_NOPASSPOINT;
    map.objectlayer[50][7] = BALL_NOPASSPOINT;
 }
 else if (episode==3 && lvlnum==9)
 {
    map.objectlayer[45][106] = BALL_NOPASSPOINT;

 }
 else if (episode==3 && lvlnum==4)
 {
    map.objectlayer[94][17] = BALL_NOPASSPOINT;
 }
*/
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
char loadstrings()
{
FILE *fp;
char state;
	std::string stName;
	std::string stString;
char stAttr[80];
int i,c;
int attrIndex=0;
int waitChar, gotoState;
char highlight;

  #define STSTATE_WAITCHAR      0
  #define STSTATE_READNAME      1
  #define STSTATE_READSTRING    2
  #define STSTATE_READATTR      3

  g_pLogFile->ftextOut("loadstrings(): Opening string file 'strings.dat'.<br>");
  fp = OpenGameFile("strings.dat", "rb");
  if (!fp)
  {
	  g_pLogFile->ftextOut("loadstrings(): String file unable to open.<br>");
    return 1;
  }

  // go through all the strings and NULL out the entries...this will
  // let us know which ones are in use (and need to be free()d at shutdown)
  for(i=0;i<MAX_STRINGS;i++)
  {
    strings[i].name = "";
    strings[i].stringptr = "";
    strings[i].numAttributes = 0;
  }

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
		  stName += (char)c;
      }
      else
      {
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
        if (stString.size()>0 && stString[stString.size()-1]==char('\\'+(highlight*128)))
        {  // delimiter detected
          if (c=='(')
          {
            stString[stString.size()-1] = '[' + (highlight*128);
          }
          else if (c==')')
          {
            stString[stString.size()-1] = ']' + (highlight*128);
          }
          else if (c=='H')
          {
            highlight = 1;
            stString.erase(stString.size()-1);
          }
          else if (c=='h')
          {
            highlight = 0;
			stString.erase(stString.size()-1);
          }
          else if (c=='\\')
          {
            stString[stString.size() - 1] = '\\' + (highlight*128);
          }
        }
        else
        { // normal non-delimited char
			stString += (char)c + ((highlight && c!=0 && c!=13) ? 128 : 0);
        }
      }
      else
      {
//		  HexDump(GetConstIterator(stName), printOnLogger<notes>, 0);
//		  HexDump(GetConstIterator(stString), printOnLogger<notes>, 0);
//		  notes.flush();
        /* save the string to the strings[] structure */

        // copy the string info to the newly malloc()'d memory area
		  strings[numStrings].name = stName;
		  strings[numStrings].stringptr = stString;
		  stName = "";
		  stString = "";
		  
        numStrings++;
        // read the name of the next string
        state = STSTATE_READNAME;
      }
      break;
    }

  } while(1);

  g_pLogFile->ftextOut("loadstrings(): loaded %d strings from 'strings.dat'.<br>", numStrings);
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
    if (strings[i].name != "")
    {
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

/*static void dumpstrings() {
	notes << "Available strings: ";
	for(int i=0;i<numStrings;i++)
		notes << strings[i].name << ", ";
	notes << endl;
}*/

// returns a pointer to the string with name 'name'
std::string getstring(const std::string& name)
{
  for(int i=0;i<numStrings;i++)
  {
	  if (name == strings[i].name)
	  {
		  // Returning is not enough anymore
		  // Convert the /h and /H into proper colours
		  // This is necessary since the new graphics engine

		  return strings[i].stringptr;
	  }
  }

	//dumpstrings();
	
  return "UNKNOWN '" + name + "' STRING";
}

// because windows and linux read files differently, these is to function to get them correctly
std::string formatPathString(const std::string& path)
{
	std::string buffer = path;
	if( buffer.size() > 0 && buffer[buffer.size()-1] != '/' )
		buffer += '/';
	return buffer;
}

// returns attribute attrname of string stringname, or -1 if it doesn't exist.
int GetStringAttribute(const std::string& stringName, const char *attrName)
{
int i,j;
  for(i=0;i<numStrings;i++)
  {
    if (stringName == strings[i].name)
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
