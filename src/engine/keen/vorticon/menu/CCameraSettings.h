/*
 * CCameraSettings.h
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#ifndef CCAMERASETTINGS_H_
#define CCAMERASETTINGS_H_

#include "VorticonMenu.h"
#include "widgets/NumberControl.h"

#include <base/video/CVidConfig.h>

namespace vorticon
{

class CCameraSettings : public VorticonMenu
{
public:
	CCameraSettings();

    void refresh();

	void release();

private:
	st_camera_bounds m_CameraBounds;
	int m_changeint;

    NumberControl	*mpLeftAdjust;
    NumberControl	*mpRightAdjust;
    NumberControl	*mpTopAdjust;
    NumberControl	*mpBottomAdjust;
    NumberControl	*mpSpeedAdjust;
};

}

#endif /* CCAMERASETTINGS_H_ */
