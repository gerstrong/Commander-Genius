/*
 * CHighScores.h
 *
 *  Created on: 09.04.2009
 *      Author: gerstrong
 */

#ifndef CHIGHSCORES_H_
#define CHIGHSCORES_H_

class CHighScores {
public:
	CHighScores(stCloneKeenPlus *poutsideCKP);
	virtual ~CHighScores();

	char showHighScore(void);
	char writeHighScore(int points, bool *extras, int cities);
	char loadHighScoreTable(void);

private:
	char 			Name[7][16];
	char 			Score[7][8];
	bool			Extra[7][4];
	int				Cities[7];
	int				ItemTiles[4];

	char			Episode;
	char			*DataDirectory;

	stCloneKeenPlus *pCKP;

	void writeName(char *name, int place);
	char saveHighScoreTable(void);

};

#endif /* CHIGHSCORES_H_ */
