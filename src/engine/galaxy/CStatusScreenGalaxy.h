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
#include "ItemsGalaxy.h"


class CStatusScreenGalaxy {
public:
	CStatusScreenGalaxy(const stItemGalaxy& Item);
	~CStatusScreenGalaxy();

	// This will generate the status screen. It must be derived by other classes, depending on the Episode
	virtual void GenerateStatus() = 0;

	// Generate the base for the Status screen. This function is common for all Keen4-6 galaxy games
	// and start_x and start_y will return the start offset pixel where particular stuff can be drawn
	void drawBase(SDL_Rect &EditRect);

	// Draw the status
	void draw();

	bool m_showstatus;
	SDL_Surface *mp_StatusSurface;
	const stItemGalaxy& m_Item;
};


#endif /* CSTATUSSCREENGALAXY_H_ */
