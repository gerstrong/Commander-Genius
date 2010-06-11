/*
 * CHighScores.cpp
 *
 *  Created on: 09.04.2009
 *      Author: gerstrong
 */

#include <iostream>
#include <fstream>
#include <string>

#include "CHighScores.h"
#include "../../sdl/CInput.h"
#include "../../graphics/CGfxEngine.h"
#include "../../StringUtils.h"
#include "../../FindFile.h"
#include "../../sdl/CVideoDriver.h"
#include "../../common/CMapLoader.h"

const int HIGHSCORETABLE_X = 1344;
const int HIGHSCORETABLE_Y = 32;
const int BLINK_TIME = 10;

using namespace std;


CHighScores::CHighScores(int Episode, const std::string &DataDirectory, bool saving_mode) :
m_Place(0), m_blink(true), m_blinkctr(0)
{
	m_Map.setScrollSurface(g_pVideoDriver->ScrollSurface);
	m_Map.setTileMap(g_pGfxEngine->getTileMap(0));

	// Set default Scores
	m_Name[0] = "Gerstrong";
	m_Name[1] = "Tulip";
	m_Name[2] = "Albert";
	m_Name[3] = "Pickle";
	m_Name[4] = "Pizza2004";
	m_Name[5] = "DaVince";
	m_Name[6] = "Katy Shaw";
	m_Name[7] = "The Fans";
	
	for(int i=0 ; i<8 ; i++)
		m_Score[i] = "100";
	
	memset(m_Extra, 0, 8*4*sizeof(bool));
	memset(m_Cities, 0, 8*sizeof(unsigned int));

	// Which process function will be cycled trough
	mp_process = &CHighScores::processShow;

	m_Episode = Episode;
	m_DataDirectory = DataDirectory;

	loadHighScoreTable();

	// Load the map for the background
	CMapLoader MapLoader(&m_Map);
	MapLoader.load(m_Episode, 90, DataDirectory, false);

    m_Map.gotoPos(HIGHSCORETABLE_X, HIGHSCORETABLE_Y);

    // Draw Background. This is only needed once, since Scrollsurface
    // won't be cleared every update screen
    m_Map.drawAll();

	// Setup the Bitmaps that will be shown
	stBitmap bmp;

	bmp.p_Bitmap = g_pGfxEngine->getBitmap("HIGHSCOR");
	bmp.rect.x = 160-(bmp.p_Bitmap->getWidth()/2);
	bmp.rect.y = 6;
	bmp.rect.w = bmp.p_Bitmap->getWidth();
	bmp.rect.h = bmp.p_Bitmap->getHeight();
	m_Bitmaps.push_back(bmp);

	if(m_Episode == 2)
	{
		bmp.p_Bitmap = g_pGfxEngine->getBitmap("SAVED");
		bmp.rect.x = 232;
		bmp.rect.y = 32;
		bmp.rect.w = bmp.p_Bitmap->getWidth();
		bmp.rect.h = bmp.p_Bitmap->getHeight();
		m_Bitmaps.push_back(bmp);
		bmp.rect.y = 36;
	}
	else if(m_Episode == 1)
	{

		bmp.p_Bitmap = g_pGfxEngine->getBitmap("PARTS");
		bmp.rect.x = 232;
		bmp.rect.y = 32;
		bmp.rect.w = bmp.p_Bitmap->getWidth();
		bmp.rect.h = bmp.p_Bitmap->getHeight();
		m_Bitmaps.push_back(bmp);

		// Put the Tiles, of the parts that were collected
		if(!saving_mode)
		{
			for( int i=0 ; i<8 ; i++ )
			{
				if(m_Extra[i][0]) m_Map.setTile(98,5+i,221+(i%4), true);
				if(m_Extra[i][1]) m_Map.setTile(99,5+i,237+((i+1)%4), true);
				if(m_Extra[i][2]) m_Map.setTile(100,5+i,241+((i+2)%4), true);
				if(m_Extra[i][3]) m_Map.setTile(101,5+i,245+((i+3)%4), true);
			}
		}
	}
	else bmp.rect.y = 32;
	
	bmp.p_Bitmap = g_pGfxEngine->getBitmap("NAME");
	bmp.rect.x = (m_Episode == 3) ? 69 : 40;
	bmp.rect.w = bmp.p_Bitmap->getWidth();
	bmp.rect.h = bmp.p_Bitmap->getHeight();
	m_Bitmaps.push_back(bmp);
	
	bmp.p_Bitmap = g_pGfxEngine->getBitmap("SCORE");
	bmp.rect.x = (m_Episode == 3) ? 207 : 154;
	bmp.rect.w = bmp.p_Bitmap->getWidth();
	bmp.rect.h = bmp.p_Bitmap->getHeight();
	m_Bitmaps.push_back(bmp);
}

void CHighScores::process()
{
	SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;

	// draw the Bitmaps
	std::vector<stBitmap>::iterator it_bmp = m_Bitmaps.begin();
	for(; it_bmp != m_Bitmaps.end() ; it_bmp++)
	{
		it_bmp->p_Bitmap->draw(sfc, it_bmp->rect.x, it_bmp->rect.y);
	}

	// Print the text of the highscores
	for( Uint8 i=0 ; i<8 ; i++ )
	{
		int x = (m_Episode == 3) ? 69 : 40;
		int x2 = (m_Episode == 3) ? 255 : 202;
		int y = (m_Episode == 2) ? 56 : 52;
		g_pGfxEngine->getFont(0).drawFont(sfc, m_Name[i],x,y+(i<<4), true);
		g_pGfxEngine->getFont(0).drawFont(sfc, m_Score[i],x2-((m_Score[i].size())<<3),y+(i<<4), true);
	}

	// Here it must be split up into Episodes 1, 2 and 3.
	if(m_Episode == 2)
	{
		for( Uint8 i=0 ; i<8 ; i++ )
			g_pGfxEngine->getFont(0).drawFont(sfc, itoa(m_Cities[i]), 252, 56+(i<<4), true);
	}

	(this->*mp_process)();

	m_Map.animateAllTiles();
}

void CHighScores::processShow()
{
	if(g_pInput->getPressedAnyCommand())
		m_destroy_me=true;
}

void CHighScores::processWriting()
{
	SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;

	// Add or character depending on what the user writes
	for(int i=KA ; i<=KZ ; i++)
	{
		if (g_pInput->getHoldedKey(KSHIFT) && g_pInput->getPressedKey(i) && m_Name[m_Place].size() < 13)
			m_Name[m_Place].push_back('A' + i - KA);
		else if(g_pInput->getPressedKey(i) && m_Name[m_Place].size() < 13)
			m_Name[m_Place].push_back('a' + i - KA);
	}

	if(g_pInput->getPressedKey(KBCKSPCE) && (m_Name[m_Place].size() > 0))
	{
		m_Name[m_Place].erase(m_Name[m_Place].size()-1);
	}

	if(g_pInput->getPressedKey(KENTER))
	{
		// Save the Table and change to show mode, which can be closed by any other key
		saveHighScoreTable();
		mp_process = &CHighScores::processShow;
	}


	int x = (m_Episode == 3) ? 69 : 40;
	int y = (m_Episode == 2) ? 56 : 52;
	if(m_blink)	g_pGfxEngine->getFont(0).drawFont(sfc, m_Name[m_Place]+"_",x, y+(m_Place<<4), true);
	else g_pGfxEngine->getFont(0).drawFont(sfc, m_Name[m_Place]+" ",x, y+(m_Place<<4), true);

	if(m_blinkctr > BLINK_TIME){
		m_blinkctr = 0;
		m_blink = !m_blink;
	}
	else m_blinkctr++;
}

void CHighScores::writeEP1HighScore(int score, bool extra[4])
{
	writeHighScoreCommon(score);
	memcpy(m_Extra[m_Place], extra,4*sizeof(bool));

	// Set the tiles if EP1 (collected parts)
	if(m_Episode == 1)
	{
		// Put the Tiles, of the parts that were collected
		for( int i=0 ; i<8 ; i++ )
		{
			if(m_Extra[i][0]) m_Map.setTile(98,5+i,221, true);
			if(m_Extra[i][1]) m_Map.setTile(99,5+i,237, true);
			if(m_Extra[i][2]) m_Map.setTile(100,5+i,241, true);
			if(m_Extra[i][3]) m_Map.setTile(101,5+i,245, true);
		}
	}
}

void CHighScores::writeEP2HighScore(int score, int cities_saved)
{
	writeHighScoreCommon(score);
	m_Cities[m_Place] = cities_saved;
}

void CHighScores::writeHighScoreCommon(int score)
{
	m_Place=6;
	int num;

	num = atoi(m_Score[m_Place]);
	if(num > score)
		return; // Sorry, but score not high enough

	// Enough Points,
	// change the Highscore config to writing..
	mp_process = &CHighScores::processWriting;

	while(m_Place>0 && score > num)
	{
		m_Name[m_Place] = m_Name[m_Place-1];
		m_Score[m_Place] = m_Score[m_Place-1];
		memcpy(m_Extra[m_Place], m_Extra[m_Place-1], 4);
		m_Cities[m_Place] = m_Cities[m_Place-1];
		m_Place--;

		if(m_Place > 0)
			num = atoi(m_Score[m_Place-1]);
	}

	m_Score[m_Place] = itoa(score);

	m_Name[m_Place+1] = m_Name[m_Place];

	if(score < 999999)
		m_Score[m_Place] = itoa(score);
	else
		m_Score[m_Place] = "*MAX*";

	m_Name[m_Place] = "";
}

bool CHighScores::loadHighScoreTable()
{
	ifstream ScoreTableFile;
	std::string buf = "highscoreep"+itoa(m_Episode)+".dat";
	OpenGameFileR(ScoreTableFile, buf, ios::binary);

	if(!ScoreTableFile) return false;

	char c_name[8][16];
	char c_score[8][8];
	unsigned char c_extra[8][4];
	int c_cities[8];

	memset(c_name, 0, 8*16*sizeof(char));
	memset(c_score, 0, 8*8*sizeof(char));
	memset(c_extra, 0, 8*4*sizeof(char));
	memset(c_cities, 0, 8*sizeof(char));

	ScoreTableFile.read((char*)c_name, 8*16*sizeof(char));
	ScoreTableFile.seekg(8*16*sizeof(char),ios_base::cur);
	ScoreTableFile.read((char*)c_score, 8*8*sizeof(char));
	ScoreTableFile.seekg(8*8*sizeof(char),ios_base::cur);
	ScoreTableFile.read((char*)(c_extra), 8*4*sizeof(unsigned char));
	ScoreTableFile.seekg(8*4*sizeof(char),ios_base::cur);
	ScoreTableFile.read((char*)(c_cities), 8*sizeof(int));

	// Format the name to C++ Strings
	for(size_t i=0 ; i<8 ; i++)
	{
		m_Name[i] = c_name[i];
		m_Score[i] = c_score[i];
		memcpy(&m_Extra[i], &c_extra[i], 4);
		m_Cities[i] = c_cities[i];
	}

	ScoreTableFile.close();

	return true;
}

bool CHighScores::saveHighScoreTable()
{
	ofstream ScoreTableFile;
	std::string buf = "highscoreep"+itoa(m_Episode)+".dat";
	OpenGameFileW(ScoreTableFile, buf, ios::binary);

	if(!ScoreTableFile) return false;

	char c_name[8][16];
	char c_score[8][8];
	unsigned char c_extra[8][4];
	int c_cities[8];

	memset(c_name, 0, 8*16*sizeof(char));
	memset(c_score, 0, 8*8*sizeof(char));
	memset(c_extra, 0, 8*4*sizeof(char));
	memset(c_cities, 0, 8*sizeof(char));

	// Format the name to C++ Strings
	for(size_t i=0 ; i<8 ; i++)
	{
		memcpy(&c_name[i], m_Name[i].c_str(), 16);
		memcpy(&c_score[i], m_Score[i].c_str(), 8);
		memcpy(&c_extra[i], m_Extra[i], 4);
		c_cities[i] = m_Cities[i];
	}

	ScoreTableFile.write((char*)(c_name), 8*16*sizeof(char));
	ScoreTableFile.seekp(8*16*sizeof(char),ios_base::cur);
	ScoreTableFile.write((char*)(c_score), 8*8*sizeof(char));
	ScoreTableFile.seekp(8*8*sizeof(char),ios_base::cur);
	ScoreTableFile.write((char*)(c_extra), 8*4*sizeof(unsigned char));
	ScoreTableFile.seekp(8*4*sizeof(char),ios_base::cur);
	ScoreTableFile.write((char*) c_cities, 8*sizeof(int));

	ScoreTableFile.close();

	return true;
}

CHighScores::~CHighScores() {
}

