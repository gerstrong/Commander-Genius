/*
 * CGraphics.h
 *
 *  Created on: 03.05.2009
 *      Author: gerstrong
 */

#ifndef CGRAPHICS_H_
#define CGRAPHICS_H_

#include "CSingleton.h"
#define g_pGraphics CGraphics::Get()

#include <string>
#include <SDL.h>

class CGraphics : public CSingleton<CGraphics>
{
public:
	CGraphics();
	bool allocScrollBufmem(void);
	void freemem(void);
	void sb_setpixel(int x, int y, unsigned char c);
	unsigned char sb_getpixel(int x, int y);
	void drawSprite_direct(int x, int y, unsigned int t);
	void saveArea(int x1, int y1, int x2, int y2);
	void restoreArea(int x1, int y1, int x2, int y2);
	void stopGraphics(void);
	void drawBitmap(int xa, int ya, int b);
	void drawBitmap2FG(int xa, int ya, int b);
	int  getBitmapNumberFromName(const char *bmname);
	void drawFont(const std::string& text, int xoff, int yoff, int highlight);
	void sb_font_draw(const std::string& text, int xoff, int yoff, bool useHighlightcolour=false);
	void sb_mask_font_draw(const std::string& text, int xoff, int yoff, char mask);
	void sb_color_font_draw(const std::string& text, int xoff, int yoff, unsigned int colour, unsigned short bgcolour);
	void setFadeBlack(bool value);

	Uint8 *getScrollbuffer(void);

private:
	Uint32 scrollbuf_memsize;
	Uint32 blitbuf_memsize;

	unsigned char *scrollbuffer;
	unsigned char *blitbuffer;

	bool fade_black;
};

#endif /* CGRAPHICS_H_ */
