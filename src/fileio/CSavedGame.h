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
	CSavedGame(stLevelControl *lvlcontrol);
	virtual ~CSavedGame();

	char save(int slot);
	char IsValidSaveGame(char *fname);
	bool load(int slot);
	void deleteSaveGame(int slot);

private:
	void readHeader(FILE *fp, uchar *episode, uchar *level, uchar *lives, uchar *num_players);

	stLevelControl *mp_levelcontrol;
};

#endif /* CSAVEDGAME_H_ */
