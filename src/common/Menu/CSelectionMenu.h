/*
 * CSelectionMenu.h
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 *
 *  Based on the work of Pizza2004
 *  This is template for normal selection menus. It can tell what item the user selected
 */

#ifndef CSELECTIONMENU_H_
#define CSELECTIONMENU_H_

#include <string>
#include <list>
#include "CBaseMenu.h"
#include "common/Menu/CMenuController.h"
#include "common/CBehaviorEngine.h"


/*class CSelectionMenu : public CBaseMenu
{
public:
	CSelectionMenu(const Uint8 dlg_theme) :
		CBaseMenu(dlg_theme, CRect<float>(0.10f, 0.10f , 0.10f, 0.10f ) ),
		mSelected(output_selection)
	{

		mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	}

	int fetchSelOptionButtonID()
	{

		std::list<CGUIButton*>::iterator button = mpButtonList.begin();
		for( int i=0 ; button != mpButtonList.end() ; button++, i++ )
		{
			if( (*button)->Up() )
				return i;
		}

		return -1;

	}


	virtual void init() = 0;

	virtual void release() = 0;

private:
	int mSelected;
	std::list<CGUIButton*> mpButtonList;
};*/




#endif /* CSELECTIONMENU_H_ */
