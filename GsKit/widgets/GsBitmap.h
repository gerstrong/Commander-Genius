/*
 * CGUIBitmap.h
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#ifndef CGUIBITMAP_H_
#define CGUIBITMAP_H_

#include <memory>
#include <string>


#include "GsControl.h"
#include "graphics/GsBitmap.h"

class CGUIBitmap : public GsControl
{
public:

    // Create an empty bitmap
    CGUIBitmap() {}

	// Loads an Bitmap that is given directly
    CGUIBitmap(std::shared_ptr<GsBitmap> &pBitmap);

    // Sets another Bitmap Pointer
    void setBitmapPtr(std::shared_ptr<GsBitmap> &pBitmap);

	// Load an Bitmap using an internal string ID of all the loaded Bitmaps
	CGUIBitmap(const std::string &text);

    void updateGraphics();

	void processLogic();

	void processRender(const GsRect<float> &RectDispCoordFloat);


private:

    std::shared_ptr<GsBitmap> mpBitmap;
    GsBitmap mScaledBitmap;
    std::string mTextID;
};

#endif /* CGUIBITMAP_H_ */
