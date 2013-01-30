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
#include "engine/galaxy/res/ItemsGalaxy.h"

class CStatusScreenGalaxy
{
public:
	CStatusScreenGalaxy(const stItemGalaxy& Item, const std::string &LevelName);
	virtual ~CStatusScreenGalaxy() {}

	// This will generate the status screen. It must be derived by other classes, depending on the Episode
	virtual void GenerateStatus() = 0;

	// Generate the base for the Status screen. This function is common for all Keen4-6 galaxy games
	// and start_x and start_y will return the start offset pixel where particular stuff can be drawn
	void drawBase(SDL_Rect &EditRect);

	// return the pointer to the allocated status screen surface
	SDL_Surface *getStatusSfc()
	{	return	mpStatusSurface.get();	}

	// Draw the status
	void draw();

	bool m_showstatus;
	std::shared_ptr<SDL_Surface> mpStatusSurface;
	const stItemGalaxy& m_Item;
	const std::string &m_LevelName;
};


#endif /* CSTATUSSCREENGALAXY_H_ */
