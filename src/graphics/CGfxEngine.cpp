/*
 * CGfxEngine.cpp
 *
 *  Created on: 26.08.2009
 *      Author: gerstrong
 */

#include "CGfxEngine.h"

CGfxEngine::CGfxEngine() {
	// TODO Auto-generated constructor stub

}

CGfxEngine::~CGfxEngine() {
	// TODO Auto-generated destructor stub
}

// Needed when the fade effect is called.
void CGfxEngine::setColorPalettes(SDL_Color *Palette)
{
	Font.setColorPalette(Palette);
}
