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
	void stopGraphics(void);
	void drawFont(const std::string& text, int xoff, int yoff, int highlight);
	void sb_font_draw(const std::string& text, int xoff, int yoff, bool useHighlightcolour=false);
	void sb_mask_font_draw(const std::string& text, int xoff, int yoff, char mask);
	void sb_color_font_draw(const std::string& text, int xoff, int yoff, unsigned int colour, unsigned short bgcolour);
	void setFadeBlack(bool value);

private:
	Uint32 scrollbuf_memsize;
	Uint32 blitbuf_memsize;

	unsigned char *blitbuffer;

	bool fade_black;
};

#endif /* CGRAPHICS_H_ */
