/*
 * CCameraSettings.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include "CCameraSettings.h"
#include "common/CSettings.h"
#include <base/utils/StringUtils.h>

CCameraSettings::CCameraSettings():
CBaseMenu(GsRect<float>(0.1f, 0.24f, 0.8f, 0.4f) ),
m_CameraBounds(gVideoDriver.getCameraBounds())
{

	mpLeftAdjust = new CGUINumberControl( "Left",
											 50, 270, 10, 60 );
	mpMenuDialog->addControl( mpLeftAdjust );

	mpRightAdjust = new CGUINumberControl( "Right",
											 50, 270, 10, 60 );
	mpMenuDialog->addControl( mpRightAdjust );

	mpTopAdjust = new CGUINumberControl( "Top",
											 50, 150, 10, 60 );
	mpMenuDialog->addControl( mpTopAdjust );

	mpBottomAdjust = new CGUINumberControl( "Bottom",
											 50, 150, 10, 60 );
	mpMenuDialog->addControl( mpBottomAdjust );

	mpSpeedAdjust = new CGUINumberControl( "Speed",
											 1, 50, 1, 25 );
	mpMenuDialog->addControl( mpSpeedAdjust );

	setMenuLabel("OPTIONSMENULABEL");
}


void CCameraSettings::init()
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

	g_pSettings->saveDrvCfg();
}
