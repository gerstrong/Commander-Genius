/*
 * CFinaleStaticScene.h
 *
 *  Created on: 10.11.2009
 *      Author: gerstrong
 */

#ifndef CFINALESTATICSCENE_H_
#define CFINALESTATICSCENE_H_

#include "../../dialog/CTextBox.h"
#include "../../dialog/CDlgFrame.h"
#include <SDL/SDL.h>
#include <string>
#include <list>

// Prototype Reference to finale.cpp. This one still uses old C code.
void finale_draw( SDL_Surface *sfc, const std::string& filename, const std::string& path);

class CFinaleStaticScene {
public:
	CFinaleStaticScene(const std::string &game_path, const std::string &scene_file);

	void push_string(const std::string &text, Uint32 delay);
	void process();
	bool mustclose() { return m_mustclose; }

	virtual ~CFinaleStaticScene();

private:
	std::list<CTextBox*> mp_textbox_list;
	SDL_Surface *mp_SceneSurface;
	CDlgFrame *mp_DlgFrame;
	CTextBox *mp_current_tb;
	bool m_mustclose;
};

#endif /* CFINALESTATICSCENE_H_ */
