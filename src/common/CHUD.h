/*
 * CHUD.h
 *
 *  Created on: 01.07.2010
 *      Author: gerstrong
 *
 *  This is the class which process the code of the HUD.
 *  It is meant to be used, so both Vorticon and Galaxy Engine
 *  or even another game can use it.
 */

#ifndef CHUD_H_
#define CHUD_H_

#include <SDL.h>
#include "Playerdefines.h"
#include "../engine/spritedefines.h"

class CHUD {
public:
	CHUD(unsigned long &score, signed char &lives, unsigned int &charges);

	void render();

	virtual ~CHUD();

private:

	void CreateBackground();
	void DrawCircle(int x, int y, int width);

	SDL_Rect m_Rect;

	unsigned long &m_score;
	signed char &m_lives;
	unsigned int &m_charges;

	SDL_Surface *mp_Background;
};

#endif /* CHUD_H_ */
