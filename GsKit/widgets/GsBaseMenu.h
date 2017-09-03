/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

#include <list>
#include <memory>


const int NO_SELECTION = -1;

#include "GsDialog.h"
#include "GsButton.h"

class CBaseMenu
{
public:

	enum Property
	{
		CLOSEABLE,
		CANGOBACK
	};


    CBaseMenu( const GsRect<float>& rect,
               GsButton *returnButton,
               const CGUIDialog::FXState fx = CGUIDialog::FXState(0));

    virtual ~CBaseMenu() { }

    /**
     * @brief refresh This is called, when the MenuController has several loaded menus and suddenly
     *        the user switches from one menu to another or load one for the first time.
     *        if your menu is pretty dynamic, please overload that method so it is guaranteed that
     *        the elements to be refreshed on that called are up to date.
     */
    virtual void refresh() {}

    void select(const size_t value);

    virtual void release() {}

	void setMenuLabel(const std::string &label);

	// Processes the stuff that the menus have in common
    virtual void ponder(const float deltaT);

    virtual void render();

	virtual void sendEvent(std::shared_ptr<CEvent> &command)
	{
		mpMenuDialog->sendEvent(command);
	}

	void setProperty( const Property newProperty )
	{
		mpReturnButton->setText( newProperty == CLOSEABLE ? "x" : "\025" );
	}

    void updateGraphics()
    {
        refresh();
        mpMenuDialog->updateGraphics();
    }

protected:
	std::unique_ptr<CGUIDialog> mpMenuDialog;
	GsButton *mpReturnButton;
	std::list< std::unique_ptr<CEvent> > mEventList;
};

#endif /* CBASEMENU_H_ */
