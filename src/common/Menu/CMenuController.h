/*
 * CMenuController.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include "CBaseMenu.h"
#include "engine/CEvent.h"
#include "CSingleton.h"
#include <memory>

#ifndef CMENUCONTROLLER_H_
#define CMENUCONTROLLER_H_


#define gpMenuController CMenuController::Get()


/**
 * Events
 */


struct OpenMenuEvent : CEvent
{
    OpenMenuEvent(CBaseMenu* menuDlgPtr) : mMenuDialogPointer(menuDlgPtr) {}

	std::shared_ptr<CBaseMenu> mMenuDialogPointer;
};

struct CloseMenuEvent : CEvent
{};

struct CloseAllMenusEvent : CEvent
{};

/**
 * Class Declaration
 */


class CMenuController : public CSingleton<CMenuController>
{

public:
	CMenuController() : mOpenedGamePlay(false),
                          mLocked(false),
                          mHidden(false) {}

	void emptyMenuStack();

	void openMainMenu();

    void ponder();

    void render();

	bool active()
	{	return	!mMenuStack.empty();	}

    void lock(const bool value)
    {	mLocked = value;	}

    void hide(const bool value)
    {	mHidden = value;	}

	bool mOpenedGamePlay;

private:

	void popBackMenu();
	
	std::list< std::shared_ptr<CBaseMenu> > mMenuStack;

	bool mLocked;
    bool mHidden;
};

#endif /* CMENUCONTROLLER_H_ */
