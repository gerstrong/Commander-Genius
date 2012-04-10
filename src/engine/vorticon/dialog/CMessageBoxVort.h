/*
 * CMessageBoxVort.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXVORT_H_
#define CMESSAGEBOXVORT_H_

#include "dialog/CMessageBox.h"
#include "SmartPointer.h"

class CMessageBoxVort : public CMessageBox
{
public:
	CMessageBoxVort(const std::string& Text, bool lower = false, bool keymsg = false, bool leftbound = false);

	/**
	 * \brief This will add an extra tile to the message box. Mostly used by the dialog "Ship missing" in Keen 1
	 */
	void addTileAt(Uint16 tile, Uint16 x, Uint16 y);

private:
	std::vector<TileHolder> m_Tiles;
};

#endif /* CMESSAGEBOXVORT_H_ */
