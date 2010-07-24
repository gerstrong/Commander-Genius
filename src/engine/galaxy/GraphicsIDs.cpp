/*
 * GraphicsIDs.h
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 *
 *  This file holds the ID strings of the Graphics used in order to
 *  draw them in the game. This is the Galaxy Engine, beware!
 */

#include "CEGAGraphicsGalaxy.h"

namespace galaxy
{

void CEGAGraphicsGalaxy::createBitmapsIDs()
{
	/// Set the bitmap names

	// Keen 4 Bitmap IDs
	m_BitmapNameMap[0][103] = "TITLE";
	m_BitmapNameMap[0][97] = "KEENSWATCH";

	// Menu Labels
	m_BitmapNameMap[0][82] = "MAINMENULABEL";
	m_BitmapNameMap[0][83] = "NEWGAMELABEL";
	m_BitmapNameMap[0][84] = "LOADMENULABEL";
	m_BitmapNameMap[0][85] = "SAVEMENULABEL";
	m_BitmapNameMap[0][86] = "CONFIGMENULABEL";
	m_BitmapNameMap[0][87] = "SNDEFFMENULABEL";
	m_BitmapNameMap[0][88] = "MUSICMENULABEL";
	m_BitmapNameMap[0][89] = "KEYBMENULABEL";
	m_BitmapNameMap[0][90] = "MOVEMENULABEL";
	m_BitmapNameMap[0][91] = "BUTTONMENULABEL";
	m_BitmapNameMap[0][92] = "JOYMENULABEL";
	m_BitmapNameMap[0][93] = "OPTIONSMENULABEL";
	m_BitmapNameMap[0][94] = "PADDLELABEL";
	m_BitmapNameMap[0][95] = "QUITLABEL";

}

}
