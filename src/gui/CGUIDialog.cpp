/*
 * CGUIDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  GUI Class for a Dialog which might carry other controls
 */

#include "CGUIDialog.h"
#include "sdl/CVideoDriver.h"


CGUIDialog::CGUIDialog(const CRect &NewRect) :
mRect(NewRect)
{}


void CGUIDialog::processLogic()
{
	g_pVideoDriver->mDrawTasks.add( new DrawGUIRenderTask(this) );

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = ControlList.begin() ;
		 it != ControlList.end() ; it++ )
	{
		(*it)->processLogic();
	}
}


void CGUIDialog::processRendering()
{
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_FillRect(Blitsurface, &mRect, 0x00FFFFFF);

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = ControlList.begin() ;
		 it != ControlList.end() ; it++ )
	{
		(*it)->processRender();
	}

}
