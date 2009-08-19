/*
 * CHighScores.cpp
 *
 *  Created on: 09.04.2009
 *      Author: gerstrong
 */

#include <iostream>
#include <fstream>
#include <string>

#include <cstring>

#include "../keen.h"
#include "../include/menu.h"
#include "../include/gamedo.h"
#include "CHighScores.h"
#include "../sdl/CInput.h"
#include "../sdl/CTimer.h"
#include "../CGraphics.h"
#include "../StringUtils.h"
#include "../FindFile.h"

#define HIGHSCORETABLE_X			1344
#define HIGHSCORETABLE_Y			32

using namespace std;


CHighScores::CHighScores(stCloneKeenPlus *poutsideCKP) {
	// Set default Scores
	strcpy(Name[0],"Gerstrong");
	strcpy(Name[1],"Tulip");
	strcpy(Name[2],"Spleen");
	strcpy(Name[3],"Yorpy");
	strcpy(Name[4],"Garg");
	strcpy(Name[5],"Vorticon");
	strcpy(Name[6],"maybeyou");

	int i;
	for(i=0 ; i<7 ; i++)
		sprintf(Score[i],"100");

	memset(Extra, 0, 7*4*sizeof(unsigned char));
	memset(Cities, 0, 7*sizeof(unsigned int));

	pCKP = poutsideCKP;

	Episode = pCKP->Control.levelcontrol.episode;
	DataDirectory = pCKP->Resources.GameDataDirectory;
}

CHighScores::~CHighScores() {
}


char CHighScores::showHighScore(void)
{
	unsigned char i;
	int x1,y1;
	int x2,y2;
	int x3;
	int x4;
	int bTitle;
	int bName;
	int bScore;
	int bExtra;

	loadHighScoreTable();

	// show High-score Frame
	g_pGraphics->unloadHQGraphics();
	showmapatpos(90, HIGHSCORETABLE_X, HIGHSCORETABLE_Y, pCKP);

	// Prepare some extra items if available (Ep1 + 2)
	memset(ItemTiles,0,4*sizeof(int));

	if(pCKP->Control.levelcontrol.episode == 1)
	{
		int t;
		for(t=0 ; t<numtiles ;t++)
		{
			if(TileProperty[t][BEHAVIOR] == 11)
				ItemTiles[0] = t;
			if(TileProperty[t][BEHAVIOR] == 12)
				ItemTiles[1] = t;
			if(TileProperty[t][BEHAVIOR] == 13)
				ItemTiles[2] = t;
			if(TileProperty[t][BEHAVIOR] == 14)
				ItemTiles[3] = t;
		}
	}

	// Get the Bitmap IDs and set the correct positions on screen
    bTitle = g_pGraphics->getBitmapNumberFromName("HIGHSCOR");
    bName = g_pGraphics->getBitmapNumberFromName("NAME");
    bScore = g_pGraphics->getBitmapNumberFromName("SCORE");

    if(Episode == 2)
    	bExtra = g_pGraphics->getBitmapNumberFromName("SAVED");
    else
    	bExtra = g_pGraphics->getBitmapNumberFromName("PARTS");

    x1 = 160-(bitmaps[bTitle].xsize/2);
    y1 = 8;
    x2 = 40;
    y2 = 42;
    x3 = 178-(bitmaps[bScore].xsize/2);
    x4 = 230;

    // They are blit once.
    g_pGraphics->drawBitmap(x1, y1, bTitle);
    g_pGraphics->drawBitmap(x2, y2, bName);
    g_pGraphics->drawBitmap(x3, y2, bScore);
    if(Episode == 2)
    	g_pGraphics->drawBitmap(x4, y2-8, bExtra);
    else
    	g_pGraphics->drawBitmap(x4, y2, bExtra);

	// This cycle only serves as a key which must be pressed for now
	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// Here it must be split up in Episodes 1, 2 and 3.
			if(pCKP->Control.levelcontrol.episode == 1)
			{
				// Print the labels
				for( i=0 ; i<7 ; i++ )
				{
					g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);

					g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4),true);
					if(Extra[i][0] == true)
						g_pGraphics->drawTile(32,90+(i<<4),ItemTiles[0]);
					if(Extra[i][1] == true)
						g_pGraphics->drawTile(48,90+(i<<4),ItemTiles[1]);
					if(Extra[i][2] == true)
						g_pGraphics->drawTile(64,90+(i<<4),ItemTiles[2]);
					if(Extra[i][3] == true)
						g_pGraphics->drawTile(80,90+(i<<4),ItemTiles[3]);
				}
			}
			else if(pCKP->Control.levelcontrol.episode == 2)
			{
				for( i=0 ; i<7 ; i++ )
				{
					std::string buf;
					g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);
					g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4), true);
					buf = itoa(Cities[i]);
					g_pGraphics->sb_font_draw(buf,250,64+(i<<4), true);
				}
			}
			else
			{
				for( i=0 ; i<7 ; i++ )
				{
					g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);
					g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4), true);
				}
			}

			gamedo_AnimatedTiles();
			g_pInput->pollEvents();
		}
		if (g_pTimer->TimeToRender())
			gamedo_RenderScreen();

	}while(!g_pInput->getPressedAnyKey());

	return 0;
}

char CHighScores::writeHighScore(int points, bool *extras, int cities)
{
	int place;
	char buf[256];

	unsigned char i;
	int num;
	int x1,y1;
	int x2,y2;
	int x3;
	int x4;
	int bTitle;
	int bName;
	int bScore;
	int bExtra;

	loadHighScoreTable();

	// show High-score Frame
	showmapatpos(90, HIGHSCORETABLE_X, HIGHSCORETABLE_Y, pCKP);

	fade.mode = FADE_GO;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;
	fade.rate = FADE_NORM;
	ShipQueuePtr = 0;

	showmapatpos(90, (104 << 2)+256+256+80, 32-4, pCKP);

	place=6;
	sscanf(Score[place],"%d",&num);
	if(num > points)
		return 1; // Sorry, but score not high enough

	while(place>0 && points > num)
	{
		memcpy(Name[place],Name[place-1],16*sizeof(char));
		memcpy(Score[place],Score[place-1],8*sizeof(char));
		memcpy(Extra[place],Extra[place-1],4*sizeof(char));
		place--;

		if(place > 0)
			sscanf(Score[place-1],"%d",&num);
	}

	sprintf(buf,"%d", points);

	memcpy(Name[place+1],Name[place],16*sizeof(char));
	memcpy(Score[place],buf,8*sizeof(char));

	if(points < 999999)
		sprintf(Score[place],"%d",points);
	else
		sprintf(Score[place],"*MAX*");

	memcpy(Extra[place],extras,4*sizeof(bool));
	Cities[place] = cities;

	memset(Name[place],0,16*sizeof(char));

	string WrittenName;

	// Prepare some extra items if available (Ep1 + 2)
	memset(ItemTiles,0,4*sizeof(int));

	if(pCKP->Control.levelcontrol.episode == 1)
	{
		int t;
		for(t=0 ; t<numtiles ;t++)
		{
			if(TileProperty[t][BEHAVIOR] == 11)
				ItemTiles[0] = t;
			if(TileProperty[t][BEHAVIOR] == 12)
				ItemTiles[1] = t;
			if(TileProperty[t][BEHAVIOR] == 13)
				ItemTiles[2] = t;
			if(TileProperty[t][BEHAVIOR] == 14)
				ItemTiles[3] = t;
		}
	}
	else if(pCKP->Control.levelcontrol.episode == 2)
	{
		for( i=0 ; i<7 ; i++ )
		{
			std::string buf;
			g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);
			g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4), true);
			buf = itoa(Cities[i]);
			g_pGraphics->sb_font_draw(buf,250,64+(i<<4), true);
		}
	}
	else
	{
		for( i=0 ; i<7 ; i++ )
		{
			g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);
			g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4), true);
		}
	}


	// Get the Bitmap IDs and set the correct positions on screen
    bTitle = g_pGraphics->getBitmapNumberFromName("HIGHSCOR");
    bName = g_pGraphics->getBitmapNumberFromName("NAME");
    bScore = g_pGraphics->getBitmapNumberFromName("SCORE");

    if(Episode == 2)
    	bExtra = g_pGraphics->getBitmapNumberFromName("SAVED");
    else
    	bExtra = g_pGraphics->getBitmapNumberFromName("PARTS");

    x1 = 160-(bitmaps[bTitle].xsize/2);
    y1 = 8;
    x2 = 40;
    y2 = 42;
    x3 = 178-(bitmaps[bScore].xsize/2);
    x4 = 230;


    // They are blit once.
    g_pGraphics->drawBitmap(x1, y1, bTitle);
    g_pGraphics->drawBitmap(x2, y2, bName);
    g_pGraphics->drawBitmap(x3, y2, bScore);
    if(Episode == 2)
    	g_pGraphics->drawBitmap(x4, y2-8, bExtra);
    else
    	g_pGraphics->drawBitmap(x4, y2, bExtra);

    memset(buf,0,256);

	// This cycle will wait for the input of name and hit of enter
    bool blink = true;
    int blinkctr = 0;
	do
	{
		if(g_pTimer->TimeToRunLogic())
		{
			// Blit all the text and images
			for(i=KA ; i<KZ ; i++)
			{
				if(g_pInput->getPressedKey(i))
				{
					sprintf(buf,"%c",'A' + i - KA);
					WrittenName.append(buf);
					copy(WrittenName.data(),WrittenName.data()+WrittenName.length(),Name[place]);
					WrittenName.copy(buf,WrittenName.length(),0);
				}
			}
			if(g_pInput->getPressedKey(KBCKSPCE) && (WrittenName.length() > 0))
			{
				memset(buf,0,256);
				g_pGraphics->sb_font_draw("              ",40,64+(place<<4), true);
				WrittenName.erase(WrittenName.length()-1);
				WrittenName.copy(buf,WrittenName.length(),0);
				memset(Name[place],0,16);
				WrittenName.copy(Name[place],WrittenName.length(),0);
			}


			if(WrittenName.length() > 14)
				break;

			// Here it must be split up in Episodes 1, 2 and 3.
			// Print the labels
			for( i=0 ; i<7 ; i++ )
			{
				if(i != place)
					g_pGraphics->sb_font_draw(Name[i],40,64+(i<<4), true);
				else
				{
					g_pGraphics->sb_font_draw(buf,40,64+(i<<4), true);
					if(blink)
						g_pGraphics->sb_font_draw("_",40+(strlen(buf)<<3),64+(i<<4), true);
					else
						g_pGraphics->sb_font_draw(" ",40+(strlen(buf)<<3),64+(i<<4), true);
				}
				g_pGraphics->sb_font_draw(Score[i],200-(strlen(Score[i])<<3),64+(i<<4), true);

				if(pCKP->Control.levelcontrol.episode == 1)
				{
					if(Extra[i][0] == true)
						g_pGraphics->drawTile(32,90+(i<<4),ItemTiles[0]);
					if(Extra[i][1] == true)
						g_pGraphics->drawTile(48,90+(i<<4),ItemTiles[1]);
					if(Extra[i][2] == true)
						g_pGraphics->drawTile(64,90+(i<<4),ItemTiles[2]);
					if(Extra[i][3] == true)
						g_pGraphics->drawTile(80,90+(i<<4),ItemTiles[3]);
				}
			}

			gamedo_AnimatedTiles();
			g_pInput->pollEvents();

			blinkctr++; // The blinking cursor
			if(blinkctr > 100)
			{
				blink = !blink;
				blinkctr = 0;
			}
		}
		if (g_pTimer->TimeToRender())
			gamedo_RenderScreen();

	}while(!g_pInput->getPressedKey(KENTER));

	saveHighScoreTable();

	return 0;
}

char CHighScores::loadHighScoreTable(void)
{
	string sBuf;
	std::string chBuf = itoa(Episode);

	sBuf.append("data/");
	sBuf.append(DataDirectory);
	sBuf.append("highscoreep");
	sBuf.append(chBuf);
	sBuf.append(".dat");

	ifstream ScoreTableFile; OpenGameFileR(ScoreTableFile, sBuf, ios::binary);

	if(ScoreTableFile == NULL)
	{
		return -1;
	}

	ScoreTableFile.read((*Name), 7*16*sizeof(char));
	ScoreTableFile.seekg(7*16*sizeof(char),ios_base::cur);
	ScoreTableFile.read((*Score), 7*8*sizeof(char));
	ScoreTableFile.seekg(7*8*sizeof(char),ios_base::cur);
	ScoreTableFile.read((char*)(*Extra), 7*4*sizeof(unsigned char));
	ScoreTableFile.seekg(7*4*sizeof(char),ios_base::cur);
	ScoreTableFile.read((char*)Cities, 7*sizeof(unsigned char));

	ScoreTableFile.close();

	return 0;
}

char CHighScores::saveHighScoreTable(void)
{
	string sBuf;
	char chBuf[256];

	sprintf(chBuf,"%d",Episode);

	sBuf.append("data/");
	sBuf.append(DataDirectory);
	sBuf.append("highscoreep");
	sBuf.append(chBuf);
	sBuf.append(".dat");

	ofstream ScoreTableFile; OpenGameFileW(ScoreTableFile, sBuf, ios::binary);

	if(ScoreTableFile == NULL)
	{
		return -1;
	}

	ScoreTableFile.write((*Name), 7*16*sizeof(char));
	ScoreTableFile.seekp(7*16*sizeof(char),ios_base::cur);
	ScoreTableFile.write((*Score), 7*8*sizeof(char));
	ScoreTableFile.seekp(7*8*sizeof(char),ios_base::cur);
	ScoreTableFile.write((char*)(*Extra), 7*4*sizeof(unsigned char));
	ScoreTableFile.seekp(7*4*sizeof(char),ios_base::cur);
	ScoreTableFile.write((char*) Cities, 7*sizeof(unsigned char));


	ScoreTableFile.close();

	return 0;
}

