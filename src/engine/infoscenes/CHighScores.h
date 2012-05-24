/*
 * CHighScores.h
 *
 *  Created on: 09.04.2009
 *      Author: gerstrong
 */

#ifndef CHIGHSCORES_H_
#define CHIGHSCORES_H_

#include "common/CMap.h"
#include "graphics/CBitmap.h"
#include "CInfoScene.h"
#include "SmartPointer.h"
#include <string>
#include <vector>

struct stBitmap
{
	CBitmap *p_Bitmap;
	SDL_Rect rect;
};

class CHighScores : public CInfoScene
{
public:

	CHighScores();

	void fetchScoreTable();

	void init();
	void teardown();

	void writeEP1HighScore(int score, bool extra[4]);
	void writeEP2HighScore(int score, int cities_saved);
	void writeHighScoreCommon(int score);
	
	void process();

private:
	SmartPointer<CMap> mpMap;

	std::string		m_Name[16];
	std::string		m_Score[8];
	bool			m_Extra[8][4];
	int				m_Cities[8];
	int				m_Place;
	int				m_CursorPosition;
	
	char					m_Episode;
	char					m_CurrentLetter;
	std::string				m_DataDirectory;
	std::vector<stBitmap>	m_Bitmaps;

	SmartPointer<SDL_Surface> mpTextSfc;

	// This cycle will wait for the input of name and hit of enter
	bool m_blink;
	int m_blinkctr;
	bool mTableLoaded;

	void (CHighScores::*mp_process)();
	
	void processShowing();
	void processWriting();

	bool saveHighScoreTable();
	bool loadHighScoreTable();
};

#endif /* CHIGHSCORES_H_ */
