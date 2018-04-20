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
#include <string>
#include <functional>
#include <memory>
#include <graphics/GsSprite.h>
#include <graphics/GsSurface.h>

#include "Playerdefines.h"
#include "engine/core/spritedefines.h"
#include "graphics/GsSprite.h"
#include <graphics/GsSurface.h>


class CHUD
{
public:
    CHUD(unsigned long &score,
         signed char &lives,
         signed int &charges,
         const int playerIdx, const int spriteVar); // TODO: Must be removed later and replaced by a reference


	//CHUD(const CHUD &orig);


	/**
	 * \brief This part of the code will render the entire HUD
	 */
    void render();

    void createHUDBlit();

    /**
      * \brief Sets up the HUD Box for certain id sprite variant
      */
    void setup(const int playerIdx, const int spriteVar);

	/**
	 * \brief This will sync some data in the HUD, especially the oldScore variable
	 */
	void sync();

private:

    void CreateVorticonBackground();
    void renderGalaxy();
	void renderVorticon();
	void DrawCircle(int x, int y, int width);

    SDL_Rect mRenderRect;

	unsigned long &m_score;
	signed char &mLives;
	signed int &m_charges;

	unsigned long mOldScore;
	signed int mOldCharges;

    GsSprite mHUDBox;

    GsSurface mBackground;

    GsSurface mHUDBlit;
    int mId;

	int timer;

    GsSprite mKeenHeadSprite;
    GsSprite mKeenGunSprite;
};

#endif /* CHUD_H_ */
