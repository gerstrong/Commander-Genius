/*
 * CGUINumberControl.h
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#ifndef __VORTICONNUMBERCONTROL_H_
#define __VORTICONNUMBERCONTROL_H_

#include <widgets/GsNumberControl.h>

#include <string>
#include <graphics/GsSurface.h>


namespace vorticon
{

class NumberControl: public CGUINumberControl
{
public:

    NumberControl(	const std::string& text,
			const int startValue,
			const int endValue,
			const int deltaValue,
			const int value,
            const bool slider);



	void processRender(const GsRect<float> &RectDispCoordFloat);
};

}

#endif /* __VORTICONNUMBERCONTROL_H_ */
