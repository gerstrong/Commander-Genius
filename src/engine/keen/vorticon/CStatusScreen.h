/*
 * CStatusScreen.h
 *
 *  Created on: 15.10.2009
 *      Author: gerstrong
 */

#ifndef CSTATUSSCREEN_H_
#define CSTATUSSCREEN_H_

#include <SDL.h>
#include "engine/core/inventory.h"
#include "graphics/GsSurface.h"

class CStatusScreen
{
public:
    CStatusScreen(char episode, stInventory *p_inventory, bool *p_level_completed, int &ankhtime, int baseframe, int varSprite);

	void draw();
	std::string fetchDifficultyText();
    void createInventorySfcEp1(const int varSpr);
    void createInventorySfcEp2(const int varSpr);
    void createInventorySfcEp3(const int varSpr);

	bool isClosed() {	return m_closed;	}
	void close() {	m_closing = true;	}

private:


	char m_episode;
	int m_baseframe;
	stInventory *mp_inventory;
    bool *mpLevelCompleted;
    int &m_ankhtime;

    bool m_closing = false;
    bool m_closed = false;

    GsSurface mStatusSfc;
	SDL_Rect m_StatusRect;
};

#endif /* CSTATUSSCREEN_H_ */
