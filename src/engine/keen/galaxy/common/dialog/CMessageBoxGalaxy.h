/*
 * CMessageBoxGalaxy.h
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXGALAXY_H_
#define CMESSAGEBOXGALAXY_H_

#include "CVec.h"
#include <string>
#include <SDL.h>
#include <memory>

#include <base/GsEvent.h>

namespace galaxy
{

class CMessageBoxGalaxy
{
public:

	/**
	 * \brief This constructor creates a typical Keen 4 Message Box
	 * \param Text 				Message to be shown
	 */
    CMessageBoxGalaxy(const std::string& Text);

	virtual void init();

    virtual void ponder();
    virtual void render();

	SDL_Surface *getSfc()
	{ return mpMBSurface.get(); }
	
	SDL_Rect getRect() const
	{ return mMBRect; }

	const bool isFinished() const
	{	return mMustClose;	}


protected:

	void initGalaxyFrame();
	virtual void initText(const SDL_Rect &rect);


	bool mMustClose;
	SDL_Rect mMBRect;
	std::string mText;
	std::shared_ptr<SDL_Surface> mpMBSurface;
	unsigned int mTextHeight;

};


// This class is the same CMessageBoxGalaxy with the only difference,
// that if the MessageBox is closed it will push an event to the Event Manager

class CMessageBoxGalaxyWithEvent : public CMessageBoxGalaxy
{
public:

    CMessageBoxGalaxyWithEvent(const std::string& text) :
        CMessageBoxGalaxy(text) {}

    CMessageBoxGalaxyWithEvent(const std::string& text,
                      std::shared_ptr<CEvent> &event) :
        CMessageBoxGalaxy(text),
        mpTriggerEvent(event) {}

    virtual void ponder();

private:

    std::shared_ptr<CEvent> mpTriggerEvent;

};

void showMsg( const std::string &text );

}

#endif /* CMESSAGEBOXGALAXY_H_ */
