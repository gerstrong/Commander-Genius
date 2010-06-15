/*
 * CMessageBox.h
 *
 *  Created on: 04.12.2009
 *      Author: gerstrong
 *
 *  This is a normal Message box that shows some text and exits
 *  when someone presses a key or button (command input)
 */

#ifndef CMESSAGEBOX_H_
#define CMESSAGEBOX_H_

#include "../dialog/CDlgFrame.h"
#include "../common/CBehaviorEngine.h"
#include <string>
#include <vector>

// Holder for some tile that have to be drawn
struct TileHolder{
	Uint16 tile;
	Uint16 x;
	Uint16 y;
};

class CMessageBox {
public:
	// Init functions
	CMessageBox(const std::string& Text, bool lower = false, bool keymsg = false, bool leftbound = false);
	void addTileAt(Uint16 tile, Uint16 x, Uint16 y);

	// Processing
	void process();

	// Getters
	bool isFinished();

	// destructors
	virtual ~CMessageBox();

private:
	bool m_mustclose;
	bool m_keymsg;
	std::vector<std::string> m_Lines;
	CDlgFrame *mp_DlgFrame;
	SDL_Rect m_gamerect;

	std::vector<TileHolder> m_Tiles;

	Uint8 m_twirltimer;
	Uint8 m_twirlframe;
};

#endif /* CMESSAGEBOX_H_ */
