/*
 * CMessageBoxBitmapGalaxy.h
 *
 *  Created on: 04.05.2011
 *      Author: gerstrong
 */

#ifndef CMESSAGEBOXBITMAPGALAXY_H_
#define CMESSAGEBOXBITMAPGALAXY_H_

#include "CMessageBoxGalaxy.h"
#include "graphics/CGfxEngine.h"
#include "common/CSpriteObject.h"

class CMessageBoxBitmapGalaxy : public CMessageBoxGalaxy
{
public:

	/**
	 * \brief This constructor creates a typical Keen 4 Message Box
	 * \param Text 				Message to be shown
	 * \param BitmapId 			Bitmap that will be shown to the left side in the box
	 */
	CMessageBoxBitmapGalaxy( const std::string& Text, const CBitmap &BitmapRef,
							const direction_t alignment = LEFT );
	virtual ~CMessageBoxBitmapGalaxy() {};

	virtual void init();

private:
	const CBitmap &mBitmap;
	const direction_t mAlignment;
};

#endif /* CMESSAGEBOXBITMAPGALAXY_H_ */
