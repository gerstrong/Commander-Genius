/*
 * CStatusScreenGalaxy.h
 *
 *  Created on: 05.03.2011
 *      Author: gerstrong
 */

#ifndef CSTATUSSCREENGALAXY_H_
#define CSTATUSSCREENGALAXY_H_

#include <SDL.h>

class CStatusScreenGalaxy {
public:
	CStatusScreenGalaxy();
	~CStatusScreenGalaxy();

	// Generate the base for the Status screen. This function is common for all Keen4-6 galaxy games
	// and start_x and start_y will return the start offset pixel where particular stuff can be drawn
	void drawBase(SDL_Rect &EditRect);

	// Generation of the surface for blitting the statusscreen
	void generateStatusEP4();

	// Draw the status
	void draw();

	bool m_showstatus;
	SDL_Surface *mp_StatusSurface;
};


#endif /* CSTATUSSCREENGALAXY_H_ */
