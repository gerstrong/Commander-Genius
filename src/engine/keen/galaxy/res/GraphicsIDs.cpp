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
    m_BitmapNameMap[0][74]  = "HELP_UPPERBORDER";
    m_BitmapNameMap[0][75]  = "HELP_LEFTBORDER";
    m_BitmapNameMap[0][76]  = "HELP_RIGHTBORDER";
    m_BitmapNameMap[0][77]  = "HELP_LOWERBORDERCONTROL";
    m_BitmapNameMap[0][78]  = "HELP_LOWERBORDER";
    m_BitmapNameMap[0][39]  = "HELPHAND";
	m_BitmapNameMap[0][97]  = "KEENSWATCH";
    m_BitmapNameMap[0][102] = "STARWARS";
	m_BitmapNameMap[0][103] = "TITLE";
	m_BitmapNameMap[0][105] = "KEENTALKING";
    m_BitmapNameMap[0][106] = "KEENTHUMBSUP";
    m_BitmapNameMap[0][109] = "KEENTHUMBSUPLOADING";
    m_BitmapNameMap[0][110] = "KEENTHUMBSUPLOADING1";
    m_BitmapNameMap[0][111] = "KEENTHUMBSUPLOADING2";
    m_BitmapNameMap[0][112] = "KEENTHUMBSUPLOADING3";
    m_BitmapNameMap[0][113] = "KEENTHUMBSUPLOADING4";
    m_BitmapNameMap[0][114] = "KEENTHUMBSUPLOADING5";
    m_SpriteNameMap[0][129] = "HUDBACKGROUND";

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

	/// Keen 5 Image IDs
	// Bitmaps
    m_BitmapNameMap[1][22]  = "HELP_UPPERBORDER";
    m_BitmapNameMap[1][23]  = "HELP_LEFTBORDER";
    m_BitmapNameMap[1][24]  = "HELP_RIGHTBORDER";
    m_BitmapNameMap[1][25]  = "HELP_LOWERBORDERCONTROL";
    m_BitmapNameMap[1][26]  = "HELP_LOWERBORDER";
    m_BitmapNameMap[1][18] = "HELPHAND";
    m_BitmapNameMap[1][81] = "STARWARS";
	m_BitmapNameMap[1][82] = "TITLE";
	m_BitmapNameMap[1][76] = "KEENSWATCH";
	m_BitmapNameMap[1][85] = "KEENTHUMBSUP";
    m_BitmapNameMap[1][86] = "KEENTHUMBSUPLOADING";
    m_BitmapNameMap[1][87] = "KEENTHUMBSUPLOADING1";
    m_BitmapNameMap[1][88] = "KEENTHUMBSUPLOADING2";
    m_BitmapNameMap[1][89] = "KEENTHUMBSUPLOADING3";
    m_BitmapNameMap[1][90] = "KEENTHUMBSUPLOADING4";
    m_BitmapNameMap[1][91] = "KEENTHUMBSUPLOADING5";

	
	// Sprites
	m_SpriteNameMap[1][133] = "HUDBACKGROUND";

	/// Keen 6 Image IDs
	// Bitmaps
    m_BitmapNameMap[2][27] = "STARWARS";
	m_BitmapNameMap[2][28] = "TITLE";
	m_BitmapNameMap[2][20] = "KEENSWATCH";
	m_BitmapNameMap[2][29] = "KEENTALKING";
	m_BitmapNameMap[2][30] = "KEENTHUMBSUP";
    m_BitmapNameMap[2][31] = "KEENTHUMBSUPLOADING";
    m_BitmapNameMap[2][32] = "KEENTHUMBSUPLOADING1";
    m_BitmapNameMap[2][33] = "KEENTHUMBSUPLOADING2";
    m_BitmapNameMap[2][34] = "KEENTHUMBSUPLOADING3";
    m_BitmapNameMap[2][35] = "KEENTHUMBSUPLOADING4";
    m_BitmapNameMap[2][36] = "KEENTHUMBSUPLOADING5";

    m_BitmapNameMap[2][0] = "HELPHAND";
	
	// Sprites
	m_SpriteNameMap[2][129] = "HUDBACKGROUND";

	/// Keen 6 demo Image IDs
	// Bitmaps
    m_BitmapNameMap[3][22] = "STARWARS";
	m_BitmapNameMap[3][23] = "TITLE";
	m_BitmapNameMap[3][15] = "KEENSWATCH";
	m_BitmapNameMap[3][24] = "KEENTALKING";
	m_BitmapNameMap[3][25] = "KEENTHUMBSUP";
    m_BitmapNameMap[3][26] = "KEENTHUMBSUPLOADING";
    m_BitmapNameMap[3][27] = "KEENTHUMBSUPLOADING1";
    m_BitmapNameMap[3][28] = "KEENTHUMBSUPLOADING2";
    m_BitmapNameMap[3][29] = "KEENTHUMBSUPLOADING3";
    m_BitmapNameMap[3][30] = "KEENTHUMBSUPLOADING4";
    m_BitmapNameMap[3][31] = "KEENTHUMBSUPLOADING5";

    // The following is not right for k6demo, but here as a placeholder:
    m_BitmapNameMap[3][0] = "HELPHAND";
	
	// Sprites
	m_SpriteNameMap[3][129] = "HUDBACKGROUND";

	// Menu Labels
	m_BitmapNameMap[1][61] = "MAINMENULABEL";
	m_BitmapNameMap[1][62] = "NEWGAMELABEL";
	m_BitmapNameMap[1][63] = "LOADMENULABEL";
	m_BitmapNameMap[1][64] = "SAVEMENULABEL";
	m_BitmapNameMap[1][65] = "CONFIGMENULABEL";
	m_BitmapNameMap[1][66] = "SNDEFFMENULABEL";
	m_BitmapNameMap[1][67] = "MUSICMENULABEL";
	m_BitmapNameMap[1][68] = "KEYBMENULABEL";
	m_BitmapNameMap[1][69] = "MOVEMENULABEL";
	m_BitmapNameMap[1][70] = "BUTTONMENULABEL";
	m_BitmapNameMap[1][71] = "JOYMENULABEL";
	m_BitmapNameMap[1][72] = "OPTIONSMENULABEL";
	m_BitmapNameMap[1][73] = "PADDLELABEL";
	m_BitmapNameMap[1][74] = "QUITLABEL";

	// Menu Labels
	m_BitmapNameMap[2][5]  = "MAINMENULABEL";
	m_BitmapNameMap[2][6]  = "NEWGAMELABEL";
	m_BitmapNameMap[2][7]  = "LOADMENULABEL";
	m_BitmapNameMap[2][8]  = "SAVEMENULABEL";
	m_BitmapNameMap[2][9]  = "CONFIGMENULABEL";
	m_BitmapNameMap[2][10] = "SNDEFFMENULABEL";
	m_BitmapNameMap[2][11] = "MUSICMENULABEL";
	m_BitmapNameMap[2][12] = "KEYBMENULABEL";
	m_BitmapNameMap[2][13] = "MOVEMENULABEL";
	m_BitmapNameMap[2][14] = "BUTTONMENULABEL";
	m_BitmapNameMap[2][15] = "JOYMENULABEL";
	m_BitmapNameMap[2][16] = "OPTIONSMENULABEL";
	m_BitmapNameMap[2][17] = "PADDLELABEL";
	m_BitmapNameMap[2][18] = "QUITLABEL";

	// Menu Labels (demo)
	m_BitmapNameMap[3][0]  = "MAINMENULABEL";
	m_BitmapNameMap[3][1]  = "NEWGAMELABEL";
	m_BitmapNameMap[3][2]  = "LOADMENULABEL";
	m_BitmapNameMap[3][3]  = "SAVEMENULABEL";
	m_BitmapNameMap[3][4]  = "CONFIGMENULABEL";
	m_BitmapNameMap[3][5]  = "SNDEFFMENULABEL";
	m_BitmapNameMap[3][6]  = "MUSICMENULABEL";
	m_BitmapNameMap[3][7]  = "KEYBMENULABEL";
	m_BitmapNameMap[3][8]  = "MOVEMENULABEL";
	m_BitmapNameMap[3][9]  = "BUTTONMENULABEL";
	m_BitmapNameMap[3][10] = "JOYMENULABEL";
	m_BitmapNameMap[3][11] = "OPTIONSMENULABEL";
	m_BitmapNameMap[3][12] = "PADDLELABEL";
	m_BitmapNameMap[3][13] = "QUITLABEL";

}

}
