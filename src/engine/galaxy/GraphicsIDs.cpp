/*
 * GraphicsIDs.h
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 *
<<<<<<< HEAD
 *  This file holds the ID numbers of the Graphics used in order to
 *  draw them in the game. This is the Galaxy Engine, beware!
 */

#ifndef GRAPHICSIDS_H_
#define GRAPHICSIDS_H_

const int BMP_TITLE = 103;
const int BMP_KEENSWATCH = 97;

// Menu Labels
const int BMP_MAINMENULABEL = 82;
const int BMP_NEWGAMELABEL = 83;
const int BMP_LOADMENULABEL = 84;
const int BMP_SAVEMENULABEL = 85;
const int BMP_CONFIGMENULABEL = 86;
const int BMP_SNDEFFMENULABEL = 87;
const int BMP_MUSICMENULABEL = 88;
const int BMP_KEYBMENULABEL = 89;
const int BMP_MOVEMENULABEL = 90;
const int BMP_BUTTONMENULABEL = 91;
const int BMP_JOYMENULABEL = 92;
const int BMP_OPTIONSMENULABEL = 93;
const int BMP_PADDLELABEL = 94;
const int BMP_QUITLABEL = 95;

#endif /* GRAPHICSIDS_H_ */
=======
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
>>>>>>> 1123848... fixed enemy and shot speeds relative to keen as far as I could
