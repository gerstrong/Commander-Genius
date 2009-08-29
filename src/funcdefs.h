
#ifndef __CG_FUNCDEFS_H__
#define __CG_FUNCDEFS_H__

#include <string>

// gamepdowm.c
char wm_issolid(int xb, int yb, int *levels_completed);

// game.c
void overrun_detect(void);
void scrolltest(void);
void give_keycard(int doortile, int p);
void take_keycard(int doortile, int p);
void extralifeat(int p);
unsigned char spawn_object(int x, int y, int otype);
void common_enemy_ai(int o);
char hitdetect(int object1, int object2);
void freezeplayer(int theplayer);
void unregister_animtiles(int tile);
char checkobjsolid(unsigned int x, unsigned int y, unsigned int cp);
void CopyTileToSprite(int t, int s, int ntilestocopy, int transparent);
void GiveAnkh(int cp);
// map.c
void map_scroll_right(void);
void map_scroll_left(void);
void map_scroll_down(void);
void map_scroll_up(void);
void map_draw_vstripe(unsigned int x, unsigned int mpx);
void map_draw_hstripe(unsigned int y, unsigned int mpy);
void nosb_map_draw_vstripe(unsigned int x, unsigned int mapx);
unsigned int getmaptileat(unsigned int x, unsigned int y);
unsigned int getlevelat(unsigned int x, unsigned int y);
void drawmap(void);
void map_unregister_all_animtiles(void);
void map_deanimate(int x, int y);
int map_isanimated(int x, int y);
void map_animate(int x, int y);
char map_findobject(unsigned int obj, int *xout, int *yout);
char map_findtile(unsigned int tile, int *xout, int *yout);
void map_redraw(void);
void map_chgtile(unsigned int x,unsigned  int y, int newtile);
// dos\snddrv.c
void SoundDrv_Stop(void);
void sound_do(void);
void sound_play_stereo(int snd, char mode, short balance);
void sound_play_stereo_from_coord(int snd, char mode, unsigned int xcoordinate);
void sound_play(int snd, char mode);
char sound_is_playing(int snd);
void sound_stop(int snd);
void sound_stop_all(void);
void sound_pause(void);
void sound_resume(void);
char sound_extraction_of_exe_files(char *inputpath, int episode);

// graphics.c
void freemem(void);
void Graphics_Stop(void);
void /*inline*/ sb_setpixel(int x, int y, unsigned char c);
unsigned char sb_getpixel(int x, int y);
void drawtile(int x, int y, unsigned int t);
void drawtile_direct(int x, int y, unsigned int t);
void drawtilewithmask(int x, int y, unsigned int til, unsigned int tmask);
void drawprioritytile(int x, int y, unsigned int til);
void drawsprite_direct(int x, int y, unsigned int t);
void drawsprite(int x, int y, unsigned int s, int objectnum);
void erasesprite(int x, int y, unsigned int s, int objectnum);
void drawcharacter(int x, int y, int f);
void sb_drawcharacter(int x, int y, int f);
void sb_drawcharacterwithmask(int x, int y, int f, char mask);
void sb_drawcolorcharacter(int x, int y, int f, unsigned short colour, unsigned short bgcolour);
void sb_drawcharacterinverse(int x, int y, int f);
void save_area(int x1, int y1, int x2, int y2);
void restore_area(int x1, int y1, int x2, int y2);
void setvideomode(unsigned char mode);

// viddrv.c
#include "fileio.h"

bool loadtileattributes(int episode, int version, unsigned char *filebuf);
int freestrings(void);
std::string getstring(const std::string& name);
int GetStringAttribute(const std::string& stringName, const char *attrName);

// keydrv.c
char KeyDrv_Start(void);
void KeyDrv_Stop(void);

// misc.c
void banner(void);
void dialogbox(int x1, int y1, int w, int h);
void statusbox(void);

int savegameiswm(char *fname);
void usage(void);
void radar(void);
void SetAllCanSupportPlayer(int o, int state);
void AllPlayersInvisible(void);

// menu.c
int GetBitmapNumberFromName(const char *bmname);

// eseq_ep1.c
void addshipqueue(int cmd, int time, int flag1);

// lz.c
char lz_decompress(FILE *lzfile, unsigned char *outbuffer);

// finale.c
void finale_draw(const std::string& filename, const std::string& path);

#endif

