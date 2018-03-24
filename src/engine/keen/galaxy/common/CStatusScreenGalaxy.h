/*
 * CStatusScreenGalaxy.h
 *
 *  Created on: 05.03.2011
 *      Author: gerstrong
 *
 *  Base class for the creation of the Status Screen
 */

#ifndef CSTATUSSCREENGALAXY_H_
#define CSTATUSSCREENGALAXY_H_

#include <SDL.h>
#include <string>
#include <memory>
#include <graphics/GsSurface.h>

#include "../res/ItemsGalaxy.h"

class CStatusScreenGalaxy
{
public:
    CStatusScreenGalaxy(const stItemGalaxy& Item);

	// This will generate the status screen. It must be derived by other classes, depending on the Episode
    void GenerateStatusEp4();
    void GenerateStatusEp5();
    void GenerateStatusEp6();


    // Returns the right text string to show if the currently setup difficulty
    std::string getDifficultyText();

    // Scales to the apropiate resolution
    void scaleToResolution();

	// Generate the base for the Status screen. This function is common for all Keen4-6 galaxy games
	// and start_x and start_y will return the start offset pixel where particular stuff can be drawn
    void drawBase(SDL_Rect &EditRect);

	// return the pointer to the allocated status screen surface
    /*SDL_Surface *getStatusSfc()
    {	return	mStatusSfcTransformed.getSDLSurface();	}*/

	// Draw the status
	void draw();

	bool m_showstatus;
    GsSurface mStatusSurface;

    GsSurface mStatusSfcTransformed;
	const stItemGalaxy& m_Item;
};


#endif /* CSTATUSSCREENGALAXY_H_ */
