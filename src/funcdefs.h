
#ifndef __CG_FUNCDEFS_H__
#define __CG_FUNCDEFS_H__

#include <string>
#include <SDL.h>

// game.c
/*void freezeplayer(int theplayer);

// map.c
char map_findtile(unsigned int tile, int *xout, int *yout);
void map_redraw(void);*/

// dos\snddrv.c
/*void sound_play_stereo(int snd, char mode, short balance);
void sound_play_stereo_from_coord(int snd, char mode, unsigned int xcoordinate);
void sound_play(int snd, char mode);
void sound_stop(int snd);
void sound_stop_all(void);
void sound_pause(void);
void sound_resume(void);*/

// viddrv.c
//#include "fileio.h"

//bool loadtileattributes(int episode, int version, unsigned char *filebuf);
//int freestrings(void);
std::string getstring(const std::string& name);
//int GetStringAttribute(const std::string& stringName, const char *attrName);

void usage(void);
//void SetAllCanSupportPlayer(int o, int state);
//void AllPlayersInvisible(void);

// lz.c
char lz_decompress(FILE *lzfile, unsigned char *outbuffer);

#endif

