/*
 * CMessageBoxGalaxy.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXGALAXY_H_
#define CMESSAGEBOXGALAXY_H_

//#include <base/utils/CVec.h>
#include <string>
#include <SDL.h>
#include <memory>

#include <base/GsEvent.h>
#include <graphics/GsSurface.h>

namespace galaxy
{


class CMessageBoxGalaxy
{
public:

	/**
     * \brief This constructor creates a typical Keen Galaxy Message Box
	 * \param Text 				Message to be shown
	 */
    CMessageBoxGalaxy(const std::string& Text, CEvent *closeEv = nullptr);

	virtual void init();

    virtual void ponder();
    virtual void render();

    SDL_Surface *getSfc()
    {
        return mMBSurface.getSDLSurface();
    }
	
	SDL_Rect getRect() const
	{ return mMBRect; }

	const bool isFinished() const
	{	return mMustClose;	}


    void setCloseEvent(std::unique_ptr<CEvent> &ev)
    {  mCloseEv = std::move(ev); }

protected:

	void initGalaxyFrame();
	virtual void initText(const SDL_Rect &rect);


	bool mMustClose;
	SDL_Rect mMBRect;
	std::string mText;
    GsSurface mMBSurface;
	unsigned int mTextHeight;
    std::unique_ptr<CEvent> mCloseEv;
};


void showMsg(const std::string &text, CEvent *closeEv = nullptr);

struct EventSendDialog : CEvent
{
    std::shared_ptr<CMessageBoxGalaxy> mMsgBox;

    EventSendDialog(std::shared_ptr<CMessageBoxGalaxy>& msgBox) :
        mMsgBox( msgBox ) {}

    EventSendDialog(CMessageBoxGalaxy *msgBox) :
        mMsgBox( msgBox ) {}
};

void showMsgVec( std::vector<CMessageBoxGalaxy*> &msgs );

}

#endif /* CMESSAGEBOXGALAXY_H_ */
