/*
 * CPreviews.cpp
 *
 *  Created on: 10.01.2011
 *      Author: gerstrong
 */

#include "CPreviews.h"

#include "common/CBehaviorEngine.h"
#include "FindFile.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "StringUtils.h"
#include "common/CVorticonMapLoader.h"
#include "sdl/extensions.h"

void CPreviews::init()
{
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
	m_episode = ExeFile.getEpisode();
	std::string DataDirectory = ExeFile.getDataDirectory();
	mpMap.reset(new CMap());

	CVorticonMapLoader Maploader(mpMap);
	Maploader.load(m_episode, 90, DataDirectory);

	mpMap->gotoPos( 0, 0 );

	// draw level map
	mpMap->drawAll();

	m_scene_number = 1;
	openNextScene();
}

int CPreviews::openNextScene()
{
	// Here we set the function pointer for the cycle and load stuff for it...
	m_scene_number++;
	std::string filename = "preview";
	filename += itoa(m_scene_number);
	filename += ".ck";
	filename += itoa(m_episode);

	// If the we have scene to load, load it, else open the text which ends the preview
	if(openScene(filename))
	{
		this->process_ptr = &CPreviews::drawPreviewScene;
	}
	else
	{
		std::string filename = JoinPaths(g_pBehaviorEngine->m_ExeFile.getDataDirectory(), "previews.ck");
		filename += itoa(g_pBehaviorEngine->getEpisode());

		mp_TextViewer.reset( new CTextViewer(0, 8, 320, 160) );

		if(!mp_TextViewer->loadTextfromFile(filename))
			m_destroy_me = true;

		this->process_ptr = &CPreviews::showText;
	}

	return 0;
}

bool CPreviews::openScene(const std::string& filename)
{
	// This will open one the preview scenes
	mp_StaticScene.reset( new CFinaleStaticScene(g_pBehaviorEngine->m_ExeFile.getDataDirectory(), filename) );

	if( mp_StaticScene->mustclose() )
		return false;
	else
		mp_StaticScene->setTimer(1000);

	return true;
}

void CPreviews::drawPreviewScene()
{
	// This will show a scene of the preview scenes
	mp_StaticScene->process();

	if( mp_StaticScene->mustclose() || g_pInput->getPressedAnyCommand() )
		openNextScene();
}

void CPreviews::showText()
{
	// This is called after the preview screens were shown
	mp_TextViewer->process();

	if( mp_TextViewer->hasClosed() || g_pInput->getPressedCommand(IC_BACK) )
	{
		m_destroy_me = true;
	}
}

void CPreviews::process()
{
	// Here only the variable for the scene function should be called. Use a pointer to function for that...
	(*this.*process_ptr)();
}

void CPreviews::teardown()
{
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();
	EventContainer.add(new ResetScrollSurface);
}

