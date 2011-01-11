/*
 * CFinaleStaticScene.h
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALESTATICSCENE_H_
#define CFINALESTATICSCENE_H_

#include "../../../dialog/CMessageBox.h"
#include "../../../graphics/CBitmap.h"
#include <SDL.h>
#include <string>
#include <list>

struct bitmap_structure {
	CBitmap *p_bitmap;
	SDL_Rect dest_rect;
	Uint16 from_count;
	Uint16 to_count;
};

// Prototype Reference to finale.cpp. This one still uses old C code.
bool finale_draw( SDL_Surface *sfc, const std::string& filename, const std::string& path);

class CFinaleStaticScene {
public:
	CFinaleStaticScene(const std::string &game_path, const std::string &scene_file);

	void push_string(const std::string &text, Uint32 delay);
	void showBitmapAt(const std::string &bitmapname, Uint16 from_count, Uint16 to_count, Uint16 x, Uint16 y);
	void setTimer(Uint16 timer) {	m_timer = timer;	}
	void process();
	bool mustclose() { return m_mustclose; }

	virtual ~CFinaleStaticScene();

private:

	std::list<CMessageBox*> mp_textbox_list;
	std::vector<bitmap_structure> m_BitmapVector;
	SDL_Surface *mp_SceneSurface;
	CMessageBox *mp_current_tb;
	bool m_mustclose;
	Uint8 m_count;
	Uint16 m_timer; // Only used, if no Text to display is set. Example is the preview section
};

#endif /* CFINALESTATICSCENE_H_ */
