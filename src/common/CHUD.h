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

#include <SDL.h>
#include <string>
#include "Playerdefines.h"
#include "engine/spritedefines.h"
#include "SmartPointer.h"
#include "graphics/CSprite.h"

#ifndef CHUD_H_
#define CHUD_H_

class CHUD {
public:
	CHUD(unsigned long &score,
		 signed char &lives,
		 unsigned int &charges,
		 int *camlead = NULL); // TODO: Must be removed later and replaced by a reference

	void render();

private:

	void CreateBackground();
	void renderGalaxy();
	void renderVorticon();
	void DrawCircle(int x, int y, int width);

	SDL_Rect m_Rect;

	unsigned long &m_score;
	signed char &m_lives;
	unsigned int &m_charges;
	CSprite *mpHUDBox;

	SmartPointer <SDL_Surface> mpBackground;
	SmartPointer <SDL_Surface> mpHUDBlit;
	int *mpCamlead;
};

#endif /* CHUD_H_ */
