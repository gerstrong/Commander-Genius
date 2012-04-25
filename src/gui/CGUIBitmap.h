/*
 * CGUIBitmap.h
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#ifndef CGUIBITMAP_H_
#define CGUIBITMAP_H_

#include "CGUIControl.h"
#include "SmartPointer.h"
#include "graphics/CBitmap.h"
#include <string>

class CGUIBitmap : public CGUIControl
{
public:
	CGUIBitmap(const std::string &text);

	void processLogic();

	void processRender(const CRect<float> &RectDispCoordFloat);

private:

	SmartPointer<CBitmap> mpBitmap;

};

#endif /* CGUIBITMAP_H_ */
