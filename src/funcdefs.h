
#ifndef __CG_FUNCDEFS_H__
#define __CG_FUNCDEFS_H__

#include <string>

// gamedo.c
int gamedo_ScrollTriggers(int theplayer);
void gamedo_AnimatedTiles(void);

void gamedo_render_eraseobjects(void);
void gamedo_render_drawdebug(void);
void gamedo_render_erasedebug(void);
void gamedo_fades(void);

// gamepdo.c


// gamepdowm.c
//void gamepdo_wm_HandlePlayer(int cp);
//void gamepdo_InertiaAndFriction_Y(int cp);
//void gamepdo_wm_AllowEnterLevel(int cp);
char wm_issolid(int xb, int yb, int *levels_completed);

// game.c
//void SetGameOver(void);
void overrun_detect(void);
void scrolltest(void);
//void gameloop_initialize(void);
void give_keycard(int doortile, int p);
void take_keycard(int doortile, int p);
//void open_door(int doortile, int doorsprite, int mpx, int mpy, int cp, stCloneKeenPlus *pCKP)
void extralifeat(int p);
char spawn_object(int x, int y, int otype);
void common_enemy_ai(int o);
char hitdetect(int object1, int object2);
void freezeplayer(int theplayer);
void unregister_animtiles(int tile);
//void endlevel(int success, stCloneKeenPlus *pCKP)
char checkobjsolid(unsigned int x, unsigned int y, unsigned int cp);
//void initsprites(stCloneKeenPlus *pCKP)
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
void DrawBitmap(int xa, int ya, int b);
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
void addpal(int c, int r, int g, int b);
void pal_init(int dark);
void pal_fade(int fadeamt);
void pal_apply(void);
void font_draw(unsigned char *text, int xoff, int yoff, int highlight);
void sb_font_draw(const unsigned char *text, int xoff, int yoff);
void sb_mask_font_draw(unsigned char *text, int xoff, int yoff, char mask);
void sb_color_font_draw(unsigned char *text, int xoff, int yoff, unsigned int colour, unsigned short bgcolour);
void sb_font_draw_inverse(unsigned char *text, int xoff, int yoff);
// viddrv.c
// fileio.c
//void addmaptile(unsigned int t, stCloneKeenPlus *pCKP);
//void addenemytile(unsigned int t, stCloneKeenPlus *pCKP);
//unsigned int fgeti(FILE *fp);
//unsigned long fgetl(FILE *fp);
//unsigned int loadmap(char *filename, char *path, int lvlnum, int isworldmap);
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
void sb_dialogbox(int x1, int y1, int w, int h);
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

// latch.c
//char latch_loadgraphics(int episode, char *path);

// sglre.c
void sgrle_reset(void);
void sgrle_compress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
void sgrle_decompress(FILE *fp, unsigned char *ptr, unsigned long nbytes);
unsigned char sgrle_get_next_byte(FILE *fp);

// lz.c
char lz_decompress(FILE *lzfile, unsigned char *outbuffer);

// finale.c
void finale_draw(const std::string& filename, const std::string& path);

// scalerx.c
//void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height);

#endif

