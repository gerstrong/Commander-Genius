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

CMessageBox::CMessageBox(const std::string& text,
                         const bool lower,
                         const bool keymsg,
                         const bool leftbound,
                         const FXKind fx) :
CGUIDialog(GsRect<float>(0.1f, 0.1f, 0.8f, 0.8f), fx),
m_mustclose(false)
{
    //const char closeChar = 0x1F;
    //std::string closeString;
    //closeString = closeChar;

    GsRect<float> closeButtonRect(0.0f, 0.0f, (0.06f)/0.8f, (0.06f)/0.8f);

    GsRect<float> facRect( gVideoDriver.getGameResFactors() );
    closeButtonRect.transformInverse(facRect);

    mpTextCtrl = new CGUIText( text, GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f) );

	// Those formulas work well with our constellation but I don't think they are perfect.
	// They transform the Message Box the way the text fits perfectly in.
    const float screenW = gVideoDriver.getGameResolution().dim.x;
    const float screenH = gVideoDriver.getGameResolution().dim.y;
    mRect.dim.x = static_cast<float>( (mpTextCtrl->mTextDim.dim.x+4)*8 )/screenW;
    mRect.dim.y = static_cast<float>( (mpTextCtrl->mTextDim.dim.y+2)*8 )/screenH;
    mRect.pos.x = (1.0f - mRect.dim.x)*0.5f;
    mRect.pos.y = (1.0f - mRect.dim.y)*0.5f;

	// now let's center that long text...
    GsRect<float> textRect;
    textRect.dim.x = ((mpTextCtrl->mTextDim.dim.x*8)/screenW) / mRect.dim.x;
    textRect.dim.y = ((mpTextCtrl->mTextDim.dim.y*8)/screenH) / mRect.dim.y;
    textRect.pos.x = (2.0f*textRect.dim.x)/(static_cast<float>(mpTextCtrl->mTextDim.dim.x));
    textRect.pos.y = (1.0f*textRect.dim.y)/(static_cast<float>(mpTextCtrl->mTextDim.dim.y));

	if(lower) // if lower is enabled, try to snap the Dlg to the bottom off the screen.
	{
        mRect.pos.y = 1.0f - mRect.dim.y;
	}

    addControl( mpTextCtrl, textRect );

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
