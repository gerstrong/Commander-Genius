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
			
			const char *buf = strings[i].stringptr.c_str();

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
