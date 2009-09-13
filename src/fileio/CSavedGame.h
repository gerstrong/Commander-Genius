/*
 * CSavedGame.h
 *
 *  Created on: 13.08.2009
 *      Author: gerstrong
 */

#ifndef CSAVEDGAME_H_
#define CSAVEDGAME_H_

#define uchar unsigned char

class CSavedGame {
public:
	CSavedGame(stLevelControl *lvlcontrol, stCloneKeenPlus *poutsideCKP);
	virtual ~CSavedGame();

	char save(int slot);
	char IsValidSaveGame(std::string fname);
	bool load(int slot);

private:
	void readHeader(FILE *fp, uchar *episode, uchar *level, uchar *lives, uchar *num_players);
	std::string		DataDirectory;

	stCloneKeenPlus *pCKP;
	stLevelControl *mp_levelcontrol;
};

#endif /* CSAVEDGAME_H_ */
