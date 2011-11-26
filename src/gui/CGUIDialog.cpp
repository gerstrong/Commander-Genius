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


CGUIDialog::CGUIDialog(const CRect<float> &SrcRect) :
mRect(SrcRect)
{}


void CGUIDialog::addControl(CGUIControl *newControl, const CRect<float>& ControlRect)
{
	newControl->mRect = ControlRect;
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
	CRect<Uint16> GameRes = g_pVideoDriver->getGameResolution();
	CRect<float> screenRect(0, 0, GameRes.w, GameRes.h);
	CRect<float> RectDispCoordFloat = mRect;

	// Transform to the blit coordinates
	RectDispCoordFloat.transform(screenRect);

	CRect<Uint16> RectDispCoord;
	RectDispCoord = RectDispCoordFloat;
	SDL_Rect lRect = RectDispCoord.SDLRect();

	SDL_FillRect(Blitsurface, &lRect, 0x00D6D6D6);

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processRender(RectDispCoordFloat);
	}

}
