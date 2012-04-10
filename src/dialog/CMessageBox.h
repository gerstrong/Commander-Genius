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

#include "dialog/CDlgFrame.h"
#include "common/CBehaviorEngine.h"
#include "SmartPointer.h"
#include "gui/CGUIDialog.h"
#include "gui/CGUIText.h"
#include <string>
#include <vector>


// Holder for some tile that have to be drawn
struct TileHolder
{
	Uint16 tile;
	Uint16 x;
	Uint16 y;
};

class CMessageBox : public CGUIDialog
{
public:
	// Init functions
	CMessageBox(const std::string& Text, bool lower, bool keymsg, bool leftbound);

	/** Processing
	 * There are dialog boxes in both engines. They have are different and must be overloaded with this function.
	 */
	void processLogic();


	/// Getters
	/*
	 * \brief Indicates whether Dialog is ready to be closed
	 * \return true means yes, it is ready
	 */
	bool isFinished();

protected:
	bool m_mustclose;
	/*bool m_keymsg;
	Uint32 m_text_width;
	Uint32 m_text_height;
	std::vector<std::string> m_Lines;
	SmartPointer<CDlgFrame> mp_DlgFrame;
	SDL_Rect m_boxrect;*/

	CGUIText *mpTextCtrl;
};

#endif /* CMESSAGEBOX_H_ */
