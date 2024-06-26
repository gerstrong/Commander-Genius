/*
 * CCameraSettings.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */
/*
#include <base/video/CVideoDriver.h>
#include "CameraSettings.h"
#include "engine/core/CSettings.h"
#include <base/interface/StringUtils.h>


CCameraSettings::CCameraSettings(const GsControl::Style style):
GameMenu(GsRect<float>(0.1f, 0.24f, 0.8f, 0.4f), style ),
m_CameraBounds(gVideoDriver.getCameraBounds())
{

    mpLeftAdjust = new NumberControl( "Left",
                                      50, 270, 10, 60,
                                      false,
                                      style );
	mpMenuDialog->addControl( mpLeftAdjust );

    mpRightAdjust = new NumberControl( "Right",
                                       50, 270, 10, 60,
                                       false,
                                       style);
	mpMenuDialog->addControl( mpRightAdjust );

    mpTopAdjust = new NumberControl( "Top",
                                     50, 150, 10, 60,
                                     false,
                                     style);
	mpMenuDialog->addControl( mpTopAdjust );

    mpBottomAdjust = new NumberControl( "Bottom",
                                        50, 150, 10, 60,
                                        false,
                                        style);
	mpMenuDialog->addControl( mpBottomAdjust );

    mpSpeedAdjust = new NumberControl( "Speed",
                                       1, 50, 1, 25,
                                       false,
                                       style);
	mpMenuDialog->addControl( mpSpeedAdjust );

	setMenuLabel("OPTIONSMENULABEL");
}


void CCameraSettings::refresh()
{
	// Load the config into the GUI
	mpLeftAdjust->setSelection( m_CameraBounds.left );
	mpRightAdjust->setSelection( m_CameraBounds.right );
	mpTopAdjust->setSelection( m_CameraBounds.up );
	mpBottomAdjust->setSelection( m_CameraBounds.down );
	mpSpeedAdjust->setSelection( m_CameraBounds.speed );
}


void CCameraSettings::release()
{
	// Save the config from the GUI
	m_CameraBounds.left = mpLeftAdjust->getSelection();
	m_CameraBounds.right = mpRightAdjust->getSelection();
	m_CameraBounds.up = mpTopAdjust->getSelection();
	m_CameraBounds.down = mpBottomAdjust->getSelection();
	m_CameraBounds.speed = mpSpeedAdjust->getSelection();

	gVideoDriver.saveCameraBounds(m_CameraBounds);

	gSettings.saveDrvCfg();
}
*/
