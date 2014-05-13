/*
 * CMessageBox.cpp
 *
 *  Created on: 04.12.2009
 *      Author: gerstrong
 */

#include "CMessageBox.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include "graphics/GsGraphics.h"
#include <base/utils/StringUtils.h>

// This is a local functor that is invoked when the dialog has be closed
struct CloseDialog : public InvokeFunctorEvent
{
	CloseDialog(bool &mustClose) : mMustClose(mustClose) {}

    void operator()() const
	{		mMustClose = true;	}

	bool &mMustClose;
};

CMessageBox::CMessageBox(const std::string& Text, bool lower, bool keymsg, bool leftbound, const FXState fx) :
CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.8f), fx),
m_mustclose(false)
{
	const char closeChar = 0x1F;
	std::string closeString;
	closeString = closeChar;

    GsRect<float> closeButtonRect(0.0f, 0.0f, (0.06f)/0.8f, (0.06f)/0.8f);

    GsRect<float> facRect( gVideoDriver.getGameResFactors() );
    closeButtonRect.transformInverse(facRect);

	mpTextCtrl = new CGUIText( Text );

	// Those formulas work well with our constellation but I don't think they are perfect.
	// They transform the Message Box the way the text fits perfectly in.
	const float screenW = gVideoDriver.getGameResolution().w;
	const float screenH = gVideoDriver.getGameResolution().h;
	mRect.w = static_cast<float>( (mpTextCtrl->mTextDim.w+4)*8 )/screenW;
	mRect.h = static_cast<float>( (mpTextCtrl->mTextDim.h+2)*8 )/screenH;
	mRect.x = (1.0f - mRect.w)/2.0f;
	mRect.y = (1.0f - mRect.h)/2.0f;

	// now let's center that long text...
	GsRect<float> TextRect;
	TextRect.w = ((mpTextCtrl->mTextDim.w*8)/screenW) / mRect.w;
	TextRect.h = ((mpTextCtrl->mTextDim.h*8)/screenH) / mRect.h;
	TextRect.x = (2.5f*TextRect.w)/(static_cast<float>(mpTextCtrl->mTextDim.w));
	TextRect.y = (1.0f*TextRect.h)/(static_cast<float>(mpTextCtrl->mTextDim.h));

	if(lower) // if lower is enabled, try to snap the Dlg to the bottom off the screen.
	{
		mRect.y = 1.0f - mRect.h;
	}

	addControl( mpTextCtrl, TextRect );

    initEmptyBackground();
}

void CMessageBox::processLogic()
{
    if( gInput.getPressedCommand(IC_STATUS) || gInput.mouseClicked() )
    {
		m_mustclose = true;
    }

	CGUIDialog::processLogic();
    render();
}

void CMessageBox::render()
{
    CGUIDialog::processRendering();    
}



bool CMessageBox::isFinished()
{ return m_mustclose; }
