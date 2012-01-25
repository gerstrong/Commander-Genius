/*
 * CGUIDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *
 *  GUI Class for a Dialog which is able to carry other controls
 */

#include "CGUIDialog.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"

CGUIDialog::CGUIDialog(const CRect<float> &SrcRect) :
mRect(SrcRect)
{}


void CGUIDialog::addControl(CGUIControl *newControl, const CRect<float>& RelRect)
{
	CRect<float> AbsRect = RelRect;
	AbsRect.transform(mRect);
	newControl->mRect = AbsRect;
	mControlList.push_back( newControl );
}


void CGUIDialog::processLogic()
{
	// Render the stuff
	g_pVideoDriver->mDrawTasks.add( new DrawGUIRenderTask(this) );

	// Prepare the subcontrols for rendering
	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processLogic();
	}

	g_pInput->m_EventList.clear();
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

	SDL_FillRect(Blitsurface, &lRect, 0x00E6E6E6);

	for( std::list<
		 SmartPointer<CGUIControl> >::iterator it = mControlList.begin() ;
		 it != mControlList.end() ; it++ )
	{
		(*it)->processRender(screenRect);
	}

}
