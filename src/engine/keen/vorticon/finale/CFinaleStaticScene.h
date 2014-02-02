/*
 * CFinaleStaticScene.h
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALESTATICSCENE_H_
#define CFINALESTATICSCENE_H_

#include "../dialog/CMessageBoxVort.h"
#include "graphics/GsBitmap.h"
#include <SDL.h>
#include <string>
#include <list>
#include <memory>

struct bitmap_structure
{
    GsBitmap bitmap;
	SDL_Rect dest_rect;
	Uint16 from_count;
	Uint16 to_count;
};

// Prototype Reference to finale.cpp. This one still uses old C code.
bool finale_draw( SDL_Surface *sfc, const std::string& filename, const std::string& path);

class CFinaleStaticScene
{
public:
	CFinaleStaticScene(const std::string &game_path, const std::string &scene_file);

	void showBitmapAt(const std::string &bitmapname, Uint16 from_count, Uint16 to_count, Uint16 x, Uint16 y);
	void setTimer(Uint16 timer) {	m_timer = timer;	}
    void ponder();
    void render(const int step);
    bool mustclose() { return m_mustclose; }

private:

	std::vector<bitmap_structure> m_BitmapVector;
	std::shared_ptr<SDL_Surface> mpSceneSurface;
	bool m_mustclose;    
    int m_timer; // Only used, if no Text to display is set. Example is the preview section
};

#endif /* CFINALESTATICSCENE_H_ */
