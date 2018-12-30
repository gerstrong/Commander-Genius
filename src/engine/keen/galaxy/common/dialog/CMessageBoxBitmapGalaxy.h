/*
 * CMessageBoxBitmapGalaxy.h
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXBITMAPGALAXY_H_
#define CMESSAGEBOXBITMAPGALAXY_H_

#include "CMessageBoxGalaxy.h"
#include "graphics/GsGraphics.h"
#include "engine/core/CSpriteObject.h"

namespace galaxy
{

class CMessageBoxBitmapGalaxy : public CMessageBoxGalaxy
{
public:

	/**
	 * \brief This constructor creates a typical Keen 4 Message Box
	 * \param Text 				Message to be shown
	 * \param BitmapId 			Bitmap that will be shown to the left side in the box
	 */
    CMessageBoxBitmapGalaxy(int sprVar, const std::string& Text,
                            const GsBitmap &BitmapRef,
                            const direction_t alignment,
                            const bool animation,
                            CEvent *closeEv);

    virtual ~CMessageBoxBitmapGalaxy() {}

    virtual void init() override;

    void ponder() override;
    void render() override;


private:
    const GsBitmap &mBitmap;
	const direction_t mAlignment;        

    /**
     * @brief mMBAnimatedSurface Some boxes have animation,
     *                           like Billys hand forming a fist
     */
    GsSurface mMBAnimatedSurface;
    int mAnimtimer = 0;
    int mAnimFrame = 1;

    int mShowtime = -1;
};

void showMsgWithBmp(const int sprVar, const std::string &text,
                    const std::string &bmp,
                    const direction_t alignment,
                    const bool animation,
                    CEvent *closeEv);


void showMsgWithBmp(const int sprVar, const std::string &text,
                    const unsigned int bmpID,
                    const direction_t alignment ,
                    const bool animation,
                    CEvent *closeEv);


}

#endif /* CMESSAGEBOXBITMAPGALAXY_H_ */
