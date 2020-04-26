/*
 * GsButton.h
 *
 *  Created on: 26.04.2020
 *      Author: gerstrong
 *
 *  Widget for simple buttons
 */

#ifndef GsBitmapButton_H_
#define GsBitmapButton_H_

#include "GsButton.h"

class GsBitmapButton : public GsButton
{
public:

    GsBitmapButton(const std::string& text,
                   const GsRect<float> &rect,
                   CEvent *ev = nullptr,
                   const int fontId = -1,
                   const GsColor &color = GsColor(0xEE, 0xEE, 0xFF));

    bool loadBgBitmap(const std::string &filepath);

    void drawNoStyle(const SDL_Rect& lRect) override;

private:
    std::shared_ptr<GsBitmap> mpBitmap; // optional bitmap for the button

};

#endif // GsBitmapButton_H_
