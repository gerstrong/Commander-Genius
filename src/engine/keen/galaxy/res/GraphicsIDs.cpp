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
    auto &keen4bmpName = m_BitmapNameMap[0];
    auto &keen4sprName = m_SpriteNameMap[0];

    keen4bmpName[74]  = "HELP_UPPERBORDER";
    keen4bmpName[75]  = "HELP_LEFTBORDER";
    keen4bmpName[76]  = "HELP_RIGHTBORDER";
    keen4bmpName[77]  = "HELP_LOWERBORDERCONTROL";
    keen4bmpName[78]  = "HELP_LOWERBORDER";
    keen4bmpName[39]  = "HELPHAND";
    keen4bmpName[97]  = "KEENSWATCH";
    keen4bmpName[102] = "STARWARS";
    keen4bmpName[103] = "TITLE";
    keen4bmpName[105] = "KEENTALKING";
    keen4bmpName[106] = "KEENTHUMBSUP";
    keen4bmpName[109] = "KEENTHUMBSUPLOADING";
    keen4bmpName[110] = "KEENTHUMBSUPLOADING1";
    keen4bmpName[111] = "KEENTHUMBSUPLOADING2";
    keen4bmpName[112] = "KEENTHUMBSUPLOADING3";
    keen4bmpName[113] = "KEENTHUMBSUPLOADING4";
    keen4bmpName[114] = "KEENTHUMBSUPLOADING5";
    keen4sprName[129] = "HUDBACKGROUND";

	// Menu Labels
    keen4bmpName[82] = "MAINMENULABEL";
    keen4bmpName[83] = "NEWGAMELABEL";
    keen4bmpName[84] = "LOADMENULABEL";
    keen4bmpName[85] = "SAVEMENULABEL";
    keen4bmpName[86] = "CONFIGMENULABEL";
    keen4bmpName[87] = "SNDEFFMENULABEL";
    keen4bmpName[88] = "MUSICMENULABEL";
    keen4bmpName[89] = "KEYBMENULABEL";
    keen4bmpName[90] = "MOVEMENULABEL";
    keen4bmpName[91] = "BUTTONMENULABEL";
    keen4bmpName[92] = "JOYMENULABEL";
    keen4bmpName[93] = "OPTIONSMENULABEL";
    keen4bmpName[94] = "PADDLELABEL";
    keen4bmpName[95] = "QUITLABEL";

	/// Keen 5 Image IDs
	// Bitmaps
    auto &keen5bmpName = m_BitmapNameMap[1];
    auto &keen5sprName = m_SpriteNameMap[1];

    keen5bmpName[22]  = "HELP_UPPERBORDER";
    keen5bmpName[23]  = "HELP_LEFTBORDER";
    keen5bmpName[24]  = "HELP_RIGHTBORDER";
    keen5bmpName[25]  = "HELP_LOWERBORDERCONTROL";
    keen5bmpName[26]  = "HELP_LOWERBORDER";
    keen5bmpName[18] = "HELPHAND";
    keen5bmpName[81] = "STARWARS";
    keen5bmpName[82] = "TITLE";
    keen5bmpName[76] = "KEENSWATCH";
    keen5bmpName[85] = "KEENTHUMBSUP";
    keen5bmpName[86] = "KEENTHUMBSUPLOADING";
    keen5bmpName[87] = "KEENTHUMBSUPLOADING1";
    keen5bmpName[88] = "KEENTHUMBSUPLOADING2";
    keen5bmpName[89] = "KEENTHUMBSUPLOADING3";
    keen5bmpName[90] = "KEENTHUMBSUPLOADING4";
    keen5bmpName[91] = "KEENTHUMBSUPLOADING5";

	
	// Sprites
    keen5sprName[133] = "HUDBACKGROUND";

	/// Keen 6 Image IDs
	// Bitmaps
    auto setBmpNames = [](std::unordered_map<int, std::string> &name)
    {
        name[0] = "HELPHAND";

        // Menu Labels
        name[5]  = "MAINMENULABEL";
        name[6]  = "NEWGAMELABEL";
        name[8]  = "SAVEMENULABEL";
        name[9]  = "CONFIGMENULABEL";
        name[10] = "SNDEFFMENULABEL";
        name[11] = "MUSICMENULABEL";
        name[12] = "KEYBMENULABEL";
        name[7]  = "LOADMENULABEL";
        name[13] = "MOVEMENULABEL";
        name[14] = "BUTTONMENULABEL";
        name[15] = "JOYMENULABEL";
        name[16] = "OPTIONSMENULABEL";
        name[17] = "PADDLELABEL";
        name[18] = "QUITLABEL";

        name[27] = "STARWARS";
        name[28] = "TITLE";
        name[20] = "KEENSWATCH";
        name[29] = "KEENTALKING";
        name[30] = "KEENTHUMBSUP";
        name[31] = "KEENTHUMBSUPLOADING";
        name[32] = "KEENTHUMBSUPLOADING1";
        name[33] = "KEENTHUMBSUPLOADING2";
        name[34] = "KEENTHUMBSUPLOADING3";
        name[35] = "KEENTHUMBSUPLOADING4";
        name[36] = "KEENTHUMBSUPLOADING5";
    };
    setBmpNames(m_BitmapNameMap[2]);
    setBmpNames(m_BitmapNameMap[5]);

	
	// Sprites
    auto setSprNames = [](std::unordered_map<int, std::string> &name)
    {
        name[129] = "HUDBACKGROUND";
    };
    setSprNames(m_SpriteNameMap[2]);
    setSprNames(m_SpriteNameMap[5]);

	/// Keen 6 demo Image IDs
	// Bitmaps
    auto &keen6DemobmpName = m_BitmapNameMap[3];
    keen6DemobmpName[22] = "STARWARS";
    keen6DemobmpName[23] = "TITLE";
    keen6DemobmpName[15] = "KEENSWATCH";
    keen6DemobmpName[24] = "KEENTALKING";
    keen6DemobmpName[25] = "KEENTHUMBSUP";
    keen6DemobmpName[26] = "KEENTHUMBSUPLOADING";
    keen6DemobmpName[27] = "KEENTHUMBSUPLOADING1";
    keen6DemobmpName[28] = "KEENTHUMBSUPLOADING2";
    keen6DemobmpName[29] = "KEENTHUMBSUPLOADING3";
    keen6DemobmpName[30] = "KEENTHUMBSUPLOADING4";
    keen6DemobmpName[31] = "KEENTHUMBSUPLOADING5";

    // The following is not right for k6demo, but here as a placeholder:
    keen6DemobmpName[0] = "HELPHAND";
	
	// Sprites
    auto &keen6DemosprName = m_SpriteNameMap[3];
    keen6DemosprName[129] = "HUDBACKGROUND";

	// Menu Labels
    keen5bmpName[61] = "MAINMENULABEL";
    keen5bmpName[62] = "NEWGAMELABEL";
    keen5bmpName[63] = "LOADMENULABEL";
    keen5bmpName[64] = "SAVEMENULABEL";
    keen5bmpName[65] = "CONFIGMENULABEL";
    keen5bmpName[66] = "SNDEFFMENULABEL";
    keen5bmpName[67] = "MUSICMENULABEL";
    keen5bmpName[68] = "KEYBMENULABEL";
    keen5bmpName[69] = "MOVEMENULABEL";
    keen5bmpName[70] = "BUTTONMENULABEL";
    keen5bmpName[71] = "JOYMENULABEL";
    keen5bmpName[72] = "OPTIONSMENULABEL";
    keen5bmpName[73] = "PADDLELABEL";
    keen5bmpName[74] = "QUITLABEL";


	// Menu Labels (demo)
    keen6DemobmpName[0]  = "MAINMENULABEL";
    keen6DemobmpName[1]  = "NEWGAMELABEL";
    keen6DemobmpName[2]  = "LOADMENULABEL";
    keen6DemobmpName[3]  = "SAVEMENULABEL";
    keen6DemobmpName[4]  = "CONFIGMENULABEL";
    keen6DemobmpName[5]  = "SNDEFFMENULABEL";
    keen6DemobmpName[6]  = "MUSICMENULABEL";
    keen6DemobmpName[7]  = "KEYBMENULABEL";
    keen6DemobmpName[8]  = "MOVEMENULABEL";
    keen6DemobmpName[9]  = "BUTTONMENULABEL";
    keen6DemobmpName[10] = "JOYMENULABEL";
    keen6DemobmpName[11] = "OPTIONSMENULABEL";
    keen6DemobmpName[12] = "PADDLELABEL";
    keen6DemobmpName[13] = "QUITLABEL";

}

}
