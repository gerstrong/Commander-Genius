/*
 * CDlgFrame.h
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#ifndef CDLGFRAME_H_
#define CDLGFRAME_H_

#include <SDL.h>
#include "engine/core/CBehaviorEngine.h"


class CDlgFrame
{
public:
	CDlgFrame(int x, int y, int w, int h, int tilewidth = 8, int tileheight = 8);

	void draw(SDL_Surface *dst);

	/**
	 * \brief Resizes the dialog box to new dimensions
	 * \param width New width of the box
	 * \param height New height of the box
	 */
	void resize(const Uint32 width, const Uint32 height);

	/**
	 * \brief Sets the Position of dialog box
	 * \param x New x Position of the box
	 * \param y New y Position of the box
	 */
	void setPos(const Uint32 x, const Uint32 y);

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

	void drawVorticonFrame(SDL_Surface *dst);
	void drawGalaxyFrame(SDL_Surface *dst);

};

#endif /* CDLGFRAME_H_ */
