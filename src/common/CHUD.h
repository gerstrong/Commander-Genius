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
#include <functional>
#include <memory>
#include "Playerdefines.h"
#include "engine/spritedefines.h"
#include "graphics/CSprite.h"
#include "sdl/extensions.h"

#ifndef CHUD_H_
#define CHUD_H_


class CHUD
{
public:
	CHUD(unsigned long &score,
		 signed char &lives,
		 unsigned int &charges,
		 int *camlead = NULL); // TODO: Must be removed later and replaced by a reference

	/**
	 * \brief This part of the code will render the entire HUD
	 */
	void render(const int place, const int players);
	
	/**
	 * \brief This will sync some data in the HUD, especially the oldScore variable
	 */	
	void sync();

private:

	void CreateBackground();
	void renderGalaxy(const int place, const int players);
	void renderVorticon();
	void DrawCircle(int x, int y, int width);

	SDL_Rect m_Rect;

	unsigned long &m_score;
	signed char &m_lives;
	unsigned int &m_charges;
	
	unsigned long m_oldScore;
	unsigned int m_oldCharges;	
	
	CSprite *mpHUDBox;

	std::unique_ptr<SDL_Surface, SDL_Surface_Deleter> mpBackground;
	std::shared_ptr<SDL_Surface> mpHUDBlit;
	int *mpCamlead;
	
	int timer;
};

#endif /* CHUD_H_ */
