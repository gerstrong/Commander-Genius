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

#include <widgets/GsDialog.h>
#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <string>
#include <vector>

#include "dialog/CDlgFrame.h"
#include "common/CBehaviorEngine.h"


class CMessageBox : public CGUIDialog
{
public:
	// Init functions
    CMessageBox(const std::string& Text, bool lower, bool keymsg, bool leftbound, const FXState fx = NONE);
	virtual ~CMessageBox() {}

	/** Processing
	 * There are dialog boxes in both engines. They have are different and must be overloaded with this function.
	 */
    void processLogic();

    void render();



	/// Getters
	/*
	 * \brief Indicates whether Dialog is ready to be closed
	 * \return true means yes, it is ready
	 */
	bool isFinished();

protected:
	bool m_mustclose;
	GsButton *mpReturnButton;

	CGUIText *mpTextCtrl;
};

#endif /* CMESSAGEBOX_H_ */
