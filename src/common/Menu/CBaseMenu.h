/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

const int NO_SELECTION = -1;

#include "dialog/CDialog.h"
#include "SmartPointer.h"
#include "gui/CGUIDialog.h"
#include "gui/CGUIButton.h"
#include <list>

// Active means, when the player is playing, PASSIVE when the Player is not playing
enum menumodes
{
	ACTIVE, PASSIVE
}; // TODO: Remove this!

class CBaseMenu
{
public:

	enum Property
	{
		CLOSEABLE,
		CANGOBACK
	};


	CBaseMenu( const Uint8 dlgTheme, const CRect<float>& rect );

	virtual ~CBaseMenu() { };

	virtual void init() {};

	virtual void release() {};

	// Processes the stuff that the menus have in common
	virtual void process();

	virtual void sendEvent(SmartPointer<CEvent> command)
	{
		mpMenuDialog->sendEvent(command);
	}

	void setProperty( const Property newProperty )
	{
		mpReturnButton->setText( newProperty == CLOSEABLE ? "x" : "\025" );
	}


protected:
	SmartPointer<CGUIDialog> mpMenuDialog;
	CGUIButton *mpReturnButton;
	std::list< SmartPointer<CEvent> > mEventList;

};

#endif /* CBASEMENU_H_ */
