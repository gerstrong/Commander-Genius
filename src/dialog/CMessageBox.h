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

class CMessageBox : public CGUIDialog
{
public:
    /**
     * @brief CMessageBox
     * @param Text
     * @param lower if lower is enabled, try to snap the Dlg to the bottom off the screen.
     * @param keymsg
     * @param vorticonBorders
     * @param fx
     */
    CMessageBox(const std::string& Text,
                const bool lower,
                const bool keymsg,
                const bool vorticonBorders,
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

    std::shared_ptr<GsText> mpTextCtrl;
};


#endif /* CMESSAGEBOX_H_ */
