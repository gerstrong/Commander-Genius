/*
 * CPixelate.h
 *
 *  Created on: 09.03.2010
 *      Author: gerstrong
 */

#ifndef CPIXELATE_H_
#define CPIXELATE_H_

#include <base/video/GsEffectController.h>

#include <graphics/GsSurface.h>

class CPixelate : public CEffects
{
public:
    CPixelate(const float speed,
              const bool modern);
	void getSnapshot();
    void ponder(const float deltaT);

    void renderRetro();
    void renderModern();

    void render();

private:
    GsSurface mOldSurface;

	// m_line tells until which line, we have to draw the pixels
	// m_lines_completed gets ++, if a line has no more pixel that need to be drawn
	unsigned short m_line, m_lines_completed;

	// m_pixel_per_line tells at the given line how many pixels have already been drawn.
    //unsigned short *m_pixels_per_line;
    //bool *m_drawmap;
    std::vector<unsigned short> mPixelsPerLine;
    std::vector<bool> mDrawMap;
    float mSpeed;
	Uint32 mColorkey;

    bool mModernMode = true;
};

#endif /* CPIXELATE_H_ */
