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


CGUIDialog::CGUIDialog(const CRect<float> &SrcRect)
{
	CRect<float> NewRect = SrcRect;
	CRect<float> ScaleRect;

	ScaleRect = g_pVideoDriver->getGameResolution();

	NewRect.transform(ScaleRect);
	mRect = NewRect;
}


void CGUIDialog::addControl(CGUIControl *newControl, const CRect<float>& ControlRect)
{
	CRect<float> NewRect = ControlRect;
	NewRect.transform(mRect);
	newControl->mRect = NewRect;

	mControlList.push_back( newControl );
}


void CGUIDialog::processLogic()
{
	g_pVideoDriver->mDrawTasks.add( new DrawGUIRenderTask(this) );

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processLogic();
	}
}


void CGUIDialog::processRendering()
{
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_Rect lRect = mRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0x00D6D6D6);

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processRender();
	}

}
