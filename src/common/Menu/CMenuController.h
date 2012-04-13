/*
 * CMenuController.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include "CBaseMenu.h"
#include "engine/CEvent.h"

#ifndef CMENUCONTROLLER_H_
#define CMENUCONTROLLER_H_


/**
 * Events
 */


struct OpenMenuEvent : CEvent
{
	OpenMenuEvent(CBaseMenu* menuDlgPtr) : mMenuDialogPointer(menuDlgPtr) {};

	SmartPointer<CBaseMenu> mMenuDialogPointer;
};

struct CloseMenuEvent : CEvent
{};

struct CloseAllMenusEvent : CEvent
{};

/**
 * Class Declaration
 */


class CMenuController
{

public:
	CMenuController() : mOpenedGamePlay(false) {}

	void process();

	void setPlaying(const bool openedGamePlay)
	{	mOpenedGamePlay = openedGamePlay;	}

private:

	SmartPointer<CBaseMenu> mpMenu;
	std::list< SmartPointer<CBaseMenu> > mMenuStack;

	bool mOpenedGamePlay;
};

#endif /* CMENUCONTROLLER_H_ */
