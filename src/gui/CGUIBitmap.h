/*
 * CGUIBitmap.h
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#ifndef CGUIBITMAP_H_
#define CGUIBITMAP_H_

#include "CGUIControl.h"
#include "graphics/CBitmap.h"
#include <memory>
#include <string>

class CGUIBitmap : public CGUIControl
{
public:
    
	// Loads an Bitmap that is given directly
	CGUIBitmap(std::unique_ptr<CBitmap>&& pBitmap);
    
	// Load an Bitmap using an internal string ID of all the loaded Bitmaps
	CGUIBitmap(const std::string &text);
    
    
	void processLogic();
    
	void processRender(const CRect<float> &RectDispCoordFloat);
    
private:
    
	std::unique_ptr<CBitmap> mpBitmap;
    
};

#endif /* CGUIBITMAP_H_ */
