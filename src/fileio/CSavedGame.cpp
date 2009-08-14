/*
 * CSavedGame.cpp
 *
 *  Created on: 13.08.2009
 *      Author:      Caitlin Shaw
 *      Adaptation : Gerstrong
 *  Routines for handling save&load of savegames
 */

#include "../keen.h"
#include "../externals.h"
#include "../include/game.h"
#include "../CLogFile.h"
#include "../FindFile.h"
#include "CSavedGame.h"
#include <string>

#define SG_HEADERSIZE			7

void sgrle_initdecompression(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
char sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes);

void initgame(stLevelControl *p_levelcontrol);

CSavedGame::CSavedGame(stLevelControl *lvlcontrol) {
	// TODO Auto-generated constructor stub
	mp_levelcontrol = lvlcontrol;
}

CSavedGame::~CSavedGame() {
	// TODO Auto-generated destructor stub
}

char CSavedGame::save(int slot)
{
FILE *fp;
std::string fname="savegame" + itoa(slot) + ".dat";

	// can't save game under certain circumstances
	//if (fade_in_progress()) return 1;

	g_pLogFile->textOut("Saving game...\n");
	fp = OpenGameFile(fname, "wb");
	if (!fp) return 1;

	fprintf(fp, "CKSAVE%c", SAVEGAMEVERSION);
	fputc(mp_levelcontrol->episode, fp);
	fputc(mp_levelcontrol->curlevel, fp);
	fputc(player[0].inventory.lives, fp);
	fputc(numplayers, fp);
	fputc(primaryplayer, fp);

	sgrle_compress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol));
	sgrle_compress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x));
	sgrle_compress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf));
	sgrle_compress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix));
	sgrle_compress(fp, (unsigned char *)&mapx, sizeof(mapx));
	sgrle_compress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos));

	sgrle_compress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y));
	sgrle_compress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf));
	sgrle_compress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy));
	sgrle_compress(fp, (unsigned char *)&mapy, sizeof(mapy));
	sgrle_compress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos));

	sgrle_compress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x));
	sgrle_compress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y));

	sgrle_compress(fp, (unsigned char *)&map, sizeof(map));

	fputi(highest_objslot, fp);
	sgrle_compress(fp, (unsigned char *)&objects[0], sizeof(objects));
	sgrle_compress(fp, (unsigned char *)&tiles[0], sizeof(tiles));

	for(unsigned i=0;i<numplayers;i++)
		sgrle_compress(fp, (unsigned char *)&player[i], sizeof(player[i]));

	// save state of partially-opened doors
	fputc(sprites[DOOR_YELLOW_SPRITE].ysize, fp);
	fputc(sprites[DOOR_RED_SPRITE].ysize, fp);
	fputc(sprites[DOOR_GREEN_SPRITE].ysize, fp);
	fputc(sprites[DOOR_BLUE_SPRITE].ysize, fp);

	fclose(fp);
	return 0;
}


char CSavedGame::IsValidSaveGame(char *fname)
{
FILE *fp;
unsigned int i;
const char *verify = "CKSAVE";
	fp = OpenGameFile(fname, "rb");
	if (!fp) return 0;

	for(i=0;i<strlen(verify);i++)
	{
		if (fgetc(fp) != verify[i])
		{
			fclose(fp);
			return 0;
		}
	}
	if (fgetc(fp) != SAVEGAMEVERSION)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

// this is seperated out of game_load for modularity because menumanager.c
// also uses it, in it's save-game "preview" menu on the load game screen
void CSavedGame::readHeader(FILE *fp, uchar *episode, uchar *level, uchar *lives, uchar *num_players)
{
	fseek(fp, SG_HEADERSIZE, SEEK_SET);		// skip past the CKSAVE%c
	*episode = fgetc(fp);
	*level = fgetc(fp);
	*lives = fgetc(fp);
	*num_players = fgetc(fp);
}

bool CSavedGame::load(int slot)
{
FILE *fp;
char fname[20];
unsigned char episode, level, lives;
unsigned int i;

	sprintf(fname, "savegame%d.dat", slot);

	if (!IsValidSaveGame(fname))
	{
		g_pLogFile->ftextOut("%s is not a valid save-game.\n", fname);
		return false;
	}

	g_pLogFile->ftextOut("Loading game from file %s\n", fname);
	fp = OpenGameFile(fname, "rb");
	if (!fp) { g_pLogFile->ftextOut("unable to open %s\n",fname); return 1; }

	readHeader(fp, &episode, &level, &lives, &numplayers);

	mp_levelcontrol->episode = episode;
	mp_levelcontrol->curlevel = level;
	player[0].inventory.lives = lives;

	g_pLogFile->ftextOut("game_load: restoring structures...\n");
	primaryplayer = fgetc(fp);

	sgrle_compress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol));

	// note that we don't have to load the LEVEL, because the state
	// of the map is already saved inside the save-game.
	sgrle_initdecompression();
	if (sgrle_decompress(fp, (unsigned char *) mp_levelcontrol, sizeof(*mp_levelcontrol))) return 1;

	if (sgrle_decompress(fp, (unsigned char *)&scroll_x, sizeof(scroll_x))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollx_buf, sizeof(scrollx_buf))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpix, sizeof(scrollpix))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapx, sizeof(mapx))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapxstripepos, sizeof(mapxstripepos))) return 1;

	if (sgrle_decompress(fp, (unsigned char *)&scroll_y, sizeof(scroll_y))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrolly_buf, sizeof(scrolly_buf))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&scrollpixy, sizeof(scrollpixy))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapy, sizeof(mapy))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&mapystripepos, sizeof(mapystripepos))) return 1;

	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_x, sizeof(max_scroll_x))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&max_scroll_y, sizeof(max_scroll_y))) return 1;

	if (sgrle_decompress(fp, (unsigned char *)&map, sizeof(map))) return 1;

	highest_objslot = fgeti(fp);
	if (sgrle_decompress(fp, (unsigned char *)&objects[0], sizeof(objects))) return 1;
	if (sgrle_decompress(fp, (unsigned char *)&tiles[0], sizeof(tiles))) return 1;

	for(i=0;i<numplayers;i++)
	{
		if (sgrle_decompress(fp, (unsigned char *)&player[i], sizeof(player[i]))) return 1;
	}

	sprites[DOOR_YELLOW_SPRITE].ysize = fgetc(fp);
	sprites[DOOR_RED_SPRITE].ysize = fgetc(fp);
	sprites[DOOR_GREEN_SPRITE].ysize = fgetc(fp);
	sprites[DOOR_BLUE_SPRITE].ysize = fgetc(fp);

	fclose(fp);

	g_pLogFile->ftextOut("Structures restored: map size: %d,%d\n", map.xsize, map.ysize);
	g_pLogFile->ftextOut("Load game OK\n");
	return true;
}

void CSavedGame::deleteSaveGame(int slot)
{
	char fname[20];

	sprintf(fname, "savegame%d.dat", slot);
	remove(fname);
}
