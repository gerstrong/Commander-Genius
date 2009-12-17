/*
 * CHighScores.h
 *
 *  Created on: 09.04.2009
 *      Author: gerstrong
 */

#ifndef CHIGHSCORES_H_
#define CHIGHSCORES_H_

#include "../../common/CMap.h"
#include "../../graphics/CBitmap.h"
#include "CInfoScene.h"
#include <string>
#include <vector>

struct stBitmap{
	CBitmap *p_Bitmap;
	SDL_Rect rect;
};

class CHighScores : public CInfoScene {
public:

	CHighScores(int Episode, const std::string &DataDirectory);
	
	void process();

	virtual ~CHighScores();

private:
	CMap m_Map;

	std::string		m_Name[16];
	std::string		m_Score[8];
	bool			m_Extra[7][4];
	int				m_Cities[7];
	int				m_ItemTiles[4];
	int				m_Place;
	
	char					m_Episode;
	std::string				m_DataDirectory;
	std::vector<stBitmap>	m_Bitmaps;

	// This cycle will wait for the input of name and hit of enter
	bool m_blink;
	int m_blinkctr;

	void (CHighScores::*mp_process)();
	
	void processShow();
	void processWriting();

	void writeEP1HighScore(int score, bool extra[4]);
	void writeEP2HighScore(int score, int cities_saved);
	void writeHighScoreCommon(int score);
	void writeName(char *name, int place);
	bool saveHighScoreTable();
	bool loadHighScoreTable();
};

#endif /* CHIGHSCORES_H_ */
