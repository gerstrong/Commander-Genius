/*
 * CDlgFrame.h
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGFRAME_H_
#define CDLGFRAME_H_

#include "graphics/GsSurface.h"
#include "engine/core/CBehaviorEngine.h"


class CDlgFrame
{
public:
    CDlgFrame(const int x, const int y,
              const int w, const int h,
              const int tilewidth = 8, const int tileheight = 8);

    void draw(GsSurface &dst);

	/**
	 * \brief Resizes the dialog box to new dimensions
	 * \param width New width of the box
	 * \param height New height of the box
	 */
    void resize(const int width,
                const int height);

	/**
	 * \brief Sets the Position of dialog box
	 * \param x New x Position of the box
	 * \param y New y Position of the box
	 */
    void setPos(const int x,
                const int y);

	/**
	 * Getter for Tile width
	 */
	int TileWidth();

	int m_x;
	int m_y;
	int m_w;
	int m_h;

private:

	int m_8x8tileheight;
	int m_8x8tilewidth;

	EngineType m_theme;

    void drawVorticonFrame(GsSurface &dstSDL_Surface);
    void drawGalaxyFrame(GsSurface &dstSDL_Surface);

};

#endif /* CDLGFRAME_H_ */
