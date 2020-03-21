/*
 * CMessageBoxGalaxy.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXGALAXY_H_
#define CMESSAGEBOXGALAXY_H_

#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

#include <base/GsEvent.h>
#include <graphics/GsSurface.h>

namespace galaxy
{


class CMessageBoxGalaxy
{
public:

    virtual ~CMessageBoxGalaxy();

	/**
     * \brief This constructor creates a typical Keen Galaxy Message Box
	 * \param Text 				Message to be shown
	 */
    CMessageBoxGalaxy(const int sprVar,
                      const std::string& Text,
                      CEvent *closeEv = nullptr,
                      const bool isModal = true,
                      const bool alignTop = false,
                      const int timeout = 0);

	virtual void init();

    virtual void ponder(const int deltaT);
    virtual void render();

    SDL_Surface *getSfc()
    {
        return mMBSurface.getSDLSurface();
    }
	
	SDL_Rect getRect() const
	{ return mMBRect; }

    bool isFinished() const
	{	return mMustClose;	}


    void setCloseEvent(std::unique_ptr<CEvent> &ev)
    {  mCloseEv = std::move(ev); }

    bool isModal() const
    {   return mIsModal;    }

protected:

	void initGalaxyFrame();
	virtual void initText(const SDL_Rect &rect);


	bool mMustClose;
	SDL_Rect mMBRect;
	std::string mText;
    GsSurface mMBSurface;
	unsigned int mTextHeight;
    std::unique_ptr<CEvent> mCloseEv;
    int mSprVar = 0;

    bool mIsModal = true;
    int mTimeout = 0; // Timeout in msecs for the non-modal dialog to close itself
};


void showMsg(const int sprVar, const std::string &text,
             CEvent *closeEv = nullptr,
             const bool isModal = true,
             const bool alignTop = false,
             const int timeout = 0);




struct EventSendDialog : CEvent
{
    ~EventSendDialog();

    std::shared_ptr<CMessageBoxGalaxy> mMsgBox;

    EventSendDialog(std::shared_ptr<CMessageBoxGalaxy>& msgBox) :
        mMsgBox( msgBox ) {}

    EventSendDialog(CMessageBoxGalaxy *msgBox) :
        mMsgBox( msgBox ) {}

};

void showMsgVec( std::vector<CMessageBoxGalaxy*> &msgs );

}

#endif /* CMESSAGEBOXGALAXY_H_ */
