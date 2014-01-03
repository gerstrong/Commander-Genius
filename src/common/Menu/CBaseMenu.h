/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

const int NO_SELECTION = -1;

#include "gui/CGUIDialog.h"
#include "gui/CGUIButton.h"
#include <list>
#include <memory>

class CBaseMenu
{
public:

	enum Property
	{
		CLOSEABLE,
		CANGOBACK
	};


	CBaseMenu( const GsRect<float>& rect );

    virtual ~CBaseMenu() { }

    virtual void init() {}

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
        mpMenuDialog->updateGraphics();
    }


protected:
	std::unique_ptr<CGUIDialog> mpMenuDialog;
	CGUIButton *mpReturnButton;
	std::list< std::unique_ptr<CEvent> > mEventList;
};

#endif /* CBASEMENU_H_ */
