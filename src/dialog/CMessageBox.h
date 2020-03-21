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
#include "engine/core/CBehaviorEngine.h"


class CMessageBox : public CGUIDialog
{
public:
	// Init functions
    CMessageBox(const std::string& Text,
                const bool lower,
                const bool keymsg,
                const bool leftbound,
                const FXKind fx = FXKind::NONE);
	virtual ~CMessageBox() {}

    /**
     * @brief processLogic There are dialog boxes in both engines.
     *                     They have are different and must be
     *                     overloaded with this method.
     */
    void processLogic();

    virtual void render();



	/// Getters
	/*
	 * \brief Indicates whether Dialog is ready to be closed
	 * \return true means yes, it is ready
	 */
	bool isFinished();

protected:
	bool m_mustclose;

    std::shared_ptr<CGUIText> mpTextCtrl;
};


#endif /* CMESSAGEBOX_H_ */
