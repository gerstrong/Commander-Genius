/*
 * CMessageBoxVort.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#include "CMessageBoxVort.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "graphics/CGfxEngine.h"

CMessageBoxVort::CMessageBoxVort(const std::string& Text, bool lower, bool keymsg, bool leftbound) :
CMessageBox(Text, lower, keymsg, leftbound)
{}

// This function is used in your Ship need those parts.
// This special Messagebox can hold up to 4 images
void CMessageBoxVort::addTileAt(Uint16 tile, Uint16 x, Uint16 y)
{
	TileHolder tileholder;

	tileholder.tile = tile;
	tileholder.x = x;
	tileholder.y = y;

	m_Tiles.push_back(tileholder);

	// new CGUIImage(...); // TODO...

	//addControl( mpTextCtrl, TextRect );
}
