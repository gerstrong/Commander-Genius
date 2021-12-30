/*
 * GsBitmapBox.h
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#ifndef GsBitmapBox_H_
#define GsBitmapBox_H_

#include <memory>
#include <string>


#include "GsWidget.h"
#include "graphics/GsBitmap.h"

#include <base/utils/property_tree/property_tree.h>

class GsBitmapBox : public GsWidget
{
public:


    GsBitmapBox(const GsRect<float> &rect);

    // Create an empty bitmap. TODO: Must become deprecated
    //GsBitmapBox() {}

    // Loads an Bitmap that is given directly
    GsBitmapBox(std::shared_ptr<GsBitmap> &bmpPtr,
                const GsRect<float> &rect);

    // Loads an Bitmap that is given directly
    GsBitmapBox(std::shared_ptr<GsBitmap> &bmpPtr);

    // Sets another Bitmap Pointer
    void setBitmapPtr(std::shared_ptr<GsBitmap> &bmpPtr);

    // Load an Bitmap using an internal string ID of all the loaded Bitmaps
    GsBitmapBox(const std::string &text,
                const GsRect<float> &rect);

    void updateGraphics();

    void processLogic();

    void processRender(const GsRect<float> &RectDispCoordFloat);

private:

    std::shared_ptr<GsBitmap> mpBitmap;
    std::shared_ptr<GsBitmap> mScaledBitmapPtr;
    std::string mTextID;
};

std::shared_ptr<GsBitmapBox> createBitmapBoxFrom(const GsKit::ptree &node);

#endif /* GsBitmapBox_H_ */
