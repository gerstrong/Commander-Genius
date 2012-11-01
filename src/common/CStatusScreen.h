/*
 * CStatusScreen.h
 *
 *  Created on: 15.10.2009
 *      Author: gerstrong
 */

#ifndef CSTATUSSCREEN_H_
#define CSTATUSSCREEN_H_

#include <SDL.h>
#include "inventory.h"
#include <memory>

class CStatusScreen {
public:
	CStatusScreen
	(char episode, stInventory *p_inventory, bool *p_level_completed, int ankhtime, int baseframe);

	void draw();
	std::string fetchDifficultyText();
	void createInventorySfcEp1();
	void createInventorySfcEp2();
	void createInventorySfcEp3();

	bool isClosed() {	return m_closed;	}
	void close() {	m_closing = true;	}

private:

	SDL_Surface* CreateStatusSfc();

	char m_episode;
	int m_baseframe;
	stInventory *mp_inventory;
	bool *mp_level_completed;
	int m_ankhtime;

	bool m_closing, m_closed;

	std::shared_ptr<SDL_Surface> mpStatusSfc;
	SDL_Rect m_StatusRect;
};

#endif /* CSTATUSSCREEN_H_ */
