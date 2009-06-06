/*
 * fileio.h
 *
 *  Created on: 09.01.2009
 *      Author: gerstrong
 */

unsigned int loadmap(char filename[MAX_STRING_LENGTH], char *path, int lvlnum, int isworldmap, stCloneKeenPlus *pCKP);
void addmaptile(unsigned int t);
void addenemytile(unsigned int t, stCloneKeenPlus *pCKP);
short checkConsistencyofGameData(stGameData *p_GameData);
void formatPathString(char *output, const char *path);
